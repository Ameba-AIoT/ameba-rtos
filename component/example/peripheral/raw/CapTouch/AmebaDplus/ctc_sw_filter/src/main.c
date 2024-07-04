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

/* NOTE: adjust your parameters here!!! */
const CapTouch_CHInitTypeDef ctc_ch_config[CT_CHANNEL_NUM] = {
	/*DiffThreshold, MbiasCurrent, ETCNNoiseThr, ETCPNoiseThr, CHEnable*/
	{800,		0x0C,		400,		400,		ENABLE}, /* Channel 0 */
	{800,		0x0C,		400,		400,		DISABLE}, /* Channel 1 */
	{800,		0x0C,		400,		400,		DISABLE}, /* Channel 2 */
	{800,		0x0C,		400,		400,		DISABLE}, /* Channel 3 */
};

const u8 ctc_ch_pad[CT_CHANNEL_NUM] = {
	CTC_CH0_PIN, // Touch0
	CTC_CH1_PIN, // Touch1
	CTC_CH2_PIN, // Touch2
	CTC_CH3_PIN, // Touch3
};

u32 ctc_irq_handler(void *para)
{
	(void)para;

	u8 i;
	u32 IntStatus = CapTouch_GetISR(CAPTOUCH_DEV);

	for (i = 0; i < CT_CHANNEL_NUM; i++) {
		if (IntStatus & CT_CHX_PRESS_INT(i)) {
			RTK_LOGS(NOTAG, "Key %d pressed \n", i);
		} else if (IntStatus & CT_CHX_RELEASE_INT(i)) {
			RTK_LOGS(NOTAG, "Key %d released \n", i);
		}
	}

	if (IntStatus & CT_BIT_OVER_N_NOISE_TH_INTR) {
		RTK_LOGS(NOTAG, "CT_BIT_OVER_N_NOISE_TH_INTR \n");
	}

	if (IntStatus & CT_BIT_OVER_P_NOISE_TH_INTR) {
		RTK_LOGS(NOTAG, "CT_BIT_OVER_P_NOISE_TH_INTR \n");
	}

	CapTouch_INTClearPendingBit(CAPTOUCH_DEV, IntStatus);

	return 0;
}

void ctc_filter_task(void)
{
	CapTouch_InitTypeDef CapTouch_InitStruct;
	u32 i;
	u32 mdec;

	RCC_PeriphClockCmd(APBPeriph_ADC, APBPeriph_ADC_CLOCK, ENABLE);

	for (i = 0; i < CT_CHANNEL_NUM; i++) {
		if (ENABLE == ctc_ch_config[i].CT_CHEnable) {
			Pinmux_Config(ctc_ch_pad[i], PINMUX_FUNCTION_CAP_TOUCH);
			PAD_PullCtrl(ctc_ch_pad[i], GPIO_PuPd_NOPULL);
			PAD_SleepPullCtrl(ctc_ch_pad[i], GPIO_PuPd_NOPULL);
			PAD_InputCtrl(ctc_ch_pad[i], DISABLE);
		}
	}

	/* init CTC */
	CapTouch_StructInit(&CapTouch_InitStruct);

	_memcpy((void *)CapTouch_InitStruct.CT_Channel, (void *)ctc_ch_config, CT_CHANNEL_NUM * sizeof(CapTouch_CHInitTypeDef));

	CapTouch_Init(CAPTOUCH_DEV, &CapTouch_InitStruct);

	/* set sample clock: xtal4m/24 */
	CapTouch_SetClkPara(CAPTOUCH_DEV, CTC_CLK_XTAL, CTC_CLK_DIV24);

	/* get and set decimation factor of decimation filter */
#if ACFREQ_IS_50
	mdec = CapTouch_GetMDec(CAPTOUCH_DEV, 50);
#else
	mdec = CapTouch_GetMDec(CAPTOUCH_DEV, 60);
#endif
	CapTouch_SetMDec(CAPTOUCH_DEV, mdec);

	/* enable iir filter */
	CapTouch_FilterIIRCmd(CAPTOUCH_DEV, ENABLE);
	/* enable filter */
	CapTouch_FilterCmd(CAPTOUCH_DEV, ENABLE);

	CapTouch_INTConfig(CAPTOUCH_DEV, CT_ALL_INT_EN, ENABLE);
	/* Data will be sent to FIFO automatically even when debug mode is off since BCut */
	CapTouch_INTConfig(CAPTOUCH_DEV, CT_BIT_AFIFO_OVERLVL_INTR_EN, DISABLE);
	CapTouch_INTConfig(CAPTOUCH_DEV, CT_BIT_AFIFO_OVERFLOW_INTR_EN, DISABLE);

	InterruptRegister((IRQ_FUN) ctc_irq_handler, CTOUCH_IRQ, (u32)NULL, INT_PRI_MIDDLE);
	InterruptEn(CTOUCH_IRQ, INT_PRI_MIDDLE);

	CapTouch_Cmd(CAPTOUCH_DEV, ENABLE);

	RTK_LOGS(NOTAG, "CapTouch init done\n");

	while (1) {
		rtos_task_delete(NULL);
	}
}

int main(void)
{
	if (rtos_task_create(NULL, "CTC_FILTER_DEMO", (rtos_task_t)ctc_filter_task, NULL, 2048, (1)) != SUCCESS) {
		RTK_LOGS(NOTAG, "Cannot create CTC_FILTER_DEMO\r\n");
	}

	rtos_sched_start();

	return 0;
}
