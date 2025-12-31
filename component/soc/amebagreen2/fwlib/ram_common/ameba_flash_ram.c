/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "os_wrapper.h"
#include "os_wrapper_specific.h"

static const char *const TAG = "FLASH";
uint32_t PrevIrqStatus;

#define WRITE_SYNC_CLEAR   0
#define WRITE_SYNC_LOCK    1
#define WRITE_SYNC_UNLOCK  2

#ifdef CONFIG_ARM_CORE_CM4_KM4NS
static u32 Start_Timer_Cnt = 0;
static u32 Start_Systick_Cnt = 0;

u32 xTaskIncrementTick(void);
u32 xTaskGetTickCountFromISR(void);

void FLASH_Write_IPC_Int(void *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	(void) Data;
	(void) IrqStatus;
	(void) ChanNum;

	__disable_irq();

	PIPC_MSG_STRUCT ipc_msg = (PIPC_MSG_STRUCT)ipc_get_message(IPC_AP_TO_NP, IPC_A2N_FLASHPG_REQ);
	u8 *pflag = (u8 *)ipc_msg->msg;
	DCache_Invalidate((u32)pflag, sizeof(pflag));

	if (*pflag == WRITE_SYNC_LOCK) {
		/* Km4 in FLASH_Write_Lock, waiting for Km0 to record timer value */
		Start_Timer_Cnt = SYSTIMER_TickGet();
		Start_Systick_Cnt = xTaskGetTickCountFromISR();
		*pflag = WRITE_SYNC_CLEAR;
	} else if (*pflag == WRITE_SYNC_UNLOCK) {
		/* Km4 in FLASH_Write_Unlock, waiting for Km0 to compensate systick value */
		u32 time_pass_ms = SYSTIMER_GetPassTime(Start_Timer_Cnt);
		u32 End_Systick_Cnt = xTaskGetTickCountFromISR();
		u32 systick_pass_tick = 0;
		u8 xSwitchRequired = FALSE;

		if (End_Systick_Cnt >= Start_Systick_Cnt) {
			systick_pass_tick = End_Systick_Cnt - Start_Systick_Cnt;
		} else {
			systick_pass_tick = 0xffffffff - (Start_Systick_Cnt - End_Systick_Cnt);
		}

		u32 step_tick = (time_pass_ms > (systick_pass_tick + 1)) ? (time_pass_ms - (systick_pass_tick + 1)) : 0;
		/*  update kernel tick */
		while (step_tick > 0) {
			/* Increment the RTOS tick. */
			if (xTaskIncrementTick() != FALSE) {
				xSwitchRequired = TRUE;
			}
			step_tick --;
		}

		*pflag = WRITE_SYNC_CLEAR;
		/* Pend a context switch. */
		portEND_SWITCHING_ISR(xSwitchRequired);
	}
	DCache_Clean((u32)pflag, sizeof(pflag));

	__enable_irq();
}

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_flashpg_table[] = {
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = FLASH_Write_IPC_Int,
		.RxIrqData = (void *) NULL,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_AP_TO_NP,
		.IPC_Channel = IPC_A2N_FLASHPG_REQ
	}
};
#else
/* CONFIG_ARM_CORE_CM4_KM4TZ */
ALIGNMTO(CACHE_LINE_SIZE) static u8 Flash_Sync_Flag[CACHE_LINE_SIZE];

static void Flash_Write_Lock_IPC(u8 sync_type)
{
	IPC_MSG_STRUCT ipc_msg_temp;
	/* Set lock flag */
	Flash_Sync_Flag[0] = sync_type;
	DCache_Clean((u32)Flash_Sync_Flag, sizeof(Flash_Sync_Flag));

	ipc_msg_temp.msg_type = IPC_USER_POINT;
	ipc_msg_temp.msg = (u32)Flash_Sync_Flag;
	ipc_msg_temp.msg_len = 1;
	ipc_msg_temp.rsvd = 0;
	ipc_send_message(IPC_AP_TO_NP, IPC_A2N_FLASHPG_REQ, &ipc_msg_temp);

	while (1) {
		DCache_Invalidate((u32)Flash_Sync_Flag, sizeof(Flash_Sync_Flag));
		if (Flash_Sync_Flag[0] == WRITE_SYNC_CLEAR) {
			break;
		}
	}
}
#endif

/**
 * @brief  This function is used to lock CPU when write or erase flash under XIP.
 * @note  All interrupt include systick will be stopped.
 * @retval None
 */
