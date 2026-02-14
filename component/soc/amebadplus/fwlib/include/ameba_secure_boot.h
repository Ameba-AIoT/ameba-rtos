/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SECURITY_BOOT_H_
#define _AMEBA_SECURITY_BOOT_H_

#define SBOOT_ERR_AUTH_NOT_SUPPORT					-1
#define SBOOT_ERR_PK_HASH_INVALID					-2
#define SBOOT_ERR_SIGN_INVALID						-3
#define SBOOT_ERR_ECDSA_CURVE_NOT_SUPPORT			-4
#define SBOOT_ERR_HASH_NOT_SUPPORT					-6
#define SBOOT_ERR_IMG_HASH_INVALID					-7
#define SBOOT_ERR_PK_HASH_ENGINE_ERROR				-8
#define SBOOT_ERR_EDDSA_HASH_ENGINE					-9
#define SBOOT_ERR_ECDSA_HASH_ENGINE					-10
#define SBOOT_ERR_IMG_HASH_ENGINE					-11
#define SBOOT_ERR_AUTH_ALG_NOT_MATCH				-12
#define SBOOT_ERR_HASH_ALG_NOT_MATCH				-13

#define IMG_SIZE_INVALID							-0xFFFE
#define IMG_INVALID									-0xFFFF

/*ImgID_TypeDef*/
#define IMGID_BOOT	0
#define IMGID_APP	1
#define IMGID_DSP	2
#define IMGID_BL1	3

/*AuthAlg_TypeDef*/
#define AUTHID_EDDSA_25519				0
#define AUTHID_ECDSA_SECP192R1			1
#define AUTHID_ECDSA_SECP224R1			2
#define AUTHID_ECDSA_SECP256R1			3
#define AUTHID_ECDSA_BP256R1			6
#define AUTHID_ECDSA_CURVE25519			9
#define AUTHID_ECDSA_SECP192K1			10
#define AUTHID_ECDSA_SECP224K1			11
#define AUTHID_ECDSA_SECP256K1			12

/* PQC_AuthAlg_TypeDef: 0xf0, 0xf1, 0xf2 */
#define AUTHID_PQC_ML_DSA_44			240
#define AUTHID_PQC_ML_DSA_65			241
#define AUTHID_PQC_ML_DSA_87			242
#define PQC_ML_DSA_44_PUBKEY_LEN        1312
#define PQC_ML_DSA_65_PUBKEY_LEN        1952
#define PQC_ML_DSA_87_PUBKEY_LEN        2592
#define PQC_ML_DSA_44_SIG_LEN           2420
#define PQC_ML_DSA_65_SIG_LEN           3309
#define PQC_ML_DSA_87_SIG_LEN           4627

#define SBOOT_ECP_MAX_BITS				256   /**< Maximum bit size of ecdsa groups */
#define SBOOT_ECP_MAX_BYTES				( ( SBOOT_ECP_MAX_BITS + 7 ) / 8 )
#define SBOOT_ECP_MAX_PT_LEN			( 2 * SBOOT_ECP_MAX_BYTES)
#define SBOOT_ECP_BIT2BYTE(x)			( ( (x) + 7 ) / 8 )

#define IS_EDDSA(x)				(x == AUTHID_EDDSA_25519)
#define IS_ECDSA(x)				(x == AUTHID_ECDSA_SECP192R1) || \
								(x == AUTHID_ECDSA_SECP224R1) || \
								(x == AUTHID_ECDSA_SECP256R1) || \
								(x == AUTHID_ECDSA_BP256R1) || \
								(x == AUTHID_ECDSA_CURVE25519) || \
								(x == AUTHID_ECDSA_SECP192K1) || \
								(x == AUTHID_ECDSA_SECP224K1) || \
								(x == AUTHID_ECDSA_SECP256K1)
#define IS_PQC_ML_DSA_65(x)     (x == AUTHID_PQC_ML_DSA_65)

/*HashAlg_TypeDef*/
#define HASHID_SHA256		0
#define HASHID_SHA384		1
#define HASHID_SHA512		2
#define HASHID_HMAC256		3
#define HASHID_HMAC384		4
#define HASHID_HMAC512		5

