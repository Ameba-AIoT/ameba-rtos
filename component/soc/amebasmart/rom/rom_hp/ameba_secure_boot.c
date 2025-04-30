#include "ameba_soc.h"
#include "ameba_secure_boot.h"
#include <boot_rom_homekit_ed25519.h>

HAL_ROM_TEXT_SECTION
void SBOOT_SwapData(u8 *Outbuf, u8 *Inbuf, u8 Len)
{
	u8 i = 0;

	for (i = 0; i < Len; i++) {
		Outbuf[i] = Inbuf[Len - 1 - i];
	}

	return;
}

HAL_ROM_TEXT_SECTION
int SBOOT_GetMdType(u8 HashId, SHA2_TYPE *MdType, u8 *IsHmac)
{
	int ret = 0;

	switch (HashId) {
	case HASHID_SHA256:
		*MdType = SHA2_256;
		*IsHmac = 0;
		break;
	case HASHID_SHA384:
		*MdType = SHA2_384;
		*IsHmac = 0;
		break;
	case HASHID_SHA512:
		*MdType = SHA2_512;
		*IsHmac = 0;
		break;
	case HASHID_HMAC256:
		*MdType = SHA2_256;
		*IsHmac = 1;
		break;
	case HASHID_HMAC384:
		*MdType = SHA2_384;
		*IsHmac = 1;
		break;
	case HASHID_HMAC512:
		*MdType = SHA2_512;
		*IsHmac = 1;
		break;
	default:
		ret = SBOOT_ERR_HASH_NOT_SUPPORT;
		break;
	}

	return ret;
}


HAL_ROM_TEXT_SECTION
int SBOOT_EDDSA_Verify_SW(u8 *Pk, u8 *Msg, u32 Len, u8 *Sig)
{
	int ret = 0;

	/* We can call ed25519_verify_signature directly here because all memory address is Secure when boot*/
	ret = boot_rom_ed25519_verify_signature(Sig, (unsigned char *)Msg, Len, Pk);
	if (ret != 0) {
		ret = SBOOT_ERR_SIGN_INVALID;
	}

	return ret;
}

HAL_ROM_TEXT_SECTION
int SBOOT_EDDSA_Verify_HW(u8 *Pk, u8 *Msg, u32 Len, u8 *Sig)
{
	hw_sha_context Context;
	u8 Hash[64];
	int ret = 0;

	_memset(&Context, 0, sizeof(hw_sha_context));

	/* calculate sha-512 hash value */
	CRYPTO_CHK(rtl_crypto_sha2_init(SHA2_512, &Context), SBOOT_ERR_EDDSA_HASH_ENGINE);
	CRYPTO_CHK(rtl_crypto_sha2_update(Sig, 32, &Context), SBOOT_ERR_EDDSA_HASH_ENGINE);
	CRYPTO_CHK(rtl_crypto_sha2_update(Pk, 32, &Context), SBOOT_ERR_EDDSA_HASH_ENGINE);
	CRYPTO_CHK(rtl_crypto_sha2_update((u8 *)Msg, Len, &Context), SBOOT_ERR_EDDSA_HASH_ENGINE);
	CRYPTO_CHK(rtl_crypto_sha2_final(Hash, &Context), SBOOT_ERR_EDDSA_HASH_ENGINE);

	/* verify */
	ret = EDDSA_Verify_HalfAutoMode(ED25519_S_BASE, Pk, Sig, Hash);
	if (ret != 0) {
		ret = SBOOT_ERR_SIGN_INVALID;
	}

exit:
	return ret;
}

