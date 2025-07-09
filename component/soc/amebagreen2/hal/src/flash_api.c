/** mbed Microcontroller Library
 ******************************************************************************
 * @file	flash_api.c
 * @author
 * @version V1.0.0
 * @date	2016-08-01
 * @brief	This file provides mbed API for flash.
 ******************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
 ******************************************************************************
 */

#include "objects.h"
#include "PinNames.h"
#include "ameba_soc.h"
#include "flash_api.h"


/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @defgroup MBED_FLASH
 *  @brief    MBED_FLASH driver modules.
 *  @{
 */

/** @defgroup MBED_FLASH_Exported_Functions MBED_FLASH Exported Functions
  * @{
  */

/**
  * @brief  Configure flash write protect function.
  * @param  obj: Flash object defined in application software.
  * @param  protect: This parameter can be 1 or 0.
  *		@arg 1: Protect the whole chip from being programmed/erased.
  * 	@arg 0: Unprotect the whole chip from being programmed/erased.
  * @retval none
  */
void flash_write_protect(flash_t *obj, u32 protect)
{
	/* To avoid gcc warnings */
	(void) obj;

	FLASH_SetStatusBitsXIP(0x1c, protect);
}

/**
  * @brief  Accomplish desired operation by setting status register.
  * @param  obj: Flash object defined in application software.
  * @param  data: Specify the bit of status1 that user want to set.
  * For example, if user want to set the 3rd bit, data will be 0x8.
  * @return Result. It can be one of the following values:
  * @retval 1: Success.
  * @retval Others: Error.
  * @note  Please refer to the flash datasheet for more info about status register,
  *        as well as block protected area and related control bits.
  */
SRAMDRAM_ONLY_TEXT_SECTION
int flash_set_status(flash_t *obj, u32 data)
{
	/* To avoid gcc warnings */
	(void) obj;

	u8 status[2];
	u8 StatusLen = 1;

	status[0] = (u8)data;
	FLASH_Write_Lock();

	/* check if status2, some flash need to write status2 */
	if (flash_init_para.FLASH_Status2_exist) {
		StatusLen = 2;
		FLASH_RxCmd(flash_init_para.FLASH_cmd_rd_status2, 1, &status[1]);
	}

	if (!flash_init_para.FLASH_cmd_wr_status2) {
		FLASH_SetStatus(flash_init_para.FLASH_cmd_wr_status, StatusLen, status);
	} else {
		FLASH_SetStatus(flash_init_para.FLASH_cmd_wr_status, 1, &status[0]);
		FLASH_SetStatus(flash_init_para.FLASH_cmd_wr_status2, 1, &status[1]);
	}
	FLASH_Write_Unlock();

	return 1;
}

/**
  * @brief Reset status register1 to 0.
  * @param obj: Flash object defined in application software.
  * @retval none
  * @note Make sure the operation is appropriate and remember to reset status register2 to 0 if flash chip has status register2.
  */
void flash_reset_status(flash_t *obj)
{
	flash_set_status(obj, 0);
	return;
}

/**
  * @brief  Get the value of status register1.
  * @param  obj: Flash object defined in application software.
  * @return The value of status register1.
  */
int flash_get_status(flash_t *obj)
{
	/* To avoid gcc warnings */
	(void) obj;

	int data;
	FLASH_RxCmdXIP(flash_init_para.FLASH_cmd_rd_status, 1, (u8 *)&data);
	return data;
}

/**
  * @brief  Erase flash sector(4KB).
  * @param  obj: Flash object defined in application software.
  * @param  address: Starting address to be erased.
  * @retval none
  * @note LSB 12bits will be masked.
  */
void flash_erase_sector(flash_t *obj, u32 address)
{
	/* To avoid gcc warnings */
	(void) obj;
	FLASH_EraseXIP(EraseSector, address);
}

/**
  * @brief  Erase flash block(64KB).
  * @param  obj: Flash object defined in application software.
  * @param  address: Starting address to be erased.
  * @retval none
  * @note LSB 16bits will be masked.
  */
void flash_erase_block(flash_t *obj, u32 address)
{
	/* To avoid gcc warnings */
	(void) obj;
	FLASH_EraseXIP(EraseBlock, address);
}

/**
  * @brief  Erase the whole flash chip.
  * @param  obj: Flash object defined in application software.
  * @retval none
  */
void flash_erase_chip(flash_t *obj)
{
	/* To avoid gcc warnings */
	(void) obj;
	FLASH_EraseXIP(EraseChip, 0);
}

/**
  * @brief  Read a word from specified address.
  * @param  obj: Flash object defined in application software.
  * @param  address: Address to read from.
  * @param  data: Address to save the readback data.
  * @return Read result.
  * @retval 1: Success.
  * @retval Others: Error.
  * @note It is ok to read under auto mode due to flash cache.
  */
int  flash_read_word(flash_t *obj, u32 address, u32 *data)
{
	/* To avoid gcc warnings */
	(void) obj;
	FLASH_ReadStream(address, sizeof(u32), (u8 *)data);
	return 1;
}

/**
  * @brief  Write a word to specified address.
  * @param  obj: Flash object defined in application software.
  * @param  address: Address to be programmed to.
  * @param  data: Data to be programmed.
  * @return Write result.
  * @retval 1: Success.
  * @retval Others: Error.
  * @note User mode write used.
  */
int  flash_write_word(flash_t *obj, u32 address, u32 data)
{
	/* To avoid gcc warnings */
	(void) obj;
	FLASH_WriteStream(address, sizeof(u32), (u8 *)&data);

	return 1;
}

/**
  * @brief  Read a stream of data from specified address.
  * @param obj: Flash object defined in application software.
  * @param  address: Starting address to read from.
  * @param  len: Length of the data to read.
  * @param  data: Address to save the readback data.
  * @return Read result.
  * @retval 1: Success.
  * @retval Others: Error.
  * @note It is ok to read under auto mode due to flash cache.
  */
int  flash_stream_read(flash_t *obj, u32 address, u32 len, u8 *data)
{
	/* To avoid gcc warnings */
	(void) obj;
	FLASH_ReadStream(address, len, data);

	return 1;
}

/**
  * @brief  Write a stream of data to specified address.
  * @param  obj: Flash object defined in application software.
  * @param  address: Starting address to write to.
  * @param  len: Length of the data to write.
  * @param  data: Pointer to a byte array that is to be written.
  * @return Write result.
  * @retval 1: Success.
  * @retval Others: Error.
  */
int  flash_stream_write(flash_t *obj, u32 address, u32 len, u8 *data)
{
	/* To avoid gcc warnings */
	(void) obj;
	FLASH_WriteStream(address, len, data);

	return 1;
}

/**
  * @brief  Write a stream of data to specified address.
  * @param  obj: Flash object define in application software.
  * @param  address: Specifies the starting address to write to.
  * @param  len: Specifies the length of the data to write.
  * @param  data: Pointer to a byte array that is to be written.
  * @return Write result.
  * @retval 1: Success.
  * @retval Others: Error.
  * @note  It is the same with function flash_stream_write().
  */
int flash_burst_write(flash_t *obj, u32 address, u32 Length, u8 *data)
{
	flash_stream_write(obj, address, Length, data);
	return 1;
}

/**
  * @brief  Read a stream of data from specified address.
  * @param  obj: Flash object defined in application software.
  * @param  address: Starting address to read from.
  * @param  len: Length of the data to read.
  * @param  data: Address to save the readback data.
  * @return Read result.
  * @retval 1: Success.
  * @retval Others: Error.
  * @note It is the same with function flash_stream_read().
  */
int  flash_burst_read(flash_t *obj, u32 address, u32 Length, u8 *data)
{
	flash_stream_read(obj, address, Length, data);
	return 1;
}

/**
  * @brief  Set extended address when access beyond 16MB by switching between 8 16MB-area(segment).
  * @param  obj: Flash object defined in application software.
  * @param  data: Specified segment.
  * @return Result.
  * @retval 1: Success.
  * @retval Others: Error.
  * @attention Only for Micron 128MB flash.
  * @note Please refer to the flash datasheet for more info about memory mapping.
  */
int flash_set_extend_addr(flash_t *obj, u32 data)
{
	/* To avoid gcc warnings */
	(void) obj;

	/*Write Extended Address Register to select operating segment*/
	u8 segnum = (u8)(data & 0x07);
	FLASH_SetStatusXIP(0xC5, 1, &segnum);
	return 1;
}

/**
  * @brief  Read from Extended Address Register, which indicates the current segment.
  * @param  obj: Flash object defined in application software.
  * @return The value of current Extended Address Register.
  * @attention Only for Micron 128MB flash.
  * @note Please refer to the flash datasheet for more info about memory mapping.
  */
int flash_get_extend_addr(flash_t *obj)
{
	/* To avoid gcc warnings */
	(void) obj;

	u8 temp = 0;
	FLASH_RxCmdXIP(0xC8, 1, &temp);
	return temp;
}

/**
  * @brief  Get flash ID (command: 0x9F).
  * @param  obj: Flash object define in application software.
  * @param  buf: Pointer to a byte array to save the readback ID.
  * @param  len: Specifies the length of the buf. It should be 3.
  * @retval -1: Fail.
  */
int flash_read_id(flash_t *obj, uint8_t *buf, uint8_t len)
{
	/* To avoid gcc warnings */
	(void) obj;

	assert_param(buf != NULL);
	assert_param(len >= 3);

	FLASH_RxCmdXIP(flash_init_para.FLASH_cmd_rd_id, len, buf);

	return len;
}
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
