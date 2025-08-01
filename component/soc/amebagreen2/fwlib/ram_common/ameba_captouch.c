/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *const TAG = "CAPTOUCH";
extern bool vref_init_done;

u8 mbias_init_done = FALSE;

struct MbiasTypeDef Mbias_Config[16] = {
	/* current offset start  end*/
	{64000,     0x0,   0x0,   0xA},
	{32000,     0x2,   0x0,   0x9},
	{32000,     0x4,   0x0,   0x9},
	{16000,     0x6,   0x0,   0x8},
	{8000,      0x8,   0x0,   0x7},
	{8000,      0x8,   0x0,   0x7},
	{4000,      0xA,   0x0,   0x6},
	{2000,      0x2,   0xA,   0xF},
	{2000,      0x2,   0xA,   0xF},
	{1000,      0x4,   0xA,   0xF},
	{500,       0x0,   0xB,   0xF},
	{500,       0x0,   0xB,   0xF},
	{250,       0x6,   0x9,   0xB},
	{250,       0x6,   0x9,   0xB},
	{250,       0x6,   0x9,   0xB},
	{250,       0x6,   0x9,   0xB}
};

/** @addtogroup Ameba_Periph_Driver
 * @{
 */

/** @defgroup CAPTOUCH
 * @brief CAPTOUCH driver modules
 * @{
 */

/* Exported functions --------------------------------------------------------*/
/** @defgroup CAPTOUCH_Exported_Functions CAPTOUCH Exported Functions
 * @{
 */

/**
 * @brief Fill each CapTouch_InitStruct member with its default value.
 * @param CapTouch_InitStruct Pointer to a CapTouch_InitTypeDef structure which will be initialized.
 * @return None
 */
void CapTouch_StructInit(CapTouch_InitTypeDef *CapTouch_InitStruct)
{
	u8 i;
	/* Load HAL initial data structure default value */
	CapTouch_InitStruct->CT_DebounceEn = 1;
	CapTouch_InitStruct->CT_SampleCnt = 6; // 2^8 points
	CapTouch_InitStruct->CT_ScanInterval = 60;
	CapTouch_InitStruct->CT_SamplePeriodUs = 6;
	CapTouch_InitStruct->CT_ETCStep = 1;
	CapTouch_InitStruct->CT_ETCFactor = 4;
	CapTouch_InitStruct->CT_ETCScanInterval = 3;
	CapTouch_InitStruct->CT_MedCoef = CTC_MED_3;
	CapTouch_InitStruct->CT_IIRCoefNorm = 5;
	CapTouch_InitStruct->CT_IIRCoefAccel = 3;

	for (i = 0; i < CT_CHANNEL_NUM; i++) {
		CapTouch_InitStruct->CT_Channel[i].CT_CHEnable = DISABLE;
		CapTouch_InitStruct->CT_Channel[i].CT_DiffThrehold = 0xf0;
		CapTouch_InitStruct->CT_Channel[i].CT_MbiasCurrent = 0x200; // 4uA
		CapTouch_InitStruct->CT_Channel[i].CT_ETCNNoiseThr = 100;
		CapTouch_InitStruct->CT_Channel[i].CT_ETCPNoiseThr = 100;
	}
}

/**
 * @brief Initialize the CapTouch peripheral according to the specified parameters in the CapTouch_InitStruct.
 * @param CapTouch CAPTOUCH_DEV.
 * @param CapTouch_InitStruct Pointer to a CapTouch_InitTypeDef structure that contains
 * 		the configuration information of the specified CapTouch peripheral.
 * @return None
 */
void CapTouch_Init(CAPTOUCH_TypeDef *CapTouch, CapTouch_InitTypeDef *CapTouch_InitStruct)
{
	u32 i;

#ifdef TODO
	u32 value;
	u8 vref_sel = 0x7;
#endif

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(CapTouch_InitStruct->CT_DebounceEn <= 1);
	assert_param(CapTouch_InitStruct->CT_SampleCnt <= 7);
	assert_param(CapTouch_InitStruct->CT_ScanInterval <= 0xFFF);
	assert_param(CapTouch_InitStruct->CT_SamplePeriodUs <= 0x80);
	assert_param(CapTouch_InitStruct->CT_ETCStep <= 0xF);
	assert_param(CapTouch_InitStruct->CT_ETCFactor <= 0xF);
	assert_param(CapTouch_InitStruct->CT_ETCScanInterval <= 0x7F);

	for (i = 0; i < CT_CHANNEL_NUM; i++) {
		assert_param(CapTouch_InitStruct->CT_Channel[i].CT_DiffThrehold  <= 0xFFF);
		assert_param(CapTouch_InitStruct->CT_Channel[i].CT_ETCNNoiseThr <= 0xFFF);
		assert_param(CapTouch_InitStruct->CT_Channel[i].CT_ETCPNoiseThr <= 0xFFF);
	}

	/* Disable CTC, clear pending interrupt*/
	CapTouch_Cmd(CapTouch, DISABLE);

#ifdef TODO
	/* Calibrate Vref according to EFuse */
	if (!vref_init_done) {
		/* step1: read EFuse */
		OTP_Read8(VREF_SEL_ADDR, &vref_sel);
		/* [2:0]: Vref Selection */
		vref_sel &= 0x7;

		/* step2: update vref sel para */
		value = PLL_BASE->PLL_LPAD0;
		value &= ~PLL_MASK_VREF_SEL;
		if (vref_sel == 0x3) {
			value |= PLL_VREF_SEL(0x3); // 011:0.875V
		} else {
			value |= PLL_VREF_SEL(0x2); // 010:0.85V
		}
		PLL_BASE->PLL_LPAD0 = value;

		vref_init_done = TRUE;
	}
#endif

	/* set CTC clock */
	CapTouch->CT_CTC_DIV_HCNT = CT_CTC_CLK_HCNT(CapTouch_InitStruct->CT_SamplePeriodUs * 0x4 - 0x4);
	CapTouch->CT_CTC_DIV_LCNT = CT_CTC_CLK_LCNT0(0x2) | CT_CTC_CLK_LCNT1(0x3) | CT_CTC_CLK_LCNT2(0x4);

	/* Disable and clear all the interrupt */
	CapTouch->CT_INTERRUPT_ENABLE = 0;
	CapTouch->CT_INTERRUPT_ALL_CLR = CT_BIT_INTR_ALL_CLR;

	/* Set control register*/
	CapTouch->CT_CTC_CTRL = CT_BIT_BASELINE_INI;
	if (CapTouch_InitStruct->CT_DebounceEn == 1) {
		CapTouch->CT_CTC_CTRL |= CT_BIT_DEBOUNCE_EN;
	} else {
		CapTouch->CT_CTC_CTRL &= ~CT_BIT_DEBOUNCE_EN;
	}

	CapTouch->CT_SCAN_PERIOD = CT_SAMPLE_AVE_CTRL(CapTouch_InitStruct->CT_SampleCnt) | \
							   CapTouch_InitStruct->CT_ScanInterval;

	CapTouch->CT_ETC_CTRL = CT_BASELINE_UPD_STEP(CapTouch_InitStruct->CT_ETCStep) | \
							CT_BASELINE_WT_FACTOR(CapTouch_InitStruct->CT_ETCFactor) | \
							CT_ETC_SCAN_INTERVAL(CapTouch_InitStruct->CT_ETCScanInterval) | \
							CT_BIT_ETC_FUNC_CTRL;

	CapTouch->CT_DEBUG_MODE_CTRL = CT_BIT_CH_SWITCH_CTRL;

	/* Enable diode protect */
	CapTouch->CT_CTC_ANALOG |= CT_BIT_REG_DIODE;

	/* Configure filter parameters and enable filter */
	CapTouch_SetMedCoef(CapTouch, CapTouch_InitStruct->CT_MedCoef);
	CapTouch_FilterIIRConfig(CapTouch, ENABLE, CapTouch_InitStruct->CT_IIRCoefNorm, CapTouch_InitStruct->CT_IIRCoefAccel);
	CapTouch_SetMDec(CapTouch, 0x0);
	CapTouch_FilterIIRCmd(CapTouch, ENABLE);
	CapTouch_FilterCmd(CapTouch, ENABLE);
	CapTouch_AveMedConfig(CapTouch, 0x3, 0x1); // LCNT0/LCNT1/LCN2: 2/3/4

	/* Configure each channel */
	for (i = 0; i < CT_CHANNEL_NUM; i++) {
		if (CapTouch_InitStruct->CT_Channel[i].CT_CHEnable) {
			CapTouch->CT_CH[i].CT_CHx_CTRL = CT_CHx_D_TOUCH_TH(CapTouch_InitStruct->CT_Channel[i].CT_DiffThrehold) | CT_BIT_CHx_EN;
			CapTouch->CT_CH[i].CT_CHx_MBIAS_ATH = CT_CHx_MBIAS(CapTouch_InitStruct->CT_Channel[i].CT_MbiasCurrent);
			CapTouch->CT_CH[i].CT_CHx_NOISE_TH = CT_CHx_N_ENT(CapTouch_InitStruct->CT_Channel[i].CT_ETCNNoiseThr) | \
												 CT_CHx_P_ENT(CapTouch_InitStruct->CT_Channel[i].CT_ETCPNoiseThr);
		}
	}
}

