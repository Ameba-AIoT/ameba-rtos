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

void FLASH_UserMode_Enter(void);
void FLASH_UserMode_Exit(void);
void FLASH_WriteEn_InUserMode(void);
void FLASH_WaitBusy_InUserMode(u32 WaitType);

#define TEST_DATA_SIZE          256
#define SCUR_NUM                3
#define FLASH_CMD_ERSCUR		0x44            // erase security register
#define FLASH_CMD_WRSCUR		0x42            // write security register 
#define FLASH_CMD_RDSCUR		0x48            // read security register
#define FLASH_CMD_WRSR2         0x31            // write status register2
#define FLASH_CMD_RDSR2         0x35            // read status register2
#define SCUR_OTP_BITS          0x0800           // Security Register Lock Bit
#define SCUR_GROUP(Address)   ((Address & 0xfffff000) >> 12) // get secure reister group
#define SCUR_OFFSET(Address)  (Address & 0x00000fff)  // get secure reister offset
#define IS_IN_SCUR_GROUP(Address)   ((SCUR_GROUP(Address) >= 1) && (SCUR_GROUP(Address) <= 3)) // Indicates address is in Security Register1 range or not
#define IS_IN_SCUR_OFFSET(Address)   (SCUR_OFFSET(Address) < 0x100) // Indicates address is in Security Register2 range or not
#define IS_IN_SCUR_RANGE(Address)    (IS_IN_SCUR_GROUP(Address) && IS_IN_SCUR_OFFSET(Address)) // Indicates address is in Security Register1~3 range or not

/**
* @brief  This function is used to read data from flash in User Mode.
* @param  cmd: Read data command.
* @param  Address: Start address in flash from which SPIC wants to read.
* @param  read_len: the number of bytes that SPIC wants to read.
* @param  read_data: pointer to a byte array which is used to save received data.
* @retval none
*/
SRAMDRAM_ONLY_TEXT_SECTION
void FLASH_RxCmd_Patch(u8 cmd, u32 Address, u32 read_len, u8 *read_data)
{
	SPIC_TypeDef *spi_flash = SPIC;
	u8 rd_cmd = cmd;
	u32 rx_num = 0;
	u32 value;

	FLASH_SetSpiMode(&flash_init_para, SpicOneBitMode);
	FLASH_UserMode_Enter();

	spi_flash->RX_NDF = RX_NDF(read_len);
	spi_flash->TX_NDF = 0;

	/* Set ADDR length */
	value = spi_flash->USER_LENGTH & ~MASK_USER_ADDR_LENGTH;
	if (flash_init_para.FLASH_dum_en) {
		value |= USER_ADDR_LENGTH(flash_init_para.FLASH_addr_phase_len + 1);
	} else {
		value |= USER_ADDR_LENGTH(flash_init_para.FLASH_addr_phase_len);
	}
	spi_flash->USER_LENGTH = value;
	spi_flash->CTRLR0 = ((spi_flash->CTRLR0 & 0xFFF0FFFF) | TMOD(3));

	/* set flash_cmd: write cmd & address to fifo & addr is MSB */
	spi_flash->DR[0].BYTE = rd_cmd;
	if (flash_init_para.FLASH_addr_phase_len == ADDR_4_BYTE) {
		spi_flash->DR[0].BYTE = (u8)((Address & 0xFF000000) >> 24);
	}

	spi_flash->DR[0].BYTE = (u8)((Address & 0xFF0000) >> 16);
	spi_flash->DR[0].BYTE = (u8)((Address & 0xFF00) >> 8);
	spi_flash->DR[0].BYTE = (u8)(Address & 0xFF);

	/* Push dummy byte*/
	if (flash_init_para.FLASH_dum_en) {
		spi_flash->DR[0].BYTE = 0x0;
	}

	/* Enable SSIENR to start the transfer */
	spi_flash->SSIENR = BIT_SPIC_EN;

	rx_num = 0;
	while (rx_num < read_len) {
		if (spi_flash->SR & BIT_RFNE) {
			read_data[rx_num] = spi_flash->DR[0].BYTE;
			rx_num += 1;
		}
	}

	/* Wait transfer complete. When complete, SSIENR.SPIC_EN are cleared by HW automatically. */
	FLASH_WaitBusy_InUserMode(WAIT_TRANS_COMPLETE);
	FLASH_UserMode_Exit();
	FLASH_SetSpiMode(&flash_init_para, flash_init_para.FLASH_cur_bitmode);
}

