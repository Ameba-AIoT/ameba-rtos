/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "os_wrapper.h"
#include <stdio.h>

#define ACFREQ_IS_50	1 // 0: 60Hz; 1: 50Hz
#define CTC_EN_CH		3 // Enabled channel numbers
#define IIR_ORDER		1 // IIR filter order
#define CTC_DATA_DBG	0 // debug ctc data or not

#if ACFREQ_IS_50
#define SCAN_PERIOD		5 // 5ms
#define PBUF_NUM		(20 / SCAN_PERIOD) // 50Hz: 20ms
#define SAMPLE_CNT		0
#define SAMPLE_NUM		BIT(SAMPLE_CNT + 2) // 2^(SAMPLE_CNT+2) points each sample
#else
#define SCAN_PERIOD		4 // 4ms
#define PBUF_NUM		(16 / SCAN_PERIOD) // 60Hz: 16.7ms
#define SAMPLE_CNT		0
#define SAMPLE_NUM		BIT(SAMPLE_CNT + 2) // 2^(SAMPLE_CNT+2) points each sample
#endif

/* update baseline by sw */
/* check baseline every 5 cycles and each cycle consists of 4 scan intervals */
#define BASE_UPD_CYCLE	5
/* update baseline when diff or accumulated diff is over threshold */
#define BASE_UPD_THRE	4
/* baseline update step value */
#define BASE_UPD_STEP	2

/* debounce */
#define CTC_DBC_CNT		2

/* n_noise timing */
#define NNOISE_RST_LMT	400

#define ABS(a,b) ((a) > (b) ? (a) - (b):(b) - (a))

#if CTC_DATA_DBG
#define CTC_DBG_SZ	1024
#endif

enum ctc_event_type {
	NO_EVENT,
	CH_PRESSED,
	CH_RELEASED,
	P_NOISE,
	N_NOISE,
};

typedef struct {
	u16 raw_buf[2][PBUF_NUM];
	u16 last_data[2];
	double iir_x[IIR_ORDER + 1]; // IIR filter input
	double iir_y[IIR_ORDER + 1]; // IIR filter output
	u32 buf_index;
	u16 ctc_base; // baseline
	u16 ctc_thre; // touch threshold
	u16 ctc_flt_data; // filtered data
	s16 ctc_acc_diff; // accumulated diff value
	u8 ctc_ch_idx;
	u8 ctc_cycle_cnt; // baseline update cycle count
	u8 ctc_press_cnt;
	u8 ctc_release_cnt;
	u16 ctc_nnoise_cnt;
	BOOL ctc_is_pressed;
#if CTC_DATA_DBG
	u16 *praw_data;
	u16 *pmed_data;
	u16 *piir_data;
	u16 *pavg_data;
	u16 *pbase_data;
	u16 raw_idx;
	u16 med_idx;
	u16 iir_idx;
	u16 avg_idx;
	u16 base_idx;
#endif
} CTC_FLT_Typedef, *PCTC_FLT_Typedef;

CTC_FLT_Typedef ctc_flt[CTC_EN_CH];

u32 buffer_switch = 0;
static rtos_sema_t ctc_sema = NULL;

#if CTC_DATA_DBG
u16 ctc_raw_data[CTC_EN_CH][CTC_DBG_SZ];
u16 ctc_med_data[CTC_EN_CH][CTC_DBG_SZ];
u16 ctc_iir_data[CTC_EN_CH][CTC_DBG_SZ];
u16 ctc_avg_data[CTC_EN_CH][CTC_DBG_SZ / PBUF_NUM];
u16 ctc_base_data[CTC_EN_CH][CTC_DBG_SZ / PBUF_NUM];
#endif

