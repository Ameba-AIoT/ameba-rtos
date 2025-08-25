/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _HAL_BOOT_
#define _HAL_BOOT_

#define CERT_SIZE_4K_ALIGN		0x1000
#define CERT_MAX_KEYINFO		5

#define KEYID_NSPE				0
#define KEYID_SPE				1
#define KEYID_DSP				2

/* This is used as a signature to validate the blob header */
#define TOC_HEADER_NAME	0xAA640001

/* Length of a node address (an IEEE 802 address). */
#define	_UUID_NODE_LEN		6

#define _FIP_MAX_SIZE		0x00100000

struct uuid {
	uint8_t		time_low[4];
	uint8_t		time_mid[2];
	uint8_t		time_hi_and_version[2];
	uint8_t		clock_seq_hi_and_reserved;
	uint8_t		clock_seq_low;
	uint8_t		node[_UUID_NODE_LEN];
};

/* XXX namespace pollution? */
typedef struct uuid uuid_t;

typedef struct fip_toc_header {
	uint32_t	name;
	uint32_t	serial_number;
	uint64_t	flags;
} fip_toc_header_t;

typedef struct fip_toc_entry {
	uuid_t		uuid;
	uint64_t	offset_address;
	uint64_t	size;
	uint64_t	flags;
} fip_toc_entry_t;

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
void BOOT_OTFMaskConfig(void);
u8 BOOT_LoadRDPImg(Manifest_TypeDef *Manifest, SubImgInfo_TypeDef *SubImgInfo, u32 SrcAddr);
u8 BOOT_SignatureCheck(Manifest_TypeDef *Manifest, SubImgInfo_TypeDef *SubImgInfo, u8 SubImgNum, Certificate_TypeDef *Cert, u32 KeyID);
u8 BOOT_CertificateCheck(Certificate_TypeDef *Cert, u32 addr);
u8 BOOT_Extract_SignatureCheck(Manifest_TypeDef *Manifest, SubImgInfo_TypeDef *SubImgInfo, u8 SubImgNum);

void BOOT_RSIPIvSet(Manifest_TypeDef *Manifest, u32 IV_index);
void BOOT_OTFCheck(u32 start_addr, u32 end_addr, u32 IV_index, u32 OTF_index);
u8 BOOT_LoadSubImage(SubImgInfo_TypeDef *SubImgInfo, u32 StartAddr, u8 Num, char **ImgName, u8 ErrLog);

void BOOT_LoadDSP_SetCert(Certificate_TypeDef *Cert);
int BOOT_LoadDSPImg(Certificate_TypeDef *Cert);
u8 BOOT_LoadDSPImg_FromImg2(SubImgInfo_TypeDef *SubImgInfo, u8 Index, u32 Img2Addr);
void BOOT_Enable_Config(void);
void BOOT_SRAM_Extend_Config(void);
void BOOT_CFG_CORE_Vol(void);
void BOOT_VerCheck(void);
#endif

