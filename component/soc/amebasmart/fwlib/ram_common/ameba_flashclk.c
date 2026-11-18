/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

#define FLASH_CALIBRATION_DEBUG		0
#define FLASH_CS_HIGH_MIN			12

static const char *const TAG = "FLASH";
static const FlashInfo_TypeDef *current_IC;

/**
  * @brief  Set Flash Clock
  * @param  Div: clock Dividor for SPIC Clock.
  *   This parameter can be any one of the following values:
  *		 @arg FLASH_CLK_DIV4 => SPIC-1/4 nppll
  *		 @arg FLASH_CLK_DIV5 => SPIC-1/5 nppll
  *		 @arg FLASH_CLK_DIV6 => SPIC-1/6 nppll
  *		 @arg FLASH_CLK_DIV7 => SPIC-1/7 nppll
  *		 @arg FLASH_CLK_DIV8 => SPIC-1/8 nppll
  *		 @arg FLASH_CLK_DIV9 => SPIC-1/9 nppll
  *		 @arg FLASH_CLK_DIV10 => SPIC-1/10 nppll
  */
u32 FLASH_ClockDiv(u8 Div)
{
	u32 temp = 0;
	int timeout = 10;
	PLL_TypeDef *PLL = (PLL_TypeDef *)PLL_BASE;

	/* 1. Gate flash clock, dont reset SPIC */
	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_FLASH_CLOCK, DISABLE);

	/* disable clock div */
	temp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL);
	temp &= ~(HSYS_BIT_FLASH_DIV_EN | HSYS_BIT_FLASH_PS_DIV_EN);
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL, temp);

	/* 2. set clock div */
	temp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL);
	temp &= ~(HSYS_MASK_FLASH_DIV_INT);
	temp |= HSYS_FLASH_DIV_INT(Div);
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL, temp);

	/* 3. polling npll_pow_erc: 0x4200_8840h[31] */
	while (timeout > 0) {
		timeout--;
		temp = PLL->PLL_NPPLL_CTRL0;
		if (temp & PLL_BIT_NPLL_POW_ERC) {
			break;
		}
	}

	timeout = 10;
	/* 4. polling npll_pow_pll: 0x4200_8840h[29] */
	while (timeout > 0) {
		timeout--;
		temp = PLL->PLL_NPPLL_CTRL0;
		if (temp & PLL_BIT_NPLL_POW_PLL) {
			break;
		}
	}

	/* 5. enable clock div */
	temp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL);
	temp |= HSYS_BIT_FLASH_DIV_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL, temp);

	/* 6. enable clock ps div */
	temp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL);
	temp |= HSYS_BIT_FLASH_PS_DIV_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL, temp);

	timeout = 10;
	/* 7. wait ready flag of Flash clock with phase shift, 65 ns cost */
	while (timeout > 0) {
		timeout--;
		temp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL);
		temp &= HSYS_BIT_FLASH_DIV_RDY | HSYS_BIT_FLASH_PS_DIV_RDY;
		if (temp == (HSYS_BIT_FLASH_DIV_RDY | HSYS_BIT_FLASH_PS_DIV_RDY)) {
			break;
		}
	}

	/* 8. Release gating of flash clock after BIT_FLASH_DIV_RDY. */
	RCC_PeriphClockCmd(APBPeriph_FLASH, APBPeriph_FLASH_CLOCK, ENABLE);
	RCC_PeriphClockSource_SPIC(BIT_LSYS_CKSL_SPIC_PLL);
	return TRUE;
}

/**
  * @brief  Set New Calibration Clock Phase shift step for calibration
  * @param  phase_idx: get from calibration to set FLASH_PS_PHASE
  */
