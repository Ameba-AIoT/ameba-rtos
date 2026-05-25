/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SHA_H_
#define _AMEBA_SHA_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup SHA
  * @brief SHA driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup SHA_Register_Definitions SHA Register Definitions
  * @{
  */

/** @defgroup SHA_CONTROL
  * @brief SHA2 engine control register
  * @{
  */
#define SHA_BIT_SOFT_RESET_TRIGGER_OR_SOFT_RESET_BUSY ((u32)0x00000001 << 24)          /*!< R/W1T 0x0  Write 0x1 to trigger a soft reset. All registers will be reset except mutex register. Keep reading this bit field until the result is 0x0 to make sure the soft reset complete.*/
#define SHA_BIT_DIGEST_ADDR_SWAP_ENABLE               ((u32)0x00000001 << 16)          /*!< R/W 0x0  Indicated address mapping of SHA_Digest0~15, SHA_Digest_alias256_0~7 and LMOTS_sig_alias256_0~7 are in 0x0 : Native address map 0x1 : Inverse address map. Example :  Name             Native address     Inverse address SHA_Digest0     0x10                 0x4C SHA_Digest1     0x14                 0x48 ... SHA_Digest15    0x4C               0x10*/
#define SHA_BIT_DIGEST_BYTE_SWAP_ENABLE               ((u32)0x00000001 << 15)          /*!< R/W 0x0  Indicated data structure of SHA_Digest0~15, SHA_Digest_alias256_0~7 and LMOTS_sig_alias256_0~7 are in  0x0 : Big-endian 0x1 : Little-endian Example :  Memory               byte3   byte2   byte1   byte0 ADDR 0x4             0x7a    0xaa    0x21    0x22 ADDR 0x0             0xee    0xfe     0x97    0xad If write data of user is 0xeefe97ad & 0x7aaa2122,  the byte_swap_enable is 0x0 = The input to the SHA2 engine will be 0xad97feee & 0x2221aa7a. 0x1 = The input to the SHA2 engine will be 0xeefe97ad & 0x7aaa2122.*/
#define SHA_BIT_FIFO_FULL_SUSPEND                     ((u32)0x00000001 << 14)          /*!< R/W 0x1  The register is available only when Slave_mode = 0x1 If user writes data to SHA_FIFO_in but the FIFO space is full 0x0 : accept signal is always asserted 0x1 : accept signal is not asserted until FIFO space is available*/
#define SHA_BIT_KEY_BYTE_SWAP_ENABLE                  ((u32)0x00000001 << 13)          /*!< R/W 0x1  The data structure of key is in 0x0 : Big-endian 0x1 : Little-endian Note : The example of byte order is same as example in digest_byte_swap_enable register.*/
#define SHA_BIT_PAYLOAD_BYTE_SWAP_ENABLE              ((u32)0x00000001 << 12)          /*!< R/W 0x1  The data structure of received message  0x0 : use native byte order in memory 0x1 : use inverse byte order Example :  Memory               byte3   byte2  byte1  byte0 ADDR 0x4             0x7a   0xaa    0x21   0x22 ADDR 0x0             0xee   0xfe   0x97    0xad If input data is 64-bit, byte_inversion_enable is 0x1 = The input to the SHA2 engine will be 0xad97feee2221aa7a. 0x0 = The input to the SHA2 engine will be 0x7aaa2122eefe97ad. If input data is 48-bit, byte_inversion_enable is 0x1 = The input to the SHA2 engine will be 0xad97feee2221. 0x0 = The input to the SHA2 engine will be 0x2122eefe97ad.*/
#define SHA_BIT_TSHA512_USE_256                       ((u32)0x00000001 << 11)          /*!< R/W 0x0  If the engine is in truncated SHA-512 mode, decide the truncated SHA-512 is in 0x0 : SHA-512/224 mode 0x1 : SHA-512/256 mode Note :  The register is available only when config SUPPORT_TRUNCATED_SHA512 The register is available only when SHA512_truncate_en= 0x1 (truncated SHA-512)*/
#define SHA_BIT_SHA512_TRUNCATE_EN                    ((u32)0x00000001 << 10)          /*!< R/W 0x0  If the engine is in SHA-512 mode, decide the SHA-512 is in 0x0 : normal SHA-512 0x1 : truncated SHA-512 Note :  The register is available only when config SUPPORT_TRUNCATED_SHA512 The register is available only when SHA_mode = 0x3 (SHA-512)*/
#define SHA_MASK_KEY_SIZE                             ((u32)0x00000003 << 8)           /*!< R/W 0x0  Decide size of key for HMAC (or LMOTS) process 0x0 : 128 bits 0x1 : 192 bits 0x2 : 256 bits 0x3 : 512 bits (only available when SUPPORT_MAXIMUM_KEY_EN=1) Note :  The register is available only when SUPPORT_HMAC_MODE (or SUPPORT_LMS_MODE)  is configured In LMS mode, please set this register to 0x2 (LMS only support key with 256 bits).*/
#define SHA_KEY_SIZE(x)                               (((u32)((x) & 0x00000003) << 8))
#define SHA_GET_KEY_SIZE(x)                           ((u32)(((x >> 8) & 0x00000003)))
#define SHA_BIT_SM3_MODE                              ((u32)0x00000001 << 7)           /*!< R/W 0x0  If the register is 0x1, the engine is working in SM3 mode. (Priority: SM3>SHA1>SHA2) Note :  The register is always 0x1 if SHA1_CORE and SHA2_CORE are disabled The register is always 0x0 if SM3_CORE is disabled*/
#define SHA_BIT_SLAVE_MODE                            ((u32)0x00000001 << 6)           /*!< R/W 0x0  Decide message data is received by 0 : DMA mode. 1 : slave mode. SW can push data by wrting FIFO Data Register in slave mode.*/
#define SHA_BIT_ICG_ENABLE                            ((u32)0x00000001 << 5)           /*!< R/W 0x1  Enable auto clock gating*/
#define SHA_BIT_HMAC_MODE                             ((u32)0x00000001 << 4)           /*!< R/W 0x0  Decide the SHA2 engine is in 0x0 : hash mode. 0x1 : HMAC mode. Note : The register is available only when the SUPPORT_HMAC_MODE is configured*/
#define SHA_BIT_SHA1_MODE                             ((u32)0x00000001 << 3)           /*!< W/R 0x0  If SM3_mode is disabled and the register is 0x1, the engine is working in SHA1 mode. (Priority: SM3>SHA1>SHA2) Note : The register is always 0x1 if SM3_CORE and SHA2_CORE are disabled The register is always 0x0 if SHA1_CORE is disabled*/
#define SHA_BIT_DMA_MODE                              ((u32)0x00000001 << 2)           /*!< R/W 0x0  Decide the DMA of SHA2 engine is in 0x0 : read-only mode. 0x1 : copy mode*/
#define SHA_MASK_SHA2_MODE                            ((u32)0x00000003 << 0)           /*!< W/R 0x2  Decide hash algorithm of SHA2 engine is 0x0 : SHA-224 0x1 : SHA-256 0x2 : SHA-384 0x3 : SHA-512*/
#define SHA_SHA2_MODE(x)                              (((u32)((x) & 0x00000003) << 0))
#define SHA_GET_SHA2_MODE(x)                          ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup SHA_STATUS
  * @brief SHA2 engine status register
  * @{
  */
#define SHA_BIT_ENGINE_BUSY   ((u32)0x00000001 << 28)          /*!< RO 0x0  1: engine busy*/
#define SHA_MASK_FIFO_SPACE   ((u32)0x0000001F << 0)           /*!< RO 0x10  Number of available FIFO space in byte*/
#define SHA_FIFO_SPACE(x)     (((u32)((x) & 0x0000001F) << 0))
#define SHA_GET_FIFO_SPACE(x) ((u32)(((x >> 0) & 0x0000001F)))
/** @} */

/** @defgroup SHA_BLOCK_CONTROL
  * @brief SHA2 engine block control register
  * @{
  */
#define SHA_BIT_END_SLAVE_INPUT ((u32)0x00000001 << 2) /*!< R/W 0x0  If hash process is in slave mode and sending all input data has been completed, set this bit to 1 to trigger final hash (padding) process. Note : The register is avaliable when engine is in slave mode Automatically cleared by hardware.*/
#define SHA_BIT_LAST_BLOCK      ((u32)0x00000001 << 1) /*!< R/W 0x0  Indicates this operation is the last block. When the eingine is in SHA2 mode, the padding step will be executed at the end. HMAC mode, the o_pad step will be executed at the end. Note : Need to be cleared by software.*/
#define SHA_BIT_FIRST_BLOCK     ((u32)0x00000001 << 0) /*!< R/W 0x0  Indicates this operation is the first block. When the eingine is in SHA2 mode, the digest will be initialized automatically. HMAC mode, the i_pad step will be executed first. Note : Automatically cleared by hardware.*/
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
#define SHA_MASK_MUTEX_STATUS   ((u32)0x00000003 << 0)           /*!< RS/W0C 0x0  Behavior of read If the read data is  0x0 : the mutex is idle 0x1 : secure domain lock mutex 0x2 : non-secure domain lock mutex Behavior of write 0x0 : unlock mutex Mutex is a mechanism to prevent two domains from R/W to the same engine at the same time. Notice for user : 1. Any domain acess is invalid when the initial status is 0x0. 2. Any domain wants to use the engine needs to read This first. 3. If read 0x0 means the engine is idle, the engine will automatically lock on the corresponding mutex status. 4. Please write 0x0 to unlock the mutex status after using SHA2 engine.*/
#define SHA_MUTEX_STATUS(x)     (((u32)((x) & 0x00000003) << 0))
#define SHA_GET_MUTEX_STATUS(x) ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup SHA_MUTEX_OVERWRITE
  * @brief Mutex overwrite register
  * @{
  */
#define SHA_BIT_MUTEX_OVERWRITE ((u32)0x00000001 << 0) /*!< R/W 0x0  Allowing secure domain can overwrite current mutex is 0x1 : enable 0x0 : disable Note: The register is available only for secure master Overwrite Flow of MUTEX: 1. Check If DMA is working. Read the DMA.ChEnReg(0x3a0) to get the DMA status. If it is not working, skip to step 2.     a. Write DMA.CFGx.CH_SUSP (bit 8) to cancel task.     b. Polling DMA.CFGx.INACTIVE (bit0) to determine if the DMA has ended the task.     c. Clear DMA.ChEnReg to disable the channel and clear the task status.       * The corresponding CH_EN_WE must be set to 1.     d. Clear the DMA interrupt in case the DMA task is completed in the above step. 2. Set the Mutex_overwrite in the secure domain. 3. Read the Mutex_status in the secure domain to overwrite mutex. 4. Polling SHA2-CTRL.soft_reset_busy (bit 24) is 0 to confirm the completion of the soft reset process. 5. Resetting the engine and DMA configuration registers. 6. Enable DMA task.*/
/** @} */

/** @defgroup SHA_CONFIGURATION
  * @brief Configuration information register
  * @{
  */
#define SHA_BIT_DISABLE_CORE_SM3         ((u32)0x00000001 << 14) /*!< RO   1: SM3 is disabled by hardware 0: SM3 is enabled*/
#define SHA_BIT_DISABLE_CORE_SHA1        ((u32)0x00000001 << 13) /*!< RO   1: SHA1 is disabled by hardware 0: SHA1 is enabled*/
#define SHA_BIT_DISABLE_CORE_SHA2        ((u32)0x00000001 << 12) /*!< RO   1: SHA2 is disabled by hardware 0: SHA2 is enabled*/
#define SHA_BIT_SUPPORT_LMS_MODE         ((u32)0x00000001 << 10) /*!< RO   0: disabled; 1: enabled*/
#define SHA_BIT_SM3_CORE_EN              ((u32)0x00000001 << 9)  /*!< RO   0: disabled; 1: enabled*/
#define SHA_BIT_SHA1_CORE_EN             ((u32)0x00000001 << 8)  /*!< RO   0: disabled; 1: enabled*/
#define SHA_BIT_TRUST_ZONE_EN            ((u32)0x00000001 << 7)  /*!< RO   0: disabled; 1: enabled*/
#define SHA_BIT_KEY_MANAGER_EN           ((u32)0x00000001 << 6)  /*!< RO   0: disabled; 1: enabled*/
#define SHA_BIT_EXTERNAL_KEY_EN          ((u32)0x00000001 << 5)  /*!< RO   0: disabled; 1: enabled*/
#define SHA_BIT_SUPPORT_COPY_MODE        ((u32)0x00000001 << 4)  /*!< RO   0: disabled; 1: enabled*/
#define SHA_BIT_SUPPORT_HMAC_MODE        ((u32)0x00000001 << 3)  /*!< RO   0: disabled; 1: enabled*/
#define SHA_BIT_SUPPORT_TRUNCATED_SHA512 ((u32)0x00000001 << 2)  /*!< RO   0: disabled; 1: enabled*/
#define SHA_BIT_SUPPORT_SHA512           ((u32)0x00000001 << 1)  /*!< RO   0: disabled; 1: enabled*/
#define SHA_BIT_SHA2_CORE_EN             ((u32)0x00000001 << 0)  /*!< RO   0: disabled; 1: enabled*/
/** @} */

/** @defgroup SHA_REVISION
  * @brief Revision register
  * @{
  */
#define SHA_MASK_EDITION_YEAR    ((u32)0x000000FF << 16)           /*!< RO 0x25  Edition year of SHA2*/
#define SHA_EDITION_YEAR(x)      (((u32)((x) & 0x000000FF) << 16))
#define SHA_GET_EDITION_YEAR(x)  ((u32)(((x >> 16) & 0x000000FF)))
#define SHA_MASK_MAJOR_VERSION   ((u32)0x000000FF << 8)            /*!< RO 0x0  Major version of SHA2*/
#define SHA_MAJOR_VERSION(x)     (((u32)((x) & 0x000000FF) << 8))
#define SHA_GET_MAJOR_VERSION(x) ((u32)(((x >> 8) & 0x000000FF)))
#define SHA_MASK_MINOR_VERSION   ((u32)0x000000FF << 0)            /*!< RO 0x0  Minor version of SHA2*/
#define SHA_MINOR_VERSION(x)     (((u32)((x) & 0x000000FF) << 0))
#define SHA_GET_MINOR_VERSION(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup SHA_SVN_NUMBER
  * @brief SVN register
  * @{
  */
#define SHA_MASK_SVN_NUMBER   ((u32)0xFFFFFFFF << 0)           /*!< RO   SVN number of SHA2*/
#define SHA_SVN_NUMBER(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SHA_GET_SVN_NUMBER(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SHA_GENERATE_DATE
  * @brief Generated date register
  * @{
  */
#define SHA_MASK_YEAR    ((u32)0x0000FFFF << 16)           /*!< RO -  generated year*/
#define SHA_YEAR(x)      (((u32)((x) & 0x0000FFFF) << 16))
#define SHA_GET_YEAR(x)  ((u32)(((x >> 16) & 0x0000FFFF)))
#define SHA_MASK_MONTH   ((u32)0x000000FF << 8)            /*!< RO -  generated month*/
#define SHA_MONTH(x)     (((u32)((x) & 0x000000FF) << 8))
#define SHA_GET_MONTH(x) ((u32)(((x >> 8) & 0x000000FF)))
#define SHA_MASK_DATE    ((u32)0x0000FFFF << 0)            /*!< RO -  generated date*/
#define SHA_DATE(x)      (((u32)((x) & 0x0000FFFF) << 0))
#define SHA_GET_DATE(x)  ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup SHA_Exported_Types SHA Exported Types
  * @{
  */

/** @brief SHA Register Declaration
  */

typedef struct {
	__IO uint32_t SHA_CONTROL            ;  /*!< SHA2 ENGINE CONTROL REGISTER,  Address offset:0x000 */
	__I  uint32_t SHA_STATUS             ;  /*!< SHA2 ENGINE STATUS REGISTER,  Address offset:0x004 */
	__IO uint32_t SHA_BLOCK_CONTROL      ;  /*!< SHA2 ENGINE BLOCK CONTROL REGISTER,  Address offset:0x008 */
	__IO uint32_t RSVD0                  ;  /*!< Reserved,  Address offset:0x00C-0x00F */
	__IO uint32_t SHA_DIGESTx[16]        ;  /*!< HASH VALUE x Register,  Address offset:0x010-0x04C */
	__O  uint32_t SHA_FIFO_IN            ;  /*!< FIFO INPUT DATA REGISTER FOR SLAVE MODE Register,  Address offset:0x050 */
	__IO uint32_t RSVD1[11]              ;  /*!< Reserved,  Address offset:0x054-0x07F */
	__IO uint32_t SHA_HASH_INPUT_LEN_x[4];  /*!< TOTAL INPUT BYTE LENGTH REGISTER x Register,  Address offset:0x080-0x08C */
	__IO uint32_t RSVD2[36]              ;  /*!< Reserved,  Address offset:0x090-0x11F */
	__IO uint32_t SHA_DMA_HS_CFG         ;  /*!< DMA HARDWARE HANDSHAKE CONFIGURATION REGISTER,  Address offset:0x120 */
	__IO uint32_t SHA_DMA_HS_BLOCK_TS    ;  /*!< DMA HARDWARE HANDSHAKE BLOCK TRANSFER SIZE REGISTER,  Address offset:0x124 */
	__IO uint32_t SHA_DMA_HS_BUFF_BYTE   ;  /*!< DMA HARDWARE HANDSHAKE BUFFER DATA BYTE OF ENGINE REGISTER,  Address offset:0x128 */
	__IO uint32_t SHA_DMA_HS_EN          ;  /*!< DMA HARDWARE HANDSHAKE ENABLE REGISTER,  Address offset:0x12C */
	__IO uint32_t RSVD3[756]             ;  /*!< Reserved,  Address offset:0x130-0xCFF */
	__IO uint32_t SHA_MUTEX_STATUS       ;  /*!< MUTEX STATUS REGISTER,  Address offset:0xD00 */
	__IO uint32_t SHA_MUTEX_OVERWRITE    ;  /*!< MUTEX OVERWRITE REGISTER,  Address offset:0xD04 */
	__IO uint32_t RSVD4[186]             ;  /*!< Reserved,  Address offset:0xD08-0xFEF */
	__I  uint32_t SHA_CONFIGURATION      ;  /*!< CONFIGURATION INFORMATION REGISTER,  Address offset:0xFF0 */
	__I  uint32_t SHA_REVISION           ;  /*!< REVISION REGISTER,  Address offset:0xFF4 */
	__I  uint32_t SHA_SVN_NUMBER         ;  /*!< SVN REGISTER,  Address offset:0xFF8 */
	__I  uint32_t SHA_GENERATE_DATE      ;  /*!< GENERATED DATE REGISTER,  Address offset:0xFFC */
} SHA_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


/* Exported constants --------------------------------------------------------*/
/** @defgroup Hash_Engine_Mode
  * @{
  */
typedef enum {
	HASH_SHA2_MODE = 0,
	HASH_SM3_MODE = 1
} hash_engine_mode;
/** @} */

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
	u32 DmaMode;     /*!< Specifies SHA DMA mode. This parameter can be a value of @ref SHA_Dma_Mode. */
	u32 HmacMode;    /*!< Specifies the HMAC mode. This parameter can be a value of @ref SHA_BIT_HMAC_MODE. */
} SHA_InitTypeDef;

/**
  * @brief SHA Context Structure Definition
  */
typedef struct {
	u32 sha_mode;       /*!< specifies the SHA mode.This parameter can be a value of @ref SHA_Mode. */
	u32 hmac_mode;      /*!< hmac opration mode. 0x0: use hash mode. 0x1: use hmac mode.*/
	u32 block_size;		/*!< block size. This parameter can be a value of @ref SHA_BLOCK_SIZE. */
	u32 ilen_bytes;		/*!< bytes occupied by input length storage. */
	u32 digest_len;		/*!< digest length. */
	u32 state[16];      /*!< digest state  */
	u8 buffer[128] __attribute__((aligned(32)));   	/*!< data block being processed */
	u32 buf_used_bytes;	/*!< buf used bytes */
	u32 total_len;	    /*!< The total number of data bytes will be processed */
	u8 dma_copy;        /*!< DMA opration mode. 0x0: read-only mode. 0x1: copy mode*/
	u8 seq_hash_first;
	u8 key_id;
	u32 key_len_bits;
	u32 DMA_CTL_LOW;
} SHA_context;
/** @} */

/* Exported constants --------------------------------------------------------*/
/** @defgroup SHA_Exported_Constants SHA Exported Constants
  * @{
  */

/** @defgroup SHA_Mode
  * @{
  */
#define SHA_224			((u8)0x00)
#define SHA_256			((u8)0x01)
#define SHA_384			((u8)0x02)
#define SHA_512			((u8)0x03)

#define IS_SHA_MODE(MODE) (((MODE) == SHA_224) || \
								((MODE) == SHA_256) || \
								((MODE) == SHA_384) || \
								((MODE) == SHA_512))

/* SM3 mode flag - used to distinguish SM3 from SHA2 in context */
#define SHA_MODE_SM3_FLAG		((u8)0x80)
#define IS_SM3_MODE(MODE) (((MODE) & SHA_MODE_SM3_FLAG) != 0)
/** @} */

/** @defgroup SHA_Byte_Swap_Defaults
  * @{
  */
#define SHA_MASK_BYTE_INVERSION         ((u32)0x00000003 << 12)
#define SHA_BYTE_SWAP_DEFAULTS          ((u32)(~SHA_BIT_KEY_BYTE_SWAP_ENABLE) & SHA_BIT_PAYLOAD_BYTE_SWAP_ENABLE)
#define SHA_BYTE_SWAP_KEY_SWAP          ((u32)  SHA_BIT_KEY_BYTE_SWAP_ENABLE  | SHA_BIT_PAYLOAD_BYTE_SWAP_ENABLE)
/** @} */

/** @defgroup SHA_Engine_Mode
  * @{
  */
#define SHA_DMA_MODE			((u8)0x00)
#define SHA_SLAVE_MODE			((u8)0x01)
#define IS_SHA_ENGINE_MODE(MODE) (((MODE) == SHA_DMA_MODE) || \
								((MODE) == SHA_SLAVE_MODE))
/** @} */

/** @defgroup SHA_DMA_Mode
  * @{
  */
#define SHA_READ_MODE		    ((u8)0x00)
#define SHA_COPY_MODE		    ((u8)0x01)
#define SHA_COPY_TO_READ_SWITCH ((u8)0x01)
#define SHA_READ_TO_COPY_SWITCH ((u8)0x02)
#define IS_SHA_DMA_MODE(MODE) (((MODE) == SHA_READ_MODE) || \
								((MODE) == SHA_COPY_MODE))
/** @} */

/** @defgroup SHA_Byte_Inv
  * @{
  */
#define SHA_NATIVE_ORDER		((u8)0x00)
#define SHA_INVERSE_ORDER		((u8)0x01)
#define IS_SHA_BYTE_INV(INV) (((INV) == SHA_NATIVE_ORDER) || \
								((INV) == SHA_INVERSE_ORDER))
/** @} */

/** @defgroup SHA_HMAC_MODE
  * @{
  */
#define SHA_HASH_MODE		(0x00)
#define SHA_HMAC_MODE		(0x01)
#define IS_SHA_HMAC_MODE(MODE) (((MODE) == SHA_HASH_MODE) || \
								((MODE) == SHA_HMAC_MODE))
