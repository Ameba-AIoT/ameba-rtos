/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBAGREEN2_SHA_H_
#define _AMEBAGREEN2_SHA_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup SHA SHA
  * @brief SHA driver modules
  * @{
  */

/// @cond
/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup SHA_Register_Definitions SHA Register Definitions
  * @{
  */

/** @defgroup SHA_CONTROL
  * @brief HMAC engine control register
  * @{
  */
#define SHA_BIT_SOFT_RESET_TRIGGER_BUSY       ((u32)0x00000001 << 24)          /*!< R/W1T 0x0  Write 1 to trigger a soft reset. All registers except the mutex register will be reset. Read the bit until 0 to determine if the soft reset is complete.*/
#define SHA_BIT_FIFO_FULL_SUSPEND             ((u32)0x00000001 << 14)          /*!< R/W 0x1  User writes data to SHA_FIFO_in but the FIFO space is full. 0x0: accept signal is always asserted 0x1: accept signal is not asserted until FIFO space is available Note: This function is only available when Slave_mode = 0x1*/
#define SHA_BIT_KEY_BYTE_INVERSION_ENABLE     ((u32)0x00000001 << 13)          /*!< R/W 0x1  0x0: use native byte order in memory 0x1: inverse byte order of read data*/
#define SHA_BIT_PAYLOAD_BYTE_INVERSION_ENABLE ((u32)0x00000001 << 12)          /*!< R/W 0x1  0x0: use native byte order in memory 0x1: inverse byte order of read data*/
#define SHA_BIT_TSHA512_USE_256               ((u32)0x00000001 << 11)          /*!< R/W 0x0  Only available when config SUPPORT_TRUNCATED_SHA512 and SHA512_truncate_en= 0x1 (truncated SHA-512) 0: SHA-512/224 mode 1: SHA-512/256 mode*/
#define SHA_BIT_SHA512_TRUNCATE_EN            ((u32)0x00000001 << 10)          /*!< R/W 0x0  Only available when config SUPPORT_TRUNCATED_SHA512 and SHA_mode = 0x3 (SHA-512) 0: normal SHA-512 1: truncated SHA-512*/
#define SHA_MASK_HMAC_KEY_SIZE                ((u32)0x00000003 << 8)           /*!< R/W 0x0  Only available when config HMAC_MODE 0x0: 128 bits 0x1: 192 bits 0x2: 256 bits*/
#define SHA_HMAC_KEY_SIZE(x)                  (((u32)((x) & 0x00000003) << 8))
#define SHA_GET_HMAC_KEY_SIZE(x)              ((u32)(((x >> 8) & 0x00000003)))
#define SHA_BIT_SLAVE_MODE                    ((u32)0x00000001 << 6)           /*!< R/W 0x0  0: DMA mode. 1: slave mode. SW can push data by wrting FIFO Data Register in slave mode.*/
#define SHA_BIT_ICG_ENABLE                    ((u32)0x00000001 << 5)           /*!< R/W 0x1  Enable auto clock gating*/
#define SHA_BIT_HMAC_MODE                     ((u32)0x00000001 << 4)           /*!< R/W 0x0  Only available when config HMAC_MODE 0x0: use HASH mode. 0x1: use HMAC mode.*/
#define SHA_BIT_DMA_MODE                      ((u32)0x00000001 << 2)           /*!< R/W 0x0  0x0: read-only mode 0x1: copy mode*/
#define SHA_MASK_SHA2_MODE                    ((u32)0x00000003 << 0)           /*!< W/R 0x0  0x0: SHA-224 0x1: SHA-256 0x2: SHA-384 0x3: SHA-512*/
#define SHA_SHA2_MODE(x)                      (((u32)((x) & 0x00000003) << 0))
#define SHA_GET_SHA2_MODE(x)                  ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup SHA_STATUS
  * @brief HMAC engine status register
  * @{
  */
#define SHA_BIT_ENGINE_BUSY   ((u32)0x00000001 << 28)          /*!< RO 0x0  1: engine busy*/
#define SHA_MASK_FIFO_SPACE   ((u32)0x0000001F << 0)           /*!< RO 0x10  Number of available FIFO space in byte*/
#define SHA_FIFO_SPACE(x)     (((u32)((x) & 0x0000001F) << 0))
#define SHA_GET_FIFO_SPACE(x) ((u32)(((x >> 0) & 0x0000001F)))
/** @} */

/** @defgroup SHA_HMAC_BLOCK_CONTROL
  * @brief HMAC engine block control register
  * @{
  */
#define SHA_BIT_END_SLAVE_INPUT ((u32)0x00000001 << 2) /*!< R/W 0x0  Avaliable when using slave mode If all input data has been completed, set this bit to 1 to start padding.*/
#define SHA_BIT_LAST_BLOCK      ((u32)0x00000001 << 1) /*!< R/W 0x0  Indicates this operation is the last block. If set to SHA2 mode, the padding step will be executed at the end. If set to HMAC mode, the o_pad step will be executed at the end.*/
#define SHA_BIT_FIRST_BLOCK     ((u32)0x00000001 << 0) /*!< R/W 0x0  Indicates this operation is the first block. If set to SHA2 mode, the digest will be initialized automatically. If set to HMAC mode, the i_pad step will be executed first.*/
/** @} */

/** @defgroup SHA_DIGESTx
  * @brief Hash value x
  * @{
  */
#define SHA_MASK_DIGESTx   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0xcbbb9d5d  Hash value x. Reset value is SHA384 IV.*/
#define SHA_DIGESTx(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SHA_GET_DIGESTx(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SHA_FIFO_IN
  * @brief FIFO input data register for slave mode
  * @{
  */
#define SHA_MASK_FIFO_IN   ((u32)0xFFFFFFFF << 0)           /*!< WO 0x0  Input data register for slave mode.*/
#define SHA_FIFO_IN(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SHA_GET_FIFO_IN(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SHA_HASH_INPUT_LEN_x
  * @brief Total input byte length register x
  * @{
  */
#define SHA_MASK_HASH_INPUT_LENx   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Hash length[32*(3-x)+31:32*(3-x)]*/
#define SHA_HASH_INPUT_LENx(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SHA_GET_HASH_INPUT_LENx(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SHA_SHA256_DIGEST_ALIAS_x
  * @brief SHA256 Hash value register x
  * @{
  */
#define SHA_MASK_SHA256_DIGEST_ALIAS_x   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0xcbbb9d5d  SHA256 Hash value x. Reset value is SHA384 IV.*/
#define SHA_SHA256_DIGEST_ALIAS_x(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SHA_GET_SHA256_DIGEST_ALIAS_x(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SHA_DMA_HS_CFG
  * @brief DMA hardware handshake configuration register
  * @{
  */
#define SHA_MASK_DMA_HS_MSIZE      ((u32)0x00000007 << 3)           /*!< R/W 0x0  The expected number of burst write transaction length. Please set this field to 3'b000 to avoid data loss.*/
#define SHA_DMA_HS_MSIZE(x)        (((u32)((x) & 0x00000007) << 3))
#define SHA_GET_DMA_HS_MSIZE(x)    ((u32)(((x >> 3) & 0x00000007)))
#define SHA_MASK_DMA_HS_TR_WIDTH   ((u32)0x00000007 << 0)           /*!< R/W 0x0  The expected data width of each write transaction from DMA. Since the engine uses the APB protocol, please set the field to 3'b010.*/
#define SHA_DMA_HS_TR_WIDTH(x)     (((u32)((x) & 0x00000007) << 0))
#define SHA_GET_DMA_HS_TR_WIDTH(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup SHA_DMA_HS_BLOCK_TS
  * @brief DMA hardware handshake block transfer size register
  * @{
  */
#define SHA_MASK_DMA_HS_BLOCK_TS   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Total number of data byte for requesting DMA*/
#define SHA_DMA_HS_BLOCK_TS(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SHA_GET_DMA_HS_BLOCK_TS(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SHA_DMA_HS_BUFF_BYTE
  * @brief DMA hardware handshake buffer data byte of engine register
  * @{
  */
#define SHA_MASK_DMA_HS_BUFF_BYTE   ((u32)0x000000FF << 0)           /*!< R/W 0x0  The register indicates number of data byte which has been processed in engine. This number will not larger than block size of engine.*/
#define SHA_DMA_HS_BUFF_BYTE(x)     (((u32)((x) & 0x000000FF) << 0))
#define SHA_GET_DMA_HS_BUFF_BYTE(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup SHA_DMA_HS_EN
  * @brief DMA hardware handshake enable register
  * @{
  */
#define SHA_BIT_DMA_HS_EN ((u32)0x00000001 << 0) /*!< R/W 0x0  Once the register is enabled, the engine starts to send dma_req until received wdata reaches trans_data_request.*/
/** @} */

/** @defgroup SHA_MUTEX_STATUS
  * @brief Mutex status register
  * @{
  */
#define SHA_MASK_ENGINE_MUTEX_STATUS   ((u32)0x00000003 << 0)           /*!< RS/W0C 0x0  If read 0, lock mutex. If not 0, the engine is being used. 2'b01: secure domain lock mutex 2'b10: non-secure domain lock mutex Write 0 to unlock mutex*/
#define SHA_ENGINE_MUTEX_STATUS(x)     (((u32)((x) & 0x00000003) << 0))
#define SHA_GET_ENGINE_MUTEX_STATUS(x) ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup SHA_MUTEX_OVERWRITE
  * @brief Mutex overwrite register
  * @{
  */
#define SHA_BIT_ENGINE_MUTEX_OVERWRITE ((u32)0x00000001 << 0) /*!< R/W (sec only) 0x0  Allows secure domain overwrite mutex 1: enable 0: disable*/
/** @} */

/** @defgroup SHA_CONFIGURATION
  * @brief Configuration information register
  * @{
  */
#define SHA_BIT_DISABLE_CORE_SM3         ((u32)0x00000001 << 14) /*!< RO   1: SM3 is disabled by hardware 0: SM3 is enabled*/
#define SHA_BIT_DISABLE_CORE_SHA1        ((u32)0x00000001 << 13) /*!< RO   1: SHA1 is disabled by hardware 0: SHA1 is enabled*/
#define SHA_BIT_DISABLE_CORE_SHA2        ((u32)0x00000001 << 12) /*!< RO   1: SHA2 is disabled by hardware 0: SHA2 is enabled*/
#define SHA_BIT_SM3_CORE_EN              ((u32)0x00000001 << 9)  /*!< RO   1: SM3 is implmented 0: SM3 is not implmented*/
#define SHA_BIT_SHA1_CORE_EN             ((u32)0x00000001 << 8)  /*!< RO   1: SHA1 is implmented 0: SHA1 is not implmented*/
#define SHA_BIT_TRUST_ZONE_EN            ((u32)0x00000001 << 7)  /*!< RO   0: disabled 1: enabled*/
#define SHA_BIT_KEY_MANAGER_EN           ((u32)0x00000001 << 6)  /*!< RO   0: disabled 1: enabled*/
#define SHA_BIT_EXTERNAL_KEY_EN          ((u32)0x00000001 << 5)  /*!< RO   0: disabled 1: enabled*/
#define SHA_BIT_SUPPORT_COPY_MODE        ((u32)0x00000001 << 4)  /*!< RO   0: disabled 1: enabled*/
#define SHA_BIT_SUPPORT_HMAC_MODE        ((u32)0x00000001 << 3)  /*!< RO   0: disabled 1: enabled*/
#define SHA_BIT_SUPPORT_TRUNCATED_SHA512 ((u32)0x00000001 << 2)  /*!< RO   0: disabled 1: enabled*/
#define SHA_BIT_SUPPORT_SHA512           ((u32)0x00000001 << 1)  /*!< RO   0: disabled 1: enabled*/
#define SHA_BIT_SHA2_CORE_EN             ((u32)0x00000001 << 0)  /*!< RO   1: SHA2 is implmented 0: SHA2 is not implmented*/
/** @} */

/** @defgroup SHA_VERSION
  * @brief Version number register
  * @{
  */
#define SHA_MASK_SVN_NUMBER       ((u32)0x0000FFFF << 16)           /*!< RO   */
#define SHA_SVN_NUMBER(x)         (((u32)((x) & 0x0000FFFF) << 16))
#define SHA_GET_SVN_NUMBER(x)     ((u32)(((x >> 16) & 0x0000FFFF)))
#define SHA_MASK_ENGINE_VERSION   ((u32)0x0000FFFF << 0)            /*!< RO   */
#define SHA_ENGINE_VERSION(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define SHA_GET_ENGINE_VERSION(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup SHA_DATE
  * @brief Generated date register
  * @{
  */
#define SHA_MASK_YEAR   ((u32)0x0000FFFF << 16)           /*!< RO   */
#define SHA_YEAR(x)     (((u32)((x) & 0x0000FFFF) << 16))
#define SHA_GET_YEAR(x) ((u32)(((x >> 16) & 0x0000FFFF)))
#define SHA_MASK_DATE   ((u32)0x0000FFFF << 0)            /*!< RO   */
#define SHA_DATE(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define SHA_GET_DATE(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup SHA_Exported_Types SHA Exported Types
  * @{
  */

/** @brief SHA Register Declaration
  */

typedef struct {
	__IO uint32_t SHA_CONTROL                 ;  /*!< HMAC ENGINE CONTROL REGISTER,  Address offset:0x000 */
	__I  uint32_t SHA_STATUS                  ;  /*!< HMAC ENGINE STATUS REGISTER,  Address offset:0x004 */
	__IO uint32_t SHA_HMAC_BLOCK_CONTROL      ;  /*!< HMAC ENGINE BLOCK CONTROL REGISTER,  Address offset:0x008 */
	__IO uint32_t RSVD0                       ;  /*!< Reserved,  Address offset:0x00C-0x00F */
	__IO uint32_t SHA_DIGESTx[16]             ;  /*!< HASH VALUE x Register,  Address offset:0x010-0x04C */
	__O  uint32_t SHA_FIFO_IN                 ;  /*!< FIFO INPUT DATA REGISTER FOR SLAVE MODE Register,  Address offset:0x050 */
	__IO uint32_t RSVD1[11]                   ;  /*!< Reserved,  Address offset:0x054-0x07F */
	__IO uint32_t SHA_HASH_INPUT_LEN_x[4]     ;  /*!< TOTAL INPUT BYTE LENGTH REGISTER x Register,  Address offset:0x080-0x08C */
	__IO uint32_t RSVD2[28]                   ;  /*!< Reserved,  Address offset:0x090-0x0FF */
	__IO uint32_t SHA_SHA256_DIGEST_ALIAS_x[8];  /*!< SHA256 HASH VALUE REGISTER x Register,  Address offset:0x100-0x11C */
	__IO uint32_t SHA_DMA_HS_CFG              ;  /*!< DMA HARDWARE HANDSHAKE CONFIGURATION REGISTER,  Address offset:0x120 */
	__IO uint32_t SHA_DMA_HS_BLOCK_TS         ;  /*!< DMA HARDWARE HANDSHAKE BLOCK TRANSFER SIZE REGISTER,  Address offset:0x124 */
	__IO uint32_t SHA_DMA_HS_BUFF_BYTE        ;  /*!< DMA HARDWARE HANDSHAKE BUFFER DATA BYTE OF ENGINE REGISTER,  Address offset:0x128 */
	__IO uint32_t SHA_DMA_HS_EN               ;  /*!< DMA HARDWARE HANDSHAKE ENABLE REGISTER,  Address offset:0x12C */
	__IO uint32_t RSVD3[756]                  ;  /*!< Reserved,  Address offset:0x130-0xCFF */
	__IO uint32_t SHA_MUTEX_STATUS            ;  /*!< MUTEX STATUS REGISTER,  Address offset:0xD00 */
	__IO uint32_t SHA_MUTEX_OVERWRITE         ;  /*!< MUTEX OVERWRITE REGISTER,  Address offset:0xD04 */
	__IO uint32_t RSVD4[186]                  ;  /*!< Reserved,  Address offset:0xD08-0xFEF */
	__I  uint32_t SHA_CONFIGURATION           ;  /*!< CONFIGURATION INFORMATION REGISTER,  Address offset:0xFF0 */
	__IO uint32_t RSVD5                       ;  /*!< Reserved,  Address offset:0xFF4-0xFF7 */
	__I  uint32_t SHA_VERSION                 ;  /*!< VERSION NUMBER REGISTER,  Address offset:0xFF8 */
	__I  uint32_t SHA_DATE                    ;  /*!< GENERATED DATE REGISTER,  Address offset:0xFFC */
} SHA_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */
/// @endcond

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


/* Exported Types --------------------------------------------------------*/
/** @addtogroup SHA_Exported_Types SHA Exported Types
  * @{
  */
/**
  * @brief  SHA Init Structure Definition
  */
typedef struct {
	u32 EngineMode;  /*!< Specifies the SHA engine mode. This parameter can be a value of @ref SHA_Engine_Mode. */
	u32 IcgEn;       /*!< Specifies enable or disable SHA auto clock gating. This parameter can be ENABLE or DISABLE. */
	u32 DmaMode;     /*!< Specifies SHA DMA mode. This parameter can be a value of @ref SHA_DMA_Mode. */
	u32 HmacMode;    /*!< Specifies the HMAC mode. This parameter can be a value of @ref SHA_HMAC_MODE. */
} SHA_InitTypeDef;

/**
  * @brief SHA Context Structure Definition
  */
typedef struct {
	u32 sha_mode;       /*!< Specifies the SHA mode. This parameter can be a value of @ref SHA_Mode. */
	u32 hmac_mode;      /*!< HMAC operation mode. 0x0: hash mode. 0x1: HMAC mode.*/
	u32 block_size;		/*!< Block size. This parameter can be a value of @ref SHA_BLOCK_SIZE. */
	u32 ilen_bytes;		/*!< Bytes occupied by input length storage. */
	u32 digest_len;		/*!< Digest length. */
	u32 state[16];      /*!< Digest state */
	u8 buffer[128] __attribute__((aligned(32)));   	/*!< Data block being processed */
	u32 buf_used_bytes;	/*!< Buffer bytes used */
	u32 total_len;	    /*!< The total number of data bytes to be processed */
	u8 dma_copy;        /*!< DMA operation mode. 0x0: read-only mode. 0x1: copy mode*/
	u8 seq_hash_first;	/*!< Flag for sequential hash: 1 = this is the first block, 0 = subsequent block */
	u8 key_id;			/*!< Key management key ID used for HMAC */
	u32 key_len_bits;	/*!< HMAC key length in bits */
	u32 DMA_CTL_LOW;	/*!< DMA control register low word value cached for this context */
} SHA_context;
/** @} */

/* Exported constants --------------------------------------------------------*/
/** @defgroup SHA_Exported_Constants SHA Exported Constants
  * @{
  */

/** @defgroup SHA_Mode SHA Mode
  * @{
  */
#define SHA_224			((u8)0x00)	/*!< SHA-224 algorithm */
#define SHA_256			((u8)0x01)	/*!< SHA-256 algorithm */
#define SHA_384			((u8)0x02)	/*!< SHA-384 algorithm */
#define SHA_512			((u8)0x03)	/*!< SHA-512 algorithm */

/** @brief Check whether MODE is a valid SHA-2 algorithm selection. */
#define IS_SHA_MODE(MODE) (((MODE) == SHA_224) || \
								((MODE) == SHA_256) || \
								((MODE) == SHA_384) || \
								((MODE) == SHA_512))
/** @} */

/** @defgroup SHA_Byte_Swap_Defaults SHA Byte Swap Defaults
  * @{
  */
#define SHA_MASK_BYTE_INVERSION         ((u32)0x00000003 << 12)	/*!< Bitmask for key and payload byte inversion bits in CONTROL register */
#define SHA_BYTE_SWAP_DEFAULTS          ((u32)(~SHA_BIT_KEY_BYTE_INVERSION_ENABLE) & SHA_BIT_PAYLOAD_BYTE_INVERSION_ENABLE)	/*!< Default: key in native order, payload byte-inverted */
#define SHA_BYTE_SWAP_KEY_SWAP          ((u32)  SHA_BIT_KEY_BYTE_INVERSION_ENABLE  | SHA_BIT_PAYLOAD_BYTE_INVERSION_ENABLE)	/*!< Both key and payload in inverse byte order */
/** @} */

/** @defgroup SHA_Engine_Mode SHA Engine Mode
  * @{
  */
#define SHA_DMA_MODE			((u8)0x00)	/*!< DMA mode: message data provided via DMA */
#define SHA_SLAVE_MODE			((u8)0x01)	/*!< Slave mode: message data written directly to FIFO */
/** @brief Check whether MODE is a valid SHA engine mode. */
#define IS_SHA_ENGINE_MODE(MODE) (((MODE) == SHA_DMA_MODE) || \
								((MODE) == SHA_SLAVE_MODE))
/** @} */

/** @defgroup SHA_DMA_Mode SHA DMA Mode
  * @{
  */
#define SHA_READ_MODE		    ((u8)0x00)	/*!< DMA read-only mode: engine reads source, no output copy */
#define SHA_COPY_MODE		    ((u8)0x01)	/*!< DMA copy mode: engine reads source and writes to destination */
#define SHA_COPY_TO_READ_SWITCH ((u8)0x01)	/*!< Switch from copy mode to read-only mode on next DMA transfer */
#define SHA_READ_TO_COPY_SWITCH ((u8)0x02)	/*!< Switch from read-only mode to copy mode on next DMA transfer */
/** @brief Check whether MODE is a valid SHA DMA mode. */
#define IS_SHA_DMA_MODE(MODE) (((MODE) == SHA_READ_MODE) || \
								((MODE) == SHA_COPY_MODE))
/** @} */

/** @defgroup SHA_Byte_Inv SHA Byte Inversion
  * @{
  */
#define SHA_NATIVE_ORDER		((u8)0x00)	/*!< Native (big-endian) byte order */
#define SHA_INVERSE_ORDER		((u8)0x01)	/*!< Inverse (little-endian) byte order */
/** @brief Check whether INV is a valid SHA byte inversion setting. */
#define IS_SHA_BYTE_INV(INV) (((INV) == SHA_NATIVE_ORDER) || \
								((INV) == SHA_INVERSE_ORDER))
/** @} */

/** @defgroup SHA_HMAC_MODE SHA HMAC Mode
  * @{
  */
#define SHA_HASH_MODE		(0x00)	/*!< Pure hash mode (no HMAC padding) */
#define SHA_HMAC_MODE		(0x01)	/*!< HMAC mode (i-pad and o-pad applied to key) */
/** @brief Check whether MODE is a valid SHA/HMAC operation mode. */
#define IS_SHA_HMAC_MODE(MODE) (((MODE) == SHA_HASH_MODE) || \
								((MODE) == SHA_HMAC_MODE))
/** @} */

/** @defgroup SHA_HMAC_KEY_SIZE SHA HMAC Key Size
  * @{
  */
#define SHA_HMAC_KEY_SIZE_128		(0x00)	/*!< Register encoding for 128-bit HMAC key */
#define SHA_HMAC_KEY_SIZE_192		(0x01)	/*!< Register encoding for 192-bit HMAC key */
#define SHA_HMAC_KEY_SIZE_256		(0x02)	/*!< Register encoding for 256-bit HMAC key */
#define SHA_HMAC_KEY_BIT_128        128		/*!< HMAC key length: 128 bits */
#define SHA_HMAC_KEY_BIT_192        192		/*!< HMAC key length: 192 bits */
#define SHA_HMAC_KEY_BIT_256        256		/*!< HMAC key length: 256 bits */
/** @brief Check whether SIZE is a valid HMAC key length in bits. */
#define IS_SHA_HMAC_KEY_BIT(SIZE) (((SIZE) == SHA_HMAC_KEY_BIT_128) || \
							 		((SIZE) == SHA_HMAC_KEY_BIT_192) || \
								    ((SIZE) == SHA_HMAC_KEY_BIT_256))
/** @} */

/** @defgroup SHA_BLOCK_SIZE SHA Block Size
  * @{
  */
#define SHA_BLOCK_256			64	/*!< Block size in bytes for SHA-224/256 (512-bit block = 64 bytes) */
#define SHA_BLOCK_512 			128	/*!< Block size in bytes for SHA-384/512 (1024-bit block = 128 bytes) */
/** @} */

/** @defgroup SHA_ILEN_BYTES SHA Input Length Bytes
  * @{
  */
#define SHA_ILEN_BYTES_256			8	/*!< Input length field size for SHA-224/256: 8 bytes (64-bit) */
#define SHA_ILEN_BYTES_512			16	/*!< Input length field size for SHA-384/512: 16 bytes (128-bit) */
/** @} */

/** @defgroup SHA_DIGEST_LEN SHA Digest Length
  * @{
  */
#define SHA_DIGEST_224			28	/*!< SHA-224 digest output length in bytes */
#define SHA_DIGEST_256			32	/*!< SHA-256 digest output length in bytes */
#define SHA_DIGEST_384			48	/*!< SHA-384 digest output length in bytes */
#define SHA_DIGEST_512			64	/*!< SHA-512 digest output length in bytes */
/** @} */

/** @defgroup SHA_HMAC_KEY_BYTE SHA HMAC Key Byte Length
  * @{
  */
#define SHA_HMAC_KEY_BYTE_128			16	/*!< HMAC key length: 16 bytes (128 bits) */
#define SHA_HMAC_KEY_BYTE_192			24	/*!< HMAC key length: 24 bytes (192 bits) */
#define SHA_HMAC_KEY_BYTE_256			32	/*!< HMAC key length: 32 bytes (256 bits) */
/** @} */

/** @defgroup SHA_TIMEOUT SHA Timeout
  * @{
  */
#define SHA_DMA_TIMEOUT			((u32) 1000000)		/*!< DMA mode operation timeout count */
#define SHA_MUTEX_TIMEOUT		((u32) 1000000)		/*!< Engine mutex acquisition timeout count */
#define SHA_SLAVE_TIMEOUT		((u32) 10000)		/*!< Slave mode operation timeout count */
/** @} */

/** @defgroup SHA_DMA SHA DMA
  * @{
  */
#define SHA_DMA_CH_NUM                          1		/*!< DMA channel number for SHA transfers */
#define SHA_DMA_CH_MASK                         ((u32)0x00000001 << SHA_DMA_CH_NUM)	/*!< DMA channel enable mask bit for SHA */
#define SHA_DMA_MAX_BLOCK_TS_256                0x1FFFFFC0	/*!< Max DMA block transfer size for SHA-224/256 (64-byte aligned) */
#define SHA_DMA_MAX_BLOCK_TS_512                0x1FFFFF80	/*!< Max DMA block transfer size for SHA-384/512 (128-byte aligned) */
#define SHA_DMA_DST_TR_WIDTH                    TrWidthOneByte	/*!< DMA destination transfer width: byte */
#define SHA_DMA_SRC_TR_WIDTH                    TrWidthOneByte	/*!< DMA source transfer width: byte */
/** @} */

/** @defgroup SHA_DMA_BEAT SHA DMA Beat
  * @{
  */
/** @brief Write a 64-bit value into byte buffer at offset i in big-endian order; used for SHA padding. */
#ifndef SHA_PUT_UINT64_BE
#define SHA_PUT_UINT64_BE(n,b,i)                            \
{														\
	(b)[(i)    ] = (unsigned char) ( (n) >> 56 );		\
	(b)[(i) + 1] = (unsigned char) ( (n) >> 48 );		\
	(b)[(i) + 2] = (unsigned char) ( (n) >> 40 );		\
	(b)[(i) + 3] = (unsigned char) ( (n) >> 32 );		\
	(b)[(i) + 4] = (unsigned char) ( (n) >> 24 );		\
	(b)[(i) + 5] = (unsigned char) ( (n) >> 16 );		\
	(b)[(i) + 6] = (unsigned char) ( (n) >>  8 );		\
	(b)[(i) + 7] = (unsigned char) ( (n)	   );		\
}
#endif

/** @brief Write a 32-bit value into byte buffer at offset i in big-endian order; used for SHA padding. */
#ifndef SHA_PUT_UINT32_BE
#define SHA_PUT_UINT32_BE(n,b,i)                            \
{														\
	(b)[(i)    ] = (unsigned char) ( (n) >> 24 );		\
	(b)[(i) + 1] = (unsigned char) ( (n) >> 16 );		\
	(b)[(i) + 2] = (unsigned char) ( (n) >>  8 );		\
	(b)[(i) + 3] = (unsigned char) ( (n)	   );		\
}
#endif
/** @} */

/** @} */

/* Exported functions --------------------------------------------------------*/
/** @defgroup SHA_Exported_Functions SHA Exported Functions
  * @{
  */


/** @defgroup SHA_HAL_Functions SHA HAL Functions
  * @{
  */


/**
 * @brief Initialize the SHA-2 context for read-only DMA mode computation.
 * @param  SHAtype
 *            @arg SHA_224
 *            @arg SHA_256
 *            @arg SHA_384
 *            @arg SHA_512
 * @param  ctx SHA context. This function will initialize the context.
 * @note The engine will be locked during the execution of this function,
 *       and will be unlocked after execution.
 *       Avoid multiple threads/CPUs using the engine at the same time.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_sha2_init(SHA_context *ctx, u32 SHAtype);

/**
  * @brief  Update SHA state.
  * @param 	ctx SHA context
  * @param  input Pointer to input data.
  * @param  dst Pointer to output data. If in read-only mode, set it to NULL
  * @param  len Input data length.
  * @note
  *         - If copy mode and read-only mode are mixed for update,
  *           then ensure that the message lengths of both copy mode and read-only mode are integer multiples of the SHA-BLOCK length.
  *           (64-byte alignment for SHA-224/256, 128-byte alignment for SHA-384/512)
  *         - The engine will be locked during the execution of this function,
  *           and will be unlocked after execution.
  *           Avoid multiple threads/CPUs using the engine at the same time.
  *         - In copy mode, the destination address and calculation length must be 32 bytes (cache line size) aligned.
  * @return Process status:
  *         - 0: success
  *         - Other: error code. Refer to @ref CRYPTO_Process_Status
  */
_LONG_CALL_ int crypto_sha2_update(SHA_context *ctx, const u8 *input, u8 *dst, size_t len);

/**
  * @brief  Perform SHA final process.
  * @param 	ctx SHA context
  * @param  output The result of SHA function
  * @note
  *         - Since the SHA engine does not support hardware padding, software padding is performed instead.
  *         - The engine will be locked during the execution of this function,
  *           and will be unlocked after execution.
  *           Avoid multiple threads/CPUs using the engine at the same time.
  * @return Process status:
  *         - 0: success
  *         - Other: error code. Refer to @ref CRYPTO_Process_Status
  */
_LONG_CALL_ int crypto_sha2_final(SHA_context *ctx, u8 *output);

/**
 * @brief  Set software key value for HMAC
 * @param  key_id Value of @ref KM_KEY
 * @param  key_len_bits
 *            @arg KEY_BIT_128
 *            @arg KEY_BIT_192
 *            @arg KEY_BIT_256
 * @param  key_addr Address to software key array
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_hmac_sha2_set_sw_key(u8 key_id, u32 key_len_bits, const u8 *key_addr);

/**
 * @brief  Control whether security keys can be shared with non-secure world.
 * @param  key_id Value of @ref KM_KEY
 * @param  is_share
 *      @arg 1: share
 *      @arg 0: not share (secure only)
 * @note If sharing is enabled, non-secure code can only trigger the use of the key, but cannot read or write.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_hmac_sha2_share_secure_key(const u8 key_id, const u8 is_share);

/**
 * @brief Initialize the HMAC-SHA2 context for read-only DMA mode computation.
 * @param  SHAtype
 *            @arg SHA_224
 *            @arg SHA_256
 *            @arg SHA_384
 *            @arg SHA_512
 * @param  ctx SHA context.
 * @param  key_id Value of @ref KM_KEY
 * @param  key_len_bits
 *            @arg KEY_BIT_128
 *            @arg KEY_BIT_192
 *            @arg KEY_BIT_256
 * @note The engine will be locked during the execution of this function,
 *       and will be unlocked after execution.
 *       Avoid multiple threads/CPUs using the engine at the same time.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_hmac_sha2_init(SHA_context *ctx, u32 SHAtype, u8 key_id, u32 key_len_bits);

/**
 * @brief  Update HMAC state.
 * @param  ctx SHA context.
 * @param  input Pointer to input data.
 * @param  dst Pointer to output data. If in read-only mode, set it to NULL
 * @param  len Input data length.
 * @note
 *         - If copy mode and read-only mode are mixed for update,
 *           then ensure that the message lengths of both copy mode and read-only mode are integer multiples of the SHA-BLOCK length.
 *           (64-byte alignment for SHA-224/256, 128-byte alignment for SHA-384/512)
 *         - The HMAC key will be reloaded before each update. If a software key is used,
 *           and the key is modified between updates, the calculation result will be wrong.
 *           Ensure the software key is set to the correct value before calling update.
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the engine at the same time.
 *         - In copy mode, the destination address and calculation length must be 32 bytes (cache line size) aligned.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_hmac_sha2_update(SHA_context *ctx, const u8 *input, u8 *dst, size_t len);

/**
 * @brief  Perform HMAC final process.
 * @param  ctx SHA context.
 * @param  output The result of HMAC function
 * @note
 *         - The HMAC key will be reloaded before the final process.
 *           If a software key is used and the key is modified in final, the calculation result will be wrong.
 *           Ensure the software key is set to the correct value before calling the final process.
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the engine at the same time.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_hmac_sha2_final(SHA_context *ctx, u8 *output);
/** @} */
/** @} */

/** @defgroup SHA_Normal_Functions SHA Normal Functions
  * @{
  */

/**
 * @brief Return different SHA/HMAC engine register addresses according to the security state
 * @return SHA_REG_BASE or SHA_REG_BASE_S
 */
_LONG_CALL_ SHA_TypeDef *SHA2_Get_SHA_Addr(void);

/**
 * @brief Return different SHA key management register addresses according to the security state
 * @return SHA_KEY_REG_BASE or SHA_KEY_REG_BASE_S
 */
_LONG_CALL_ KEY_MANAGEMENT_HMAC_TypeDef *SHA2_get_km_hmac_addr(void);

/**
 * @brief Return different crypto engine DMA register addresses according to the security state
 * @return AES_SHA_DMA_REG_BASE or AES_SHA_DMA_REG_BASE_S
 */
_LONG_CALL_ GDMA_TypeDef *SHA2_Get_DMA_Addr(void);

/**
 * @brief Restore SHA/HMAC engine registers from context
 * @param  ctx Pointer to @ref SHA_context structure
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int SHA2_Restore(SHA_context *ctx);

/**
 * @brief Save SHA/HMAC engine registers to context
 * @param  ctx Pointer to @ref SHA_context structure
 */
_LONG_CALL_ void SHA2_Save(SHA_context *ctx);

/**
  * @brief  Get SHA final digest result.
  * @param 	ctx SHA context
  * @param 	out Final digest buffer
  * @return Process status:
  *         - 0: success
  *         - Other: error code. Refer to @ref CRYPTO_Process_Status
  */
_LONG_CALL_ int SHA2_Get_Final_Digest(SHA_context *ctx, u8 *out);

/**
  * @brief  Fill each @ref SHA_InitTypeDef member with its default value.
  * @param  SHA_InitStruct Pointer to a @ref SHA_InitTypeDef structure which will be initialized.
  */
_LONG_CALL_ void SHA2_StructInit(SHA_InitTypeDef *SHA_InitStruct);

/**
  * @brief  Initialize the SHA peripheral according to the specified
  *              parameters in the SHA_InitStruct.
  * @param  SHA_InitStruct Pointer to a @ref SHA_InitTypeDef structure that contains
  *              the configuration information for SHA peripheral.
  * @param  ctx SHA context
  * @return Process status:
  *         - 0: success
  *         - Other: error code. Refer to @ref CRYPTO_Process_Status
  */
_LONG_CALL_ int SHA2_Init(SHA_InitTypeDef *SHA_InitStruct, SHA_context *ctx);

/**
  * @brief  Initialize the SHA GDMA
  * @param  ctx SHA context
  */
_LONG_CALL_ void SHA2_DMA_Init(SHA_context *ctx);

/**
  * @brief  Process SHA data in slave mode.
  * @param  input Pointer to input data.
  * @param  input_len The length of input data.
  * @param  end_input Flag indicating this is the last input block.
  * @note If the input length is less than a block size, the hardware will perform automatic padding.
  * @return Process status:
  *         - 0: success
  *         - Other: error code. Refer to @ref CRYPTO_Process_Status
  */
_LONG_CALL_ int SHA2_ProcessSlave(const u8 *input, u32 input_len, u8 end_input);

/**
  * @brief  Process SHA input data in DMA mode. SHA DMA mode must be initialized with SHA2_DMA_Init() before calling this function.
  * @param 	ctx SHA context
  * @param  input Pointer to input data.
  * @param  dst Pointer to output data. If in read-only mode, set it to NULL
  * @param  bodylen The byte number of input data.
  * @return Process status:
  *         - 0: success
  *         - Other: error code. Refer to @ref CRYPTO_Process_Status
  */
_LONG_CALL_ int SHA2_ProcessDma(SHA_context *ctx, const u8 *input, u8 *dst, u32 bodylen);

/**
 * @brief Lock the SHA engine mutex before accessing registers.
 *        Without locking, engine registers cannot be accessed.
 *        This driver calls this API internally. Do not call this API from application code.
 */
_LONG_CALL_ void SHA2_lock_mutex(void);

/**
 * @brief Release the SHA engine mutex after use.
 *        This driver calls this API internally. Do not call this API from application code.
 */
_LONG_CALL_ void SHA2_unlock_mutex(void);
/** @} */

/**
 * @brief  Set key byte swap and payload byte swap.
 * @param  new_status Value of SHA_BIT_KEY_BYTE_INVERSION_ENABLE and SHA_BIT_PAYLOAD_BYTE_INVERSION_ENABLE
 * @note This bit needs to be configured only in special cases. Users generally do not need to call this API.
 * @return Old byte swap setting
 */
_LONG_CALL_ int SHA2_set_swap_setting(u32 new_status);

/**
 * @brief  Get current byte swap setting
 * @return Current byte swap setting. SHA_MASK_BYTE_INVERSION bits
 */
_LONG_CALL_ int SHA2_get_swap_setting(void);

/** @} */
/** @} */
#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */


#endif