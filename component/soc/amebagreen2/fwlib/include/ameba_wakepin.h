/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_WAKEPIN_H_
#define _AMEBA_WAKEPIN_H_


#define	WAKEPIN_0							((u32)0x00000000)
#define	WAKEPIN_1							((u32)0x00000001)
#define	WAKEPIN_2							((u32)0x00000002)
#define	WAKEPIN_3							((u32)0x00000003)

#define WAKEPIN_EVENT	(AON_BIT_GPIO_PIN0_WAKDET_EVT | AON_BIT_GPIO_PIN1_WAKDET_EVT | AON_BIT_GPIO_PIN2_WAKDET_EVT | AON_BIT_GPIO_PIN3_WAKDET_EVT)


#ifdef __cplusplus
extern "C" {
#endif

enum wakepin_config {
	LOW_LEVEL_WAKEUP	= 0, /* when GPIO level is high, will wakeup*/
	HIGH_LEVEL_WAKEUP	= 1, /* when GPIO level is low, will wakeup*/
	DISABLE_WAKEPIN	= 2, /* disable wakepin */
};

struct WAKEPIN_TypeDef {
	u32 wakepin;
	enum wakepin_config config;
};

extern int WakePin_Get_Idx(void);
extern void Wakepin_Setting(u32 PinIdx, u32 Polarity);
extern void Wakepin_Debounce_Setting(u32 Dbnc_cycle, u32 Status);
extern void WakePin_ClearINT(u32 wakepin);


#ifdef __cplusplus
}
#endif
#endif  //_AMEBA_WAKEPIN_H_