/* NOTE: adjust your parameters here!!! */
const CapTouch_CHInitTypeDef ctc_ch_config[CT_CHANNEL_NUM] = {
	/*DiffThreshold, MbiasCurrent, ETCNNoiseThr, ETCPNoiseThr, CHEnable*/
	{400,			0x0C,			200,			200,			ENABLE}, /* Channel 0 */
	{400,			0x0C,			200,			200,			DISABLE}, /* Channel 1 */
	{400,			0x0C,			200,			200,			ENABLE}, /* Channel 2 */
	{400,			0x0C,			200,			200,			DISABLE}, /* Channel 3 */
	{400,			0x0C,			200,			200,			ENABLE}, /* Channel 4 */
	{400,			0x0C,			200,			200,			DISABLE}, /* Channel 5 */
	{400,			0x0C,			200,			200,			DISABLE}, /* Channel 6 */
	{400,			0x0C,			200,			200,			DISABLE}, /* Channel 7 */
	{400,			0x0C,			200,			200,			DISABLE}, /* Channel 8: guard */
};

const u8 ctc_ch_pad[CT_CHANNEL_NUM] = {
	CTC_CH0_PIN, // Touch0
	CTC_CH1_PIN, // Touch1
	CTC_CH2_PIN, // Touch2
	CTC_CH3_PIN, // Touch3
	CTC_CH4_PIN, // Touch4
	CTC_CH5_PIN, // Touch5
	CTC_CH6_PIN, // Touch6
	CTC_CH7_PIN, // Touch7
	CTC_CH8_PIN  // Touch8
};

void ctc_med_filter(u16 *ctc_data, u16 last0, u16 last1)
{
	u16 raw_data[PBUF_NUM + 2];
	u16 med_data[PBUF_NUM];
	u32 i;

	raw_data[0] = last0;
	raw_data[1] = last1;

	_memcpy((void *)(&raw_data[2]), (void *)ctc_data, sizeof(u16) * PBUF_NUM);

	for (i = 1; i < PBUF_NUM + 1; i++) {
		if (raw_data[i - 1] > raw_data[i]) {
			if (raw_data[i] > raw_data[i + 1]) {
				med_data[i - 1] = raw_data[i];
			} else if (raw_data[i - 1] > raw_data[i + 1]) {
				med_data[i - 1] = raw_data[i + 1];
			} else {
				med_data[i - 1] = raw_data[i - 1];
			}
		} else {
			if (raw_data[i] < raw_data[i + 1]) {
				med_data[i - 1] = raw_data[i];
			} else if (raw_data[i - 1] < raw_data[i + 1]) {
				med_data[i - 1] = raw_data[i + 1];
			} else {
				med_data[i - 1] = raw_data[i - 1];
			}
		}
	}

	_memcpy((void *)ctc_data, (void *)med_data, sizeof(u16) * PBUF_NUM);
}

void ctc_iir_filter(PCTC_FLT_Typedef pctc_flt, double n_div)
{
	u32 i, j;
	u16 *ctc_data = (u16 *)(pctc_flt->raw_buf[1 - buffer_switch % 2]);

	double iir_tmp = 0;
	// const double iir_b[IIR_ORDER + 1] = {1.09180311e-04, 7.12218554e-05, 7.12218554e-05, 1.09180311e-04};
	// const double iir_a[IIR_ORDER + 1] = {1.0, -2.85425833, 2.71894454, -0.86432541};

	double a = 1.0 / n_div;
	const double iir_b[IIR_ORDER + 1] = {a, 0};
	const double iir_a[IIR_ORDER + 1] = {1.0, a - 1.0};

	for (i = 0; i < PBUF_NUM; i++) {
		/* update x and y */
		for (j = IIR_ORDER; j > 0; j--) {
			pctc_flt->iir_x[j] = pctc_flt->iir_x[j - 1];
			pctc_flt->iir_y[j] = pctc_flt->iir_y[j - 1];
		}
		pctc_flt->iir_x[0] = ctc_data[i];

		/* calculate current y */
		for (j = 0; j < IIR_ORDER + 1; j++) {
			iir_tmp += iir_b[j] * pctc_flt->iir_x[j];
			if (j != 0) {
				iir_tmp -= iir_a[j] * pctc_flt->iir_y[j];
			}
		}
		iir_tmp /= iir_a[0];

		/* update x and y */
		ctc_data[i] = (u16)iir_tmp;
		pctc_flt->iir_y[0] = iir_tmp;
		iir_tmp = 0;
	}
}

