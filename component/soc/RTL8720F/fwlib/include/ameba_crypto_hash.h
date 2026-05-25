/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_CRYPTO_HASH_H_
#define _AMEBA_CRYPTO_HASH_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup Crypto_Hash
  * @brief Crypto Hash driver modules
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup Crypto_Hash_Exported_Constants Crypto Hash Exported Constants
  * @{
  */

/** @defgroup Hash_Algorithm_Selection
  * @{
  */
#define HASH_ALGO_SHA2			((u8)0x00)
#define HASH_ALGO_SM3			((u8)0x01)
/** @} */

/** @defgroup Hash_Block_Size
  * @{
  */
#define HASH_BLOCK_SIZE_224		64
#define HASH_BLOCK_SIZE_256		64
#define HASH_BLOCK_SIZE_384		128
#define HASH_BLOCK_SIZE_512		128
#define HASH_BLOCK_SIZE_SM3		64
/** @} */

/** @defgroup Hash_Digest_Length
  * @{
  */
#define HASH_DIGEST_224			28
#define HASH_DIGEST_256			32
#define HASH_DIGEST_384			48
#define HASH_DIGEST_512			64
#define HASH_DIGEST_SM3			32
/** @} */

/** @defgroup Crypto_Timeout
  * @{
  */
#define CRYPTO_DMA_TIMEOUT				((u32) 1000000)
#define CRYPTO_MUTEX_TIMEOUT				((u32) 1000000)
#define CRYPTO_SLAVE_TIMEOUT				((u32) 10000)
/** @} */

/** @} */

/* Exported functions --------------------------------------------------------*/
/** @defgroup Crypto_Hash_Exported_Functions Crypto Hash Exported Functions
  * @{
  */

/** @defgroup Crypto_Hash_Normal_Functions Crypto Hash Normal Functions
  * @{
  */

/**
 * @brief Hash initialization function
 * @param  is_sm3: 0 for SHA2, 1 for SM3
 * @param  ctx: Hash context. This function will init ctx.
 * @param  SHAtype: Hash algorithm type
 *            @arg SHA_MODE_224
 *            @arg SHA_MODE_256
 *            @arg SHA_MODE_384
 *            @arg SHA_MODE_512
 *            @arg NULL for SM3
 * @note 1. The engine will be locked during the execution of this function,
 *       and will be unlocked after execution.
 *       Avoid multiple threads/CPUs using the hash engine at the same time.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_hw_hash_init(u8 is_sm3, SHA_context *ctx, u32 SHAtype);

/**
 * @brief Hash update function
 * @param  is_sm3: 0 for SHA2, 1 for SM3
 * @param  ctx: Hash context
 * @param  input: Pointer to input data.
 * @param  dst: Pointer to output data. If in read-only mode, set it to NULL
 * @param  len: Input data length.
 * @note 1.If copy mode and read-only mode are mixed for update,
 *         then ensure that the message lengths of both copy mode and read-only mode are integer multiples of the hash BLOCK length.
 *         (64-byte alignment for SHA-224/256/SM3-256, 128-byte alignment for SHA-384/512)
 * @note 2.The engine will be locked during the execution of this function,
 *         and will be unlocked after execution.
 *          Avoid multiple threads/CPUs using the hash engine at the same time.
 * @note 3.In copy mode, the destination address and calculation length must be 32 bytes (cacheline size) aligned.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_hw_hash_update(SHA_context *ctx, const u8 *input, u8 *dst, size_t len);

/**
 * @brief Hash final function
 * @param  is_sm3: 0 for SHA2, 1 for SM3
 * @param  ctx: Hash context
 * @param  output: the result of hash function
 * @note 1.Since SHA3/SM3 is not support hw padding at the moment, it must do sw padding.
 * @note 2.The engine will be locked during the execution of this function,
 *         and will be unlocked after execution.
 *          Avoid multiple threads/CPUs using the hash engine at the same time.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_hw_hash_final(SHA_context *ctx, u8 *output);

/** @} */

/** @defgroup Crypto_Hash_HMAC_Functions Crypto Hash HMAC Functions
  * @{
  */

/**
 * @brief HMAC init to read-only mode
 * @param  is_sm3: 0 for SHA2, 1 for SM3
 * @param  ctx: Hash context.
 * @param  SHAtype:
 *            @arg SHA_MODE_224
 *            @arg SHA_MODE_256
 *            @arg SHA_MODE_384
 *            @arg SHA_MODE_512
 *            @arg NULL for SM3
 * @param  key_id: value of KM_HMAC_KEY in ameba_key_management.h
 * @param  key_len_bits:
 *            @arg SHA_HMAC_KEY_BIT_128
 *            @arg SHA_HMAC_KEY_BIT_192
 *            @arg SHA_HMAC_KEY_BIT_256
 * @note The engine will be locked during the execution of this function,
 *       and will be unlocked after execution.
 *       Avoid multiple threads/CPUs using the hash engine at the same time.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_hmac_hash_init(u8 is_sm3, SHA_context *ctx, u32 SHAtype, u8 key_id, u32 key_len_bits);

/**
 * @brief  HMAC update
 * @param  is_sm3: 0 for SHA2, 1 for SM3
 * @param  ctx: Hash context.
 * @param  input: Pointer to input data.
 * @param  dst: Pointer to output data. If in read-only mode, set it to NULL
 * @param  len: Input data length.
 * @note 1.If copy mode and read-only mode are mixed for update,
 *         then ensure that the message lengths of both copy mode and read-only mode are integer multiples of the hash BLOCK length.
 *         (64-byte alignment for SHA-224/256/SM3-256, 128-byte alignment for SHA-384/512)
 * @note  2.The HMAC KEY will be reloaded before each update. If a software key is used,
 *         and the key is modified between updates, the calculation result will be wrong.
 *         So if the value of the software key is modified in the middle, please modify it to the correct key before updating.
 * @note   3.The engine will be locked during the execution of this function,
 *         and will be unlocked after execution.
 *         Avoid multiple threads/CPUs using the hash engine at the same time.
 * @note   4.In copy mode, the destination address and calculation length must be 32 bytes (cacheline size) aligned.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_hmac_hash_update(SHA_context *ctx, const u8 *input, u8 *dst, size_t len);

/**
 * @brief  HMAC final process
 * @param  is_sm3: 0 for SHA2, 1 for SM3
 * @param  ctx: Hash context.
 * @param  output: the result of HMAC function
 * @note 1.The HMAC KEY will be reloaded before the final process.
 *         If a software key is used and the key is modified in final, the calculation result will be wrong.
 *         So if the value of the software key is modified in the middle, please modify it to the correct key before the final process.
 * @note  2.The engine will be locked during the execution of this function,
 *         and will be unlocked after execution.
 *         Avoid multiple threads/CPUs using the hash engine at the same time.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_hmac_hash_final(SHA_context *ctx, u8 *output);

#ifdef __cplusplus
}
#endif

/** @} */
/** @} */
/** @} */
/** @} */

#endif /* _AMEBA_CRYPTO_HASH_H_ */