/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifndef GPIO_EXT_H
#define GPIO_EXT_H
#include "platform_autoconf.h"

#define DIGIT_PATH_EN		1

#if defined (CONFIG_AMEBASMART)
#define AUDIO_PAD_PIN				_PA_20

/* Audio pad region and types */
#define APAD_NAME_START				_PA_18
#define APAD_NAME_END				_PB_6

#define APAD_MIC_PAD_START			_PA_20
#define APAD_MIC_PAD_END			_PA_29

#define APAD_MIC_BIAS_PAD_START		_PA_30
#define APAD_MIC_BIAS_PAD_END		_PB_2

#define APAD_OUT_PAD_START			_PB_3
#define APAD_OUT_PAD_END			_PB_6

#elif defined (CONFIG_AMEBALITE)
#define AUDIO_PAD_PIN				_PB_14

/* Audio pad region and types */
#define APAD_NAME_START				_PB_11
#define APAD_NAME_END				_PB_19

#define APAD_MIC_PAD_GRP1_START		_PB_11
#define APAD_MIC_PAD_GRP1_END		_PB_14

#define APAD_MIC_PAD_GRP2_START		_PB_18
#define APAD_MIC_PAD_GRP2_END		_PB_19

#define APAD_MIC_BIAS_PAD			_PB_15

#define APAD_OUT_PAD_START			_PB_16
#define APAD_OUT_PAD_END			_PB_17

#endif
extern int example_raw_gpio_audio_pad(void);
#endif
