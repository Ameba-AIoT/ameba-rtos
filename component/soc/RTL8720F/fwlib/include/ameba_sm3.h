/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SM3_H_
#define _AMEBA_SM3_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup SM3 SM3
  * @brief SM3 driver modules
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup SM3_Exported_Constants SM3 Exported Constants
  * @{
  */

/** @defgroup SM3_Mode SM3 Mode
  * @{
  */
#define SM3_256				SHA_256	/*!< SM3 operates in a single mode equivalent to SHA-256 (256-bit block, 256-bit digest) */
/** @} */

/** @defgroup SM3_Block_Size SM3 Block Size
  * @{
  */
#define SM3_BLOCK_256			64	/*!< SM3 block size in bytes (512-bit block = 64 bytes) */
/** @} */

/** @defgroup SM3_Digest_Length SM3 Digest Length
  * @{
  */
#define SM3_DIGEST_256			32	/*!< SM3 digest output length in bytes */
/** @} */

/** @} */

/* Exported functions --------------------------------------------------------*/
/** @defgroup SM3_Exported_Functions SM3 Exported Functions
  * @{
  */

/** @defgroup SM3_Normal_Functions SM3 Normal Functions
  * @{
  */

/**
 * @brief Initialize the SM3 context for read-only DMA mode computation.
 * @param  ctx SM3 context. This function will initialize the context.
 * @note The SM3 engine will be locked during the execution of this function,
 *       and will be unlocked after execution.
 *       Avoid multiple threads/CPUs using the SM3 engine at the same time.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_sm3_init(SHA_context *ctx);

/**
  * @brief  Update SM3 state.
  * @param 	ctx SM3 context
  * @param  input Pointer to input data.
  * @param  dst Pointer to output data. If in read-only mode, set it to NULL
  * @param  len Input data length.
  * @note
  *         - If copy mode and read-only mode are mixed for update,
  *           then ensure that the message lengths of both copy mode and read-only mode are integer multiples of the SM3-BLOCK length.
  *           (64-byte alignment for SM3-256)
  *         - The engine will be locked during the execution of this function,
  *           and will be unlocked after execution.
  *           Avoid multiple threads/CPUs using the SM3 engine at the same time.
  *         - In copy mode, the destination address and calculation length must be 32 bytes (cache line size) aligned.
  * @return Process status:
  *         - 0: success
  *         - Other: error code. Refer to @ref CRYPTO_Process_Status
  */
_LONG_CALL_ int crypto_sm3_update(SHA_context *ctx, const u8 *input, u8 *dst, size_t len);

/**
  * @brief  Finalize SM3 computation.
  * @param 	ctx SM3 context
  * @param  output The result of SM3 function
  * @note
  *         - Since SM3 does not support hardware padding, software padding must be applied.
  *         - The engine will be locked during the execution of this function,
  *           and will be unlocked after execution.
  *           Avoid multiple threads/CPUs using the SM3 engine at the same time.
  * @return Process status:
  *         - 0: success
  *         - Other: error code. Refer to @ref CRYPTO_Process_Status
  */
_LONG_CALL_ int crypto_sm3_final(SHA_context *ctx, u8 *output);

/** @} */

/** @defgroup SM3_HMAC_Functions SM3 HMAC Functions
  * @{
  */

/**
 * @brief Initialize the HMAC-SM3 context for read-only DMA mode computation.
 * @param  ctx SM3 context.
 * @param  key_id Value of @ref KM_KEY
 * @param  key_len_bytes Key length in bytes (not bits, unlike crypto_hmac_sha2_init). Valid values:
 *            @arg 16 (128-bit key)
 *            @arg 24 (192-bit key)
 *            @arg 32 (256-bit key)
 * @note The engine will be locked during the execution of this function,
 *       and will be unlocked after execution.
 *       Avoid multiple threads/CPUs using the SM3 engine at the same time.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
int crypto_hmac_sm3_init(SHA_context *ctx, u8 key_id, u32 key_len_bytes);

/**
 * @brief  Update HMAC state.
 * @param  ctx SM3 context.
 * @param  input Pointer to input data.
 * @param  dst Pointer to output data. If in read-only mode, set it to NULL
 * @param  len Input data length.
 * @note
 *         - If copy mode and read-only mode are mixed for update,
 *           then ensure that the message lengths of both copy mode and read-only mode are integer multiples of the SM3-BLOCK length.
 *           (64-byte alignment for SM3-256)
 *         - The HMAC key will be reloaded before each update. If a software key is used,
 *           and the key is modified between updates, the calculation result will be wrong.
 *           Ensure the software key is set to the correct value before calling update.
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the SM3 engine at the same time.
 *         - In copy mode, the destination address and calculation length must be 32 bytes (cache line size) aligned.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_hmac_sm3_update(SHA_context *ctx, const u8 *input, u8 *dst, size_t len);

/**
 * @brief  Finalize HMAC computation.
 * @param  ctx SM3 context.
 * @param  output The result of HMAC function
 * @note
 *         - The HMAC key will be reloaded before the final process.
 *           If a software key is used and the key is modified in final, the calculation result will be wrong.
 *           Ensure the software key is set to the correct value before calling the final process.
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the SM3 engine at the same time.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_hmac_sm3_final(SHA_context *ctx, u8 *output);

#ifdef __cplusplus
}
#endif

/** @} */
/** @} */
/** @} */
/** @} */

#endif /* _AMEBA_SM3_H_ */