/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "boot_security_km4ns_solo.h"
#include "boot_ota_km4ns_solo.h"

static const char *const TAG = "BOOT";
extern u32 Cert_PKHash_OTP_ADDR;
extern u8 Signature[2][SIGN_MAX_LEN];
volatile u8 SecureBootEn = DISABLE;
volatile u8 SecureBootEn_PQC = DISABLE;
volatile global_cfi_t global_cfi;

fih_ret BOOT_ROM_OTFCheck(u32 start_addr, u32 end_addr, u32 IV_index, u32 OTF_index,
						  u8 RSIP_KEY_ID, u32 GCM_TagBase, u8 RSIPConfig, u8 img_index)
{
	FIH_DECLARE(fih_rc, FIH_FAILURE);
	u32 otp_value = RSIP_INVALID_MODE;
	u32 manifest_value = MANIFEST_RSIP_INVALID_MODE;
	u32 rsip_mode = OTF_INVALID_MODE;
	u8 GCM_tag_length_byte = 0;

	/* 1. check if RSIP enable */
	if (FIH_NOT_EQ(TRUE, SYSCFG_OTP_RSIPEn())) {
		FIH_RET(FIH_SUCCESS);
	}

	/* check if manifest is invalid */
	manifest_value = MANIFEST_RSIP_GET_IMG_MODE(img_index, RSIPConfig);
	if (manifest_value == MANIFEST_RSIP_INVALID_MODE) {
		FIH_RET(SBOOT_ERR_OTF_CHK_FAIL);
	}

	/* 2. get RSIP mode */
	otp_value = SYSCFG_OTP_RSIPMode();
	if (otp_value == RSIP_GCM_MODE) {
		if (manifest_value != MANIFEST_RSIP_GCM_MODE) {
			FIH_RET(SBOOT_ERR_OTF_CHK_FAIL);
		}
		rsip_mode = OTF_GCM_MODE;
	} else if (otp_value == RSIP_XTS_MODE) {
		if (manifest_value != MANIFEST_RSIP_XTS_MODE) {
			FIH_RET(SBOOT_ERR_OTF_CHK_FAIL);
		}
		rsip_mode = OTF_XTS_MODE;
	} else {
		if (manifest_value != MANIFEST_RSIP_CTR_MODE) {
			FIH_RET(SBOOT_ERR_OTF_CHK_FAIL);
		}
		rsip_mode = OTF_CTR_MODE;
	}

	/* 3. if gcm mode, set GCM tag size */
	if (rsip_mode == OTF_GCM_MODE) {
		GCM_tag_length_byte = 1 << (MANIFEST_RSIP_CFG_GET_GCM_TAG_LEN(RSIPConfig) + 2);
		RSIP_OTF_Set_GCM_Tag_Length(GCM_tag_length_byte);
	}

	/* 4. enable otf entry */
	RSIP_OTF_Enable(OTF_index, start_addr, end_addr, ENABLE, IV_index, RSIP_KEY_ID, rsip_mode, GCM_TagBase);
	RSIP_OTF_Cmd(ENABLE);

	RTK_LOGI(TAG, "IMG%d OTF EN, Set: %u\n", img_index, rsip_mode);
	FIH_CALL(global_cfi_increase, fih_rc, &global_cfi, CFI_STEP_OTF);

	FIH_RET(fih_rc);
}

fih_ret BOOT_SignatureCheck(Manifest_TypeDef *Manifest, Certificate_TypeDef *Cert, u32 KeyID)
{
	FIH_DECLARE(fih_rc, FIH_FAILURE);
	u8 i;
	u8 AuthAlg, HashAlg;

	/* 1. check if secure boot enable. */
	if (FIH_EQ(DISABLE, SecureBootEn)) {
		FIH_RET(FIH_SUCCESS);
	}

	/* 2. verify signature */
	/* 2.2 Check algorithm from manifest against OTP configuration if need. */
	FIH_CALL(SBOOT_Validate_Algorithm, fih_rc, &AuthAlg, &HashAlg, Manifest->AuthAlg, Manifest->HashAlg);
	if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
		goto SBOOT_FAIL;
	}

	/* 2.3 validate pubkey hash */
	for (i = 0; i < Cert->EntryNum; i++) {
		if (Cert->PKInfo[i].KeyID == KeyID) {
			break;
		}
	}

	FIH_CALL(SBOOT_Validate_PubKey, fih_rc, AuthAlg, Manifest->SBPubKey, Cert->PKInfo[i].Hash);
	if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
		goto SBOOT_FAIL;
	}

	/* 2.4 validate signature */
	FIH_CALL(SBOOT_Validate_Signature, fih_rc, AuthAlg, HashAlg, Manifest->SBPubKey, (u8 *)Manifest, sizeof(Manifest_TypeDef) - SIGN_MAX_LEN,
			 Manifest->Signature);
	if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
		goto SBOOT_FAIL;
	}

