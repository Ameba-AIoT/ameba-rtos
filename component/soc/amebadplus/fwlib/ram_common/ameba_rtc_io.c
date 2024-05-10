/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup RTC_IO
* @brief RTC_IO driver modules
* @{
*/

/* Exported functions --------------------------------------------------------*/
/** @defgroup RTC_IO_Exported_Functions RTC_IO Exported Functions
  * @{
  */

/**
  * @brief  Configure RTCIO work mode,it works MODE_SHIFT_DISABLE by default
  * @param  Mode: new work mode of RTCIO.
  *   This parameter can be:
  *    MODE_SHIFT_DISABLE,
  *    MODE_SHIFT_ENABLE,
  *    MODE_RTC_START,
  * @retval None
  */
void RTCIO_SetWorkMode(u32 Mode)
{
	u32 Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_RTC_IO_CTRL);

	assert_param(Mode <= 2);

	Temp &= ~AON_MASK_RTC_IO_CTRL;
	Temp |= AON_RTC_IO_CTRL(Mode);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_RTC_IO_CTRL, Temp);
}

/**
  * @brief  Get RTCIO work mode
  * @param  None
  * @retval Mode: current work mode of RTCIO.
  *    This parameter can be:
  *    0: MODE_SHIFT_DISABLE,
  *    1: MODE_SHIFT_ENABLE,
  *    2: MODE_RTC_START,
  */
u32 RTCIO_GetWorkMode(void)
{
	u32 Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_RTC_IO_CTRL);

	Temp = AON_GET_RTC_IO_CTRL(Temp);

	return Temp;
}

/**
  * @brief Check RTCIO Module enabled or not
  * @param None
  * @retval
  *    TRUE: enabled
  *    FALSE: disabled
  */
u32 RTCIO_IsEnabled(void)
{
	u32 ret = FALSE;
	u32 Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_RTC_IO_CTRL);

	ret = (Temp & AON_BIT_RTC_MODE) ? TRUE : FALSE;

	return ret;
}

/**
  * @brief Check RTC Module PredivAsync setting
           under RTCIO Module permission or not
  * @param None
  * @retval TRUE: under permission
            FALSE: beyond permission.
  */
static u32 RTCIO_CheckRTCPredivAsync(void)
{
	u32 ret = FALSE;
	u32 rtc_divA = 0;

	RTC_TypeDef *RTC = RTC_DEV;

	if (TrustZone_IsSecure()) {
		RTC = RTC_DEV_S;
	}

	rtc_divA = RTCIO_GET_RTC_PREDIV_ASYNC(RTC->RTC_PRER);

	ret = RTCIO_CHECK_RTC_PREDIV_ASYNC(rtc_divA) ? TRUE : FALSE;
	assert_param(ret);

	return ret;
}

/**
  * @brief Receive calibration value from rtc_sio
  * @param
  *   This parameter can be:
  *    RTCIO_RECV_RVAL_RST,
  *    RTCIO_RECV_RVAL_CAL
  * @retval None
  */
void RTCIO_SetRValue(u32 ResetRcal)
{
	u32 temp = 0;
	u32 bkp1 = 0;
	u32 ldo_rcal = 0;
	LDO_TypeDef *LDO = LDO_BASE;

	/* get rcal value from LDO */
	ldo_rcal = LDO_GET_RCAL(LDO->LDO_32K_OSC_CTRL);

	/* store BKUP1 default value */
	bkp1 = temp = BKUP_Read(BKUP_REG1);
	temp &= ~RTCIO_MASK_DATAIN_RCAL;

	if (ResetRcal == RTCIO_RECV_RVAL_RST) {
		//only send 6b'0 to RTCIO

	} else if (ResetRcal == RTCIO_RECV_RVAL_CAL) {
		//send rcal from ldo to RTCIO
		temp |= RTCIO_DATAIN_RCAL(ldo_rcal);
	}

	/* step1: write din to register */
	BKUP_Write(BKUP_REG1, temp);

	/* step2: send din to rtcio module by goto test mode */
	RTCIO_SetWorkMode(MODE_SHIFT_ENABLE);
	DelayMs(15);
	RTCIO_SetWorkMode(MODE_SHIFT_DISABLE);

	/* restore BKUP1 default value */
	BKUP_Write(BKUP_REG1, bkp1);

}