u16 ctc_ave_filter(u16 *ctc_data)
{
	u32 i;
	u32 sum = 0;

	for (i = 0; i < PBUF_NUM; i++) {
		sum += ctc_data[i];
	}

	return (sum / PBUF_NUM);
}

u16 ctc_sw_filter(PCTC_FLT_Typedef pctc_flt)
{
	u32 buf_sel = 1 - buffer_switch % 2;
	u16 last_temp0, last_temp1;

	/* backup last 2 data */
	last_temp0 = pctc_flt->raw_buf[buf_sel][PBUF_NUM - 2];
	last_temp1 = pctc_flt->raw_buf[buf_sel][PBUF_NUM - 1];

#if CTC_DATA_DBG
	if (pctc_flt->raw_idx < CTC_DBG_SZ) {
		_memcpy((void *)(pctc_flt->praw_data + pctc_flt->raw_idx), pctc_flt->raw_buf[buf_sel], PBUF_NUM * sizeof(u16));
		pctc_flt->raw_idx += PBUF_NUM;
	}
#endif
	/* step1: median filter */
	ctc_med_filter((u16 *)(pctc_flt->raw_buf[buf_sel]), pctc_flt->last_data[0], pctc_flt->last_data[1]);

#if CTC_DATA_DBG
	if (pctc_flt->med_idx < CTC_DBG_SZ) {
		_memcpy((void *)(pctc_flt->pmed_data + pctc_flt->med_idx), pctc_flt->raw_buf[buf_sel], PBUF_NUM * sizeof(u16));
		pctc_flt->med_idx += PBUF_NUM;
	}
#endif

	/* update last 2 data */
	pctc_flt->last_data[0] = last_temp0;
	pctc_flt->last_data[1] = last_temp1;

	/* step2: iir lp filter */
	if (pctc_flt->ctc_nnoise_cnt > 0) {
		ctc_iir_filter(pctc_flt, 16.0);
	} else {
		ctc_iir_filter(pctc_flt, 64.0);
	}

#if CTC_DATA_DBG
	if (pctc_flt->iir_idx < CTC_DBG_SZ) {
		_memcpy((void *)(pctc_flt->piir_data + pctc_flt->iir_idx), pctc_flt->raw_buf[buf_sel], PBUF_NUM * sizeof(u16));
		pctc_flt->iir_idx += PBUF_NUM;
	}
#endif

	/* step3: average filter */
	return ctc_ave_filter((u16 *)(pctc_flt->raw_buf[buf_sel]));
}

void ctc_base_init(PCTC_FLT_Typedef pctc_flt)
{
	u32 i, j;

	RTK_LOGS(NOTAG, ">>>> %s\n", __FUNCTION__);

	/* reset captouch baseline by sw */
	for (i = 0; i < CTC_EN_CH; i++) {
		pctc_flt[i].ctc_base = pctc_flt[i].ctc_flt_data;
		pctc_flt[i].last_data[0] = pctc_flt[i].ctc_base;
		pctc_flt[i].last_data[1] = pctc_flt[i].ctc_base;
		for (j = 0; j < IIR_ORDER + 1; j++) {
			pctc_flt[i].iir_x[j] = pctc_flt[i].ctc_base;
			pctc_flt[i].iir_y[j] = pctc_flt[i].ctc_base;
		}
		pctc_flt[i].ctc_acc_diff = 0;
		pctc_flt[i].ctc_cycle_cnt = 0;
		pctc_flt[i].ctc_press_cnt = 0;
		pctc_flt[i].ctc_release_cnt = 0;
		pctc_flt[i].ctc_nnoise_cnt = 0;
	}
}

void ctc_press_entry(PCTC_FLT_Typedef pctc_flt)
{
	// TODO
	RTK_LOGS(NOTAG, "CH%d pressed.\n", pctc_flt->ctc_ch_idx);
}

void ctc_release_entry(PCTC_FLT_Typedef pctc_flt)
{
	// TODO
	RTK_LOGS(NOTAG, "CH%d released.\n", pctc_flt->ctc_ch_idx);
}