/**
 * @brief Enable or disable CapTouch peripheral.
 * @param CapTouch CAPTOUCH_DEV.
 * @param NewState New state of the CapTouch peripheral.
 * 		This parameter can be ENABLE or DISABLE.
 * @return None
 */
void CapTouch_Cmd(CAPTOUCH_TypeDef *CapTouch, u8 NewState)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));

	if (NewState != DISABLE) {
		/* Enable the CapTouch peripheral and baseline */
		CapTouch->CT_CTC_CTRL |= CT_BIT_ENABLE | CT_BIT_BASELINE_INI;
	} else {
		/* Disable the CapTouch peripheral */
		CapTouch->CT_CTC_CTRL &= ~CT_BIT_ENABLE;
	}
}

/**
 * @brief Enable or disable specified CapTouch interrupt(s).
 * @param CapTouch CAPTOUCH_DEV.
 * @param CapTouch_IT Specified CapTouch interrupt(s) to be enabled or disabled.
 * 		This parameter can be one or combinations of the following values:
 * 		@arg CT_BIT_SCAN_END_INTR_EN: Sample scan finish raw interrupt.
 * 		@arg CT_BIT_CTC_BRK_BY_ADC_INTR_EN: Captouch breaked by adc interrupt.
 * 		@arg CT_BIT_AFIFO_OVERLVL_INTR_EN: Raw code FIFO over level interrupt.
 * 		@arg CT_BIT_OVER_N_NOISE_TH_INTR_EN: CapTouch negetive noise overflow interrupt.
 * 		@arg CT_BIT_AFIFO_OVERFLOW_INTR_EN: CapTouch FIFO overflow interrupt.
 * 		@arg CT_BIT_OVER_P_NOISE_TH_INTR_EN:CapTouch positive noise overflow interrupt.
 * 		@arg CT_CHX_PRESS_INT(x): CapTouch channel(x) press interrupt, where x can be 0~9.
 * 		@arg CT_CHX_RELEASE_INT(x): CapTouch channel(x) release interrupt, where x can be 0~9.
 * @param NewState New state of the specified CapTouch interrupt(s).
 * 		This parameter can be ENABLE or DISABLE.
 * @return None
 */
void CapTouch_INTConfig(CAPTOUCH_TypeDef *CapTouch, uint32_t CapTouch_IT, u8 newState)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_INT_EN(CapTouch_IT));

	if (newState == ENABLE) {
		/* Enable CapTouch interrupts */
		CapTouch->CT_INTERRUPT_ENABLE |= CapTouch_IT;
	} else {
		/* Disable CapTouch interrupts */
		CapTouch->CT_INTERRUPT_ENABLE &= (~CapTouch_IT);
	}
}

/**
 * @brief Clear specified CapTouch interrupt pending bit(s).
 * @param CapTouch CAPTOUCH_DEV.
 * @param CapTouch_IT Specified CapTouch interrupt bit(s) to be cleared.
 * 		This parameter can be one or combinations of the following values:
 * 		@arg CT_BIT_SCAN_END_CLR: Sample scan finish raw interrupt.
 * 		@arg CT_BIT_CTC_BRK_BY_ADC_CLR: Captouch breaked by adc interrupt.
 * 		@arg CT_BIT_AFIFO_OVERLVL_CLR: Raw code FIFO over level interrupt.
 * 		@arg CT_BIT_OVER_N_NOISE_TH_CLR: CapTouch negetive noise overflow interrupt.
 * 		@arg CT_BIT_AFIFO_OVERFLOW_CLR: CapTouch FIFO overflow interrupt.
 * 		@arg CT_BIT_OVER_P_NOISE_TH_CLR:CapTouch positive noise overflow interrupt.
 * 		@arg CT_CHX_PRESS_INT(x): CapTouch channel(x) press interrupt, where x can be 0~9.
 * 		@arg CT_CHX_RELEASE_INT(x): CapTouch channel(x) release interrupt, where x can be 0~9.
 * @return None
 */
