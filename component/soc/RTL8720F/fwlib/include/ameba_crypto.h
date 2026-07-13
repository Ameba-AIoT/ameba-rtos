/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBAGREEN2_KEY_MANAGEMENT_H_
#define _AMEBAGREEN2_KEY_MANAGEMENT_H_

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif

//Please add your definition here

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @brief Evaluate `f` and exit with an error on failure. */
#define CRYPTO_CHK(f, errno) do { if((ret = f) != 0) {ret = errno; goto exit;}} while( 0 )

/** @defgroup HW_CRYPTO_ENGINE HW Crypto Engine
  * @brief HW_CRYPTO_ENGINE driver modules
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup HW_CRYPTO_ENGINE_Exported_Constants HW_CRYPTO_ENGINE Exported Constants
  * @{
  */

/** @defgroup CRYPTO_Process_Status Crypto Process Status
  * @{
  */

#define _ERRNO_CRYPTO_DESC_NUM_SET_OutRange     -2    /*!< Descriptor number out of valid range */
#define _ERRNO_CRYPTO_BURST_NUM_SET_OutRange    -3    /*!< DMA burst number out of valid range */
#define _ERRNO_CRYPTO_NULL_POINTER              -4    /*!< NULL pointer passed as argument */
#define _ERRNO_CRYPTO_ENGINE_NOT_INIT           -5    /*!< Crypto engine not initialized */
#define _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned    -6    /*!< Buffer address not 4-byte aligned */
#define _ERRNO_CRYPTO_KEY_OutRange              -7    /*!< Key index out of valid range */
#define _ERRNO_CRYPTO_MSG_OutRange              -8    /*!< Message length out of valid range */
#define _ERRNO_CRYPTO_IV_OutRange               -9    /*!< IV length out of valid range */
#define _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH       -10   /*!< Authentication type mismatch */
#define _ERRNO_CRYPTO_CIPHER_TYPE_NOT_MATCH     -11   /*!< Cipher type mismatch */
#define _ERRNO_CRYPTO_KEY_IV_LEN_DIFF           -12   /*!< Key and IV length mismatch */
#define _ERRNO_CRYPTO_HASH_FINAL_NO_UPDATE      -13   /*!< hash_final called without prior hash_update */
#define _ERRNO_CRYPTO_KEY_SECURE_ERR            -14   /*!< Secure key access error */
#define _ERRNO_CRYPTO_KEY_LENGTH_ERR            -15   /*!< Invalid key length */
#define _ERRNO_CRYPTO_KEY_LOAD_ERR              -16   /*!< Key load failure */
#define _ERRNO_CRYPTO_DMA_ERR                   -17   /*!< DMA transfer error */
#define _ERRNO_CRYPTO_MESSAGE_LEN_ERR           -18   /*!< Invalid message length */
#define _ERRNO_CRYPTO_TAG_OutRange              -19   /*!< Tag length out of valid range */
#define _ERRNO_CRYPTO_GCM_TAG_NOT_MATCH         -20   /*!< GCM authentication tag verification failed */
#define _ERRNO_CRYPTO_SLAVE_TIME_OUT            -21   /*!< Slave mode operation timeout */
#define _ERRNO_CRYPTO_SLAVE_ERROR               -22   /*!< Slave mode hardware error */
#define _ERRNO_CRYPTO_AAD_LENGTH_OutRange       -23   /*!< AAD length out of valid range */
#define _ERRNO_CRYPTO_DMA_TIMEOUT               -24   /*!< DMA operation timeout */
#define _ERRNO_CRYPTO_MODE_ERR                  -25   /*!< Invalid cipher mode */
#define _ERRNO_CRYPTO_BUFFER_TOO_SMALL          -26   /*!< Output buffer too small */
#define _ERRNO_CRYPTO_AUTH_FAILED               -27   /*!< Authentication/tag verification failed */
/** @} */

/** @defgroup CRYPTO_Key Crypto Key
  * @{
  */
#define KEY_BIT_128					(128)	/*!< 128 bits */
#define KEY_BIT_192					(192)	/*!< 192 bits */
#define KEY_BIT_256					(256)	/*!< 256 bits */
/** @brief Check whether LEN is a valid AES key length in bits. */
#define CRYPTO_IS_KEY_LEN_BIT(LEN) (((LEN) == KEY_BIT_128) || \
						 	 ((LEN) == KEY_BIT_192) || \
							 ((LEN) == KEY_BIT_256))

/** @brief Reverse the byte order of a 32-bit word (swap big-endian and little-endian). */
#define BIG_LITTLE_SWAP32(x)        ( (((*(long int *)&x) & 0xff000000) >> 24) | \
                                      (((*(long int *)&x) & 0x00ff0000) >> 8) | \
                                      (((*(long int *)&x) & 0x0000ff00) << 8) | \
                                      (((*(long int *)&x) & 0x000000ff) << 24) )
/** @} */
/** @} */

/* Exported functions --------------------------------------------------------*/
/* group: HW_CRYPTO_ENGINE_Exported_Functions */
/* Since following functions which will not be extracted to the doxygen doc, remove the definition of the doxygen group. */

/**
 * @brief Enable crypto engine (both SHA and AES engines) function and clock.
 *        Crypto DMA is enabled by default in the bootloader.
 */
_LONG_CALL_ void CRYPTO_Init(void);

/**
 * @brief Control whether the driver internally checks if the data length is cache line (32-byte) aligned
 *        (if there is no length alignment, the log will be output and the calculation will continue).
 *        We strongly recommend that the data length be cache line aligned,
 *        otherwise you may encounter cache inconsistency when executing cache line invalidation.
 * @param is_valid 1 to enable the alignment check; 0 to disable it.
 */
_LONG_CALL_ void CRYPTO_Length_Not_Alined_Warning(int is_valid);

/**
 * @brief Control whether the driver internally checks if the destination address is cache line (32-byte) aligned
 *        (if there is no address alignment, the log will be output and the calculation will continue).
 *        We strongly recommend that the destination address be cache line aligned,
 *        otherwise you may encounter cache inconsistency when executing cache line invalidation.
 * @param is_valid 1 to enable the address alignment check; 0 to disable it.
 */
_LONG_CALL_ void CRYPTO_Addr_Not_Alined_Warning(int is_valid);

_LONG_CALL_ void crypto_dump_word(void *addr, size_t len, char *name);
_LONG_CALL_ void crypto_dump_bytes(void *addr, size_t len, char *name);

/** @} */

/** @} */
#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

#endif