void ctc_pnoise_entry(PCTC_FLT_Typedef pctc_flt)
{
	// TODO
	RTK_LOGS(NOTAG, "CH%d P_Noise.\n", pctc_flt->ctc_ch_idx);
	DelayMs(1);
	ctc_base_init(ctc_flt);
}

void ctc_nnoise_entry(PCTC_FLT_Typedef pctc_flt)
{
	// TODO
	// RTK_LOGS(NOTAG, "CH%d N_Noise %d.\n", pctc_flt->ctc_ch_idx, pctc_flt->ctc_nnoise_cnt);

	if (++pctc_flt->ctc_nnoise_cnt >= NNOISE_RST_LMT) {
		ctc_base_init(ctc_flt);
	}
}

u32 ctc_irq_handler(void *para)
{
	(void)para;

	u8 i, j;
	u32 IntStatus, IntStatus0;
	static u32 fifo_read_cnt = 0;

	IntStatus = CapTouch_GetISR(CAPTOUCH_DEV);
	IntStatus0 = CAPTOUCH_DEV->CT_ECO_USE0;

	if (IntStatus0 & CT_BIT_SCAN_END_INTR) {
		for (i = 0; i < CTC_EN_CH; i++) {
			ctc_flt[i].raw_buf[buffer_switch % 2][ctc_flt[i].buf_index++] = CapTouch_GetChAveData(CAPTOUCH_DEV, ctc_flt[i].ctc_ch_idx);
		}

		if (++fifo_read_cnt == PBUF_NUM) {
			fifo_read_cnt = 0;
			for (i = 0; i < CTC_EN_CH; i++) {
				/* Not get sufficient data: clear all channel buffer */
				if (ctc_flt[i].buf_index != PBUF_NUM) {
					RTK_LOGS(NOTAG, "=========Corrupted FIFO=============\n");
					for (j = 0; j < CTC_EN_CH; j++) {
						_memset((void *) & (ctc_flt[j].raw_buf[buffer_switch % 2][0]), 0x0, 2 * PBUF_NUM);
						ctc_flt[j].buf_index = 0;
					}
					goto exit;
				}
			}

			/* Get sufficient data: switch to sw_filter task */
			for (i = 0; i < CTC_EN_CH; i++) {
				ctc_flt[i].buf_index = 0;
			}
			buffer_switch++;

			rtos_sema_give(ctc_sema);
		}
		CapTouch_INTClearPendingBit(CAPTOUCH_DEV, CT_BIT_SCAN_END_CLR);
		return 0;
	}

	if (IntStatus & CT_BIT_AFIFO_OVERFLOW_INTR) {
		RTK_LOGS(NOTAG, "CT_BIT_AFIFO_OVERFLOW_INTR \n");
	}

exit:
	CapTouch_INTClearPendingBit(CAPTOUCH_DEV, IntStatus);
	return 0;
}

