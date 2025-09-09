/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup SWR
* @brief SWR driver modules
* @{
*/
static const char *const TAG = "SWR";

/** @defgroup SWR_Exported_Functions SWR Exported Functions
  * @{
  */
/**
  * @brief  Enable or disable SWR_MEN.
  * @param  NewState: DISABLE/ENABLE
  * @retval SWR operation status, 0 operation OK, 1 operation fail.
  */
u32 SWR_MEM(u32 NewState)
{
	SWR_TypeDef *swr = SWR2_BASE;
	u32 Rtemp;
	/*timeout value according to SWR_MASK_STD_L1 of SWR_ON_CTRL0*/
	u32 swr_timeout = 100000;

	if (NewState == ENABLE) {
		Rtemp = swr->SWR_OFF_CTRL1;
		Rtemp &= (~SWR_MASK_VOFB_SEL);
		Rtemp |= SWR_VOFB_SEL_(0x1);
		swr->SWR_OFF_CTRL1 = Rtemp;

		swr->SWR_ON_CTRL0 &= (~SWR_MASK_STD_L1);

		/* 1.35v for DDR3L, 1.8v for psram and DDR2 */
		swr->SWR_ON_CTRL2 &= (~SWR_MASK_REG_PWM_VREF_SEL);
		swr->SWR_ON_CTRL0 &= (~SWR_MASK_VREFPFM_L);
		if (ChipInfo_DDRType() == MCM_DDR3L) {
			swr->SWR_ON_CTRL2 |= DDR3L_Vol;
			swr->SWR_ON_CTRL0 |= SWR_VREFPFM_L(DDR3L_Vol_PFM);
		} else {
			swr->SWR_ON_CTRL2 |= DDR2_PS_Vol;
			swr->SWR_ON_CTRL0 |= SWR_VREFPFM_L(DDR2_PS_Vol_PFM);
		}

		SWR_Calib_MEM();

		swr->REGU_POWER_CTRL |= SWR_BIT_POW_BG;

		DelayUs(300);

		swr->REGU_POWER_CTRL |= SWR_BIT_POW_SW;

		// Wait ready
		while (!(swr->REGU_POWER_CTRL & SWR_BIT_PRDY_SWR)) {
			swr_timeout--;
			if (swr_timeout == 0) {
				return 1;
			}
		}
		return 0;
	} else {
		swr->REGU_POWER_CTRL &= (~SWR_BIT_POW_SW);
		swr->REGU_POWER_CTRL &= (~SWR_BIT_POW_BG);
		return 0;
	}
}

/**
  * @brief  Enable or disable Manual mode.
  * @param  NewState: DISABLE/ENABLE
  * @retval SWR operation status, 0 operation OK, 1 operation fail.
  */
void SWR_MEM_Manual(u32 NewState)
{
	SWR_TypeDef *swr = SWR2_BASE;

	if (NewState) {
		//mode by manual
		swr->REGU_POWER_CTRL |= SWR_BIT_SWR1P8_MANUAL_EN;
	} else {
		//mode with core swr
		swr->REGU_POWER_CTRL &= ~SWR_BIT_SWR1P8_MANUAL_EN;

	}

}

/**
  * @brief  Get mem swr mode.
  * @retval swr mode.
  *   swr mode shows as follows:
  *            - SWR_PWM : PWM mode
  *            - SWR_PFM : PFM mode
  */
u32 SWR_MEM_Mode_Get(void)
{
	SWR_TypeDef *swr = SWR2_BASE;

	return ((swr->REGU_POWER_CTRL & SWR_BIT_MODE_L) >> 18);
}


/**
  * @brief  Switch SWR_MEN mode.
  * @param  SWR_Mode: SWR_PWM/SWR_PFM
  * @retval SWR operation status, 0 operation OK, 1 operation fail.
  */
u32 SWR_MEM_Mode_Set(u32 SWR_Mode)
{
	SWR_TypeDef *swr = SWR2_BASE;
	u32 Rtemp;
	/*mode switch 100us*/
	u32 swr_timeout = 10000;

	assert_param(IS_SWR_MODE(SWR_Mode));

	if (SWR_PWM == SWR_Mode) {
		if (SWR_PWM == SWR_MEM_Mode_Get()) {
			return 0;
		}
		swr->SWR_ON_CTRL1 |= SWR_BIT_PWM_FORCE;

		DelayUs(100);

		Rtemp = swr->SWR_ON_CTRL0;
		Rtemp |= SWR_BIT_FPWM_L1;
		swr->SWR_ON_CTRL0 = Rtemp;

		DelayUs(100);

		swr->SWR_ON_CTRL1 &= (~SWR_BIT_PWM_FORCE);

		while (!(swr->REGU_POWER_CTRL & SWR_BIT_MODE_L)) {
			swr_timeout--;
			if (swr_timeout == 0) {
				return 1;
			}
		}
		return 0;
	} else {
		if (SWR_PFM == SWR_MEM_Mode_Get()) {
			return 0;
		}

		Rtemp = swr->SWR_ON_CTRL0;
		Rtemp &= (~SWR_BIT_FPWM_L1);
		swr->SWR_ON_CTRL0 = Rtemp;

		while (swr->REGU_POWER_CTRL & SWR_BIT_MODE_L) {
			swr_timeout--;
			if (swr_timeout == 0) {
				return 1;
			}
		}
		return 0;
	}
}

/**
  * @brief  Enable or disable SWR_Audio.
  * @param  NewState: DISABLE/ENABLE
  * @retval SWR operation status, 0 operation OK, 1 operation fail.
  */
u32 SWR_AUDIO(u32 NewState)
{
	SWR_TypeDef *swr = SWR1_BASE;
	u32 Rtemp;
	u32 swr_timeout = 100000;

	if (NewState == ENABLE) {
		SWR_Calib_AUD();

		Rtemp = swr->SWR_OFF_CTRL1;
		Rtemp &= (~SWR_MASK_VOFB_SEL);
		Rtemp |= SWR_VOFB_SEL_(0x1);
		swr->SWR_OFF_CTRL1 = Rtemp;

		swr->SWR_ON_CTRL0 &= (~SWR_MASK_STD_L1);
		swr->REGU_POWER_CTRL |= SWR_BIT_POW_BG;

		DelayUs(300);

		swr->REGU_POWER_CTRL |= SWR_BIT_POW_SW;

		// Wait ready
		while (!(swr->REGU_POWER_CTRL & SWR_BIT_PRDY_SWR)) {
			swr_timeout--;
			if (swr_timeout == 0) {
				return 1;
			}
		}
		return 0;
	} else {
		swr->REGU_POWER_CTRL &= (~SWR_BIT_POW_SW);
		swr->REGU_POWER_CTRL &= (~SWR_BIT_POW_BG);
		return 0;
	}
}

/**
  * @brief  Enable or disable Manual mode.
  * @param  NewState: DISABLE/ENABLE
  * @retval SWR operation status, 0 operation OK, 1 operation fail.
  */
void SWR_AUDIO_Manual(u32 NewState)
{
	SWR_TypeDef *swr = SWR1_BASE;

	if (NewState) {
		//mode by manual
		swr->REGU_POWER_CTRL |= SWR_BIT_SWR1P8_MANUAL_EN;
	} else {
		//mode with core swr
		swr->REGU_POWER_CTRL &= ~SWR_BIT_SWR1P8_MANUAL_EN;

	}

}

/**
  * @brief  Get psram swr mode.
  * @retval swr mode.
  *   swr mode shows as follows:
  *            - SWR_PWM : PWM mode
  *            - SWR_PFM : PFM mode
  */
u32 SWR_AUDIO_Mode_Get(void)
{
	SWR_TypeDef *swr = SWR1_BASE;

	return ((swr->REGU_POWER_CTRL & SWR_BIT_MODE_L) >> 18);
}

/**
  * @brief  Switch SWR_PSRAM mode.
  * @param  SWR_Mode: SWR_PWM/SWR_PFM
  * @retval SWR operation status, 0 operation OK, 1 operation fail.
  */
u32 SWR_AUDIO_Mode_Set(u32 SWR_Mode)
{
	SWR_TypeDef *swr = SWR1_BASE;
	u32 Rtemp;
	/*mode switch 100us*/
	u32 swr_timeout = 10000;

	assert_param(IS_SWR_MODE(SWR_Mode));

	if (SWR_PWM == SWR_Mode) {
		if (SWR_PWM == SWR_AUDIO_Mode_Get()) {
			return 0;
		}
		swr->SWR_ON_CTRL1 |= SWR_BIT_PWM_FORCE;

		DelayUs(100);

		Rtemp = swr->SWR_ON_CTRL0;
		Rtemp |= SWR_BIT_FPWM_L1;
		swr->SWR_ON_CTRL0 = Rtemp;

		DelayUs(100);

		swr->SWR_ON_CTRL1 &= (~SWR_BIT_PWM_FORCE);

		while (!(swr->REGU_POWER_CTRL & SWR_BIT_MODE_L)) {
			swr_timeout--;
			if (swr_timeout == 0) {
				return 1;
			}
		}
		return 0;
	} else {
		if (SWR_PFM == SWR_AUDIO_Mode_Get()) {
			return 0;
		}

		Rtemp = swr->SWR_ON_CTRL0;
		Rtemp &= (~SWR_BIT_FPWM_L1);
		swr->SWR_ON_CTRL0 = Rtemp;

		while (swr->REGU_POWER_CTRL & SWR_BIT_MODE_L) {
			swr_timeout--;
			if (swr_timeout == 0) {
				return 1;
			}
		}
		return 0;
	}
}

/**
  * @brief  Get core swr mode.
  * @retval swr mode.
  *   swr mode shows as follows:
  *            - SWR_PWM : PWM mode
  *            - SWR_PFM : PFM mode
  */
u32 SWR_Mode_Get(void)
{
	REGU_TypeDef *regu = REGU_BASE;

	if (regu->REGU_POWER_CTRL & REGU_BIT_SYS_PWM_REQ) {
		return SWR_PWM;
	} else {
		return SWR_PFM;
	}
}

/**
  * @brief  set Core mode req from sys
  * @param  MODE: ENABLE PFM, DISABLE PWM
  * @retval None.
  */
void SWR_PFM_MODE_Set(u32 MODE)
{
	u32 temp;

	REGU_TypeDef *regu = REGU_BASE;
	temp = regu->REGU_POWER_CTRL;

	if (MODE && (SWR_PWM == SWR_Mode_Get())) {
		if (SWR_In_BST_MODE()) {
			RTK_LOGE(TAG, "In BST mode Now, goto Normal mode First !\n");
			return;
		}
		temp &= (~(REGU_BIT_SYS_PWM_REQ));
		regu->REGU_POWER_CTRL = temp;
	} else if ((!MODE) && (SWR_PFM == SWR_Mode_Get())) {
		temp |= (REGU_BIT_SYS_PWM_REQ);
		regu->REGU_POWER_CTRL = temp;
	}
	return;
}

/**
  * @brief  set Core mode bst mode
  * @param  MODE: ENABLE BST, DISABLE BST
  * @retval None.
  */
void SWR_BST_MODE_Set(u32 MODE)
{

	if (MODE) {
		REGU_BASE->REGU_POWER_CTRL = (REGU_BASE->REGU_POWER_CTRL & ~REGU_MASK_SYS_SWR_REQ) | REGU_SYS_SWR_REQ(3);
	} else {
		REGU_BASE->REGU_POWER_CTRL = (REGU_BASE->REGU_POWER_CTRL & ~REGU_MASK_SYS_SWR_REQ) | REGU_SYS_SWR_REQ(1);

	}
}

/**
  * @brief  get Core mode if in bst mode
  * @param  NONE
  * @retval 1: in burst mode, 0: in PWM mode.
  */
u8 SWR_In_BST_MODE(void)
{

	u8 mode = REGU_GET_SYS_SWR_REQ(REGU_BASE->REGU_POWER_CTRL);

	if (mode > VOL_09) {
		return 1;
	}

	return 0;
}
/**@}*/
/**@}*/
/**@}*/