u32 FLASH_CalibrationPhaseIdx(u8 phase_idx)
{
	u32 temp = 0;
	int timeout = 10;

	/* 1. Gate flash clock */
	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_FLASH_CLOCK, DISABLE);

	/* 2.1. disable clock ps div */
	temp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL);
	temp &= ~HSYS_BIT_FLASH_PS_DIV_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL, temp);

	/* 2.2. disable clock div */
	temp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL);
	temp &= ~HSYS_BIT_FLASH_DIV_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL, temp);

	/* 2.3. wait ready flag disable for Flash clock with phase shift */
	while (timeout > 0) {
		timeout--;
		temp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL);
		temp &= HSYS_BIT_FLASH_DIV_RDY | HSYS_BIT_FLASH_PS_DIV_RDY;
		if (temp == 0) {
			break;
		}
	}

	/* 3 set phase shift clock */
	temp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL);
	temp &= ~HSYS_MASK_FLASH_PS_PHASE; /* set phase shift: 5bits : 1/2 np PLL clock cycles */
	temp |= HSYS_FLASH_PS_PHASE(phase_idx);
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL, temp);

	/* 4.1. enable clock div */
	temp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL);
	temp |= HSYS_BIT_FLASH_DIV_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL, temp);

	/* 4.2. enable clock ps div */
	temp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL);
	temp |= HSYS_BIT_FLASH_PS_DIV_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL, temp);

	/* 5. wait ready flag of Flash clock with phase shift */
	timeout = 10;
	while (timeout > 0) {
		timeout--;
		temp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL);
		temp &= HSYS_BIT_FLASH_DIV_RDY | HSYS_BIT_FLASH_PS_DIV_RDY;
		if (temp == (HSYS_BIT_FLASH_DIV_RDY | HSYS_BIT_FLASH_PS_DIV_RDY)) {
			break;
		}
	}

	/* 6. Release gating of flash clock */
	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_FLASH_CLOCK, ENABLE);

	return TRUE;
}

/**
  * @brief  Set New Calibration Disable/Enable
  * @param  NewStatus: Disable/Enable
  */
SRAMDRAM_ONLY_TEXT_SECTION
u32 FLASH_CalibrationNewCmd(u32 NewStatus)
{
	u32 temp = 0;
	/* set phase calibration enable */
	temp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL);

	if (NewStatus == ENABLE) {
		temp |= HSYS_BIT_FLASH_CAL_EN;
	} else {
		temp &= ~HSYS_BIT_FLASH_CAL_EN;
	}

	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL, temp);

	return TRUE;
}

/**
  * @brief  Flash New calibration function with phase shift
  * @param  FLASH_InitStruct: pointer to a FLASH_InitTypeDef structure that contains
  *         the configuration information for the SPIC Clock.
  * @param  SpicBitMode: This parameter can be any combination of the following values
  *		 @arg SpicOneBitMode
  *		 @arg SpicDualBitMode
  *		 @arg SpicQuadBitMode
  * @param  Div: NPPLL clock Dividor for SPIC Clock.
  *   This parameter can be any combination of the following values:
  *		 @arg FLASH_CLK_DIV4 => SPIC-1/4 nppll
  *		 @arg FLASH_CLK_DIV5 => SPIC-1/5 nppll
  *		 @arg FLASH_CLK_DIV6 => SPIC-1/6 nppll
  *		 @arg FLASH_CLK_DIV7 => SPIC-1/7 nppll
  *		 @arg FLASH_CLK_DIV8 => SPIC-1/8 nppll
  *		 @arg FLASH_CLK_DIV9 => SPIC-1/9 nppll
  *		 @arg FLASH_CLK_DIV10 => SPIC-1/10 nppll
  * @param  CalStep: CalStep * 1/2 np PLL clock cycles.(If np PLL is 1GHz, the step is 0.5ns)
  * @param  LineDelay(13~21ns): include FlashDelay(winbond: 6ns)+LineDelay(1ns)+AmebaDelay(13ns)
  * @param  StartIdx: calibration start index of Flash clock phase shift in units of 1/2 np PLL clock cycles.
  * @retval status value: (window1_size | (window1_start << 16) | (window1_end << 24))
  * @note 100MHz baudrate read 32bit will cost 8(cmd)+6(addr)+6(dummy)+4(data) = 24cycles = 240ns
  * @note step=2 & 100MHz will take 56.5us for a LineDelay calibration
  * @note cache will be disable in this function, you should open cache after this function if needed
  */
