/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "FreeRTOS.h"

uint32_t PrevIrqStatus;

#ifdef CONFIG_ARM_CORE_CM4
SRAMDRAM_ONLY_TEXT_SECTION
void FLASH_Write_IPC_Int(void *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	(void) Data;
	(void) IrqStatus;
	(void) ChanNum;

	__disable_irq();

	PIPC_MSG_STRUCT ipc_msg = (PIPC_MSG_STRUCT)ipc_get_message(IPC_AP_TO_NP, IPC_A2N_FLASHPG_REQ);
	u8 *pflag = (u8 *)ipc_msg->msg;
	/* Sent Flag to let ca32 know */
	pflag[0] = 1;
	DCache_Clean((u32)pflag, sizeof(pflag));

	/* Wait ca32 program done */
	while (1) {
		DCache_Invalidate((u32)pflag, sizeof(pflag));
		if (pflag[0] == 0) {
			break;
		}
	}

	__enable_irq();
}

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_flashpg_table[] = {
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = FLASH_Write_IPC_Int,
		.RxIrqData = (void *) NULL,
		.Txfunc = NULL,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_AP_TO_NP,
		.IPC_Channel = IPC_A2N_FLASHPG_REQ
	}
};
#endif

ALIGNMTO(CACHE_LINE_SIZE) u8 Flash_Sync_Flag[CACHE_LINE_ALIGMENT(64)];

/**
  * @brief  This function is used to lock CPU when write or erase flash under XIP.
  * @note
  *		- all interrupt include systick will be stopped.
  * @retval none
  */
SRAMDRAM_ONLY_TEXT_SECTION
void FLASH_Write_Lock(void)
{
	/* disable irq */
	PrevIrqStatus = portSET_INTERRUPT_MASK_FROM_ISR();

	/* Add This Code For XIP when ca32 Program Flah */
#ifdef CONFIG_ARM_CORE_CA32
	/*1. Close Core1 to avoid Core1 Access Flash */
	vPortGateOtherCore();
#ifdef CONFIG_XIP_FLASH
	/*2. Sent IPC to KM4 */
	IPC_MSG_STRUCT ipc_msg_temp;
	_memset(Flash_Sync_Flag, 0, sizeof(Flash_Sync_Flag));
	DCache_Clean((u32)Flash_Sync_Flag, sizeof(Flash_Sync_Flag));

	ipc_msg_temp.msg_type = IPC_USER_POINT;
	ipc_msg_temp.msg = (u32)Flash_Sync_Flag;
	ipc_msg_temp.msg_len = 1;
	ipc_msg_temp.rsvd = 0; /* for coverity init issue */
	ipc_send_message(IPC_AP_TO_NP, IPC_A2N_FLASHPG_REQ, &ipc_msg_temp);

	while (1) {
		DCache_Invalidate((u32)Flash_Sync_Flag, sizeof(Flash_Sync_Flag));
		if (Flash_Sync_Flag[0]) {
			break;
		}
	}
#endif
#endif
}

/**
  * @brief  This function is used to unlock CPU after write or erase flash under XIP.
  * @note
  *		- all interrupt will be restored.
  * @retval none
  */
SRAMDRAM_ONLY_TEXT_SECTION
void FLASH_Write_Unlock(void)
{
#ifdef CONFIG_ARM_CORE_CA32
#ifdef CONFIG_XIP_FLASH
	/*1. Let KM4 Go */
	Flash_Sync_Flag[0] = 0;
	DCache_Clean((u32)Flash_Sync_Flag, sizeof(Flash_Sync_Flag));
#endif
	/*2. Wakeup Core1 */
	vPortWakeOtherCore();
#endif

	/* restore irq */
	portCLEAR_INTERRUPT_MASK_FROM_ISR(PrevIrqStatus);
}

/**
* @brief  This function is used to send Rx command to flash to get status register or flash id, and lock CPU when Rx
* @param  cmd: command that need to be sent.
* @param  read_len: the number of bytes that will be read by SPIC after sending cmd.
* @param  read_data: pointer to a byte array which is used to save received data.
* @note This function is only used for rx status/flash id ,not used for read flash data.
*		Only work in OneBitMode.
* @retval none
*/
SRAMDRAM_ONLY_TEXT_SECTION
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
  * @retval     none
  */
SRAMDRAM_ONLY_TEXT_SECTION
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
  * @retval none
  */
SRAMDRAM_ONLY_TEXT_SECTION
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
  *		- page program(256B) time typical is 0.7ms: BaudRate=2.9Mbps, so one bit mode is enough.
  *		- page program(12B) time typical is 20+2.5*11= 47.5us BaudRate = 2.02M bps, so program 12B once is enough.
  *		- for compatibility with amebaz, which has 16-byte TX FIFO is 16 byte and max len is 16-cmdlen = 12 byte
  * @retval none
  */
SRAMDRAM_ONLY_TEXT_SECTION
void FLASH_TxData12BXIP(u32 StartAddr, u8 DataPhaseLen, u8 *pData)
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
  * @retval none
  */