void CapTouch_INTClearPendingBit(CAPTOUCH_TypeDef *CapTouch, u32 CapTouch_IT)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_INT_CLR(CapTouch_IT));

	CapTouch->CT_INTERRUPT_STATUS_CLR |= CapTouch_IT;
}

/**
 * @brief Get CapTouch raw interrupt status.
 * @param CapTouch CAPTOUCH_DEV.
 * @return Raw interrupt status.
 */
u32 CapTouch_GetRawISR(CAPTOUCH_TypeDef *CapTouch)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));

	return CapTouch->CT_RAW_INTERRUPT_STATUS;
}

/**
 * @brief Get CapTouch interrupt status.
 * @param CapTouch CAPTOUCH_DEV.
 * @return Interrupt status.
 */
u32 CapTouch_GetISR(CAPTOUCH_TypeDef *CapTouch)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));

	return CapTouch->CT_INTERRUPT_STATUS;
}

/**
 * @brief Get CapTouch channel enable status.
 * @param CapTouch CAPTOUCH_DEV.
 * @param Channel Specified channel index, which can be 0~9.
 * @return Channel enable status.
 */
u32 CapTouch_GetChStatus(CAPTOUCH_TypeDef *CapTouch, u32 Channel)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));

	if (CapTouch->CT_CH[Channel].CT_CHx_CTRL & CT_BIT_CHx_EN) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/**
 * @brief Enable or disable specified channel.
 * @param CapTouch CAPTOUCH_DEV.
 * @param Channel Specified channel index, which can be 0~9.
 * @param NewState New state of the specified channel.
 * 		This parameter can be ENABLE or DISABLE.
 * @return None
 */
void CapTouch_ChCmd(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u8 NewState)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));

	if (NewState != DISABLE) {
		/* Enable the CapTouch corresponding channel */
		CapTouch->CT_CH[Channel].CT_CHx_CTRL |= CT_BIT_CHx_EN;
	} else {
		/* Disable the CapTouch peripheral */
		CapTouch->CT_CH[Channel].CT_CHx_CTRL &= ~CT_BIT_CHx_EN;
	}
}

/**
 * @brief Set CapTouch sample period in us.
 * @param CapTouch CAPTOUCH_DEV.
 * @param PeriodUs Sample period in us.
 * @return None
 */
void CapTouch_SetClk(CAPTOUCH_TypeDef *CapTouch, u16 PeriodUs)
{
	u32 TempVal;
	u16 ClkCnt = PeriodUs * 0x4;
	u16 LCnt = CT_GET_CTC_CLK_LCNT1(CapTouch->CT_CTC_DIV_LCNT);

	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(ClkCnt >= 0x4 && ClkCnt < 0x200);
	assert_param(ClkCnt > LCnt);

	CapTouch_Cmd(CapTouch, DISABLE);

	TempVal = CapTouch->CT_CTC_DIV_HCNT;
	TempVal &= ~CT_MASK_CTC_CLK_HCNT;
	TempVal |= CT_CTC_CLK_HCNT(ClkCnt - 0x1 - LCnt);
	CapTouch->CT_CTC_DIV_HCNT = TempVal;

	CapTouch_Cmd(CapTouch, ENABLE);
}

/**
 * @brief Get Mbias current setting value according to calibrated parameters.
 * @param MbiasCurrent Mbias current value in nA.
 * @return Mbias current setting value.
 */
u16 CapTouch_GetMbiasPara(u32 MbiasCurrent)
{
	u8 EfuseBuf[12]; // 0x704-0x70F
	u8 idx;
	u8 start, end, offset;
	u16 diff;
	u16 val = 0;

	assert_param(MbiasCurrent < 172000); // nA

	/* Update CTC Mbias current according to calibrated parameters */
	if (!mbias_init_done) {

		/* Get calibrated parameters from OTP */
		for (idx = 0; idx < 12; idx++) {
			OTP_Read8(CTC_MBIAS_CAL_ADDR + idx, EfuseBuf + idx);
		}

		/* Calculate CTC Mbias current according to calibrated parameters */
		for (idx = 0; idx < 16; idx++) {
			start = Mbias_Config[idx].otp_start;
			end = Mbias_Config[idx].otp_end;
			offset = Mbias_Config[idx].otp_offset;

			if ((start == 0) && (end >= 8)) {
				diff = ((EfuseBuf[offset + 1] & (BIT(end - 7) - 1)) << 8) | EfuseBuf[offset];
			} else if (start >= 8) {
				diff = (EfuseBuf[offset + 1] & ((0xFF >> (7 - (end - start))) << (start - 8))) >> (start - 8);
			} else if (end < 8) {
				diff = (EfuseBuf[offset] & ((0xFF >> (7 - (end - start))) << start)) >> start;
			} else {
				RTK_LOGW(NOTAG, "Invalid cal para!\n");
				assert_param(0);
			}

			Mbias_Config[idx].current += (u32)((float)diff * 62.5);
			if (diff & BIT(end - start)) { // negative
				Mbias_Config[idx].current -= (u32)((float)(BIT(end - start + 1)) * 62.5);
			}
		}
		mbias_init_done = TRUE;
	}

	/* Calculate setting value according to  Mbias current */
	for (idx = 0; idx < 16; idx++) {
		if (MbiasCurrent >= Mbias_Config[idx].current) {
			val |= BIT(15 - idx);
			MbiasCurrent -= Mbias_Config[idx].current;
		}
	}

	return val;
}

/**
 * @brief Set CapTouch Scan interval.
 * @param CapTouch CAPTOUCH_DEV.
 * @param Interval Scan interval in ms.
 * @return None
 */
void CapTouch_SetScanInterval(CAPTOUCH_TypeDef *CapTouch, u32 Interval)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(Interval <= 4095);

	CapTouch_Cmd(CapTouch, DISABLE);
	TempVal = CapTouch->CT_SCAN_PERIOD;
	TempVal &= ~CT_MASK_SLEEP_TIME_CNT;
	TempVal |= Interval;
	CapTouch->CT_SCAN_PERIOD = TempVal;
	CapTouch_Cmd(CapTouch, ENABLE);
}

