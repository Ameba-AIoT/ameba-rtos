/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "boot_security_km4.h"
#include "boot_ota_km4.h"

static const char *const TAG = "BOOT";
extern u32 Cert_PKHash_OTP_ADDR;
extern u8 Signature[2][SIGN_MAX_LEN];
u8 SecureBootEn = DISABLE;
u8 SecureBootEn_PQC = DISABLE;  /* Tracks if PQC is enabled for this boot */

/* Global variable for PQC verification return value (required for stack switching) */
static volatile int g_pqc_verify_ret = -1;

int BOOT_DecRDPImg(u32 SrcAddr, u8 *RdpIV, SubImgInfo_TypeDef *SubImgInfo, u8 *Cnt)
{
	IMAGE_HEADER *Hdr = NULL;
	u8 InBuf[256];
	ALIGNMTO(CACHE_LINE_SIZE) u8 Outbuf[CACHE_LINE_ALIGNMENT(16)];
	u8 Key[32], TmpIV[16];
	u32 DestAddr, TotalLen;
	u32 i = 0, sz;
	char *Km4Img3Label[2] = {"KM4 IMG3", "KM4 NSC"};
	u8 SubImgCnt = sizeof(Km4Img3Label) / sizeof(char *);

	*Cnt = SubImgCnt;

	/* Step 1: Try to load plaintext image first */
	if (BOOT_LoadSubImage(SubImgInfo, SrcAddr, SubImgCnt, Km4Img3Label, FALSE) == TRUE) {
		/* Plaintext load succeeded - print warning */
		for (i = 0; i < 5; i++) {
			RTK_LOGW(TAG, "IMG3 not encrypted! Enable RDP for MP!\n");
		}
		return TRUE;
	}

	/* Step 2: Plaintext load failed, check if RDP is enabled in OTP */
	if (SYSCFG_OTP_RDPEn() == FALSE) {
		/* RDP not enabled but image is encrypted - this is an error */
		RTK_LOGE(TAG, "Plaintext IMG3 load failed! Must enable OTP decryption!\n");
		return FALSE;
	}

	/* Step 3: RDP enabled, proceed with decryption */
	RTK_LOGI(TAG, "RDP EN\n");

	/* Initialize AES engine */
	CRYPTO_Init(NULL);
	/* Load RDP key to IPSEC */
	CRYPTO_OTPKey_Init(0, ENABLE);

	/* Key array not used, pass in for API restrict */
	rtl_crypto_aes_cbc_init(Key, 32);
	_memcpy(TmpIV, RdpIV, 16);

	for (i = 0; i < SubImgCnt; i++) {
		/* decrypt header to get image addr and size */
		_memcpy((void *)InBuf, (void *)SrcAddr, 16);
		rtl_crypto_aes_cbc_decrypt(InBuf, 16, TmpIV, 16, Outbuf);

		Hdr = (IMAGE_HEADER *)Outbuf;
		if ((Hdr->signature[0] != APP_IMAGE_PATTERN_1) || (Hdr->signature[1] != APP_IMAGE_PATTERN_2)) {
			RTK_LOGE(TAG, "RDP image Decrypt Fail!\n");
			CRYPTO_OTPKey_Init(0, DISABLE);
			return FALSE;
		}

		DestAddr = Hdr->image_addr - IMAGE_HEADER_LEN;
		TotalLen = Hdr->image_size + IMAGE_HEADER_LEN;
		RTK_LOGI(TAG, "DEC %s[%08lX:%lx]\n", Km4Img3Label[i], DestAddr, TotalLen);

		if (SubImgInfo != NULL) {
			SubImgInfo[i].Addr = DestAddr;
			SubImgInfo[i].Len = TotalLen;
		}

		/* decrypt image by segment */
		while (TotalLen > 0) {
			sz = TotalLen > 256 ? 256 : TotalLen;

			_memcpy((void *)InBuf, (void *)SrcAddr, sz);
			rtl_crypto_aes_cbc_decrypt(InBuf, sz, TmpIV, 16, (u8 *)DestAddr);

			/* store IV for next block */
			_memcpy(TmpIV, InBuf + sz - 16, 16);

			TotalLen -= sz;
			DestAddr += sz;
			SrcAddr += sz;
		}
	}

	CRYPTO_OTPKey_Init(0, DISABLE);
	return TRUE;
}