void FLASH_Write_Lock(void)
{
	rtos_sched_suspend();
	/* Get core-to-core hardware semphone */
	while (IPC_SEMTake(IPC_SEM_FLASH, 1000) != TRUE) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "FLASH_Write_Lock get hw sema fail\n");
	}
#ifdef CONFIG_ARM_CORE_CM4_KM4TZ
	/* Sent IPC to KM4NS */
	Flash_Write_Lock_IPC(WRITE_SYNC_LOCK);
#endif
	/* disable irq */
	PrevIrqStatus = irq_disable_save();
}

/**
 * @brief  This function is used to unlock CPU after write or erase flash under XIP.
 * @note All interrupt will be restored.
 * @retval None
 */
void FLASH_Write_Unlock(void)
{
#ifdef CONFIG_ARM_CORE_CM4_KM4TZ
	/* Sent IPC to KM4NS */
	Flash_Write_Lock_IPC(WRITE_SYNC_UNLOCK);
#endif
	/* restore irq */
	irq_enable_restore(PrevIrqStatus);
	/* Free core-to-core hardware semphone */
	IPC_SEMFree(IPC_SEM_FLASH);

	rtos_sched_resume();
}

/**
 * @brief  This function is used to send Rx command to flash to get status register or flash id, and lock CPU when Rx
 * @param  cmd: command that need to be sent.
 * @param  read_len: the number of bytes that will be read by SPIC after sending cmd.
 * @param  read_data: pointer to a byte array which is used to save received data.
 * @note This function is only used for rx status/flash id ,not used for read flash data.
 *		Only work in OneBitMode.
 * @retval None
 */
void FLASH_RxCmdXIP(u8 cmd, u32 read_len, u8 *read_data)
{
	FLASH_Write_Lock();

	FLASH_RxCmd(cmd, read_len, read_data);

	FLASH_Write_Unlock();
}

/**
 * @brief  FLASH_SetStatus used to set register status. FLASH_WriteEn & FLASH_WaitBusy, and lock CPU when set
 *  		are included in this function to avoid hardfault when TxCmd in XIP
 * @param    Cmd: command to be sent
 * @param    Len: the number of bytes to be sent after Cmd
 * @param    Status: pointer to byte array to be sent
 * @retval     None
 */
void FLASH_SetStatusXIP(u8 Cmd, u32 Len, u8 *Status)
{
	FLASH_Write_Lock();

	FLASH_SetStatus(Cmd, Len, Status);

	FLASH_Write_Unlock();
}

/**
 * @brief  FLASH_SetStatusBits set or clear status bits., used to set protect bit or quad enable bit, and lock CPU when set
 * @param    SetBits: 16bits valid, SetBits[7:0] is status1 & SetBits[15:8] is status2
 * @param    NewState: ENABLE/DISABLE
 * @retval None
 */
void FLASH_SetStatusBitsXIP(u32 SetBits, u32 NewState)
{
	FLASH_Write_Lock();

	FLASH_SetStatusBits(SetBits, NewState);

	FLASH_Write_Unlock();
}

/**
 * @brief  This function is used to write data to flash in OneBitMode and User Mode, and lock CPU when write.
 * @param  StartAddr: Start address in flash from which SPIC writes.
 * @param  DataPhaseLen: the number of bytes that SPIC sends in Data Phase.
 * @param  pData: pointer to a byte array that is to be sent.
 * @note
 *		- Page program (256B) time typical is 0.7ms: BaudRate=2.9Mbps, so one bit mode is enough.
 *		- Page program (12B) time typical is 20+2.5*11= 47.5us BaudRate = 2.02M bps, so program 12B once is enough.
 *		- For compatibility with amebaz, which has 16-byte TX FIFO is 16 byte and max len is 16-cmdlen = 12 byte
 * @retval None
 */
void FLASH_TxDataXIP(u32 StartAddr, u32 DataPhaseLen, u8 *pData)
{
	FLASH_Write_Lock();

	FLASH_TxData(StartAddr, DataPhaseLen, pData);
	DCache_Invalidate(SPI_FLASH_BASE + StartAddr, DataPhaseLen);

	FLASH_Write_Unlock();
}

/**
  * @brief  This function is used to erase flash, and lock CPU when erase.
  * @param EraseType: can be one of the following  parameters:
		@arg EraseChip: Erase the whole chip.
		@arg EraseBlock: Erase specified block(64KB)
		@arg EraseSector: Erase specified sector(4KB)
  * @param    Address should 4 byte align.The block/sector which
  * 		the address in will be erased.
  * @retval None
  */
