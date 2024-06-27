/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "captouch_ext.h"
#include "os_wrapper.h"
#include <stdio.h>

#define CTC_DATA_DBG	1

#define TIM_IDX			1
#define TIM_PERIOD		8000

/* NOTE: adjust your parameters here!!! */
const CapTouch_CHInitTypeDef CTC_Ch_Config[CT_CHANNEL_NUM] = {
	/*DiffThreshold, MbiasCurrent, ETCNNoiseThr, ETCPNoiseThr, CHEnable*/
	{800,           0x0C,           400,           400,           ENABLE}, /* Channel 0 */
	{800,           0x0C,           400,           400,           DISABLE}, /* Channel 1 */
	{800,           0x0C,           400,           400,           DISABLE}, /* Channel 2 */
	{800,           0x0C,           400,           400,           DISABLE}, /* Channel 3 */
#if defined (CONFIG_AMEBASMART) || defined (CONFIG_AMEBALITE)
	{800,           0x0C,           400,           400,           DISABLE}, /* Channel 4 */
	{800,           0x0C,           400,           400,           DISABLE}, /* Channel 5 */
	{800,           0x0C,           400,           400,           DISABLE}, /* Channel 6 */
	{800,           0x0C,           400,           400,           DISABLE}, /* Channel 7 */
	{800,           0x0C,           400,           400,           DISABLE}, /* Channel 8: guard */
#endif
};

u8 CTC_PAD[CT_CHANNEL_NUM] = {
	CTC_CH0_PIN, // Touch0
	CTC_CH1_PIN, // Touch1
	CTC_CH2_PIN, // Touch2
	CTC_CH3_PIN, // Touch3
#if defined (CONFIG_AMEBASMART) || defined (CONFIG_AMEBALITE)
	CTC_CH4_PIN, // Touch4
	CTC_CH5_PIN, // Touch5
	CTC_CH6_PIN, // Touch6
	CTC_CH7_PIN, // Touch7
	CTC_CH8_PIN  // Touch8
#endif
};