void BOOT_Validate_Fail_Stuck(void)
{
	/* clear stack */
	__set_MSP(MSP_RAM_HP);

	for (u32 paddr = MSPLIM_RAM_HP; paddr < MSP_RAM_HP; paddr += 4) {
		*(u32 *)(paddr) = 0;
	}

	DCache_CleanInvalidate(MSPLIM_RAM_HP, MSP_RAM_HP - MSPLIM_RAM_HP);

	while (1) {
		DelayMs(1000);
	}
}

u8 BOOT_SignatureCheck(Manifest_TypeDef *Manifest, SubImgInfo_TypeDef *SubImgInfo, u8 SubImgNum, Certificate_TypeDef *Cert, u32 KeyID)
{
	int ret = 0;
	u8 i;
	char *Name[] = {"IMG1", "IMG2", "IMG3"};
	u8 AuthAlg, HashAlg;

	/* 1. check if secure boot enable. */
	if (SecureBootEn == DISABLE) {
		return 0;
	}

	/* 2. verify signature */
	/* 2.1 Initialize hash engine */
	CRYPTO_SHA_Init(NULL);

	/* 2.2 Check algorithm from manifest against OTP configuration if need. */
	ret = SBOOT_Validate_Algorithm(&AuthAlg, &HashAlg, Manifest->AuthAlg, Manifest->HashAlg);
	if (ret != 0) {
		goto SBOOT_FAIL;
	}

	/* 2.3 validate pubkey hash */
	for (i = 0; i < Cert->EntryNum; i++) {
		if (Cert->PKInfo[i].KeyID == KeyID) {
			break;
		}
	}
	ret = SBOOT_Validate_PubKey(AuthAlg, Manifest->SBPubKey, Cert->PKInfo[i].Hash);
	if (ret != 0) {
		goto SBOOT_FAIL;
	}

	/* 2.4 validate signature */
	ret = SBOOT_Validate_Signature(AuthAlg, HashAlg, Manifest->SBPubKey, (u8 *)Manifest, sizeof(Manifest_TypeDef) - SIGN_MAX_LEN,
								   Manifest->Signature);
	if (ret != 0) {
		goto SBOOT_FAIL;
	}

	/* Hash validation moved to BOOT_OTA_LoadIMGAll() after both ECC and PQC pass */
	return ret;

SBOOT_FAIL:
	RTK_LOGE(TAG, "%s VERIFY FAIL, ret = %d\n", Name[Manifest->ImgID], ret);

	/* clear copied image */
	for (i = 0; i < SubImgNum; i++) {
		if (!IS_FLASH_ADDR(SubImgInfo[i].Addr)) {
			_memset((void *)SubImgInfo[i].Addr, 0, SubImgInfo[i].Len);
			DCache_CleanInvalidate(SubImgInfo[i].Addr, SubImgInfo[i].Len);
		}
	}
	return ret;
}

static u8 BOOT_SbootEn_Check(u8 *pk_hash)
{
	u8 i, j;
	if (SYSCFG_OTP_RMAMode()) {
		j = _rand() % 32;
		for (i = 0; i < 32; i++, j = (j + 1) % 32) {
			/* 2. read public key hash from OTP. Start with a random index to avoid side channel attack. */
			/* If in RMA mode but RMA Pubkey is not programed, skip SBOOT */
			pk_hash[j] = HAL_READ8(OTPC_REG_BASE, SEC_PKKEY_RMA_PK_0 + j);
			if (pk_hash[j] != 0xFF) {
				SecureBootEn = ENABLE;
			}
		}
	} else {
		/* 1. check if secure boot enable if not in RMA mode. */
		if (SYSCFG_OTP_SBootEn() == FALSE) {
			return SecureBootEn;
		} else {
			SecureBootEn = ENABLE;
		}

		/* 2. read public key hash from OTP. Start with a random index to avoid side channel attack. */
		j = _rand() % 32;
		for (i = 0; i < 32; i++, j = (j + 1) % 32) {
			pk_hash[j] = HAL_READ8(OTPC_REG_BASE, Cert_PKHash_OTP_ADDR + j);
		}
	}

	return SecureBootEn;
}

