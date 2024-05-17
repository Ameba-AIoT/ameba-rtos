/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifndef _AMEBA_BOOT_OTA_H_
#define _AMEBA_BOOT_OTA_H_
#include "boot_security_km4.h"

#define KEYID_NSPE				0
#define KEYID_SPE				1
#define KEYID_CNT				2

#define NONEVALIDIMG	0
#define ONEVALIDIMG		1
#define TWOVALIDIMG		2

#define SAMEVERSION		2

// IMG Type
typedef enum _IMG_TYPE_ {
	IMG_CERT   =   0x0,
	IMG_IMG2,
	IMG_TYPE_CNT,
} IMG_TYPE, *PIMG_TYPE;

u8 BOOT_OTA_SlotSelect(void);
u8 BOOT_LoadSubImage(SubImgInfo_TypeDef *SubImgInfo, u32 StartAddr, u8 Num, char **ImgName, u8 ErrLog);
void BOOT_RSIPIvSet(Manifest_TypeDef *Manifest, u32 IV_index);
void BOOT_OTFCheck(u32 start_addr, u32 end_addr, u32 IV_index, u32 OTF_index);
u8 BOOT_OTA_LoadIMGAll(u8 ImgIndex);
void BOOT_OTA_Region_Init(void);
u8 BOOT_OTA_IMG(void);

#endif