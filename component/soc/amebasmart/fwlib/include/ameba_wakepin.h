/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_WAKEPIN_H_
#define _AMEBA_WAKEPIN_H_

#ifdef __cplusplus
extern "C" {
#endif

#define	WAKEPIN_0							((u32)0x00000000)/*!< see aon_wakepin */
#define	WAKEPIN_1							((u32)0x00000001)/*!< see aon_wakepin */
#define	WAKEPIN_2							((u32)0x00000002)/*!< see aon_wakepin */
#define	WAKEPIN_3							((u32)0x00000003)/*!< see aon_wakepin */

#define WAKEPIN_EVENT	(AON_BIT_GPIO_PIN0_WAKDET_EVT | AON_BIT_GPIO_PIN1_WAKDET_EVT | AON_BIT_GPIO_PIN2_WAKDET_EVT | AON_BIT_GPIO_PIN3_WAKDET_EVT)

enum wakepin_config {
	LOW_LEVEL_WAKEUP	= 0, /* when GPIO level become high, will wakeup*/
	HIGH_LEVEL_WAKEUP	= 1, /* when GPIO level become low, will wakeup*/
	DISABLE_WAKEPIN	= 2, /* disable wakepin */
};

typedef struct {
	u32 wakepin;
	enum wakepin_config config;
} WAKEPIN_TypeDef;

extern u8 aon_wakepin[4];

extern int WakePin_Get_Idx(void);
extern void Wakepin_Setting(u32 PinIdx, u32 Polarity);
extern void Wakepin_Debounce_Setting(u32 Dbnc_cycle, u32 Status);
extern void WakePin_ClearINT(u32 wakepin);

#ifdef __cplusplus
}
#endif

#endif  //_AMEBA_WAKEPIN_H_