u8 BOOT_CertificateCheck(Certificate_TypeDef *Cert, u32 idx)
{
	u8 PubKeyHash[32];
	int ret;
	u8 AuthAlg, HashAlg;

	/* 1. check if secure boot enable. */
	/* 2. read public key hash from OTP if sboot en. Start with a random index to avoid side channel attack. */
	if (BOOT_SbootEn_Check(PubKeyHash) == DISABLE) {
		return 0;
	}

	/* 3. verify signature */
	/* 3.1 Initialize hash engine */
	CRYPTO_SHA_Init(NULL);

	/* 3.2 Check algorithm from flash against OTP configuration if need. */
	ret = SBOOT_Validate_Algorithm(&AuthAlg, &HashAlg, Cert->AuthAlg, Cert->HashAlg);
	if (ret != 0) {
		goto SBOOT_FAIL;
	}

	/* 3.3 validate pubkey hash */
	ret = SBOOT_Validate_PubKey(AuthAlg, Cert->SBPubKey, PubKeyHash);
	if (ret != 0) {
		goto SBOOT_FAIL;
	}

	/* 3.4 validate signature */
	ret = SBOOT_Validate_Signature(AuthAlg, HashAlg, Cert->SBPubKey, (u8 *)Cert, Cert->TableSize, (void *)&Signature[idx]);
	if (ret != 0) {
		goto SBOOT_FAIL;
	}

	return ret;

SBOOT_FAIL:
	RTK_LOGE(TAG, "Certificate VERIFY FAIL, ret = %d\n", ret);
	return ret;
}

u8 BOOT_Extract_SignatureCheck(Manifest_TypeDef *Manifest, SubImgInfo_TypeDef *SubImgInfo, u8 SubImgNum)
{
	u8 PubKeyHash[32];
	int ret;
	u8 AuthAlg, HashAlg;

	/* 1. check if secure boot enable. */
	/* 2. read public key hash from OTP if sboot en. Start with a random index to avoid side channel attack. */
	if (BOOT_SbootEn_Check(PubKeyHash) == DISABLE) {
		return 0;
	}

	/* 3. verify signature */
	/* 3.1 Initialize hash engine */
	CRYPTO_SHA_Init(NULL);

	/* 3.2 Check algorithm from flash against OTP configuration if need. */
	ret = SBOOT_Validate_Algorithm(&AuthAlg, &HashAlg, Manifest->AuthAlg, Manifest->HashAlg);
	if (ret != 0) {
		goto SBOOT_FAIL;
	}

	/* 3.3 validate pubkey hash */
	ret = SBOOT_Validate_PubKey(AuthAlg, Manifest->SBPubKey, PubKeyHash);
	if (ret != 0) {
		goto SBOOT_FAIL;
	}

	/* 3.4 validate signature */
	ret = SBOOT_Validate_Signature(AuthAlg, HashAlg, Manifest->SBPubKey, (u8 *)Manifest, sizeof(Manifest_TypeDef) - SIGN_MAX_LEN,
								   Manifest->Signature);
	if (ret != 0) {
		goto SBOOT_FAIL;
	}

	/* 3.5 calculate and validate image hash */
	ret = SBOOT_Validate_ImgHash(HashAlg, Manifest->ImgHash, SubImgInfo, SubImgNum);
	if (ret != 0) {
		goto SBOOT_FAIL;
	}

	RTK_LOGI(TAG, "Compressed Img VERIFY PASS\n");
	return TRUE;

SBOOT_FAIL:
	RTK_LOGE(TAG, "Compressed Img VERIFY FAIL, ret = %d\n", ret);
	return FALSE;
}

