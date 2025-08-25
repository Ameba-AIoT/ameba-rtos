/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "boot_security_km4tz.h"
#include "boot_ota_km4tz.h"

static const char *const TAG = "BOOT";
extern u32 Cert_PKHash_OTP_ADDR;
extern u8 Signature[2][SIGN_MAX_LEN];
u8 SecureBootEn = DISABLE;

fih_ret BOOT_ROM_OTFCheck(u32 start_addr, u32 end_addr, u32 IV_index, u32 OTF_index, u8 RSIP_KEY_ID);

int BOOT_DecRDPImg(u32 SrcAddr, u8 *RdpIV, SubImgInfo_TypeDef *SubImgInfo, u8 *Cnt)
{
	FIH_DECLARE(fih_rc, FIH_FAILURE);
	char *Img3Label[2] = {"AP IMG3", "AP NSC"};
	u8 SubImgCnt = sizeof(Img3Label) / sizeof(char *);

	u32 PhyAddr = SrcAddr;
	u32 LogAddr = (u32)__km4tz_img3_text_start__ - IMAGE_HEADER_LEN;
	u32 ImgAddr = SYSCFG_OTP_BootFromNor() ? LogAddr : PhyAddr;

	*Cnt = SubImgCnt;

	/* config MMU and OTF */
	RSIP_MMU_Config(MMU_ID3, LogAddr, (u32)__km4tz_img3_text_end__, PhyAddr);
	RSIP_MMU_Cmd(MMU_ID3, ENABLE);
	RSIP_MMU_Cache_Clean();

	/* check RSIP enable bit and set RSIP */
	if (FIH_NOT_EQ(FALSE, SYSCFG_OTP_RSIPEn())) {
		RSIP_IV_Set(RSIP_IV3, RdpIV);
		FIH_CALL(BOOT_ROM_OTFCheck, fih_rc, LogAddr, (u32)__km4tz_img3_text_end__, RSIP_IV3, RSIP_REGION3, RSIP_KEY_NUM0);
	}

	/* Load or XIP image */
	if (BOOT_LoadSubImage(SubImgInfo, ImgAddr, SubImgCnt, Img3Label, TRUE) == TRUE) {
		return TRUE;
	} else {
		return FALSE;
	}
}

fih_ret BOOT_SignatureCheck(Manifest_TypeDef *Manifest, SubImgInfo_TypeDef *SubImgInfo, u8 SubImgNum, Certificate_TypeDef *Cert, u32 KeyID)
{
	FIH_DECLARE(fih_rc, FIH_FAILURE);
	u8 i, j;
	char *Name[] = {"IMG1", "IMG2", "IMG3"};
	u8 AuthAlg, HashAlg;
	IMAGE_HEADER ImgHdr[2];

	if (!SYSCFG_OTP_BootFromNor()) {
		for (i = 0, j = 0; i < SubImgNum; i++) { /* copy to sram because Nand not support automode even sboot not enabled */
			if (IS_FLASH_ADDR(SubImgInfo[i].Addr)) {
				assert_param(SubImgInfo[i].Len == IMAGE_HEADER_LEN); /* Nand not support xip */

				BOOT_ImgCopy((void *)&ImgHdr[j], (void *)SubImgInfo[i].Addr, IMAGE_HEADER_LEN);
				DCache_CleanInvalidate((u32)&ImgHdr[j], IMAGE_HEADER_LEN);
				SubImgInfo[i].Addr = (u32)&ImgHdr[j];
				j++;
				assert_param(j <= sizeof(ImgHdr) / sizeof(IMAGE_HEADER));
			}
		}
	}

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

	/* 2.5 calculate and validate image hash */
	FIH_CALL(SBOOT_Validate_ImgHash, fih_rc, HashAlg, Manifest->ImgHash, SubImgInfo, SubImgNum);
	if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
		goto SBOOT_FAIL;
	}

	RTK_LOGI(TAG, "%s VERIFY PASS\n", Name[Manifest->ImgID]);
	FIH_RET(fih_rc);

SBOOT_FAIL:
	RTK_LOGE(TAG, "%s VERIFY FAIL, ret = %d\n", Name[Manifest->ImgID], fih_rc);

	/* clear copied image */
	for (i = 0; i < SubImgNum; i++) {
		if (!IS_FLASH_ADDR(SubImgInfo[i].Addr)) {
			_memset((void *)SubImgInfo[i].Addr, 0, SubImgInfo[i].Len);
			DCache_CleanInvalidate(SubImgInfo[i].Addr, SubImgInfo[i].Len);
		}
	}

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
}
