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

/** @defgroup Crypto_Hash Crypto Hash
  * @brief Crypto Hash driver modules
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup Crypto_Hash_Exported_Constants Crypto Hash Exported Constants
  * @{
  */

/** @defgroup Hash_Algorithm_Selection Hash Algorithm Selection
  * @{
  */
#define HASH_ALGO_SHA2			((u8)0x00)	/*!< Select SHA-2 hash algorithm (SHA-224/256/384/512) */
#define HASH_ALGO_SM3			((u8)0x01)	/*!< Select SM3 hash algorithm */
/** @} */

/** @defgroup Hash_Block_Size Hash Block Size
  * @{
  */
#define HASH_BLOCK_SIZE_224		64		/*!< SHA-224 block size in bytes (512-bit block) */
#define HASH_BLOCK_SIZE_256		64		/*!< SHA-256 block size in bytes (512-bit block) */
#define HASH_BLOCK_SIZE_384		128		/*!< SHA-384 block size in bytes (1024-bit block) */
#define HASH_BLOCK_SIZE_512		128		/*!< SHA-512 block size in bytes (1024-bit block) */
#define HASH_BLOCK_SIZE_SM3		64		/*!< SM3 block size in bytes (512-bit block) */
/** @} */

/** @defgroup Hash_Digest_Length Hash Digest Length
  * @{
  */
#define HASH_DIGEST_224			28		/*!< SHA-224 digest output length in bytes */
#define HASH_DIGEST_256			32		/*!< SHA-256 digest output length in bytes */
#define HASH_DIGEST_384			48		/*!< SHA-384 digest output length in bytes */
#define HASH_DIGEST_512			64		/*!< SHA-512 digest output length in bytes */
#define HASH_DIGEST_SM3			32		/*!< SM3 digest output length in bytes */
/** @} */

/** @defgroup Crypto_Timeout Crypto Timeout
  * @{
  */
#define CRYPTO_DMA_TIMEOUT			((u32) 1000000)		/*!< DMA mode operation timeout count */
#define CRYPTO_MUTEX_TIMEOUT			((u32) 1000000)		/*!< Engine mutex acquisition timeout count */
#define CRYPTO_SLAVE_TIMEOUT			((u32) 10000)		/*!< Slave mode operation timeout count */
/** @} */

/** @} */

/* Exported functions --------------------------------------------------------*/
/* group: Crypto_Hash_Exported_Functions */
/* Since following functions which will not be extracted to the doxygen doc, remove the definition of the doxygen group. */

/** @defgroup Crypto_Hash_Normal_Functions Crypto Hash Normal Functions
  * @{
  */

/**
 * @brief Initialize the hash engine
 * @param  mode Hash engine mode
 *            @arg HASH_SHA2_MODE
 *            @arg HASH_SM3_MODE
 * @param  ctx Hash context. This function will initialize the context.
 * @param  SHAtype Hash algorithm type
 *            @arg SHA_224
 *            @arg SHA_256
 *            @arg SHA_384
 *            @arg SHA_512
 *            @arg 0 (ignored when mode is HASH_SM3_MODE)
 * @note 1. The engine will be locked during the execution of this function,
 *       and will be unlocked after execution.
 *       Avoid multiple threads/CPUs using the hash engine at the same time.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_hw_hash_init(hash_engine_mode mode, SHA_context *ctx, u32 SHAtype);

/**
 * @brief Update the hash computation with new input data
 * @param  ctx Hash context
 * @param  input Pointer to input data.
 * @param  dst Pointer to output buffer (copy mode, DMA destination). Set to NULL for read-only mode (compute only, no data copy).
 * @param  len Input data length.
 * @note
 *         - If copy mode and read-only mode are mixed for update,
 *           then ensure that the message lengths of both copy mode and read-only mode are integer multiples of the hash BLOCK length.
 *           (64-byte alignment for SHA-224/256/SM3-256, 128-byte alignment for SHA-384/512)
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the hash engine at the same time.
 *         - In copy mode, the destination address and calculation length must be 32 bytes (cache line size) aligned.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_hw_hash_update(SHA_context *ctx, const u8 *input, u8 *dst, size_t len);

/**
 * @brief Finalize the hash computation and output the digest
 * @param  ctx Hash context
 * @param  output The result of hash function
 * @note
 *         - Since SHA2/SM3 does not support hardware padding, software padding is performed instead.
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the hash engine at the same time.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_hw_hash_final(SHA_context *ctx, u8 *output);

/** @} */

/** @defgroup Crypto_Hash_HMAC_Functions Crypto Hash HMAC Functions
  * @{
  */

/**
 * @brief Initialize HMAC in read-only mode
 * @param  mode Hash engine mode
 *            @arg HASH_SHA2_MODE
 *            @arg HASH_SM3_MODE
 * @param  ctx Hash context.
 * @param  SHAtype Hash algorithm type:
 *            @arg SHA_224
 *            @arg SHA_256
 *            @arg SHA_384
 *            @arg SHA_512
 *            @arg 0 (ignored when mode is HASH_SM3_MODE)
 * @param  key_id Value of @ref KM_KEY
 * @param  key_len_bits HMAC key length in bits:
 *            @arg SHA_HMAC_KEY_BIT_128
 *            @arg SHA_HMAC_KEY_BIT_192
 *            @arg SHA_HMAC_KEY_BIT_256
 * @note The engine will be locked during the execution of this function,
 *       and will be unlocked after execution.
 *       Avoid multiple threads/CPUs using the hash engine at the same time.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_hmac_hash_init(hash_engine_mode mode, SHA_context *ctx, u32 SHAtype, u8 key_id, u32 key_len_bits);

/**
 * @brief  Update the HMAC computation with new input data
 * @param  ctx Hash context.
 * @param  input Pointer to input data.
 * @param  dst Pointer to output buffer (copy mode, DMA destination). Set to NULL for read-only mode (compute only, no data copy).
 * @param  len Input data length.
 * @note
 *         - If copy mode and read-only mode are mixed for update,
 *           then ensure that the message lengths of both copy mode and read-only mode are integer multiples of the hash BLOCK length.
 *           (64-byte alignment for SHA-224/256/SM3-256, 128-byte alignment for SHA-384/512)
 *         - The HMAC key will be reloaded before each update. If a software key is used,
 *           and the key is modified between updates, the calculation result will be wrong.
 *           Ensure the software key is set to the correct value before calling update.
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the hash engine at the same time.
 *         - In copy mode, the destination address and calculation length must be 32 bytes (cache line size) aligned.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_hmac_hash_update(SHA_context *ctx, const u8 *input, u8 *dst, size_t len);

/**
 * @brief  Finalize the HMAC computation and output the result
 * @param  ctx Hash context.
 * @param  output The result of HMAC function
 * @note
 *         - The HMAC key will be reloaded before the final process.
 *           If a software key is used and the key is modified in final, the calculation result will be wrong.
 *           Ensure the software key is set to the correct value before calling the final process.
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the hash engine at the same time.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_hmac_hash_final(SHA_context *ctx, u8 *output);

#ifdef __cplusplus
}
#endif

/** @} */
/** @} */
/** @} */

#endif /* _AMEBA_CRYPTO_HASH_H_ */