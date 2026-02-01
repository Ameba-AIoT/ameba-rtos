/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_RXI300_H_
#define _AMEBA_RXI300_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup RXI300
  * @brief RXI300 driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup RXI300_Register_Definitions RXI300 Register Definitions
  * @{
  */

/** @defgroup NAME
  * @brief RXI300 design name register
  * @{
  */
#define RXI300_MASK_NAME   ((u32)0xFFFFFFFF << 0)           /*!< R 00052300h  The design name of RXI-300.*/
#define RXI300_NAME(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_NAME(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup VER
  * @brief RXI300 design version register
  * @{
  */
#define RXI300_MASK_VER   ((u32)0xFFFFFFFF << 0)           /*!< R 00000300h  The design version of RXI-300.*/
#define RXI300_VER(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_VER(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup RVE
  * @brief RXI300 design reversion register
  * @{
  */
#define RXI300_MASK_REV   ((u32)0xFFFFFFFF << 0)           /*!< R 1h  The design reversion of RXI-300.*/
#define RXI300_REV(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_REV(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup INST
  * @brief RXI300 design instance number register
  * @{
  */
#define RXI300_MASK_INST   ((u32)0xFFFFFFFF << 0)           /*!< R 0h  The design instance number of RXI-300.*/
#define RXI300_INST(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_INST(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup IMPL_Y
  * @brief RXI300 implementation year register
  * @{
  */
#define RXI300_MASK_IMPL_Y   ((u32)0xFFFFFFFF << 0)           /*!< R 2024h  The implementatin year of RXI-300. Reset value depends on implementation date.*/
#define RXI300_IMPL_Y(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_IMPL_Y(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup IMPL_D
  * @brief RXI300 implementation date register
  * @{
  */
#define RXI300_MASK_IMPL_D   ((u32)0xFFFFFFFF << 0)           /*!< R 05101058h  The implementatin date of RXI-300. Reset value depends on implementation date.*/
#define RXI300_IMPL_D(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_IMPL_D(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup DEV
  * @brief RXI300 developer register
  * @{
  */
#define RXI300_MASK_DEV   ((u32)0xFFFFFFFF << 0)           /*!< R 05207433h  The developer of RXI-300.*/
#define RXI300_DEV(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_DEV(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup PRO_NUM
  * @brief RXI300 project number register
  * @{
  */
#define RXI300_MASK_PRO_NUM   ((u32)0xFFFFFFFF << 0)           /*!< R 00007005h  The project number of RXI-300.*/
#define RXI300_PRO_NUM(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_PRO_NUM(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_PLD0_S
  * @brief Error log payload register0
  * @{
  */
#define RXI300_MASK_ELR_BSTINDEX_S   ((u32)0x000000FF << 24)           /*!< R 0h  Burst index of the error request.*/
#define RXI300_ELR_BSTINDEX_S(x)     (((u32)((x) & 0x000000FF) << 24))
#define RXI300_GET_ELR_BSTINDEX_S(x) ((u32)(((x >> 24) & 0x000000FF)))
#define RXI300_MASK_ELR_BSTLEN_S     ((u32)0x000000FF << 16)           /*!< R 0h  Burst Length of the error request.*/
#define RXI300_ELR_BSTLEN_S(x)       (((u32)((x) & 0x000000FF) << 16))
#define RXI300_GET_ELR_BSTLEN_S(x)   ((u32)(((x >> 16) & 0x000000FF)))
#define RXI300_MASK_ELR_BSTTYPE_S    ((u32)0x00000007 << 11)           /*!< R 0h  BurstSeq of the error request.*/
#define RXI300_ELR_BSTTYPE_S(x)      (((u32)((x) & 0x00000007) << 11))
#define RXI300_GET_ELR_BSTTYPE_S(x)  ((u32)(((x >> 11) & 0x00000007)))
#define RXI300_MASK_ELR_CMD_S        ((u32)0x00000007 << 8)            /*!< R 0h  CMD of the error request. 0x1: Write command 0x2: Read command*/
#define RXI300_ELR_CMD_S(x)          (((u32)((x) & 0x00000007) << 8))
#define RXI300_GET_ELR_CMD_S(x)      ((u32)(((x >> 8) & 0x00000007)))
#define RXI300_MASK_ELR_SRC_S        ((u32)0x000000FF << 0)            /*!< R 0h  Error source of the error request. 0x10: S1 securedefault slave                                                                                                                                                                                                0x20: S2 securedefault slave  0x30: S3-0~1,5,7(AHB) group secure default slave                                                                                                                                                                                                0x31: S3-2~4,6,13~22(AHB) group secure default slave                                                                                                                                                                                              0x32: S3-8(AXI) secure default slave                                                                                                                                                                                                0x33:S3-9~10(SPIC)group  secure default slave                                                                                                                                                                                              0x34: S3-11~12(PSRAM)group  secure default slave                                                                                                                                                                                                 0x60: S4-0~6 secure default slave                                                                                                                                                                                     0x61: S4-7~15 secure default slave                                                                                                                                                                                           0x70: S5 secure default slave                                                                                                                                                                                                 0x71: MPC/PPC secure default slave                                                                                                                                                                                       0x80: S8 secure default slave*/
#define RXI300_ELR_SRC_S(x)          (((u32)((x) & 0x000000FF) << 0))
#define RXI300_GET_ELR_SRC_S(x)      ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup ELR_PLD1_S
  * @brief Error log payload register1
  * @{
  */
#define RXI300_MASK_ELR_LOCK_S    ((u32)0x00000003 << 30)           /*!< R 0h  AxLock of the error request.*/
#define RXI300_ELR_LOCK_S(x)      (((u32)((x) & 0x00000003) << 30))
#define RXI300_GET_ELR_LOCK_S(x)  ((u32)(((x >> 30) & 0x00000003)))
#define RXI300_MASK_ELR_CACHE_S   ((u32)0x0000000F << 26)           /*!< R 0h  AxCache of the error request.*/
#define RXI300_ELR_CACHE_S(x)     (((u32)((x) & 0x0000000F) << 26))
#define RXI300_GET_ELR_CACHE_S(x) ((u32)(((x >> 26) & 0x0000000F)))
#define RXI300_MASK_ELR_PROT_S    ((u32)0x00000007 << 23)           /*!< R 0h  AxProt of the error request.*/
#define RXI300_ELR_PROT_S(x)      (((u32)((x) & 0x00000007) << 23))
#define RXI300_GET_ELR_PROT_S(x)  ((u32)(((x >> 23) & 0x00000007)))
#define RXI300_MASK_ELR_SIZE_S    ((u32)0x00000007 << 16)           /*!< R 0h  Size of the error request.*/
#define RXI300_ELR_SIZE_S(x)      (((u32)((x) & 0x00000007) << 16))
#define RXI300_GET_ELR_SIZE_S(x)  ((u32)(((x >> 16) & 0x00000007)))
/** @} */

/** @defgroup ELR_ID_S
  * @brief Error log ID information register
  * @{
  */
#define RXI300_MASK_ELR_ID_S   ((u32)0xFFFFFFFF << 0)           /*!< R 0h  Tagid of the error request. 9'b00000????: M0 access error 9'b00001????: M1 access error 9'b0100?????: M2 access error 9'b000110???: M3 access error 9'b0001010??: M4 access error 9'b000100110: M5-0 access error 9'b000100111: M5-1 access error 9'b0101?????: M6 access error     9'b1????????: M7 access error 9'b00010000?: M8 access error 9'b00010010: M9 access error 9'b00010001: M10 access error 9'b011000???: M11 access error*/
#define RXI300_ELR_ID_S(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_ELR_ID_S(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_ADR0_S
  * @brief Error log addrress register0
  * @{
  */
#define RXI300_MASK_ELR_ADR0_S   ((u32)0xFFFFFFFF << 0)           /*!< R 0h  Address[31:0] of the error request.*/
#define RXI300_ELR_ADR0_S(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_ELR_ADR0_S(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_ADR1_S
  * @brief Error log addrress register1
  * @{
  */
#define RXI300_MASK_ELR_ADR1_S   ((u32)0xFFFFFFFF << 32)           /*!< R 0h  Address[63:32] of the error request.*/
#define RXI300_ELR_ADR1_S(x)     (((u32)((x) & 0xFFFFFFFF) << 32))
#define RXI300_GET_ELR_ADR1_S(x) ((u32)(((x >> 32) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_CODE_S
  * @brief Error code register
  * @{
  */
#define RXI300_MASK_ELR_CODE_S   ((u32)0x000000FF << 0)           /*!< R 0h  The error code of error request, if it's 0, indicates that there is no error request.  8'h00: No error 8'h02: Secure fault error*/
#define RXI300_ELR_CODE_S(x)     (((u32)((x) & 0x000000FF) << 0))
#define RXI300_GET_ELR_CODE_S(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup ELR_INTR_CLR_S
  * @brief Error log clear register
  * @{
  */
#define RXI300_BIT_ELR_INTR_CLR_S ((u32)0x00000001 << 0) /*!< W 0h  Error log interrupt clear. Writing 1'b1 to the register will trigger a 1-cycle pulse interrupt clear. It has to write 1'b1 to this register every time when the error exception occurs. Note that this register is write-only, the return value of reading this register is meaningless.*/
/** @} */

/** @defgroup IDAU_BAx
  * @brief Base address register x in IDAU
  * @{
  */
#define RXI300_MASK_IDAU_BAx   ((u32)0x0000FFFF << 12)           /*!< R/W 0h  It's the base address of region x which can be set as non-secure.                                                                                                       [11:0]: reserved [27:12]: base address of region x                                                                                                                                                                                                [31:28]: reserved*/
#define RXI300_IDAU_BAx(x)     (((u32)((x) & 0x0000FFFF) << 12))
#define RXI300_GET_IDAU_BAx(x) ((u32)(((x >> 12) & 0x0000FFFF)))
/** @} */

/** @defgroup IDAU_LAx
  * @brief Top address register x in IDAU
  * @{
  */
#define RXI300_MASK_IDAU_LAx   ((u32)0x0000FFFF << 12)           /*!< R/W FFFFh  It's the top address of region x which can be set as non-secure. [11:0]: reserved [27:12]: top address of region x                                                                                                                                                                                                [31:28]: reserved*/
#define RXI300_IDAU_LAx(x)     (((u32)((x) & 0x0000FFFF) << 12))
#define RXI300_GET_IDAU_LAx(x) ((u32)(((x >> 12) & 0x0000FFFF)))
/** @} */

/** @defgroup IDAU_CTRL
  * @brief Enable control register in IDAU
  * @{
  */
#define RXI300_MASK_IDAU_WRITE_CTRL    ((u32)0x0000003F << 16)           /*!< R/W 0h  Region entry enable After setting WRITE_CTRL bits as 1, corresponding regions is set as write-forbidden.Note that if WRITE_CTRL bits are enabled, the region remains secure even if ba*i/la*i is set. [16]: enable entry for ba0/la0 0: disable  1: enable                                                                                                                                                                                                                                                     [17]: enable entry for ba1/la1 0: disable  1: enable                                                                                                                                                                                                                                              [18]: enable entry for ba2/la2 0: disable  1: enable [19]: enable entry for ba3/la3 0: disable  1: enable                                                                                                                                                                                                                                                 [20]: enable entry for ba4/la4 0: disable  1: enable                                                                                                                                                                                                                                               [21]: enable entry for ba5/la5 0: disable  1: enable*/
#define RXI300_IDAU_WRITE_CTRL(x)      (((u32)((x) & 0x0000003F) << 16))
#define RXI300_GET_IDAU_WRITE_CTRL(x)  ((u32)(((x >> 16) & 0x0000003F)))
#define RXI300_MASK_IDAU_SECURE_CTRL   ((u32)0x0000003F << 0)            /*!< R/W 0h  Region entry enable After setting SECURE_CTRL bits as 1, corresponding regions is set as non-secure.Note that if SECURE_CTRL bits are enabled, the region remains secure even if ba*i/la*i is set. [0]: enable entry for ba0/la0 0: disable  1: enable                                                                                                                                                                                                                                                 [1]: enable entry for ba1/la1 0: disable  1: enable                                                                                                                                                                                                                                                  [2]: enable entry for ba2/la2 0: disable  1: enable [3]: enable entry for ba3/la3 0: disable  1: enable                                                                                                                                                                                                                                                 [4]: enable entry for ba4/la4 0: disable  1: enable                                                                                                                                                                                                                                                  [5]: enable entry for ba5/la5 0: disable  1: enable*/
#define RXI300_IDAU_SECURE_CTRL(x)     (((u32)((x) & 0x0000003F) << 0))
#define RXI300_GET_IDAU_SECURE_CTRL(x) ((u32)(((x >> 0) & 0x0000003F)))
/** @} */

/** @defgroup IDAU_LOCK
  * @brief Lock control register in IDAU
  * @{
  */
#define RXI300_BIT_IDAU_LOCK ((u32)0x00000001 << 0) /*!< R/W 0h  Lock write transfer for all registers in IDAU (including lock register itself) until system reset if it is ebabled. 0: disable lock 1: enable lock*/
/** @} */

/** @defgroup IDAU1_BAx
  * @brief Base address register x in IDAU1
  * @{
  */
#define RXI300_MASK_IDAU1_BAx   ((u32)0x0000FFFF << 12)           /*!< R/W 0h  It's the base address of region x which can be set as non-secure.                                                                                                       [11:0]: reserved [27:12]: base address of region x                                                                                                                                                                                                [31:28]: reserved*/
#define RXI300_IDAU1_BAx(x)     (((u32)((x) & 0x0000FFFF) << 12))
#define RXI300_GET_IDAU1_BAx(x) ((u32)(((x >> 12) & 0x0000FFFF)))
/** @} */

/** @defgroup IDAU1_LAx
  * @brief Top address register x in IDAU1
  * @{
  */
#define RXI300_MASK_IDAU1_LAx   ((u32)0x0000FFFF << 12)           /*!< R/W FFFFh  It's the top address of region x which can be set as non-secure. [11:0]: reserved [27:12]: top address of region x                                                                                                                                                                                                [31:28]: reserved*/
#define RXI300_IDAU1_LAx(x)     (((u32)((x) & 0x0000FFFF) << 12))
#define RXI300_GET_IDAU1_LAx(x) ((u32)(((x >> 12) & 0x0000FFFF)))
/** @} */

/** @defgroup IDAU1_CTRL
  * @brief Enable control register in IDAU1
  * @{
  */
#define RXI300_MASK_IDAU1_WRITE_CTRL    ((u32)0x0000003F << 16)           /*!< R/W 0h  Region entry enable After setting WRITE_CTRL bits as 1, corresponding regions is set as write-forbidden.Note that if WRITE_CTRL bits are enabled, the region remains secure even if ba*i/la*i is set. [16]: enable entry for ba0/la0 0: disable  1: enable                                                                                                                                                                                                                                                     [17]: enable entry for ba1/la1 0: disable  1: enable                                                                                                                                                                                                                                              [18]: enable entry for ba2/la2 0: disable  1: enable [19]: enable entry for ba3/la3 0: disable  1: enable                                                                                                                                                                                                                                                 [20]: enable entry for ba4/la4 0: disable  1: enable                                                                                                                                                                                                                                               [21]: enable entry for ba5/la5 0: disable  1: enable*/
#define RXI300_IDAU1_WRITE_CTRL(x)      (((u32)((x) & 0x0000003F) << 16))
#define RXI300_GET_IDAU1_WRITE_CTRL(x)  ((u32)(((x >> 16) & 0x0000003F)))
#define RXI300_MASK_IDAU1_SECURE_CTRL   ((u32)0x0000003F << 0)            /*!< R/W 0h  Region entry enable After setting SECURE_CTRL bits as 1, corresponding regions is set as non-secure.Note that if SECURE_CTRL bits are enabled, the region remains secure even if ba*i/la*i is set. [0]: enable entry for ba0/la0 0: disable  1: enable                                                                                                                                                                                                                                                 [1]: enable entry for ba1/la1 0: disable  1: enable                                                                                                                                                                                                                                                  [2]: enable entry for ba2/la2 0: disable  1: enable [3]: enable entry for ba3/la3 0: disable  1: enable                                                                                                                                                                                                                                                 [4]: enable entry for ba4/la4 0: disable  1: enable                                                                                                                                                                                                                                                  [5]: enable entry for ba5/la5 0: disable  1: enable*/
#define RXI300_IDAU1_SECURE_CTRL(x)     (((u32)((x) & 0x0000003F) << 0))
#define RXI300_GET_IDAU1_SECURE_CTRL(x) ((u32)(((x >> 0) & 0x0000003F)))
/** @} */

/** @defgroup IDAU1_LOCK
  * @brief Lock control register in IDAU1
  * @{
  */
#define RXI300_BIT_IDAU1_LOCK ((u32)0x00000001 << 0) /*!< R/W 0h  Lock write transfer for all registers in IDAU1 (including lock register itself) until system reset if it is ebabled. 0: disable lock 1: enable lock*/
/** @} */

/** @defgroup IDAU2_BAx
  * @brief Base address register x in IDAU2
  * @{
  */
#define RXI300_MASK_IDAU2_BAx   ((u32)0x0000FFFF << 12)           /*!< R/W 0h  It's the base address of region x which can be set as non-secure.                                                                                                       [11:0]: reserved [27:12]: base address of region x                                                                                                                                                                                                [31:28]: reserved*/
#define RXI300_IDAU2_BAx(x)     (((u32)((x) & 0x0000FFFF) << 12))
#define RXI300_GET_IDAU2_BAx(x) ((u32)(((x >> 12) & 0x0000FFFF)))
/** @} */

/** @defgroup IDAU2_LAx
  * @brief Top address register x in IDAU2
  * @{
  */
#define RXI300_MASK_IDAU2_LAx   ((u32)0x0000FFFF << 12)           /*!< R/W FFFFh  It's the top address of region x which can be set as non-secure. [11:0]: reserved [27:12]: top address of region x                                                                                                                                                                                                [31:28]: reserved*/
#define RXI300_IDAU2_LAx(x)     (((u32)((x) & 0x0000FFFF) << 12))
#define RXI300_GET_IDAU2_LAx(x) ((u32)(((x >> 12) & 0x0000FFFF)))
/** @} */

/** @defgroup IDAU2_CTRL
  * @brief Enable control register in IDAU2
  * @{
  */
#define RXI300_MASK_IDAU2_WRITE_CTRL    ((u32)0x0000003F << 16)           /*!< R/W 0h  Region entry enable After setting WRITE_CTRL bits as 1, corresponding regions is set as write-forbidden.Note that if WRITE_CTRL bits are enabled, the region remains secure even if ba*i/la*i is set. [16]: enable entry for ba0/la0 0: disable  1: enable                                                                                                                                                                                                                                                     [17]: enable entry for ba1/la1 0: disable  1: enable                                                                                                                                                                                                                                              [18]: enable entry for ba2/la2 0: disable  1: enable [19]: enable entry for ba3/la3 0: disable  1: enable                                                                                                                                                                                                                                                 [20]: enable entry for ba4/la4 0: disable  1: enable                                                                                                                                                                                                                                               [21]: enable entry for ba5/la5 0: disable  1: enable*/
#define RXI300_IDAU2_WRITE_CTRL(x)      (((u32)((x) & 0x0000003F) << 16))
#define RXI300_GET_IDAU2_WRITE_CTRL(x)  ((u32)(((x >> 16) & 0x0000003F)))
#define RXI300_MASK_IDAU2_SECURE_CTRL   ((u32)0x0000003F << 0)            /*!< R/W 0h  Region entry enable After setting SECURE_CTRL bits as 1, corresponding regions is set as non-secure.Note that if SECURE_CTRL bits are enabled, the region remains secure even if ba*i/la*i is set. [0]: enable entry for ba0/la0 0: disable  1: enable                                                                                                                                                                                                                                                 [1]: enable entry for ba1/la1 0: disable  1: enable                                                                                                                                                                                                                                                  [2]: enable entry for ba2/la2 0: disable  1: enable [3]: enable entry for ba3/la3 0: disable  1: enable                                                                                                                                                                                                                                                 [4]: enable entry for ba4/la4 0: disable  1: enable                                                                                                                                                                                                                                                  [5]: enable entry for ba5/la5 0: disable  1: enable*/
#define RXI300_IDAU2_SECURE_CTRL(x)     (((u32)((x) & 0x0000003F) << 0))
#define RXI300_GET_IDAU2_SECURE_CTRL(x) ((u32)(((x >> 0) & 0x0000003F)))
/** @} */

/** @defgroup IDAU2_LOCK
  * @brief Lock control register in IDAU2
  * @{
  */
#define RXI300_BIT_IDAU2_LOCK ((u32)0x00000001 << 0) /*!< R/W 0h  Lock write transfer for all registers in IDAU2 (including lock register itself) until system reset if it is ebabled. 0: disable lock 1: enable lock*/
/** @} */

/** @defgroup PPC1_REG
  * @brief PPC1 configure register0
  * @{
  */
#define RXI300_BIT_PPE_CFG             ((u32)0x00000001 << 19) /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_AES_SHA_DMA         ((u32)0x00000001 << 18) /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_SHA_CFG             ((u32)0x00000001 << 17) /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_AES_CFG             ((u32)0x00000001 << 16) /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_SPI1                ((u32)0x00000001 << 15) /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_SPI0                ((u32)0x00000001 << 14) /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_GDMA0_CFG           ((u32)0x00000001 << 13) /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_PSRAM_PHY           ((u32)0x00000001 << 12) /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_PSRAM_SPIC_USERMODE ((u32)0x00000001 << 11) /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_SPIC_USER_MODE      ((u32)0x00000001 << 10) /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_ECC_CFG             ((u32)0x00000001 << 9)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_RMII_CFG            ((u32)0x00000001 << 8)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_SPORT0_I2S          ((u32)0x00000001 << 7)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_SDIO_HOST_DFG       ((u32)0x00000001 << 6)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_SDIO_DEVICE_CFG     ((u32)0x00000001 << 5)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_MJPEG_CFG           ((u32)0x00000001 << 4)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_LCDC_CFG            ((u32)0x00000001 << 3)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_PKE_CFG             ((u32)0x00000001 << 2)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_USB_CFG             ((u32)0x00000001 << 1)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_WIFI_CFG            ((u32)0x00000001 << 0)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
/** @} */

/** @defgroup PPC2_REG
  * @brief PPC2 configure register
  * @{
  */
#define RXI300_BIT_TIMER_0_3_BASIC ((u32)0x00000001 << 16) /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_PMC_TIMER_0_1   ((u32)0x00000001 << 15) /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_DGB_TIMER       ((u32)0x00000001 << 14) /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_THERMAL         ((u32)0x00000001 << 13) /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_IPC             ((u32)0x00000001 << 12) /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_SHA_KEY_MANAGE  ((u32)0x00000001 << 11) /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_CAP_TOUCH       ((u32)0x00000001 << 10) /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_ADC_COMP        ((u32)0x00000001 << 9)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_GPIO_A_B_C      ((u32)0x00000001 << 8)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_UART4_LOG       ((u32)0x00000001 << 7)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_UART3_BT        ((u32)0x00000001 << 6)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_UART2           ((u32)0x00000001 << 5)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_UART1           ((u32)0x00000001 << 4)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_UART0           ((u32)0x00000001 << 3)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_SYSON_NS        ((u32)0x00000001 << 2)  /*!< R/W 1h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_AES_KEY_MANAGE  ((u32)0x00000001 << 1)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_OTPC_CFG        ((u32)0x00000001 << 0)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
/** @} */

/** @defgroup PPC3_REG
  * @brief PPC3 configure register
  * @{
  */
#define RXI300_BIT_CPU_OST_CFG               ((u32)0x00000001 << 10) /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_PDM                       ((u32)0x00000001 << 9)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_I2C1                      ((u32)0x00000001 << 8)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_I2C0                      ((u32)0x00000001 << 7)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_IR                        ((u32)0x00000001 << 6)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_CAN1                      ((u32)0x00000001 << 5)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_CAN0                      ((u32)0x00000001 << 4)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_RSIP                      ((u32)0x00000001 << 3)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_RXI300                    ((u32)0x00000001 << 2)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_TRNG_POR1_PORT2           ((u32)0x00000001 << 1)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_TIMER_4_8_PULSE_PWM_TIMER ((u32)0x00000001 << 0)  /*!< R/W 0h  1: Non-secure attribution 0: Secure attribution*/
/** @} */

/** @defgroup PPC_LOCK
  * @brief PPC Lock register
  * @{
  */
#define RXI300_BIT_LOCK ((u32)0x00000001 << 0) /*!< R/W 0h  PPC lock register. When writing 1'b1 to this nit, PPC will be locked. Note: It can not be written to 1'b0 again until system reset to prevent modification. When PPC is locked, you can not program PPC registers unless system reset.*/
/** @} */

/** @defgroup MISC_ARB_R_CTRL
  * @brief The register for CPU/HPERI to SRAM/PSRAM read arbitration
  * @{
  */
#define RXI300_MASK_SHPERI_TO_SRAM_ARB   ((u32)0x000000FF << 16)           /*!< R/W F0h  Read arbitration control register. Each slot should be set by a port number. The port number represents a master group.The master group is shown as following: 0x0: LCDC 0x1: PPE And the following shows bit field of each slots: [0]: slot 0 of arbiter [1]: slot 1 of arbiter [2]: slot 2 of arbiter [3]: slot 3 of arbiter [4]: slot 4 of arbiter [5]: slot 5 of arbiter [6]: slot 6 of arbiter [7]: slot 7 of arbiter*/
#define RXI300_SHPERI_TO_SRAM_ARB(x)     (((u32)((x) & 0x000000FF) << 16))
#define RXI300_GET_SHPERI_TO_SRAM_ARB(x) ((u32)(((x >> 16) & 0x000000FF)))
#define RXI300_MASK_CPU_TO_PSRAM_ARB     ((u32)0x000000FF << 8)            /*!< R/W F0h  Read arbitration control register. Each slot should be set by a port number. The port number represents a master group.The master group is shown as following: 0x0: CPU0 0x1: CPU1 And the following shows bit field of each slots: [0]: slot 0 of arbiter [1]: slot 1 of arbiter [2]: slot 2 of arbiter [3]: slot 3 of arbiter [4]: slot 4 of arbiter [5]: slot 5 of arbiter [6]: slot 6 of arbiter [7]: slot 7 of arbiter*/
#define RXI300_CPU_TO_PSRAM_ARB(x)       (((u32)((x) & 0x000000FF) << 8))
#define RXI300_GET_CPU_TO_PSRAM_ARB(x)   ((u32)(((x >> 8) & 0x000000FF)))
#define RXI300_MASK_CPU_TO_SRAM_ARB      ((u32)0x000000FF << 0)            /*!< R/W F0h  Read arbitration control register. Each slot should be set by a port number. The port number represents a master group.The master group is shown as following: 0x0: CPU0 0x1: CPU1 And the following shows bit field of each slots: [0]: slot 0 of arbiter [1]: slot 1 of arbiter [2]: slot 2 of arbiter [3]: slot 3 of arbiter [4]: slot 4 of arbiter [5]: slot 5 of arbiter [6]: slot 6 of arbiter [7]: slot 7 of arbiter*/
#define RXI300_CPU_TO_SRAM_ARB(x)        (((u32)((x) & 0x000000FF) << 0))
#define RXI300_GET_CPU_TO_SRAM_ARB(x)    ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup SRAM_ARB_R_CTRL0
  * @brief The register for 32-bit HPERI master group to SRAM read arbitration
  * @{
  */
#define RXI300_MASK_32B_HPERI_TO_SRAM_ARB   ((u32)0x3FFFFFFF << 0)           /*!< R/W 2D91A240h  Read arbitration control register. Each slot should be set by a port number. The port number represents a master group.The master group is shown as following: 0x0: GDMA 0x1: WIFI 0x2: USB/SDIO_BT/SDIO_WIFI 0x3: OTPC/SIC 0x4: SDIO_HOST 0x5: RMII And the following shows bit field of each slots: [2:0]: slot 0 of arbiter [5:3]: slot 1 of arbiter [8:6]: slot 2 of arbiter [11:9]: slot 3 of arbiter [14:12]: slot 4 of arbiter [17:15]: slot 5 of arbiter [20:18]: slot 6 of arbiter [23:21]: slot 7 of arbiter  [26:24]: slot 8 of arbiter [29:27]: slot 9 of arbiter*/
#define RXI300_32B_HPERI_TO_SRAM_ARB(x)     (((u32)((x) & 0x3FFFFFFF) << 0))
#define RXI300_GET_32B_HPERI_TO_SRAM_ARB(x) ((u32)(((x >> 0) & 0x3FFFFFFF)))
/** @} */

/** @defgroup SRAM_ARB_R_CTRL1
  * @brief The register for 32-bit HPERI master group to SRAM read arbitration
  * @{
  */
#define RXI300_MASK_32B_HPERI_TO_SRAM_ARB   ((u32)0x3FFFFFFF << 0)           /*!< R/W 2D91A240h  This is extended register of 32B_HPERI_TO_SRAM_ARB [2:0]: slot 0 of arbiter [5:3]: slot 1 of arbiter [8:6]: slot 2 of arbiter [11:9]: slot 3 of arbiter [14:12]: slot 4 of arbiter [17:15]: slot 5 of arbiter [20:18]: slot 6 of arbiter [23:21]: slot 7 of arbiter  [26:24]: slot 8 of arbiter [29:27]: slot 9 of arbiter*/
#define RXI300_32B_HPERI_TO_SRAM_ARB(x)     (((u32)((x) & 0x3FFFFFFF) << 0))
#define RXI300_GET_32B_HPERI_TO_SRAM_ARB(x) ((u32)(((x >> 0) & 0x3FFFFFFF)))
/** @} */

/** @defgroup SRAM_ARB_R_CTRL2
  * @brief The register for CPU/LCDC/HPERI master group to SRAM read arbitration
  * @{
  */
#define RXI300_MASK_SRAM_FINAL_ARB          ((u32)0x0000FFFF << 16)           /*!< R/W A50h  Read arbitration control register. Each slot should be set by a port number. The port number represents a master group.The master group is shown as following: 0x0: CPU_TO_SRAM_ARB group 0x1: ALL_HPERI_TO_SRAM_ARB group 0x2: SHPERI_TO_SRAM_ARB group And the following shows bit field of each slots: [1:0]: slot 0 of arbiter [3:2]: slot 1 of arbiter [5:4]: slot 2 of arbiter [7:6]: slot 3 of arbiter [9:8]: slot 4 of arbiter [11:10]: slot 5 of arbiter [13:12]: slot 6 of arbiter [15:14]: slot 7 of arbiter*/
#define RXI300_SRAM_FINAL_ARB(x)            (((u32)((x) & 0x0000FFFF) << 16))
#define RXI300_GET_SRAM_FINAL_ARB(x)        ((u32)(((x >> 16) & 0x0000FFFF)))
#define RXI300_MASK_ALL_HPERI_TO_SRAM_ARB   ((u32)0x000000FF << 8)            /*!< R/W F0h  Read arbitration control register. Each slot should be set by a port number. The port number represents a master group.The master group is shown as following: 0x0: 32B_HPERI_TO_SRAM_ARB group 0x1: 64B_HPERI_TO_SRAM_ARB group And the following shows bit field of each slots: [0]: slot 0 of arbiter [1]: slot 1 of arbiter [2]: slot 2 of arbiter [3]: slot 3 of arbiter [4]: slot 4 of arbiter [5]: slot 5 of arbiter [6]: slot 6 of arbiter [7]: slot 7 of arbiter*/
#define RXI300_ALL_HPERI_TO_SRAM_ARB(x)     (((u32)((x) & 0x000000FF) << 8))
#define RXI300_GET_ALL_HPERI_TO_SRAM_ARB(x) ((u32)(((x >> 8) & 0x000000FF)))
#define RXI300_MASK_64B_HPERI_TO_SRAM_ARB   ((u32)0x000000FF << 0)            /*!< R/W F0h  Read arbitration control register. Each slot should be set by a port number. The port number represents a master group.The master group is shown as following: 0x0: AES_SHA 0x1: MJPEG And the following shows bit field of each slots: [0]: slot 0 of arbiter [1]: slot 1 of arbiter [2]: slot 2 of arbiter [3]: slot 3 of arbiter [4]: slot 4 of arbiter [5]: slot 5 of arbiter [6]: slot 6 of arbiter [7]: slot 7 of arbiter*/
#define RXI300_64B_HPERI_TO_SRAM_ARB(x)     (((u32)((x) & 0x000000FF) << 0))
#define RXI300_GET_64B_HPERI_TO_SRAM_ARB(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup PSRAM_ARB_R_CTRL0
  * @brief The register for no performance requirement master group to PSRAM read arbitration
  * @{
  */
#define RXI300_MASK_HPERI_TO_PSRAM_ARB   ((u32)0x3FFFFFFF << 0)           /*!< R/W 246D2240h  Read arbitration control register. Each slot should be set by a port number. The port number represents a master group.The master group is shown as following: 0x0: GDMA 0x1: OTPC/SIC 0x2: AED_SHA                                                                                                                                                                                                      0x3: SDIO_HOST 0x4: RMII And the following shows bit field of each slots: [2:0]: slot 0 of arbiter [5:3]: slot 1 of arbiter [8:6]: slot 2 of arbiter [11:9]: slot 3 of arbiter [14:12]: slot 4 of arbiter [17:15]: slot 5 of arbiter [20:18]: slot 6 of arbiter [23:21]: slot 7 of arbiter  [26:24]: slot 8 of arbiter [29:27]: slot 9 of arbiter*/
#define RXI300_HPERI_TO_PSRAM_ARB(x)     (((u32)((x) & 0x3FFFFFFF) << 0))
#define RXI300_GET_HPERI_TO_PSRAM_ARB(x) ((u32)(((x >> 0) & 0x3FFFFFFF)))
/** @} */

/** @defgroup PSRAM_ARB_R_CTRL1
  * @brief The register for  PSRAM read arbitration
  * @{
  */
#define RXI300_MASK_R_CTRL1_PSRAM_FINAL_ARB   ((u32)0x3FFFFFFF << 0)           /*!< R/W 1A489200h  Read arbitration control register. Each slot should be set by a port number. The port number represents a master group.The master group is shown as following: 0x0: CPU0/CPU1 0x1: HPERI_TO_PSRAM_ARB group 0x2: WIFI                                                                                                                                                                                                                0x3: USB 0x4: MJPEG 0x5: LCDC 0x6: PPE                                                                                                                                                                                                       And the following shows bit field of each slots: [2:0]: slot 0 of arbiter [5:3]: slot 1 of arbiter [8:6]: slot 2 of arbiter [11:9]: slot 3 of arbiter [14:12]: slot 4 of arbiter [17:15]: slot 5 of arbiter [20:18]: slot 6 of arbiter [23:21]: slot 7 of arbiter  [26:24]: slot 8 of arbiter [29:27]: slot 9 of arbiter*/
#define RXI300_R_CTRL1_PSRAM_FINAL_ARB(x)     (((u32)((x) & 0x3FFFFFFF) << 0))
#define RXI300_GET_R_CTRL1_PSRAM_FINAL_ARB(x) ((u32)(((x >> 0) & 0x3FFFFFFF)))
/** @} */

/** @defgroup PSRAM_ARB_R_CTRL2
  * @brief The register for  PSRAM read arbitration
  * @{
  */
#define RXI300_MASK_R_CTRL2_PSRAM_FINAL_ARB   ((u32)0x3FFFFFFF << 0)           /*!< R/W 36B6C91Bh  This is extended register of PSRAM_FINAL_ARB slot [2:0]: slot 10 of arbiter [5:3]: slot 11 of arbiter [8:6]: slot 12 of arbiter [11:9]: slot 13 of arbiter [14:12]: slot 14 of arbiter [17:15]: slot 15 of arbiter [20:18]: slot 16 of arbiter [23:21]: slot 17 of arbiter  [26:24]: slot 18 of arbiter [29:27]: slot 19 of arbiter*/
#define RXI300_R_CTRL2_PSRAM_FINAL_ARB(x)     (((u32)((x) & 0x3FFFFFFF) << 0))
#define RXI300_GET_R_CTRL2_PSRAM_FINAL_ARB(x) ((u32)(((x >> 0) & 0x3FFFFFFF)))
/** @} */

/** @defgroup PSRAM_ARB_R_CTRL3_RD_WEIGHT
  * @brief The register for  PSRAM read arbitration and weight control of SRAM controller
  * @{
  */
#define RXI300_MASK_RD_WEIGHT_BUFFER   ((u32)0x000001FF << 21)           /*!< R/W 4h  Read CMD weight of buffer_ram*/
#define RXI300_RD_WEIGHT_BUFFER(x)     (((u32)((x) & 0x000001FF) << 21))
#define RXI300_GET_RD_WEIGHT_BUFFER(x) ((u32)(((x >> 21) & 0x000001FF)))
#define RXI300_MASK_RD_WEIGHT_SRAM     ((u32)0x000001FF << 12)           /*!< R/W 2h  Read CMD weight of hs_sram0*/
#define RXI300_RD_WEIGHT_SRAM(x)       (((u32)((x) & 0x000001FF) << 12))
#define RXI300_GET_RD_WEIGHT_SRAM(x)   ((u32)(((x >> 12) & 0x000001FF)))
#define RXI300_MASK_PSRAM_ARB_CTRL1    ((u32)0x00000FFF << 0)            /*!< R/W 6h  This is extended register of PSRAM_ARB_CTRL0 slot. [3:0]: slot 8 of arbiter [7:4]: slot 9 of arbiter [11:8]: slot 10 of arbiter [15:12]: slot 11 of arbiter [19:16]: slot 12 of arbiter [23:20]: slot 13 of arbiter [27:24]: slot 14 of arbiter [31:28]: slot 15 of arbiter*/
#define RXI300_PSRAM_ARB_CTRL1(x)      (((u32)((x) & 0x00000FFF) << 0))
#define RXI300_GET_PSRAM_ARB_CTRL1(x)  ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup MISC_ARB_W_CTRL
  * @brief The register for CPU/SHPERI to SRAM/PSRAM write arbitration
  * @{
  */
#define RXI300_MASK_SHPERI_TO_SRAM_ARB   ((u32)0x000000FF << 16)           /*!< R/W F0h  Write arbitration control register. Each slot should be set by a port number. The port number represents a master group.The master group is shown as following: 0x0: LCDC 0x1: PPE And the following shows bit field of each slots: [0]: slot 0 of arbiter [1]: slot 1 of arbiter [2]: slot 2 of arbiter [3]: slot 3 of arbiter [4]: slot 4 of arbiter [5]: slot 5 of arbiter [6]: slot 6 of arbiter [7]: slot 7 of arbiter*/
#define RXI300_SHPERI_TO_SRAM_ARB(x)     (((u32)((x) & 0x000000FF) << 16))
#define RXI300_GET_SHPERI_TO_SRAM_ARB(x) ((u32)(((x >> 16) & 0x000000FF)))
#define RXI300_MASK_CPU_TO_PSRAM_ARB     ((u32)0x000000FF << 8)            /*!< R/W F0h  Write arbitration control register. Each slot should be set by a port number. The port number represents a master group.The master group is shown as following: 0x0: CPU0 0x1: CPU1 And the following shows bit field of each slots: [0]: slot 0 of arbiter [1]: slot 1 of arbiter [2]: slot 2 of arbiter [3]: slot 3 of arbiter [4]: slot 4 of arbiter [5]: slot 5 of arbiter [6]: slot 6 of arbiter [7]: slot 7 of arbiter*/
#define RXI300_CPU_TO_PSRAM_ARB(x)       (((u32)((x) & 0x000000FF) << 8))
#define RXI300_GET_CPU_TO_PSRAM_ARB(x)   ((u32)(((x >> 8) & 0x000000FF)))
#define RXI300_MASK_CPU_TO_SRAM_ARB      ((u32)0x000000FF << 0)            /*!< R/W F0h  Write arbitration control register. Each slot should be set by a port number. The port number represents a master group.The master group is shown as following: 0x0: CPU0 0x1: CPU1 And the following shows bit field of each slots: [0]: slot 0 of arbiter [1]: slot 1 of arbiter [2]: slot 2 of arbiter [3]: slot 3 of arbiter [4]: slot 4 of arbiter [5]: slot 5 of arbiter [6]: slot 6 of arbiter [7]: slot 7 of arbiter*/
#define RXI300_CPU_TO_SRAM_ARB(x)        (((u32)((x) & 0x000000FF) << 0))
#define RXI300_GET_CPU_TO_SRAM_ARB(x)    ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup SRAM_ARB_W_CTRL0
  * @brief The register 32-bit HPERI master group  to SRAM write arbitration
  * @{
  */
#define RXI300_MASK_32B_HPERI_TO_SRAM_ARB   ((u32)0x3FFFFFFF << 0)           /*!< R/W 2D91A240h  Write arbitration control register. Each slot should be set by a port number. The port number represents a master group.The master group is shown as following: 0x0: GDMA 0x1: WIFI 0x2: USB/SDIO_BT/SDIO_WIFI 0x3: OTPC/SIC 0x4: SDIO_HOST 0x5: RMII And the following shows bit field of each slots: [2:0]: slot 0 of arbiter [5:3]: slot 1 of arbiter [8:6]: slot 2 of arbiter [11:9]: slot 3 of arbiter [14:12]: slot 4 of arbiter [17:15]: slot 5 of arbiter [20:18]: slot 6 of arbiter [23:21]: slot 7 of arbiter  [26:24]: slot 8 of arbiter [29:27]: slot 9 of arbiter*/
#define RXI300_32B_HPERI_TO_SRAM_ARB(x)     (((u32)((x) & 0x3FFFFFFF) << 0))
#define RXI300_GET_32B_HPERI_TO_SRAM_ARB(x) ((u32)(((x >> 0) & 0x3FFFFFFF)))
/** @} */

/** @defgroup SRAM_ARB_W_CTRL1
  * @brief The register 32-bit HPERI master group  to SRAM write arbitration
  * @{
  */
#define RXI300_MASK_32B_HPERI_TO_SRAM_ARB   ((u32)0x3FFFFFFF << 0)           /*!< R/W 2D91A240h  This is extended register of 32B_HPERI_TO_SRAM_ARB [2:0]: slot 10 of arbiter [5:3]: slot 11 of arbiter [8:6]: slot 12 of arbiter [11:9]: slot 13 of arbiter [14:12]: slot 14 of arbiter [17:15]: slot 15 of arbiter [20:18]: slot 16 of arbiter [23:21]: slot 17 of arbiter  [26:24]: slot 18 of arbiter [29:27]: slot 19 of arbiter*/
#define RXI300_32B_HPERI_TO_SRAM_ARB(x)     (((u32)((x) & 0x3FFFFFFF) << 0))
#define RXI300_GET_32B_HPERI_TO_SRAM_ARB(x) ((u32)(((x >> 0) & 0x3FFFFFFF)))
/** @} */

/** @defgroup SRAM_ARB_W_CTRL2
  * @brief The register for CPU/LCD/ HPERI master group  to SRAM write arbitration
  * @{
  */
#define RXI300_MASK_SRAM_FINAL_ARB          ((u32)0x0000FFFF << 16)           /*!< R/W A50h  Write arbitration control register. Each slot should be set by a port number. The port number represents a master group.The master group is shown as following: 0x0: CPU_TO_SRAM_ARB group 0x1: ALL_HPERI_TO_SRAM_ARB group 0x2: SHPERI_TO_SRAM_ARB group And the following shows bit field of each slots: [1:0]: slot 0 of arbiter [3:2]: slot 1 of arbiter [5:4]: slot 2 of arbiter [7:6]: slot 3 of arbiter [9:8]: slot 4 of arbiter [11:10]: slot 5 of arbiter [13:12]: slot 6 of arbiter [15:14]: slot 7 of arbiter*/
#define RXI300_SRAM_FINAL_ARB(x)            (((u32)((x) & 0x0000FFFF) << 16))
#define RXI300_GET_SRAM_FINAL_ARB(x)        ((u32)(((x >> 16) & 0x0000FFFF)))
#define RXI300_MASK_ALL_HPERI_TO_SRAM_ARB   ((u32)0x000000FF << 8)            /*!< R/W F0h  Write arbitration control register. Each slot should be set by a port number. The port number represents a master group.The master group is shown as following: 0x0: 32B_HPERI_TO_SRAM_ARB group 0x1: 64B_HPERI_TO_SRAM_ARB group And the following shows bit field of each slots: [0]: slot 0 of arbiter [1]: slot 1 of arbiter [2]: slot 2 of arbiter [3]: slot 3 of arbiter [4]: slot 4 of arbiter [5]: slot 5 of arbiter [6]: slot 6 of arbiter [7]: slot 7 of arbiter*/
#define RXI300_ALL_HPERI_TO_SRAM_ARB(x)     (((u32)((x) & 0x000000FF) << 8))
#define RXI300_GET_ALL_HPERI_TO_SRAM_ARB(x) ((u32)(((x >> 8) & 0x000000FF)))
#define RXI300_MASK_64B_HPERI_TO_SRAM_ARB   ((u32)0x000000FF << 0)            /*!< R/W F0h  Write arbitration control register. Each slot should be set by a port number. The port number represents a master group.The master group is shown as following: 0x0: AES_SHA 0x1: MJPEG And the following shows bit field of each slots: [0]: slot 0 of arbiter [1]: slot 1 of arbiter [2]: slot 2 of arbiter [3]: slot 3 of arbiter [4]: slot 4 of arbiter [5]: slot 5 of arbiter [6]: slot 6 of arbiter [7]: slot 7 of arbiter*/
#define RXI300_64B_HPERI_TO_SRAM_ARB(x)     (((u32)((x) & 0x000000FF) << 0))
#define RXI300_GET_64B_HPERI_TO_SRAM_ARB(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup PSRAM_ARB_W_CTRL0
  * @brief The register for performance requirement master group to PSRAM write arbitration
  * @{
  */
#define RXI300_MASK_HPERI_TO_PSRAM_ARB   ((u32)0x3FFFFFFF << 0)           /*!< R/W 246D2240h  Write arbitration control register. Each slot should be set by a port number. The port number represents a master group.The master group is shown as following: 0x0: GDMA 0x1: OTPC/SIC 0x2: AED_SHA                                                                                                                                                                                                                                      0x3: SDIO_HOST 0x4: RMII And the following shows bit field of each slots: [2:0]: slot 0 of arbiter [5:3]: slot 1 of arbiter [8:6]: slot 2 of arbiter [11:9]: slot 3 of arbiter [14:12]: slot 4 of arbiter [17:15]: slot 5 of arbiter [20:18]: slot 6 of arbiter [23:21]: slot 7 of arbiter  [26:24]: slot 8 of arbiter [29:27]: slot 9 of arbiter*/
#define RXI300_HPERI_TO_PSRAM_ARB(x)     (((u32)((x) & 0x3FFFFFFF) << 0))
#define RXI300_GET_HPERI_TO_PSRAM_ARB(x) ((u32)(((x >> 0) & 0x3FFFFFFF)))
/** @} */

/** @defgroup PSRAM_ARB_W_CTRL1
  * @brief The register for PSRAM write arbitration
  * @{
  */
#define RXI300_MASK_W_CTRL1_PSRAM_FINAL_ARB   ((u32)0x3FFFFFFF << 0)           /*!< R/W 1A489200h  Write arbitration control register. Each slot should be set by a port number. The port number represents a master group.The master group is shown as following: 0x0: CPU0/CPU1 0x1: HPERI_TO_PSRAM_ARB group 0x2: WIFI                                                                                                                                                                                                                                               0x3: USB 0x4: MJPEG 0x5: LCDC 0x6: PPE                                                                                                                                                                                                       And the following shows bit field of each slots: [2:0]: slot 0 of arbiter [5:3]: slot 1 of arbiter [8:6]: slot 2 of arbiter [11:9]: slot 3 of arbiter [14:12]: slot 4 of arbiter [17:15]: slot 5 of arbiter [20:18]: slot 6 of arbiter [23:21]: slot 7 of arbiter  [26:24]: slot 8 of arbiter [29:27]: slot 9 of arbiter*/
#define RXI300_W_CTRL1_PSRAM_FINAL_ARB(x)     (((u32)((x) & 0x3FFFFFFF) << 0))
#define RXI300_GET_W_CTRL1_PSRAM_FINAL_ARB(x) ((u32)(((x >> 0) & 0x3FFFFFFF)))
/** @} */

/** @defgroup PSRAM_ARB_W_CTRL2
  * @brief The register for PSRAM write arbitration
  * @{
  */
#define RXI300_MASK_W_CTRL2_PSRAM_FINAL_ARB   ((u32)0x3FFFFFFF << 0)           /*!< R/W 36B6C91Bh  This is extended register of PSRAM_FINAL_ARB slot [2:0]: slot 10 of arbiter [5:3]: slot 11 of arbiter [8:6]: slot 12 of arbiter [11:9]: slot 13 of arbiter [14:12]: slot 14 of arbiter [17:15]: slot 15 of arbiter [20:18]: slot 16 of arbiter [23:21]: slot 17 of arbiter  [26:24]: slot 18 of arbiter [29:27]: slot 19 of arbiter*/
#define RXI300_W_CTRL2_PSRAM_FINAL_ARB(x)     (((u32)((x) & 0x3FFFFFFF) << 0))
#define RXI300_GET_W_CTRL2_PSRAM_FINAL_ARB(x) ((u32)(((x >> 0) & 0x3FFFFFFF)))
/** @} */

/** @defgroup PSRAM_ARB_W_CTRL3
  * @brief The register for PSRAM write arbitration
  * @{
  */
#define RXI300_MASK_W_CTRL3_PSRAM_FINAL_ARB   ((u32)0x00000FFF << 0)           /*!< R/W 6h  This is extended register of PSRAM_FINAL_ARB slot. [2:0]: slot 20 of arbiter [5:3]: slot 21 of arbiter [8:6]: slot 22 of arbiter [11:9]: slot 23 of arbiter*/
#define RXI300_W_CTRL3_PSRAM_FINAL_ARB(x)     (((u32)((x) & 0x00000FFF) << 0))
#define RXI300_GET_W_CTRL3_PSRAM_FINAL_ARB(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ELR_PLD0
  * @brief Error log payload register0
  * @{
  */
#define RXI300_MASK_ELR_BSTINDEX   ((u32)0x000000FF << 24)           /*!< R 0h  Burst index of the error request.*/
#define RXI300_ELR_BSTINDEX(x)     (((u32)((x) & 0x000000FF) << 24))
#define RXI300_GET_ELR_BSTINDEX(x) ((u32)(((x >> 24) & 0x000000FF)))
#define RXI300_MASK_ELR_BSTLEN     ((u32)0x000000FF << 16)           /*!< R 0h  Burst Length of the error request.*/
#define RXI300_ELR_BSTLEN(x)       (((u32)((x) & 0x000000FF) << 16))
#define RXI300_GET_ELR_BSTLEN(x)   ((u32)(((x >> 16) & 0x000000FF)))
#define RXI300_MASK_ELR_BSTTYPE    ((u32)0x00000007 << 11)           /*!< R 0h  BurstSeq of the error request.*/
#define RXI300_ELR_BSTTYPE(x)      (((u32)((x) & 0x00000007) << 11))
#define RXI300_GET_ELR_BSTTYPE(x)  ((u32)(((x >> 11) & 0x00000007)))
#define RXI300_MASK_ELR_CMD        ((u32)0x00000007 << 8)            /*!< R 0h  CMD of the error request. 0x1: Write command 0x2: Read command*/
#define RXI300_ELR_CMD(x)          (((u32)((x) & 0x00000007) << 8))
#define RXI300_GET_ELR_CMD(x)      ((u32)(((x >> 8) & 0x00000007)))
#define RXI300_MASK_ELR_SRC        ((u32)0x000000FF << 0)            /*!< R 0h  Error source of the error request. 0x90: timeout default slave of S6 group*/
#define RXI300_ELR_SRC(x)          (((u32)((x) & 0x000000FF) << 0))
#define RXI300_GET_ELR_SRC(x)      ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup ELR_PLD1
  * @brief Error log payload register1
  * @{
  */
#define RXI300_MASK_ELR_LOCK    ((u32)0x00000003 << 30)           /*!< R 0h  AxLock of the error request.*/
#define RXI300_ELR_LOCK(x)      (((u32)((x) & 0x00000003) << 30))
#define RXI300_GET_ELR_LOCK(x)  ((u32)(((x >> 30) & 0x00000003)))
#define RXI300_MASK_ELR_CACHE   ((u32)0x0000000F << 26)           /*!< R 0h  AxCache of the error request.*/
#define RXI300_ELR_CACHE(x)     (((u32)((x) & 0x0000000F) << 26))
#define RXI300_GET_ELR_CACHE(x) ((u32)(((x >> 26) & 0x0000000F)))
#define RXI300_MASK_ELR_PROT    ((u32)0x00000007 << 23)           /*!< R 0h  AxProt of the error request.*/
#define RXI300_ELR_PROT(x)      (((u32)((x) & 0x00000007) << 23))
#define RXI300_GET_ELR_PROT(x)  ((u32)(((x >> 23) & 0x00000007)))
#define RXI300_MASK_ELR_SIZE    ((u32)0x00000007 << 16)           /*!< R 0h  Size of the error request.*/
#define RXI300_ELR_SIZE(x)      (((u32)((x) & 0x00000007) << 16))
#define RXI300_GET_ELR_SIZE(x)  ((u32)(((x >> 16) & 0x00000007)))
/** @} */

/** @defgroup ELR_ID
  * @brief Error log ID information register
  * @{
  */
#define RXI300_MASK_ELR_ID   ((u32)0xFFFFFFFF << 0)           /*!< R 0h  Tagid of the error request. 9'b00001????: M1 access error 9'b000100110: M5-0 access error 9'b000100111: M5-1 access error*/
#define RXI300_ELR_ID(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_ELR_ID(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_ADR0
  * @brief Error log addrress register0
  * @{
  */
#define RXI300_MASK_ELR_ADR0   ((u32)0xFFFFFFFF << 0)           /*!< R 0h  Address[31:0] of the error request.*/
#define RXI300_ELR_ADR0(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_ELR_ADR0(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_ADR1
  * @brief Error log addrress register1
  * @{
  */
#define RXI300_MASK_ELR_ADR1   ((u32)0xFFFFFFFF << 32)           /*!< R 0h  Address[63:32] of the error request.*/
#define RXI300_ELR_ADR1(x)     (((u32)((x) & 0xFFFFFFFF) << 32))
#define RXI300_GET_ELR_ADR1(x) ((u32)(((x >> 32) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_CODE
  * @brief Error code register
  * @{
  */
#define RXI300_MASK_ELR_CODE   ((u32)0x000000FF << 0)           /*!< R 0h  The error code of error request, if it's 0, indicates that there is no error request.  8'h00: No error 8'h01: Address hole error*/
#define RXI300_ELR_CODE(x)     (((u32)((x) & 0x000000FF) << 0))
#define RXI300_GET_ELR_CODE(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup ELR_INTR_CLR
  * @brief Error log clear register
  * @{
  */
#define RXI300_BIT_ELR_INTR_CLR ((u32)0x00000001 << 0) /*!< W 0h  Error log interrupt clear. Writing 1'b1 to the register will trigger a 1-cycle pulse interrupt clear. It has to write 1'b1 to this register every time when the error exception occurs. Note that this register is write-only, the return value of reading this register is meaningless.*/
/** @} */

/** @defgroup TIMEOUT_GUARD_EN
  * @brief Timeout guardian enable register
  * @{
  */
#define RXI300_MASK_GUARDIAN_MODE_EN   ((u32)0x000000FF << 0)           /*!< R/W 0h  The enable of timeout monitor group                                                                                                                                      0x0: Disable timeout check feature 0x1: Enable timeoute check feature*/
#define RXI300_GUARDIAN_MODE_EN(x)     (((u32)((x) & 0x000000FF) << 0))
#define RXI300_GET_GUARDIAN_MODE_EN(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup TIMEOUT_CLEAN_N
  * @brief Timeout clear register
  * @{
  */
#define RXI300_BIT_TIMEOUT_CLEAR_N ((u32)0x00000001 << 0) /*!< R/W 1h  The register can only written to 0 when a time out occures, and automatically clear this register by hardware*/
/** @} */

/** @defgroup TIMEOUT_MON_CFG
  * @brief Timeout monitor configuration register0
  * @{
  */
#define RXI300_MASK_GRADE       ((u32)0x0000000F << 16)           /*!< R/W Fh  Timeout grade                                                                                                                                                                                                                                      Timeout total cycle = [256*(2^(grade-1) +1)*threshold] - inaccurate                                                                                                       Note: the range of inaccurate = 0 ~ [256*(2^(grade-1) +1)-2]*/
#define RXI300_GRADE(x)         (((u32)((x) & 0x0000000F) << 16))
#define RXI300_GET_GRADE(x)     ((u32)(((x >> 16) & 0x0000000F)))
#define RXI300_MASK_THRESHOLD   ((u32)0x000000FF << 0)            /*!< R/W FFh  Timeout threshold*/
#define RXI300_THRESHOLD(x)     (((u32)((x) & 0x000000FF) << 0))
#define RXI300_GET_THRESHOLD(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup PREFETCH_CTRL
  * @brief Prefetch control setting of stream buffer
  * @{
  */
#define RXI300_MASK_DETECT_NUM_SINGLE   ((u32)0x0000000F << 20)           /*!< R/W 3h  The number of required sequential single accesses before initiating prefetch*/
#define RXI300_DETECT_NUM_SINGLE(x)     (((u32)((x) & 0x0000000F) << 20))
#define RXI300_GET_DETECT_NUM_SINGLE(x) ((u32)(((x >> 20) & 0x0000000F)))
#define RXI300_MASK_DETECT_NUM_BURST    ((u32)0x0000000F << 16)           /*!< R/W 3h  The number of required sequential cache line accesses before initiating prefetch*/
#define RXI300_DETECT_NUM_BURST(x)      (((u32)((x) & 0x0000000F) << 16))
#define RXI300_GET_DETECT_NUM_BURST(x)  ((u32)(((x >> 16) & 0x0000000F)))
#define RXI300_MASK_PREFETCH_DIST       ((u32)0x00000003 << 8)            /*!< R/W 2h  The size of prefech data                                                                                                                                        2'b00: no prefetch 2'h01: one cache line                                                                                                                                                                                        2'h1?: two cache line*/
#define RXI300_PREFETCH_DIST(x)         (((u32)((x) & 0x00000003) << 8))
#define RXI300_GET_PREFETCH_DIST(x)     ((u32)(((x >> 8) & 0x00000003)))
#define RXI300_MASK_REMAIN_ENTRY        ((u32)0x0000001F << 0)            /*!< R/W 8h  The remaining number of valid entries in the stream buffer required before automatically intiating prefetch.(support up to 16)*/
#define RXI300_REMAIN_ENTRY(x)          (((u32)((x) & 0x0000001F) << 0))
#define RXI300_GET_REMAIN_ENTRY(x)      ((u32)(((x >> 0) & 0x0000001F)))
/** @} */

/** @defgroup ADDR_REGION
  * @brief The address region that requires prefetch
  * @{
  */
#define RXI300_MASK_ADDR_TOP    ((u32)0x0000FFFF << 16)           /*!< R/W 6FFFh  The top address boundary of prefetch. (only consider address[31:16])*/
#define RXI300_ADDR_TOP(x)      (((u32)((x) & 0x0000FFFF) << 16))
#define RXI300_GET_ADDR_TOP(x)  ((u32)(((x >> 16) & 0x0000FFFF)))
#define RXI300_MASK_ADDR_BASE   ((u32)0x0000FFFF << 0)            /*!< R/W 6000h  The base address boundary of prefetch. (only consider address[31:16])*/
#define RXI300_ADDR_BASE(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define RXI300_GET_ADDR_BASE(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup EVENT_CTRL
  * @brief Enable event counter
  * @{
  */
#define RXI300_BIT_EVENT_CTRL_EN ((u32)0x00000001 << 0) /*!< R/W 1h  Enable event counter.*/
/** @} */

/** @defgroup EVENT_CMD_CNT
  * @brief The number of master conmmand
  * @{
  */
#define RXI300_MASK_CMD_NUM   ((u32)0x00FFFFFF << 0)           /*!< R 0h  The number of master command*/
#define RXI300_CMD_NUM(x)     (((u32)((x) & 0x00FFFFFF) << 0))
#define RXI300_GET_CMD_NUM(x) ((u32)(((x >> 0) & 0x00FFFFFF)))
/** @} */

/** @defgroup EVENT_PREFETCH_CNT
  * @brief The number of prefetch
  * @{
  */
#define RXI300_MASK_PREFETCH_NUM   ((u32)0x00FFFFFF << 0)           /*!< R 0h  The number of prefetch*/
#define RXI300_PREFETCH_NUM(x)     (((u32)((x) & 0x00FFFFFF) << 0))
#define RXI300_GET_PREFETCH_NUM(x) ((u32)(((x >> 0) & 0x00FFFFFF)))
/** @} */

/** @defgroup EVENT_PREFETCH_HIT_CNT
  * @brief The number of cache line hits after prefetch
  * @{
  */
#define RXI300_MASK_PREFETCH_HIT   ((u32)0x00FFFFFF << 0)           /*!< R 0h  The number of transfer data is not immediately returned after cache line hits in ther stream buffer*/
#define RXI300_PREFETCH_HIT(x)     (((u32)((x) & 0x00FFFFFF) << 0))
#define RXI300_GET_PREFETCH_HIT(x) ((u32)(((x >> 0) & 0x00FFFFFF)))
/** @} */

/** @defgroup EVENT_DATA_DELAY_CNT
  * @brief The number of transfer data is not immediately returned after cache line hits in the stream buffer
  * @{
  */
#define RXI300_MASK_DATA_DELAY   ((u32)0x00FFFFFF << 0)           /*!< R 0h  The number of cache line hits after prefetch*/
#define RXI300_DATA_DELAY(x)     (((u32)((x) & 0x00FFFFFF) << 0))
#define RXI300_GET_DATA_DELAY(x) ((u32)(((x >> 0) & 0x00FFFFFF)))
/** @} */

/** @defgroup FLUSH_DONE
  * @brief Indicate flush is complete
  * @{
  */
#define RXI300_BIT_FLUSH_DONE ((u32)0x00000001 << 0) /*!< R 0h  Indicate flush is complete*/
/** @} */

/** @defgroup FLUSH
  * @brief Flush FIFO or event counter
  * @{
  */
#define RXI300_BIT_EVENT_CNT_CLEAR ((u32)0x00000001 << 8) /*!< W 0h  Clear event counter*/
#define RXI300_BIT_ELR_INTR_CLR    ((u32)0x00000001 << 0) /*!< W 0h  Flush FIFO*/
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup RXI300_Exported_Types RXI300 Exported Types
  * @{
  */

/** @brief RXI300 Port
  */
typedef struct {
	__IO uint32_t IDAU_BAx;  /*!< BASE ADDRESS REGISTER x IN IDAU Register */
	__IO uint32_t IDAU_LAx;  /*!< TOP ADDRESS REGISTER x IN IDAU Register */
} MPC_EntryTypeDef;

/** @brief RXI300 Port
  */
typedef struct {
	__IO uint32_t PPC1_REG;  /*!< PPC1 CONFIGURE REGISTER0 Register */
	__IO uint32_t PPC2_REG;  /*!< PPC2 CONFIGURE REGISTER */
	__IO uint32_t PPC3_REG;  /*!< PPC3 CONFIGURE REGISTER */
	__IO uint32_t PPC_LOCK;  /*!< PPC LOCK REGISTER */
} PPC_TypeDef;

/** @brief RXI300 Port
  */
typedef struct {
	__IO uint32_t MISC_ARB_R_CTRL            ;  /*!< THE REGISTER FOR CPU/HPERI TO SRAM/PSRAM READ ARBITRATION Register */
	__IO uint32_t SRAM_ARB_R_CTRL0           ;  /*!< THE REGISTER FOR 32-BIT HPERI MASTER GROUP TO SRAM READ ARBITRATION Register */
	__IO uint32_t SRAM_ARB_R_CTRL1           ;  /*!< THE REGISTER FOR 32-BIT HPERI MASTER GROUP TO SRAM READ ARBITRATION Register */
	__IO uint32_t SRAM_ARB_R_CTRL2           ;  /*!< THE REGISTER FOR CPU/LCDC/HPERI MASTER GROUP TO SRAM READ ARBITRATION Register */
	__IO uint32_t PSRAM_ARB_R_CTRL0          ;  /*!< THE REGISTER FOR NO PERFORMANCE REQUIREMENT MASTER GROUP TO PSRAM READ ARBITRATION Register */
	__IO uint32_t PSRAM_ARB_R_CTRL1          ;  /*!< THE REGISTER FOR  PSRAM READ ARBITRATION Register */
	__IO uint32_t PSRAM_ARB_R_CTRL2          ;  /*!< THE REGISTER FOR  PSRAM READ ARBITRATION Register */
	__IO uint32_t PSRAM_ARB_R_CTRL3_RD_WEIGHT;  /*!< THE REGISTER FOR  PSRAM READ ARBITRATION AND WEIGHT CONTROL OF SRAM CONTROLLER Register */
	__IO uint32_t RSVD0[8]                   ;  /*!< Reserved */
	__IO uint32_t MISC_ARB_W_CTRL            ;  /*!< THE REGISTER FOR CPU/SHPERI TO SRAM/PSRAM WRITE ARBITRATION Register */
	__IO uint32_t SRAM_ARB_W_CTRL0           ;  /*!< THE REGISTER 32-BIT HPERI MASTER GROUP  TO SRAM WRITE ARBITRATION Register */
	__IO uint32_t SRAM_ARB_W_CTRL1           ;  /*!< THE REGISTER 32-BIT HPERI MASTER GROUP  TO SRAM WRITE ARBITRATION Register */
	__IO uint32_t SRAM_ARB_W_CTRL2           ;  /*!< THE REGISTER FOR CPU/LCD/ HPERI MASTER GROUP  TO SRAM WRITE ARBITRATION Register */
	__IO uint32_t PSRAM_ARB_W_CTRL0          ;  /*!< THE REGISTER FOR PERFORMANCE REQUIREMENT MASTER GROUP TO PSRAM WRITE ARBITRATION Register */
	__IO uint32_t PSRAM_ARB_W_CTRL1          ;  /*!< THE REGISTER FOR PSRAM WRITE ARBITRATION Register */
	__IO uint32_t PSRAM_ARB_W_CTRL2          ;  /*!< THE REGISTER FOR PSRAM WRITE ARBITRATION Register */
	__IO uint32_t PSRAM_ARB_W_CTRL3          ;  /*!< THE REGISTER FOR PSRAM WRITE ARBITRATION Register */
} RXI300_WRR_TypeDef;

/** @brief RXI300 Port
  */
typedef struct {
	__IO uint32_t PREFETCH_CTRL         ;  /*!< PREFETCH CONTROL SETTING OF STREAM BUFFER Register */
	__IO uint32_t ADDR_REGION           ;  /*!< THE ADDRESS REGION THAT REQUIRES PREFETCH Register */
	__IO uint32_t EVENT_CTRL            ;  /*!< ENABLE EVENT COUNTER Register */
	__IO uint32_t RSVD0[5]              ;  /*!< Reserved */
	__I  uint32_t EVENT_CMD_CNT         ;  /*!< THE NUMBER OF MASTER CONMMAND Register */
	__I  uint32_t EVENT_PREFETCH_CNT    ;  /*!< THE NUMBER OF PREFETCH Register */
	__I  uint32_t EVENT_PREFETCH_HIT_CNT;  /*!< THE NUMBER OF CACHE LINE HITS AFTER PREFETCH Register */
	__I  uint32_t EVENT_DATA_DELAY_CNT  ;  /*!< THE NUMBER OF TRANSFER DATA IS NOT IMMEDIATELY RETURNED AFTER CACHE LINE HITS IN THE STREAM BUFFER Register */
	__I  uint32_t FLUSH_DONE            ;  /*!< INDICATE FLUSH IS COMPLETE Register */
	__IO uint32_t RSVD1[2]              ;  /*!< Reserved */
	__O  uint32_t FLUSH                 ;  /*!< FLUSH FIFO OR EVENT COUNTER Register */
} RXI300_OST_TypeDef;

/** @brief RXI300 Register Declaration
  */

typedef struct {
	__I  uint32_t NAME                          ;  /*!< RXI300 DESIGN NAME REGISTER,  Address offset:0x000 */
	__I  uint32_t VER                           ;  /*!< RXI300 DESIGN VERSION REGISTER,  Address offset:0x004 */
	__I  uint32_t RVE                           ;  /*!< RXI300 DESIGN REVERSION REGISTER,  Address offset:0x008 */
	__I  uint32_t INST                          ;  /*!< RXI300 DESIGN INSTANCE NUMBER REGISTER,  Address offset:0x00C */
	__I  uint32_t IMPL_Y                        ;  /*!< RXI300 IMPLEMENTATION YEAR REGISTER,  Address offset:0x010 */
	__I  uint32_t IMPL_D                        ;  /*!< RXI300 IMPLEMENTATION DATE REGISTER,  Address offset:0x014 */
	__I  uint32_t DEV                           ;  /*!< RXI300 DEVELOPER REGISTER,  Address offset:0x018 */
	__I  uint32_t PRO_NUM                       ;  /*!< RXI300 PROJECT NUMBER REGISTER,  Address offset:0x01C */
	__IO uint32_t RSVD0[56]                     ;  /*!< Reserved,  Address offset:0x020-0x0FF */
	__I  uint32_t ELR_PLD0_S                    ;  /*!< ERROR LOG PAYLOAD REGISTER0 Register,  Address offset:0x100 */
	__I  uint32_t ELR_PLD1_S                    ;  /*!< ERROR LOG PAYLOAD REGISTER1 Register,  Address offset:0x104 */
	__I  uint32_t ELR_ID_S                      ;  /*!< ERROR LOG ID INFORMATION REGISTER,  Address offset:0x108 */
	__I  uint32_t ELR_ADR0_S                    ;  /*!< ERROR LOG ADDRRESS REGISTER0 Register,  Address offset:0x10C */
	__I  uint32_t ELR_ADR1_S                    ;  /*!< ERROR LOG ADDRRESS REGISTER1 Register,  Address offset:0x110 */
	__IO uint32_t RSVD1[7]                      ;  /*!< Reserved,  Address offset:0x114-0x12F */
	__I  uint32_t ELR_CODE_S                    ;  /*!< ERROR CODE REGISTER,  Address offset:0x130 */
	__IO uint32_t RSVD2[2]                      ;  /*!< Reserved,  Address offset:0x134-0x13B */
	__O  uint32_t ELR_INTR_CLR_S                ;  /*!< ERROR LOG CLEAR REGISTER,  Address offset:0x13C */
	__IO uint32_t RSVD3[48]                     ;  /*!< Reserved,  Address offset:0x140-0x1FF */
	MPC_EntryTypeDef MPC_Entry[6]               ;  /*!< RXI300 IP have 6 ports,  Address offset:0x200-0x22C */
	__IO uint32_t RSVD4[4]                      ;  /*!< Reserved,  Address offset:0x230-0x23F */
	__IO uint32_t IDAU_CTRL                     ;  /*!< ENABLE CONTROL REGISTER IN IDAU Register,  Address offset:0x240 */
	__IO uint32_t IDAU_LOCK                     ;  /*!< LOCK CONTROL REGISTER IN IDAU Register,  Address offset:0x244 */
	__IO uint32_t RSVD5[46]                     ;  /*!< Reserved,  Address offset:0x248-0x2FF */
	MPC_EntryTypeDef MPC1_Entry[6]              ;  /*!< RXI300 IP have 6 ports,  Address offset:0x300-0x32C */
	__IO uint32_t RSVD6[4]                      ;  /*!< Reserved,  Address offset:0x330-0x33F */
	__IO uint32_t IDAU1_CTRL                    ;  /*!< ENABLE CONTROL REGISTER IN IDAU1 Register,  Address offset:0x340 */
	__IO uint32_t IDAU1_LOCK                    ;  /*!< LOCK CONTROL REGISTER IN IDAU1 Register,  Address offset:0x344 */
	__IO uint32_t RSVD7[46]                     ;  /*!< Reserved,  Address offset:0x348-0x3FF */
	MPC_EntryTypeDef MPC2_Entry[6]              ;  /*!< RXI300 IP have 6 ports,  Address offset:0x400-0x42C */
	__IO uint32_t RSVD8[4]                      ;  /*!< Reserved,  Address offset:0x430-0x43F */
	__IO uint32_t IDAU2_CTRL                    ;  /*!< ENABLE CONTROL REGISTER IN IDAU2 Register,  Address offset:0x440 */
	__IO uint32_t IDAU2_LOCK                    ;  /*!< LOCK CONTROL REGISTER IN IDAU2 Register,  Address offset:0x444 */
	__IO uint32_t RSVD9[46]                     ;  /*!< Reserved,  Address offset:0x448-0x4FF */
	PPC_TypeDef PPC                             ;  /*!< RXI300 IP have 1 ports,  Address offset:0x500-0x50C */
	__IO uint32_t RSVD10[700]                   ;  /*!< Reserved,  Address offset:0x510-0xFFF */
	RXI300_WRR_TypeDef RXI300_WRR               ;  /*!< RXI300 IP have 1 ports,  Address offset:0x1000-0x105C */
	__IO uint32_t RSVD11[56]                    ;  /*!< Reserved,  Address offset:0x1060-0x113F */
	__I  uint32_t ELR_PLD0                      ;  /*!< ERROR LOG PAYLOAD REGISTER0 Register,  Address offset:0x1140 */
	__I  uint32_t ELR_PLD1                      ;  /*!< ERROR LOG PAYLOAD REGISTER1 Register,  Address offset:0x1144 */
	__I  uint32_t ELR_ID                        ;  /*!< ERROR LOG ID INFORMATION REGISTER,  Address offset:0x1148 */
	__I  uint32_t ELR_ADR0                      ;  /*!< ERROR LOG ADDRRESS REGISTER0 Register,  Address offset:0x114C */
	__I  uint32_t ELR_ADR1                      ;  /*!< ERROR LOG ADDRRESS REGISTER1 Register,  Address offset:0x1150 */
	__IO uint32_t RSVD12[7]                     ;  /*!< Reserved,  Address offset:0x1154-0x116F */
	__I  uint32_t ELR_CODE                      ;  /*!< ERROR CODE REGISTER,  Address offset:0x1170 */
	__IO uint32_t RSVD13[2]                     ;  /*!< Reserved,  Address offset:0x1174-0x117B */
	__O  uint32_t ELR_INTR_CLR                  ;  /*!< ERROR LOG CLEAR REGISTER,  Address offset:0x117C */
	__IO uint32_t RSVD14[96]                    ;  /*!< Reserved,  Address offset:0x1180-0x12FF */
	__IO uint32_t TIMEOUT_GUARD_EN              ;  /*!< TIMEOUT GUARDIAN ENABLE REGISTER,  Address offset:0x1300 */
	__IO uint32_t RSVD15[15]                    ;  /*!< Reserved,  Address offset:0x1304-0x133F */
	__IO uint32_t TIMEOUT_CLEAN_N               ;  /*!< TIMEOUT CLEAR REGISTER,  Address offset:0x1340 */
	__IO uint32_t RSVD16[15]                    ;  /*!< Reserved,  Address offset:0x1344-0x137F */
	__IO uint32_t TIMEOUT_MON_CFG               ;  /*!< TIMEOUT MONITOR CONFIGURATION REGISTER0 Register,  Address offset:0x1380 */
	__IO uint32_t RSVD17[13087]                 ;  /*!< Reserved,  Address offset:0x1384-0xDFFF */
	RXI300_OST_TypeDef RXI300_OST               ;  /*!< RXI300 IP have 1 ports,  Address offset:0xE000-0xE03C */
} RXI300_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your defination here

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

/** @} */

/** @} */

#endif