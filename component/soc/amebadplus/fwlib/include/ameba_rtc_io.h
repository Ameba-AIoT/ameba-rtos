/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_RTC_IO_H_
#define _AMEBA_RTC_IO_H_

/* AUTO_GEN_START */

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup RTC_IO
  * @brief RTC_IO driver modules
  * @{
  */

/* AUTO_GEN_END */


/* MANUAL_GEN_START */

//Please add your defination here
/* Exported constants --------------------------------------------------------*/

/** @defgroup RTC_IO_Exported_Constants RTC_IO Exported Constants
  * @{
  */

/** @defgroup RTC_IO_Work_Mode
  * @brief RTC_IO work mode
  * @{
  */
#define MODE_SHIFT_DISABLE						(0)
#define MODE_SHIFT_ENABLE						(1)
#define MODE_RTC_START							(2)
/** @} */

/** @defgroup RTC_IO_RECV_RVAL
  * @brief RTC_IO recv new Rval or Reset value
  * @{
  */
#define RTCIO_RECV_RVAL_RST						TRUE
#define RTCIO_RECV_RVAL_CAL						FALSE
/** @} */

/** @defgroup RTC_IO_Time_Convert
  * @brief Time Convert from TIME_PRE to TIME_POST
  * @{
  */
#define TIME_STEP(TIME_PRE, TIME_STEP_CNT, MOD_NUM, TIME_POST) do {\
		TIME_POST = (TIME_PRE + TIME_STEP_CNT) % MOD_NUM; \
		TIME_STEP_CNT = (TIME_PRE + TIME_STEP_CNT) / MOD_NUM; \
} while (0)
/** @} */

/** @} */

/* Exported types ------------------------------------------------------------*/
/** @addtogroup RTC_IO_Exported_Types RTC_IO Exported Types
  * @{
  */

/**
  * @brief RTCIO_TimeInfo Structure Definition
  */
typedef struct {//Take Care!!! member's Order and Size are forbidden change
	u32 Bkup_Seconds;
	u32 Bkup_Minutes;
	u32 Bkup_Hours;
	u32 Bkup_Days;
	u32 Bkup_Year;//time stored before power down; fmt: 24H
	//u8 padding; //align RTC_TimeTypeDef

	u32 Pwd_Counter;//counter in power off; unit: second
} RTCIO_TimeInfo;
/** @} */

/** @} */
/** @} */

/* following shouldn't be placed into ebook -------------------------------*/

/* start defgroup RTC_IO_test_din */

//0x420080E0~0x420080EC(AON_BK0 ~ AON_BK3) rtc_io_test_din [127:0]
//input LDO_RCAL
#define RTCIO_MASK_DATAIN_RCAL					((u32)0x0000003F << 26)
#define RTCIO_DATAIN_RCAL(x)					((u32)((x) & 0x0000003F) << 26)			/* 6'b0; 0x410080E4[31:26] << 0x410080E0[63:58]; LDO_RCAL*/
#define RTCIO_GET_DATAIN_RCAL(x)				((u32)((x >> 26) & 0x0000003F))

//MUST 0 from DD
#define RTCIO_MASK_DATAIN_LIMIT					((u32)0x000003FF << 2)
#define RTCIO_DATAIN_LIMIT(x)					((u32)((x) & 0x000003FF) << 2)			/* 10'b0; 0x410080E8[11:2] << 0x410080E0[75:66]; MUST be 0*/
#define RTCIO_GET_DATAIN_LIMIT(x)				((u32)((x >> 2) & 0x000003FF))
/* end group RTC_IO_test_din */

/* start defgroup RTC_IO_test_dout */
//BASE 0x4100_80C0[127:0]
#define RTCIO_DATAOUT_ADDR0						(SYSTEM_CTRL_BASE + REG_AON_RTC_IO_SOUT0)
#define RTCIO_DATAOUT_ADDR1						(SYSTEM_CTRL_BASE + REG_AON_RTC_IO_SOUT1)
#define RTCIO_DATAOUT_ADDR2						(SYSTEM_CTRL_BASE + REG_AON_RTC_IO_SOUT2)
#define RTCIO_DATAOUT_ADDR3						(SYSTEM_CTRL_BASE + REG_AON_RTC_IO_SOUT3)

