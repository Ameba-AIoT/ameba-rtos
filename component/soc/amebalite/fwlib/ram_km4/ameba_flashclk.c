/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

#define FLASH_CALIBRATION_DEBUG		0

static const char *const TAG = "FLASH";
static const FlashInfo_TypeDef *current_IC;

/* Flag to check configuration register or not. Necessary for wide-range VCC MXIC flash */
static u8 check_config_reg = 0;

void flash_clksrc_sel(FlashDivInt_E div, u8 isCPUPLL, u8 is_handshake)
{
	if (isCPUPLL == BIT_LSYS_CKSL_SPIC_CPUPLL) {
		/*HW need DSPPLL & CPUPLL open*/
		FLASH_CalibrationPLLSel(FALSE);
		FLASH_Calibration_PSPLL_Open();
		if (is_handshake) {
			/* clock gate for ps_pll */
			FLASH_CalibrationPLLPSCmd(DISABLE);
		}

		/* SPIC clock switch to PLLM */
		FLASH_CalibrationPLLSel(TRUE);
		FLASH_PLLInit_ClockDiv(div);
		if (is_handshake) {
			/* clock gate for ps_pll */
			FLASH_CalibrationPLLPSCmd(DISABLE);
		}
	} else {
		/*HW need DSPPLL & CPUPLL open*/
		FLASH_CalibrationPLLSel(TRUE);
		FLASH_Calibration_PSPLL_Open();
		if (is_handshake) {
			/* clock gate for ps_pll */
			FLASH_CalibrationPLLPSCmd(DISABLE);
		}

		/* SPIC clock switch to PLLD */
		FLASH_CalibrationPLLSel(FALSE);
		FLASH_PLLInit_ClockDiv(div);
		if (is_handshake) {
			/* clock gate for ps_pll */
			FLASH_CalibrationPLLPSCmd(DISABLE);
		}
	}
}

SRAMDRAM_ONLY_TEXT_SECTION
int flash_handshake_highspeed(FlashDivInt_E div)
{
	int Ret = RTK_SUCCESS;
	u8 spic_mode = flash_init_para.FLASH_cur_bitmode;
	u8 Dphy_Dly_Cnt = 3; /* DD recommend this value */

	flash_init_para.debug = FLASH_CALIBRATION_DEBUG;
	flash_clksrc_sel(div, BIT_LSYS_CKSL_SPIC_CPUPLL, TRUE);

	RSIP_MMU_Cache_Clean(); 	/* Clean Cache in RSIP when otf enabled */

	if (FLASH_Read_HandShake(&flash_init_para, spic_mode, Dphy_Dly_Cnt) == TRUE) {
		FLASH_Read_HandShake_Cmd(Dphy_Dly_Cnt, ENABLE);
		flash_init_para.FLASH_rd_sample_dly_cycle_cal = Dphy_Dly_Cnt + 2;
		flash_init_para.FLASH_rd_sample_dly_cycle = flash_init_para.FLASH_rd_sample_dly_cycle_cal;

		/* this code is rom code, so it is safe */
		FLASH_Init(spic_mode);

		RTK_LOGI(TAG, "FLASH HandShake[0x%lx OK]\n", div);
	} else {
		RCC_PeriphClockSource_SPIC(BIT_LSYS_CKSL_SPIC_LBUS);
		RTK_LOGE(TAG, "FLASH HandShake[0x%lx FAIL]\n", div);
		Ret = RTK_FAIL;
	}

#if FLASH_CALIBRATION_DEBUG
	BKUP_Write(BKUP_REG3, Ret);
#endif

	return Ret;
}

static u8 flash_get_option(u32 sys_data, bool is_speed)
{
	u16 tmp = (u16)BIT15;
	u8 cnt = 0;

	while (tmp) {
		if ((sys_data & tmp) != 0) {
			break;
		} else {
			tmp = tmp >> 1;
			cnt++;
		}
	}

	/* cnt =0 --> return FLASH_CLK_DIV10
	 * cnt =1 --> return FLASH_CLK_DIV9
	 * ...
	 * cnt>=8 --> return FLASH_CLK_DIV2
	*/
	if (is_speed) {
		if (cnt > 8) {
			cnt = 8;
		}

		cnt = FLASH_CLK_DIV10 - cnt;
	}

	return cnt;
}