/** @} */

/** @defgroup SHA_KEY_SIZE
  * @{
  */
#define SHA_HMAC_KEY_SIZE_128		(0x00)
#define SHA_HMAC_KEY_SIZE_192		(0x01)
#define SHA_HMAC_KEY_SIZE_256		(0x02)
#define SHA_HMAC_KEY_BIT_128        128
#define SHA_HMAC_KEY_BIT_192        192
#define SHA_HMAC_KEY_BIT_256        256
#define IS_SHA_HMAC_KEY_BIT(SIZE) (((SIZE) == SHA_HMAC_KEY_BIT_128) || \
							 		((SIZE) == SHA_HMAC_KEY_BIT_192) || \
								    ((SIZE) == SHA_HMAC_KEY_BIT_256))
/** @} */

/** @defgroup SHA_BLOCK_SIZE
  * @{
  */
#define SHA_BLOCK_256			64
#define SHA_BLOCK_512 			128
/** @} */

/** @defgroup SHA_ILEN_BYTES
  * @{
  */
#define SHA_ILEN_BYTES_256			8
#define SHA_ILEN_BYTES_512			16
/** @} */

/** @defgroup SHA_DIGEST_LEN
  * @{
  */
#define SHA_DIGEST_224			28
#define SHA_DIGEST_256			32
#define SHA_DIGEST_384			48
#define SHA_DIGEST_512			64
/** @} */

