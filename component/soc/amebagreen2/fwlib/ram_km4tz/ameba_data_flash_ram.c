/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

/**
 * @brief  This function is used to send Rx command to flash to get status register or flash id, and lock CPU when Rx
 * @param  cmd: command that need to be sent.
 * @param  read_len: the number of bytes that will be read by SPIC after sending cmd.
 * @param  read_data: pointer to a byte array which is used to save received data.
 * @note This function is only used for rx status/flash id ,not used for read flash data.
 *		Only work in OneBitMode.
 * @retval None
 */
void DATA_FLASH_RxCmdXIP(u8 cmd, u32 read_len, u8 *read_data)
{
	IPC_SEMTake(IPC_SEM_DATA_FLASH, 0xffffffff);

	DATA_FLASH_RxCmd(cmd, read_len, read_data);

	IPC_SEMFree(IPC_SEM_DATA_FLASH);

}

/**
 * @brief  This function is used to set register status. FLASH_WriteEn & FLASH_WaitBusy, and lock CPU when set
 *  		are included in this function to avoid hardfault when TxCmd in XIP
 * @param    Cmd: command to be sent
 * @param    Len: the number of bytes to be sent after Cmd
 * @param    Status: pointer to byte array to be sent
 * @retval     None
 */
void DATA_FLASH_SetStatusXIP(u8 Cmd, u32 Len, u8 *Status)
{
	IPC_SEMTake(IPC_SEM_DATA_FLASH, 0xffffffff);

	DATA_FLASH_SetStatus(Cmd, Len, Status);

	IPC_SEMFree(IPC_SEM_DATA_FLASH);

}

/**
 * @brief  This function is used to set or clear status bits., used to set protect bit or quad enable bit, and lock CPU when set
 * @param    SetBits: 16bits valid, SetBits[7:0] is status1 & SetBits[15:8] is status2
 * @param    NewState: ENABLE/DISABLE
 * @retval None
 */
void DATA_FLASH_SetStatusBitsXIP(u32 SetBits, u32 NewState)
{
	IPC_SEMTake(IPC_SEM_DATA_FLASH, 0xffffffff);

	DATA_FLASH_SetStatusBits(SetBits, NewState);

	IPC_SEMFree(IPC_SEM_DATA_FLASH);
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
void DATA_FLASH_TxDataXIP(u32 StartAddr, u32 DataPhaseLen, u8 *pData)
{
	IPC_SEMTake(IPC_SEM_DATA_FLASH, 0xffffffff);

	DATA_FLASH_TxData(StartAddr, DataPhaseLen, pData);
	DCache_Invalidate(DATA_FLASH_BASE + StartAddr, DataPhaseLen);

	IPC_SEMFree(IPC_SEM_DATA_FLASH);
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
void DATA_FLASH_EraseXIP(u32 EraseType, u32 Address)
{
	IPC_SEMTake(IPC_SEM_DATA_FLASH, 0xffffffff);

	DATA_FLASH_Erase(EraseType, Address);
	if (EraseType == EraseSector) {
		DCache_Invalidate(DATA_FLASH_BASE + Address, 0x1000);
	} else if (EraseType == EraseBlock) {
		DCache_Invalidate(DATA_FLASH_BASE + Address, 0x10000);
	} else {
		DCache_Invalidate(DATA_FLASH_BASE, 0x0FFFFFFF - DATA_FLASH_BASE);
	}

	IPC_SEMFree(IPC_SEM_DATA_FLASH);
}

/**
 * @brief  This function is used to read a stream of data from specified address
 * @param obj: Flash object define in application software.
 * @param  address: Specifies the starting address to read from.
 * @param  len: Specifies the length of the data to read.
 * @param  data: Specified the address to save the readback data.
 * @retval   status: Success:1 or Failure: Others.
 * @note Auto mode is ok, because we have flash cache
 */
int DATA_FLASH_ReadStream(u32 address, u32 len, u8 *pbuf)
{
	assert_param(pbuf != NULL);

	IPC_SEMTake(IPC_SEM_DATA_FLASH, 0xffffffff);
	_memcpy(pbuf, (const void *)(DATA_FLASH_BASE + address), len);
	IPC_SEMFree(IPC_SEM_DATA_FLASH);

	return 1;
}

/**
 * @brief  This function is used to write a stream of data to specified address
 * @param  address: Specifies the starting address to write to.
 * @param  len: Specifies the length of the data to write.
 * @param  data: Pointer to a byte array that is to be written.
 * @retval   status: Success:1 or Failure: Others.
 */
int DATA_FLASH_WriteStream(u32 address, u32 len, u8 *pbuf)
{
	/* Check address: 4byte aligned & page(256bytes) aligned */
	u32 page_begin = address & (~0xff);
	u32 page_end = (address + len - 1) & (~0xff);
	u32 page_cnt = ((page_end - page_begin) >> 8) + 1;

	u32 addr_begin = address;
	u32 addr_end = (page_cnt == 1) ? (address + len) : (page_begin + 0x100);
	u32 size = addr_end - addr_begin;

	if (len == 0) {
		RTK_LOGW(NOTAG, "Func %s: invalid Len\r\n", __func__);
		goto exit;
	}

	if (IS_DATA_FLASH_ADDR((u32)pbuf)) {
		RTK_LOGE(NOTAG, "invalid src: %08x\r\n", pbuf);
		assert_param(0);
	}

	IPC_SEMTake(IPC_SEM_DATA_FLASH, 0xffffffff);
	while (page_cnt) {
		DATA_FLASH_TxData(addr_begin, size, pbuf);
		pbuf += size;

		page_cnt--;
		addr_begin = addr_end;
		addr_end = (page_cnt == 1) ? (address + len) : (addr_begin + 0x100);
		size = addr_end - addr_begin;
	}

	DCache_Invalidate(DATA_FLASH_BASE + address, len);
	RSIP_MMU_Cache_Clean();
	IPC_SEMFree(IPC_SEM_DATA_FLASH);

exit:
	return 1;
}