/**
  * @brief  Clear PQC used stack with random SRAM values
  * @param  stack_top_addr: Top address of PQC stack (MSP value)
  * @param  size: Size of PQC stack to clear
  * @retval 0 on success
  * @note   Copies random values from SRAM to clear sensitive data
  */
static int SBOOT_Clear_PQC_Stack(u32 stack_top_addr, u32 size)
{
	u32 pqc_msp_bottom = stack_top_addr - size;
	u32 rand_val_size = 1024 * 10;
	u32 rand_val_addr = pqc_msp_bottom - rand_val_size;
	u32 cleared_size = 0;

	/* clear pqc used stack with rand SRAM value */
	while (pqc_msp_bottom < stack_top_addr) {
		if (stack_top_addr - pqc_msp_bottom > rand_val_size) {
			cleared_size = rand_val_size;
		} else {
			cleared_size = stack_top_addr - pqc_msp_bottom;
		}

		_memcpy((void *)pqc_msp_bottom, (void *)rand_val_addr, cleared_size);
		pqc_msp_bottom += cleared_size;
	}

	return 0;
}

/**
  * @brief  Validate PQC public key hash (ML-DSA-65)
  * @param  PubKey: PQC public key (1952 bytes for ML-DSA-65)
  * @param  PubKeySize: Public key size
  * @param  ExpectedHash: Expected hash from OTP (32 bytes)
  * @retval 0: Success, -1: Fail
  * @note   ROM's SBOOT_Validate_PubKey only supports EDDSA, this is PQC version
  */
static int SBOOT_Validate_PubKey_PQC(u8 *PubKey, u32 PubKeySize, u8 *ExpectedHash)
{
	ALIGNMTO(CACHE_LINE_SIZE) u8 CalcHash[CACHE_LINE_ALIGNMENT(32)];
	u8 i, j, error = 0;
	int ret;

	/* Initialize SHA engine (required for AmebaDplus) */
	CRYPTO_SHA_Init(NULL);

	/* Calculate SHA-256 hash of PQC public key */
	ret = rtl_crypto_sha2(SHA2_256, PubKey, PubKeySize, CalcHash);
	if (ret != 0) {
		RTK_LOGE(TAG, "PQC: Hash calculation FAIL (ret=%d)\n", ret);
		return -1;
	}

	/* Compare with random index for side-channel protection */
	j = _rand() % 32;
	for (i = 0; i < 32; i++, j = (j + 1) % 32) {
		error |= CalcHash[j] ^ ExpectedHash[j];
	}

	if (error || i != 32) {
		RTK_LOGE(TAG, "PQC: Hash mismatch (error=0x%02x)\n", error);
		return -1;
	}

	return 0;
}

/**
  * @brief  Read PQC public key hash from OTP at SEC_PKKEY_PK2_0 (0x340)
  * @param  pk_hash: Output buffer for PQC public key hash (32 bytes)
  * @retval 0: Success, -1: Fail
  * @note   Read with random index for side-channel protection
  */
static int BOOT_Read_PQC_PubKeyHash_OTP(u8 *pk_hash)
{
	u8 i, j;
	u32 PQC_PKHash_ADDR = SEC_PKKEY_PK2_0;  /* Fixed address 0x340 */

	/* Read public key hash from OTP with random index (side-channel protection) */
	j = _rand() % 32;
	for (i = 0; i < 32; i++, j = (j + 1) % 32) {
		pk_hash[j] = HAL_READ8(OTPC_REG_BASE, PQC_PKHash_ADDR + j);
	}

	return 0;
}

