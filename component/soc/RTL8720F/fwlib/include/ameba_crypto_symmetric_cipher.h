/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_CRYPTO_SYMMETRIC_CIPHER_H_
#define _AMEBA_CRYPTO_SYMMETRIC_CIPHER_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup Crypto_Symmetric_Cipher
  * @brief Crypto Symmetric Cipher driver modules
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup Crypto_Engine_Mode
  * @{
  */
typedef enum {
	CRYPTO_AES_MODE = 0,
	CRYPTO_SM4_MODE = 1
} crypto_engine_mode;
/** @} */

#define CRYPTO_CTX_STARTED  0x96965a5a

/* Exported functions --------------------------------------------------------*/
/** @defgroup Crypto_Symmetric_Cipher_Exported_Functions Crypto Symmetric Cipher Exported Functions
  * @{
  */

/** @defgroup Crypto_Symmetric_Cipher_Slave_Functions Crypto Symmetric Cipher Slave Functions
  * @{
  */

/**
 * @brief Symmetric cipher start operation. Slave mode
 * @param  mode: Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param  key_id: KM_KEY_SEL_NUM_x
 * @param  key_len_bits:
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param  key_addr: Software key address, set to NULL if use hardware key
 * @param  aes_mode: Cipher mode
 * @param  is_encryption:
 * 		 @arg 1: encryption
 * 		 @arg 0: decryption
 * @param  input: The address of the input data (16 bytes).
 * @param  iv: iv array address. Not used in ECB mode.
 * @param  output: The address of the output data (16 bytes).
 * @note 1. The engine will be locked during the execution of this function,
 *          and will be unlocked after execution.
 *          Avoid multiple threads/CPUs using the cipher engine at the same time.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_sym_slave_start(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, u8 *key_addr, int aes_mode, u8 is_encryption, const u8 input[16],
									   u8 iv[16],
									   u8 output[16]);

/**
 * @brief Symmetric cipher ECB mode. Slave mode
 * @param  mode: Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param  key_id: KM_KEY_SEL_NUM_x
 * @param  key_len_bits:
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param  key_addr: Software key address, set to NULL if use hardware key
 * @param  is_encryption:
 * 		 @arg 1: encryption
 * 		 @arg 0: decryption
 * @param  input: The address of the input data (16 bytes).
 * @param  output: The address of the output data (16 bytes).
 * @note 1. The engine will be locked during the execution of this function,
 *          and will be unlocked after execution.
 *          Avoid multiple threads/CPUs using the cipher engine at the same time.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_sym_ecb_slave(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, u8 *key_addr, u8 is_encryption, const u8 input[16], u8 output[16]);

/**
 * @brief Symmetric cipher CTR mode. Slave mode
 * @param  mode: Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param  key_id: KM_KEY_SEL_NUM_x
 * @param  key_len_bits:
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param  key_addr: Software key address, set to NULL if use hardware key
 * @param  is_encryption:
 * 		 @arg 1: encryption
 * 		 @arg 0: decryption
 * @param  input: The address of the input data (16 bytes).
 * @param  iv: iv array address. IV length must be 16Byte.
 * @param  output: The address of the output data (16 bytes).
 * @note 1. The engine will be locked during the execution of this function,
 *          and will be unlocked after execution.
 *          Avoid multiple threads/CPUs using the cipher engine at the same time.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_sym_ctr_slave(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, u8 *key_addr, u8 is_encryption, const u8 input[16], u8 iv[16],
									 u8 output[16]);

/** @} */

/** @defgroup Crypto_Symmetric_Cipher_DMA_Functions Crypto Symmetric Cipher DMA Functions
  * @{
  */

/**
 * @brief Symmetric cipher start operation. DMA mode
 * @param  mode: Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param  key_id: KM_KEY_SEL_NUM_x
 * @param  key_len_bits:
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param  aes_mode: Cipher mode
 * @param  is_encryption:
 * 		 @arg 1: encryption
 * 		 @arg 0: decryption
 * @param  input: The address of the input data.
 * @param  len: The length of the data.
 * @param  iv: iv array address. IV length must be 16Byte.
 * @param  output: The address of the output data.
 * @note 1. The engine will be locked during the execution of this function,
 *          and will be unlocked after execution.
 *          Avoid multiple threads/CPUs using the cipher engine at the same time.
 * @note 2. The length of encrypted and decrypted data must be 32-byte (cacheline size) aligned.
 *          At the same time, the destination address is also 32-byte aligned. Otherwise, an error will be reported.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_sym_start(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, int aes_mode, u8 is_encryption, const u8 *input, u32 len, u8 iv[16],
								 u8 *output);

/**
 * @brief Symmetric cipher ECB mode. DMA mode
 * @param  mode: Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param  key_id: KM_KEY_SEL_NUM_x
 * @param  key_len_bits:
 *		 @arg KEY_BIT_128
 * @arg KEY_BIT_192
 * @arg KEY_BIT_256
 * @param is_encryption:
 * 		 @arg 1: encryption
 * 		 @arg 0: decryption
 * @param  input: The address of the input data.
 * @param  len: The length of the data.
 * @param  output: The address of the output data.
 * @note 1. The engine will be locked during the execution of this function,
 *          and will be unlocked after execution.
 *          Avoid multiple threads/CPUs using the cipher engine at the same time.
 * @note 2. The length of encrypted and decrypted data must be 32-byte (cacheline size) aligned.
 *          At the same time, the destination address is also 32-byte aligned. Otherwise, an error will be reported.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_sym_ecb(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, u8 is_encryption, const u8 *input, u32 len, u8 *output);

/**
 * @brief Symmetric cipher CBC mode. DMA mode
 * @param  mode: Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param  key_id: KM_KEY_SEL_NUM_x
 * @param  key_len_bits:
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param is_encryption:
 * 		 @arg 1: encryption
 * 		 @arg 0: decryption
 * @param  input: The address of the input data.
 * @param  len: The length of the data.
 * @param  iv: iv array address. IV length must be 16Byte.
 * @param  output: The address of the output data.
 * @note 1. The engine will be locked during the execution of this function,
 *          and will be unlocked after execution.
 *          Avoid multiple threads/CPUs using the cipher engine at the same time.
 * @note 2. The length of encrypted and decrypted data must be 32-byte (cacheline size) aligned.
 *          At the same time, the destination address is also 32-byte aligned. Otherwise, an error will be reported.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_sym_cbc(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, u8 is_encryption, const u8 *input, u32 len, u8 iv[16], u8 *output);

/**
 * @brief Symmetric cipher CFB mode. DMA mode
 * @param  mode: Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param  key_id: KM_KEY_SEL_NUM_x
 * @param  key_len_bits:
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 * @arg KEY_BIT_256
 * @param is_encryption:
 * 		 @arg 1: encryption
 * 		 @arg 0: decryption
 * @param  input: The address of the input data.
 * @param  len: The length of the data.
 * @param  iv: iv array address. IV length must be 16Byte.
 * @param  output: The address of the output data.
 * @note 1. The engine will be locked during the execution of this function,
 *          and will be unlocked after execution.
 *          Avoid multiple threads/CPUs using the cipher engine at the same time.
 * @note 2. The length of encrypted and decrypted data must be 32-byte (cacheline size) aligned.
 *          At the same time, the destination address is also 32-byte aligned. Otherwise, an error will be reported.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_sym_cfb(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, u8 is_encryption, const u8 *input, u32 len, u8 iv[16], u8 *output);

/**
 * @brief Symmetric cipher OFB mode. DMA mode
 * @param  mode: Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param  key_id: KM_KEY_SEL_NUM_x
 * @param  key_len_bits:
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param is_encryption:
 * 		 @arg 1: encryption
 * 		 @arg 0: decryption
 * @param  input: The address of the input data.
 * @param  len: The length of the data.
 * @param  iv: iv array address. IV length must be 16Byte.
 * @param  output: The address of the output data.
 * @note 1. The engine will be locked during the execution of this function,
 *          and will be unlocked after execution.
 *          Avoid multiple threads/CPUs using the cipher engine at the same time.
 * @note 2. The length of encrypted and decrypted data must be 32-byte (cacheline size) aligned.
 *          At the same time, the destination address is also 32-byte aligned. Otherwise, an error will be reported.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_sym_ofb(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, u8 is_encryption, const u8 *input, u32 len, u8 iv[16], u8 *output);

/**
 * @brief Symmetric cipher CTR mode. DMA mode
 * @param  mode: Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param  key_id: KM_KEY_SEL_NUM_x
 * @param  key_len_bits:
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param is_encryption:
 * 		 @arg 1: encryption
 * 		 @arg 0: decryption
 * @param  input: The address of the input data.
 * @param  len: The length of the data.
 * @param  iv: iv array address. IV length must be 16Byte.
 * @param  output: The address of the output data.
 * @note 1. The engine will be locked during the execution of this function,
 *          and will be unlocked after execution.
 *          Avoid multiple threads/CPUs using the cipher engine at the same time.
 * @note 2. The length of encrypted and decrypted data must be 32-byte (cacheline size) aligned.
 *          At the same time, the destination address is also 32-byte aligned. Otherwise, an error will be reported.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_sym_ctr(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, u8 is_encryption, const u8 *input, u32 len, u8 iv[16], u8 *output);

/** @} */

/** @defgroup Crypto_Symmetric_Cipher_Auth_Functions Crypto Symmetric Cipher Authentication Functions
  * @{
  */

/**
 * @brief Symmetric cipher CMAC. DMA mode
 * @param  mode: Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param  key_id: KM_KEY_SEL_NUM_x
 * @param  key_len_bits:
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param  input: The address of the input data.
 * @param  len: The length of the data.
 * @param  tag: The address of the CMAC output data.
 * @note 1. The engine will be locked during the execution of this function,
 *          and will be unlocked after execution.
 *          Avoid multiple threads/CPUs using the cipher engine at the same time.
 * @note 2. The length of input data must be 32-byte (cacheline size) aligned.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_sym_cmac(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, const u8 *input, u32 len, u8 *tag);

/** @} */

/** @defgroup Crypto_Symmetric_Cipher_AEAD_Functions Crypto Symmetric Cipher AEAD Functions
  * @{
  */

/**
 * @brief Symmetric cipher GCM mode encryption and tag generation. DMA mode
 * @param  mode: Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param  key_id: KM_KEY_SEL_NUM_x
 * @param  key_len_bits:
 *		 @arg KEY_BIT_128
 * @arg KEY_BIT_192
 * @arg KEY_BIT_256
 * @param  is_encryption: encryption mode flag
 * @param  len: The length of the plaintext.
 * @param  iv_addr: iv array address.
 * @param  iv_len: iv length.
 * @param  aad_addr: The address of the AAD data.
 * @param  aad_len: The length of the AAD data.
 * @param  input: The address of the plaintext data.
 * @param  output: The address of the ciphertext data.
 * @param  tag_len: The length of the tag.
 * @param  tag: The address of the tag output data.
 * @note 1. The engine will be locked during the execution of this function,
 *          and will be unlocked after execution.
 *          Avoid multiple threads/CPUs using the cipher engine at the same time.
 * @note 2. The length of input data, output data and aad data must be 32-byte (cacheline size) aligned.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_sym_gcm_encrypt_and_tag(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, u8 is_encryption, u32 len, u8 *iv_addr, u32 iv_len,
		u8 *aad_addr,
		u32 aad_len, const u8 *input,
		u8 *output, u32 tag_len, u8 *tag);

/**
 * @brief Symmetric cipher GCM mode authenticated decryption. DMA mode
 * @param  mode: Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param  key_id: KM_KEY_SEL_NUM_x
 * @param  key_len_bits:
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 * @arg KEY_BIT_256
 * @param  len: The length of the ciphertext.
 * @param  iv_addr: iv array address.
 * @param  iv_len: iv length.
 * @param  aad_addr: The address of the AAD data.
 * @param  aad_len: The length of the AAD data.
 * @param  input: The address of the ciphertext data.
 * @param  output: The address of the plaintext data.
 * @param  tag_len: The length of the tag.
 * @param  tag: The address of the tag input data.
 * @note 1. The engine will be locked during the execution of this function,
 *          and will be unlocked after execution.
 *          Avoid multiple threads/CPUs using the cipher engine at the same time.
 * @note 2. The length of input data, output data and aad data must be 32-byte (cacheline size) aligned.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_sym_gcm_authenticated_decrypt(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, u32 len, u8 *iv_addr, u32 iv_len, u8 *aad_addr,
		u32 aad_len,
		const u8 *input, u8 *output, u32 tag_len, u8 *tag);

/**
 * @brief Symmetric cipher CCM mode encryption and tag generation. DMA mode
 * @param  mode: Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param  key_id: KM_KEY_SEL_NUM_x
 * @param  key_len_bits:
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 * @arg KEY_BIT_256
 * @param  key_addr: Software key address, set to NULL if use hardware key
 * @param  is_encryption: encryption mode flag
 * @param  msg_len: The length of the plaintext.
 * @param  nonce_addr: nonce array address.
 * @param  nonce_len: nonce length.
 * @param  aad_addr: The address of the AAD data.
 * @param  aad_len: The length of the AAD data.
 * @param  input: The address of the plaintext data.
 * @param  output: The address of the ciphertext data.
 * @param  tag_len: The length of the tag.
 * @param  tag: The address of the tag output data.
 * @note 1. The engine will be locked during the execution of this function,
 *          and will be unlocked after execution.
 *          Avoid multiple threads/CPUs using the cipher engine at the same time.
 * @note 2. The length of input data, output data and aad data must be 32-byte (cacheline size) aligned.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_sym_ccm_encrypt_and_tag(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, u8 *key_addr, u8 is_encryption, u32 msg_len,
		u8 *nonce_addr, u32 nonce_len, u8 *aad_addr, u32 aad_len,
		const u8 *input, u8 *output, u32 tag_len, u8 *tag);

/**
 * @brief Symmetric cipher CCM mode authenticated decryption. DMA mode
 * @param  mode: Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param  key_id: KM_KEY_SEL_NUM_x
 * @param  key_len_bits:
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 * @arg KEY_BIT_256
 * @param  key_addr: Software key address, set to NULL if use hardware key
 * @param  msg_len: The length of the ciphertext.
 * @param  nonce_addr: nonce array address.
 * @param  nonce_len: nonce length.
 * @param  aad_addr: The address of the AAD data.
 * @param  aad_len: The length of the AAD data.
 * @param  input: The address of the ciphertext data.
 * @param  output: The address of the plaintext data.
 * @param  tag_len: The length of the tag.
 * @param  tag: The address of the tag input data.
 * @note 1. The engine will be locked during the execution of this function,
 *          and will be unlocked after execution.
 *          Avoid multiple threads/CPUs using the cipher engine at the same time.
 * @note 2. The length of input data, output data and aad data must be 32-byte (cacheline size) aligned.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_sym_ccm_auth_decrypt(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, u8 *key_addr, u32 msg_len,
		u8 *nonce_addr, u32 nonce_len, u8 *aad_addr, u32 aad_len,
		const u8 *input, u8 *output, u32 tag_len, u8 *tag);

#ifdef __cplusplus
}
#endif

/** @} */
/** @} */
/** @} */
/** @} */

#endif /* _AMEBA_CRYPTO_SYMMETRIC_CIPHER_H_ */