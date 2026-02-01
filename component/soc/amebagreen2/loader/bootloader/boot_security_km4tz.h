/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SECURITY_AP_H_
#define _AMEBA_SECURITY_AP_H_

#include "ameba_secure_boot.h"

#define CERT_SIZE_4K_ALIGN		0x1000
#define CERT_MAX_KEYINFO		5

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	u32 KeyID;
	unsigned char Hash[32];

} CertEntry_TypeDef;

typedef struct {
	u8 Pattern[8];
	u8 Rsvd1[8];
	u8 Ver;
	u8 ImgID;
	u8 AuthAlg;
	u8 HashAlg;
	u16 MajorKeyVer;
	u16 MinorKeyVer;
	u32 TableSize;
	u32 EntryNum;
	u8 SBPubKey[PKEY_MAX_LEN];
	CertEntry_TypeDef PKInfo[CERT_MAX_KEYINFO];
	// Signature
} Certificate_TypeDef;

int BOOT_DecRDPImg(u32 SrcAddr, u8 *RdpIV, SubImgInfo_TypeDef *SubImgInfo, u8 *SubImgCnt);
fih_ret BOOT_SignatureCheck(Manifest_TypeDef *Manifest, SubImgInfo_TypeDef *SubImgInfo, u8 SubImgNum, Certificate_TypeDef *Cert, u32 KeyID);
fih_ret BOOT_CertificateCheck(Certificate_TypeDef *Cert, u32 idx);
fih_ret BOOT_Extract_SignatureCheck(Manifest_TypeDef *Manifest, SubImgInfo_TypeDef *SubImgInfo, u8 SubImgNum);
fih_ret BOOT_RAM_TZCfg(void);
fih_ret BOOT_Wake_TZCfg(void);

#ifdef __cplusplus
}
#endif

#endif