/**
  * @brief  Check if PQC secure boot is enabled based on manifest version
  * @param  Manifest: Pointer to Manifest to check version
  * @retval SecureBootEn_PQC status (ENABLE or DISABLE)
  * @note   PQC is enabled when Manifest->Ver >= MANIFEST_VERSION_8KB (2)
  */
u8 BOOT_PQC_En_Check(Manifest_TypeDef *Manifest)
{
	/* PQC verification is enabled when manifest version >= 2 */
	if (Manifest->Ver >= MANIFEST_VERSION_8KB) {
		SecureBootEn_PQC = ENABLE;
		RTK_LOGI(TAG, "IMG2 SBOOT PQC EN\n");
	} else {
		SecureBootEn_PQC = DISABLE;
	}

	return SecureBootEn_PQC;
}

/**
  * @brief  Execute PQC Certificate verification with stack switch
  * @param  PQC_PubKeyHash: PQC public key hash from OTP (32 bytes)
  * @param  Cert: Pointer to Certificate
  * @param  ImgAddr: Certificate image address
  * @retval None (result stored in global variable g_pqc_verify_ret)
  * @note   Actual PQC verification using PQCLEAN_MLDSA65_CLEAN_crypto_sign_verify_ctx
  * @note   Uses global variable for return value to prevent loss during stack switch
  * @note   Uses __NO_INLINE to prevent compiler optimization issues with stack switch
  */
__NO_INLINE
static void SBOOT_CertificateVerify_PQC_Execute(u8 *PQC_PubKeyHash, Certificate_TypeDef *Cert, u32 ImgAddr)
{
	extern volatile int g_pqc_verify_ret;
	Certificate_TypeDef_Ext Cert_Ext;
	u32 ext_cert_addr;

	g_pqc_verify_ret = SBOOT_ERR_AUTH_NOT_SUPPORT;  /* Initialize to failure */

	/* 1. Copy basic certificate first */
	_memset(&Cert_Ext, 0, sizeof(Certificate_TypeDef_Ext));
	_memcpy(&Cert_Ext.basic_cert, Cert, sizeof(Certificate_TypeDef));

	/* 2. Calculate and copy PQC extension address */
	/* 8KB certificate: PQC extension is after signature */
	ext_cert_addr = ImgAddr + Cert->TableSize + SIGN_MAX_LEN;
	_memcpy((void *)&Cert_Ext.PQC_AuthAlg, (void *)ext_cert_addr,
			sizeof(Certificate_TypeDef_Ext) - sizeof(Certificate_TypeDef));

	/* 3. Check PQC algorithm and sizes - only ML-DSA-65 supported */
	if ((Cert_Ext.PQC_AuthAlg != AUTHID_PQC_ML_DSA_65) ||
		(Cert_Ext.PQC_KeySize != PQC_ML_DSA_65_PUBKEY_LEN) ||
		(Cert_Ext.PQC_SigSize != PQC_ML_DSA_65_SIG_LEN)) {
		RTK_LOGE(TAG, "PQC: Unsupported algorithm or sizes (Alg=%d, KeySize=%d, SigSize=%d)\n",
				 Cert_Ext.PQC_AuthAlg, Cert_Ext.PQC_KeySize, Cert_Ext.PQC_SigSize);
		g_pqc_verify_ret = SBOOT_ERR_AUTH_ALG_NOT_MATCH;
		return;
	}

	/* 4. Validate PQC pubkey hash against OTP using PQC-specific function */
	g_pqc_verify_ret = SBOOT_Validate_PubKey_PQC((u8 *)&Cert_Ext.PQCPubKey, Cert_Ext.PQC_KeySize, PQC_PubKeyHash);
	if (g_pqc_verify_ret != 0) {
		RTK_LOGE(TAG, "PQC public key hash validation FAIL\n");
		return;
	}

	/* 5. PQC Certificate verification for ML-DSA-65 */
	/* Signature: (u8 *)&Cert_Ext.PQCSignature, Cert_Ext.PQC_SigSize */
	/* Message: (u8 *)Cert, Cert->TableSize */
	/* Context: NULL, 0 (no context string) */
	/* Public key: (u8 *)&Cert_Ext.PQCPubKey */
	g_pqc_verify_ret = PQCLEAN_MLDSA65_CLEAN_crypto_sign_verify_ctx(
						   (u8 *)&Cert_Ext.PQCSignature, Cert_Ext.PQC_SigSize,
						   (u8 *)Cert, Cert->TableSize,
						   NULL, 0,
						   (u8 *)&Cert_Ext.PQCPubKey);

	if (g_pqc_verify_ret != 0) {
		RTK_LOGE(TAG, "PQC Certificate signature verify FAIL, ret=%d\n", g_pqc_verify_ret);
		g_pqc_verify_ret = SBOOT_ERR_SIGN_INVALID;
		return;
	}

	g_pqc_verify_ret = 0;
}