u32 FLASH_CalibrationNew(FLASH_InitTypeDef *FLASH_InitStruct, u8 SpicBitMode, u8 Div, u8 CalStep, u8 LineDelay,
						 u8 StartIdx)
{
	SPIC_TypeDef *spi_flash = SPIC;

	u32 pgolden_data[2];
	u8 shift_step = 0;
	u8 phase_shift_end = (Div + 1) * 2 - 1; /* FLASH_PS_PHASE range is [0, (FLASH_DIV_INT+1)*2-1] */

	int window1_start = -1;
	int window1_size = 0;
	int window1_end = -1;

	/* set bit mode  */
	if (SYSCFG_OTP_BootFromNor()) {
		FLASH_SetSpiMode(&flash_init_para, SpicBitMode);
	} else {
		NAND_SetSpiMode(&flash_init_para, SpicBitMode);
	}

	/* set spic register before IC clock close */
	spi_flash->BAUDR = SCKDV(flash_init_para.FLASH_baud_rate);
	spi_flash->AUTO_LENGTH = (spi_flash->AUTO_LENGTH & ~MASK_IN_PHYSICAL_CYC) | IN_PHYSICAL_CYC(LineDelay);

	/* calibration Enable */
	FLASH_CalibrationNewCmd(DISABLE);
	FLASH_CalibrationNewCmd(ENABLE);

	/* first phase sample data */
	shift_step = StartIdx;
	while (1) {
		if (shift_step > phase_shift_end) {
			break;
		}

		FLASH_CalibrationPhaseIdx(shift_step);

		DCache_Invalidate(SPI_FLASH_BASE, 8);
		RSIP_MMU_Cache_Clean();
		pgolden_data[0] = HAL_READ32(SPI_FLASH_BASE, 0x00);
		pgolden_data[1] = HAL_READ32(SPI_FLASH_BASE, 0x04);

		if (FLASH_InitStruct->debug) {
			RTK_LOGD(TAG, "FLASH_CalibrationNew %lx:%lx phase_shift:%x\n", pgolden_data[0], pgolden_data[1], shift_step);
		}

		/* compare data */
		if (_memcmp(pgolden_data, SPIC_CALIB_PATTERN, 8) == 0) {
			if (window1_start < 0) {
				window1_size = 1;
				window1_start = shift_step;
			} else {
				window1_size += CalStep;
			}
			window1_end = shift_step;
		} else {
			if (window1_size > 0) {
				break;
			}
		}

		shift_step = shift_step + CalStep;
	}

	if (FLASH_InitStruct->debug) {
		RTK_LOGD(TAG, "FLASH_CalibrationNew LineDelay:%x\n", LineDelay);
		RTK_LOGD(TAG, "FLASH_CalibrationNew window1_start:%d window1_size:%d \n", window1_start, window1_size);
	}

	if (window1_size > 0) {
		/* get phase_shift_idx */
		FLASH_InitStruct->phase_shift_idx = window1_start + window1_size / 2;

		if (FLASH_InitStruct->debug) {
			RTK_LOGD(TAG, "FLASH_CalibrationNew phase_shift_idx:%x \n", FLASH_InitStruct->phase_shift_idx);
		}
	}

	/* should disable it, enbale it outside if needed */
	FLASH_CalibrationNewCmd(DISABLE);

	/* Recover  */
	spi_flash->BAUDR = SCKDV(flash_init_para.FLASH_baud_boot);

	if (SYSCFG_OTP_BootFromNor()) {
		FLASH_SetSpiMode(&flash_init_para, flash_init_para.FLASH_cur_bitmode);
	} else {
		NAND_SetSpiMode(&flash_init_para, flash_init_para.FLASH_cur_bitmode);
	}

	return (window1_size | (window1_start << 16) | (window1_end << 24));
}

/**
  * @brief  This function is used to calibration for multi-channel communication.
  * @param FLASH_InitStruct: pointer to a FLASH_InitTypeDef
  *         structure that contains the configuration information for SPIC.
  * @param SpicBitMode: the bitmode to be calibrated
  * @param  LineDelay: include FlashDelay(winbond: 6ns)+LineDelay(1ns)+AmebaDelay(13ns)
  * @retval 1: calibration success
  *		0: calibration fail
  */