SBOOT_FAIL:
	FIH_RET(fih_rc);
}

static u8 BOOT_SbootEn_Check(u8 *pk_hash)
{
	u8 i, j;

	if (FIH_NOT_EQ(FALSE, SYSCFG_OTP_RMAMode())) {
		j = _rand() % 32;
		for (i = 0; i < 32; i++, j = (j + 1) % 32) {
			/* 2. read public key hash from OTP. Start with a random index to avoid side channel attack. */
			/* If in RMA mode but RMA Pubkey is not programed, skip SBOOT */
			pk_hash[j] = HAL_READ8(OTPC_REG_BASE, SEC_PKKEY1_RMA_PK_0 + j);
			if (pk_hash[j] != 0xFF) {
				SecureBootEn++;
			}
		}
	} else {
		/* 1. check if secure boot enable if not in RMA mode. */
		if (FIH_EQ(FALSE, SYSCFG_OTP_SBootEn())) {
			return SecureBootEn;
		} else {
			FIH_SET(SecureBootEn, ENABLE);
		}

		/* 2. read public key hash from OTP. Start with a random index to avoid side channel attack. */
		j = _rand() % 32;
		for (i = 0; i < 32; i++, j = (j + 1) % 32) {
			pk_hash[j] = HAL_READ8(OTPC_REG_BASE, Cert_PKHash_OTP_ADDR + j);
		}
	}

	return SecureBootEn;
}

fih_ret BOOT_CertificateCheck(Certificate_TypeDef *Cert, u32 idx)
{
	u8 PubKeyHash[32];
	FIH_DECLARE(fih_rc, FIH_FAILURE);
	u8 AuthAlg, HashAlg;

	/* 1. check if secure boot enable. */
	/* 2. read public key hash from OTP if sboot en. Start with a random index to avoid side channel attack. */
	if (FIH_EQ(DISABLE, BOOT_SbootEn_Check(PubKeyHash))) {
		FIH_RET(FIH_SUCCESS);
	}

	/* 3. verify signature */
	/* 3.1 Initialize hash engine */

	/* 3.2 Check algorithm from flash against OTP configuration if need. */
	FIH_CALL(SBOOT_Validate_Algorithm, fih_rc, &AuthAlg, &HashAlg, Cert->AuthAlg, Cert->HashAlg);
	if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
		goto SBOOT_FAIL;
	}

	/* 3.3 validate pubkey hash */
	FIH_CALL(SBOOT_Validate_PubKey, fih_rc, AuthAlg, Cert->SBPubKey, PubKeyHash);
	if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
		goto SBOOT_FAIL;
	}

	/* 3.4 validate signature */
	FIH_CALL(SBOOT_Validate_Signature, fih_rc, AuthAlg, HashAlg, Cert->SBPubKey, (u8 *)Cert, Cert->TableSize, (void *)&Signature[idx]);
	if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
		goto SBOOT_FAIL;
	}

	FIH_RET(fih_rc);

SBOOT_FAIL:
	RTK_LOGE(TAG, "Certificate VERIFY FAIL, ret = %d\n", fih_rc);
	FIH_RET(fih_rc);
}

fih_ret BOOT_Extract_SignatureCheck(Manifest_TypeDef *Manifest, SubImgInfo_TypeDef *SubImgInfo, u8 SubImgNum)
{
#ifdef RTL8720F_TODO
	u8 PubKeyHash[32];
	FIH_DECLARE(fih_rc, FIH_FAILURE);
	u8 AuthAlg, HashAlg;

	/* 1. check if secure boot enable. */
	/* 2. read public key hash from OTP if sboot en. Start with a random index to avoid side channel attack. */
	if (FIH_EQ(DISABLE, BOOT_SbootEn_Check(PubKeyHash))) {
		FIH_RET(FIH_SUCCESS);
	}

	/* 3. verify signature */
	/* 3.1 Initialize hash engine */

	/* 3.2 Check algorithm from flash against OTP configuration if need. */
	FIH_CALL(SBOOT_Validate_Algorithm, fih_rc, &AuthAlg, &HashAlg, Manifest->AuthAlg, Manifest->HashAlg);
	if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
		goto SBOOT_FAIL;
	}

	/* 3.3 validate pubkey hash */
	FIH_CALL(SBOOT_Validate_PubKey, fih_rc, AuthAlg, Manifest->SBPubKey, PubKeyHash);
	if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
		goto SBOOT_FAIL;
	}

	/* 3.4 validate signature */
	FIH_CALL(SBOOT_Validate_Signature, fih_rc, AuthAlg, HashAlg, Manifest->SBPubKey, (u8 *)Manifest, sizeof(Manifest_TypeDef) - SIGN_MAX_LEN,
			 Manifest->Signature);
	if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
		goto SBOOT_FAIL;
	}

	/* 3.5 calculate and validate image hash */
	FIH_CALL(SBOOT_Validate_ImgHash, fih_rc, HashAlg, Manifest->ImgHash, SubImgInfo, SubImgNum);
	if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
		goto SBOOT_FAIL;
	}

	RTK_LOGI(TAG, "Compressed Img VERIFY PASS\n");
	FIH_RET(fih_rc);