void ctc_board_init(void)
{
	u8 ctc_ch[CTC_EN_CH];
	u32 i, j, ch_idx = 0;
	CapTouch_InitTypeDef CapTouch_InitStruct;

#if CTC_DATA_DBG
	_memset((void *)ctc_raw_data, 0x00, CTC_EN_CH * CTC_DBG_SZ * sizeof(u16));
	_memset((void *)ctc_med_data, 0x00, CTC_EN_CH * CTC_DBG_SZ * sizeof(u16));
	_memset((void *)ctc_iir_data, 0x00, CTC_EN_CH * CTC_DBG_SZ * sizeof(u16));
	_memset((void *)ctc_avg_data, 0x00, CTC_EN_CH * CTC_DBG_SZ * sizeof(u16) / PBUF_NUM);
	_memset((void *)ctc_base_data, 0x00, CTC_EN_CH * CTC_DBG_SZ * sizeof(u16) / PBUF_NUM);
#endif

	RCC_PeriphClockCmd(APBPeriph_CTC, APBPeriph_CTC_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_ADC, APBPeriph_ADC_CLOCK, ENABLE);

	for (i = 0; i < CT_CHANNEL_NUM; i++) {
		if (ctc_ch_config[i].CT_CHEnable == ENABLE) {
			ctc_ch[ch_idx++] = i;
			PAD_PullCtrl(ctc_ch_pad[i], GPIO_PuPd_NOPULL);
			PAD_InputCtrl(ctc_ch_pad[i], DISABLE);
			Pinmux_Config(ctc_ch_pad[i], PINMUX_FUNCTION_CAPTOUCH);
		}
	}

	if (ch_idx != CTC_EN_CH) {
		RTK_LOGS(NOTAG, "\nWarning:\n");
		RTK_LOGS(NOTAG, "\tPlease modify CTC_EN_CH to match enabled chan num!!!\n");
		assert_param(0);
	}

	/* init CTC */
	CapTouch_StructInit(&CapTouch_InitStruct);

	_memcpy((void *)CapTouch_InitStruct.CT_Channel, (void *)ctc_ch_config, CT_CHANNEL_NUM * sizeof(CapTouch_CHInitTypeDef));

	CapTouch_InitStruct.CT_ScanInterval = SCAN_PERIOD; // 5ms
	CapTouch_InitStruct.CT_SampleCnt = SAMPLE_CNT; // 2^2=4/131072 --> 31us, SAMPLE_NUM = 4
	CapTouch_Init(CAPTOUCH_DEV, &CapTouch_InitStruct);

	InterruptRegister((IRQ_FUN) ctc_irq_handler, CTOUCH_IRQ, (u32)NULL, INT_PRI_MIDDLE);
	InterruptEn(CTOUCH_IRQ, INT_PRI_MIDDLE);

	CapTouch_Cmd(CAPTOUCH_DEV, ENABLE);
	RTK_LOGS(NOTAG, "CapTouch init done\r\n");

	/* delay_etc = (etc_interval + 1) * scan_period */
	/* default: etc_interval = 3 */
	DelayMs(4 * SCAN_PERIOD);

	for (i = 0; i < CTC_EN_CH; i++) {
		ctc_flt[i].ctc_base = CapTouch_GetChBaseline(CAPTOUCH_DEV, ctc_ch[i]);
		ctc_flt[i].last_data[0] = ctc_flt[i].ctc_base;
		ctc_flt[i].last_data[1] = ctc_flt[i].ctc_base;
		ctc_flt[i].ctc_flt_data = ctc_flt[i].ctc_base;
		for (j = 0; j < IIR_ORDER + 1; j++) {
			ctc_flt[i].iir_x[j] = ctc_flt[i].ctc_base;
			ctc_flt[i].iir_y[j] = ctc_flt[i].ctc_base;
		}
		ctc_flt[i].ctc_thre = CT_GET_CHx_D_TOUCH_TH(CAPTOUCH_DEV->CT_CH[ctc_ch[i]].CT_CHx_CTRL);
		ctc_flt[i].ctc_ch_idx = ctc_ch[i];
		RTK_LOGS(NOTAG, "[%d]BASE %d\n", ctc_flt[i].ctc_ch_idx, ctc_flt[i].ctc_base);
		RTK_LOGS(NOTAG, "[%d]THRE %d\n", ctc_flt[i].ctc_ch_idx, ctc_flt[i].ctc_thre);
		ctc_flt[i].ctc_acc_diff = 0;
		ctc_flt[i].ctc_cycle_cnt = 0;
		ctc_flt[i].ctc_press_cnt = 0;
		ctc_flt[i].ctc_release_cnt = 0;
		ctc_flt[i].ctc_nnoise_cnt = 0;
#if CTC_DATA_DBG
		ctc_flt[i].praw_data = (u16 *)ctc_raw_data[i];
		ctc_flt[i].pmed_data = (u16 *)ctc_med_data[i];
		ctc_flt[i].piir_data = (u16 *)ctc_iir_data[i];
		ctc_flt[i].pavg_data = (u16 *)ctc_avg_data[i];
		ctc_flt[i].pbase_data = (u16 *)ctc_base_data[i];

		ctc_flt[i].raw_idx = 0;
		ctc_flt[i].med_idx = 0;
		ctc_flt[i].iir_idx = 0;
		ctc_flt[i].avg_idx = 0;
		ctc_flt[i].base_idx = 0;

		RTK_LOGS(NOTAG, "[%d][0x%x-0x%x]raw_data\n", ctc_flt[i].ctc_ch_idx, (u32)ctc_raw_data[i], (u32)ctc_raw_data[i] + CTC_DBG_SZ * sizeof(u16));
		RTK_LOGS(NOTAG, "[%d][0x%x-0x%x]med_data\n", ctc_flt[i].ctc_ch_idx, (u32)ctc_med_data[i], (u32)ctc_med_data[i] + CTC_DBG_SZ * sizeof(u16));
		RTK_LOGS(NOTAG, "[%d][0x%x-0x%x]iir_data\n", ctc_flt[i].ctc_ch_idx, (u32)ctc_iir_data[i], (u32)ctc_iir_data[i] + CTC_DBG_SZ * sizeof(u16));
		RTK_LOGS(NOTAG, "[%d][0x%x-0x%x]avg_data\n", ctc_flt[i].ctc_ch_idx, (u32)ctc_avg_data[i], (u32)ctc_avg_data[i] + CTC_DBG_SZ * sizeof(u16) / PBUF_NUM);
		RTK_LOGS(NOTAG, "[%d][0x%x-0x%x]base_data\n", ctc_flt[i].ctc_ch_idx, (u32)ctc_base_data[i], (u32)ctc_base_data[i] + CTC_DBG_SZ * sizeof(u16) / PBUF_NUM);
#endif
	}

	/* enable SCAN_END interrupt */
	/* disable PRESS/RELEASE interrupt */
	CapTouch_INTConfig(CAPTOUCH_DEV, CT_BIT_SCAN_END_INTR_EN | CT_BIT_AFIFO_OVERFLOW_INTR_EN, ENABLE);
}