/** @defgroup SHA_DIGEST_LEN
  * @{
  */
#define SHA_HMAC_KEY_BYTE_128			16
#define SHA_HMAC_KEY_BYTE_192			24
#define SHA_HMAC_KEY_BYTE_256			32
/** @} */

/** @defgroup SHA_TIMEOUT
  * @{
  */
#define SHA_DMA_TIMEOUT				((u32) 1000000)
#define SHA_MUTEX_TIMEOUT			((u32) 1000000)
#define SHA_SLAVE_TIMEOUT			((u32) 10000)
/** @} */

/** @defgroup SHA_DMA
  * @{
  */
#define SHA_DMA_CH_NUM                          1
#define SHA_DMA_CH_MASK                         ((u32)0x00000001 << SHA_DMA_CH_NUM)
#define SHA_DMA_MAX_BLOCK_TS_256                0x1FFFFFC0
#define SHA_DMA_MAX_BLOCK_TS_512                0x1FFFFF80
#define SHA_DMA_DST_TR_WIDTH                    TrWidthOneByte
#define SHA_DMA_SRC_TR_WIDTH                    TrWidthOneByte
/** @} */

#define HASH_INIT_MASK (SHA_BIT_SLAVE_MODE | SHA_BIT_DMA_MODE | SHA_BIT_HMAC_MODE | SHA_MASK_SHA2_MODE | SHA_BIT_SM3_MODE)