/**
  * @brief  This function is used to write data to flash in OneBitMode and User Mode.
  * @param  cmd: transmit data command.
  * @param  Address: Start address in flash from which SPIC writes.
  * @param  DataLen: the number of bytes that SPIC sends in Data Phase.
  * @param  TransmitData: pointer to a byte array that is to be sent.
  * @retval none
  */
SRAMDRAM_ONLY_TEXT_SECTION
void FLASH_TxCmd_Patch(u8 cmd, u32 Address, u32 DataLen, u8 *TransmitData)
{
	SPIC_TypeDef *spi_flash = SPIC;
	u32 tx_num = 0;
	u32 ctrl0;
	u32 value;

	/* write enable cmd */
	FLASH_WriteEn();
	/* Enter user mode */
	FLASH_UserMode_Enter();

	/* set CTRLR0: TX mode and one bit mode */
	ctrl0 = spi_flash->CTRLR0;
	spi_flash->CTRLR0 &= ~(TMOD(3) | CMD_CH(3) | ADDR_CH(3) | DATA_CH(3));

	/* Set ADDR length */
	value = spi_flash->USER_LENGTH & ~MASK_USER_ADDR_LENGTH;
	spi_flash->USER_LENGTH = value | USER_ADDR_LENGTH(flash_init_para.FLASH_addr_phase_len);

	/* Set TX_NDF: frame number of Tx data. */
	spi_flash->RX_NDF = 0;
	spi_flash->TX_NDF = TX_NDF(DataLen);

	/* set flash_cmd: write cmd & address to fifo & addr is MSB */
	spi_flash->DR[0].BYTE = cmd;
	if (flash_init_para.FLASH_addr_phase_len == ADDR_4_BYTE) {
		spi_flash->DR[0].BYTE = (u8)((Address & 0xFF000000) >> 24);
	}

	spi_flash->DR[0].BYTE = (u8)((Address & 0xFF0000) >> 16);
	spi_flash->DR[0].BYTE = (u8)((Address & 0xFF00) >> 8);
	spi_flash->DR[0].BYTE = (u8)(Address & 0xFF);

	/* write some data to fill the fifo before set SSIENR. */
	tx_num = 0;
	while (tx_num < DataLen) {
		if (spi_flash->SR & BIT_TFNF) {
			spi_flash->DR[0].BYTE = (u8) * (TransmitData + tx_num);
			tx_num += 1;
		} else {
			break;
		}
	}

	/* Enable SSIENR to start the transfer */
	spi_flash->SSIENR = BIT_SPIC_EN;

	/* write the remaining data into fifo */
	while (tx_num < DataLen) {
		if (spi_flash->SR & BIT_TFNF) {
			spi_flash->DR[0].BYTE = (u8) * (TransmitData + tx_num);
			tx_num += 1;
		}
	}

	/* Wait transfer complete. When complete, SSIENR.SPIC_EN are cleared by HW automatically. */
	FLASH_WaitBusy_InUserMode(WAIT_TRANS_COMPLETE);

	/* Restore bitmode */
	spi_flash->CTRLR0 = ctrl0;

	FLASH_WaitBusy_InUserMode(WAIT_FLASH_BUSY);
	FLASH_UserMode_Exit();
}
/**
  * @brief Erase Security Register
  * @param  Address: the security register address that you want to erase
  * @retval  true or false
  */
