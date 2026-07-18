/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _HAL_BOOT_
#define _HAL_BOOT_

#ifdef __cplusplus
extern "C" {
#endif

#define CERT_SIZE_4K_ALIGN		0x1000
#define CERT_MAX_KEYINFO		5

#define KEYID_NSPE				0
#define KEYID_SPE				1
#define KEYID_AP				2

typedef struct {
	uint32_t KeyID;
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

void BOOT_FaultHandler(uint32_t mstack[], uint32_t pstack[], uint32_t lr_value, uint32_t fault_id);
void BOOT_INT_SecureFault(void);
void BOOT_ImgCopy(void *__restrict dst0, const void *__restrict src0, size_t len0);
void BOOT_OTFMaskConfig(void);
u8 BOOT_LoadRDPImg(Manifest_TypeDef *Manifest, SubImgInfo_TypeDef *SubImgInfo, u32 SrcAddr);
u8 BOOT_SignatureCheck(Manifest_TypeDef *Manifest, SubImgInfo_TypeDef *SubImgInfo, u8 SubImgNum, Certificate_TypeDef *Cert, u32 KeyID);
u8 BOOT_CertificateCheck(Certificate_TypeDef *Cert, u32 addr);
u8 BOOT_Extract_SignatureCheck(Manifest_TypeDef *Manifest, SubImgInfo_TypeDef *SubImgInfo, u8 SubImgNum);
void BOOT_VerCheck(void);

/* Platform init utilities (also used by Zephyr KM4 MCUboot boot_prepare.c) */
void BOOT_ReasonSet(void);
void BOOT_SOC_ClkSet(void);
void BOOT_GRstConfig(void);
void BOOT_RccConfig(void);
void BOOT_Log_Init(void);
void BOOT_DDR_Init(void);
void BOOT_DDR_LCDC_HPR(void);
void BOOT_PSRAM_Init(void);
void BOOT_Enable_KM0(void);
void BOOT_Enable_AP(void);
void BOOT_Disable_AP(void);
void BOOT_TCMSet(u32 TCM_Size);
u32  BOOT_Share_Memory_Patch(void);
void Peripheral_Reset(void);
bool BOOT_RRAM_InfoValid(void);

#ifdef __cplusplus
}
#endif
#endif