SRAMDRAM_ONLY_TEXT_SECTION
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
  * @brief  This function is used to write data to flash in OneBitMode and User Mode, and lock CPU when write.
  * @param  StartAddr: Start address in flash from which SPIC writes.
  * @param  DataPhaseLen: the number of bytes that SPIC sends in Data Phase.
  * @param  pData: pointer to a byte array that is to be sent.
  * @note
  *		- page program(256B) time typical is 0.7ms: BaudRate=2.9Mbps, so one bit mode is enough.
  *		- should use FLASH_SW_CS_Control to protect flash write
  * @retval none
  */
SRAMDRAM_ONLY_TEXT_SECTION
void FLASH_TxData256BXIP(u32 StartAddr, u32 DataPhaseLen, u8 *pData)
{
	FLASH_Write_Lock();

	FLASH_TxData(StartAddr, DataPhaseLen, pData);
	DCache_Invalidate(SPI_FLASH_BASE + StartAddr, DataPhaseLen);

	FLASH_Write_Unlock();
}

/**
  * @brief  Read a stream of data from specified address
  * @param obj: Flash object define in application software.
  * @param  address: Specifies the starting address to read from.
  * @param  len: Specifies the length of the data to read.
  * @param  data: Specified the address to save the readback data.
  * @retval   status: Success:1 or Failure: Others.
  * @note auto mode is ok, because we have flash cache
  */
SRAMDRAM_ONLY_TEXT_SECTION
int  FLASH_ReadStream(u32 address, u32 len, u8 *pbuf)
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
SRAMDRAM_ONLY_TEXT_SECTION
int  FLASH_WriteStream(u32 address, u32 len, u8 *pbuf)
{
	// Check address: 4byte aligned & page(256bytes) aligned
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
	/* Clean MMU cache */
	RSIP_MMU_Cache_Clean();
	FLASH_Write_Unlock();

exit:
	return 1;
}


/**
  * @brief    Configure SPIC IP Clock.
  * @param  Source:  This parameter can be one of the following values:
  *                            @arg BIT_SHIFT_FLASH_CLK_XTAL
  *                            @arg BIT_SHIFT_FLASH_CLK_PLL
  * @param  Protection:  if disable interrupt when switch clock:
  * @retval   None
  */
SRAMDRAM_ONLY_TEXT_SECTION
void FLASH_ClockSwitch(u32 Source, u32 Protection)
{
	/* To avoid gcc warnings */
	(void) Source;
	(void) Protection;
#if defined (CONFIG_ARM_CORE_CM0)
	u32 Temp = 0;
	u32 timeout = 20;

	if (Protection) {
		asm volatile("cpsid i" : : : "memory");
		//asm volatile ("cpsid f" : : : "memory");
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	}

	/* sequence should be followed strickly */
	if (Source == BIT_LSYS_CKSL_SPIC_XTAL) {
		/* 1. clock source switch to XTAL */
		RCC_PeriphClockSource_SPIC(Source);
		DelayUs(10);

		/* 2. disable clock ps div & disable clock div */
		Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_HSYS_SPIC_CTRL);
		Temp &= ~(HSYS_BIT_FLASH_DIV_EN | HSYS_BIT_FLASH_PS_DIV_EN);
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_HSYS_SPIC_CTRL, Temp);
		FLASH_CalibrationNewCmd(DISABLE);

		/* 3. Disable PLL_PS, SMART does not use PLL_PS */

		/* 4. SPIC Dummy to low speed dummy */
		flash_init_para.FLASH_rd_sample_phase = SPIC_LOWSPEED_SAMPLE_PHASE;
		FLASH_SetSpiMode(&flash_init_para, flash_init_para.FLASH_cur_bitmode);
	} else if (Source == BIT_SHIFT_FLASH_CLK_PLL) {
		/* 1. enable PLL_PS, SMART does not use PLL_PS */

		/* 2. enable clock ps div & disable clock div */
		Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_HSYS_SPIC_CTRL);
		Temp |= (HSYS_BIT_FLASH_DIV_EN | HSYS_BIT_FLASH_PS_DIV_EN); /* enable clock ps div & enable clock div*/
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_HSYS_SPIC_CTRL, Temp);
		FLASH_CalibrationNewCmd(ENABLE);

		/* wait clock ready about 40us */
		while (timeout > 0) {
			timeout--;
			Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_HSYS_SPIC_CTRL);
			if ((Temp & HSYS_BIT_FLASH_DIV_RDY) && (Temp & HSYS_BIT_FLASH_PS_DIV_RDY)) {
				break;
			}
		}

		/* 3. clock source switch */
		RCC_PeriphClockSource_SPIC(Source);
		DelayUs(10);

		/* 4. SPIC Dummy to high speed dummy */
		flash_init_para.FLASH_rd_sample_phase = flash_init_para.FLASH_rd_sample_phase_cal;
		FLASH_SetSpiMode(&flash_init_para, flash_init_para.FLASH_cur_bitmode);
	}

	if (Protection) {
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
		asm volatile("cpsie i" : : : "memory");
		//asm volatile ("cpsie f" : : : "memory");
	}
#endif
}