/**
  * @brief  PQC Certificate verification with stack switching
  * @param  PQC_PubKeyHash: PQC public key hash from OTP (32 bytes)
  * @param  Cert: Pointer to Certificate
  * @param  ImgAddr: Certificate image address
  * @retval Verification result (0 = success, -1 = failure)
  * @note   Uses global variable g_pqc_verify_ret to prevent return value loss during stack switch
  * @note   Uses __NO_INLINE to prevent compiler optimization issues with manual stack switch
  */
__NO_INLINE
static int SBOOT_CertificateVerify_PQC(u8 *PQC_PubKeyHash, Certificate_TypeDef *Cert, u32 ImgAddr)
{
	extern volatile int g_pqc_verify_ret;
	u32 msp_bk;

	/* 1. Save current MSP */
	msp_bk = __get_MSP();

	/* 2. Set new MSP to end of BD RAM (PQC uses last 64KB) */
	__set_MSP((u32)__km4_bd_ram_end__  | (u32)TZ_IDAU_SEC_OFFSET);
	__ISB();

	/* 3. Execute PQC verification in new stack (result stored in g_pqc_verify_ret) */
	SBOOT_CertificateVerify_PQC_Execute(PQC_PubKeyHash, Cert, ImgAddr);

	/* 4. Restore MSP */
	__set_MSP(msp_bk);
	__ISB();

	/* 5. Clear PQC used stack with random SRAM value */
	SBOOT_Clear_PQC_Stack((u32)__km4_bd_ram_end__  | (u32)TZ_IDAU_SEC_OFFSET, SBOOT_PQC_STACK_SIZE);

	return g_pqc_verify_ret;
}

/**
  * @brief  Check PQC Certificate (if manifest version >= 2)
  * @param  Manifest: Pointer to Manifest (to check version)
  * @param  Cert: Pointer to Certificate
  * @param  idx: Slot index (0 or 1)
  * @retval Verification result (0 = success, non-zero = failure)
  * @note   Uses __NO_INLINE to prevent compiler optimization issues
  *         with manual stack switching in SBOOT_CertificateVerify_PQC
  */
__NO_INLINE
int BOOT_CertificateCheck_PQC(Manifest_TypeDef *Manifest, Certificate_TypeDef *Cert, u32 idx)
{
	/* PQC public key hash buffer (non-static, protected by noinline) */
	u8 PQC_PubKeyHash[32];
	int ret;
	extern u32 OTA_Region[IMG_TYPE_CNT][2];

	/* 1. Check if PQC enabled based on manifest version, skip if version < 2 */
	if (BOOT_PQC_En_Check(Manifest) == DISABLE) {
		return 0;  /* PQC disabled (version < 2) is OK */
	}

	/* 2. Read PQC public key hash from OTP at 0x340 */
	if (BOOT_Read_PQC_PubKeyHash_OTP(PQC_PubKeyHash) != 0) {
		RTK_LOGE(TAG, "PQC public key hash read from OTP FAIL\n");
		return -1;
	}

	/* 3. Verify certificate signature with PQC (includes public key hash validation) */
	ret = SBOOT_CertificateVerify_PQC(PQC_PubKeyHash, Cert, OTA_Region[IMG_CERT][idx]);

	if (ret != 0) {
		RTK_LOGE(TAG, "PQC Certificate VERIFY FAIL, ret = %d\n", ret);
		return ret;
	}

	return ret;
}