u32 ctc_event_detect(PCTC_FLT_Typedef pctc_flt)
{
	u16 data_diff = 0;
	BOOL larger_data = _TRUE;

	if (pctc_flt->ctc_flt_data >= pctc_flt->ctc_base) {
		larger_data = _TRUE;
	} else {
		larger_data = _FALSE;
	}
	data_diff = ABS(pctc_flt->ctc_flt_data, pctc_flt->ctc_base);

	if (!pctc_flt->ctc_is_pressed) {
		if (!larger_data) {
			if (data_diff > pctc_flt->ctc_thre) {
//				RTK_LOGS(NOTAG, "p%d\n", pctc_flt->ctc_press_cnt);
				if (++pctc_flt->ctc_press_cnt >= CTC_DBC_CNT) {
					pctc_flt->ctc_is_pressed = _TRUE;
					pctc_flt->ctc_press_cnt = 0;
					pctc_flt->ctc_release_cnt = 0;
					pctc_flt->ctc_nnoise_cnt = 0;
					return CH_PRESSED;
				}
			} else if (data_diff > ctc_ch_config[pctc_flt->ctc_ch_idx].CT_ETCNNoiseThr) {
				return N_NOISE;
			}
		} else if (data_diff > ctc_ch_config[pctc_flt->ctc_ch_idx].CT_ETCPNoiseThr) {
			pctc_flt->ctc_nnoise_cnt = 0;
			return P_NOISE;
		}
	} else if (data_diff <= pctc_flt->ctc_thre) {
//		RTK_LOGS(NOTAG, "r%d\n", pctc_flt->ctc_release_cnt);
		if (++pctc_flt->ctc_release_cnt >= CTC_DBC_CNT) {
			pctc_flt->ctc_is_pressed = _FALSE;
			pctc_flt->ctc_press_cnt = 0;
			pctc_flt->ctc_release_cnt = 0;
			pctc_flt->ctc_nnoise_cnt = 0;
			return CH_RELEASED;
		}
	}

	pctc_flt->ctc_nnoise_cnt = 0;
	return NO_EVENT;
}

