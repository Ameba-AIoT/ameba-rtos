/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_RXI300_P1_H_
#define _AMEBA_RXI300_P1_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup RXI300_P1
  * @brief RXI300_P1 driver modules
  * @{
  */


/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup RXI300_P1_Register_Definitions RXI300_P1 Register Definitions
  * @{
  */

/** @defgroup ELR_0_PLD0_S
  * @brief Error log payload register0
  * @{
  */
#define RXI300_P1_MASK_ELR_BSTINDEX_S   ((u32)0x000000FF << 24)           /*!< R 0h  Burst index of the error request.*/
#define RXI300_P1_ELR_BSTINDEX_S(x)     (((u32)((x) & 0x000000FF) << 24))
#define RXI300_P1_GET_ELR_BSTINDEX_S(x) ((u32)(((x >> 24) & 0x000000FF)))
#define RXI300_P1_MASK_ELR_BSTLEN_S     ((u32)0x000000FF << 16)           /*!< R 0h  Burst Length of the error request.*/
#define RXI300_P1_ELR_BSTLEN_S(x)       (((u32)((x) & 0x000000FF) << 16))
#define RXI300_P1_GET_ELR_BSTLEN_S(x)   ((u32)(((x >> 16) & 0x000000FF)))
#define RXI300_P1_MASK_ELR_BSTTYPE_S    ((u32)0x00000007 << 11)           /*!< R 0h  BurstSeq of the error request.*/
#define RXI300_P1_ELR_BSTTYPE_S(x)      (((u32)((x) & 0x00000007) << 11))
#define RXI300_P1_GET_ELR_BSTTYPE_S(x)  ((u32)(((x >> 11) & 0x00000007)))
#define RXI300_P1_MASK_ELR_CMD_S        ((u32)0x00000007 << 8)            /*!< R 0h  CMD of the error request. 0x1: Write command 0x2: Read command*/
#define RXI300_P1_ELR_CMD_S(x)          (((u32)((x) & 0x00000007) << 8))
#define RXI300_P1_GET_ELR_CMD_S(x)      ((u32)(((x >> 8) & 0x00000007)))
#define RXI300_P1_MASK_ELR_SRC_S        ((u32)0x000000FF << 0)            /*!< R 0h  Error source of the error request. 0x20: M0,M8,M10 access p1s0-0 secure default                                                                                                                                                                                             0x21: M0,M8,M10 access p1s0-1~p1s0-4 secure default                                                                                                                                                                                                0x22: M0,M8,M10 access p1s0-5~p1s0-6 secure default                                                                                                                                                                                        0x11: M0,M8,M10 access p1s0-7~p1s0-8 secure default                                                                                                                                                                                                0x10: M0,M8,M10 access p1s1-0~p1s1-6 secure default*/
#define RXI300_P1_ELR_SRC_S(x)          (((u32)((x) & 0x000000FF) << 0))
#define RXI300_P1_GET_ELR_SRC_S(x)      ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup ELR_0_PLD1_S
  * @brief Error log payload register1
  * @{
  */
#define RXI300_P1_MASK_ELR_LOCK_S    ((u32)0x00000003 << 30)           /*!< R 0h  AxLock of the error request.*/
#define RXI300_P1_ELR_LOCK_S(x)      (((u32)((x) & 0x00000003) << 30))
#define RXI300_P1_GET_ELR_LOCK_S(x)  ((u32)(((x >> 30) & 0x00000003)))
#define RXI300_P1_MASK_ELR_CACHE_S   ((u32)0x0000000F << 26)           /*!< R 0h  AxCache of the error request.*/
#define RXI300_P1_ELR_CACHE_S(x)     (((u32)((x) & 0x0000000F) << 26))
#define RXI300_P1_GET_ELR_CACHE_S(x) ((u32)(((x >> 26) & 0x0000000F)))
#define RXI300_P1_MASK_ELR_PROT_S    ((u32)0x00000007 << 23)           /*!< R 0h  AxProt of the error request.*/
#define RXI300_P1_ELR_PROT_S(x)      (((u32)((x) & 0x00000007) << 23))
#define RXI300_P1_GET_ELR_PROT_S(x)  ((u32)(((x >> 23) & 0x00000007)))
#define RXI300_P1_MASK_ELR_SIZE_S    ((u32)0x00000007 << 16)           /*!< R 0h  Size of the error request.*/
#define RXI300_P1_ELR_SIZE_S(x)      (((u32)((x) & 0x00000007) << 16))
#define RXI300_P1_GET_ELR_SIZE_S(x)  ((u32)(((x >> 16) & 0x00000007)))
/** @} */

/** @defgroup ELR_0_ID_S
  * @brief Error log ID information register
  * @{
  */
#define RXI300_P1_MASK_ELR_ID_S   ((u32)0xFFFFFFFF << 0)           /*!< R 0h  Tagid of the error request. 8'b0000????: M0 KM4TZ security error 8'b1101????: M8 KM4NS security error 8'b1111????: M10 OTPC/SIC access error*/
#define RXI300_P1_ELR_ID_S(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_P1_GET_ELR_ID_S(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_0_ADR0_S
  * @brief Error log addrress register0
  * @{
  */
#define RXI300_P1_MASK_ELR_ADR0_S   ((u32)0xFFFFFFFF << 0)           /*!< R 0h  Address[31:0] of the error request.*/
#define RXI300_P1_ELR_ADR0_S(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_P1_GET_ELR_ADR0_S(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_0_ADR1_S
  * @brief Error log addrress register1
  * @{
  */
#define RXI300_P1_MASK_ELR_ADR1_S   ((u32)0xFFFFFFFF << 32)           /*!< R 0h  Address[63:32] of the error request.*/
#define RXI300_P1_ELR_ADR1_S(x)     (((u32)((x) & 0xFFFFFFFF) << 32))
#define RXI300_P1_GET_ELR_ADR1_S(x) ((u32)(((x >> 32) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_0_CODE_S
  * @brief Error code register
  * @{
  */
#define RXI300_P1_MASK_ELR_CODE_S   ((u32)0x000000FF << 0)           /*!< R 0h  The error code of error request, if it's 0, indicates that there is no error request.  8'h00: No error 8'h02: Secure fault error*/
#define RXI300_P1_ELR_CODE_S(x)     (((u32)((x) & 0x000000FF) << 0))
#define RXI300_P1_GET_ELR_CODE_S(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup ELR_0_INTR_CLR_S
  * @brief Error log clear register
  * @{
  */
#define RXI300_P1_BIT_ELR_INTR_CLR_S ((u32)0x00000001 << 0) /*!< W 0h  Error log interrupt clear. Writing 1'b1 to the register will trigger a 1-cycle pulse interrupt clear. It has to write 1'b1 to this register every time when the error exception occurs. Note that this register is write-only, the return value of reading this register is meaningless.*/
/** @} */

/** @defgroup ELR_1_PLD0_S
  * @brief Error log payload register0
  * @{
  */
#define RXI300_P1_MASK_ELR_BSTINDEX_S   ((u32)0x000000FF << 24)           /*!< R 0h  Burst index of the error request.*/
#define RXI300_P1_ELR_BSTINDEX_S(x)     (((u32)((x) & 0x000000FF) << 24))
#define RXI300_P1_GET_ELR_BSTINDEX_S(x) ((u32)(((x >> 24) & 0x000000FF)))
#define RXI300_P1_MASK_ELR_BSTLEN_S     ((u32)0x000000FF << 16)           /*!< R 0h  Burst Length of the error request.*/
#define RXI300_P1_ELR_BSTLEN_S(x)       (((u32)((x) & 0x000000FF) << 16))
#define RXI300_P1_GET_ELR_BSTLEN_S(x)   ((u32)(((x >> 16) & 0x000000FF)))
#define RXI300_P1_MASK_ELR_BSTTYPE_S    ((u32)0x00000007 << 11)           /*!< R 0h  BurstSeq of the error request.*/
#define RXI300_P1_ELR_BSTTYPE_S(x)      (((u32)((x) & 0x00000007) << 11))
#define RXI300_P1_GET_ELR_BSTTYPE_S(x)  ((u32)(((x >> 11) & 0x00000007)))
#define RXI300_P1_MASK_ELR_CMD_S        ((u32)0x00000007 << 8)            /*!< R 0h  CMD of the error request. 0x1: Write command 0x2: Read command*/
#define RXI300_P1_ELR_CMD_S(x)          (((u32)((x) & 0x00000007) << 8))
#define RXI300_P1_GET_ELR_CMD_S(x)      ((u32)(((x >> 8) & 0x00000007)))
#define RXI300_P1_MASK_ELR_SRC_S        ((u32)0x000000FF << 0)            /*!< R 0h  Error source of the error request. 0x20: M0,M8,M10 access p1s0-0 secure default                                                                                                                                                                                             0x21: M0,M8,M10 access p1s0-1~p1s0-4 secure default                                                                                                                                                                                                0x22: M0,M8,M10 access p1s0-5~p1s0-6 secure default                                                                                                                                                                                        0x11: M0,M8,M10 access p1s0-7~p1s0-8 secure default                                                                                                                                                                                                0x10: M0,M8,M10 access p1s1-0~p1s1-6 secure default*/
#define RXI300_P1_ELR_SRC_S(x)          (((u32)((x) & 0x000000FF) << 0))
#define RXI300_P1_GET_ELR_SRC_S(x)      ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup ELR_1_PLD1_S
  * @brief Error log payload register1
  * @{
  */
#define RXI300_P1_MASK_ELR_LOCK_S    ((u32)0x00000003 << 30)           /*!< R 0h  AxLock of the error request.*/
#define RXI300_P1_ELR_LOCK_S(x)      (((u32)((x) & 0x00000003) << 30))
#define RXI300_P1_GET_ELR_LOCK_S(x)  ((u32)(((x >> 30) & 0x00000003)))
#define RXI300_P1_MASK_ELR_CACHE_S   ((u32)0x0000000F << 26)           /*!< R 0h  AxCache of the error request.*/
#define RXI300_P1_ELR_CACHE_S(x)     (((u32)((x) & 0x0000000F) << 26))
#define RXI300_P1_GET_ELR_CACHE_S(x) ((u32)(((x >> 26) & 0x0000000F)))
#define RXI300_P1_MASK_ELR_PROT_S    ((u32)0x00000007 << 23)           /*!< R 0h  AxProt of the error request.*/
#define RXI300_P1_ELR_PROT_S(x)      (((u32)((x) & 0x00000007) << 23))
#define RXI300_P1_GET_ELR_PROT_S(x)  ((u32)(((x >> 23) & 0x00000007)))
#define RXI300_P1_MASK_ELR_SIZE_S    ((u32)0x00000007 << 16)           /*!< R 0h  Size of the error request.*/
#define RXI300_P1_ELR_SIZE_S(x)      (((u32)((x) & 0x00000007) << 16))
#define RXI300_P1_GET_ELR_SIZE_S(x)  ((u32)(((x >> 16) & 0x00000007)))
/** @} */

/** @defgroup ELR_1_ID_S
  * @brief Error log ID information register
  * @{
  */
#define RXI300_P1_MASK_ELR_ID_S   ((u32)0xFFFFFFFF << 0)           /*!< R 0h  Tagid of the error request. 8'b0000????: M0 KM4TZ security error 8'b1101????: M8 KM4NS security error 8'b1111????: M10 OTPC/SIC access error*/
#define RXI300_P1_ELR_ID_S(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_P1_GET_ELR_ID_S(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_1_ADR0_S
  * @brief Error log addrress register0
  * @{
  */
#define RXI300_P1_MASK_ELR_ADR0_S   ((u32)0xFFFFFFFF << 0)           /*!< R 0h  Address[31:0] of the error request.*/
#define RXI300_P1_ELR_ADR0_S(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_P1_GET_ELR_ADR0_S(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_1_ADR1_S
  * @brief Error log addrress register1
  * @{
  */
#define RXI300_P1_MASK_ELR_ADR1_S   ((u32)0xFFFFFFFF << 32)           /*!< R 0h  Address[63:32] of the error request.*/
#define RXI300_P1_ELR_ADR1_S(x)     (((u32)((x) & 0xFFFFFFFF) << 32))
#define RXI300_P1_GET_ELR_ADR1_S(x) ((u32)(((x >> 32) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_1_CODE_S
  * @brief Error code register
  * @{
  */
#define RXI300_P1_MASK_ELR_CODE_S   ((u32)0x000000FF << 0)           /*!< R 0h  The error code of error request, if it's 0, indicates that there is no error request.  8'h00: No error 8'h02: Secure fault error*/
#define RXI300_P1_ELR_CODE_S(x)     (((u32)((x) & 0x000000FF) << 0))
#define RXI300_P1_GET_ELR_CODE_S(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup ELR_1_INTR_CLR_S
  * @brief Error log clear register
  * @{
  */
#define RXI300_P1_BIT_ELR_INTR_CLR_S ((u32)0x00000001 << 0) /*!< W 0h  Error log interrupt clear. Writing 1'b1 to the register will trigger a 1-cycle pulse interrupt clear. It has to write 1'b1 to this register every time when the error exception occurs. Note that this register is write-only, the return value of reading this register is meaningless.*/
/** @} */

/** @defgroup ELR_2_PLD0_S
  * @brief Error log payload register0
  * @{
  */
#define RXI300_P1_MASK_ELR_BSTINDEX_S   ((u32)0x000000FF << 24)           /*!< R 0h  Burst index of the error request.*/
#define RXI300_P1_ELR_BSTINDEX_S(x)     (((u32)((x) & 0x000000FF) << 24))
#define RXI300_P1_GET_ELR_BSTINDEX_S(x) ((u32)(((x >> 24) & 0x000000FF)))
#define RXI300_P1_MASK_ELR_BSTLEN_S     ((u32)0x000000FF << 16)           /*!< R 0h  Burst Length of the error request.*/
#define RXI300_P1_ELR_BSTLEN_S(x)       (((u32)((x) & 0x000000FF) << 16))
#define RXI300_P1_GET_ELR_BSTLEN_S(x)   ((u32)(((x >> 16) & 0x000000FF)))
#define RXI300_P1_MASK_ELR_BSTTYPE_S    ((u32)0x00000007 << 11)           /*!< R 0h  BurstSeq of the error request.*/
#define RXI300_P1_ELR_BSTTYPE_S(x)      (((u32)((x) & 0x00000007) << 11))
#define RXI300_P1_GET_ELR_BSTTYPE_S(x)  ((u32)(((x >> 11) & 0x00000007)))
#define RXI300_P1_MASK_ELR_CMD_S        ((u32)0x00000007 << 8)            /*!< R 0h  CMD of the error request. 0x1: Write command 0x2: Read command*/
#define RXI300_P1_ELR_CMD_S(x)          (((u32)((x) & 0x00000007) << 8))
#define RXI300_P1_GET_ELR_CMD_S(x)      ((u32)(((x >> 8) & 0x00000007)))
#define RXI300_P1_MASK_ELR_SRC_S        ((u32)0x000000FF << 0)            /*!< R 0h  Error source of the error request. 0x20: M0,M8,M10 access p1s0-0 secure default                                                                                                                                                                                             0x21: M0,M8,M10 access p1s0-1~p1s0-4 secure default                                                                                                                                                                                                0x22: M0,M8,M10 access p1s0-5~p1s0-6 secure default                                                                                                                                                                                        0x11: M0,M8,M10 access p1s0-7~p1s0-8 secure default                                                                                                                                                                                                0x10: M0,M8,M10 access p1s1-0~p1s1-6 secure default*/
#define RXI300_P1_ELR_SRC_S(x)          (((u32)((x) & 0x000000FF) << 0))
#define RXI300_P1_GET_ELR_SRC_S(x)      ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup ELR_2_PLD1_S
  * @brief Error log payload register1
  * @{
  */
#define RXI300_P1_MASK_ELR_LOCK_S    ((u32)0x00000003 << 30)           /*!< R 0h  AxLock of the error request.*/
#define RXI300_P1_ELR_LOCK_S(x)      (((u32)((x) & 0x00000003) << 30))
#define RXI300_P1_GET_ELR_LOCK_S(x)  ((u32)(((x >> 30) & 0x00000003)))
#define RXI300_P1_MASK_ELR_CACHE_S   ((u32)0x0000000F << 26)           /*!< R 0h  AxCache of the error request.*/
#define RXI300_P1_ELR_CACHE_S(x)     (((u32)((x) & 0x0000000F) << 26))
#define RXI300_P1_GET_ELR_CACHE_S(x) ((u32)(((x >> 26) & 0x0000000F)))
#define RXI300_P1_MASK_ELR_PROT_S    ((u32)0x00000007 << 23)           /*!< R 0h  AxProt of the error request.*/
#define RXI300_P1_ELR_PROT_S(x)      (((u32)((x) & 0x00000007) << 23))
#define RXI300_P1_GET_ELR_PROT_S(x)  ((u32)(((x >> 23) & 0x00000007)))
#define RXI300_P1_MASK_ELR_SIZE_S    ((u32)0x00000007 << 16)           /*!< R 0h  Size of the error request.*/
#define RXI300_P1_ELR_SIZE_S(x)      (((u32)((x) & 0x00000007) << 16))
#define RXI300_P1_GET_ELR_SIZE_S(x)  ((u32)(((x >> 16) & 0x00000007)))
/** @} */

/** @defgroup ELR_2_ID_S
  * @brief Error log ID information register
  * @{
  */
#define RXI300_P1_MASK_ELR_ID_S   ((u32)0xFFFFFFFF << 0)           /*!< R 0h  Tagid of the error request. 8'b0000????: M0 KM4TZ security error 8'b1101????: M8 KM4NS security error 8'b1111????: M10 OTPC/SIC access error*/
#define RXI300_P1_ELR_ID_S(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_P1_GET_ELR_ID_S(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_2_ADR0_S
  * @brief Error log addrress register0
  * @{
  */
#define RXI300_P1_MASK_ELR_ADR0_S   ((u32)0xFFFFFFFF << 0)           /*!< R 0h  Address[31:0] of the error request.*/
#define RXI300_P1_ELR_ADR0_S(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_P1_GET_ELR_ADR0_S(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_2_ADR1_S
  * @brief Error log addrress register1
  * @{
  */
#define RXI300_P1_MASK_ELR_ADR1_S   ((u32)0xFFFFFFFF << 32)           /*!< R 0h  Address[63:32] of the error request.*/
#define RXI300_P1_ELR_ADR1_S(x)     (((u32)((x) & 0xFFFFFFFF) << 32))
#define RXI300_P1_GET_ELR_ADR1_S(x) ((u32)(((x >> 32) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_2_CODE_S
  * @brief Error code register
  * @{
  */
#define RXI300_P1_MASK_ELR_CODE_S   ((u32)0x000000FF << 0)           /*!< R 0h  The error code of error request, if it's 0, indicates that there is no error request.  8'h00: No error 8'h02: Secure fault error*/
#define RXI300_P1_ELR_CODE_S(x)     (((u32)((x) & 0x000000FF) << 0))
#define RXI300_P1_GET_ELR_CODE_S(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup ELR_2_INTR_CLR_S
  * @brief Error log clear register
  * @{
  */
#define RXI300_P1_BIT_ELR_INTR_CLR_S ((u32)0x00000001 << 0) /*!< W 0h  Error log interrupt clear. Writing 1'b1 to the register will trigger a 1-cycle pulse interrupt clear. It has to write 1'b1 to this register every time when the error exception occurs. Note that this register is write-only, the return value of reading this register is meaningless.*/
/** @} */

/** @defgroup SLOT_CTRL
  * @brief Slot control of weighted round-robin for SRAM
  * @{
  */
#define RXI300_P1_MASK_AW_SLOT_0   ((u32)0x00000003 << 30)           /*!< R/W 1h  Control value of write slot 0*/
#define RXI300_P1_AW_SLOT_0(x)     (((u32)((x) & 0x00000003) << 30))
#define RXI300_P1_GET_AW_SLOT_0(x) ((u32)(((x >> 30) & 0x00000003)))
#define RXI300_P1_MASK_AW_SLOT_1   ((u32)0x00000003 << 28)           /*!< R/W 0h  Control value of write slot 1*/
#define RXI300_P1_AW_SLOT_1(x)     (((u32)((x) & 0x00000003) << 28))
#define RXI300_P1_GET_AW_SLOT_1(x) ((u32)(((x >> 28) & 0x00000003)))
#define RXI300_P1_MASK_AW_SLOT_2   ((u32)0x00000003 << 26)           /*!< R/W 2h  Control value of write slot 2*/
#define RXI300_P1_AW_SLOT_2(x)     (((u32)((x) & 0x00000003) << 26))
#define RXI300_P1_GET_AW_SLOT_2(x) ((u32)(((x >> 26) & 0x00000003)))
#define RXI300_P1_MASK_AW_SLOT_3   ((u32)0x00000003 << 24)           /*!< R/W 1h  Control value of write slot 3*/
#define RXI300_P1_AW_SLOT_3(x)     (((u32)((x) & 0x00000003) << 24))
#define RXI300_P1_GET_AW_SLOT_3(x) ((u32)(((x >> 24) & 0x00000003)))
#define RXI300_P1_MASK_AW_SLOT_4   ((u32)0x00000003 << 22)           /*!< R/W 0h  Control value of write slot 4*/
#define RXI300_P1_AW_SLOT_4(x)     (((u32)((x) & 0x00000003) << 22))
#define RXI300_P1_GET_AW_SLOT_4(x) ((u32)(((x >> 22) & 0x00000003)))
#define RXI300_P1_MASK_AW_SLOT_5   ((u32)0x00000003 << 20)           /*!< R/W 2h  Control value of write slot 5*/
#define RXI300_P1_AW_SLOT_5(x)     (((u32)((x) & 0x00000003) << 20))
#define RXI300_P1_GET_AW_SLOT_5(x) ((u32)(((x >> 20) & 0x00000003)))
#define RXI300_P1_MASK_AW_SLOT_6   ((u32)0x00000003 << 18)           /*!< R/W 1h  Control value of write slot 6*/
#define RXI300_P1_AW_SLOT_6(x)     (((u32)((x) & 0x00000003) << 18))
#define RXI300_P1_GET_AW_SLOT_6(x) ((u32)(((x >> 18) & 0x00000003)))
#define RXI300_P1_MASK_AW_SLOT_7   ((u32)0x00000003 << 16)           /*!< R/W 0h  Control value of write slot 7*/
#define RXI300_P1_AW_SLOT_7(x)     (((u32)((x) & 0x00000003) << 16))
#define RXI300_P1_GET_AW_SLOT_7(x) ((u32)(((x >> 16) & 0x00000003)))
#define RXI300_P1_MASK_AR_SLOT_0   ((u32)0x00000003 << 14)           /*!< R/W 1h  Control value of read slot 0*/
#define RXI300_P1_AR_SLOT_0(x)     (((u32)((x) & 0x00000003) << 14))
#define RXI300_P1_GET_AR_SLOT_0(x) ((u32)(((x >> 14) & 0x00000003)))
#define RXI300_P1_MASK_AR_SLOT_1   ((u32)0x00000003 << 12)           /*!< R/W 0h  Control value of read slot 1*/
#define RXI300_P1_AR_SLOT_1(x)     (((u32)((x) & 0x00000003) << 12))
#define RXI300_P1_GET_AR_SLOT_1(x) ((u32)(((x >> 12) & 0x00000003)))
#define RXI300_P1_MASK_AR_SLOT_2   ((u32)0x00000003 << 10)           /*!< R/W 2h  Control value of read slot 2*/
#define RXI300_P1_AR_SLOT_2(x)     (((u32)((x) & 0x00000003) << 10))
#define RXI300_P1_GET_AR_SLOT_2(x) ((u32)(((x >> 10) & 0x00000003)))
#define RXI300_P1_MASK_AR_SLOT_3   ((u32)0x00000003 << 8)            /*!< R/W 1h  Control value of read slot 3*/
#define RXI300_P1_AR_SLOT_3(x)     (((u32)((x) & 0x00000003) << 8))
#define RXI300_P1_GET_AR_SLOT_3(x) ((u32)(((x >> 8) & 0x00000003)))
#define RXI300_P1_MASK_AR_SLOT_4   ((u32)0x00000003 << 6)            /*!< R/W 0h  Control value of read slot 4*/
#define RXI300_P1_AR_SLOT_4(x)     (((u32)((x) & 0x00000003) << 6))
#define RXI300_P1_GET_AR_SLOT_4(x) ((u32)(((x >> 6) & 0x00000003)))
#define RXI300_P1_MASK_AR_SLOT_5   ((u32)0x00000003 << 4)            /*!< R/W 2h  Control value of read slot 5*/
#define RXI300_P1_AR_SLOT_5(x)     (((u32)((x) & 0x00000003) << 4))
#define RXI300_P1_GET_AR_SLOT_5(x) ((u32)(((x >> 4) & 0x00000003)))
#define RXI300_P1_MASK_AR_SLOT_6   ((u32)0x00000003 << 2)            /*!< R/W 1h  Control value of read slot 6*/
#define RXI300_P1_AR_SLOT_6(x)     (((u32)((x) & 0x00000003) << 2))
#define RXI300_P1_GET_AR_SLOT_6(x) ((u32)(((x >> 2) & 0x00000003)))
#define RXI300_P1_MASK_AR_SLOT_7   ((u32)0x00000003 << 0)            /*!< R/W 0h  Control value of read slot 7*/
#define RXI300_P1_AR_SLOT_7(x)     (((u32)((x) & 0x00000003) << 0))
#define RXI300_P1_GET_AR_SLOT_7(x) ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup ELR_0_PLD0
  * @brief Error log payload register0
  * @{
  */
#define RXI300_P1_MASK_ELR_BSTINDEX   ((u32)0x000000FF << 24)           /*!< R 0h  Burst index of the error request.*/
#define RXI300_P1_ELR_BSTINDEX(x)     (((u32)((x) & 0x000000FF) << 24))
#define RXI300_P1_GET_ELR_BSTINDEX(x) ((u32)(((x >> 24) & 0x000000FF)))
#define RXI300_P1_MASK_ELR_BSTLEN     ((u32)0x000000FF << 16)           /*!< R 0h  Burst Length of the error request.*/
#define RXI300_P1_ELR_BSTLEN(x)       (((u32)((x) & 0x000000FF) << 16))
#define RXI300_P1_GET_ELR_BSTLEN(x)   ((u32)(((x >> 16) & 0x000000FF)))
#define RXI300_P1_MASK_ELR_BSTTYPE    ((u32)0x00000007 << 11)           /*!< R 0h  BurstSeq of the error request.*/
#define RXI300_P1_ELR_BSTTYPE(x)      (((u32)((x) & 0x00000007) << 11))
#define RXI300_P1_GET_ELR_BSTTYPE(x)  ((u32)(((x >> 11) & 0x00000007)))
#define RXI300_P1_MASK_ELR_CMD        ((u32)0x00000007 << 8)            /*!< R 0h  CMD of the error request. 0x1: Write command 0x2: Read command*/
#define RXI300_P1_ELR_CMD(x)          (((u32)((x) & 0x00000007) << 8))
#define RXI300_P1_GET_ELR_CMD(x)      ((u32)(((x >> 8) & 0x00000007)))
#define RXI300_P1_MASK_ELR_SRC        ((u32)0x000000FF << 0)            /*!< R 0h  Error source of the error request. 0x00: km4ns or otpc address hole default slave                                                                                                                                              0x24: wifi or aes_sha address hole default slave*/
#define RXI300_P1_ELR_SRC(x)          (((u32)((x) & 0x000000FF) << 0))
#define RXI300_P1_GET_ELR_SRC(x)      ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup ELR_0_PLD1
  * @brief Error log payload register1
  * @{
  */
#define RXI300_P1_MASK_ELR_LOCK    ((u32)0x00000003 << 30)           /*!< R 0h  AxLock of the error request.*/
#define RXI300_P1_ELR_LOCK(x)      (((u32)((x) & 0x00000003) << 30))
#define RXI300_P1_GET_ELR_LOCK(x)  ((u32)(((x >> 30) & 0x00000003)))
#define RXI300_P1_MASK_ELR_CACHE   ((u32)0x0000000F << 26)           /*!< R 0h  AxCache of the error request.*/
#define RXI300_P1_ELR_CACHE(x)     (((u32)((x) & 0x0000000F) << 26))
#define RXI300_P1_GET_ELR_CACHE(x) ((u32)(((x >> 26) & 0x0000000F)))
#define RXI300_P1_MASK_ELR_PROT    ((u32)0x00000007 << 23)           /*!< R 0h  AxProt of the error request.*/
#define RXI300_P1_ELR_PROT(x)      (((u32)((x) & 0x00000007) << 23))
#define RXI300_P1_GET_ELR_PROT(x)  ((u32)(((x >> 23) & 0x00000007)))
#define RXI300_P1_MASK_ELR_SIZE    ((u32)0x00000007 << 16)           /*!< R 0h  Size of the error request.*/
#define RXI300_P1_ELR_SIZE(x)      (((u32)((x) & 0x00000007) << 16))
#define RXI300_P1_GET_ELR_SIZE(x)  ((u32)(((x >> 16) & 0x00000007)))
/** @} */

/** @defgroup ELR_0_ID
  * @brief Error log ID information register
  * @{
  */
#define RXI300_P1_MASK_ELR_ID   ((u32)0xFFFFFFFF << 0)           /*!< R 0h  Tagid of the error request. 8'b0000????: M0 KM4TZaccess error 8'b1101????: M8 KM4NS access error 8'b1111????: M10 OTPC/SIC access error 8'b101?????: M11 AES_SHA access error 8'b1110????: M9 WIFI access error*/
#define RXI300_P1_ELR_ID(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_P1_GET_ELR_ID(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_0_ADR0
  * @brief Error log addrress register0
  * @{
  */
#define RXI300_P1_MASK_ELR_ADR0   ((u32)0xFFFFFFFF << 0)           /*!< R 0h  Address[31:0] of the error request.*/
#define RXI300_P1_ELR_ADR0(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_P1_GET_ELR_ADR0(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_0_ADR1
  * @brief Error log addrress register1
  * @{
  */
#define RXI300_P1_MASK_ELR_ADR1   ((u32)0xFFFFFFFF << 32)           /*!< R 0h  Address[63:32] of the error request.*/
#define RXI300_P1_ELR_ADR1(x)     (((u32)((x) & 0xFFFFFFFF) << 32))
#define RXI300_P1_GET_ELR_ADR1(x) ((u32)(((x >> 32) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_0_CODE
  * @brief Error code register
  * @{
  */
#define RXI300_P1_MASK_ELR_CODE   ((u32)0x000000FF << 0)           /*!< R 0h  The error code of error request, if it's 0, indicates that there is no error request.  8'h00: No error 8'h01: Address hole error*/
#define RXI300_P1_ELR_CODE(x)     (((u32)((x) & 0x000000FF) << 0))
#define RXI300_P1_GET_ELR_CODE(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup ELR_0_INTR_CLR
  * @brief Error log clear register
  * @{
  */
#define RXI300_P1_BIT_ELR_INTR_CLR ((u32)0x00000001 << 0) /*!< W 0h  Error log interrupt clear. Writing 1'b1 to the register will trigger a 1-cycle pulse interrupt clear. It has to write 1'b1 to this register every time when the error exception occurs. Note that this register is write-only, the return value of reading this register is meaningless.*/
/** @} */

/** @defgroup ELR_1_PLD0
  * @brief Error log payload register0
  * @{
  */
#define RXI300_P1_MASK_ELR_BSTINDEX   ((u32)0x000000FF << 24)           /*!< R 0h  Burst index of the error request.*/
#define RXI300_P1_ELR_BSTINDEX(x)     (((u32)((x) & 0x000000FF) << 24))
#define RXI300_P1_GET_ELR_BSTINDEX(x) ((u32)(((x >> 24) & 0x000000FF)))
#define RXI300_P1_MASK_ELR_BSTLEN     ((u32)0x000000FF << 16)           /*!< R 0h  Burst Length of the error request.*/
#define RXI300_P1_ELR_BSTLEN(x)       (((u32)((x) & 0x000000FF) << 16))
#define RXI300_P1_GET_ELR_BSTLEN(x)   ((u32)(((x >> 16) & 0x000000FF)))
#define RXI300_P1_MASK_ELR_BSTTYPE    ((u32)0x00000007 << 11)           /*!< R 0h  BurstSeq of the error request.*/
#define RXI300_P1_ELR_BSTTYPE(x)      (((u32)((x) & 0x00000007) << 11))
#define RXI300_P1_GET_ELR_BSTTYPE(x)  ((u32)(((x >> 11) & 0x00000007)))
#define RXI300_P1_MASK_ELR_CMD        ((u32)0x00000007 << 8)            /*!< R 0h  CMD of the error request. 0x1: Write command 0x2: Read command*/
#define RXI300_P1_ELR_CMD(x)          (((u32)((x) & 0x00000007) << 8))
#define RXI300_P1_GET_ELR_CMD(x)      ((u32)(((x >> 8) & 0x00000007)))
#define RXI300_P1_MASK_ELR_SRC        ((u32)0x000000FF << 0)            /*!< R 0h  Error source of the error request. 0x00: km4ns or otpc address hole default slave                                                                                                                                              0x24: wifi or aes_sha address hole default slave*/
#define RXI300_P1_ELR_SRC(x)          (((u32)((x) & 0x000000FF) << 0))
#define RXI300_P1_GET_ELR_SRC(x)      ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup ELR_1_PLD1
  * @brief Error log payload register1
  * @{
  */
#define RXI300_P1_MASK_ELR_LOCK    ((u32)0x00000003 << 30)           /*!< R 0h  AxLock of the error request.*/
#define RXI300_P1_ELR_LOCK(x)      (((u32)((x) & 0x00000003) << 30))
#define RXI300_P1_GET_ELR_LOCK(x)  ((u32)(((x >> 30) & 0x00000003)))
#define RXI300_P1_MASK_ELR_CACHE   ((u32)0x0000000F << 26)           /*!< R 0h  AxCache of the error request.*/
#define RXI300_P1_ELR_CACHE(x)     (((u32)((x) & 0x0000000F) << 26))
#define RXI300_P1_GET_ELR_CACHE(x) ((u32)(((x >> 26) & 0x0000000F)))
#define RXI300_P1_MASK_ELR_PROT    ((u32)0x00000007 << 23)           /*!< R 0h  AxProt of the error request.*/
#define RXI300_P1_ELR_PROT(x)      (((u32)((x) & 0x00000007) << 23))
#define RXI300_P1_GET_ELR_PROT(x)  ((u32)(((x >> 23) & 0x00000007)))
#define RXI300_P1_MASK_ELR_SIZE    ((u32)0x00000007 << 16)           /*!< R 0h  Size of the error request.*/
#define RXI300_P1_ELR_SIZE(x)      (((u32)((x) & 0x00000007) << 16))
#define RXI300_P1_GET_ELR_SIZE(x)  ((u32)(((x >> 16) & 0x00000007)))
/** @} */

/** @defgroup ELR_1_ADR0
  * @brief Error log addrress register0
  * @{
  */
#define RXI300_P1_MASK_ELR_ADR0   ((u32)0xFFFFFFFF << 0)           /*!< R 0h  Address[31:0] of the error request.*/
#define RXI300_P1_ELR_ADR0(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_P1_GET_ELR_ADR0(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_1_ADR1
  * @brief Error log addrress register1
  * @{
  */
#define RXI300_P1_MASK_ELR_ADR1   ((u32)0xFFFFFFFF << 32)           /*!< R 0h  Address[63:32] of the error request.*/
#define RXI300_P1_ELR_ADR1(x)     (((u32)((x) & 0xFFFFFFFF) << 32))
#define RXI300_P1_GET_ELR_ADR1(x) ((u32)(((x >> 32) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_1_CODE
  * @brief Error code register
  * @{
  */
#define RXI300_P1_MASK_ELR_CODE   ((u32)0x000000FF << 0)           /*!< R 0h  The error code of error request, if it's 0, indicates that there is no error request.  8'h00: No error 8'h01: Address hole error*/
#define RXI300_P1_ELR_CODE(x)     (((u32)((x) & 0x000000FF) << 0))
#define RXI300_P1_GET_ELR_CODE(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup ELR_1_INTR_CLR
  * @brief Error log clear register
  * @{
  */
#define RXI300_P1_BIT_ELR_INTR_CLR ((u32)0x00000001 << 0) /*!< W 0h  Error log interrupt clear. Writing 1'b1 to the register will trigger a 1-cycle pulse interrupt clear. It has to write 1'b1 to this register every time when the error exception occurs. Note that this register is write-only, the return value of reading this register is meaningless.*/
/** @} */

/** @defgroup NAME
  * @brief RXI300 design name register
  * @{
  */
#define RXI300_P1_MASK_NAME   ((u32)0xFFFFFFFF << 0)           /*!< R 00052300h  The design name of RXI-300.*/
#define RXI300_P1_NAME(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_P1_GET_NAME(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup VER
  * @brief RXI300 design version register
  * @{
  */
#define RXI300_P1_MASK_VER   ((u32)0xFFFFFFFF << 0)           /*!< R 00000300h  The design version of RXI-300.*/
#define RXI300_P1_VER(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_P1_GET_VER(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup RVE
  * @brief RXI300 design reversion register
  * @{
  */
#define RXI300_P1_MASK_REV   ((u32)0xFFFFFFFF << 0)           /*!< R 0h  The design reversion of RXI-300.*/
#define RXI300_P1_REV(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_P1_GET_REV(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup INST
  * @brief RXI300 design instance number register
  * @{
  */
#define RXI300_P1_MASK_INST   ((u32)0xFFFFFFFF << 0)           /*!< R 0h  The design instance number of RXI-300.*/
#define RXI300_P1_INST(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_P1_GET_INST(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup IMPL_Y
  * @brief RXI300 implementation year register
  * @{
  */
#define RXI300_P1_MASK_IMPL_Y   ((u32)0xFFFFFFFF << 0)           /*!< R 2025h  The implementatin year of RXI-300. Reset value depends on implementation date.*/
#define RXI300_P1_IMPL_Y(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_P1_GET_IMPL_Y(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup IMPL_D
  * @brief RXI300 implementation date register
  * @{
  */
#define RXI300_P1_MASK_IMPL_D   ((u32)0xFFFFFFFF << 0)           /*!< R 03131449h  The implementatin date of RXI-300. Reset value depends on implementation date.*/
#define RXI300_P1_IMPL_D(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_P1_GET_IMPL_D(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup DEV
  * @brief RXI300 developer register
  * @{
  */
#define RXI300_P1_MASK_DEV   ((u32)0xFFFFFFFF << 0)           /*!< R 00522734h  The developer of RXI-300.*/
#define RXI300_P1_DEV(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_P1_GET_DEV(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup PRO_NUM
  * @brief RXI300 project number register
  * @{
  */
#define RXI300_P1_MASK_PRO_NUM   ((u32)0xFFFFFFFF << 0)           /*!< R 00006955h  The project number of RXI-300.*/
#define RXI300_P1_PRO_NUM(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_P1_GET_PRO_NUM(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup RXI300_P1_Exported_Types RXI300_P1 Exported Types
  * @{
  */

/** @brief RXI300_P1 Register Declaration
  */

typedef struct {
	__I  uint32_t ELR_0_PLD0_S    ;  /*!< ERROR LOG PAYLOAD REGISTER0 Register,  Address offset:0x000 */
	__I  uint32_t ELR_0_PLD1_S    ;  /*!< ERROR LOG PAYLOAD REGISTER1 Register,  Address offset:0x004 */
	__I  uint32_t ELR_0_ID_S      ;  /*!< ERROR LOG ID INFORMATION REGISTER,  Address offset:0x008 */
	__I  uint32_t ELR_0_ADR0_S    ;  /*!< ERROR LOG ADDRRESS REGISTER0 Register,  Address offset:0x00C */
	__I  uint32_t ELR_0_ADR1_S    ;  /*!< ERROR LOG ADDRRESS REGISTER1 Register,  Address offset:0x010 */
	__IO uint32_t RSVD0[7]        ;  /*!< Reserved,  Address offset:0x014-0x02F */
	__I  uint32_t ELR_0_CODE_S    ;  /*!< ERROR CODE REGISTER,  Address offset:0x030 */
	__IO uint32_t RSVD1[2]        ;  /*!< Reserved,  Address offset:0x034-0x03B */
	__O  uint32_t ELR_0_INTR_CLR_S;  /*!< ERROR LOG CLEAR REGISTER,  Address offset:0x03C */
	__I  uint32_t ELR_1_PLD0_S    ;  /*!< ERROR LOG PAYLOAD REGISTER0 Register,  Address offset:0x040 */
	__I  uint32_t ELR_1_PLD1_S    ;  /*!< ERROR LOG PAYLOAD REGISTER1 Register,  Address offset:0x044 */
	__I  uint32_t ELR_1_ID_S      ;  /*!< ERROR LOG ID INFORMATION REGISTER,  Address offset:0x048 */
	__I  uint32_t ELR_1_ADR0_S    ;  /*!< ERROR LOG ADDRRESS REGISTER0 Register,  Address offset:0x04C */
	__I  uint32_t ELR_1_ADR1_S    ;  /*!< ERROR LOG ADDRRESS REGISTER1 Register,  Address offset:0x050 */
	__IO uint32_t RSVD2[7]        ;  /*!< Reserved,  Address offset:0x054-0x06F */
	__I  uint32_t ELR_1_CODE_S    ;  /*!< ERROR CODE REGISTER,  Address offset:0x070 */
	__IO uint32_t RSVD3[2]        ;  /*!< Reserved,  Address offset:0x074-0x07B */
	__O  uint32_t ELR_1_INTR_CLR_S;  /*!< ERROR LOG CLEAR REGISTER,  Address offset:0x07C */
	__I  uint32_t ELR_2_PLD0_S    ;  /*!< ERROR LOG PAYLOAD REGISTER0 Register,  Address offset:0x080 */
	__I  uint32_t ELR_2_PLD1_S    ;  /*!< ERROR LOG PAYLOAD REGISTER1 Register,  Address offset:0x084 */
	__I  uint32_t ELR_2_ID_S      ;  /*!< ERROR LOG ID INFORMATION REGISTER,  Address offset:0x088 */
	__I  uint32_t ELR_2_ADR0_S    ;  /*!< ERROR LOG ADDRRESS REGISTER0 Register,  Address offset:0x08C */
	__I  uint32_t ELR_2_ADR1_S    ;  /*!< ERROR LOG ADDRRESS REGISTER1 Register,  Address offset:0x090 */
	__IO uint32_t RSVD4[7]        ;  /*!< Reserved,  Address offset:0x094-0x0AF */
	__I  uint32_t ELR_2_CODE_S    ;  /*!< ERROR CODE REGISTER,  Address offset:0x0B0 */
	__IO uint32_t RSVD5[2]        ;  /*!< Reserved,  Address offset:0x0B4-0x0BB */
	__O  uint32_t ELR_2_INTR_CLR_S;  /*!< ERROR LOG CLEAR REGISTER,  Address offset:0x0BC */
	__IO uint32_t RSVD6[976]      ;  /*!< Reserved,  Address offset:0x0C0-0xFFF */
	__IO uint32_t SLOT_CTRL       ;  /*!< SLOT CONTROL OF WEIGHTED ROUND-ROBIN FOR SRAM Register,  Address offset:0x1000 */
	__IO uint32_t RSVD7[63]       ;  /*!< Reserved,  Address offset:0x1004-0x10FF */
	__I  uint32_t ELR_0_PLD0      ;  /*!< ERROR LOG PAYLOAD REGISTER0 Register,  Address offset:0x1100 */
	__I  uint32_t ELR_0_PLD1      ;  /*!< ERROR LOG PAYLOAD REGISTER1 Register,  Address offset:0x1104 */
	__IO uint32_t RSVD8           ;  /*!< Reserved,  Address offset:0x1108-0x110B */
	__I  uint32_t ELR_0_ADR0      ;  /*!< ERROR LOG ADDRRESS REGISTER0 Register,  Address offset:0x110C */
	__I  uint32_t ELR_0_ADR1      ;  /*!< ERROR LOG ADDRRESS REGISTER1 Register,  Address offset:0x1110 */
	__IO uint32_t RSVD9[7]        ;  /*!< Reserved,  Address offset:0x1114-0x112F */
	__I  uint32_t ELR_0_CODE      ;  /*!< ERROR CODE REGISTER,  Address offset:0x1130 */
	__IO uint32_t RSVD10[2]       ;  /*!< Reserved,  Address offset:0x1134-0x113B */
	__O  uint32_t ELR_0_INTR_CLR  ;  /*!< ERROR LOG CLEAR REGISTER,  Address offset:0x113C */
	__I  uint32_t ELR_1_PLD0      ;  /*!< ERROR LOG PAYLOAD REGISTER0 Register,  Address offset:0x1140 */
	__I  uint32_t ELR_1_PLD1      ;  /*!< ERROR LOG PAYLOAD REGISTER1 Register,  Address offset:0x1144 */
	__I  uint32_t ELR_0_ID        ;  /*!< ERROR LOG ID INFORMATION REGISTER,  Address offset:0x1148 */
	__I  uint32_t ELR_1_ADR0      ;  /*!< ERROR LOG ADDRRESS REGISTER0 Register,  Address offset:0x114C */
	__I  uint32_t ELR_1_ADR1      ;  /*!< ERROR LOG ADDRRESS REGISTER1 Register,  Address offset:0x1150 */
	__IO uint32_t RSVD11[7]       ;  /*!< Reserved,  Address offset:0x1154-0x116F */
	__I  uint32_t ELR_1_CODE      ;  /*!< ERROR CODE REGISTER,  Address offset:0x1170 */
	__IO uint32_t RSVD12[2]       ;  /*!< Reserved,  Address offset:0x1174-0x117B */
	__O  uint32_t ELR_1_INTR_CLR  ;  /*!< ERROR LOG CLEAR REGISTER,  Address offset:0x117C */
	__IO uint32_t RSVD13[32]      ;  /*!< Reserved,  Address offset:0x1180-0x11FF */
	__I  uint32_t NAME            ;  /*!< RXI300 DESIGN NAME REGISTER,  Address offset:0x1200 */
	__I  uint32_t VER             ;  /*!< RXI300 DESIGN VERSION REGISTER,  Address offset:0x1204 */
	__I  uint32_t RVE             ;  /*!< RXI300 DESIGN REVERSION REGISTER,  Address offset:0x1208 */
	__I  uint32_t INST            ;  /*!< RXI300 DESIGN INSTANCE NUMBER REGISTER,  Address offset:0x120C */
	__I  uint32_t IMPL_Y          ;  /*!< RXI300 IMPLEMENTATION YEAR REGISTER,  Address offset:0x1210 */
	__I  uint32_t IMPL_D          ;  /*!< RXI300 IMPLEMENTATION DATE REGISTER,  Address offset:0x1214 */
	__I  uint32_t DEV             ;  /*!< RXI300 DEVELOPER REGISTER,  Address offset:0x1218 */
	__I  uint32_t PRO_NUM         ;  /*!< RXI300 PROJECT NUMBER REGISTER,  Address offset:0x121C */
} RXI300_P1_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


// Please add your definitions here

/* Exported Types ------------------------------------------------------------*/

/* Exported Constants --------------------------------------------------------*/

/* Exported Functions --------------------------------------------------------*/


/** @} */

/** @} */

// If other definitions do not need to be extracted to CHM document, add them here.
/* Other Definitions --------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

#endif