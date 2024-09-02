/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "boot_security_km4.h"
#include "boot_ota_km4.h"

static const char *TAG = "BOOT";
extern u32 Cert_PKHash_OTP_ADDR;
extern u8 Signature[2][SIGN_MAX_LEN];
u8 SecureBootEn = DISABLE;

int BOOT_DecRDPImg(u32 SrcAddr, u8 *RdpIV, SubImgInfo_TypeDef *SubImgInfo, u8 *Cnt)
{
	IMAGE_HEADER *Hdr = NULL;
	u8 InBuf[256];
	ALIGNMTO(CACHE_LINE_SIZE) u8 Outbuf[CACHE_LINE_ALIGMENT(16)];
	u8 Key[32], TmpIV[16];
	u32 DestAddr, TotalLen;
	u32 i = 0, sz;
	char *Km4Img3Label[2] = {"KM4 IMG3", "KM4 NSC"};
	u8 SubImgCnt = sizeof(Km4Img3Label) / sizeof(char *);

	*Cnt = SubImgCnt;

	/* check if RDP image should decrypt. If not, copy to secure RAM directly */
	if (BOOT_LoadSubImage(SubImgInfo, SrcAddr, SubImgCnt, Km4Img3Label, _FALSE) == _TRUE) {
		return TRUE;
	}

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
		if ((Hdr->signature[0] != 0x35393138) || (Hdr->signature[1] != 0x31313738)) {
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

	/* 2.5 calculate and validate image hash */
	ret = SBOOT_Validate_ImgHash(HashAlg, Manifest->ImgHash, SubImgInfo, SubImgNum);
	if (ret != 0) {
		goto SBOOT_FAIL;
	}

	RTK_LOGI(TAG, "%s VERIFY PASS\n", Name[Manifest->ImgID]);
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
