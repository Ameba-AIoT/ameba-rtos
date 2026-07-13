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

/** @defgroup Crypto_Symmetric_Cipher Crypto Symmetric Cipher
  * @brief Crypto Symmetric Cipher driver modules
  * @{
  */

/* Exported types --------------------------------------------------------*/
/** @defgroup Crypto_Symmetric_Cipher_Exported_Types Crypto Symmetric Cipher Exported Types
  * @{
  */
/** @defgroup Crypto_Engine_Mode Crypto Engine Mode
  * @{
  */
/**
 * @brief Cipher engine selector for unified symmetric cipher APIs.
 */
typedef enum {
	CRYPTO_AES_MODE = 0,	/*!< Use AES cipher engine */
	CRYPTO_SM4_MODE = 1		/*!< Use SM4 cipher engine */
} crypto_engine_mode;
/** @} */

/** @} */

/* Exported constants --------------------------------------------------------*/
/** @defgroup Crypto_Symmetric_Cipher_Exported_Constants Crypto Symmetric Cipher Exported Constants
  * @{
  */
#define CRYPTO_CTX_STARTED  0x96965a5a	/*!< Magic value marking a symmetric cipher context as initialized */
/** @} */

/* Exported functions --------------------------------------------------------*/

/** @defgroup Crypto_Symmetric_Cipher_Slave_Functions Crypto Symmetric Cipher Slave Functions
  * @{
  */

/**
 * @brief Perform symmetric cipher start operation (slave mode).
 * @param  mode Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param  key_id Value of @ref KM_KEY. Used only when key_addr is NULL.
 * @param  key_len_bits
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param  key_addr Software key address, set to NULL if use hardware key
 * @param  aes_mode Cipher mode
 * @param  is_encryption
 * 		 @arg 1: encryption
 * 		 @arg 0: decryption
 * @param  input The address of the input data (16 bytes).
 * @param  iv IV array address. Not used in ECB mode.
 * @param  output The address of the output data (16 bytes).
 * @note The engine will be locked during the execution of this function,
 *          and will be unlocked after execution.
 *          Avoid multiple threads/CPUs using the cipher engine at the same time.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_sym_slave_start(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, u8 *key_addr, int aes_mode, u8 is_encryption, const u8 input[16],
									   u8 iv[16],
									   u8 output[16]);

/**
 * @brief Perform symmetric cipher ECB operation (slave mode).
 * @param  mode Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param  key_id Value of @ref KM_KEY. Used only when key_addr is NULL.
 * @param  key_len_bits
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param  key_addr Software key address, set to NULL if use hardware key
 * @param  is_encryption
 * 		 @arg 1: encryption
 * 		 @arg 0: decryption
 * @param  input The address of the input data (16 bytes).
 * @param  output The address of the output data (16 bytes).
 * @note The engine will be locked during the execution of this function,
 *          and will be unlocked after execution.
 *          Avoid multiple threads/CPUs using the cipher engine at the same time.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_sym_ecb_slave(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, u8 *key_addr, u8 is_encryption, const u8 input[16], u8 output[16]);

/**
 * @brief Perform symmetric cipher CTR operation (slave mode).
 * @param  mode Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param  key_id Value of @ref KM_KEY. Used only when key_addr is NULL.
 * @param  key_len_bits
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param  key_addr Software key address, set to NULL if use hardware key
 * @param  is_encryption
 * 		 @arg 1: encryption
 * 		 @arg 0: decryption
 * @param  input The address of the input data (16 bytes).
 * @param  iv IV array address. IV length must be 16Byte.
 * @param  output The address of the output data (16 bytes).
 * @note The engine will be locked during the execution of this function,
 *          and will be unlocked after execution.
 *          Avoid multiple threads/CPUs using the cipher engine at the same time.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_sym_ctr_slave(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, u8 *key_addr, u8 is_encryption, const u8 input[16], u8 iv[16],
									 u8 output[16]);

/** @} */

/** @defgroup Crypto_Symmetric_Cipher_DMA_Functions Crypto Symmetric Cipher DMA Functions
  * @{
  */

