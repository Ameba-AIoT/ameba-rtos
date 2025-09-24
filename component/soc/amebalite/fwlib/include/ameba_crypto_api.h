/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __AMEBA_CRYPTO_API_H__
#define __AMEBA_CRYPTO_API_H__
#include "ameba_ipsec.h"

/** @addtogroup HW_CRYPTO_ENGINE
  * @verbatim
  *****************************************************************************************
  * Method 2 (use upper level API, for user use)
  *****************************************************************************************
  *	-call rtl_cryptoEngine_init to open IPSEC function & clock
  *
  *	-call following API for set key:
  *		-rtl_crypto_xxx_init
  *
  *	-call following API for authentication/encrypt/decrypt:
  *		authentication		-rtl_crypto_xxx_process or
  *		sequential hash		-rtl_crypto_xxx_update and
  *						-rtl_crypto_xxx_final
  *
  *		encrypt			-rtl_crypto_xxx_encrypt
  *		decrypt			-rtl_crypto_xxx_decrypt
  *
  *****************************************************************************************
  * @endverbatim
  */

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup HW_CRYPTO_ENGINE
  * @brief HW_CRYPTO_ENGINE driver modules
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup HW_CRYPTO_ENGINE_Exported_Constants HW_CRYPTO_ENGINE Exported Constants
  * @{
  */

/** @defgroup CRYPTO_Process_Status
  * @{
  */
#define _ERRNO_CRYPTO_DESC_NUM_SET_OutRange 	-2
#define _ERRNO_CRYPTO_BURST_NUM_SET_OutRange	-3
#define _ERRNO_CRYPTO_NULL_POINTER				-4
#define _ERRNO_CRYPTO_ENGINE_NOT_INIT			-5
#define _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned	-6
#define _ERRNO_CRYPTO_KEY_OutRange				-7
#define _ERRNO_CRYPTO_MSG_OutRange				-8
#define _ERRNO_CRYPTO_IV_OutRange				-9
#define _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH		-10
#define _ERRNO_CRYPTO_CIPHER_TYPE_NOT_MATCH 	-11
#define _ERRNO_CRYPTO_KEY_IV_LEN_DIFF			-12
#define _ERRNO_CRYPTO_HASH_FINAL_NO_UPDATE		-13
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup HW_CRYPTO_ENGINE_Exported_Functions HW_CRYPTO_ENGINE Exported Functions
  * @{
  */

/** @defgroup HW_CRYPTO_ENGINE_Authentication_Functions
  * @{
  */
_LONG_CALL_ int rtl_crypto_md5(const u8 *message, const u32 msglen, u8 *pDigest);
_LONG_CALL_ int rtl_crypto_md5_init(hw_sha_context *ctx);
_LONG_CALL_ int rtl_crypto_md5_process(const u8 *message, const u32 msglen, u8 *pDigest);
_LONG_CALL_ int rtl_crypto_md5_update(u8 *message, u32 msglen, hw_sha_context *ctx);
_LONG_CALL_ int rtl_crypto_md5_update_rom(const u8 *message, const u32 msglen, hw_sha_context *ctx);
_LONG_CALL_ int rtl_crypto_md5_final(u8 *pDigest, hw_sha_context *ctx);

_LONG_CALL_ int rtl_crypto_sha1(const u8 *message, const u32 msglen, u8 *pDigest);
_LONG_CALL_ int rtl_crypto_sha1_init(hw_sha_context *ctx);
_LONG_CALL_ int rtl_crypto_sha1_process(const u8 *message, const u32 msglen, u8 *pDigest);
_LONG_CALL_ int rtl_crypto_sha1_update(u8 *message, u32 msglen, hw_sha_context *ctx);
_LONG_CALL_ int rtl_crypto_sha1_update_rom(const u8 *message, const u32 msglen, hw_sha_context *ctx);
_LONG_CALL_ int rtl_crypto_sha1_final(u8 *pDigest, hw_sha_context *ctx);

_LONG_CALL_ int rtl_crypto_sha2(const SHA2_TYPE sha2type, const u8 *message, const u32 msglen, u8 *pDigest);
_LONG_CALL_ int rtl_crypto_sha2_init(const SHA2_TYPE sha2type, hw_sha_context *ctx);
_LONG_CALL_ int rtl_crypto_sha2_process(const u8 *message, const u32 msglen, u8 *pDigest);
_LONG_CALL_ int rtl_crypto_sha2_update(u8 *message, u32 msglen, hw_sha_context *ctx);
_LONG_CALL_ int rtl_crypto_sha2_update_rom(const u8 *message, const u32 msglen, hw_sha_context *ctx);
_LONG_CALL_ int rtl_crypto_sha2_final(u8 *pDigest, hw_sha_context *ctx);

_LONG_CALL_ int rtl_crypto_hmac_md5(const u8 *message, const u32 msglen, const u8 *key, const u32 keylen, u8 *pDigest);
_LONG_CALL_ int rtl_crypto_hmac_md5_init(const u8 *key, const u32 keylen, hw_sha_context *ctx);
_LONG_CALL_ int rtl_crypto_hmac_md5_process(const u8 *message, const u32 msglen, u8 *pDigest);
_LONG_CALL_ int rtl_crypto_hmac_md5_update(u8 *message, u32 msglen, hw_sha_context *ctx);
_LONG_CALL_ int rtl_crypto_hmac_md5_update_rom(const u8 *message, const u32 msglen, hw_sha_context *ctx);
_LONG_CALL_ int rtl_crypto_hmac_md5_final(u8 *pDigest, hw_sha_context *ctx);

_LONG_CALL_ int rtl_crypto_hmac_sha1(const u8 *message, const u32 msglen, const u8 *key, const u32 keylen, u8 *pDigest);
_LONG_CALL_ int rtl_crypto_hmac_sha1_start(const u8 *key, const u32 keylen);
_LONG_CALL_ int rtl_crypto_hmac_sha1_init(const u8 *key, const u32 keylen, hw_sha_context *ctx);
_LONG_CALL_ int rtl_crypto_hmac_sha1_process(const u8 *message, const u32 msglen, u8 *pDigest);
_LONG_CALL_ int rtl_crypto_hmac_sha1_update(u8 *message, u32 msglen, hw_sha_context *ctx);
_LONG_CALL_ int rtl_crypto_hmac_sha1_update_rom(const u8 *message, const u32 msglen, hw_sha_context *ctx);
_LONG_CALL_ int rtl_crypto_hmac_sha1_final(u8 *pDigest, hw_sha_context *ctx);

_LONG_CALL_ int rtl_crypto_hmac_sha2(const SHA2_TYPE sha2type, const u8 *message, const u32 msglen, const u8 *key, const u32 keylen,
									 u8 *pDigest);
_LONG_CALL_ int rtl_crypto_hmac_sha2_start(const u8 *key, const u32 keylen);
_LONG_CALL_ int rtl_crypto_hmac_sha2_init(const SHA2_TYPE sha2type, const u8 *key, const u32 keylen, hw_sha_context *ctx);
_LONG_CALL_ int rtl_crypto_hmac_sha2_process(const u8 *message, const u32 msglen, u8 *pDigest);
_LONG_CALL_ int rtl_crypto_hmac_sha2_update(u8 *message, u32 msglen, hw_sha_context *ctx);
_LONG_CALL_ int rtl_crypto_hmac_sha2_update_rom(const u8 *message, const u32 msglen, hw_sha_context *ctx);
_LONG_CALL_ int rtl_crypto_hmac_sha2_final(u8 *pDigest, hw_sha_context *ctx);
/**
  * @}
  */

/** @defgroup HW_CRYPTO_ENGINE_Cipher_Functions
  * @{
  */
_LONG_CALL_ int rtl_crypto_aes_cbc_init(const u8 *key, const u32 keylen);
_LONG_CALL_ int rtl_crypto_aes_cbc_encrypt_rom(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);
_LONG_CALL_ int rtl_crypto_aes_cbc_decrypt_rom(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);

_LONG_CALL_ int rtl_crypto_aes_ecb_init(const u8 *key, const u32 keylen);
_LONG_CALL_ int rtl_crypto_aes_ecb_encrypt_rom(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);
_LONG_CALL_ int rtl_crypto_aes_ecb_decrypt_rom(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);

_LONG_CALL_ int rtl_crypto_aes_ctr_init(const u8 *key, const u32 keylen);
_LONG_CALL_ int rtl_crypto_aes_ctr_encrypt_rom(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);
_LONG_CALL_ int rtl_crypto_aes_ctr_decrypt_rom(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);

_LONG_CALL_ int rtl_crypto_aes_cfb_init(const u8 *key, const u32 keylen);
_LONG_CALL_ int rtl_crypto_aes_cfb_encrypt_rom(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);
_LONG_CALL_ int rtl_crypto_aes_cfb_decrypt_rom(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);

_LONG_CALL_ int rtl_crypto_aes_ofb_init(const u8 *key, const u32 keylen);
_LONG_CALL_ int rtl_crypto_aes_ofb_encrypt_rom(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);
_LONG_CALL_ int rtl_crypto_aes_ofb_decrypt_rom(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);

_LONG_CALL_ int rtl_crypto_aes_gcm_init(const u8 *key, const u32 keylen);
_LONG_CALL_ int rtl_crypto_aes_gcm_encrypt_rom(const u8 *message, const u32 msglen, const u8 *iv, const u8 *aad, const u32 aadlen,
		u8 *pResult,
		u8 *pTag);
_LONG_CALL_ int rtl_crypto_aes_gcm_decrypt_rom(const u8 *message, const u32 msglen, const u8 *iv, const u8 *aad, const u32 aadlen,
		u8 *pResult,
		u8 *pTag);

_LONG_CALL_ int rtl_crypto_3des_cbc_init(const u8 *key, const u32 keylen);
_LONG_CALL_ int rtl_crypto_3des_cbc_encrypt(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);
_LONG_CALL_ int rtl_crypto_3des_cbc_decrypt(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);

_LONG_CALL_ int rtl_crypto_3des_ecb_init(const u8 *key, const u32 keylen);
_LONG_CALL_ int rtl_crypto_3des_ecb_encrypt(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);
_LONG_CALL_ int rtl_crypto_3des_ecb_decrypt(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);

_LONG_CALL_ int rtl_crypto_3des_ctr_init(const u8 *key, const u32 keylen);
_LONG_CALL_ int rtl_crypto_3des_ctr_encrypt(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);
_LONG_CALL_ int rtl_crypto_3des_ctr_decrypt(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);

_LONG_CALL_ int rtl_crypto_3des_cfb_init(const u8 *key, const u32 keylen);
_LONG_CALL_ int rtl_crypto_3des_cfb_encrypt(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);
_LONG_CALL_ int rtl_crypto_3des_cfb_decrypt(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);

_LONG_CALL_ int rtl_crypto_3des_ofb_init(const u8 *key, const u32 keylen);
_LONG_CALL_ int rtl_crypto_3des_ofb_encrypt(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);
_LONG_CALL_ int rtl_crypto_3des_ofb_decrypt(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);

_LONG_CALL_ int rtl_crypto_des_cbc_init(const u8 *key, const u32 keylen);
_LONG_CALL_ int rtl_crypto_des_cbc_encrypt(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);
_LONG_CALL_ int rtl_crypto_des_cbc_decrypt(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);

_LONG_CALL_ int rtl_crypto_des_ecb_init(const u8 *key, const u32 keylen);
_LONG_CALL_ int rtl_crypto_des_ecb_encrypt(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);
_LONG_CALL_ int rtl_crypto_des_ecb_decrypt(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);

_LONG_CALL_ int rtl_crypto_des_ctr_init(const u8 *key, const u32 keylen);
_LONG_CALL_ int rtl_crypto_des_ctr_encrypt(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);
_LONG_CALL_ int rtl_crypto_des_ctr_decrypt(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);

_LONG_CALL_ int rtl_crypto_des_cfb_init(const u8 *key, const u32 keylen);
_LONG_CALL_ int rtl_crypto_des_cfb_encrypt(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);
_LONG_CALL_ int rtl_crypto_des_cfb_decrypt(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);

_LONG_CALL_ int rtl_crypto_des_ofb_init(const u8 *key, const u32 keylen);
_LONG_CALL_ int rtl_crypto_des_ofb_encrypt(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);
_LONG_CALL_ int rtl_crypto_des_ofb_decrypt(const u8 *message, const u32 msglen, const u8 *iv, const u32 ivlen, u8 *pResult);

_LONG_CALL_ int rtl_crypto_poly1305_init(const u8 *key);
_LONG_CALL_ int rtl_crypto_poly1305_process(const u8 *message, const u32 msglen, u8 *pDigest);
_LONG_CALL_ int rtl_crypto_poly1305(const u8 *message, const u32 msglen, const u8 *key, u8 *pDigest);

_LONG_CALL_ int rtl_crypto_chacha_init(const u8 *key);
_LONG_CALL_ int rtl_crypto_chacha_encrypt(const u8 *message, const u32 msglen, const u8 *iv, const u32 count, u8 *pResult);
_LONG_CALL_ int rtl_crypto_chacha_decrypt(const u8 *message, const u32 msglen, const u8 *iv, const u32 count, u8 *pResult);

_LONG_CALL_ int rtl_crypto_chacha_poly1305_init(const u8 *key);
_LONG_CALL_ int rtl_crypto_chacha_poly1305_encrypt(const u8 *message, 	const u32 msglen, const u8 *nonce, const u8 *aad, const u32 aadlen,
		u8 *pResult, u8 *pTag);
_LONG_CALL_ int rtl_crypto_chacha_poly1305_decrypt(const u8 *message, 	const u32 msglen, const u8 *nonce, const u8 *aad, const u32 aadlen,
		u8 *pResult, u8 *pTag);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif /* __AMEBA_CRYPTO_API_H__ */