u32 FLASH_Calibration(FLASH_InitTypeDef *FLASH_InitStruct, u8 SpicBitMode, u8 LineDelay)
{
	SPIC_TypeDef *spi_flash = SPIC;
	u32 rd_data;
	u32 baudr = FLASH_InitStruct->FLASH_baud_rate;
	u32 pass = 0;
	u32 tempBaudr = spi_flash->BAUDR;
	u32 tempAutolen = spi_flash->AUTO_LENGTH;
	u32 sample_phase = 0;
	int window_start = -1;
	int window_size = 0;

	/* in old calibration, we should close new calibration */
	FLASH_CalibrationNewCmd(DISABLE);

	/* set bit mode  */
	if (SYSCFG_OTP_BootFromNor()) {
		FLASH_SetSpiMode(&flash_init_para, SpicBitMode);
	} else {
		NAND_SetSpiMode(&flash_init_para, SpicBitMode);
	}

	/* start calibration */
	spi_flash->BAUDR = SCKDV(baudr);

	/* unit is spic IP clock */
	for (sample_phase = 0; sample_phase < baudr * 2 + LineDelay; sample_phase++) {
		spi_flash->AUTO_LENGTH = (spi_flash->AUTO_LENGTH & ~MASK_IN_PHYSICAL_CYC) | IN_PHYSICAL_CYC(sample_phase);

		DCache_Invalidate(SPI_FLASH_BASE, 8);
		RSIP_MMU_Cache_Clean();
		rd_data = HAL_READ32(SPI_FLASH_BASE,  0);

		if (FLASH_InitStruct->debug) {
			RTK_LOGD(TAG, "FLASH_Calibration %lx sample_phase:%lx rate:%lx\n", rd_data, sample_phase, baudr);
		}

		/* compare data */
		if (rd_data == SPIC_CALIB_PATTERN[0]) {
			window_size++;
			if (window_start < 0) {
				window_start = sample_phase;
			}
			pass = 1;
		}
	}
	/* stop calibration */

	if (window_size > 0) {
		sample_phase = window_start + window_size / 2;
		FLASH_InitStruct->FLASH_rd_sample_phase = sample_phase;

		if (FLASH_InitStruct->debug) {
			RTK_LOGD(TAG, "FLASH_Calibration sample_phase:%lx rate:%lx\n", sample_phase, baudr);
		}
	}

	/* Recover SPI mode */
	if (SYSCFG_OTP_BootFromNor()) {
		FLASH_SetSpiMode(&flash_init_para, flash_init_para.FLASH_cur_bitmode);
	} else {
		NAND_SetSpiMode(&flash_init_para, flash_init_para.FLASH_cur_bitmode);
	}

	/* restore baudr & AUTO_LENGTH */
	spi_flash->BAUDR = tempBaudr ;
	spi_flash->AUTO_LENGTH = tempAutolen;

	if (pass) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/**
  * @brief  Open PLL Power
  */
SRAMDRAM_ONLY_TEXT_SECTION
void FLASH_Calibration_PLL_Open(void)
{
	// u32 temp = 0;
	// PLL_TypeDef *PLL = (PLL_TypeDef *)PLL_BASE;

	// /* 3. clear PLL_PS[14], then set to 1. Should delay 10 us before enable clock div */
	// temp = PLL->PLL_PS;
	// temp &= ~PLL_BIT_POW_CKGEN;
	// PLL->PLL_PS = temp;

	// temp |= PLL_BIT_POW_CKGEN;
	// PLL->PLL_PS = temp;
	// DelayUs(10);

	// /* 4. Enable CK_400M and CK_400M_PS for divider */
	// PLL->PLL_PS |= PLL_BIT_EN_CK600M | PLL_BIT_EN_CK600M_PS;
}
/**
  * @brief  Close PLL Power
  * @param  none.
  */
SRAMDRAM_ONLY_TEXT_SECTION
void FLASH_Calibration_PLL_Close(void)
{
	// PLL_TypeDef *PLL = (PLL_TypeDef *)PLL_BASE;

	// /* set PLL 600M phase shift enable */
	// PLL->PLL_PS &= ~(PLL_BIT_EN_CK600M | PLL_BIT_EN_CK600M_PS);

	// /* 2. clear PLL_PS[14] to dislabe PLL power */
	// PLL->PLL_PS &= ~PLL_BIT_POW_CKGEN;
}

/**
  * @brief    Configure SPIC IP Clock.
  * @param  Source:  This parameter can be one of the following values:
  *                            @arg BIT_LSYS_CKSL_SPIC_XTAL
  *                            @arg BIT_LSYS_CKSL_SPIC_PLL
  * @param  Protection:  if disable interrupt when switch clock:
  * @retval   None
  */
SRAMDRAM_ONLY_TEXT_SECTION
void FLASH_ClockSwitch(u32 Source, u32 Protection)
{
	FLASH_InitTypeDef *FLASH_InitStruct = &flash_init_para;
	u32 Temp, PreState_irq;

	if (Protection) {
		PreState_irq = irq_disable_save();
	}

	/* sequence should be followed strickly */
	if (Source == BIT_LSYS_CKSL_SPIC_XTAL) {
		/* 1. clock source switch to XTAL */
		RCC_PeriphClockSource_SPIC(BIT_LSYS_CKSL_SPIC_XTAL);

		/* 2. close 400M & 400MPS */
		Temp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL);
		Temp &= ~(HSYS_BIT_FLASH_DIV_EN | HSYS_BIT_FLASH_PS_DIV_EN); /* disable clock ps div & disable clock div*/
		HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL, Temp);

		FLASH_CalibrationNewCmd(DISABLE);

		FLASH_Calibration_PLL_Close();

		/* 3. SPIC Dummy to low speed dummy */
		FLASH_InitStruct->FLASH_rd_sample_phase = SPIC_LOWSPEED_SAMPLE_PHASE;
		SPIC->AUTO_LENGTH = (SPIC->AUTO_LENGTH & ~MASK_IN_PHYSICAL_CYC) | IN_PHYSICAL_CYC(FLASH_InitStruct->FLASH_rd_sample_phase);
	} else {
		/* 1. enable 600M & 600MPS */
		FLASH_Calibration_PLL_Open();

		// Temp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL);
		// Temp &= ~HSYS_MASK_FLASH_PS_PHASE; /* set phase shift: 5bits : 1/2 np PLL clock cycles */
		// Temp |= HSYS_FLASH_PS_PHASE(flash_init_para.phase_shift_idx);
		// HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL, Temp);

		Temp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL);
		Temp |= (HSYS_BIT_FLASH_DIV_EN | HSYS_BIT_FLASH_PS_DIV_EN); /* enable clock ps div & enable clock div*/
		HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL, Temp);

		/* wait clock ready timing need check */
		while (1) {
			Temp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SPIC_CTRL);
			if ((Temp & HSYS_BIT_FLASH_PS_DIV_RDY) && (Temp & HSYS_BIT_FLASH_DIV_RDY)) {
				break;
			}
		}

		FLASH_CalibrationNewCmd(ENABLE);

		/* 2. clock source switch */
		RCC_PeriphClockSource_SPIC(BIT_LSYS_CKSL_SPIC_PLL);

		/* 3. SPIC Dummy to high speed dummy */
		FLASH_InitStruct->FLASH_rd_sample_phase = FLASH_InitStruct->FLASH_rd_sample_phase_cal;
		SPIC->AUTO_LENGTH = (SPIC->AUTO_LENGTH & ~MASK_IN_PHYSICAL_CYC) | IN_PHYSICAL_CYC(FLASH_InitStruct->FLASH_rd_sample_phase);
		// FLASH_SetSpiMode(&flash_init_para, flash_init_para.FLASH_cur_bitmode);
	}

	if (Protection) {
		irq_enable_restore(PreState_irq);
	}
}

