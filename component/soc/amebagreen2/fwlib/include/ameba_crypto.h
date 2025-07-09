/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBAGREEN2_KEY_MANAGEMENT_H_
#define _AMEBAGREEN2_KEY_MANAGEMENT_H_

/* MANUAL_GEN_START */
//Please add your definition here

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

#define _ERRNO_CRYPTO_DESC_NUM_SET_OutRange     -2
#define _ERRNO_CRYPTO_BURST_NUM_SET_OutRange    -3
#define _ERRNO_CRYPTO_NULL_POINTER              -4
#define _ERRNO_CRYPTO_ENGINE_NOT_INIT           -5
#define _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned    -6
#define _ERRNO_CRYPTO_KEY_OutRange              -7
#define _ERRNO_CRYPTO_MSG_OutRange              -8
#define _ERRNO_CRYPTO_IV_OutRange               -9
#define _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH       -10
#define _ERRNO_CRYPTO_CIPHER_TYPE_NOT_MATCH     -11
#define _ERRNO_CRYPTO_KEY_IV_LEN_DIFF           -12
#define _ERRNO_CRYPTO_HASH_FINAL_NO_UPDATE      -13
#define _ERRNO_CRYPTO_KEY_SECURE_ERR            -14
#define _ERRNO_CRYPTO_KEY_LENGTH_ERR            -15
#define _ERRNO_CRYPTO_KEY_LOAD_ERR              -16
#define _ERRNO_CRYPTO_DMA_ERR                   -17
#define _ERRNO_CRYPTO_MESSAGE_LEN_ERR           -18
#define _ERRNO_CRYPTO_TAG_OutRange              -19
#define _ERRNO_CRYPTO_GCM_TAG_NOT_MATCH         -20
#define _ERRNO_CRYPTO_SLAVE_TIME_OUT            -21
#define _ERRNO_CRYPTO_SLAVE_ERROR               -22
/** @} */

/** @defgroup CRYPTO_Key
  * @{
  */
#define KEY_BIT_128					(128)	/*!< 128 bits */
#define KEY_BIT_192					(192)	/*!< 192 bits */
#define KEY_BIT_256					(256)	/*!< 256 bits */
#define IS_KEY_LEN_BIT(LEN) (((LEN) == KEY_BIT_128) || \
						 	 ((LEN) == KEY_BIT_192) || \
							 ((LEN) == KEY_BIT_256))

#define BIG_LITTLE_SWAP32(x)        ( (((*(long int *)&x) & 0xff000000) >> 24) | \
                                      (((*(long int *)&x) & 0x00ff0000) >> 8) | \
                                      (((*(long int *)&x) & 0x0000ff00) << 8) | \
                                      (((*(long int *)&x) & 0x000000ff) << 24) )
/** @} */
/** @} */

/* Exported functions --------------------------------------------------------*/
/** @defgroup HW_CRYPTO_ENGINE_Exported_Functions HW_CRYPTO_ENGINE Exported Functions
  * @{
  */

/**
 * @brief Enable crypto engine (both SHA and AES engine) function and clock.
 *        We will default enable crypto DMA in bootloader.
 * @retval None
 */
_LONG_CALL_ void CRYPTO_Init(void);

/**
 * @brief It is used to drive the internal judgment of whether the input destination address is cacheline (32 Byte) aligned
 *        (if there is no address alignment, the log will be output and the calculation will continue).
 *        We strongly recommend that the destination address be cacheline aligned,
 *        otherwise you may encounter cache inconsistency when executing cacheline invalid.
 * @retval None
 */
_LONG_CALL_ void CRYPTO_Length_Not_Alined_Warning(int is_valid);

/**
 * @brief It is used to drive the internal judgment of whether the data length is cacheline (32 Byte) aligned
 *        (if there is no length alignment, the log will be output and the calculation will continue).
 *        We strongly recommend that the data length be cacheline aligned,
 *        otherwise you may encounter cache inconsistency when executing cacheline invalid.
 * @retval None
 */


_LONG_CALL_ void CRYPTO_Addr_Not_Alined_Warning(int is_valid);
/** @} */

/** @} */

/** @} */
/* MANUAL_GEN_END */

#endif
