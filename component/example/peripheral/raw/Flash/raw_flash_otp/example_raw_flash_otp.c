/**
  ******************************************************************************
  * @file
  * @author
  * @version
  * @date
  * @brief   This file provides a example to aperate OTP
  *             including four sections:
  *           - Initialization
  *           - Test security registers before OTP
  *           - Set security register OTP bit
  *           - Check OTP result after set OTP bit
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */
#include "ameba_soc.h"
#include "main.h"
#include <stdbool.h>
#include "os_wrapper.h"
#include <stdio.h>

#define TEST_DATA_SIZE          256
#define SCUR_NUM                3
#define FLASH_CMD_ERSCUR        0x44            // erase security register
#define FLASH_CMD_WRSCUR        0x42            // write security register
#define FLASH_CMD_RDSCUR        0x48            // read security register
#define FLASH_CMD_WRSR2         0x31            // write status register2
#define FLASH_CMD_RDSR2         0x35            // read status register2
#define SCUR_OTP_BITS          0x0800           // Security Register Lock Bit
#define SCUR_GROUP(Address)   ((Address & 0xfffff000) >> 12) // get secure reister group
#define SCUR_OFFSET(Address)  (Address & 0x00000fff)  // get secure reister offset
#define IS_IN_SCUR_GROUP(Address)   ((SCUR_GROUP(Address) >= 1) && (SCUR_GROUP(Address) <= 3)) // Indicates address is in Security Register1 range or not
#define IS_IN_SCUR_OFFSET(Address)   (SCUR_OFFSET(Address) < 0x100) // Indicates address is in Security Register2 range or not
#define IS_IN_SCUR_RANGE(Address)    (IS_IN_SCUR_GROUP(Address) && IS_IN_SCUR_OFFSET(Address)) // Indicates address is in Security Register1~3 range or not

/**
  * @brief Erase Security Register
  * @param  Address: the security register address that you want to erase
  * @retval  true or false
  */
SRAMDRAM_ONLY_TEXT_SECTION
bool EraseSecurityRegister(u32 Address)
{
	if (!IS_IN_SCUR_RANGE(Address)) {
		RTK_LOGI(NOTAG, " Address is illegal.\n");
		return false;
	}
	FLASH_Write_Lock();
	FLASH_TxBasic(FLASH_CMD_ERSCUR, Address, 0, NULL);
	FLASH_Write_Unlock();
	return true;
}

/**
  * @brief  This function is used to write data to flash in OneBitMode and User Mode.
  * @param  cmd: Program security register command.
  * @param  Address: Start address in flash from which SPIC writes.
  * @param  DataLen: the number of bytes that SPIC sends in Data Phase.
  * @param  TransmitData: pointer to a byte array that is to be sent.
  * @retval none
  */
SRAMDRAM_ONLY_TEXT_SECTION
bool ProgramSecurityRegister(u32 Address, u32 DataLen, u8 *TransmitData)
{
	if (!(IS_IN_SCUR_RANGE(Address) && IS_IN_SCUR_RANGE((Address + DataLen)))) {
		RTK_LOGI(NOTAG, " Address is illegal.\n");
		return false;
	}
	FLASH_Write_Lock();
	FLASH_TxBasic(FLASH_CMD_WRSCUR, Address, DataLen, TransmitData);
	FLASH_Write_Unlock();
	return true;
}

/**
  * @brief Read Security Register
  * @param  cmd: Read security register command.
  * @param  Address: the security register address that you want to read
  * @param  read_len: the data numbers(unit:byte) that you want to read
  * @param  read_data: pointer to byte array which is used to save received data.
  * @retval  true or false
  */
SRAMDRAM_ONLY_TEXT_SECTION
bool ReadSecurityRegister(u32 Address, u32 read_len, u8 *read_data)
{
	if (!(IS_IN_SCUR_RANGE(Address) && IS_IN_SCUR_RANGE((Address + read_len)))) {
		RTK_LOGI(NOTAG, " Address is illegal.\n");
		return false;
	}
	FLASH_Write_Lock();
	flash_init_para.FLASH_rd_dummy_cycle[0] = 8;
	FLASH_RxBasic(FLASH_CMD_RDSCUR, Address, read_len, read_data);
	flash_init_para.FLASH_rd_dummy_cycle[0] = 0;
	FLASH_Write_Unlock();
	return true;
}

/**
  * @brief According to Address lock security Register
  * @param  Address: the security register address that you want to lock.
  * @retval  true or false
  */
SRAMDRAM_ONLY_TEXT_SECTION
bool LockSecurityRegister(u32 Address)
{
	u32 lockbits;
	if (!(IS_IN_SCUR_RANGE(Address))) {
		RTK_LOGI(NOTAG, " Address is illegal.\n");
		return false;
	}
	lockbits = SCUR_OTP_BITS << (SCUR_GROUP(Address) - 1);
	FLASH_SetStatusBits(lockbits, ENABLE);
	return true;
}