bool _flash_calibration_highspeed(u8 SpicBitMode, u8 div)
{
	u32 line_delay_temp = 0;
	u32 window_temp = 0;
	u32 window_size_temp = 0;

	u32 window_start_idx = 0;
	u32 window_end_idx = 0;
	u32 window_middle_idx = 0;
	u32 final_phase_idx = 0;
	u32 final_shift_idx = 0;

	/* 4.808 ms take when enter this function */
	/* 511us will take for flash full calibration */
	for (line_delay_temp = 0; line_delay_temp <= 4; line_delay_temp++) {
		window_temp = FLASH_CalibrationNew(&flash_init_para, SpicBitMode, div, 1, line_delay_temp, 1);
		window_size_temp = (window_temp & 0xFF);

		if (flash_init_para.phase_shift_idx != 0) {
			if (window_size_temp > 2) {
				flash_init_para.phase_shift_idx |= BIT(0);    /* odd is better */
			}
		}

#if FLASH_CALIBRATION_DEBUG
		u32 window_start_temp = ((window_temp >> 16) & 0xFF);
		u32 window_end_temp = ((window_temp >> 24) & 0xFF);

		RTK_LOGD(TAG, "calibration_result:[%lu:%lu:%lu][%lx:%lx] \n", line_delay_temp, window_size_temp, flash_init_para.phase_shift_idx,
				 window_start_temp, window_end_temp);
		BKUP_Write(BKUP_REG3, ((window_start_temp << 24) | (line_delay_temp << 16) | (window_size_temp << 8) | flash_init_para.phase_shift_idx));
#endif

		if (window_size_temp != 0) {
			if (window_start_idx == 0) {
				window_start_idx = line_delay_temp * 10 + ((window_temp >> 16) & 0xFF);
			}
			window_end_idx = line_delay_temp * 10 + ((window_temp >> 24) & 0xFF);
		}
		flash_init_para.phase_shift_idx = 0;
	}

	if (window_start_idx > 0) {
		window_middle_idx = (window_start_idx + window_end_idx) / 2;
		if ((window_middle_idx % 10) == 0) {
			window_middle_idx += 1;
		}
		final_phase_idx = window_middle_idx / 10;
		final_shift_idx = window_middle_idx % 10;

		RTK_LOGI(TAG, "start_idx:%d, end_idx:%d, middle_idx:%d, final_phase_idx:%d, final_shift_idx:%d \n",
				 window_start_idx, window_end_idx, window_middle_idx, final_phase_idx, final_shift_idx);
		flash_init_para.phase_shift_idx = final_shift_idx;
		flash_init_para.FLASH_rd_sample_phase_cal = final_phase_idx;
		flash_init_para.FLASH_rd_sample_phase = flash_init_para.FLASH_rd_sample_phase_cal;

		return TRUE;
	} else {
		flash_init_para.phase_shift_idx = 0;
		flash_init_para.FLASH_rd_sample_phase = SPIC_LOWSPEED_SAMPLE_PHASE;
	}

	return FALSE;
}