/**
 * @brief Set Mbias current for specified channel.
 * @param CapTouch CAPTOUCH_DEV.
 * @param Channel Specified channel index, which can be 0~9.
 * @param Mbias Mbias value.
 * @return None
 */
void CapTouch_SetChMbias(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u16 Mbias)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));

	CapTouch_Cmd(CapTouch, DISABLE);
	TempVal = CapTouch->CT_CH[Channel].CT_CHx_MBIAS_ATH;
	TempVal &= ~CT_MASK_CHx_MBIAS;
	TempVal |= CT_CHx_MBIAS(Mbias);
	CapTouch->CT_CH[Channel].CT_CHx_MBIAS_ATH = TempVal;
	CapTouch_Cmd(CapTouch, ENABLE);
}

/**
 * @brief Set relative touch threshold for related channel.
 * @param CapTouch CAPTOUCH_DEV.
 * @param Channel Specified channel index, which can be 0~9.
 * @param Threshold Related threshold value.
 * @return None
 */
void CapTouch_SetChDiffThres(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u32 Threshold)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));
	assert_param(Threshold < 4096);

	CapTouch_Cmd(CapTouch, DISABLE);
	TempVal = CapTouch->CT_CH[Channel].CT_CHx_CTRL;
	TempVal &= ~CT_MASK_CHx_D_TOUCH_TH;
	TempVal |= CT_CHx_D_TOUCH_TH(Threshold);
	CapTouch->CT_CH[Channel].CT_CHx_CTRL = TempVal;
	CapTouch_Cmd(CapTouch, ENABLE);
}

/**
 * @brief Set N-noise threshold for related channel.
 * @param CapTouch CAPTOUCH_DEV.
 * @param Channel Specified channel index, which can be 0~9.
 * @param Threshold Related N-noise threshold value.
 * @return None
 */
void CapTouch_SetNNoiseThres(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u16 Threshold)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));
	assert_param(Threshold < 4096);

	CapTouch_Cmd(CapTouch, DISABLE);
	TempVal = CapTouch->CT_CH[Channel].CT_CHx_NOISE_TH;
	TempVal &= ~CT_MASK_CHx_N_ENT;
	TempVal |= CT_CHx_N_ENT(Threshold);
	CapTouch->CT_CH[Channel].CT_CHx_NOISE_TH = TempVal;
	CapTouch_Cmd(CapTouch, ENABLE);
}

/**
 * @brief Set P-noise threshold for related channel.
 * @param CapTouch CAPTOUCH_DEV.
 * @param Channel Specified channel index, which can be 0~9.
 * @param Threshold Related P-noise threshold value.
 * @return None
 */
void CapTouch_SetPNoiseThres(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u16 Threshold)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));
	assert_param(Threshold < 4096);

	CapTouch_Cmd(CapTouch, DISABLE);
	TempVal = CapTouch->CT_CH[Channel].CT_CHx_NOISE_TH;
	TempVal &= ~CT_MASK_CHx_P_ENT;
	TempVal |= CT_CHx_P_ENT(Threshold);
	CapTouch->CT_CH[Channel].CT_CHx_NOISE_TH = TempVal;
	CapTouch_Cmd(CapTouch, ENABLE);
}

/**
 * @brief Set CapTouch full level for AFIFO.
 * @param CapTouch CAPTOUCH_DEV.
 * @param Level AFIFO full level, which can be 0~63.
 * @return None
 * @note When raw code FIFO is over level, AFIFO_OVERLVL_INTR will arise.
 */
void CapTouch_SetFifoLevel(CAPTOUCH_TypeDef *CapTouch, u8 Level)
{
	u32 TempVal;
	bool ctc_is_en = FALSE;

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(Level < 64);

	if (CapTouch->CT_CTC_CTRL & CT_BIT_ENABLE) {
		CapTouch_Cmd(CapTouch, DISABLE);
		ctc_is_en = TRUE;
	}

	TempVal = CapTouch->CT_RAW_CODE_FIFO_STATUS;
	TempVal &= ~CT_MASK_AFIFO_FULL_LVL;
	TempVal |= CT_AFIFO_FULL_LVL(Level);
	CapTouch->CT_RAW_CODE_FIFO_STATUS = TempVal;

	if (ctc_is_en) {
		CapTouch_Cmd(CapTouch, ENABLE);
	}
}

/**
 * @brief Get Mbias current value for specified channel.
 * @param CapTouch CAPTOUCH_DEV.
 * @param Channel Specified channel index, which can be 0~9.
 * @return Mbias data.
 */
u32 CapTouch_GetChMbias(CAPTOUCH_TypeDef *CapTouch, u8 Channel)
{
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));

	return CT_GET_CHx_MBIAS(CapTouch->CT_CH[Channel].CT_CHx_MBIAS_ATH);
}

/**
 * @brief Get relative threshold of touch judgement for specified channel.
 * @param CapTouch CAPTOUCH_DEV.
 * @param Channel Specified channel index, which can be 0~9.
 * @return Difference threshold of specified channel.
 */
u32 CapTouch_GetChDiffThres(CAPTOUCH_TypeDef *CapTouch, u8 Channel)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));

	return CT_GET_CHx_D_TOUCH_TH(CapTouch->CT_CH[Channel].CT_CHx_CTRL);
}

/**
 * @brief Get absolute threshold of touch judgement for specified channel.
 * @param CapTouch CAPTOUCH_DEV.
 * @param Channel Specified channel index, which can be 0~9.
 * @return Difference threshold of specified channel.
 */
u32 CapTouch_GetChAbsThres(CAPTOUCH_TypeDef *CapTouch, u8 Channel)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));

	return CT_GET_CHx_A_TOUCH_TH(CapTouch->CT_CH[Channel].CT_CHx_MBIAS_ATH);
}

/**
 * @brief Get positive or negative noise threshold for specified channel.
 * @param CapTouch CAPTOUCH_DEV.
 * @param Channel Specified channel index, which can be 0~9.
 * @param type Noise type, which can be P_NOISE_THRES or N_NOISE_THRES.
 * @return Noise threshold of specified channel.
 */
u32 CapTouch_GetNoiseThres(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u8 type)
{
	u32 value;

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));

	if (type) {
		value = CT_GET_CHx_N_ENT(CapTouch->CT_CH[Channel].CT_CHx_NOISE_TH);
	} else {
		value = CT_GET_CHx_P_ENT(CapTouch->CT_CH[Channel].CT_CHx_NOISE_TH);
	}

	return value;
}