/**
  * @brief Enable RTCIO by setting work mode to MODE_RTC_START
  * @param None
  * @retval None
  */
void RTCIO_ModeRTCStart(void)
{
	/* enable RTCIO work mode */
	RTCIO_SetWorkMode(MODE_RTC_START);

	/* check Prediv_Async setting in RTC Module  */
	RTCIO_CheckRTCPredivAsync();
}

/**
  * @brief Shift out time info:
  *         1.time stored from RTC before power off
  *         2.counter in power off period
  * @outparam struct RTCIO_TimeInfo
  * @retval None
  */
static void RTCIO_ShiftOutTimeInfo(RTCIO_TimeInfo *pDataOut)
{
	u32 Temp0, Temp1;
	u32 year_l, year_h;
	u32 sec_l, sec_h;
	u32 sec_0p5, sec;

	/* step1: get bkup time before pwd */
	Temp0 = HAL_READ32(RTCIO_DATAOUT_ADDR0, 0);
	Temp1 = HAL_READ32(RTCIO_DATAOUT_ADDR1, 0);

	pDataOut->Bkup_Seconds = RTCIO_BKUP_TIME_SEC(Temp0);
	pDataOut->Bkup_Minutes = RTCIO_BKUP_TIME_MIN(Temp0);
	pDataOut->Bkup_Hours = RTCIO_BKUP_TIME_HOUR(Temp0);
	pDataOut->Bkup_Days = RTCIO_BKUP_TIME_DAY(Temp0);

	year_l = RTCIO_BKUP_TIME_YEAR_L(Temp0);
	year_h = RTCIO_BKUP_TIME_YEAR_H(Temp1);
	pDataOut->Bkup_Year = RTCIO_BKUP_TIME_YEAR(year_h, year_l);

	/* step2: get counter in pwd period */
	Temp0 = HAL_READ32(RTCIO_DATAOUT_ADDR1, 0);
	sec_0p5 = RTCIO_CNT_SECS_0P5(Temp0);
	sec_l = RTCIO_CNT_SECS_L(Temp0);

	Temp1 = HAL_READ32(RTCIO_DATAOUT_ADDR2, 0);
	sec_h = RTCIO_CNT_SECS_H(Temp1);

	sec = RTCIO_CNT_SECS(sec_h, sec_l) + RTCIO_CNT_SECS_OVER_0P5(sec_0p5);

	pDataOut->Pwd_Counter = sec;

}

/**
  * @brief Output time info after setting work mode to MODE_SHIFT_ENABLE
  * @paramOut struct RTCIO_TimeInfo
  * @retval None
  */
void RTCIO_GetTimeInfo(RTCIO_TimeInfo *pDataOut)
{
#if 0	//optional from DD
	u32 temp = 0;

	/* 0x410080E0[75:66] MUST be 0 from DD */
	temp = HAL_READ32(RTCIO_DATAIN_ADDR2, 0);
	temp &= ~RTCIO_MASK_DATAIN_LIMIT;
	HAL_WRITE32(RTCIO_DATAIN_ADDR2, 0, temp);
#endif

	/* set rval later */

	/* step2: set shift enable mode */
	RTCIO_SetWorkMode(MODE_SHIFT_ENABLE);

	/* step3: wait 15ms */
	DelayMs(15);

	/* step4: get bkup time and counter */
	RTCIO_ShiftOutTimeInfo(pDataOut);

	/* step5: set shift enable mode */
	RTCIO_SetWorkMode(MODE_SHIFT_DISABLE);

}

/** @} */
/** @} */
/** @} */