/**
  * @brief  Set the SPIC CS timing for the given SPIC bus clock from the NOR flash CS timing.
  * @param  bus_clk: the SPIC bus clock in Hz it is going to run at. TPR0 counts bus_clk cycles,
  *                  spi_sclk = bus_clk / (2 * BAUDR).
  * @note   Must be set before switching to that clock. Values beyond the register field
  *         are clamped: hold 0~15, CS high 12~63 (never looser than the original driver).
  * @retval None
  */
static void flash_cs_timing_config(u32 bus_clk)
{
	u32 tcss_ns, tchsh_ns, tshsl_ns;
	u32 cs_hold, cs_high;

	flash_nor_get_cs_timing(&tcss_ns, &tchsh_ns, &tshsl_ns);

	cs_hold = MIN(SPIC_CS_HOLD_CYCLES(tchsh_ns, bus_clk, flash_init_para.FLASH_baud_rate), GET_CS_ACTIVE_HOLD(MASK_CS_ACTIVE_HOLD));
	cs_high = MIN(MAX(SPIC_CS_HIGH_CYCLES(tshsl_ns, bus_clk), FLASH_CS_HIGH_MIN), GET_CS_H_RD_DUM_LEN(MASK_CS_H_RD_DUM_LEN));

	SPIC->TPR0 = (SPIC->TPR0 & ~(MASK_CS_ACTIVE_HOLD | MASK_CS_H_WR_DUM_LEN | MASK_CS_H_RD_DUM_LEN)) |
				 CS_ACTIVE_HOLD(cs_hold) | CS_H_WR_DUM_LEN(cs_high) | CS_H_RD_DUM_LEN(cs_high);
}

int flash_calibration_highspeed(u8 div, u32 flash_clk)
{
	int Ret = RTK_SUCCESS;
	u8 spic_mode = flash_init_para.FLASH_cur_bitmode;

	flash_init_para.debug = FLASH_CALIBRATION_DEBUG;

	if (SYSCFG_OTP_BootFromNor()) {
		/* The flash side of HPM has to be in place before the clock is raised. If it
		 * cannot be set the flash stays on the boot clock. */
		if (flash_nor_set_dc(flash_clk) != RTK_SUCCESS) {
			RTK_LOGE(TAG, "Flash HPM set fail, use XTAL\n");
			return RTK_FAIL;
		}

		/* CS timing for the PLL clock, in place before the clock is switched */
		flash_cs_timing_config(flash_clk * 2 * flash_init_para.FLASH_baud_rate);
	}

	/* SPIC clock switch to PLL */
	FLASH_ClockDiv(div);

	if (_flash_calibration_highspeed(spic_mode, div) == TRUE) {
		/* we should open calibration new first, and then set phase index */
		FLASH_CalibrationNewCmd(ENABLE);
		FLASH_CalibrationPhaseIdx(flash_init_para.phase_shift_idx);

		/* this code is rom code, so it is safe */
		if (SYSCFG_OTP_BootFromNor()) {
			FLASH_Init(spic_mode);
		} else {
			NAND_Init(spic_mode);
		}

		RTK_LOGI(TAG, "FLASH CALIB[0x%x OK]\n", div);
	} else {
		/* calibration fail, revert SPIC clock to XTAL */
		RCC_PeriphClockSource_SPIC(BIT_LSYS_CKSL_SPIC_XTAL);

		RTK_LOGE(TAG, "FLASH CALIB[0x%x FAIL, use XTAL]\n", div);

		Ret = RTK_FAIL;
	}


	return Ret;
}