/*
1.rtc_io_test_dout[33：0]: {year[7:0], day[8:0], hour[4:0], min[5:0], sec[5:0] }
2.binary storage for year/day/hour/minute/second, eg: 33sec saved as sec[5:0]=6’b10_0001
3.new_sec[:] = sec[5:0] + rtc_io_test_dout[83:59]
*/
#define RTCIO_BKUP_TIME_SEC(x)					((u32)((x >> 0) & 0x0000003F))			/* 6'b0; 0x4100_80C0[5:0]; sec */
#define RTCIO_BKUP_TIME_MIN(x)					((u32)((x >> 6) & 0x0000003F))			/* 6'b0; 0x4100_80C0[11:6]; min */
#define RTCIO_BKUP_TIME_HOUR(x)					((u32)((x >> 12) & 0x0000001F))			/* 5'b0; 0x4100_80C0[16:12]; hour */
#define RTCIO_BKUP_TIME_DAY(x)					((u32)((x >> 17) & 0x000001FF))			/* 9'b0; 0x4100_80C0[25:17]; day */
#define RTCIO_BKUP_TIME_YEAR_L(x)				((u32)((x >> 26) & 0x0000003F))			/* 6'b0; 0x4100_80C0[31:26]; year_l */
#define RTCIO_BKUP_TIME_YEAR_H(x)				((u32)((x >> 0) & 0x00000003))			/* 2'b0; 0x4100_80C4[1:0]; year_h */
#define RTCIO_BKUP_TIME_YEAR(yearh, yearl)		((u32)((yearh << 6) | yearl))			/* 8'b0; 0x4100_80C0[33:26] */

#define RTCIO_CNT_SECS_L(x)						((u32)((x >> 27) & 0x0000001F))			/* 5'b0; 0x4100_80C4[31:27] << 0x4100_80C0[63:59] */
#define RTCIO_CNT_SECS_H(x)						((u32)((x >> 0) & 0x000FFFFF))			/* 20'b0; 0x4100_80C8[19:0] << 0x4100_80C0[83:64] */
#define RTCIO_CNT_SECS(sech, secl)				((u32)((sech << 5) | secl))				/* 25'b0; 0x4100_80C0[83:59] */

/*
4. Calc 0.5s -> carryflag
1).0x4100_80C0[83:42] records Counter in pwr down period, unit second;
2).RTCIO counter clock is 131.072KHz
  1.0s ->0x2_0000[bit17];
  0.5s -> 0x1_0000[bit16];
3).bit16 in 0x4100_80C0[83:42] is bit58; Equal 0x4100_80C4[26]
*/
#define RTCIO_CNT_SECS_0P5(x)					((u32)((x >> 26) & 0x00000001))			/* 1'b0; 0x4100_80C4[26] << 0x4100_80C0[58] */
#define RTCIO_CNT_SECS_OVER_0P5(x)				(x ? 1 : 0) 							/* 1: no less than 0.5s -> need carry;0 less than 0.5s -> ignore */
#define RTCIO_MASK_CNT_SECS_FRACTION			((u32)0x0001FFFF << 10)					/* 17'b0; 0x4100_80C4[26:10] << 0x4100_80C0[58:42]*/
/* end group RTC_IO_test_dout */

/* brief get RTC time format for converting after re-power on */
#define RTCIO_CHECK_RTC_PREDIV_ASYNC(PREDIV_A)	((PREDIV_A) <= 0x7F)
#define RTCIO_GET_RTC_PREDIV_ASYNC(x)			((u32)((x >> 16) & 0x000001FF))			/* 9'b0; 0x4100_8A0C[24:16] */

#define RTCIO_GET_RTC_HOUR_FMT_CTRL(x)			((u32)((x >> 7) & 0x00000001))			/* CR bit7; 0: 24 hour format; 1: AM/PM hour format */
#define RTCIO_GET_RTC_HOUR_FMT_NOTIFY(x)		((u32)((x >> 22) & 0x00000001))			/* TR bit22; 0: AM or 24-hour format; 1: PM format*/

#define RTC_HOUR_FMT_CTRL_24H					(0)										/* Ctrl: 24 hour format*/
#define RTC_HOUR_FMT_CTRL_PMAM					(1)										/* Ctrl: AM/PM hour format*/

#define RTC_HOUR_FMT_NOTIFY_24H_AM				(0)										/* Notify: AM or 24-hour */
#define RTC_HOUR_FMT_NOTIFY_PM					(1)										/* Notify: PM*/


_LONG_CALL_ void RTCIO_SetWorkMode(u32 Mode);
_LONG_CALL_ u32 RTCIO_GetWorkMode(void);
_LONG_CALL_ u32 RTCIO_IsEnabled(void);
//_LONG_CALL_ static u32 RTCIO_CheckRTCPredivAsync(void);
//_LONG_CALL_ void RTCIO_ShiftOutTimeInfo(RTCIO_TimeInfo *pDataOut);
_LONG_CALL_ void RTCIO_SetRValue(u32 ResetRcal);
_LONG_CALL_ void RTCIO_ModeRTCStart(void);
_LONG_CALL_ void RTCIO_GetTimeInfo(RTCIO_TimeInfo *pDataOut);

/* MANUAL_GEN_END */

#endif