/**
 * @brief Read baseline data from specified channel.
 * @param CapTouch CAPTOUCH_DEV.
 * @param Channel Specified channel index, which can be 0~9.
 * @return Baseline data.
 */
u32 CapTouch_GetChBaseline(CAPTOUCH_TypeDef *CapTouch, u8 Channel)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));

	return CT_GET_CHx_BASELINE(CapTouch->CT_CH[Channel].CT_CHx_CTRL);
}

/**
 * @brief Read average data from specified channel.
 * @param CapTouch CAPTOUCH_DEV.
 * @param Channel Specified channel index, which can be 0~9.
 * @return Average data.
 */
u32 CapTouch_GetChAveData(CAPTOUCH_TypeDef *CapTouch, u8 Channel)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));

	return CT_GET_CHx_DATA_AVE(CapTouch->CT_CH[Channel].CT_CHx_DATA_INF);
}

/**
 * @brief Enable or disable Debug mode.
 * @param CapTouch CAPTOUCH_DEV.
 * @param NewState New state of the Debug mode.
 * 		This parameter can be ENABLE or DISABLE.
 * @return None
 */
void CapTouch_DbgCmd(CAPTOUCH_TypeDef *CapTouch, u8 NewState)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));

	if (NewState != DISABLE) {
		/* Enable the CapTouch debug mode */
		CapTouch->CT_DEBUG_MODE_CTRL |= CT_BIT_DEBUG_EN;
	} else {
		/* Disable the CapTouch peripheral */
		CapTouch->CT_DEBUG_MODE_CTRL |= CT_BIT_CH_SWITCH_CTRL;
		CapTouch->CT_DEBUG_MODE_CTRL &= ~CT_BIT_DEBUG_EN;
	}
}

/**
 * @brief Select debug channel.
 * @param CapTouch CAPTOUCH_DEV.
 * @param Channel Specified channel index, which can be 0~9.
 * @param NewState New state of channel, which can be one of the following values:
 * 		@arg ENABLE: manual channel switch.
 * 		@arg DISABLE: auto channel switch.
 * @return None
 * @note Manual channel switch only works when debug mode is enabled.
 */
void CapTouch_DbgChannel(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u8 NewState)
{
	u32 data;

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));

	data = CapTouch->CT_DEBUG_MODE_CTRL;

	if (NewState != DISABLE) {
		assert_param(IS_CT_CHANNEL(Channel));

		data &= ~(CT_MASK_CH_CTRL | CT_BIT_CH_SWITCH_CTRL);
		data |= CT_CH_CTRL(Channel);
	} else {
		data |= CT_BIT_CH_SWITCH_CTRL;
	}

	CapTouch->CT_DEBUG_MODE_CTRL = data;
}

/**
 * @brief Enable or disable CapTouch continuos sample.
 * @param CapTouch CAPTOUCH_DEV.
 * @param NewState New state, which means whether to enable continuos sample or not.
 * 		This parameter can be ENABLE or DISABLE.
 * @return None
 * @note Continuos sample only works when debug mode and manual switch are both enabled.
 */
void CapTouch_DbgContCmd(CAPTOUCH_TypeDef *CapTouch, u8 NewState)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));

	if (NewState != DISABLE) {
		/* Enable continuos sample */
		CapTouch->CT_DEBUG_MODE_CTRL |= CT_BIT_CONTINUE_SAMPLE_EN;
	} else {
		/* Disable continuos sample */
		CapTouch->CT_DEBUG_MODE_CTRL &= ~CT_BIT_CONTINUE_SAMPLE_EN;
	}
}

/**
 * @brief Get Raw data in debug mode.
 * @param CapTouch CAPTOUCH_DEV.
 * @return Raw data.
 */
u32 CapTouch_DbgRawData(CAPTOUCH_TypeDef *CapTouch)
{
	u32 loop = 0;
	u32 data;

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));

	do {
		data = CapTouch->CT_RAW_CODE_FIFO_READ;

		if (loop++ > 200000) {
			RTK_LOGI(TAG, "sample timeout \n");
			return 0xEAEA;
		}

	} while ((data & CT_BIT_AFIFO_RD_DATA_VLD) == 0);

	return CTC_ID_AND_DATA(data);
}

/**
 * @brief Dump all CapTouch registers.
 * @param CapTouch CAPTOUCH_DEV.
 * @return None
 */
