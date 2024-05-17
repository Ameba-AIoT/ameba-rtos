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

typedef enum {
	FLASH_CLK_DIV2 = 1,
	FLASH_CLK_DIV3,
	FLASH_CLK_DIV4,
	FLASH_CLK_DIV5,
	FLASH_CLK_DIV6,
	FLASH_CLK_DIV7,
	FLASH_CLK_DIV8,
	FLASH_CLK_DIV9,
	FLASH_CLK_DIV10,
} FlashDivInt;

void flash_highspeed_setup(void);
#endif  //_AMEBA_FLASH_CLK_H