#define HASH_MAX_LEN		64
#define PKEY_MAX_LEN		144 //((((2 * MBEDTLS_ECP_MAX_BYTES -1) >> 4) + 1) << 4) //144 byte (16-byte aligned, format type field not included)
#define SIGN_MAX_LEN		144 //((((2 * MBEDTLS_ECP_MAX_BYTES -1) >> 4) + 1) << 4) //144 byte (16-byte aligned, raw data format, not ASN.1)

#define SBOOT_PUBKEY_HASH_SIZE		32
#define SBOOT_PQC_STACK_SIZE		(64 * 1024)  /* 64KB for PQC verification */

/* Extended manifest sizes for PQC support */
#define PKEY_MAX_LEN_EXT	2592 /* Extended public key length for ml-dsa-87 algorithms (16-byte aligned) */
#define SIGN_MAX_LEN_EXT	4640 /* Extended signature length for ml-dsa-87 algorithms (16-byte aligned) */

#define CRYPTO_CHK(f, errno) do { if((ret = f) != 0) {ret = errno; goto exit;}} while( 0 )

#define MANIFEST_SIZE_4K_ALIGN		0x1000
#define MANIFEST_SIZE_8K_ALIGN		0x2000

/* Manifest version flags */
#define MANIFEST_VERSION_4KB		1
#define MANIFEST_VERSION_8KB		2

typedef struct {
	u32 Pattern[2];
	u8 Rsvd1[8];
	u8 Ver;
	u8 ImgID;
	u8 AuthAlg;
	u8 HashAlg;
	u16 MajorImgVer;
	u16 MinorImgVer;
	u32 ImgSize;
	u32 SecEpoch;
	u8 RsipIV[16];
	u8 ImgHash[HASH_MAX_LEN];
	u8 SBPubKey[PKEY_MAX_LEN];
	u8 Signature[SIGN_MAX_LEN];
} Manifest_TypeDef;

/* PQC Manifest Extension */
typedef struct {
	u8 PQC_AuthAlg;                /* PQC authentication algorithm */
	u8 Rsvd2[3];
	u16 PQC_KeySize;               /* PQC public key size */
	u16 PQC_SigSize;               /* PQC signature size */
	u8 Rsvd3[8];
	u8 PQCPubKey[PKEY_MAX_LEN_EXT];    /* PQC public key (up to PKEY_MAX_LEN_EXT bytes) */
	u8 PQCSignature[SIGN_MAX_LEN_EXT]; /* PQC signature (up to SIGN_MAX_LEN_EXT bytes) */
} Manifest_PQC_TypeDef;

/* Extended manifest structure for 8KB support and PQC algorithms */
typedef struct {
	Manifest_TypeDef basic_manifest;

	/* Extension area for PQC algorithms */
	Manifest_PQC_TypeDef pqc_manifest;
} Manifest_TypeDef_Ext;

typedef struct {
	u32 Addr;
	u32 Len;
} SubImgInfo_TypeDef;

int PQCLEAN_MLDSA65_CLEAN_crypto_sign_verify_ctx(const uint8_t *sig,
		size_t siglen, const uint8_t *m, size_t mlen, const uint8_t *ctx, size_t ctxlen, const uint8_t *pk);
int SBOOT_GetMdType(u8 HashId, SHA2_TYPE *MdType, u8 *IsHmac);
int SBOOT_Validate_Algorithm(u8 *AuthAlg, u8 *HashAlg, u8 ManiAuth, u8 ManiHash);
int SBOOT_Validate_PubKey(u8 AuthAlg, u8 *Pk, u8 *Hash);
int SBOOT_Validate_Signature(u8 AuthAlg, u8 HashAlg, u8 *Pk, u8 *Msg, u32 Len, u8 *Sig);
int SBOOT_Validate_ImgHash(u8 HashAlg, u8 *ImgHash, SubImgInfo_TypeDef *SubImgInfo, u8 Num);
int SBOOT_SignatureVerify(u8 *PubKeyHash, Manifest_TypeDef *Manifest, SubImgInfo_TypeDef *SubImgInfo, u8 SubImgCnt);
u32 BOOT_OTA_GetManifestSize(Manifest_TypeDef *Manifest);

#endif