static const FlashInfo_TypeDef *flash_get_chip_info(u32 flash_id)
{
	u32 i = 0;
	u32 temp;
	const FlashInfo_TypeDef *pAVL;
	pAVL = Flash_AVL;

	while (pAVL[i].flash_class != FlashClassNone) {
		temp = flash_id & pAVL[i].id_mask;
		if (pAVL[i].flash_id == temp) {
			return &pAVL[i];
		}

		i++;
	}

	return NULL;
}

static void flash_get_vendor(void)
{
	u8 flash_ID[4];
	u32 flash_capacity = 0;

	/* Read flash ID */
	FLASH_RxCmd(flash_init_para.FLASH_cmd_rd_id, 3, flash_ID);
	/* Byte -> Mbits: 10 + 10 - 3 = 17 (0x11) */
	flash_capacity = (1 << (flash_ID[2] - 0x11));
	RTK_LOGI(TAG, "Flash ID: %x-%x-%x (Capacity: %dM-bit)\n", flash_ID[0], flash_ID[1], flash_ID[2], flash_capacity);

	/* Get flash chip information */
	current_IC = flash_get_chip_info((flash_ID[2] << 16) | (flash_ID[1] << 8) | flash_ID[0]);
	if (current_IC == NULL) {
		RTK_LOGE(TAG, "This flash type is not supported!\n");
		assert_param(0);
	}

	/* Re-initialize flash init structure according to classification */
	switch (current_IC->flash_class) {
	case FlashClass1:
		FLASH_StructInit(&flash_init_para);
		break;
	case FlashClass2:
		FLASH_StructInit_GD(&flash_init_para);
		/* GD flash */
		if (flash_ID[0] == 0xC8) {
			/* 3.3v flash_id[1] = 40h or 1.8v ~ 3.3v flash_id[1] = 65h */
			if ((flash_ID[1] == 0x40) || (flash_ID[1] == 0x65)) {
				/* GD capacity more than 2MB, need 31h cmd to write SR2 */
				if (flash_ID[2] >= 0x16) {
					flash_init_para.FLASH_cmd_wr_status2 = 0x31;
				}
			}
		} else {
			flash_init_para.FLASH_cmd_wr_status2 = 0x31;
		}
		break;
	case FlashClass3:
		FLASH_StructInit_MXIC(&flash_init_para);
		break;
	case FlashClass4:	/* EON without QE bit */
		FLASH_StructInit_MXIC(&flash_init_para);
		flash_init_para.FLASH_QuadEn_bit = 0;
		break;
	case FlashClass5:
		FLASH_StructInit_Micron(&flash_init_para);
		break;
	case FlashClass6:	/* MXIC wide-range VCC chip */
		FLASH_StructInit_MXIC(&flash_init_para);
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
		flash_init_para.FLASH_addr_phase_len = ADDR_4_BYTE;
	}

}

static void flash_set_status_register(void)
{
	u8 StatusLen = 1;
	u32 data = 0;
	u32 status = 0;
	u32 mask = current_IC->sta_mask;

	if (flash_init_para.FLASH_QuadEn_bit != 0) {
		data |= flash_init_para.FLASH_QuadEn_bit;
	}

	/* read status1 register */
	FLASH_RxCmd(flash_init_para.FLASH_cmd_rd_status, 1, (u8 *)&status);

	/* check if status2 exist */
	if (flash_init_para.FLASH_Status2_exist) {
		StatusLen = 2;
		FLASH_RxCmd(flash_init_para.FLASH_cmd_rd_status2, 1, ((u8 *)&status) + 1);

	} else if (check_config_reg) {	/* for MXIC wide-range flash, 1 status register + 2 config register */
		/* Read configuration register */
		FLASH_RxCmd(0x15, 2, ((u8 *)&status) + 1);
		StatusLen = 3;

		/* L/H Switch */
		data |= (BIT(9) << 8);
	}

	status &= mask;
	if (_memcmp((void *)&status, (void *)&data, StatusLen)) {
		if (!flash_init_para.FLASH_cmd_wr_status2) {
			FLASH_SetStatus(flash_init_para.FLASH_cmd_wr_status, StatusLen, (u8 *)&data);
		} else {
			FLASH_SetStatus(flash_init_para.FLASH_cmd_wr_status, 1, (u8 *)&data);
			FLASH_SetStatus(flash_init_para.FLASH_cmd_wr_status2, 1, ((u8 *)&data) + 1);
		}
		RTK_LOGI(TAG, "Flash status register changed:0x%x -> 0x%x\n", status, data);
	}
}