static u8 flash_get_option(u32 sys_data, bool is_speed)
{
	u16 tmp = 0x8000;
	u8 cnt = 0;
	while (tmp) {
		if ((sys_data & tmp) != 0) {
			break;
		} else {
			tmp = tmp >> 1;
			cnt++;
		}
	}

	if (is_speed) {
		if (cnt > 6) {
			cnt = 6;
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
	if (SYSCFG_OTP_BootFromNor()) {
		pAVL = Flash_AVL;
	} else {
		pAVL = NAND_AVL;
	}

	while (pAVL[i].flash_class != FlashClassNone) {
		temp = flash_id & pAVL[i].id_mask;
		if (pAVL[i].flash_id == temp) {
			return &pAVL[i];
		}

		i++;
	}

	return NULL;
}

static void nand_get_vendor(void)
{
	u8 flash_ID[4];

	/* Read flash ID */
	NAND_RxCmd_Safe(flash_init_para.FLASH_cmd_rd_id, 0, NULL, 2, flash_ID);
	RTK_LOGI(TAG, "NAND ID: %x-%x\n", flash_ID[0], flash_ID[1]);

	/* Get flash chip information */
	current_IC = flash_get_chip_info((flash_ID[2] << 16) | (flash_ID[1] << 8) | flash_ID[0]);
	if (current_IC == NULL) {
		RTK_LOGW(TAG, "This flash type is not supported!\n");
		assert_param(0);
	}

	/* Re-initialize flash init structure according to classification */
	switch (current_IC->flash_class) {
	case FlashClass1:
		NAND_StructInit(&flash_init_para);
		break;
	case FlashClass2:
		NAND_StructInit_GD(&flash_init_para);
		break;
	case FlashClass3:
		NAND_StructInit_MXIC(&flash_init_para);
		break;
	case FlashClass5:
		NAND_StructInit_Micron(&flash_init_para);
		break;
	case FlashClassUser:
		assert_param(current_IC->FlashInitHandler != NULL);
		current_IC->FlashInitHandler();
		break;
	default:
		break;
	}
}

/**
  * @brief  Switch the flash read bitmode, falling back until the data reads back correctly.
  * @param  spic_mode: the bitmode to try first, degraded down to SpicOneBitMode on failure.
  * @param  flash_clk: the flash clock in Hz spic_mode will run at, passed on to flash_nor_set_hpm_mode().
  * @retval RTK_SUCCESS if the flash reads back in spic_mode, RTK_FAIL otherwise
  */
int flash_rx_mode_switch(u32 spic_mode, u32 flash_clk)
{
	u32 req_mode = spic_mode;
	u32 pdata[2];
	char *str[] = {"1IO", "2O", "2IO", "4O", "4IO"};

	/* Try sequentially: 4IO, 4O, 2IO, 2O, 1bit */
	do {
		if (SYSCFG_OTP_BootFromNor()) {
			/* 4IO or 2IO should enable dummy byte function */
			if ((spic_mode == SpicQuadIOBitMode) || (spic_mode == SpicDualIOBitMode)) {
				flash_init_para.FLASH_dum_en = 1;
			}

			/* Apply the vendor setting this bitmode & clock needs before probing it */
			flash_nor_set_hpm_mode(spic_mode, flash_clk);

			flash_init_para.FLASH_rd_sample_phase = SPIC_LOWSPEED_SAMPLE_PHASE;
			FLASH_Init(spic_mode);
		} else {
			flash_init_para.FLASH_rd_sample_phase = SPIC_LOWSPEED_SAMPLE_PHASE;
			NAND_Init(spic_mode);
		}

		DCache_Invalidate(SPI_FLASH_BASE, 8);
		RSIP_MMU_Cache_Clean();
		pdata[0] = HAL_READ32(SPI_FLASH_BASE, 0x00);
		pdata[1] = HAL_READ32(SPI_FLASH_BASE, 0x04);

		if (_memcmp(pdata, SPIC_CALIB_PATTERN, 8) == 0) {
			break;
		}
	} while (spic_mode--);

	if (spic_mode != req_mode) {
		RTK_LOGE(TAG, "Flash Read %s not reached\n", str[req_mode]);
		return RTK_FAIL;
	}

	RTK_LOGI(TAG, "Flash Read %s\n", str[spic_mode]);
	return RTK_SUCCESS;
}

/**
  * @brief  Set the flash read mode and clock.
  * @param  read_mode: the read bitmode to set.
  * @param  spic_max: the SPIC bus clock upper limit in Hz. The fastest PLL divider not above it is used.
  * @note   Fail leaves the flash readable (fallback bitmode and/or boot clock), the request is not met.
  * @retval RTK_SUCCESS if the flash is in read_mode and calibrated at the selected clock, RTK_FAIL otherwise
  */
int flash_highspeed_setup(u32 read_mode, u32 spic_max)
{
	int Ret;
	u8 flash_speed;
	u8 nand_reg;
	u8 spic_ckd;
	u32 pll_clk = PLL_NP_ClkGet();
	/* Boot clock first: the flash runs at XTAL / 2 until the PLL clock is taken */
	u32 flash_clk = XTAL_ClkGet() / 2;

	flash_speed = flash_get_option(Flash_Speed, TRUE);

	spic_ckd = DIV_ROUND_UP(pll_clk, spic_max) - 1;
	flash_speed = MAX(flash_speed, spic_ckd);

	__asm volatile("cpsid i");

	/* SPIC stay in BUSY state when there are more than 0x1_0000 cycles between two input data.
	 * Disable DREIR to avoid that interrupt hanler time is lager than 0x1_0000 SPIC cycles.
	 */
	SPIC_TypeDef *spi_flash = SPIC;
	spi_flash->CTRLR0 |= BIT_SPI_DREIR_R_DIS;

	/* set tSHSL (CS_H) to min 60ns (12 spic_clk) */
	spi_flash->TPR0 = (spi_flash->TPR0 & ~MASK_CS_H_RD_DUM_LEN) | CS_H_RD_DUM_LEN(12);

	if (SYSCFG_OTP_BootFromNor()) {
		/* Get flash ID to reinitialize FLASH_InitTypeDef structure */
		flash_nor_get_vendor();

		/* Set flash status register: set QE, the read mode below needs it */
		flash_nor_set_qe_register();
	} else {
		/* Get Nand ID and Set QE */
		nand_get_vendor();
		/* QuadEnable */
		if (flash_init_para.FLASH_QuadEn_bit != 0) {
			nand_reg = NAND_GetStatus(NAND_REG_CFG);
			if (nand_reg != (nand_reg | NAND_CFG_QUAD_ENABLE)) {
				NAND_SetStatus(NAND_REG_CFG, nand_reg | NAND_CFG_QUAD_ENABLE);
			}
		}

		/*After power-up, the chip is in protection state, set feature bits BPx to 0 to unlock all block. */
		nand_reg = NAND_GetStatus(NAND_REG_BLOCK_LOCK);
		if (nand_reg != NAND_BL_ALL_UNLOCKED) {
			NAND_SetStatus(NAND_REG_BLOCK_LOCK, NAND_BL_ALL_UNLOCKED);
			RTK_LOGI(TAG, "Flash all block unlocked. Feature reg 0x%02x set to 0x%02x\n", NAND_REG_BLOCK_LOCK, NAND_BL_ALL_UNLOCKED);
		}
	}

	/* Set flash I/O mode and high-speed calibration */
	Ret = flash_rx_mode_switch(read_mode, flash_clk);

	if (SYSCFG_CHIPType_Get() == CHIP_TYPE_FPGA) {
		goto exit;
	}

	flash_clk = pll_clk / (2 * (flash_speed + 1));
	RTK_LOGI(TAG, "FLASH CLK: %d Hz\n", flash_clk);

	/* The flash cannot run at that clock even with its DC bit set: stay on XTAL */
	if (SYSCFG_OTP_BootFromNor() && flash_nor_check_clk(flash_clk) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Flash out of spec at %d Hz, use XTAL\n", flash_clk);
		Ret = RTK_FAIL;
		goto exit;
	}

	if (flash_calibration_highspeed(flash_speed, flash_clk) != RTK_SUCCESS) {
		Ret = RTK_FAIL;
	}

exit:
	__asm volatile("cpsie i");
	return Ret;
}