u32 CapTouch_irq_handler(void *para)
{
	(void)para;

	u8 i;
	u32 IntStatus;
	IntStatus = CapTouch_GetISR(CAPTOUCH_DEV);

#if defined (CONFIG_AMEBASMART) || defined (CONFIG_AMEBALITE)
	for (i = 0; i < CT_CHANNEL_NUM - 1; i++) {
#elif defined (CONFIG_AMEBADPLUS)
	for (i = 0; i < CT_CHANNEL_NUM; i++) {
#endif
		if (IntStatus & CT_CHX_PRESS_INT(i)) {
			RTIM_Cmd(TIMx[TIM_IDX], DISABLE);
			RTIM_Reset(TIMx[TIM_IDX]);
			printf("Key %d pressed \n", i);
		} else if (IntStatus & CT_CHX_RELEASE_INT(i)) {
			RTIM_Cmd(TIMx[TIM_IDX], DISABLE);
			RTIM_Reset(TIMx[TIM_IDX]);
			printf("Key %d released \n", i);
		}
	}

#if defined (CONFIG_AMEBASMART) || defined (CONFIG_AMEBALITE)
	if (IntStatus & CT_BIT_GUARD_PRESS_INTR) {
		RTIM_Cmd(TIMx[TIM_IDX], DISABLE);
		RTIM_Reset(TIMx[TIM_IDX]);
		printf("Guard sensor pressed \n");
	}

	if (IntStatus & CT_BIT_GUARD_RELEASE_INTR) {
		RTIM_Cmd(TIMx[TIM_IDX], DISABLE);
		RTIM_Reset(TIMx[TIM_IDX]);
		printf("Guard sensor released \n");
	}
#endif

	if (IntStatus & CT_BIT_AFIFO_OVERFLOW_INTR) {
		printf("CT_BIT_AFIFO_OVERFLOW_INTR \n");
	}

	if (IntStatus & CT_BIT_OVER_N_NOISE_TH_INTR) {
		printf("CT_BIT_OVER_N_NOISE_TH_INTR \n");

		RTIM_Cmd(TIMx[TIM_IDX], ENABLE);
	}

	if (IntStatus & CT_BIT_OVER_P_NOISE_TH_INTR) {
		RTIM_Cmd(TIMx[TIM_IDX], DISABLE);
		RTIM_Reset(TIMx[TIM_IDX]);
		printf("CT_BIT_OVER_P_NOISE_TH_INTR \n");
		CapTouch_Cmd(CAPTOUCH_DEV, DISABLE);
		CapTouch_Cmd(CAPTOUCH_DEV, ENABLE);
	}

	CapTouch_INTClearPendingBit(CAPTOUCH_DEV, IntStatus);

	return 0;
}

u32 ctc_timeout_handler(void *Data) {
	(void)Data;

	printf("Captouch N_Noise for %dms ==>\n", TIM_PERIOD);
	printf("\t Reset baseline...\n\n");

	RTIM_Cmd(TIMx[TIM_IDX], DISABLE);
	RTIM_Reset(TIMx[TIM_IDX]);
	RTIM_INTClear(TIMx[TIM_IDX]);
	RTIM_INTClear(TIMx[TIM_IDX]);

	CapTouch_Cmd(CAPTOUCH_DEV, DISABLE);
	CapTouch_Cmd(CAPTOUCH_DEV, ENABLE);

	return 0;
}

void ctc_timer_init(void) {
	RTIM_TimeBaseInitTypeDef TIM_InitStruct;

	RCC_PeriphClockCmd(APBPeriph_TIMx[TIM_IDX], APBPeriph_TIMx_CLOCK[TIM_IDX], ENABLE);

	RTIM_TimeBaseStructInit(&TIM_InitStruct);
	TIM_InitStruct.TIM_Idx = TIM_IDX;
	TIM_InitStruct.TIM_Period = TIM_PERIOD / 1000 * 32768 - 1;

	RTIM_TimeBaseInit(TIMx[TIM_IDX], (&TIM_InitStruct), TIMx_irq[TIM_IDX], (IRQ_FUN)ctc_timeout_handler, NULL);
	RTIM_INTConfig(TIMx[TIM_IDX], TIM_IT_Update, ENABLE);
}

void ctc_period_debug(void) {
	u32 i;

	for (i = 0; i < CT_CHANNEL_NUM; i++) {
		if (ENABLE == CTC_Ch_Config[i].CT_CHEnable) {
			printf("ch:%lu, sample_data:%lu\r\n", i, CapTouch_GetChAveData(CAPTOUCH_DEV, i));
			DelayMs(500);
		}
	}
}

void ctc_press_release_task(void) {
	CapTouch_InitTypeDef CapTouch_InitStruct;
	u32 i;

	RCC_PeriphClockCmd(APBPeriph_ADC, APBPeriph_ADC_CLOCK, ENABLE);
#if defined (CONFIG_AMEBASMART) || defined (CONFIG_AMEBALITE)
	RCC_PeriphClockCmd(APBPeriph_CTC, APBPeriph_CTC_CLOCK, ENABLE);
#endif

	for (i = 0; i < CT_CHANNEL_NUM; i++) {
		if (ENABLE == CTC_Ch_Config[i].CT_CHEnable) {
			Pinmux_Config(CTC_PAD[i], PINMUX_FUNCTION_CAPTOUCH);
			PAD_PullCtrl(CTC_PAD[i], GPIO_PuPd_NOPULL);
			PAD_SleepPullCtrl(CTC_PAD[i], GPIO_PuPd_NOPULL);
			PAD_InputCtrl(CTC_PAD[i], DISABLE);
		}
	}

	CapTouch_StructInit(&CapTouch_InitStruct);

	_memcpy((void *)CapTouch_InitStruct.CT_Channel, (void *)CTC_Ch_Config, CT_CHANNEL_NUM * sizeof(CapTouch_CHInitTypeDef));

	CapTouch_Init(CAPTOUCH_DEV, &CapTouch_InitStruct);
	CapTouch_Cmd(CAPTOUCH_DEV, ENABLE);
	CapTouch_INTConfig(CAPTOUCH_DEV, CT_ALL_INT_EN, ENABLE);
#if defined (CONFIG_AMEBASMART)
	CapTouch_INTConfig(CAPTOUCH_DEV, CT_BIT_SCAN_END_INTR_EN, DISABLE);
#elif defined (CONFIG_AMEBADPLUS)
	/* Data will be sent to FIFO automatically even when debug mode is off since BCut */
	CapTouch_INTConfig(CAPTOUCH_DEV, CT_BIT_AFIFO_OVERLVL_INTR_EN, DISABLE);
	CapTouch_INTConfig(CAPTOUCH_DEV, CT_BIT_AFIFO_OVERFLOW_INTR_EN, DISABLE);
#endif

	InterruptRegister((IRQ_FUN) CapTouch_irq_handler, CTOUCH_IRQ, (u32)NULL, INT_PRI_MIDDLE);
	InterruptEn(CTOUCH_IRQ, INT_PRI_MIDDLE);

	printf("CapTouch init done \r\n");

	ctc_timer_init();

	while (1) {
#if CTC_DATA_DBG
		/* call function ctc_period_debug() to check captouch channel sample data */
		ctc_period_debug();
#endif
	}
}

int main(void) {
	if (rtos_task_create(NULL, "CTC_TOUCH_RELEASE_DEMO", (rtos_task_t)ctc_press_release_task, NULL, 3072, (1)) != SUCCESS) {
		printf("Cannot create CTC_TOUCH_RELEASE_DEMO\r\n");
	}

	rtos_sched_start();

	return 0;
}
