/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_WAKEPIN_H_

#ifdef __cplusplus
extern "C" {
#endif

#define _AMEBA_WAKEPIN_H_

#define	WAKEPIN_0							((u32)0x00000000)/* correspond to PA_0 */
#define	WAKEPIN_1							((u32)0x00000001)/* correspond to PA_1 */

#define WAKEPIN_EVENT	(AON_BIT_GPIO_PIN0_WAKDET_EVT | AON_BIT_GPIO_PIN1_WAKDET_EVT)

enum wakepin_config {
	LOW_LEVEL_WAKEUP	= 0, /* when GPIO level become high, will wakeup*/
	HIGH_LEVEL_WAKEUP	= 1, /* when GPIO level become low, will wakeup*/
	DISABLE_WAKEPIN	= 2, /* disable wakepin */
};

typedef struct {
	u32 wakepin;
	enum wakepin_config config;
} WAKEPIN_TypeDef;



extern int WakePin_Get_Idx(void);
extern void Wakepin_Setting(u32 PinIdx, u32 Polarity);
extern void Wakepin_Debounce_Setting(u32 Dbnc_cycle, u32 Status);
extern void WakePin_ClearINT(u32 wakepin);

#ifdef __cplusplus
}
#endif


#endif /* _AMEBA_WAKEPIN_H_*/