SRAMDRAM_ONLY_TEXT_SECTION
int flash_rx_mode_switch(u8 read_mode)
{
	int Ret = RTK_SUCCESS;
	u8 status = 0, spic_mode = 0, i;
	u32 pdata[2];
	char *str[] = {"1IO", "2O", "2IO", "4O", "4IO"};

	for (i = read_mode; i < 5; i++) {
		/* Try sequentially: 4IO, 4O, 2IO, 2O, 1bit */
		spic_mode = 4 - i;

		/* 4IO or 2IO should enable dummy byte function */
		if ((spic_mode == SpicQuadIOBitMode) || (spic_mode == SpicDualIOBitMode)) {
			flash_init_para.FLASH_dum_en = 1;
		}

		if (flash_init_para.FLASH_Id == FLASH_ID_MICRON) {
			FLASH_RxCmd(0x85, 1, &status);

			status = (status & 0x0f) | (flash_init_para.FLASH_rd_dummy_cycle[spic_mode] << 4);
			FLASH_SetStatus(0x81, 1, &status);
		}

		flash_init_para.FLASH_rd_sample_dly_cycle = SPIC_LOWSPEED_SAMPLE_PHASE;
		FLASH_Init(spic_mode);

		DCache_Invalidate(SPI_FLASH_BASE, 8);
		RSIP_MMU_Cache_Clean();		/* Clean Cache in RSIP when otf enabled */
		pdata[0] = HAL_READ32(SPI_FLASH_BASE, 0x00);
		pdata[1] = HAL_READ32(SPI_FLASH_BASE, 0x04);

		if (_memcmp(pdata, SPIC_CALIB_PATTERN, 8) == 0) {
			RTK_LOGI(TAG, "Flash Read %s\n", str[spic_mode]);
			break;
		} else {
			if (flash_init_para.debug) {
				RTK_LOGI(TAG, "Flash Read %s, FAIL\n", str[spic_mode]);
			}
		}
	}

	if (i == 5) {
		RTK_LOGE(TAG, "Flash Switch Read Mode FAIL\n");
		Ret = RTK_FAIL;
	}

	return Ret;
}

void flash_highspeed_setup(void)
{
	u8 read_mode, flash_speed;
	u8 spic_ckd;
	u32 pllm_clk = PLL_ClkGet(CLK_CPU_MPLL);

	read_mode = flash_get_option(Flash_ReadMode, FALSE);
	flash_speed = flash_get_option(Flash_Speed, TRUE);

	spic_ckd = CLKDIV_ROUND_UP(pllm_clk, SPIC_CLK_LIMIT) - 1;
	flash_speed = MAX(flash_speed, spic_ckd);

	RTK_LOGI(TAG, "SPIC CLK: %d Hz\n", pllm_clk / (flash_speed + 1));


	__disable_irq();

	/* SPIC stay in BUSY state when there are more than 0x1_0000 cycles between two input data.
	 * Disable DREIR to avoid that interrupt hanler time is lager than 0x1_0000 SPIC cycles.
	 */
	SPIC_TypeDef *spi_flash = SPIC;
	spi_flash->CTRLR0 |= BIT_SPI_DREIR_R_DIS;

	/* set tSHSL (CS_H) to min 60ns (12 spic_clk) */
	spi_flash->TPR0 = (spi_flash->TPR0 & ~MASK_CS_H_RD_DUM_LEN) | CS_H_RD_DUM_LEN(12);

	/* if 8 auto rd wrap ever occur, and user rd + auto rd wrap is together, SPIC will return 0xFFFFFFFF only in Lite. */
	/* Set DIS_WRAP_ALIGN = 1, SPIC will chop wrap transation into two incremental transactions to avoid corner case.*/
	spi_flash->CTRLR2 |= BIT_DIS_WRAP_ALIGN;
	/* use uncacheable read (no wrap) to restore to normal function */
	DCache_Invalidate(SPI_FLASH_BASE, 8);
	RSIP_MMU_Cache_Clean(); 	/* Clean Cache in RSIP when otf enabled */
	HAL_READ32(SPI_FLASH_BASE, 0x00);

	/* Get flash ID to reinitialize FLASH_InitTypeDef structure */
	flash_get_vendor();

	/* Set flash status register: set QE, clear protection bits */
	flash_set_status_register();

	/* Set flash I/O mode and high-speed calibration */
	flash_rx_mode_switch(read_mode);

	if (SYSCFG_CHIPType_Get() != CHIP_TYPE_FPGA) {
		flash_handshake_highspeed(flash_speed);
	}

	__enable_irq();
}

