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

/** @defgroup SM3
  * @brief SM3 driver modules
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup SM3_Exported_Constants SM3 Exported Constants
  * @{
  */

/** @defgroup SM3_Mode
  * @{
  */
/* SM3 has only one mode, equivalent to SHA-256 in parameters */
#define SM3_256				SHA_256
/** @} */

/** @defgroup SM3_Block_Size
  * @{
  */
#define SM3_BLOCK_256			64
/** @} */

/** @defgroup SM3_Digest_Length
  * @{
  */
#define SM3_DIGEST_256			32
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
 * @brief SM3 init to read-only mode
 * @param  ctx: SM3 context. This function will init ctx.
 * @note The engine will be locked during the execution of this function,
 *       and will be unlocked after execution.
 *       Avoid multiple threads/CPUs using engine at the same time.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_sm3_init(SHA_context *ctx);

/**
  * @brief  SM3 update.
  * @param 	ctx: SM3 context
  * @param  input: Pointer to input data.
  * @param  dst: Pointer to output data. If in read-only mode, set it to NULL
  * @param  len: Input data length.
  * @note 1.If copy mode and read-only mode are mixed for update,
  *         then ensure that the message lengths of both copy mode and read-only mode are integer multiples of the SM3-BLOCK length.
  *         (64-byte alignment for SM3-256)
  * @note 2.The engine will be locked during the execution of this function,
  *         and will be unlocked after execution.
  *          Avoid multiple threads/CPUs using the SM3 engine at the same time.
  * @note 3.In copy mode, the destination address and calculation length must be 32 bytes (cacheline size) aligned.
  * @retval 0: success
  * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
  */
_LONG_CALL_ int crypto_sm3_update(SHA_context *ctx, const u8 *input, u8 *dst, size_t len);

/**
  * @brief  SM3 final process.
  * @param 	ctx: SM3 context
  * @param  output: the result of SM3 function
  * @note 1.Since SM3 is not support hw padding at the moment, it must do sw padding.
  * @note 2.The engine will be locked during the execution of this function,
  *         and will be unlocked after execution.
  *          Avoid multiple threads/CPUs using the SM3 engine at the same time.
  * @retval 0: success
  * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
  */
_LONG_CALL_ int crypto_sm3_final(SHA_context *ctx, u8 *output);

/** @} */

/** @defgroup SM3_HMAC_Functions SM3 HMAC Functions
  * @{
  */

/**
 * @brief HMAC init to read-only mode
 * @param  ctx: SM3 context.
 * @param  key_id: value of KM_HMAC_KEY in ameba_key_management.h
 * @param  key_len_bytes
 * @note The engine will be locked during the execution of this function,
 *       and will be unlocked after execution.
 *       Avoid multiple threads/CPUs using the SM3 engine at the same time.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
int crypto_hmac_sm3_init(SHA_context *ctx, u8 key_id, u32 key_len_bytes);

/**
 * @brief  HMAC update
 * @param  ctx: SM3 context.
 * @param  input: Pointer to input data.
 * @param  dst: Pointer to output data. If in read-only mode, set it to NULL
 * @param  len: Input data length.
 * @note 1.If copy mode and read-only mode are mixed for update,
 *         then ensure that the message lengths of both copy mode and read-only mode are integer multiples of the SM3-BLOCK length.
 *         (64-byte alignment for SM3-256)
 * @note  2.The HMAC KEY will be reloaded before each update. If a software key is used,
 *         and the key is modified between updates, the calculation result will be wrong.
 *         So if the value of the software key is modified in the middle, please modify it to the correct key before updating.
 * @note   3.The engine will be locked during the execution of this function,
 *         and will be unlocked after execution.
 *         Avoid multiple threads/CPUs using the SM3 engine at the same time.
 * @note   4.In copy mode, the destination address and calculation length must be 32 bytes (cacheline size) aligned.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_hmac_sm3_update(SHA_context *ctx, const u8 *input, u8 *dst, size_t len);

/**
 * @brief  HMAC final process.
 * @param  ctx: SM3 context.
 * @param  output: the result of HMAC function
 * @note 1.The HMAC KEY will be reloaded before the final process.
 *         If a software key is used and the key is modified in final, the calculation result will be wrong.
 *         So if the value of the software key is modified in the middle, please modify it to the correct key before the final process.
 * @note  2.The engine will be locked during the execution of this function,
 *         and will be unlocked after execution.
 *         Avoid multiple threads/CPUs using the SM3 engine at the same time.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
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