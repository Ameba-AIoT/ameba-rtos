/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

#define FLASH_RAM_TEXT_SECTION	SRAMDRAM_ONLY_TEXT_SECTION

uint32_t PrevIrqStatus;

/**
  * @brief  This function is used to lock CPU when write or erase flash under XIP.
  * @note
  *		- all interrupt include systick will be stopped.
  * @retval none
  */
void FLASH_Write_Lock(void)
{
#ifdef CONFIG_CODE_PSRAM_DATA_SRAM
	/* Get core-to-core hardware semphone */
	while (!IPC_SEMTake(IPC_SEM_FLASH, 0xFFFFFFFF)) {
		rtos_time_delay_ms(1);
	}
#else
	/* disable irq */
	PrevIrqStatus = irq_disable_save();

	/* Get core-to-core hardware semphone */
	while (!IPC_SEMTake(IPC_SEM_FLASH, 0xFFFFFFFF));
#endif

}

/**
  * @brief  This function is used to unlock CPU after write or erase flash under XIP.
  * @note
  *		- all interrupt will be restored.
  * @retval none
  */
void FLASH_Write_Unlock(void)
{
#ifdef CONFIG_CODE_PSRAM_DATA_SRAM
	/* Free core-to-core hardware semphone */
	while (!IPC_SEMFree(IPC_SEM_FLASH)) {
		rtos_time_delay_ms(1);
	}
#else
	/* Free core-to-core hardware semphone */
	while (!IPC_SEMFree(IPC_SEM_FLASH));
	/* restore irq */
	irq_enable_restore(PrevIrqStatus);
#endif




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
int  FLASH_ReadStream(u32 address, u32 len, u8 *pbuf)
{
	assert_param(pbuf != NULL);

	FLASH_Write_Lock();
	_memcpy(pbuf, (const void *)(SPI_FLASH_BASE + address), len);
	FLASH_Write_Unlock();

	return 1;
}

/**
  * @brief  Write a stream of data to specified address
  * @param  address: Specifies the starting address to write to.
  * @param  len: Specifies the length of the data to write.
  * @param  data: Pointer to a byte array that is to be written.
  * @retval   status: Success:1 or Failure: Others.
  */
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
FLASH_RAM_TEXT_SECTION
void FLASH_ClockSwitch(u32 Source, u32 Protection)
{
	u32 Temp = 0;
	u32 PreState_tick = 0;
	u32 PreState_irq = 0;

	if (Protection) {
		PreState_tick = Systick_State();
		PreState_irq = irq_disable_save();
		Systick_Cmd(DISABLE);
	}

	/* sequence should be followed strickly */
	if (Source == BIT_LSYS_CKSL_SPIC_LBUS) {
		/* 1. clock source switch to XTAL */
		RCC_PeriphClockSource_SPIC(BIT_LSYS_CKSL_SPIC_LBUS);

		/* 2. close 400M & 400MPS */
		Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_SPIC_CTRL);
		Temp &= ~(LSYS_BIT_FLASH_PS_DIV_EN | LSYS_BIT_FLASH_DIV_EN); /* disable clock ps div & disable clock div*/
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_SPIC_CTRL, Temp);

		if (flash_init_para.phase_shift_idx != 0) {
			FLASH_CalibrationNewCmd(DISABLE);
		} else {
			FLASH_Read_HandShake_Cmd(0, DISABLE);
		}

		FLASH_CalibrationPLLSel(TRUE);
		FLASH_Calibration_PSPLL_Close();
		FLASH_CalibrationPLLSel(FALSE);
		FLASH_Calibration_PSPLL_Close();

		/* 3. SPIC Dummy to low speed dummy */
		flash_init_para.FLASH_rd_sample_dly_cycle = SPIC_LOWSPEED_SAMPLE_PHASE;
		FLASH_SetSpiMode(&flash_init_para, flash_init_para.FLASH_cur_bitmode);
	} else {
		/* 1. enable 400M & 400MPS */
		/*HW need DSPPLL & CPUPLL open*/
		if (Source) {
			FLASH_CalibrationPLLSel(FALSE);
			FLASH_Calibration_PSPLL_Open();
			FLASH_CalibrationPLLSel(TRUE);
			FLASH_Calibration_PSPLL_Open();
		} else {
			FLASH_CalibrationPLLSel(TRUE);
			FLASH_Calibration_PSPLL_Open();
			FLASH_CalibrationPLLSel(FALSE);
			FLASH_Calibration_PSPLL_Open();
		}

		u8 phase_sel = FLASH_SHIFT_IDX_TO_PAHSE(flash_init_para.phase_shift_idx);
		FLASH_CalibrationPLLPS_Shift(phase_sel);

		Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_SPIC_CTRL);
		Temp |= (LSYS_BIT_FLASH_PS_DIV_EN | LSYS_BIT_FLASH_DIV_EN); /* enable clock ps div & enable clock div*/
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_SPIC_CTRL, Temp);

		/* wait clock ready timing need check */
		while (1) {
			Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_SPIC_CTRL);
			if ((Temp & LSYS_BIT_FLASH_PS_DIV_RDY) && (Temp & LSYS_BIT_FLASH_DIV_RDY)) {
				break;
			}
		}

		if (flash_init_para.phase_shift_idx != 0) {
			FLASH_CalibrationNewCmd(ENABLE);
		} else {
			FLASH_Read_HandShake_Cmd(flash_init_para.FLASH_rd_sample_dly_cycle_cal - 2, ENABLE);
		}

		/* 2. clock source switch */
		if (Source) {
			RCC_PeriphClockSource_SPIC(BIT_LSYS_CKSL_SPIC_CPUPLL);
		} else {
			RCC_PeriphClockSource_SPIC(BIT_LSYS_CKSL_SPIC_DSPPLL);
		}

		/* 3. SPIC Dummy to high speed dummy */
		flash_init_para.FLASH_rd_sample_dly_cycle = flash_init_para.FLASH_rd_sample_dly_cycle_cal;
		FLASH_SetSpiMode(&flash_init_para, flash_init_para.FLASH_cur_bitmode);
	}

	if (Protection) {
		Systick_Cmd(PreState_tick);
		irq_enable_restore(PreState_irq);
	}
}
