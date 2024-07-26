/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "ameba_soc.h"
#include "gpio_ext.h"
#include "device.h"
#include "os_wrapper.h"
#include <stdio.h>

#if defined (CONFIG_AMEBALITE)
#define APBPeriph_AUDIO			APBPeriph_AC_AIP
#endif

void digit_app_entry(void)
{
	GPIO_InitTypeDef GPIO_InitStruct_AUDIO_PIN;
	Pinmux_Config(AUDIO_PAD_PIN, PINMUX_FUNCTION_GPIO);

	/* enable gpio function & clock */
	RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);

	GPIO_InitStruct_AUDIO_PIN.GPIO_Pin = AUDIO_PAD_PIN;
	GPIO_InitStruct_AUDIO_PIN.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&GPIO_InitStruct_AUDIO_PIN);

	while (1) {
		GPIO_WriteBit(AUDIO_PAD_PIN, 0x1);
		DelayMs(500);
		GPIO_WriteBit(AUDIO_PAD_PIN, 0x0);
		DelayMs(500);
	}
}

void audio_app_entry(void)
{
	/* edit your audio applicaiton here */
	while (1);
}

void audio_pad_digital_demo(void)
{
	u8 PinName = AUDIO_PAD_PIN;
	RCC_PeriphClockCmd(APBPeriph_AUDIO, APBPeriph_CLOCK_NULL, ENABLE);

#ifdef DIGIT_PATH_EN

#if defined (CONFIG_AMEBASMART)

	/* --- digital path init begin --- */
	if ((PinName >= APAD_MIC_PAD_START) && (PinName <= APAD_MIC_PAD_END)) {
		/* keep audio MICBST be mute */
		switch (PinName) {
		case _PA_20:
		case _PA_21:
			/* mute AMIC1 */
			AUDIO_CODEC_SetMicBstChnMute(AMIC1, MICIN, MUTE);
			break;
		case _PA_22:
		case _PA_23:
			/* mute AMIC2 */
			AUDIO_CODEC_SetMicBstChnMute(AMIC2, MICIN, MUTE);
			break;
		case _PA_24:
		case _PA_25:
			/* mute AMIC3 */
			AUDIO_CODEC_SetMicBstChnMute(AMIC3, MICIN, MUTE);
			break;
		case _PA_26:
		case _PA_27:
			/* mute AMIC4 */
			AUDIO_CODEC_SetMicBstChnMute(AMIC4, MICIN, MUTE);
			break;
		case _PA_28:
		case _PA_29:
			/* mute AMIC5 */
			AUDIO_CODEC_SetMicBstChnMute(AMIC5, MICIN, MUTE);
			break;
		default:
			break;
		}

	} else if ((PinName >= APAD_MIC_BIAS_PAD_START) && (PinName <= APAD_MIC_BIAS_PAD_END)) {
		/* keep audio MICBIAS power be power down mode */
		AUDIO_CODEC_SetMicBiasPowerMode(POWER_DOWN);

		switch (PinName) {
		case _PA_30:
			AUDIO_CODEC_SetMicBiasPCUTMode(AMIC1, POWER_DOWN);
			break;
		case _PA_31:
			AUDIO_CODEC_SetMicBiasPCUTMode(AMIC2, POWER_DOWN);
			break;
		case _PB_0:
			AUDIO_CODEC_SetMicBiasPCUTMode(AMIC3, POWER_DOWN);
			break;
		case _PB_1:
			AUDIO_CODEC_SetMicBiasPCUTMode(AMIC4, POWER_DOWN);
			break;
		case _PB_2:
			AUDIO_CODEC_SetMicBiasPCUTMode(AMIC5, POWER_DOWN);
			break;
		default:
			break;
		}

	} else if ((PinName >= APAD_OUT_PAD_START) && (PinName <= APAD_OUT_PAD_END)) {
		/* keep audio HPO power be power down mode */
		switch (PinName) {
		case _PB_3:
		case _PB_4:
			/* AOUT LPLN */
			AUDIO_CODEC_SetHPOPowerMode(CHN_L, POWER_DOWN);
			break;
		case _PB_5:
		case _PB_6:
			/* AOUT RPRN */
			AUDIO_CODEC_SetHPOPowerMode(CHN_R, POWER_DOWN);
			break;
		default:
			break;
		}
	}

#elif defined (CONFIG_AMEBALITE)

	if (((PinName >= APAD_MIC_PAD_GRP1_START) && (PinName <= APAD_MIC_PAD_GRP1_END)) || \
		((PinName >= APAD_MIC_PAD_GRP2_START) && (PinName <= APAD_MIC_PAD_GRP2_END))) {

		/* keep audio MICBST be mute */
		switch (PinName) {
		case _PB_11:
		case _PB_12:
			/* mute AMIC1 */
			AUDIO_CODEC_SetMicBstChnMute(AMIC1, MICIN, MUTE);
			break;
		case _PB_13:
		case _PB_14:
			/* mute AMIC2 */
			AUDIO_CODEC_SetMicBstChnMute(AMIC2, MICIN, MUTE);
			break;
		case _PB_18:
		case _PB_19:
			/* mute AMIC3 */
			AUDIO_CODEC_SetMicBstChnMute(AMIC3, MICIN, MUTE);
			break;
		default:
			break;
		}

	} else if (PinName == APAD_MIC_BIAS_PAD) {

		/* keep audio MICBIAS power be power down mode */
		AUDIO_CODEC_SetMicBiasPowerMode(POWER_DOWN);

	} else if ((PinName >= APAD_OUT_PAD_START) && (PinName <= APAD_OUT_PAD_END)) {

		/* keep audio LINEOUT power be power down mode */
		switch (PinName) {
		case _PB_16:
		case _PB_17:
			AUDIO_CODEC_SetLineOutPowerMode(DAC_L, POWER_DOWN);
			break;
		default:
			break;
		}
	}

#endif

	APAD_InputCtrl(AUDIO_PAD_PIN, ENABLE);
	/* --- digital path init done --- */

	/* digital application entry */
	digit_app_entry();

#else

	APAD_InputCtrl(AUDIO_PAD_PIN, DISABLE);
	Pinmux_Config(AUDIO_PAD_PIN, PINMUX_FUNCTION_AUDIO);

	/* audio application entry */
	audio_app_entry();

#endif
}

int main(void)
{
	if (SUCCESS != rtos_task_create(NULL, "AUDIO_PAD_DIGITAL_TASK", (rtos_task_t)audio_pad_digital_demo, (void *)NULL, 1024 * 4,
									(1))) {
		printf("Create AUDIO_PAD_DIGITAL_TASK Err!!\n");
	}

	rtos_sched_start();

	return 0;
}