/** @defgroup SHA_DMA_BEAT
  * @{
  */
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

/** @defgroup SHA_Normal_Functions SHA Normal Functions
  * @{
  */

/**
 * @brief Return different SHA/HMCA engine register addresses according to the state of secure
 * @retval SHA_REG_BASE or SHA_REG_BASE_S
 */
_LONG_CALL_ SHA_TypeDef *SHA2_Get_SHA_Addr(void);

/**
 * @brief Return different crypto engine DMA register addresses according to the state of secure
 * @retval AES_SHA_DMA_REG_BASE or AES_SHA_DMA_REG_BASE_S
 */
_LONG_CALL_ GDMA_TypeDef *SHA2_Get_DMA_Addr(void);

/**
 * @brief Restore HMAC engine registers from context
 * @param  ctx: pointer to SHA_context structure
 * @retval	None
 */
_LONG_CALL_ int SHA2_Restore(SHA_context *ctx);

/**
 * @brief Save HMAC engine registers to context
 * @param  ctx: pointer to SHA_context structure
 * @retval	None
 */
_LONG_CALL_ void SHA2_Save(SHA_context *ctx);

/**
  * @brief  SHA get final digest result.
  * @param 	ctx: SHA context
  * @param 	out: final digest buffer
  * @retval	None
  */