HAL_ROM_TEXT_SECTION
int SBOOT_ECDSA_Verify_HW(u8 AuthAlg, u8 HashAlg, u8 *Pk, u8 *Msg, u32 Len, u8 *Sig)
{
	ecdsa_ecp_group Curve;
	size_t Csize;
	u8 HashLen;
	u8 Hash[HASH_MAX_LEN];
	SHA2_TYPE MdType;
	u8 IsHmac = 0;
	u8 r[SBOOT_ECP_MAX_BYTES];
	u8 s[SBOOT_ECP_MAX_BYTES];
	u8 Qx[SBOOT_ECP_MAX_BYTES];
	u8 Qy[SBOOT_ECP_MAX_BYTES];
	int ret = 0;

	/* get curve info from AuthID */
	Curve.curve_id = (u32)AuthAlg;
	ret = ECDSA_CurveSel(ECDSA_S_BASE, &Curve);
	if (ret != 0) {
		ret = SBOOT_ERR_ECDSA_CURVE_NOT_SUPPORT;
		goto exit;
	}

	Csize = (Curve.curve_bit + 7) / 8;

	/* load pubkey: Q */
	SBOOT_SwapData(Qx, Pk, Csize);
	SBOOT_SwapData(Qy, Pk + Csize, Csize);

	/* load signature: (r,s) */
	SBOOT_SwapData(r, Sig, Csize);
	SBOOT_SwapData(s, Sig + Csize, Csize);

	/* calculate manifest hash, utilize the same hash algorithm with image hash */
	ret = SBOOT_GetMdType(HashAlg, &MdType, &IsHmac);
	if (ret != 0) {
		goto exit;
	}

	if (IsHmac) {
		/* SHA engine auto-load HMAC key */
		CRYPTO_OTPKey_SHA_Init(1, ENABLE);
		ret = rtl_crypto_hmac_sha2(MdType, (u8 *)Msg, Len, NULL, 32, Hash);
		CRYPTO_OTPKey_SHA_Init(1, DISABLE);
	} else {
		ret = rtl_crypto_sha2(MdType, (u8 *)Msg, Len, Hash);
	}

	if (ret != 0) {
		ret = SBOOT_ERR_ECDSA_HASH_ENGINE;
		goto exit;
	}

	/* verify signature */
	HashLen = (u8)MdType;
	ret = ECDSA_Verify(ECDSA_S_BASE, AuthAlg, Hash, HashLen, Qx, Qy, r, s);
	if (ret != 0) {
		ret = SBOOT_ERR_SIGN_INVALID;
	}

exit:
	return ret;

}

HAL_ROM_TEXT_SECTION
int SBOOT_Validate_Algorithm(u8 *AuthAlg, u8 *HashAlg, u8 ManiAuth, u8 ManiHash)
{
	u8 Value = HAL_READ8(OTPC_REG_BASE, SEC_BOOT_ALG);
	u8 Temp;

	Temp = SEC_GET_AUTH_ALG(Value);
	if ((Temp != 0xF) && (Temp != ManiAuth)) {
		return SBOOT_ERR_AUTH_ALG_NOT_MATCH;
	} else {
		*AuthAlg = ManiAuth;
	}

	Temp = SEC_GET_HASH_ALG(Value);
	if ((Temp != 0xF) && (Temp != ManiHash)) {
		return SBOOT_ERR_HASH_ALG_NOT_MATCH;
	} else {
		*HashAlg = ManiHash;
	}

	return 0;
}

HAL_ROM_TEXT_SECTION
int SBOOT_Validate_Signature(u8 AuthAlg, u8 HashAlg, u8 *Pk, u8 *Msg, u32 Len, u8 *Sig)
{
	int ret = 0;

	if (IS_EDDSA(AuthAlg)) {

		if (SYSCFG_OTP_SBoot_HW_DIS() == FALSE) {
			/* ed25519 HW implementatioin, by default */
			ret = SBOOT_EDDSA_Verify_HW(Pk, Msg, Len, Sig);
		} else {
			/* ed25519 SW implementation after PG corresponding OTP bit */
			ret = SBOOT_EDDSA_Verify_SW(Pk, Msg, Len, Sig);
		}
	} else if (IS_ECDSA(AuthAlg)) {

		/* ecdsa HW implementation */
		ret = SBOOT_ECDSA_Verify_HW(AuthAlg, HashAlg, Pk, Msg, Len, Sig);
	} else {
		ret = SBOOT_ERR_AUTH_NOT_SUPPORT;
	}

	return ret;
}

HAL_ROM_TEXT_SECTION
int SBOOT_Validate_PubKey(u8 AuthAlg, u8 *Pk, u8 *Hash)
{
	ecdsa_ecp_group Curve;
	u8 CalcHash[32];
	u32 PubKeySize = 0;
	u32 Temp;
	u8 i, j, error = 0;
	int ret = 0;

	if (IS_EDDSA(AuthAlg)) {
		PubKeySize = 32;
	} else if (IS_ECDSA(AuthAlg)) {

		Curve.curve_id = (u32)AuthAlg;
		ECDSA_CurveSel(ECDSA_S_BASE, &Curve);

		Temp = (Curve.curve_bit + 7) / 8;
		PubKeySize = Temp * 2;

	} else {
		ret = SBOOT_ERR_AUTH_NOT_SUPPORT;
		goto exit;
	}

	ret = rtl_crypto_sha2(SHA2_256, Pk, PubKeySize, CalcHash);
	if (ret != 0) {
		ret = SBOOT_ERR_PK_HASH_ENGINE_ERROR;
		goto exit;
	}

	/* Compare */
	j = _rand() % 32;
	for (i = 0; i < 32; i ++, j = (j + 1) % 32) {
		error |= CalcHash[j] ^ Hash[j];
	}

	if (error || i != 32) {
		ret = SBOOT_ERR_PK_HASH_INVALID;
	}

exit:
	return ret;
}

