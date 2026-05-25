/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_RSIP_H_
#define _AMEBA_RSIP_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup RSIP
  * @brief RSIP driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup RSIP_Register_Definitions RSIP Register Definitions
  * @{
  */

/** @defgroup RSIP_STATUS
  * @brief Status Register
  * @{
  */
#define RSIP_BIT_IDLE               ((u32)0x00000001 << 0)           /*!< R 0x1  Indicates whether there is any outstanding transaction. Flahs_sec registers only can update when idle = 1.*/
#define RSIP_BIT_KEY_INIT_DONE      ((u32)0x00000001 << 1)           /*!< R 0x0  Indicates whether the initialization of the key is completed. Only used in MIX mode.*/
#define RSIP_BIT_INTR_ERR_TAG       ((u32)0x00000001 << 2)           /*!< R 0x0  Indicates a GCM tag mismatch error occurred.*/
#define RSIP_BIT_INTR_ERR_RESP      ((u32)0x00000001 << 3)           /*!< R 0x0  Indicates a the error response is received from slave.*/
#define RSIP_BIT_INTR_ERR_APB       ((u32)0x00000001 << 4)           /*!< R 0x0  Indicates whether the setting configuration failed. When a transaction is in progress, all settings except clear interruption are invalid. offset indicates that reads the locked key.*/
#define RSIP_BIT_INTR_ERR_INIT      ((u32)0x00000001 << 5)           /*!< R 0x0  Indicates the transaction was received before the key initialization is completed.*/
#define RSIP_BIT_REMAP_REGION_ERROR ((u32)0x00000001 << 6)           /*!< R 0x0  Indicates the read address hits multiple remapping region.*/
#define RSIP_BIT_REMAP_ADDR_ERROR   ((u32)0x00000001 << 7)           /*!< R 0x0  Indicates the address after adding offset size cross 256MB boundaries*/
#define RSIP_BIT_CONTROL_LOCKED     ((u32)0x00000001 << 8)           /*!< R 0x0  Indicates whether the setting is locked. If it is locked, only Clr_intr and Maintenance registers can be set*/
#define RSIP_MASK_KEY_LOCKED        ((u32)0x000000FF << 9)           /*!< R 0x0  Indicates whether the keyx is locked. If it is locked, keyx cannot be written or read*/
#define RSIP_KEY_LOCKED(x)          (((u32)((x) & 0x000000FF) << 9))
#define RSIP_GET_KEY_LOCKED(x)      ((u32)(((x >> 9) & 0x000000FF)))
/** @} */

/** @defgroup RSIP_CID
  * @brief Component Identification Register
  * @{
  */
#define RSIP_MASK_REGION_NUMBER       ((u32)0x0000000F << 0)            /*!< R ---  Number of supported decryption regions*/
#define RSIP_REGION_NUMBER(x)         (((u32)((x) & 0x0000000F) << 0))
#define RSIP_GET_REGION_NUMBER(x)     ((u32)(((x >> 0) & 0x0000000F)))
#define RSIP_MASK_REMAP_NUMBER        ((u32)0x0000000F << 4)            /*!< R ---  Number of supported remap regions*/
#define RSIP_REMAP_NUMBER(x)          (((u32)((x) & 0x0000000F) << 4))
#define RSIP_GET_REMAP_NUMBER(x)      ((u32)(((x >> 4) & 0x0000000F)))
#define RSIP_MASK_IV_NUMBER           ((u32)0x0000000F << 8)            /*!< R ---  Number of supported IVs registers*/
#define RSIP_IV_NUMBER(x)             (((u32)((x) & 0x0000000F) << 8))
#define RSIP_GET_IV_NUMBER(x)         ((u32)(((x >> 8) & 0x0000000F)))
#define RSIP_MASK_CTR_KEY_NUMBER      ((u32)0x0000000F << 12)           /*!< R ---  Number of supported CTR key registers, valid when support_ctr or support_gcm is 1*/
#define RSIP_CTR_KEY_NUMBER(x)        (((u32)((x) & 0x0000000F) << 12))
#define RSIP_GET_CTR_KEY_NUMBER(x)    ((u32)(((x >> 12) & 0x0000000F)))
#define RSIP_MASK_MIX_KEY_NUMBER      ((u32)0x0000000F << 16)           /*!< R ---  Number of supported MIX key registers, valid when support_mix is 1*/
#define RSIP_MIX_KEY_NUMBER(x)        (((u32)((x) & 0x0000000F) << 16))
#define RSIP_GET_MIX_KEY_NUMBER(x)    ((u32)(((x >> 16) & 0x0000000F)))
#define RSIP_BIT_SUPPORT_CTR          ((u32)0x00000001 << 20)           /*!< R ---  Indicate whether CTR mode is supported.*/
#define RSIP_BIT_SUPPORT_MIX          ((u32)0x00000001 << 21)           /*!< R ---  Indicate whether MIX mode is supported.*/
#define RSIP_BIT_SUPPORT_GCM          ((u32)0x00000001 << 22)           /*!< R ---  Indicate whether GCM mode is supported.*/
#define RSIP_BIT_KEY_SOURCE           ((u32)0x00000001 << 23)           /*!< R ---  Indicates the key source 0: from dedicated key port 1: from APB register*/
#define RSIP_MASK_LINE_SIZE           ((u32)0x00000007 << 24)           /*!< R ---  Cache line size 0: 16B = 1 AES block 1: 32B = 2 AES blocks 2: 64B = 4 AES blocks 3: 128B = 8 AES blocks Indicates how many 128-bit blocks are used to calculate the tag cache line size (Byte) = 16 * 2BLOCK_SIZE*/
#define RSIP_LINE_SIZE(x)             (((u32)((x) & 0x00000007) << 24))
#define RSIP_GET_LINE_SIZE(x)         ((u32)(((x >> 24) & 0x00000007)))
#define RSIP_BIT_INDEPENDENT_TAG_ADDR ((u32)0x00000001 << 27)           /*!< R ---  Indicates the storage location of the tag 0: consecutive space - interleaved with each data blocks 1: separate space from data - use another base address*/
/** @} */

/** @defgroup RSIP_ERROR_ADDR
  * @brief Error Address Log Register
  * @{
  */
#define RSIP_MASK_ERROR_ADDRESS   ((u32)0xFFFFFFFF << 0)           /*!< R 0x0  Recorded address when GCM Tag mismatch error or receiving an error response.*/
#define RSIP_ERROR_ADDRESS(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RSIP_GET_ERROR_ADDRESS(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup RSIP_CLR_INTR
  * @brief Interrupt Clear Register
  * @{
  */
#define RSIP_BIT_CLR_INTR ((u32)0x00000001 << 0) /*!< W 0x0  Interrupt clear. It has to write 1'b1 to this register every time when the error exception occurs.*/
/** @} */

/** @defgroup RSIP_CONTROL
  * @brief Control Register
  * @{
  */
#define RSIP_MASK_TAG_SIZE     ((u32)0x00000003 << 0)           /*!< R/W 0x0  Tag size. The size of the tag will affect the length of each tag command. 0: 32 bits 1: 64 bits 2: 128bits*/
#define RSIP_TAG_SIZE(x)       (((u32)((x) & 0x00000003) << 0))
#define RSIP_GET_TAG_SIZE(x)   ((u32)(((x >> 0) & 0x00000003)))
#define RSIP_BIT_APB_BYTE_SWAP ((u32)0x00000001 << 2)           /*!< R/W 0x0  Determine the endian of apb register. If set to 1, can swap byte order.*/
#define RSIP_BIT_AXI_BYTE_SWAP ((u32)0x00000001 << 3)           /*!< R/W 0x0  Determine the endian of ciphertext in slave memory. If set to 1, can swap byte order.*/
#define RSIP_BIT_AXI_WORD_SWAP ((u32)0x00000001 << 4)           /*!< R/W 0x0  Determine the endian of ciphertext in slave memory. If set to 1, can swap word order.*/
#define RSIP_MASK_KEY_SIZE     ((u32)0x00000003 << 5)           /*!< R/W 0x2  Determine the size of KEY 0: 128 bits 1: 192 bits 2: 256 bits*/
#define RSIP_KEY_SIZE(x)       (((u32)((x) & 0x00000003) << 5))
#define RSIP_GET_KEY_SIZE(x)   ((u32)(((x >> 5) & 0x00000003)))
#define RSIP_BIT_AES_EN        ((u32)0x00000001 << 8)           /*!< R/W 0x0  Determine whether to enable the AES engine 0: disable decryption engine (expect remapping function) 1: enable decryption engine and line buffer*/
#define RSIP_BIT_ICG_CTRL_0    ((u32)0x00000001 << 16)          /*!< R/W 0x0  Disable line-buffer auto clock gating*/
#define RSIP_BIT_ICG_CTRL_1    ((u32)0x00000001 << 17)          /*!< R/W 0x0  Disable fetch-unit auto clock gating*/
#define RSIP_BIT_ICG_CTRL_2    ((u32)0x00000001 << 18)          /*!< R/W 0x0  Disable aes-engine auto clock gating*/
#define RSIP_BIT_ICG_CTRL_3    ((u32)0x00000001 << 19)          /*!< R/W 0x0  Disable apb-ctrl auto clock gating*/
#define RSIP_BIT_ICG_CTRL_4    ((u32)0x00000001 << 20)          /*!< R/W 0x0  Disable key-reg auto clock gating*/
/** @} */

/** @defgroup RSIP_MAINTENANCE
  * @brief Maintenance Operation Register
  * @{
  */
#define RSIP_BIT_DEC_KEY_INIT     ((u32)0x00000001 << 1) /*!< W 0x0  Only used in MIX mode. Whenever the key is changed, write 1 to this bit to initialize the decryption key. When keys are update by external port, sec engine will trigger this bit automatically. The status of completion can be determined by reading bit 1 of the status register.*/
#define RSIP_BIT_CLEAN_CACHE_LINE ((u32)0x00000001 << 2) /*!< W 0x0  Manually clear the line buffer. Write 1 to clear all after flash data update.*/
/** @} */

/** @defgroup RSIP_LOCK
  * @brief Lock Control Register
  * @{
  */
#define RSIP_MASK_LOCK_CONTROL   ((u32)0x000000FF << 0)           /*!< W 0x0  When set DC to this field, all registers except Clr_intr and Maintenance are not allowed to be written. Once the lock is set, the way to unlock is reset the flash SEC engine. If SW wants to modified the setting of control registers, it should keep this lock unset.*/
#define RSIP_LOCK_CONTROL(x)     (((u32)((x) & 0x000000FF) << 0))
#define RSIP_GET_LOCK_CONTROL(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup RSIP_GEN_DATE
  * @brief Generate Date Register
  * @{
  */
#define RSIP_MASK_DATE   ((u32)0x0000FFFF << 0)            /*!< R ---  Date of code generation*/
#define RSIP_DATE(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define RSIP_GET_DATE(x) ((u32)(((x >> 0) & 0x0000FFFF)))
#define RSIP_MASK_YEAR   ((u32)0x0000FFFF << 16)           /*!< R ---  Year of code generation*/
#define RSIP_YEAR(x)     (((u32)((x) & 0x0000FFFF) << 16))
#define RSIP_GET_YEAR(x) ((u32)(((x >> 16) & 0x0000FFFF)))
/** @} */

/** @defgroup RSIP_VERSION
  * @brief Version Register
  * @{
  */
#define RSIP_MASK_VERSION   ((u32)0xFFFFFFFF << 0)           /*!< R ---  Version of code generation*/
#define RSIP_VERSION(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RSIP_GET_VERSION(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup RSIP_LOCK_KEYx
  * @brief Lock key x register
  * @{
  */
#define RSIP_MASK_LOCK_KEYx   ((u32)0x000000FF << 0)           /*!< W 0x0  When set 0xDC to this field, key_x are not allowed to be written or read. Once the lock is set, the way to unlock is reset the flash SEC engine. If SW wants to modified the key registers, it should keep this lock unset.*/
#define RSIP_LOCK_KEYx(x)     (((u32)((x) & 0x000000FF) << 0))
#define RSIP_GET_LOCK_KEYx(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup RSIP_IV_x_LOW
  * @brief IV x Bit [95:64]
  * @{
  */
#define RSIP_MASK_IV_x_LOW   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Fixed field of IV value. iv_x [95:64] If LINE_SIZE = 32B, IV = iv_x_high32 || iv_x_low32 || (address[27:0] >> (4+log2(LINE_SIZE/16)))32 || (2)32 If LINE_SIZE > 32B, IV = iv_x_high32 || iv_x_low32 || (address[27:0] >> (4+log2(LINE_SIZE/16)))32 || (address[4+log2(LINE_SIZE/16)-1:4] + 2)32*/
#define RSIP_IV_x_LOW(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RSIP_GET_IV_x_LOW(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup RSIP_IV_x_HIGH
  * @brief IV x Bit [127:96]
  * @{
  */
#define RSIP_MASK_IV_x_HIGH   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Fixed field of IV value. iv_x [127:96] If LINE_SIZE = 32B, IV = iv_x_high32 || iv_x_low32 || (address[27:0] >> (4+log2(LINE_SIZE/16)))32 || (2)32 If LINE_SIZE > 32B, IV = iv_x_high32 || iv_x_low32 || (address[27:0] >> (4+log2(LINE_SIZE/16)))32 || (address[4+log2(LINE_SIZE/16)-1:4] + 2)32*/
#define RSIP_IV_x_HIGH(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RSIP_GET_IV_x_HIGH(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup RSIP_REGION_x_BASE
  * @brief Region x base address register
  * @{
  */
#define RSIP_MASK_REGION_x_BASE   ((u32)0x07FFFFFF << 5)           /*!< R/W 0x0  Region x base address register.*/
#define RSIP_REGION_x_BASE(x)     (((u32)((x) & 0x07FFFFFF) << 5))
#define RSIP_GET_REGION_x_BASE(x) ((u32)(((x >> 5) & 0x07FFFFFF)))
/** @} */

/** @defgroup RSIP_REGION_x_END
  * @brief Region x end address register
  * @{
  */
#define RSIP_MASK_REGION_x_END   ((u32)0x07FFFFFF << 5)           /*!< R/W 0x0  Region x end address register.*/
#define RSIP_REGION_x_END(x)     (((u32)((x) & 0x07FFFFFF) << 5))
#define RSIP_GET_REGION_x_END(x) ((u32)(((x >> 5) & 0x07FFFFFF)))
/** @} */

/** @defgroup RSIP_REGION_x_CONFIG
  * @brief Region x setting register
  * @{
  */
#define RSIP_BIT_REGION_x_ENABLE     ((u32)0x00000001 << 0)           /*!< R/W 0x0  Enable region x setting*/
#define RSIP_MASK_REGION_x_MODE      ((u32)0x00000003 << 1)           /*!< R/W 0x0  Mode used in region x 2'b00: GCM mode 2'b01: CTR mode 2'b10: MIX mode 2'b11: Reserved*/
#define RSIP_REGION_x_MODE(x)        (((u32)((x) & 0x00000003) << 1))
#define RSIP_GET_REGION_x_MODE(x)    ((u32)(((x >> 1) & 0x00000003)))
#define RSIP_MASK_REGION_x_IV_NUM    ((u32)0x00000007 << 3)           /*!< R/W 0x0  IV used in region x Apply which IV reigster to this region Number corresponds to IV register*/
#define RSIP_REGION_x_IV_NUM(x)      (((u32)((x) & 0x00000007) << 3))
#define RSIP_GET_REGION_x_IV_NUM(x)  ((u32)(((x >> 3) & 0x00000007)))
#define RSIP_MASK_REGION_x_KEY_NUM   ((u32)0x00000007 << 6)           /*!< R/W 0x0  Key used in region x Apply which key reigster to this region Number corresponds to key register*/
#define RSIP_REGION_x_KEY_NUM(x)     (((u32)((x) & 0x00000007) << 6))
#define RSIP_GET_REGION_x_KEY_NUM(x) ((u32)(((x >> 6) & 0x00000007)))
/** @} */

/** @defgroup RSIP_REGION_x_TAG_BASE
  * @brief Region x GCM tag base address register
  * @{
  */
#define RSIP_MASK_REGION_x_TAG_BASE_ADDR   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Region x GCM tag base address register.*/
#define RSIP_REGION_x_TAG_BASE_ADDR(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RSIP_GET_REGION_x_TAG_BASE_ADDR(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup RSIP_REMAP_x_BASE_ADDR
  * @brief Remap x Region Base Address Register
  * @{
  */
#define RSIP_MASK_REMAP_BASE_x_ADDR   ((u32)0x007FFFFF << 9)           /*!< R/W 0x0  Remap region x base address [27:5] Region_Base_Addr = {flash_base[31:28], remap_base_x_addr, {SIZE_BIT{1'b0}} } The region address is configurable, and not cross 256KB Note: User-configured aligned address is 16, 32, 64, 128, 256, 512, 1024, 2048 or 4096 bytes. The corresponding SIZE_BIT is 4, 5, 6, 7, 8, 9, 10, 11 or 12, default SIZE_BIT is 5.*/
#define RSIP_REMAP_BASE_x_ADDR(x)     (((u32)((x) & 0x007FFFFF) << 9))
#define RSIP_GET_REMAP_BASE_x_ADDR(x) ((u32)(((x >> 9) & 0x007FFFFF)))
/** @} */

/** @defgroup RSIP_REMAP_x_END_ADDR
  * @brief Remap x Region End Address Register
  * @{
  */
#define RSIP_MASK_REMAP_END_x_ADDR   ((u32)0x007FFFFF << 9)           /*!< R/W 0x0  Remap region x end address [27:5] Region_End_Addr = {flash_base[31:28], remap_end_x_addr, {SIZE_BIT{1'b0}} } The region address is configurable, and not cross 256KB Note: User-configured aligned address is 16, 32, 64, 128, 256, 512, 1024, 2048 or 4096 bytes. The corresponding SIZE_BIT is 4, 5, 6, 7, 8, 9, 10, 11 or 12, default SIZE_BIT is 5.*/
#define RSIP_REMAP_END_x_ADDR(x)     (((u32)((x) & 0x007FFFFF) << 9))
#define RSIP_GET_REMAP_END_x_ADDR(x) ((u32)(((x >> 9) & 0x007FFFFF)))
/** @} */

/** @defgroup RSIP_REMAP_x_OFFSET
  * @brief Remap x Offset Register
  * @{
  */
#define RSIP_BIT_REMAP_x_ENABLE    ((u32)0x00000001 << 0)           /*!< R/W 0x0  Remap region x enable*/
#define RSIP_MASK_REMAP_x_OFFSET   ((u32)0x007FFFFF << 9)           /*!< R/W 0x0  Remap region x offset [27:SIZE_BIT] Region_Offset = {4'b0,remap_x_offset, {SIZE_BIT{1'b0}} } The output address is "Access address - Region_Base_Addr + Region_Offset" Default SIZE_BIT is 5.*/
#define RSIP_REMAP_x_OFFSET(x)     (((u32)((x) & 0x007FFFFF) << 9))
#define RSIP_GET_REMAP_x_OFFSET(x) ((u32)(((x >> 9) & 0x007FFFFF)))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup RSIP_Exported_Types RSIP Exported Types
  * @{
  */

/** @brief RSIP Port
  */
typedef struct {
	__IO uint32_t RSIP_IV_x_LOW ;  /*!< IV x BIT [95:64] Register */
	__IO uint32_t RSIP_IV_x_HIGH;  /*!< IV x BIT [127:96] Register */
} OTF_IV_TypeDef;

/** @brief RSIP Port
  */
typedef struct {
	__IO uint32_t RSIP_REGION_x_BASE    ;  /*!< REGION x BASE ADDRESS REGISTER */
	__IO uint32_t RSIP_REGION_x_END     ;  /*!< REGION x END ADDRESS REGISTER */
	__IO uint32_t RSIP_REGION_x_CONFIG  ;  /*!< REGION x SETTING REGISTER */
	__IO uint32_t RSIP_REGION_x_TAG_BASE;  /*!< REGION x GCM TAG BASE ADDRESS REGISTER */
} OTF_Region_TypeDef;

/** @brief RSIP Port
  */
typedef struct {
	__IO uint32_t RSIP_REMAP_x_BASE_ADDR;  /*!< REMAP x REGION BASE ADDRESS REGISTER */
	__IO uint32_t RSIP_REMAP_x_END_ADDR ;  /*!< REMAP x REGION END ADDRESS REGISTER */
	__IO uint32_t RSIP_REMAP_x_OFFSET   ;  /*!< REMAP x OFFSET REGISTER */
} RSIP_MMU_TypeDef;

/** @brief RSIP Register Declaration
  */

typedef struct {
	__I  uint32_t RSIP_STATUS                     ;  /*!< STATUS REGISTER,  Address offset:0x000 */
	__I  uint32_t RSIP_CID                        ;  /*!< COMPONENT IDENTIFICATION REGISTER,  Address offset:0x004 */
	__I  uint32_t RSIP_ERROR_ADDR                 ;  /*!< ERROR ADDRESS LOG REGISTER,  Address offset:0x008 */
	__IO uint32_t RSVD0                           ;  /*!< Reserved,  Address offset:0x00C-0x00F */
	__O  uint32_t RSIP_CLR_INTR                   ;  /*!< INTERRUPT CLEAR REGISTER,  Address offset:0x010 */
	__IO uint32_t RSIP_CONTROL                    ;  /*!< CONTROL REGISTER,  Address offset:0x014 */
	__O  uint32_t RSIP_MAINTENANCE                ;  /*!< MAINTENANCE OPERATION REGISTER,  Address offset:0x018 */
	__O  uint32_t RSIP_LOCK                       ;  /*!< LOCK CONTROL REGISTER,  Address offset:0x01C */
	__IO uint32_t RSVD1[2]                        ;  /*!< Reserved,  Address offset:0x020-0x027 */
	__I  uint32_t RSIP_GEN_DATE                   ;  /*!< GENERATE DATE REGISTER,  Address offset:0x028 */
	__I  uint32_t RSIP_VERSION                    ;  /*!< VERSION REGISTER,  Address offset:0x02C */
	__O  uint32_t RSIP_LOCK_KEYx[8]               ;  /*!< LOCK KEY x REGISTER,  Address offset:0x030-0x04C */
	__IO uint32_t RSVD2[4]                        ;  /*!< Reserved,  Address offset:0x050-0x05F */
	OTF_IV_TypeDef OTF_IV[4]                      ;  /*!< RSIP IP have 4 ports,  Address offset:0x060-0x07C */
	__IO uint32_t RSVD3[32]                       ;  /*!< Reserved,  Address offset:0x080-0x0FF */
	OTF_Region_TypeDef OTF_REGION[4]              ;  /*!< RSIP IP have 4 ports,  Address offset:0x100-0x13C */
	__IO uint32_t RSVD4[16]                       ;  /*!< Reserved,  Address offset:0x140-0x17F */
	RSIP_MMU_TypeDef FLASH_MMU[4]                 ;  /*!< RSIP IP have 4 ports,  Address offset:0x180-0x1AC */
} RSIP_REG_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your defination here

/* Exported constants --------------------------------------------------------*/
/** @addtogroup RSIP_Exported_Constants RSIP Exported Constants
  * @{
  */

/** @defgroup RSIP_Key_Req_Poll_Times
  * @{
  */
#define KEY_REQ_POLL_TIMES			0xFF
/**
  * @}
  */

/** @defgroup RSIP_Addr_Mask
  * @{
  */
#define MMU_ADDR_BIT_MASK			((u32)0x0FFFF000)			/*!<Decoder mask enable for MMU address */
#define RSIP_ADDR_AlIGN32B(x)		((x) & (~0x1F))				/*!<Decoder mask enable for MMU address */

/**
  * @}
  */

/** @defgroup RSIP_MMU_Addr_Shift
  * @{
  */
#define MMU_END_ADDR_SHIFT			12
#define MMU_START_ADDR_SHIFT		4
#define MMU_OFFSET_ADDR_SHIFT		4
/**
  * @}
  */

/** @defgroup RSIP_MMU_Idx
  * @{
  */
#define MMU_ID0			0
#define MMU_ID1			1
#define MMU_ID2			2
#define MMU_ID3			3
/**
  * @}
  */

/** @defgroup RSIP_IV_Idx
  * @{
  */
#define RSIP_IV0			0
#define RSIP_IV1			1
#define RSIP_IV2			2
#define RSIP_IV3			3
#define IS_OTF_IVNUM(NUM) ((NUM) <= 0x3)
/**
  * @}
  */

/** @defgroup RSIP_OTF_KeyNum
  * @{
  */
#define RSIP_KEY_NUM0			0  /* Key Group 0: 0x2C0 is ECB key; 0x2E0 is CTR key */
#define RSIP_KEY_NUM1			1  /* Key Group 1: 0x200 is ECB key; 0x220 is CTR key */
#define IS_OTF_KEY_NUM(KEY)  ((KEY == RSIP_KEY_NUM0) || (KEY == RSIP_KEY_NUM1))
/**
  * @}
  */

/** @defgroup RSIP_OTF_Ivnum
  * @{
  */
#define RSIP_REGION0			0
#define RSIP_REGION1			1
#define RSIP_REGION2			2
#define RSIP_REGION3			3
/**
  * @}
  */

/** @defgroup RSIP_OTF_Mode
  * @{
  */
#define OTF_GCM_MODE		0x0
#define OTF_CTR_MODE		0x1
#define OTF_XTS_MODE		0x2
#define OTF_INVALID_MODE		0xff
#define IS_OTF_MODE(MODE) 	((MODE == OTF_GCM_MODE) || (MODE == OTF_CTR_MODE) || (MODE == OTF_XTS_MODE))
/**
  * @}
  */

/** @defgroup GCM_Tag_Length
  * @{
  */
#define GCM_4_BYTE_TAG		0x4
#define GCM_8_BYTE_TAG		0x8
#define GCM_16_BYTE_TAG		0x10
#define IS_GCM_TAG_LEN(LEN_BYTE) 	((LEN_BYTE == GCM_4_BYTE_TAG) || (LEN_BYTE == GCM_8_BYTE_TAG) || (LEN_BYTE == GCM_16_BYTE_TAG))
/**
  * @}
  */

/** @defgroup RSIP_OTF_Key_Size
  * @{
  */
#define OTF_KEY_128			0x0
#define OTF_KEY_192			0x1
#define OTF_KEY_256			0x2
#define IS_OTF_KEY(SIZE) 	((SIZE) <= 0x2)
/**
  * @}
  */


/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup RSIP_Exported_Functions RSIP Exported Functions
  * @{
  */
_LONG_CALL_ void RSIP_WAIT_IDLE(void);
_LONG_CALL_ void RSIP_OTF_init(u32 KeySize);
_LONG_CALL_ void RSIP_IV_Set(u8 index, u8 *IV);
_LONG_CALL_ void RSIP_OTF_Cmd(u32 NewStatus);
_LONG_CALL_ void RSIP_OTF_Set_GCM_Tag_Length(u8 tag_length_byte);
_LONG_CALL_ void RSIP_OTF_Enable(u32 EnIdx, u32 BaseAddr, u32 EndAddr, u32 NewStatus, u32 IVNum, u32 KeyNum, u32 Mode, u32 GCM_TagBase);
_LONG_CALL_ void RSIP_MMU_Config(u32 MMUIdx, u32 AddrStart, u32 AddrEnd, u32 AddrOffset);
_LONG_CALL_ void RSIP_MMU_Cmd(u32 MMUIdx, u32 NewStatus);
_LONG_CALL_ void RSIP_MMU_Cache_Clean(void);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

#endif

/** @} */

/** @} */