SBOOT_FAIL:
	RTK_LOGE(TAG, "Compressed Img VERIFY FAIL, ret = %d\n", fih_rc);
	FIH_RET(fih_rc);
#else
	UNUSED(Manifest);
	UNUSED(SubImgInfo);
	UNUSED(SubImgNum);

	RTK_LOGE(TAG, "Fullmac PQC sboot cannot be verified at this time (PQC stack conflict). "
			 "The image2 load method needs to be adjusted.\n");

	FIH_RET(FIH_FAILURE);
#endif
}

static u8 BOOT_SbootEn_Check_PQC(u8 *pk_hash)
{
	u8 i, j;
	u32 PQC_PKHash_ADDR = SEC_PKKEY_PK2_0;

	/* 1. check if secure boot PQC enable. */
	if (FIH_EQ(RTK_PSA_LIFECYCLE_RMA, SYSCFG_OTP_RMAMode())) {
		/* In RMA mode, bypass PQC verification and retain only traditional ECC verification. */
		SecureBootEn_PQC = DISABLE;
		goto exit;
	} else {
		/* If in Normal mode and PQC secure boot is not enable, skip */
		if (FIH_EQ(FALSE, SYSCFG_OTP_SBootEn_PQC())) {
			SecureBootEn_PQC = DISABLE;
			goto exit;
		}

		/* 2. read public key hash from OTP. Start with a random index to avoid side channel attack. */
		j = _rand() % 32;
		for (i = 0; i < 32; i++, j = (j + 1) % 32) {
			pk_hash[j] = HAL_READ8(OTPC_REG_BASE, PQC_PKHash_ADDR + j);
			SecureBootEn_PQC ++;
		}
	}
	RTK_LOGI(TAG, "IMG2 SBOOT PQC EN\n");

exit:
	return SecureBootEn_PQC;
}

/**
  * @brief  Execute PQC Certificate verification with stack switch
  * @param  PQC_PubKeyHash: PQC public key hash from OTP (32 bytes)
  * @param  Cert: Pointer to Certificate
  * @param  ImgAddr: Certificate image address
  * @retval None (result stored in global variable g_pqc_verfiy_fih_rc)
  * @note   Actual PQC verification using PQCLEAN_MLDSA65_CLEAN_crypto_sign_verify_ctx
  * @note   Uses global variable for return value to prevent loss during stack switch
  * @note   Uses __NO_INLINE to prevent compiler optimization issues with stack switch
  */
__NO_INLINE
void SBOOT_CertificateVerify_PQC_Execute(u8 *PQC_PubKeyHash, Certificate_TypeDef *Cert, u32 ImgAddr)
{
	extern volatile fih_ret g_pqc_verfiy_fih_rc;
	Certificate_TypeDef_Ext Cert_Ext;
	u8 HashAlg;
	int ret = SBOOT_ERR_AUTH_NOT_SUPPORT;
	FIH_SET(g_pqc_verfiy_fih_rc, FIH_FAILURE);
	u32 ext_cert_addr = ImgAddr + Cert->TableSize + SIGN_MAX_LEN;

	/* 1. Read extension part of certificate */
	_memcpy(&Cert_Ext.basic_cert, Cert, sizeof(Certificate_TypeDef));
	BOOT_ImgCopy((void *)((u8 *)&Cert_Ext + sizeof(Certificate_TypeDef)),
				 (void *)ext_cert_addr,
				 sizeof(Certificate_TypeDef_Ext) - sizeof(Certificate_TypeDef));

	/* 2. Check HASH algo. */
	FIH_CALL(SBOOT_Validate_HASH_Algorithm, g_pqc_verfiy_fih_rc, &HashAlg, Cert_Ext.basic_cert.HashAlg);
	if (FIH_NOT_EQ(g_pqc_verfiy_fih_rc, FIH_SUCCESS)) {
		return;
	}

	/* 3. validate PQC pubkey hash */
	FIH_CALL(SBOOT_Validate_PubKey, g_pqc_verfiy_fih_rc, Cert_Ext.PQC_AuthAlg, (u8 *)&Cert_Ext.PQCPubKey, PQC_PubKeyHash);
	if (FIH_NOT_EQ(g_pqc_verfiy_fih_rc, FIH_SUCCESS)) {
		return;
	}

	/* 4. PQC Certificate verification for ML-DSA-65 */
	FIH_SET(g_pqc_verfiy_fih_rc, FIH_FAILURE);
	if (IS_PQC_ML_DSA_65(Cert_Ext.PQC_AuthAlg)) {
		ret = PQCLEAN_MLDSA65_CLEAN_crypto_sign_verify_ctx((u8 *)&Cert_Ext.PQCSignature, Cert_Ext.PQC_SigSize,
				(u8 *)Cert, Cert->TableSize, NULL, 0, (u8 *)&Cert_Ext.PQCPubKey);
	}
	g_pqc_verfiy_fih_rc = fih_ret_encode_zero_equality(ret);
}