/**
 * @brief Perform symmetric cipher start operation (DMA mode).
 * @param  mode Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param  key_id Value of @ref KM_KEY. Used only when key_addr is NULL.
 * @param  key_len_bits
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param  aes_mode Cipher mode
 * @param  is_encryption
 * 		 @arg 1: encryption
 * 		 @arg 0: decryption
 * @param  input The address of the input data.
 * @param  len The length of the data.
 * @param  iv IV array address. IV length must be 16Byte.
 * @param  output The address of the output data.
 * @note
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the cipher engine at the same time.
 *         - The length of encrypted and decrypted data must be 32-byte (cache line size) aligned.
 *           At the same time, the destination address is also 32-byte aligned. Otherwise, an error will be reported.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_sym_start(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, int aes_mode, u8 is_encryption, const u8 *input, u32 len, u8 iv[16],
								 u8 *output);

/**
 * @brief Perform symmetric cipher ECB operation (DMA mode).
 * @param  mode Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param  key_id Value of @ref KM_KEY. Used only when key_addr is NULL.
 * @param  key_len_bits
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param is_encryption
 * 		 @arg 1: encryption
 * 		 @arg 0: decryption
 * @param  input The address of the input data.
 * @param  len The length of the data.
 * @param  output The address of the output data.
 * @note
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the cipher engine at the same time.
 *         - The length of encrypted and decrypted data must be 32-byte (cache line size) aligned.
 *           At the same time, the destination address is also 32-byte aligned. Otherwise, an error will be reported.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_sym_ecb(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, u8 is_encryption, const u8 *input, u32 len, u8 *output);

/**
 * @brief Perform symmetric cipher CBC operation (DMA mode).
 * @param  mode Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param  key_id Value of @ref KM_KEY. Used only when key_addr is NULL.
 * @param  key_len_bits
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param is_encryption
 * 		 @arg 1: encryption
 * 		 @arg 0: decryption
 * @param  input The address of the input data.
 * @param  len The length of the data.
 * @param  iv IV array address. IV length must be 16Byte.
 * @param  output The address of the output data.
 * @note
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the cipher engine at the same time.
 *         - The length of encrypted and decrypted data must be 32-byte (cache line size) aligned.
 *           At the same time, the destination address is also 32-byte aligned. Otherwise, an error will be reported.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_sym_cbc(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, u8 is_encryption, const u8 *input, u32 len, u8 iv[16], u8 *output);

/**
 * @brief Perform symmetric cipher CFB operation (DMA mode).
 * @param  mode Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param  key_id Value of @ref KM_KEY. Used only when key_addr is NULL.
 * @param  key_len_bits
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param is_encryption
 * 		 @arg 1: encryption
 * 		 @arg 0: decryption
 * @param  input The address of the input data.
 * @param  len The length of the data.
 * @param  iv IV array address. IV length must be 16Byte.
 * @param  output The address of the output data.
 * @note
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the cipher engine at the same time.
 *         - The length of encrypted and decrypted data must be 32-byte (cache line size) aligned.
 *           At the same time, the destination address is also 32-byte aligned. Otherwise, an error will be reported.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_sym_cfb(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, u8 is_encryption, const u8 *input, u32 len, u8 iv[16], u8 *output);

/**
 * @brief Perform symmetric cipher OFB operation (DMA mode).
 * @param  mode Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param  key_id Value of @ref KM_KEY. Used only when key_addr is NULL.
 * @param  key_len_bits
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param is_encryption
 * 		 @arg 1: encryption
 * 		 @arg 0: decryption
 * @param  input The address of the input data.
 * @param  len The length of the data.
 * @param  iv IV array address. IV length must be 16Byte.
 * @param  output The address of the output data.
 * @note
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the cipher engine at the same time.
 *         - The length of encrypted and decrypted data must be 32-byte (cache line size) aligned.
 *           At the same time, the destination address is also 32-byte aligned. Otherwise, an error will be reported.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_sym_ofb(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, u8 is_encryption, const u8 *input, u32 len, u8 iv[16], u8 *output);

/**
 * @brief Perform symmetric cipher CTR operation (DMA mode).
 * @param  mode Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param  key_id Value of @ref KM_KEY. Used only when key_addr is NULL.
 * @param  key_len_bits
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param is_encryption
 * 		 @arg 1: encryption
 * 		 @arg 0: decryption
 * @param  input The address of the input data.
 * @param  len The length of the data.
 * @param  iv IV array address. IV length must be 16Byte.
 * @param  output The address of the output data.
 * @note
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the cipher engine at the same time.
 *         - The length of encrypted and decrypted data must be 32-byte (cache line size) aligned.
 *           At the same time, the destination address is also 32-byte aligned. Otherwise, an error will be reported.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_sym_ctr(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, u8 is_encryption, const u8 *input, u32 len, u8 iv[16], u8 *output);

/** @} */

/** @defgroup Crypto_Symmetric_Cipher_Auth_Functions Crypto Symmetric Cipher Authentication Functions
  * @{
  */