_LONG_CALL_ int SHA2_Get_Final_Digest(SHA_context *ctx, u8 *out);

/**
  * @brief  Fill each SHA_InitTypeDef member with its default value.
  * @param  SHA_InitStruct: pointer to a SHA_InitTypeDef structure which will be initialized.
  * @retval None
  */
_LONG_CALL_ void SHA2_StructInit(SHA_InitTypeDef *SHA_InitStruct);

/**
  * @brief  Initialize the SHA peripheral according to the specified
  *              parameters in the SHA_InitStruct.
  * @param  SHA_InitStruct: pointer to a SHA_InitTypeDef structure that contains
  *              the configuration information for SHA peripheral.
  * @param  ctx: SHA context
  * @retval None
  */
_LONG_CALL_ int SHA2_Init(SHA_InitTypeDef *SHA_InitStruct, SHA_context *ctx);

/**
  * @brief  Initialize the SHA GDMA
  * @param  ctx: SHA context
  * @retval None
  */
_LONG_CALL_ void SHA2_DMA_Init(SHA_context *ctx);

/**
  * @brief  SHA engine process in slave mode.
  * @param 	ctx: SHA context
  * @param  input: Point to input data.
  * @note If input len is less than a block size, then hardware will auto padding.
  * @return Result of process.
  */