SRAMDRAM_ONLY_TEXT_SECTION
void flash_test_task(void *param)
{
	uint32_t Address = 0x00001001; //the address that you want to test
	uint16_t DataLen = 8; // the test data length
	uint8_t TransmitData[TEST_DATA_SIZE];
	uint8_t ReceiveData[TEST_DATA_SIZE];
	UNUSED(param);

	/*assign values to TransmitData[]*/
	for (int i = 0; i < TEST_DATA_SIZE; i++) {
		TransmitData[i] = _rand();
	}

	/*initialize ReceiveData[]*/
	_memset(ReceiveData, 0, sizeof(ReceiveData));

	/*
	 * test security registers before OTP
	 */
	bool isErased = EraseSecurityRegister(Address);
	if (isErased == false) {
		RTK_LOGI(NOTAG, "\nErase Security Register error.\n");
		while (1);
	}

	bool isProgramed = ProgramSecurityRegister(Address, DataLen, (u8 *)&TransmitData[0]);
	if (isProgramed == false) {
		RTK_LOGI(NOTAG, "Program Security Register error.\n");
		while (1);
	}

	bool isRead = ReadSecurityRegister(Address, DataLen, (u8 *)ReceiveData);
	if (isRead == false) {
		RTK_LOGI(NOTAG, "Verify Security Register error.\n");
		while (1);
	}

	/*Verify Security Register by comparing Transmitdata with Rcevivedata*/
	for (int i = 0; i < DataLen; i++) {
		if (TransmitData[i] != ReceiveData[i]) {
			RTK_LOGI(NOTAG, "verify failed: TransmitData[%d]:%02x -> ReceiveData[%d]:%02x\r\n ", i, TransmitData[i], i, ReceiveData[i]);
		}
	}
	/*print ReceiveData before OTP*/
	RTK_LOGI(NOTAG, "\nReceiveData[] is:\n");
	for (int j = 0; j < DataLen; j++) {
		RTK_LOGI(NOTAG, "%02x ", ReceiveData[j]);
	}
	RTK_LOGI(NOTAG, "\n########Test security registers before OTP finished########\n");
	/* If you want test security register OTP function, set "#if 0" to "#if 1".
	 * Warning: Once a lock bit is set to 1, the corresponding security register will be permanently locked. */
#if 0
	uint8_t TransmitDataOTP[TEST_DATA_SIZE];
	uint8_t ReceiveDataOTP[TEST_DATA_SIZE];
	uint8_t StatusRegister2OTP = 0;
	/*assign values to TransmitDataOTP[]*/
	for (int i = 0; i < TEST_DATA_SIZE; i++) {
		TransmitDataOTP[i] = _rand();
	}
	/*initialize ReceiveDataOTP[]*/
	_memset(ReceiveDataOTP, 0, sizeof(ReceiveDataOTP));

	/*Lock security register*/
	bool isLocked = LockSecurityRegister(Address);
	if (isLocked == false) {
		RTK_LOGI(NOTAG, "Set security register OTP bit error.\n");
		while (1);
	}
	FLASH_RxCmd(FLASH_CMD_RDSR2, 1, (u8 *)&StatusRegister2OTP);           //read StatusRegister2 after setting OTP
	RTK_LOGI(NOTAG, "after set OTP bit StatusRegister2 is %x\n", StatusRegister2OTP);
	RTK_LOGI(NOTAG, "########Lock security register finished########\n");


	/*Check OTP result after set OTP bit*/
	isErased = EraseSecurityRegister(Address);
	if (isErased == false) {
		RTK_LOGI(NOTAG, "Erase OTP Security Register error.\n");
		while (1);
	}

	isProgramed = ProgramSecurityRegister(Address, DataLen, (u8 *)&TransmitDataOTP[0]);
	if (isProgramed == false) {
		RTK_LOGI(NOTAG, "Program OTP Security Register error.\n");
		while (1);
	}

	isRead = ReadSecurityRegister(Address, DataLen, (u8 *)ReceiveDataOTP);
	if (isRead == false) {
		RTK_LOGI(NOTAG, "Verify OTP Security Register error.\n");
		while (1);
	}
	/*Verify OTP result by comparing Receivedata before OTP with Rcevivedata after OTP*/
	for (int i = 0; i < DataLen; i++) {
		if (ReceiveDataOTP[i] != ReceiveData[i]) {
			RTK_LOGI(NOTAG, "OTP verify failed: ReceiveDataOTP[%d]:%02x -> ReceiveData[%d]:%02x\r\n ", i, ReceiveDataOTP[i], i, ReceiveData[i]);
		}
	}
	/*print ReceiveData after OTP*/
	RTK_LOGI(NOTAG, "ReceiveDataOTP[] is:\n");
	for (int j = 0; j < DataLen; j++) {
		RTK_LOGI(NOTAG, "%02x ", ReceiveDataOTP[j]);
	}
	RTK_LOGI(NOTAG, "\n########Check OTP result finished########\n");
#endif

	rtos_task_delete(NULL);
}

int example_raw_flash_otp(void)
{
	if (rtos_task_create(NULL, ((const char *)"flash_test_task"), flash_test_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGI(NOTAG, "\n\r%s rtos_task_create(flash_test_task) failed", __FUNCTION__);
	}
	return 0;
}