/**
  * @brief  PQC Certificate verification with stack switching
  * @param  PubKeyHash: PQC public key hash from OTP (32 bytes)
  * @param  Cert: Pointer to Certificate
  * @param  ImgAddr: Certificate image address
  * @retval Verification result (FIH_SUCCESS or FIH_FAILURE)
  * @note   Uses global variable g_pqc_verfiy_fih_rc to prevent return value loss during stack switch
  * @note   Uses __NO_INLINE to prevent compiler optimization issues with manual stack switch
  */
__NO_INLINE
fih_ret SBOOT_CertificateVerify_PQC(u8 *PubKeyHash, Certificate_TypeDef *Cert, u32 ImgAddr)
{
	extern volatile fih_ret g_pqc_verfiy_fih_rc;
	FIH_DECLARE(fih_rc, FIH_FAILURE);
	FIH_SET(g_pqc_verfiy_fih_rc, FIH_FAILURE);

	/* PQC verify need large stack, MSP must less than 0x30070000 (floader use last 64K) */
	u32 msp_bk = __get_MSP();
	__set_MSP(((u32)__km4tz_bd_ram_end__ | (u32)TZ_IDAU_SEC_OFFSET));
	__ISB();

	SBOOT_CertificateVerify_PQC_Execute(PubKeyHash, Cert, ImgAddr);

	__set_MSP(msp_bk);
	__ISB();

	/* clear pqc used stack with rand SRAM value */
	FIH_CALL(SBOOT_Clear_PQC_Stack, fih_rc, ((u32)__km4tz_bd_ram_end__ | (u32)TZ_IDAU_SEC_OFFSET), SBOOT_PQC_STACK_SIZE);

	FIH_RET(g_pqc_verfiy_fih_rc);
}

/**
  * @brief  Check PQC Certificate (if certificate version >= 2)
  * @param  Cert: Pointer to Certificate
  * @param  idx: Slot index (0 or 1)
  * @retval Verification result (FIH_SUCCESS or FIH_FAILURE)
  * @note   Uses __NO_INLINE to prevent compiler optimization issues
  *         with manual stack switching in SBOOT_CertificateVerify_PQC
  */
__NO_INLINE
fih_ret BOOT_CertificateCheck_PQC(Certificate_TypeDef *Cert, u32 idx)
{
	u8 PQC_PubKeyHash[32];
	FIH_DECLARE(fih_rc, FIH_FAILURE);
	extern u32 OTA_Region[IMG_TYPE_CNT][2];

	if (FIH_EQ(DISABLE, BOOT_SbootEn_Check_PQC(PQC_PubKeyHash))) {
		FIH_RET(FIH_SUCCESS);
	}

	FIH_CALL(SBOOT_CertificateVerify_PQC, fih_rc, PQC_PubKeyHash, Cert, OTA_Region[IMG_CERT][idx]);

	FIH_RET(fih_rc);
}

fih_ret BOOT_SignatureCheck_PQC(Manifest_TypeDef *Manifest, Certificate_TypeDef *Cert, u32 idx, u32 KeyID)
{
	FIH_DECLARE(fih_rc, FIH_FAILURE);
	u8 i;
	extern u32 OTA_Region[IMG_TYPE_CNT][2];

	/* 1. check if PQC check enabled */
	if (FIH_EQ(DISABLE, SecureBootEn_PQC)) {
		FIH_RET(FIH_SUCCESS);
	}

	/* 2. validate pubkey hash */
	for (i = 0; i < Cert->EntryNum; i++) {
		if (Cert->PKInfo[i].KeyID == KeyID) {
			break;
		}
	}

	FIH_CALL(SBOOT_SignatureVerify_PQC, fih_rc, Cert->PKInfo[i].Hash, Manifest, OTA_Region[IMG_IMG2][idx], ((u32)__km4tz_bd_ram_end__ | (u32)TZ_IDAU_SEC_OFFSET));

	FIH_RET(fih_rc);
}