_LONG_CALL_ int SHA2_ProcessSlave(const u8 *input, u32 input_len, u8 end_input);

/**
  * @brief  SHA engine process in DMA mode. Note that SHA-DMA must set
  * @param 	ctx: SHA context
  * @param  input: Point to input data.
  * @param  dst: Point to output data. If in read-only mode, set it to NULL
  * @param  bodylen: The byte number of input data.
  * @return Result of process.
  */
_LONG_CALL_ int SHA2_ProcessDma(SHA_context *ctx, const u8 *input, u8 *dst, u32 bodylen);

/**
 * @brief Before accessing the Crypto engine, it is necessary to read the mutex register once.
 *        Otherwise, any registers of the engine cannot be operated.
 *        This driver has called this API internally, so please do not reuse this API.
 * @retval None
 */
_LONG_CALL_ void SHA2_lock_mutex(void);

/**
 * @brief The occupation needs to be released after the use of the Crypto engine is over.
 *        This driver has called this API internally, so please do not reuse this API.
 * @retval None
 */
_LONG_CALL_ void SHA2_unlock_mutex(void);
/** @} */

/** @defgroup SHA_HAL_Functions SHA HAL Functions
  * @{
  */

/**
 * @brief  Set key byte swap and pay_load byte swap.
 * @param  new_status: value of SHA_BIT_KEY_BYTE_SWAP_ENABLE and SHA_BIT_PAYLOAD_BYTE_SWAP_ENABLE
 * @note This bit needs to be configured only in special cases. Users generally do not need to call this API.
 * @return Old byte swap setting
 */
