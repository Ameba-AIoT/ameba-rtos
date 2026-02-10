/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_BOOT_SECURITY_H_
#define _AMEBA_BOOT_SECURITY_H_

#include "ameba_secure_boot.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CERT_SIZE_4K_ALIGN		0x1000
#define CERT_SIZE_8K_ALIGN		0x2000
#define CERT_MAX_KEYINFO		5

/* Certificate version flags */
#define CERT_VERSION_4KB			1
#define CERT_VERSION_8KB			2

/* Secure boot image2 public key hash IDs */
#define KEYID_NSPE				0
#define KEYID_NSPE_PQC			1
#define KEYID_TOTAL_CNT         2
Compile_Assert(KEYID_TOTAL_CNT <= CERT_MAX_KEYINFO, "KEYID_TOTAL_CNT should less than CERT_MAX_KEYINFO");

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

/* Extended certificate structure for 8KB support and PQC algorithms */
typedef struct {
	Certificate_TypeDef basic_cert;  /* First member - maintains layout compatibility */

	/* Extension area for PQC algorithms */
	u8 PQC_AuthAlg;                /* PQC authentication algorithm */
	u8 Rsvd2[3];
	u16 PQC_KeySize;               /* PQC public key size */
	u16 PQC_SigSize;               /* PQC signature size */
	u8 Rsvd3[8];
	u8 PQCPubKey[PKEY_MAX_LEN_EXT];    /* PQC public key (up to PKEY_MAX_LEN_EXT bytes) */
	u8 PQCSignature[SIGN_MAX_LEN_EXT]; /* PQC signature (up to SIGN_MAX_LEN_EXT bytes) */
} Certificate_TypeDef_Ext;

int BOOT_DecRDPImg(u32 SrcAddr, u8 *RdpIV, SubImgInfo_TypeDef *SubImgInfo, u8 *SubImgCnt);
u8 BOOT_SignatureCheck(Manifest_TypeDef *Manifest, SubImgInfo_TypeDef *SubImgInfo, u8 SubImgNum, Certificate_TypeDef *Cert, u32 KeyID);
int BOOT_SignatureCheck_PQC(Manifest_TypeDef *Manifest, Certificate_TypeDef *Cert, u32 idx, u32 KeyID);
u8 BOOT_CertificateCheck(Certificate_TypeDef *Cert, u32 idx);
int BOOT_CertificateCheck_PQC(Manifest_TypeDef *Manifest, Certificate_TypeDef *Cert, u32 idx);
u8 BOOT_Extract_SignatureCheck(Manifest_TypeDef *Manifest, SubImgInfo_TypeDef *SubImgInfo, u8 SubImgNum);
void BOOT_Validate_Fail_Stuck(void);

#ifdef __cplusplus
}
#endif

#endif