HAL_ROM_TEXT_SECTION
int SBOOT_Validate_ImgHash(u8 HashAlg, u8 *ImgHash, SubImgInfo_TypeDef *SubImgInfo, u8 Num)
{
	hw_sha_context Context;
	SHA2_TYPE MdType;
	u8 CalcHash[HASH_MAX_LEN];
	u8 HashLen;
	u8 IsHmac = 0;
	u8 i, j, error = 0;
	int ret = 0;

	_memset(&Context, 0, sizeof(hw_sha_context));

	ret = SBOOT_GetMdType(HashAlg, &MdType, &IsHmac);
	if (ret != 0) {
		goto exit;
	}

	if (IsHmac) {
		/* SHA engine auto-load HMAC key */
		CRYPTO_OTPKey_SHA_Init(1, ENABLE);

		CRYPTO_CHK(rtl_crypto_hmac_sha2_init(MdType, NULL, 32, &Context), SBOOT_ERR_IMG_HASH_ENGINE);
		for (i = 0; i < Num; i++) {
			CRYPTO_CHK(rtl_crypto_hmac_sha2_update((u8 *)SubImgInfo[i].Addr, SubImgInfo[i].Len, &Context), SBOOT_ERR_IMG_HASH_ENGINE);
		}
		CRYPTO_CHK(rtl_crypto_hmac_sha2_final(CalcHash, &Context), SBOOT_ERR_IMG_HASH_ENGINE);

		CRYPTO_OTPKey_SHA_Init(1, DISABLE);
	} else {
		CRYPTO_CHK(rtl_crypto_sha2_init(MdType, &Context), SBOOT_ERR_IMG_HASH_ENGINE);
		for (i = 0; i < Num; i++) {
			CRYPTO_CHK(rtl_crypto_sha2_update((u8 *)SubImgInfo[i].Addr, SubImgInfo[i].Len, &Context), SBOOT_ERR_IMG_HASH_ENGINE);
		}
		CRYPTO_CHK(rtl_crypto_sha2_final(CalcHash, &Context), SBOOT_ERR_IMG_HASH_ENGINE);
	}

	HashLen = (u8)MdType;

	/* Compare */
	j = _rand() % HashLen;
	for (i = 0; i < HashLen; i ++, j = (j + 1) % HashLen) {
		error |= CalcHash[j] ^ ImgHash[j];
	}

	if (error || i != HashLen) {
		ret = SBOOT_ERR_IMG_HASH_INVALID;
	}

exit:
	return ret;
}

HAL_ROM_TEXT_SECTION
int SBOOT_SignatureVerify(u8 *PubKeyHash, Manifest_TypeDef *Manifest, SubImgInfo_TypeDef *SubImgInfo, u8 SubImgCnt)
{
	int ret = 0;
	u8 AuthAlg, HashAlg;

	/* Enable func & clock */
	RCC_PeriphClockCmd(APBPeriph_IPSEC, APBPeriph_IPSEC_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_LX1, APBPeriph_LX1_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_ED25519, APBPeriph_ED25519_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_ECDSA, APBPeriph_ECDSA_CLOCK, ENABLE);

	/* Initialize hash engine */
	CRYPTO_SHA_Init(NULL);

	/* Check algorithm from manifest against OTP configuration if need. */
	ret = SBOOT_Validate_Algorithm(&AuthAlg, &HashAlg, Manifest->AuthAlg, Manifest->HashAlg);
	if (ret != 0) {
		goto exit;
	}

	/* validate pubkey hash */
	TRNG_Random_Delay();
	ret = SBOOT_Validate_PubKey(AuthAlg, Manifest->SBPubKey, PubKeyHash);
	if (ret != 0) {
		goto exit;
	}

	/* validate signature */
	TRNG_Random_Delay();
	ret = SBOOT_Validate_Signature(AuthAlg, HashAlg, Manifest->SBPubKey, (u8 *)Manifest, sizeof(Manifest_TypeDef) - SIGN_MAX_LEN,
								   Manifest->Signature);
	if (ret != 0) {
		goto exit;
	}

	/* calculate and validate image hash */
	TRNG_Random_Delay();
	ret = SBOOT_Validate_ImgHash(HashAlg, Manifest->ImgHash, SubImgInfo, SubImgCnt);
	if (ret != 0) {
		goto exit;
	}

exit:
	return ret;
}