void CapTouch_DbgDumpReg(CAPTOUCH_TypeDef *CapTouch)
{
	RTK_LOGI(TAG, "\n%p: %08lX (CT_CTC_CTRL)\n", &(CapTouch->CT_CTC_CTRL), CapTouch->CT_CTC_CTRL);
	RTK_LOGI(TAG, "%p: %08lX (CT_SCAN_PERIOD)\n", &(CapTouch->CT_SCAN_PERIOD), CapTouch->CT_SCAN_PERIOD);
	RTK_LOGI(TAG, "%p: %08lX (CT_ETC_CTRL)\n", &(CapTouch->CT_ETC_CTRL), CapTouch->CT_ETC_CTRL);
	RTK_LOGI(TAG, "%p: %08lX (CT_SNR_INF)\n", &(CapTouch->CT_SNR_INF), CapTouch->CT_SNR_INF);
	RTK_LOGI(TAG, "%p: %08lX (CT_DEBUG_MODE_CTRL)\n", &(CapTouch->CT_DEBUG_MODE_CTRL), CapTouch->CT_DEBUG_MODE_CTRL);
	RTK_LOGI(TAG, "%p: %08lX (CT_RAW_CODE_FIFO_STATUS)\n", &(CapTouch->CT_RAW_CODE_FIFO_STATUS), CapTouch->CT_RAW_CODE_FIFO_STATUS);
	RTK_LOGI(TAG, "%p: %08lX (CT_RAW_CODE_FIFO_READ)\n", &(CapTouch->CT_RAW_CODE_FIFO_READ), CapTouch->CT_RAW_CODE_FIFO_READ);
	RTK_LOGI(TAG, "%p: %08lX (CT_INTERRUPT_ENABLE)\n", &(CapTouch->CT_INTERRUPT_ENABLE), CapTouch->CT_INTERRUPT_ENABLE);
	RTK_LOGI(TAG, "%p: %08lX (CT_INTERRUPT_STATUS)\n", &(CapTouch->CT_INTERRUPT_STATUS), CapTouch->CT_INTERRUPT_STATUS);
	RTK_LOGI(TAG, "%p: %08lX (CT_RAW_INTERRUPT_STATUS)\n", &(CapTouch->CT_RAW_INTERRUPT_STATUS), CapTouch->CT_RAW_INTERRUPT_STATUS);
	RTK_LOGI(TAG, "%p: %08lX (CT_INTERRUPT_ALL_CLR)\n", &(CapTouch->CT_INTERRUPT_ALL_CLR), CapTouch->CT_INTERRUPT_ALL_CLR);
	RTK_LOGI(TAG, "%p: %08lX (CT_INTERRUPT_STATUS_CLR)\n", &(CapTouch->CT_INTERRUPT_STATUS_CLR), CapTouch->CT_INTERRUPT_STATUS_CLR);
	RTK_LOGI(TAG, "%p: %08lX (CT_DEBUG_SEL)\n", &(CapTouch->CT_DEBUG_SEL), CapTouch->CT_DEBUG_SEL);
	RTK_LOGI(TAG, "%p: %08lX (CT_DEBUG_PORT)\n", &(CapTouch->CT_DEBUG_PORT), CapTouch->CT_DEBUG_PORT);
	RTK_LOGI(TAG, "%p: %08lX (CT_ECO_USE0)\n", &(CapTouch->CT_ECO_USE0), CapTouch->CT_ECO_USE0);
	RTK_LOGI(TAG, "%p: %08lX (CT_ECO_USE1)\n", &(CapTouch->CT_ECO_USE1), CapTouch->CT_ECO_USE1);
	RTK_LOGI(TAG, "%p: %08lX (CT_CTC_COMP_VERSION)\n", &(CapTouch->CT_CTC_COMP_VERSION), CapTouch->CT_CTC_COMP_VERSION);

	RTK_LOGI(TAG, "%p: %08lX (CT_CH[0].CT_CHx_CTRL)\n", &(CapTouch->CT_CH[0].CT_CHx_CTRL), CapTouch->CT_CH[0].CT_CHx_CTRL);
	RTK_LOGI(TAG, "%p: %08lX (CT_CH[0].CT_CHx_NOISE_TH)\n", &(CapTouch->CT_CH[0].CT_CHx_NOISE_TH), CapTouch->CT_CH[0].CT_CHx_NOISE_TH);
	RTK_LOGI(TAG, "%p: %08lX (CT_CH[0].CT_CHx_MBIAS_ATH)\n", &(CapTouch->CT_CH[0].CT_CHx_MBIAS_ATH), CapTouch->CT_CH[0].CT_CHx_MBIAS_ATH);
	RTK_LOGI(TAG, "%p: %08lX (CT_CH[0].CT_CHx_DATA_INF)\n", &(CapTouch->CT_CH[0].CT_CHx_DATA_INF), CapTouch->CT_CH[0].CT_CHx_DATA_INF);

	RTK_LOGI(TAG, "%p: %08lX (CT_ANA_ADC_REG0X_LPAD)\n", &(CapTouch->CT_CTC_SAMPLE_CTRL), CapTouch->CT_CTC_SAMPLE_CTRL);
}

/**
 * @brief Dump CapTouch ETC-related information.
 * @param CapTouch CAPTOUCH_DEV.
 * @param ch Specified channel index, which can be 0~9.
 * @return None
 */
void CapTouch_DbgDumpETC(CAPTOUCH_TypeDef *CapTouch, u32 ch)
{
	u32 DiffThres = 0;
	u32 Baseline = 0;
	u32 AveData = 0;
	u32 NoiseThresP = 0;
	u32 NoiseThresN = 0;
	u32 AbsoThres = 0;

	DiffThres = CapTouch_GetChDiffThres(CapTouch, ch);
	Baseline = CapTouch_GetChBaseline(CapTouch, ch);
	AveData = CapTouch_GetChAveData(CapTouch, ch);
	NoiseThresP = CapTouch_GetNoiseThres(CapTouch, ch, P_NOISE_THRES);
	NoiseThresN = CapTouch_GetNoiseThres(CapTouch, ch, N_NOISE_THRES);
	AbsoThres = CapTouch_GetChAbsThres(CapTouch, ch);

	RTK_LOGI(TAG, "\nCH[%lu] ETC: ChDiffThres:%08lX (%lu) \n", ch, DiffThres, DiffThres);
	RTK_LOGI(TAG, "CH[%lu] ETC: Baseline:%08lX (%lu) \n", ch, Baseline, Baseline);
	RTK_LOGI(TAG, "CH[%lu] ETC: RawAveData:%08lX (%lu) Diff:%ld \n", ch, AveData, AveData, AveData - Baseline);
	RTK_LOGI(TAG, "CH[%lu] ETC: NoiseThres P:%08lX (%lu) \n", ch, NoiseThresP, NoiseThresP);
	RTK_LOGI(TAG, "CH[%lu] ETC: NoiseThres N:%08lX (%lu) \n", ch, NoiseThresN, NoiseThresN);
	RTK_LOGI(TAG, "CH[%lu] ETC: AbsoThres:%08lX (%lu) \n", ch, AbsoThres, AbsoThres);
}

/**
 * @brief Enable or disable CapTouch filter function.
 * @param CapTouch CAPTOUCH_DEV.
 * @param NewState New state of the filter function.
 * 		This parameter can be ENABLE or DISABLE.
 * @return None
 */
void CapTouch_FilterCmd(CAPTOUCH_TypeDef *CapTouch, u8 NewState)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));

	if (NewState != DISABLE) {
		/* Enable the CapTouch filter function */
		CapTouch->CT_CTC_FILTER_CTRL0 |= CT_BIT_FLT_EN;
	} else {
		/* Disable the CapTouch filter function */
		CapTouch->CT_CTC_FILTER_CTRL0 &= ~CT_BIT_FLT_EN;
	}
}

/**
 * @brief Enable or disable CapTouch IIR filter function.
 * @param CapTouch CAPTOUCH_DEV.
 * @param NewState New state of the IIR filter function.
 * 		This parameter can be ENABLE or DISABLE.
 * @return None
 */
void CapTouch_FilterIIRCmd(CAPTOUCH_TypeDef *CapTouch, u8 NewState)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));

	if (NewState != DISABLE) {
		/* Enable the CapTouch IIR filter function */
		CapTouch->CT_CTC_FILTER_CTRL0 |= CT_BIT_IIR_EN;
	} else {
		/* Disable the CapTouch IIR filter function */
		CapTouch->CT_CTC_FILTER_CTRL0 &= ~CT_BIT_IIR_EN;
	}
}

