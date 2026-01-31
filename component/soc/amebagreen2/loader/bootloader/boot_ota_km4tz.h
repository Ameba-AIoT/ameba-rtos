/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SECURITY_H_
#define _AMEBA_SECURITY_H_
#include "boot_security_km4tz.h"

#define KEYID_NSPE				0
#define KEYID_SPE				1
#define KEYID_CNT				2

#define NONEVALIDIMG	0
#define ONEVALIDIMG		1
#define TWOVALIDIMG		2

#define SAMEVERSION		2

#ifdef __cplusplus
extern "C" {
#endif

// IMG Type
typedef enum _IMG_TYPE_ {
	IMG_CERT   =   0x0,
	IMG_IMG2,
	IMG_TYPE_CNT,
} IMG_TYPE, *PIMG_TYPE;

void BOOT_ImgCopy(void *__restrict dst0, const void *__restrict src0, size_t len0);
void BOOT_OTFMaskConfig(void);
void BOOT_RSIPIvSet(Manifest_TypeDef *Manifest, u32 IV_index);
u8 BOOT_LoadSubImage(SubImgInfo_TypeDef *SubImgInfo, u32 StartAddr, u8 Num, char **ImgName, u8 ErrLog);
u8 BOOT_OTA_IMG(void);

u32 Boot_Fullmac_XipEn(void);
void Boot_Fullmac_LoadImage(void);

#ifdef __cplusplus
}
#endif

#endif

