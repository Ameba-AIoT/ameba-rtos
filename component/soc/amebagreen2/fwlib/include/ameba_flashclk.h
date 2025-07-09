/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_FLASH_CLK_H
#define _AMEBA_FLASH_CLK_H

/* Other definations --------------------------------------------------------*/
typedef enum {
	FlashClass1 = 0,
	FlashClass2,
	FlashClass3,
	FlashClass4,
	FlashClass5,
	FlashClass6,
	FlashClassUser = 0xFE,
	FlashClassNone = 0xFF,
} FlashClass;

typedef struct {
	u32 flash_id;
	u32 id_mask;
	u8 flash_class;
	u32 sta_mask;
	void (*FlashInitHandler)(void);
} FlashInfo_TypeDef;

void flash_highspeed_setup(void);
void Combo_SPIC_Init(void);

#endif  //_AMEBA_FLASH_CLK_H