/**
 * @brief Configure CapTouch IIR filter, including accelerate touch response function and coefficients.
 * @param CapTouch CAPTOUCH_DEV.
 * @param AccelEn Enable IIR accelerated touch response or not.
 * 		This parameter can be ENABLE or DISABLE.
 * @param CoefNorm Normal IIR filter coefficient.
 * @param CoefAccel Accelerated IIR filter coefficient.
 * @return None
 */
void CapTouch_FilterIIRConfig(CAPTOUCH_TypeDef *CapTouch, u8 AccelEn, u8 CoefNorm, u8 CoefAccel)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(CoefNorm < 0xC);
	assert_param(CoefAccel < 0xC);

	TempVal = CapTouch->CT_CTC_FILTER_CTRL1;
	TempVal &= ~CT_MASK_IIR_K_NORMAL;
	TempVal |= CT_IIR_K_NORMAL(CoefNorm);

	if (AccelEn != DISABLE) {
		/* Enable the CapTouch accelerated touch response */
		TempVal |= CT_BIT_IIR_K_ACC_EN;
		TempVal &= ~CT_MASK_IIR_K_ACC;
		TempVal |= CT_IIR_K_ACC(CoefAccel);
	} else {
		/* Disable the CapTouch accelerated touch response */
		TempVal &= ~CT_BIT_IIR_K_ACC_EN;
	}
	CapTouch->CT_CTC_FILTER_CTRL1 = TempVal;
}

/**
 * @brief Set decimation factor of decimation filter.
 * @param CapTouch CAPTOUCH_DEV.
 * @param MDec Specified decimation of sample point.
 * @return None
 */
void CapTouch_SetMDec(CAPTOUCH_TypeDef *CapTouch, u32 MDec)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_DEC_M(MDec));

	TempVal = CapTouch->CT_CTC_FILTER_CTRL0;
	TempVal &= ~CT_MASK_MDEC;
	TempVal |= CT_MDEC(MDec);
	CapTouch->CT_CTC_FILTER_CTRL0 = TempVal;
}

/**
 * @brief Get decimation of sample point with different AC frequency.
 * @param CapTouch CAPTOUCH_DEV.
 * @param ACFreq AC frequency, which should be 50 or 60.
 * @return Decimation of sample point.
 */
u32 CapTouch_GetMDec(CAPTOUCH_TypeDef *CapTouch, u32 ACFreq)
{
	u32 clk_cnt, sample_cnt;

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));

	clk_cnt = CT_GET_CTC_CLK_HCNT(CapTouch->CT_CTC_DIV_HCNT);
	clk_cnt += CT_GET_CTC_CLK_LCNT1(CapTouch->CT_CTC_DIV_LCNT);
	sample_cnt = CT_GET_SAMPLE_AVE_CTRL(CapTouch->CT_SCAN_PERIOD);

	return 4000000 / clk_cnt / (ACFreq * BIT(sample_cnt + 2)) - 1;
}

/**
 * @brief Set coefficient of median filter.
 * @param CapTouch CAPTOUCH_DEV.
 * @param MedCoef Specified coefficient of median filter,
 * 		which can be a value of @ref CapTouch_Med_Flt_Coef.
 * @return None
 */
void CapTouch_SetMedCoef(CAPTOUCH_TypeDef *CapTouch, u8 MedCoef)
{
	u32 TempVal;

	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_MED_COEF(MedCoef));

	TempVal = CapTouch->CT_CTC_FILTER_CTRL0;
	TempVal &= ~CT_MASK_MED_COEF;
	TempVal |= CT_MED_COEF(MedCoef);
	CapTouch->CT_CTC_FILTER_CTRL0 = TempVal;
}

/**
 * @brief Configure CapTouch median average filter.
 * @param CapTouch CAPTOUCH_DEV.
 * @param MedVal Low clock count of LCNT1.
 * @param MedStep Difference value between LCNT0/LCNT1/LCNT2.
 * @note When MedStep is 0, median average filter is disabled.
 * @return None
 */
void CapTouch_AveMedConfig(CAPTOUCH_TypeDef *CapTouch, u16 MedVal, u16 MedStep)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(MedVal > MedStep);
	assert_param(MedVal + MedStep < 0x200);

	CapTouch->CT_CTC_DIV_LCNT = CT_CTC_CLK_LCNT0(MedVal - MedStep) |
								CT_CTC_CLK_LCNT1(MedVal) |
								CT_CTC_CLK_LCNT2(MedVal + MedStep);

	if (MedStep != 0) {
		/* Enable the CapTouch median average filter function */
		CapTouch->CT_CTC_FILTER_CTRL0 |= CT_BIT_MED1_EN;
	} else {
		/* Disable the CapTouch median average filter function */
		CapTouch->CT_CTC_FILTER_CTRL0 &= ~CT_BIT_MED1_EN;
	}
}

/**
 * @brief Configure CapTouch sample mode and sample count.
 * @param CapTouch CAPTOUCH_DEV.
 * @param SampMode CapTouch sample mode.
 * 		This parameter can be CTC_SAMP_REGULAR or CTC_SAMP_STAGGERED.
 * @param SampCnt CapTouch sample count and actual sample point is 2^(SampCnt+2).
 * @return None
 */
void CapTouch_OverSampConfig(CAPTOUCH_TypeDef *CapTouch, u8 SampMode, u8 SampCnt)
{
	u32 TempVal;

	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CTC_SAMP_MDOE(SampMode));
	assert_param(SampCnt < 0x8);

	if (SampMode == CTC_SAMP_STAGGERED) {
		/* All sample conversions are done in staggered sequence. */
		CapTouch->CT_CTC_OV_MODE |= CT_BIT_OV_MODE;
	} else {
		/* All sample conversions are done in regular sequence. */
		CapTouch->CT_CTC_OV_MODE &= ~CT_BIT_OV_MODE;
	}

	TempVal = CapTouch->CT_SCAN_PERIOD;
	TempVal &= ~CT_MASK_SAMPLE_AVE_CTRL;
	TempVal |= CT_SAMPLE_AVE_CTRL(SampCnt);
	CapTouch->CT_SCAN_PERIOD = TempVal;
}