/**
  * @brief  Execute PQC Manifest verification with stack switch
  * @param  PQC_PubKeyHash: PQC public key hash from Certificate PKInfo entry (32 bytes)
  * @param  Manifest: Pointer to Manifest
  * @param  ManifestAddr: Manifest image address
  * @retval None (result stored in global variable g_pqc_verify_ret)
  * @note   Actual PQC verification using PQCLEAN_MLDSA65_CLEAN_crypto_sign_verify_ctx
  * @note   Uses global variable for return value to prevent loss during stack switch
  * @note   Uses __NO_INLINE to prevent compiler optimization issues with stack switch
  */
__NO_INLINE
static void SBOOT_SignatureVerify_PQC_Execute(u8 *PQC_PubKeyHash, Manifest_TypeDef *Manifest, u32 ManifestAddr)
{
	extern volatile int g_pqc_verify_ret;
	Manifest_PQC_TypeDef pqc_manifest;
	u32 ext_manifest_addr;
	size_t pqc_msg_length;

	g_pqc_verify_ret = SBOOT_ERR_AUTH_NOT_SUPPORT;  /* Initialize to failure */

	/* 1. Copy PQC extension from flash */
	/* For 8KB manifest: extension is after basic manifest structure */
	ext_manifest_addr = ManifestAddr + sizeof(Manifest_TypeDef);
	_memcpy(&pqc_manifest, (void *)ext_manifest_addr, sizeof(Manifest_PQC_TypeDef));

	/* 2. Check PQC algorithm and sizes - only ML-DSA-65 supported */
	if ((pqc_manifest.PQC_AuthAlg != AUTHID_PQC_ML_DSA_65) ||
		(pqc_manifest.PQC_KeySize != PQC_ML_DSA_65_PUBKEY_LEN) ||
		(pqc_manifest.PQC_SigSize != PQC_ML_DSA_65_SIG_LEN)) {
		RTK_LOGE(TAG, "PQC: Unsupported algorithm or sizes (Alg=%d, KeySize=%d, SigSize=%d)\n",
				 pqc_manifest.PQC_AuthAlg, pqc_manifest.PQC_KeySize, pqc_manifest.PQC_SigSize);
		g_pqc_verify_ret = SBOOT_ERR_AUTH_ALG_NOT_MATCH;
		return;
	}

	/* 3. Validate PQC pubkey hash against certificate PKInfo entry using PQC-specific function */
	g_pqc_verify_ret = SBOOT_Validate_PubKey_PQC((u8 *)&pqc_manifest.PQCPubKey, pqc_manifest.PQC_KeySize, PQC_PubKeyHash);
	if (g_pqc_verify_ret != 0) {
		RTK_LOGE(TAG, "PQC public key hash validation FAIL\n");
		return;
	}

	/* 4. Calculate message length (same as ECC verification) */
	pqc_msg_length = sizeof(Manifest_TypeDef) - SIGN_MAX_LEN;

	/* 5. PQC Manifest verification for ML-DSA-65 */
	/* Signature: pqc_manifest.PQCSignature, pqc_manifest.PQC_SigSize */
	/* Message: (u8 *)Manifest, pqc_msg_length */
	/* Context: NULL, 0 (no context string) */
	/* Public key: pqc_manifest.PQCPubKey */
	g_pqc_verify_ret = PQCLEAN_MLDSA65_CLEAN_crypto_sign_verify_ctx(
						   pqc_manifest.PQCSignature, pqc_manifest.PQC_SigSize,
						   (u8 *)Manifest, pqc_msg_length,
						   NULL, 0,
						   pqc_manifest.PQCPubKey);

	if (g_pqc_verify_ret != 0) {
		RTK_LOGE(TAG, "PQC Manifest signature verify FAIL, ret=%d\n", g_pqc_verify_ret);
		g_pqc_verify_ret = SBOOT_ERR_SIGN_INVALID;
		return;
	}

	g_pqc_verify_ret = 0;
}