bool EraseSecurityRegister(u32 Address)
{
	if (!IS_IN_SCUR_RANGE(Address)) {
		printf(" Address is illegal.\n");
		return false;
	}
	FLASH_Write_Lock();
	FLASH_TxCmd_Patch(FLASH_CMD_ERSCUR, Address, 0, (u8 *)&Address);
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
bool ProgramSecurityRegister(u32 Address, u32 DataLen, u8 *TransmitData)
{
	if (!(IS_IN_SCUR_RANGE(Address) && IS_IN_SCUR_RANGE((Address + DataLen)))) {
		printf(" Address is illegal.\n");
		return false;
	}
	FLASH_Write_Lock();
	FLASH_TxCmd_Patch(FLASH_CMD_WRSCUR, Address, DataLen, TransmitData);
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
bool ReadSecurityRegister(u32 Address, u32 read_len, u8 *read_data)
{
	if (!(IS_IN_SCUR_RANGE(Address) && IS_IN_SCUR_RANGE((Address + read_len)))) {
		printf(" Address is illegal.\n");
		return false;
	}
	FLASH_Write_Lock();
	FLASH_RxCmd_Patch(FLASH_CMD_RDSCUR, Address, read_len, read_data);
	FLASH_Write_Unlock();
	return true;
}

/**
  * @brief According to Address lock security Register
  * @param  Address: the security register address that you want to lock.
  * @retval  true or false
  */
bool LockSecurityRegister(u32 Address)
{
	u32 lockbits;
	if (!(IS_IN_SCUR_RANGE(Address))) {
		printf(" Address is illegal.\n");
		return false;
	}
	lockbits = SCUR_OTP_BITS << (SCUR_GROUP(Address) - 1);
	FLASH_SetStatusBits(lockbits, ENABLE);
	return true;
}

static void flash_test_task(void *param)
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
		printf("\nErase Security Register error.\n");
		while (1);
	}

	bool isProgramed = ProgramSecurityRegister(Address, DataLen, (u8 *)&TransmitData[0]);
	if (isProgramed == false) {
		printf("Program Security Register error.\n");
		while (1);
	}

	bool isRead = ReadSecurityRegister(Address, DataLen, (u8 *)ReceiveData);
	if (isRead == false) {
		printf("Verify Security Register error.\n");
		while (1);
	}
	/*Verify Security Register by comparing Transmitdata with Rcevivedata*/
	for (int i = 0; i < DataLen; i++) {
		if (TransmitData[i] != ReceiveData[i]) {
			printf("verify failed: TransmitData[%d]:%02x -> ReceiveData[%d]:%02x\r\n ", i, TransmitData[i], i, ReceiveData[i]);
		}
	}
	/*print ReceiveData before OTP*/
	printf("\nReceiveData[] is:\n");
	for (int j = 0; j < DataLen; j++) {
		printf("%02x ", ReceiveData[j]);
	}
	printf("\n########Test security registers before OTP finished########\n");
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
		printf("Set security register OTP bit error.\n");
		while (1);
	}
	FLASH_RxCmd(FLASH_CMD_RDSR2, 1, (u8 *)&StatusRegister2OTP);           //read StatusRegister2 after setting OTP
	printf("after set OTP bit StatusRegister2 is %x\n", StatusRegister2OTP);
	printf("########Lock security register finished########\n");


	/*Check OTP result after set OTP bit*/
	isErased = EraseSecurityRegister(Address);
	if (isErased == false) {
		printf("Erase OTP Security Register error.\n");
		while (1);
	}

	isProgramed = ProgramSecurityRegister(Address, DataLen, (u8 *)&TransmitDataOTP[0]);
	if (isProgramed == false) {
		printf("Program OTP Security Register error.\n");
		while (1);
	}

	isRead = ReadSecurityRegister(Address, DataLen, (u8 *)ReceiveDataOTP);
	if (isRead == false) {
		printf("Verify OTP Security Register error.\n");
		while (1);
	}
	/*Verify OTP result by comparing Receivedata before OTP with Rcevivedata after OTP*/
	for (int i = 0; i < DataLen; i++) {
		if (ReceiveDataOTP[i] != ReceiveData[i]) {
			printf("OTP verify failed: ReceiveDataOTP[%d]:%02x -> ReceiveData[%d]:%02x\r\n ", i, ReceiveDataOTP[i], i, ReceiveData[i]);
		}
	}
	/*print ReceiveData after OTP*/
	printf("ReceiveDataOTP[] is:\n");
	for (int j = 0; j < DataLen; j++) {
		printf("%02x ", ReceiveDataOTP[j]);
	}
	printf("\n########Check OTP result finished########\n");
#endif

	rtos_task_delete(NULL);
}

int main(void)
{
	if (rtos_task_create(NULL, ((const char *)"flash_test_task"), flash_test_task, NULL, 1024 * 4, 1) != SUCCESS) {
		printf("\n\r%s rtos_task_create(flash_test_task) failed", __FUNCTION__);
	}

	rtos_sched_start();
	while (1) {
		rtos_time_delay_ms(1000);
	}

	return 0;
}