/**
 * @brief Set CapTouch wait time in staggered mode.
 * @param CapTouch CAPTOUCH_DEV.
 * @param WaitTime Specified wait time before starting the next periodic sampling.
 * @note WaitTime is in unit of sample clock.
 * @return None
 * @note If WaitTime is set to 0, HW will scan continuously and have no sleep time.
 */
void CapTouch_SetWaitTime(CAPTOUCH_TypeDef *CapTouch, u32 WaitTime)
{
	u32 TempVal;

	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(WaitTime < 0x40000);

	TempVal = CapTouch->CT_CTC_OV_MODE;
	TempVal &= ~CT_MASK_OV_WAIT_TIMER_CNT;
	TempVal |= CT_OV_WAIT_TIMER_CNT(WaitTime);
	CapTouch->CT_CTC_OV_MODE = TempVal;
}

/**
 * @brief Set CapTouch trigger mode.
 * @param CapTouch CAPTOUCH_DEV.
 * @param TrigMode Captouch trigger mode.
 * 		This parameter can be CTC_DUMMY_TRIG or CTC_CH_EN_TRIG.
 * @return None
 */
void CapTouch_SetTrigMode(CAPTOUCH_TypeDef *CapTouch, u8 TrigMode)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CTC_TRIG_MDOE(TrigMode));

	if (TrigMode == CTC_DUMMY_TRIG) {
		/* captouch can be triggered only in dummy cycle */
		CapTouch->CT_CTC_OV_MODE |= CT_BIT_CTC_TRIG_MODE;
	} else {
		/* captouch can be triggered when channel is enabled */
		CapTouch->CT_CTC_OV_MODE &= ~CT_BIT_CTC_TRIG_MODE;
	}
}

/**
 * @brief Configure CapTouch Rx DMA burst size.
 * @param CapTouch CAPTOUCH_DEV.
 * @param RxDmaBurstSize CapTouch Rx DMA burst size, which can be a value of @ref CapTouch_RXDMA_Setting.
 * @note Although CapTouch rx FIFO depth is 64 in hardware, Rx DMA burst size should be no more than 32 due to DMA FIFO depth limit.
 * @note Rx DMA burst size should not be smaller than GDMA_SrcMsize in function CapTouch_RXGDMA_Init();
 * 		For better performance, it is suggested to make Rx DMA burst size be equal to GDMA_SrcMsize, which is 8.
 * @return None
 */
void CapTouch_RXDMAConfig(CAPTOUCH_TypeDef *CapTouch, u32 RxDmaBurstSize)
{
	u32 TempVal;

	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CTC_RXDMA_MSIZE(RxDmaBurstSize));

	/* Set CapTouch Rx DMA burst size */
	TempVal = CapTouch->CT_CTC_DMA;
	TempVal &= ~CT_MASK_RXDMA_MSIZE;
	TempVal |= CT_RXDMA_MSIZE(RxDmaBurstSize);

	CapTouch->CT_CTC_DMA = TempVal;
}

/**
 * @brief Enable or disable CapTouch RX DMA.
 * @param CapTouch CAPTOUCH_DEV.
 * @param NewState New state of CapTouch RX DMA.
 * 		This parameter can be ENABLE or DISABLE.
 * @return None
 */
void CapTouch_RXDMACmd(CAPTOUCH_TypeDef *CapTouch, u32 NewState)
{
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));

	if (NewState != DISABLE) {
		/* enable the CapTouch RX DMA */
		CapTouch->CT_CTC_DMA |= CT_BIT_RXDMA_EN;
	} else {
		/* disable the CapTouch RX DMA */
		CapTouch->CT_CTC_DMA &= ~CT_BIT_RXDMA_EN;
	}
}

/**
 * @brief Initialize and enable CapTouch RX GDMA.
 * @param GDMA_InitStruct Pointer to a GDMA_InitTypeDef structure that contains the config info of the GDMA.
 * @param CallbackData GDMA callback data.
 * @param CallbackFunc GDMA callback function.
 * @param pRxBuf Rx Buffer.
 * @param RxCount Rx Count.
 * @retval TRUE or FALSE
 */
u8 CapTouch_RXGDMA_Init(
	CAPTOUCH_TypeDef *CapTouch,
	GDMA_InitTypeDef *GDMA_InitStruct,
	void *CallbackData,
	IRQ_FUN CallbackFunc,
	u8 *pRxBuf,
	u32 RxCount
)
{
	u8 GdmaChnl;

	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(GDMA_InitStruct != NULL);

	DCache_CleanInvalidate((u32)pRxBuf, RxCount);

	GdmaChnl = GDMA_ChnlAlloc(0, (IRQ_FUN)CallbackFunc, (u32)CallbackData, INT_PRI_MIDDLE);
	if (GdmaChnl == 0xFF) {
		/* No available DMA channel */
		return FALSE;
	}

	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));
	GDMA_InitStruct->GDMA_DIR = TTFCPeriToMem;
	GDMA_InitStruct->GDMA_SrcHandshakeInterface = GDMA_HANDSHAKE_INTERFACE_CTC_RX;
	GDMA_InitStruct->GDMA_SrcAddr = (u32)&CapTouch->CT_RAW_CODE_FIFO_READ;
	GDMA_InitStruct->GDMA_Index = 0;
	GDMA_InitStruct->GDMA_ChNum = GdmaChnl;
	GDMA_InitStruct->GDMA_IsrType = (BlockType | TransferType | ErrType);
	GDMA_InitStruct->GDMA_SrcMsize = MsizeEight;
	GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthTwoBytes;
	GDMA_InitStruct->GDMA_DstInc = IncType;
	GDMA_InitStruct->GDMA_SrcInc = NoChange;

	if (((u32)(pRxBuf) & 0x03) == 0) {
		/*  4-bytes aligned, move 4 bytes each DMA transaction */
		GDMA_InitStruct->GDMA_DstMsize = MsizeFour;
		GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;
	} else {
		/*  move 1 byte each DMA transaction */
		GDMA_InitStruct->GDMA_DstMsize = MsizeSixteen;
		GDMA_InitStruct->GDMA_DstDataWidth = TrWidthOneByte;
	}
	GDMA_InitStruct->GDMA_BlockSize = RxCount >> 1;
	GDMA_InitStruct->GDMA_DstAddr = (u32)(pRxBuf);

	/* multi block close */
	GDMA_InitStruct->MuliBlockCunt = 0;
	GDMA_InitStruct->GDMA_ReloadSrc = 0;
	GDMA_InitStruct->MaxMuliBlock = 1;

	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	return TRUE;
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