/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SM4_H_
#define _AMEBA_SM4_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup SM4
  * @brief SM4 driver modules
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup SM4_Exported_Constants SM4 Exported Constants
  * @{
  */

/** @defgroup SM4_Cipher_Mode
  * @{
  */
#define SM4_ENCRYPTION_MODE  ((u8)0x01)
#define SM4_DECRYPTION_MODE  ((u8)0x00)

#define SM4_MODE_ECB			((u8)0x00)
#define SM4_MODE_CBC			((u8)0x01)
#define SM4_MODE_CFB			((u8)0x02)
#define SM4_MODE_OFB			((u8)0x03)
#define SM4_MODE_CTR			((u8)0x04)
/** @} */

/** @} */

/* Exported functions --------------------------------------------------------*/
/** @defgroup SM4_Exported_Functions SM4 Exported Functions
  * @{
  */

/** @defgroup SM4_Slave_Functions SM4 Slave Functions
  * @{
  */

/**
 * @brief SM4-ECB mode encryption and decryption. Slave mode
 * @param  key_id: KM_KEY_SEL_NUM_x
 * @param  key_addr: Software key address, set to NULL if use hardware key
 * @param is_encryption:
 * 		 @arg 1: encryption
 * 		 @arg 0: decryption
 * @param  input: The address of the input data (16 bytes).
 * @param  output: The address of the output data (16 bytes).
 * @note 1. The engine will be locked during the execution of this function,
 *          and will be unlocked after execution.
 *          Avoid multiple threads/CPUs using the SM4 engine at the same time.
 * @note 2. SM4 only supports 128-bit key length.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_sm4_ecb_slave(u8 key_id, u8 *key_addr, u8 is_encryption, const u8 input[16], u8 output[16]);

/**
 * @brief SM4-CTR mode encryption and decryption. Slave mode
 * @param  key_id: KM_KEY_SEL_NUM_x
 * @param  key_addr: Software key address, set to NULL if use hardware key
 * @param is_encryption:
 * 		 @arg 1: encryption
 * 		 @arg 0: decryption
 * @param  input: The address of the input data (16 bytes).
 * @param  iv: iv array address. IV length must be 16Byte.
 * @param  output: The address of the output data (16 bytes).
 * @note 1. The engine will be locked during the execution of this function,
 *          and will be unlocked after execution.
 *          Avoid multiple threads/CPUs using the SM4 engine at the same time.
 * @note 2. SM4 only supports 128-bit key length.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_sm4_ctr_slave(u8 key_id, u8 *key_addr, u8 is_encryption, const u8 input[16], u8 iv[16], u8 output[16]);

/** @} */

/** @defgroup SM4_DMA_Functions SM4 DMA Functions
  * @{
  */

/**
 * @brief SM4-ECB mode encryption and decryption. DMA mode
 * @param  key_id: KM_KEY_SEL_NUM_x
 * @param is_encryption:
 * 		 @arg 1: encryption
 * 		 @arg 0: decryption
 * @param  input: The address of the input data.
 * @param  len: The length of the data.
 * @param  output: The address of the output data.
 * @note 1. The engine will be locked during the execution of this function,
 *          and will be unlocked after execution.
 *          Avoid multiple threads/CPUs using the SM4 engine at the same time.
 * @note 2. The length of encrypted and decrypted data must be 32-byte (cacheline size) aligned.
 *          At the same time, the destination address is also 32-byte aligned. Otherwise, an error will be reported.
 * @note 3. SM4 only supports 128-bit key length.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_sm4_ecb(u8 key_id, u8 is_encryption, const u8 *input, u32 len, u8 *output);

/**
 * @brief SM4-CBC mode encryption and decryption. DMA mode
 * @param  key_id: KM_KEY_SEL_NUM_x
 * @param is_encryption:
 * 		 @arg 1: encryption
 * 		 @arg 0: decryption
 * @param  input: The address of the input data.
 * @param  len: The length of the data.
 * @param  iv: iv array address. IV length must be 16Byte.
 * @param  output: The address of the output data.
 * @note 1. The engine will be locked during the execution of this function,
 *          and will be unlocked after execution.
 *          Avoid multiple threads/CPUs using the SM4 engine at the same time.
 * @note 2. The length of encrypted and decrypted data must be 32-byte (cacheline size) aligned.
 *          At the same time, the destination address is also 32-byte aligned. Otherwise, an error will be reported.
 * @note 3. SM4 only supports 128-bit key length.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_sm4_cbc(u8 key_id, u8 is_encryption, const u8 *input, u32 len, u8 iv[16], u8 *output);

/**
 * @brief SM4-CFB mode encryption and decryption. DMA mode
 * @param  key_id: KM_KEY_SEL_NUM_x
 * @param is_encryption:
 * 		 @arg 1: encryption
 * 		 @arg 0: decryption
 * @param  input: The address of the input data.
 * @param  len: The length of the data.
 * @param  iv: iv array address. IV length must be 16Byte.
 * @param  output: The address of the output data.
 * @note 1. The engine will be locked during the execution of this function,
 *          and will be unlocked after execution.
 *          Avoid multiple threads/CPUs using the SM4 engine at the same time.
 * @note 2. The length of encrypted and decrypted data must be 32-byte (cacheline size) aligned.
 *          At the same time, the destination address is also 32-byte aligned. Otherwise, an error will be reported.
 * @note 3. SM4 only supports 128-bit key length.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_sm4_cfb(u8 key_id, u8 is_encryption, const u8 *input, u32 len, u8 iv[16], u8 *output);

/**
 * @brief SM4-OFB mode encryption and decryption. DMA mode
 * @param  key_id: KM_KEY_SEL_NUM_x
 * @param is_encryption:
 * 		 @arg 1: encryption
 * 		 @arg 0: decryption
 * @param  input: The address of the input data.
 * @param  len: The length of the data.
 * @param  iv: iv array address. IV length must be 16Byte.
 * @param  output: The address of the output data.
 * @note 1. The engine will be locked during the execution of this function,
 *          and will be unlocked after execution.
 *          Avoid multiple threads/CPUs using the SM4 engine at the same time.
 * @note 2. The length of encrypted and decrypted data must be 32-byte (cacheline size) aligned.
 *          At the same time, the destination address is also 32-byte aligned. Otherwise, an error will be reported.
 * @note 3. SM4 only supports 128-bit key length.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_sm4_ofb(u8 key_id, u8 is_encryption, const u8 *input, u32 len, u8 iv[16], u8 *output);

/**
 * @brief SM4-CTR mode encryption and decryption. DMA mode
 * @param  key_id: KM_KEY_SEL_NUM_x
 * @param is_encryption:
 * 		 @arg 1: encryption
 * 		 @arg 0: decryption
 * @param  input: The address of the input data.
 * @param  len: The length of the data.
 * @param  iv: iv array address. IV length must be 16Byte.
 * @param  output: The address of the output data.
 * @note 1. The engine will be locked during the execution of this function,
 *          and will be unlocked after execution.
 *          Avoid multiple threads/CPUs using the SM4 engine at the same time.
 * @note 2. The length of encrypted and decrypted data must be 32-byte (cacheline size) aligned.
 *          At the same time, the destination address is also 32-byte aligned. Otherwise, an error will be reported.
 * @note 3. SM4 only supports 128-bit key length.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_sm4_ctr(u8 key_id, u8 is_encryption, const u8 *input, u32 len, u8 iv[16], u8 *output);

/** @} */

/** @defgroup SM4_Auth_Functions SM4 Authentication Functions
  * @{
  */

/**
 * @brief SM4-CMAC. DMA mode
 * @param  key_id: KM_KEY_SEL_NUM_x
 * @param  input: The address of the input data.
 * @param  len: The length of the data.
 * @param  tag: The address of the CMAC output data.
 * @note 1. The engine will be locked during the execution of this function,
 *          and will be unlocked after execution.
 *          Avoid multiple threads/CPUs using the SM4 engine at the same time.
 * @note 2. The length of input data must be 32-byte (cacheline size) aligned.
 * @note 3. SM4 only supports 128-bit key length.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_sm4_cmac(u8 key_id, const u8 *input, u32 len, u8 *tag);

/** @} */

/** @defgroup SM4_AEAD_Functions SM4 AEAD Functions
  * @{
  */

/**
 * @brief SM4-GCM mode encryption and tag generation. DMA mode
 * @param  key_id: KM_KEY_SEL_NUM_x
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
 *          Avoid multiple threads/CPUs using the SM4 engine at the same time.
 * @note 2. The length of input data, output data and aad data must be 32-byte (cacheline size) aligned.
 * @note 3. SM4 only supports 128-bit key length.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_sm4_gcm_encrypt_and_tag(u8 key_id, u32 len, u8 *iv_addr, u32 iv_len, u8 *aad_addr, u32 aad_len,
		const u8 *input, u8 *output, u32 tag_len, u8 *tag);

/**
 * @brief SM4-GCM mode authenticated decryption. DMA mode
 * @param  key_id: KM_KEY_SEL_NUM_x
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
 *          Avoid multiple threads/CPUs using the SM4 engine at the same time.
 * @note 2. The length of input data, output data and aad data must be 32-byte (cacheline size) aligned.
 * @note 3. SM4 only supports 128-bit key length.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_sm4_gcm_authenticated_decrypt(u8 key_id, u32 len, u8 *iv_addr, u32 iv_len, u8 *aad_addr, u32 aad_len,
		const u8 *input, u8 *output, u32 tag_len, u8 *tag);

/**
 * @brief SM4-CCM mode encryption and tag generation. DMA mode
 * @param  key_id: KM_KEY_SEL_NUM_x
 * @param  key_addr: Software key address, set to NULL if use hardware key
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
 *          Avoid multiple threads/CPUs using the SM4 engine at the same time.
 * @note 2. The length of input data, output data and aad data must be 32-byte (cacheline size) aligned.
 * @note 3. SM4 only supports 128-bit key length.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_sm4_ccm_encrypt_and_tag(u8 key_id, u8 *key_addr, u32 msg_len, u8 *nonce_addr, u32 nonce_len, u8 *aad_addr, u32 aad_len,
		const u8 *input, u8 *output, u32 tag_len, u8 *tag);

/**
 * @brief SM4-CCM mode authenticated decryption. DMA mode
 * @param  key_id: KM_KEY_SEL_NUM_x
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
 *          Avoid multiple threads/CPUs using the SM4 engine at the same time.
 * @note 2. The length of input data, output data and aad data must be 32-byte (cacheline size) aligned.
 * @note 3. SM4 only supports 128-bit key length.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_sm4_ccm_auth_decrypt(u8 key_id, u8 *key_addr, u32 msg_len, u8 *nonce_addr, u32 nonce_len, u8 *aad_addr, u32 aad_len,
		const u8 *input, u8 *output, u32 tag_len, u8 *tag);

/** @} */

/** @} */
/** @} */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _AMEBA_SM4_H_ */