_LONG_CALL_ int SHA2_set_swap_setting(u32 new_status);

/**
 * @brief  Get current byte swap setting
 * @return Current byte swap setting. SHA_MASK_BYTE_INVERSION bits
 */
_LONG_CALL_ int SHA2_get_swap_setting(void);

/**
 * @brief SHA init to read-only mode
 * @param  SHAtype:
 *            @arg SHA_224
 *            @arg SHA_256
 *            @arg SHA_384
 *            @arg SHA_512
 * @param  ctx: SHA context. This function will init ctx.
 * @note The engine will be locked during the execution of this function,
 *       and will be unlocked after execution.
 *       Avoid multiple threads/CPUs using engine at the same time.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_sha2_init(SHA_context *ctx, u32 SHAtype);

/**
  * @brief  SHA update.
  * @param 	ctx: SHA context
  * @param  input: Pointer to input data.
  * @param  dst: Pointer to output data. If in read-only mode, set it to NULL
  * @param  len: Input data length.
  * @note 1.If copy mode and read-only mode are mixed for update,
  *         then ensure that the message lengths of both copy mode and read-only mode are integer multiples of the SHA-BLOCK length.
  *         (64-byte alignment for SHA-224/256, 128-byte alignment for SHA-384/512)
  * @note 2.The engine will be locked during the execution of this function,
  *         and will be unlocked after execution.
  *          Avoid multiple threads/CPUs using engine at the same time.
  * @note 3.In copy mode, the destination address and calculation length must be 32 bytes (cacheline size) aligned.
  * @retval 0: success
  * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
  */