/**
 * @brief Compute symmetric cipher CMAC (DMA mode).
 * @param  mode Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param  key_id Value of @ref KM_KEY. Used only when key_addr is NULL.
 * @param  key_len_bits
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param  input The address of the input data.
 * @param  len The length of the data.
 * @param  tag The address of the CMAC output data.
 * @note
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the cipher engine at the same time.
 *         - The length of input data must be 32-byte (cache line size) aligned.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_sym_cmac(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, const u8 *input, u32 len, u8 *tag);

/** @} */

/** @defgroup Crypto_Symmetric_Cipher_AEAD_Functions Crypto Symmetric Cipher AEAD Functions
  * @{
  */

/**
 * @brief Perform symmetric cipher GCM mode encryption and tag generation (DMA mode).
 * @param  mode Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param  key_id Value of @ref KM_KEY. Used only when key_addr is NULL.
 * @param  key_len_bits
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param  is_encryption Encryption mode flag: 1 for encryption, 0 for decryption.
 * @param  len The length of the plaintext.
 * @param  iv_addr IV array address.
 * @param  iv_len IV length.
 * @param  aad_addr The address of the AAD data.
 * @param  aad_len The length of the AAD data.
 * @param  input The address of the plaintext data.
 * @param  output The address of the ciphertext data.
 * @param  tag_len The length of the tag.
 * @param  tag The address of the tag output data.
 * @note
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the cipher engine at the same time.
 *         - The length of input data, output data and aad data must be 32-byte (cache line size) aligned.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_sym_gcm_encrypt_and_tag(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, u8 is_encryption, u32 len, u8 *iv_addr, u32 iv_len,
		u8 *aad_addr,
		u32 aad_len, const u8 *input,
		u8 *output, u32 tag_len, u8 *tag);

/**
 * @brief Perform symmetric cipher GCM mode authenticated decryption (DMA mode).
 * @param  mode Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param  key_id Value of @ref KM_KEY. Used only when key_addr is NULL.
 * @param  key_len_bits
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param  len The length of the ciphertext.
 * @param  iv_addr IV array address.
 * @param  iv_len IV length.
 * @param  aad_addr The address of the AAD data.
 * @param  aad_len The length of the AAD data.
 * @param  input The address of the ciphertext data.
 * @param  output The address of the plaintext data.
 * @param  tag_len The length of the tag.
 * @param  tag The address of the tag input data.
 * @note
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the cipher engine at the same time.
 *         - The length of input data, output data and aad data must be 32-byte (cache line size) aligned.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_sym_gcm_authenticated_decrypt(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, u32 len, u8 *iv_addr, u32 iv_len, u8 *aad_addr,
		u32 aad_len,
		const u8 *input, u8 *output, u32 tag_len, u8 *tag);

/**
 * @brief Perform symmetric cipher CCM mode encryption and tag generation (DMA mode).
 * @param  mode Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param  key_id Value of @ref KM_KEY. Used only when key_addr is NULL.
 * @param  key_len_bits
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param  key_addr Software key address, set to NULL if use hardware key
 * @param  is_encryption Encryption mode flag: 1 for encryption, 0 for decryption.
 * @param  msg_len The length of the plaintext.
 * @param  nonce_addr Nonce array address.
 * @param  nonce_len Nonce length.
 * @param  aad_addr The address of the AAD data.
 * @param  aad_len The length of the AAD data.
 * @param  input The address of the plaintext data.
 * @param  output The address of the ciphertext data.
 * @param  tag_len The length of the tag.
 * @param  tag The address of the tag output data.
 * @note
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the cipher engine at the same time.
 *         - The length of input data, output data and aad data must be 32-byte (cache line size) aligned.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_sym_ccm_encrypt_and_tag(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, u8 *key_addr, u8 is_encryption, u32 msg_len,
		u8 *nonce_addr, u32 nonce_len, u8 *aad_addr, u32 aad_len,
		const u8 *input, u8 *output, u32 tag_len, u8 *tag);

/**
 * @brief Perform symmetric cipher CCM mode authenticated decryption (DMA mode).
 * @param  mode Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param  key_id Value of @ref KM_KEY. Used only when key_addr is NULL.
 * @param  key_len_bits
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param  key_addr Software key address, set to NULL if use hardware key
 * @param  msg_len The length of the ciphertext.
 * @param  nonce_addr Nonce array address.
 * @param  nonce_len Nonce length.
 * @param  aad_addr The address of the AAD data.
 * @param  aad_len The length of the AAD data.
 * @param  input The address of the ciphertext data.
 * @param  output The address of the plaintext data.
 * @param  tag_len The length of the tag.
 * @param  tag The address of the tag input data.
 * @note
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the cipher engine at the same time.
 *         - The length of input data, output data and aad data must be 32-byte (cache line size) aligned.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
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

#endif /* _AMEBA_CRYPTO_SYMMETRIC_CIPHER_H_ */