void FLASH_EraseXIP(u32 EraseType, u32 Address)
{
	FLASH_Write_Lock();

	FLASH_Erase(EraseType, Address);
	if (EraseType == EraseSector) {
		DCache_Invalidate(SPI_FLASH_BASE + Address, 0x1000);
	} else if (EraseType == EraseBlock) {
		DCache_Invalidate(SPI_FLASH_BASE + Address, 0x10000);
	} else {
		DCache_Invalidate(SPI_FLASH_BASE, 0x0FFFFFFF - SPI_FLASH_BASE);
	}

	FLASH_Write_Unlock();
}

/**
 * @brief  This function is used to erase some dwords, and keep other dwords unchanged in one sector.
 * @param  address: Start address in flash to be erased.
 * @param  dword_num: the number of dwords to be erased.
 * @note
 *		- This function is just used for change some dwords in one sector.
 *		- This function will erase whole sector and then write back other dwords.
 *		- Do not use this function if not needed !!!!!!!!!!!!!!
 *		- FLASH_EraseXIP is recommended if need
 * @retval None
 */
void FLASH_EraseDwordsXIP(u32 address, u32 dword_num)
{
	UNUSED(dword_num);
	UNUSED(address);
#if 0
	u8 data[256];
	u32 idx = 0;
	u32 opt_sector = address & ~(0xFFF);
	u32 erase_addr = address;
	u32 erase_num = dword_num;

	/* erase backup sector */
	FLASH_EraseXIP(EraseSector, FLASH_RESERVED_DATA_BASE);

	/* backup this sector */
	for (idx = 0; idx < 0x1000; idx += 4) {
		u32 read_addr = opt_sector + idx;

		if ((erase_num > 0) && (erase_addr == read_addr)) {
			erase_addr += 4;
			erase_num--;
		} else {
			_memcpy(data, (const void *)(SPI_FLASH_BASE + read_addr), 4);
			FLASH_TxDataXIP((FLASH_RESERVED_DATA_BASE + idx), 4, (u8 *)data);
		}
	}

	/* erase this sector */
	FLASH_EraseXIP(EraseSector, opt_sector);

	/* write this sector with target data erased */
	for (idx = 0; idx < 0x1000; idx += 256) {
		_memcpy(data, (const void *)(SPI_FLASH_BASE + FLASH_RESERVED_DATA_BASE + idx), 256);
		FLASH_WriteStream((opt_sector + idx), 256, (u8 *)data);
	}
#endif
}

/**
 * @brief  Read a stream of data from specified address
 * @param obj: Flash object define in application software.
 * @param  address: Specifies the starting address to read from.
 * @param  len: Specifies the length of the data to read.
 * @param  data: Specified the address to save the readback data.
 * @retval   status: Success:1 or Failure: Others.
 * @note Auto mode is ok, because we have flash cache
 */
int FLASH_ReadStream(u32 address, u32 len, u8 *pbuf)
{
	assert_param(pbuf != NULL);

	_memcpy(pbuf, (const void *)(SPI_FLASH_BASE + address), len);

	return 1;
}

/**
 * @brief  Write a stream of data to specified address
 * @param  address: Specifies the starting address to write to.
 * @param  len: Specifies the length of the data to write.
 * @param  data: Pointer to a byte array that is to be written.
 * @retval   status: Success:1 or Failure: Others.
 */
int FLASH_WriteStream(u32 address, u32 len, u8 *pbuf)
{
	/* Check address: 4byte aligned & page(256bytes) aligned */
	u32 page_begin = address & (~0xff);
	u32 page_end = (address + len - 1) & (~0xff);
	u32 page_cnt = ((page_end - page_begin) >> 8) + 1;

	u32 addr_begin = address;
	u32 addr_end = (page_cnt == 1) ? (address + len) : (page_begin + 0x100);
	u32 size = addr_end - addr_begin;

	if (len == 0) {
		RTK_LOGW(NOTAG, "function %s, data length is invalid (0) \r\n", __func__);
		goto exit;
	}

	if (IS_FLASH_ADDR((u32)pbuf)) {
		RTK_LOGE(NOTAG, "function %s, source address(%08x) can not be flash address\r\n", __func__, pbuf);
		assert_param(0);
	}

	FLASH_Write_Lock();
	while (page_cnt) {
		FLASH_TxData(addr_begin, size, pbuf);
		pbuf += size;

		page_cnt--;
		addr_begin = addr_end;
		addr_end = (page_cnt == 1) ? (address + len) : (addr_begin + 0x100);
		size = addr_end - addr_begin;
	}

	DCache_Invalidate(SPI_FLASH_BASE + address, len);
	RSIP_MMU_Cache_Clean();
	FLASH_Write_Unlock();

exit:
	return 1;
}