_LONG_CALL_ int crypto_sha2_update(SHA_context *ctx, const u8 *input, u8 *dst, size_t len);

/**
  * @brief  SHA final process.
  * @param 	ctx: SHA context
  * @param  output: the result of SHA function
  * @note 1.Since SHA is not support hw padding at the moment, it must do sw padding.
  * @note 2.The engine will be locked during the execution of this function,
  *         and will be unlocked after execution.
  *          Avoid multiple threads/CPUs using engine at the same time.
  * @retval 0: success
  * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
  */
_LONG_CALL_ int crypto_sha2_final(SHA_context *ctx, u8 *output);

/**
 * @brief HMAC init to read-only mode
 * @param  SHAtype:
 *            @arg SHA_224
 *            @arg SHA_256
 *            @arg SHA_384
 *            @arg SHA_512
 * @param  ctx: SHA context.
 * @param  key_id: value of KM_HMAC_KEY in ameba_key_management.h
 * @param  key_len_bits:
 *            @arg KEY_BIT_128
 *            @arg KEY_BIT_192
 *            @arg KEY_BIT_256
 * @note The engine will be locked during the execution of this function,
 *       and will be unlocked after execution.
 *       Avoid multiple threads/CPUs using engine at the same time.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_hmac_sha2_init(SHA_context *ctx, u32 SHAtype, u8 key_id, u32 key_len_bits);

/**
 * @brief  HMAC update
 * @param  ctx: SHA context.
 * @param  input: Pointer to input data.
 * @param  dst: Pointer to output data. If in read-only mode, set it to NULL
 * @param  len: Input data length.
 * @note 1.If copy mode and read-only mode are mixed for update,
 *         then ensure that the message lengths of both copy mode and read-only mode are integer multiples of the SHA-BLOCK length.
 *         (64-byte alignment for SHA-224/256, 128-byte alignment for SHA-384/512)
 * @note  2.The HMAC KEY will be reloaded before each update. If a software key is used,
 *         and the key is modified between updates, the calculation result will be wrong.
 *         So if the value of the software key is modified in the middle, please modify it to the correct key before updating.
 * @note   3.The engine will be locked during the execution of this function,
 *         and will be unlocked after execution.
 *         Avoid multiple threads/CPUs using engine at the same time.
 * @note   4.In copy mode, the destination address and calculation length must be 32 bytes (cacheline size) aligned.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_hmac_sha2_update(SHA_context *ctx, const u8 *input, u8 *dst, size_t len);

/**
 * @brief  HMAC final process.
 * @param  ctx: SHA context.
 * @param  output: the result of HMAC function
 * @note 1.The HMAC KEY will be reloaded before the final process.
 *         If a software key is used and the key is modified in final, the calculation result will be wrong.
 *         So if the value of the software key is modified in the middle, please modify it to the correct key before the final process.
 * @note  2.The engine will be locked during the execution of this function,
 *         and will be unlocked after execution.
 *         Avoid multiple threads/CPUs using engine at the same time.
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_hmac_sha2_final(SHA_context *ctx, u8 *output);
/** @} */
/** @} */

/** @} */
/** @} */
#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */


#endif