/**
  * @brief  PQC Manifest verification with stack switching
  * @param  PQC_PubKeyHash: PQC public key hash from Certificate PKInfo entry (32 bytes)
  * @param  Manifest: Pointer to Manifest
  * @param  ManifestAddr: Manifest image address
  * @retval Verification result (0 = success, -1 = failure)
  * @note   Uses global variable g_pqc_verify_ret to prevent return value loss during stack switch
  * @note   Uses __NO_INLINE to prevent compiler optimization issues with stack switch
  */
__NO_INLINE
static int SBOOT_SignatureVerify_PQC(u8 *PQC_PubKeyHash, Manifest_TypeDef *Manifest, u32 ManifestAddr)
{
	extern volatile int g_pqc_verify_ret;
	u32 msp_bk;

	/* 1. Save current MSP */
	msp_bk = __get_MSP();

	/* 2. Set new MSP to end of BD RAM (PQC uses last 64KB) */
	__set_MSP((u32)__km4_bd_ram_end__  | (u32)TZ_IDAU_SEC_OFFSET);
	__ISB();

	/* 3. Execute PQC verification in new stack (result stored in g_pqc_verify_ret) */
	SBOOT_SignatureVerify_PQC_Execute(PQC_PubKeyHash, Manifest, ManifestAddr);

	/* 4. Restore MSP */
	__set_MSP(msp_bk);
	__ISB();

	/* 5. Clear PQC used stack with random SRAM value */
	SBOOT_Clear_PQC_Stack((u32)__km4_bd_ram_end__  | (u32)TZ_IDAU_SEC_OFFSET, SBOOT_PQC_STACK_SIZE);

	return g_pqc_verify_ret;
}

/**
  * @brief  Verify PQC signature for Manifest (if manifest version >= 2)
  * @param  Manifest: Pointer to Manifest
  * @param  Cert: Pointer to Certificate (to get PQC pubkey hash from PKInfo)
  * @param  idx: Slot index (0 or 1)
  * @param  KeyID: Key ID to find in Certificate PKInfo (KEYID_NSPE_PQC = 1)
  * @retval Verification result (0 = success, non-zero = failure)
  */
int BOOT_SignatureCheck_PQC(Manifest_TypeDef *Manifest, Certificate_TypeDef *Cert, u32 idx, u32 KeyID)
{
	int ret;
	u8 i;
	u8 *PQC_PubKeyHash = NULL;
	extern u32 OTA_Region[IMG_TYPE_CNT][2];

	/* 1. Check if PQC is enabled */
	if (SecureBootEn_PQC == DISABLE) {
		return 0;  /* PQC disabled is OK */
	}

	/* 2. Find PQC public key hash from certificate entry using KeyID */
	for (i = 0; i < Cert->EntryNum; i++) {
		if (Cert->PKInfo[i].KeyID == KeyID) {  /* KeyID = KEYID_NSPE_PQC = 1 */
			PQC_PubKeyHash = Cert->PKInfo[i].Hash;
			break;
		}
	}

	if (PQC_PubKeyHash == NULL) {
		RTK_LOGE(TAG, "PQC public key hash not found in certificate (KeyID=%d)\n", KeyID);
		return -1;
	}

	/* 3. Verify manifest signature with PQC using hash from certificate entry */
	ret = SBOOT_SignatureVerify_PQC(PQC_PubKeyHash, Manifest, OTA_Region[IMG_IMG2][idx]);

	if (ret != 0) {
		RTK_LOGE(TAG, "PQC Manifest VERIFY FAIL, ret = %d\n", ret);
		return ret;
	}

	return ret;
}