void ctc_base_update(PCTC_FLT_Typedef pctc_flt)
{
	u16 data_diff;
	BOOL larger_data = _TRUE;

	if (++pctc_flt->ctc_cycle_cnt < BASE_UPD_CYCLE) {
		return;
	} else {
		pctc_flt->ctc_cycle_cnt = 0;
	}

	if (pctc_flt->ctc_flt_data >= pctc_flt->ctc_base) {
		larger_data = _TRUE;
	} else {
		larger_data = _FALSE;
	}
	data_diff = ABS(pctc_flt->ctc_flt_data, pctc_flt->ctc_base);

	/* update accumulated diff value */
	pctc_flt->ctc_acc_diff += (pctc_flt->ctc_flt_data - pctc_flt->ctc_base);

	if ((!larger_data && data_diff < ctc_ch_config[pctc_flt->ctc_ch_idx].CT_ETCNNoiseThr) || \
		(larger_data && data_diff < ctc_ch_config[pctc_flt->ctc_ch_idx].CT_ETCPNoiseThr)) {
		if (data_diff > BASE_UPD_THRE || ABS(pctc_flt->ctc_acc_diff, 0) > BASE_UPD_THRE) {
			/* 1.update baseline with step value */
			if (pctc_flt->ctc_flt_data >= pctc_flt->ctc_base) {
				pctc_flt->ctc_base += BASE_UPD_STEP;
			} else {
				pctc_flt->ctc_base -= BASE_UPD_STEP;
			}
			/* 2.clear accumulated diff value */
			pctc_flt->ctc_acc_diff = 0;
		}
	} else {
		pctc_flt->ctc_acc_diff = 0;
	}
}

void ctc_filter_task(void)
{
	u32 i;
	u32 ctc_event;

	ctc_board_init();

	while (1) {
		if (SUCCESS == rtos_sema_take(ctc_sema, RTOS_MAX_DELAY)) {
			for (i = 0; i < CTC_EN_CH; i++) {
				ctc_flt[i].ctc_flt_data = ctc_sw_filter(&ctc_flt[i]);
#if CTC_DATA_DBG
				if (ctc_flt[i].avg_idx < CTC_DBG_SZ / PBUF_NUM) {
					*(ctc_flt[i].pavg_data + ctc_flt[i].avg_idx) = ctc_flt[i].ctc_flt_data;
					ctc_flt[i].avg_idx ++;
				}
#endif
				ctc_event = ctc_event_detect(&ctc_flt[i]);

				switch (ctc_event) {
				case CH_PRESSED:
					ctc_press_entry(&ctc_flt[i]);
					break;
				case CH_RELEASED:
					ctc_release_entry(&ctc_flt[i]);
					break;
				case P_NOISE:
					ctc_pnoise_entry(&ctc_flt[i]);
					break;
				case N_NOISE:
					ctc_nnoise_entry(&ctc_flt[i]);
					break;
				case NO_EVENT:
				default:
					break;
				}
#if CTC_DATA_DBG
				if (ctc_flt[i].base_idx < CTC_DBG_SZ / PBUF_NUM) {
					*(ctc_flt[i].pbase_data + ctc_flt[i].base_idx) = ctc_flt[i].ctc_base;
					ctc_flt[i].base_idx ++;
				}
#else
				RTK_LOGS(NOTAG, "CH[%d]%d-%d: %d\n", ctc_flt[i].ctc_ch_idx, ctc_flt[i].ctc_base, ctc_flt[i].ctc_flt_data, \
						 ctc_flt[i].ctc_base - ctc_flt[i].ctc_flt_data);
#endif
				ctc_base_update(&ctc_flt[i]);
			}
		}
	}
}

int main(void)
{
	if (ctc_sema == NULL) {
		rtos_sema_create_binary(&ctc_sema);
	}

	if (rtos_task_create(NULL, "CTC_FILTER_DEMO", (rtos_task_t)ctc_filter_task, NULL, 2048, (1)) != SUCCESS) {
		RTK_LOGS(NOTAG, "Cannot create CTC_FILTER_DEMO\r\n");
	}

	rtos_sched_start();

	return 0;
}
