/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *const TAG = "FLASH";
static FlashInfo_TypeDef *current_IC;

/* Flag to check configuration register or not. Necessary for wide-range VCC MXIC flash */
static u8 check_config_reg = 0;

void flash_nor_get_vendor(void)
{
	u8 flash_ID[4];
	u32 flash_capacity = 0;
	FLASH_InitTypeDef *FLASH_InitStruct = &flash_init_para;

	/* Read flash ID */
	FLASH_RxCmd(FLASH_InitStruct->FLASH_cmd_rd_id, 3, flash_ID);
	/* Byte -> Mbits: 10 + 10 - 3 = 17 (0x11) */
	flash_capacity = (1 << (flash_ID[2] - 0x11));
	RTK_LOGI(TAG, "Flash ID: %x-%x-%x (Capacity: %dM-bit)\n", flash_ID[0], flash_ID[1], flash_ID[2], flash_capacity);

	/* Get flash chip information */
	current_IC = flash_get_chip_info((flash_ID[2] << 16) | (flash_ID[1] << 8) | flash_ID[0]);
	if (current_IC == NULL) {
		RTK_LOGW(TAG, "This flash type is not supported!\n");
		assert_param(0);
	}

	/* Re-initialize flash init structure according to classification */
	switch (current_IC->flash_class) {
	case FlashClass1:
		FLASH_StructInit(FLASH_InitStruct);
		break;
	case FlashClass2:
		FLASH_StructInit_GD(FLASH_InitStruct);
		/* GD flash */
		if (flash_ID[0] == 0xC8) {
			/* 3.3v flash_id[1] = 40h or 1.8v ~ 3.3v flash_id[1] = 65h */
			if ((flash_ID[1] == 0x40) || (flash_ID[1] == 0x65)) {
				/* GD capacity more than 2MB, need 31h cmd to write SR2 */
				if (flash_ID[2] >= 0x16) {
					FLASH_InitStruct->FLASH_cmd_wr_status2 = 0x31;
				}
			}
		} else {
			FLASH_InitStruct->FLASH_cmd_wr_status2 = 0x31;
		}
		break;
	case FlashClass3:
		FLASH_StructInit_MXIC(FLASH_InitStruct);
		break;
	case FlashClass4:	/* EON without QE bit */
		FLASH_StructInit_MXIC(FLASH_InitStruct);
		FLASH_InitStruct->FLASH_QuadEn_bit = 0;
		break;
	case FlashClass5:
		FLASH_StructInit_Micron(FLASH_InitStruct);
		break;
	case FlashClass6:	/* MXIC wide-range VCC chip */
		FLASH_StructInit_MXIC(FLASH_InitStruct);
		check_config_reg = 1;
		break;
	case FlashClassUser:
		assert_param(current_IC->FlashInitHandler != NULL);
		current_IC->FlashInitHandler();
		break;
	default:
		break;
	}

	if (SYSCFG_OTP_SPICAddr4ByteEn()) {
		FLASH_InitStruct->FLASH_addr_phase_len = ADDR_4_BYTE;
	}

}

void flash_nor_set_status_register(void)
{
	u8 StatusLen = 1;
	u32 data = 0;
	u32 status = 0;
	u32 mask = current_IC->sta_mask;
	FLASH_InitTypeDef *FLASH_InitStruct = &flash_init_para;

	if (FLASH_InitStruct->FLASH_QuadEn_bit != 0) {
		data |= FLASH_InitStruct->FLASH_QuadEn_bit;
	}

	/* read status1 register */
	FLASH_RxCmd(FLASH_InitStruct->FLASH_cmd_rd_status, 1, (u8 *)&status);

	/* check if status2 exist */
	if (FLASH_InitStruct->FLASH_Status2_exist) {
		StatusLen = 2;
		FLASH_RxCmd(FLASH_InitStruct->FLASH_cmd_rd_status2, 1, ((u8 *)&status) + 1);

	} else if (check_config_reg) {	/* for MXIC wide-range flash, 1 status register + 2 config register */
		/* Read configuration register */
		FLASH_RxCmd(0x15, 2, ((u8 *)&status) + 1);
		StatusLen = 3;

		/* L/H Switch */
		data |= (BIT(9) << 8);
	}

	status &= mask;
	if (_memcmp((void *)&status, (void *)&data, StatusLen)) {
		if (!FLASH_InitStruct->FLASH_cmd_wr_status2) {
			FLASH_SetStatus(FLASH_InitStruct->FLASH_cmd_wr_status, StatusLen, (u8 *)&data);
		} else {
			FLASH_SetStatus(FLASH_InitStruct->FLASH_cmd_wr_status, 1, (u8 *)&data);
			FLASH_SetStatus(FLASH_InitStruct->FLASH_cmd_wr_status2, 1, ((u8 *)&data) + 1);
		}
		RTK_LOGI(TAG, "Flash status register changed:0x%x -> 0x%x\n", status, data);
	}
}

const FlashInfo_TypeDef *flash_nor_get_chip_info(void)
{
	return current_IC;
}

SRAMDRAM_ONLY_TEXT_SECTION
void flash_nor_set_hpm_mode(u32 spic_mode, u32 flash_clk)
{
	u8 status = 0;
	FLASH_InitTypeDef *FLASH_InitStruct = &flash_init_para;

	if (flash_clk > FLASH_HPM_CLK_LIMIT) {
		/* Change SPIC calibration data */
		/* TODO: other bit modes & other flash */
		FLASH_InitStruct->FLASH_rd_dummy_cycle[Spic4IOBitMode] = 0xA;	// 4IO dummy cycle
	}

	if (FLASH_InitStruct->FLASH_Id == FLASH_ID_MICRON) {
		/* Micron keeps the read dummy cycle in a volatile configuration register */
		FLASH_RxCmd(0x85, 1, &status);
		status = (status & 0x0f) | (FLASH_InitStruct->FLASH_rd_dummy_cycle[spic_mode] << 4);
		FLASH_SetStatus(0x81, 1, &status);
	} else if (current_IC->flash_id == 0x85) {	/* PUYA */
		if (flash_clk > FLASH_HPM_CLK_LIMIT) {
			/* set FLASH DC bit in configuration register */
			FLASH_RxCmd(FLASH_CMD_RDCR, 1, &status);
			status |= BIT1;	// DC bit of PY25Q32H
			FLASH_SetStatus(FLASH_CMD_WRCR, 1, &status);
		}
	}
}
