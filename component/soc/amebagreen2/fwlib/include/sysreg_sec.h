/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SYSREG_SEC_H_
#define _SYSREG_SEC_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup SEC
  * @brief SEC driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup SEC_Register_Definitions SEC Register Definitions
  * @{
  */

/** @defgroup SEC_OTP_SYSCFG0
  * @brief
  * @{
  */
#define SEC_BIT_USB_DOWNLOAD_EN       ((u32)0x00000001 << 31)           /*!< R/W 0x0  USB download enable 0: disable 1: enable*/
#define SEC_BIT_USB_CLK_CAL_EN        ((u32)0x00000001 << 30)           /*!< R/W 0x0  SoC clock calibration for USB 0: disable 1: enable*/
#define SEC_BIT_SW_RSVD_02            ((u32)0x00000001 << 29)           /*!< R/W 0x0  */
#define SEC_BIT_LOGIC_RDP_EN          ((u32)0x00000001 << 28)           /*!< R/W 0x0  RDP enable*/
#define SEC_BIT_LOGIC_RSIP_EN         ((u32)0x00000001 << 27)           /*!< R/W 0x0  RSIP enable*/
#define SEC_BIT_LOGIC_SECURE_BOOT_EN  ((u32)0x00000001 << 26)           /*!< R/W 0x0  Secure boot enable*/
#define SEC_BIT_LOW_BAUD_LOG_EN       ((u32)0x00000001 << 25)           /*!< R/W 0x0  Loguart Baudrate selection 1: 115200bps 0: 1.5Mbps*/
#define SEC_BIT_DIS_BOOT_LOG_EN       ((u32)0x00000001 << 24)           /*!< R/W 0x0  Boot log disable*/
#define SEC_MASK_BOOT_NOR_FLASH       ((u32)0x00000003 << 22)           /*!< R/W 0x0  Boot from Nor flash, or Nand flash, or try. 0/3: determined by power-on latch 1: nor flash 2: nand flash*/
#define SEC_BOOT_NOR_FLASH(x)         (((u32)((x) & 0x00000003) << 22))
#define SEC_GET_BOOT_NOR_FLASH(x)     ((u32)(((x >> 22) & 0x00000003)))
#define SEC_BIT_BOOT_CNT_LMT_EN       ((u32)0x00000001 << 21)           /*!< R/W 0x0  Enable boot count limit 0: disable 1: enable*/
#define SEC_BIT_NAND_PAGE_SIZE        ((u32)0x00000001 << 20)           /*!< R/W 0x0  Nand FLASH Page size 0: 2KB 1: 4KB*/
#define SEC_BIT_BOOT_FLASH_BAUD_SEL   ((u32)0x00000001 << 19)           /*!< R/W 0x0  Boot Flash clock selection. Baudrate = 40/(2*(1+x)) 0: 20M 1: 10M*/
#define SEC_BIT_SPIC_BOOT_SPEEDUP_DIS ((u32)0x00000001 << 18)           /*!< R/W 0x0  SPIC boot speed up disable*/
#define SEC_BIT_SPIC_ADDR_4BYTE_EN    ((u32)0x00000001 << 17)           /*!< R/W 0x0  SPIC address 4-byte enable*/
#define SEC_BIT_FLASH_DEEP_SLEEP_EN   ((u32)0x00000001 << 16)           /*!< R/W 0x0  Flash deepsleep enable*/
#define SEC_MASK_SW_RSVD_01           ((u32)0x000000FF << 8)            /*!< R/W 0x0  RSVD5 for SW usage*/
#define SEC_SW_RSVD_01(x)             (((u32)((x) & 0x000000FF) << 8))
#define SEC_GET_SW_RSVD_01(x)         ((u32)(((x >> 8) & 0x000000FF)))
#define SEC_MASK_BOOT_INT_FLASH       ((u32)0x00000003 << 6)            /*!< R/W 0x0  Boot from internal flash or external flash. 0/3: determined by power-on latch 1: internal flash 2: external flash*/
#define SEC_BOOT_INT_FLASH(x)         (((u32)((x) & 0x00000003) << 6))
#define SEC_GET_BOOT_INT_FLASH(x)     ((u32)(((x >> 6) & 0x00000003)))
#define SEC_MASK_SDIO_PINMUX          ((u32)0x00000003 << 4)            /*!< R/W 0x0  00:PINMUX Index 0 01:PINMUX Index 1 10:PINMUX Index 2 11:PINMUX Index 3*/
#define SEC_SDIO_PINMUX(x)            (((u32)((x) & 0x00000003) << 4))
#define SEC_GET_SDIO_PINMUX(x)        ((u32)(((x >> 4) & 0x00000003)))
#define SEC_BIT_BT_FUNCTION_EN        ((u32)0x00000001 << 3)            /*!< R/W 0x0  0:disable 1:enable*/
#define SEC_MASK_BOOT_SRC_SEL         ((u32)0x00000003 << 1)            /*!< R/W 0x0  Choose Boot Src 0/3: boot from flash 1: boot from USB 2: boot from SDIO*/
#define SEC_BOOT_SRC_SEL(x)           (((u32)((x) & 0x00000003) << 1))
#define SEC_GET_BOOT_SRC_SEL(x)       ((u32)(((x >> 1) & 0x00000003)))
#define SEC_BIT_SW_RSVD_00            ((u32)0x00000001 << 0)            /*!< R/W 0x0  */
/** @} */

/** @defgroup SEC_OTP_SYSCFG1
  * @brief
  * @{
  */
#define SEC_MASK_SW_RSVD3   ((u32)0x000000FF << 24)           /*!< R/W 0x0  RSVD for SW usage*/
#define SEC_SW_RSVD3(x)     (((u32)((x) & 0x000000FF) << 24))
#define SEC_GET_SW_RSVD3(x) ((u32)(((x >> 24) & 0x000000FF)))
#define SEC_MASK_SW_RSVD2   ((u32)0x000000FF << 16)           /*!< R/W 0x0  RSVD for SW usage*/
#define SEC_SW_RSVD2(x)     (((u32)((x) & 0x000000FF) << 16))
#define SEC_GET_SW_RSVD2(x) ((u32)(((x >> 16) & 0x000000FF)))
#define SEC_MASK_SW_RSVD1   ((u32)0x000000FF << 8)            /*!< R/W 0x0  RSVD for SW usage*/
#define SEC_SW_RSVD1(x)     (((u32)((x) & 0x000000FF) << 8))
#define SEC_GET_SW_RSVD1(x) ((u32)(((x >> 8) & 0x000000FF)))
#define SEC_MASK_SW_RSVD0   ((u32)0x000000FF << 0)            /*!< R/W 0x0  RSVD for SW usage*/
#define SEC_SW_RSVD0(x)     (((u32)((x) & 0x000000FF) << 0))
#define SEC_GET_SW_RSVD0(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup SEC_OTP_SYSCFG2
  * @brief
  * @{
  */
#define SEC_MASK_SW_RSVD4   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  RSVD for SW usage*/
#define SEC_SW_RSVD4(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_SW_RSVD4(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SEC_OTP_SYSCFG3
  * @brief
  * @{
  */
#define SEC_MASK_SW_RSVD5   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  RSVD for SW usage*/
#define SEC_SW_RSVD5(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_SW_RSVD5(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SEC_OTP_HWCFG
  * @brief
  * @{
  */
#define SEC_BIT_BOOT_VOL_SEL ((u32)0x00000001 << 31)          /*!< R/W 0x0  BOOT Voltage Selection: 0'b0: 0.9V 0'b1: 1.0V*/
#define SEC_MASK_SW_RSVD6    ((u32)0x7FFFFFFF << 0)           /*!< R/W 0x0  RSVD for SW usage*/
#define SEC_SW_RSVD6(x)      (((u32)((x) & 0x7FFFFFFF) << 0))
#define SEC_GET_SW_RSVD6(x)  ((u32)(((x >> 0) & 0x7FFFFFFF)))
/** @} */

/** @defgroup SEC_NEWKEY_HUK_0
  * @brief
  * @{
  */
#define SEC_MASK_HUK_0_3   ((u32)0x000000FF << 24)           /*!< PROT/RMSK1 8'hff  */
#define SEC_HUK_0_3(x)     (((u32)((x) & 0x000000FF) << 24))
#define SEC_GET_HUK_0_3(x) ((u32)(((x >> 24) & 0x000000FF)))
#define SEC_MASK_HUK_0_2   ((u32)0x000000FF << 16)           /*!< PROT/RMSK1 8'hff  */
#define SEC_HUK_0_2(x)     (((u32)((x) & 0x000000FF) << 16))
#define SEC_GET_HUK_0_2(x) ((u32)(((x >> 16) & 0x000000FF)))
#define SEC_MASK_HUK_0_1   ((u32)0x000000FF << 8)            /*!< PROT/RMSK1 8'hff  */
#define SEC_HUK_0_1(x)     (((u32)((x) & 0x000000FF) << 8))
#define SEC_GET_HUK_0_1(x) ((u32)(((x >> 8) & 0x000000FF)))
#define SEC_MASK_HUK_0_0   ((u32)0x000000FF << 0)            /*!< PROT/RMSK1 8'hff  */
#define SEC_HUK_0_0(x)     (((u32)((x) & 0x000000FF) << 0))
#define SEC_GET_HUK_0_0(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup SEC_NEWKEY_HUK_1
  * @brief
  * @{
  */
#define SEC_MASK_HUK_1_3   ((u32)0x000000FF << 24)           /*!< PROT/RMSK1 8'hff  */
#define SEC_HUK_1_3(x)     (((u32)((x) & 0x000000FF) << 24))
#define SEC_GET_HUK_1_3(x) ((u32)(((x >> 24) & 0x000000FF)))
#define SEC_MASK_HUK_1_2   ((u32)0x000000FF << 16)           /*!< PROT/RMSK1 8'hff  */
#define SEC_HUK_1_2(x)     (((u32)((x) & 0x000000FF) << 16))
#define SEC_GET_HUK_1_2(x) ((u32)(((x >> 16) & 0x000000FF)))
#define SEC_MASK_HUK_1_1   ((u32)0x000000FF << 8)            /*!< PROT/RMSK1 8'hff  */
#define SEC_HUK_1_1(x)     (((u32)((x) & 0x000000FF) << 8))
#define SEC_GET_HUK_1_1(x) ((u32)(((x >> 8) & 0x000000FF)))
#define SEC_MASK_HUK_1_0   ((u32)0x000000FF << 0)            /*!< PROT/RMSK1 8'hff  */
#define SEC_HUK_1_0(x)     (((u32)((x) & 0x000000FF) << 0))
#define SEC_GET_HUK_1_0(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup SEC_NEWKEY_HUK_2
  * @brief
  * @{
  */
#define SEC_MASK_HUK_2_3   ((u32)0x000000FF << 24)           /*!< PROT/RMSK1 8'hff  */
#define SEC_HUK_2_3(x)     (((u32)((x) & 0x000000FF) << 24))
#define SEC_GET_HUK_2_3(x) ((u32)(((x >> 24) & 0x000000FF)))
#define SEC_MASK_HUK_2_2   ((u32)0x000000FF << 16)           /*!< PROT/RMSK1 8'hff  */
#define SEC_HUK_2_2(x)     (((u32)((x) & 0x000000FF) << 16))
#define SEC_GET_HUK_2_2(x) ((u32)(((x >> 16) & 0x000000FF)))
#define SEC_MASK_HUK_2_1   ((u32)0x000000FF << 8)            /*!< PROT/RMSK1 8'hff  */
#define SEC_HUK_2_1(x)     (((u32)((x) & 0x000000FF) << 8))
#define SEC_GET_HUK_2_1(x) ((u32)(((x >> 8) & 0x000000FF)))
#define SEC_MASK_HUK_2_0   ((u32)0x000000FF << 0)            /*!< PROT/RMSK1 8'hff  */
#define SEC_HUK_2_0(x)     (((u32)((x) & 0x000000FF) << 0))
#define SEC_GET_HUK_2_0(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup SEC_NEWKEY_HUK_3
  * @brief
  * @{
  */
#define SEC_MASK_HUK_3_3   ((u32)0x000000FF << 24)           /*!< PROT/RMSK1 8'hff  */
#define SEC_HUK_3_3(x)     (((u32)((x) & 0x000000FF) << 24))
#define SEC_GET_HUK_3_3(x) ((u32)(((x >> 24) & 0x000000FF)))
#define SEC_MASK_HUK_3_2   ((u32)0x000000FF << 16)           /*!< PROT/RMSK1 8'hff  */
#define SEC_HUK_3_2(x)     (((u32)((x) & 0x000000FF) << 16))
#define SEC_GET_HUK_3_2(x) ((u32)(((x >> 16) & 0x000000FF)))
#define SEC_MASK_HUK_3_1   ((u32)0x000000FF << 8)            /*!< PROT/RMSK1 8'hff  */
#define SEC_HUK_3_1(x)     (((u32)((x) & 0x000000FF) << 8))
#define SEC_GET_HUK_3_1(x) ((u32)(((x >> 8) & 0x000000FF)))
#define SEC_MASK_HUK_3_0   ((u32)0x000000FF << 0)            /*!< PROT/RMSK1 8'hff  */
#define SEC_HUK_3_0(x)     (((u32)((x) & 0x000000FF) << 0))
#define SEC_GET_HUK_3_0(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup SEC_PKKEY_PK1_0
  * @brief
  * @{
  */
#define SEC_MASK_KEY_PK1_0   ((u32)0xFFFFFFFF << 0)           /*!< PROT/R 32'hffffffff  */
#define SEC_KEY_PK1_0(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_KEY_PK1_0(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SEC_PKKEY_PK1_1
  * @brief
  * @{
  */
#define SEC_MASK_KEY_PK1_1   ((u32)0xFFFFFFFF << 0)           /*!< PROT/R 32'hffffffff  */
#define SEC_KEY_PK1_1(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_KEY_PK1_1(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SEC_PKKEY_PK1_2
  * @brief
  * @{
  */
#define SEC_MASK_KEY_PK1_2   ((u32)0xFFFFFFFF << 0)           /*!< PROT/R 32'hffffffff  */
#define SEC_KEY_PK1_2(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_KEY_PK1_2(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SEC_PKKEY_PK1_3
  * @brief
  * @{
  */
#define SEC_MASK_KEY_PK1_3   ((u32)0xFFFFFFFF << 0)           /*!< PROT/R 32'hffffffff  */
#define SEC_KEY_PK1_3(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_KEY_PK1_3(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SEC_PKKEY_PK1_4
  * @brief
  * @{
  */
#define SEC_MASK_KEY_PK1_4   ((u32)0xFFFFFFFF << 0)           /*!< PROT/R 32'hffffffff  */
#define SEC_KEY_PK1_4(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_KEY_PK1_4(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SEC_PKKEY_PK1_5
  * @brief
  * @{
  */
#define SEC_MASK_KEY_PK1_5   ((u32)0xFFFFFFFF << 0)           /*!< PROT/R 32'hffffffff  */
#define SEC_KEY_PK1_5(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_KEY_PK1_5(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SEC_PKKEY_PK1_6
  * @brief
  * @{
  */
#define SEC_MASK_KEY_PK1_6   ((u32)0xFFFFFFFF << 0)           /*!< PROT/R 32'hffffffff  */
#define SEC_KEY_PK1_6(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_KEY_PK1_6(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SEC_PKKEY_PK1_7
  * @brief
  * @{
  */
#define SEC_MASK_KEY_PK1_7   ((u32)0xFFFFFFFF << 0)           /*!< PROT/R 32'hffffffff  */
#define SEC_KEY_PK1_7(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_KEY_PK1_7(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SEC_PKKEY_PK2_0
  * @brief
  * @{
  */
#define SEC_MASK_KEY_PK2_0   ((u32)0xFFFFFFFF << 0)           /*!< PROT/R 32'hffffffff  */
#define SEC_KEY_PK2_0(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_KEY_PK2_0(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SEC_PKKEY_PK2_1
  * @brief
  * @{
  */
#define SEC_MASK_KEY_PK2_1   ((u32)0xFFFFFFFF << 0)           /*!< PROT/R 32'hffffffff  */
#define SEC_KEY_PK2_1(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_KEY_PK2_1(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SEC_PKKEY_PK2_2
  * @brief
  * @{
  */
#define SEC_MASK_KEY_PK2_2   ((u32)0xFFFFFFFF << 0)           /*!< PROT/R 32'hffffffff  */
#define SEC_KEY_PK2_2(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_KEY_PK2_2(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SEC_PKKEY_PK2_3
  * @brief
  * @{
  */
#define SEC_MASK_KEY_PK2_3   ((u32)0xFFFFFFFF << 0)           /*!< PROT/R 32'hffffffff  */
#define SEC_KEY_PK2_3(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_KEY_PK2_3(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SEC_PKKEY_PK2_4
  * @brief
  * @{
  */
#define SEC_MASK_KEY_PK2_4   ((u32)0xFFFFFFFF << 0)           /*!< PROT/R 32'hffffffff  */
#define SEC_KEY_PK2_4(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_KEY_PK2_4(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SEC_PKKEY_PK2_5
  * @brief
  * @{
  */
#define SEC_MASK_KEY_PK2_5   ((u32)0xFFFFFFFF << 0)           /*!< PROT/R 32'hffffffff  */
#define SEC_KEY_PK2_5(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_KEY_PK2_5(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SEC_PKKEY_PK2_6
  * @brief
  * @{
  */
#define SEC_MASK_KEY_PK2_6   ((u32)0xFFFFFFFF << 0)           /*!< PROT/R 32'hffffffff  */
#define SEC_KEY_PK2_6(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_KEY_PK2_6(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SEC_PKKEY_PK2_7
  * @brief
  * @{
  */
#define SEC_MASK_KEY_PK2_7   ((u32)0xFFFFFFFF << 0)           /*!< PROT/R 32'hffffffff  */
#define SEC_KEY_PK2_7(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_KEY_PK2_7(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SEC_CFG0
  * @brief
  * @{
  */
#define SEC_MASK_SWD_ID   ((u32)0xFFFFFFFF << 0)           /*!< PROT 32'hffffffff  */
#define SEC_SWD_ID(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_SWD_ID(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SEC_CFG1
  * @brief
  * @{
  */
#define SEC_BIT_SWD_PWD_EN                     ((u32)0x00000001 << 0)  /*!< PROT 1'b0  */
#define SEC_BIT_SWD_DBGEN                      ((u32)0x00000001 << 1)  /*!< PROT 1'b0  */
#define SEC_BIT_SWD_NIDEN                      ((u32)0x00000001 << 2)  /*!< PROT 1'b0  */
#define SEC_BIT_SWD_SPIDEN                     ((u32)0x00000001 << 3)  /*!< PROT 1'b0  */
#define SEC_BIT_SWD_SPNIDEN                    ((u32)0x00000001 << 4)  /*!< PROT 1'b0  */
#define SEC_BIT_SWD_PWD_R_PROTECTION_EN        ((u32)0x00000001 << 5)  /*!< PROT 1'b0  */
#define SEC_BIT_SWD_PWD_W_FORBIDDEN_EN         ((u32)0x00000001 << 6)  /*!< PROT 1'b0  */
#define SEC_BIT_HUK_W_FORBIDDEN_EN             ((u32)0x00000001 << 7)  /*!< PROT 1'b0  */
#define SEC_BIT_SWD_SOFTWARE_CTRL_EN           ((u32)0x00000001 << 8)  /*!< PROT 1'b0  */
#define SEC_BIT_PK1_W_FORBIDDEN_EN             ((u32)0x00000001 << 9)  /*!< PROT 1'b0  */
#define SEC_BIT_PK2_W_FORBIDDEN_EN             ((u32)0x00000001 << 10) /*!< PROT 1'b0  */
#define SEC_BIT_S_IPSEC_KEY1_R_PROTECTION_EN   ((u32)0x00000001 << 11) /*!< PROT 1'b0  */
#define SEC_BIT_S_IPSEC_KEY1_W_FORBIDDEN_EN    ((u32)0x00000001 << 12) /*!< PROT 1'b0  */
#define SEC_BIT_S_IPSEC_KEY2_R_PROTECTION_EN   ((u32)0x00000001 << 13) /*!< PROT 1'b0  */
#define SEC_BIT_S_IPSEC_KEY2_W_FORBIDDEN_EN    ((u32)0x00000001 << 14) /*!< PROT 1'b0  */
#define SEC_BIT_NS_IPSEC_KEY1_R_PROTECTION_EN  ((u32)0x00000001 << 15) /*!< PROT 1'b0  */
#define SEC_BIT_NS_IPSEC_KEY1_W_FORBIDDEN_EN   ((u32)0x00000001 << 16) /*!< PROT 1'b0  */
#define SEC_BIT_NS_IPSEC_KEY2_R_PROTECTION_EN  ((u32)0x00000001 << 17) /*!< PROT 1'b0  */
#define SEC_BIT_NS_IPSEC_KEY2_W_FORBIDDEN_EN   ((u32)0x00000001 << 18) /*!< PROT 1'b0  */
#define SEC_BIT_ECDSA_PRI_KEY1_R_PROTECTION_EN ((u32)0x00000001 << 19) /*!< PROT 1'b0  */
#define SEC_BIT_ECDSA_PRI_KEY1_W_FORBIDDEN_EN  ((u32)0x00000001 << 20) /*!< PROT 1'b0  */
#define SEC_BIT_ECDSA_PRI_KEY2_R_PROTECTION_EN ((u32)0x00000001 << 21) /*!< PROT 1'b0  */
#define SEC_BIT_ECDSA_PRI_KEY2_W_FORBIDDEN_EN  ((u32)0x00000001 << 22) /*!< PROT 1'b0  */
#define SEC_BIT_RSIP_KEY1_R_PROTECTION_EN      ((u32)0x00000001 << 23) /*!< PROT 1'b0  */
#define SEC_BIT_RSIP_KEY1_W_FORBIDDEN_EN       ((u32)0x00000001 << 24) /*!< PROT 1'b0  */
#define SEC_BIT_RSIP_KEY2_R_PROTECTION_EN      ((u32)0x00000001 << 25) /*!< PROT 1'b0  */
#define SEC_BIT_RSIP_KEY2_W_FORBIDDEN_EN       ((u32)0x00000001 << 26) /*!< PROT 1'b0  */
#define SEC_BIT_RSIP_MODE_W_FORBIDDEN_EN       ((u32)0x00000001 << 27) /*!< PROT 1'b0  */
#define SEC_BIT_SIC_SECURE_EN                  ((u32)0x00000001 << 28) /*!< PROT 1'b0  */
#define SEC_BIT_CPU_PC_DBG_EN                  ((u32)0x00000001 << 29) /*!< PROT 1'b0  */
#define SEC_BIT_UDF1_TRUSTZONE_EN              ((u32)0x00000001 << 30) /*!< PROT 1'b0  */
#define SEC_BIT_UDF2_TRUSTZONE_EN              ((u32)0x00000001 << 31) /*!< PROT 1'b0  */
/** @} */

/** @defgroup SEC_CFG2
  * @brief
  * @{
  */
#define SEC_BIT_UART_DOWNLOAD_DISABLE        ((u32)0x00000001 << 0)            /*!< PROT/R 1'b0  */
#define SEC_BIT_RSVD0_SW                     ((u32)0x00000001 << 1)            /*!< PROT/R 1'b0  */
#define SEC_BIT_RSIP_EN                      ((u32)0x00000001 << 2)            /*!< PROT/R 1'b0  */
#define SEC_BIT_SECURE_BOOT_EN               ((u32)0x00000001 << 3)            /*!< PROT/R 1'b0  */
#define SEC_BIT_BOOT_RAND_DELAY_EN           ((u32)0x00000001 << 4)            /*!< PROT/R 1'b0  */
#define SEC_BIT_RDP_EN                       ((u32)0x00000001 << 5)            /*!< PROT/R 1'b0  */
#define SEC_BIT_ANTI_ROLLBACK_EN             ((u32)0x00000001 << 6)            /*!< PROT/R 1'b0  */
#define SEC_BIT_FAULT_LOG_PRINT_DIS          ((u32)0x00000001 << 7)            /*!< PROT/R 1'b0  */
#define SEC_MASK_RSIP_MODE                   ((u32)0x00000003 << 8)            /*!< PROT/R 2'b0  */
#define SEC_RSIP_MODE(x)                     (((u32)((x) & 0x00000003) << 8))
#define SEC_GET_RSIP_MODE(x)                 ((u32)(((x >> 8) & 0x00000003)))
#define SEC_BIT_HUK_DERIV_EN                 ((u32)0x00000001 << 10)           /*!< PROT/R 1'b0  */
#define SEC_BIT_USER_PHYSICAL_TRUSTZONE1_EN  ((u32)0x00000001 << 11)           /*!< PROT/R 1'b0  */
#define SEC_BIT_USER_PHYSICAL_TRUSTZONE2_EN  ((u32)0x00000001 << 12)           /*!< PROT/R 1'b0  */
#define SEC_BIT_PK_IDX                       ((u32)0x00000001 << 13)           /*!< PROT/R 1'b0  (ROTPK_hash_index)*/
#define SEC_BIT_SWTRIG_UART_DOWNLOAD_DISABLE ((u32)0x00000001 << 14)           /*!< PROT/R 1'b0  */
#define SEC_BIT_SPIC_PG1B_DISABLE            ((u32)0x00000001 << 15)           /*!< PROT/R 1'b0  */
#define SEC_MASK_RSVD1_SW                    ((u32)0x000000FF << 16)           /*!< PROT/R 8'h0  */
#define SEC_RSVD1_SW(x)                      (((u32)((x) & 0x000000FF) << 16))
#define SEC_GET_RSVD1_SW(x)                  ((u32)(((x >> 16) & 0x000000FF)))
#define SEC_MASK_SECURE_BOOT_AUTH_ALG        ((u32)0x0000000F << 24)           /*!< PROT/R 4'h0  */
#define SEC_SECURE_BOOT_AUTH_ALG(x)          (((u32)((x) & 0x0000000F) << 24))
#define SEC_GET_SECURE_BOOT_AUTH_ALG(x)      ((u32)(((x >> 24) & 0x0000000F)))
#define SEC_MASK_SECURE_BOOT_HASH_ALG        ((u32)0x0000000F << 28)           /*!< PROT/R 4'h0  */
#define SEC_SECURE_BOOT_HASH_ALG(x)          (((u32)((x) & 0x0000000F) << 28))
#define SEC_GET_SECURE_BOOT_HASH_ALG(x)      ((u32)(((x >> 28) & 0x0000000F)))
/** @} */

/** @defgroup SEC_OTA_ADDR
  * @brief
  * @{
  */
#define SEC_MASK_BOOTLOADER_ADDR   ((u32)0x0000FFFF << 0)            /*!< PROT/R 16'hffff  */
#define SEC_BOOTLOADER_ADDR(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define SEC_GET_BOOTLOADER_ADDR(x) ((u32)(((x >> 0) & 0x0000FFFF)))
#define SEC_MASK_RSVD2_SW          ((u32)0x0000FFFF << 16)           /*!< PROT/R 16'hffff  */
#define SEC_RSVD2_SW(x)            (((u32)((x) & 0x0000FFFF) << 16))
#define SEC_GET_RSVD2_SW(x)        ((u32)(((x >> 16) & 0x0000FFFF)))
/** @} */

/** @defgroup SEC_BOOT_VER0
  * @brief
  * @{
  */
#define SEC_MASK_BOOTLOADER_VERSION0   ((u32)0xFFFFFFFF << 0)           /*!< PROT/R 32'hffffffff  */
#define SEC_BOOTLOADER_VERSION0(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_BOOTLOADER_VERSION0(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SEC_BOOT_VER1
  * @brief
  * @{
  */
#define SEC_MASK_BOOTLOADER_VERSION1   ((u32)0xFFFFFFFF << 0)           /*!< PROT/R 32'hffffffff  */
#define SEC_BOOTLOADER_VERSION1(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_BOOTLOADER_VERSION1(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SEC_RMA
  * @brief
  * @{
  */
#define SEC_MASK_RMA_DATA   ((u8)0x000000FF << 0)           /*!< PROT/R 8'hff  If the number of 1 is odd, it will goto RMA state.*/
#define SEC_RMA_DATA(x)     (((u8)((x) & 0x000000FF) << 0))
#define SEC_GET_RMA_DATA(x) ((u8)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup SEC_ROM_PATCH
  * @brief
  * @{
  */
#define SEC_MASK_ROM_PATCH_PG   ((u8)0x00000003 << 0)           /*!< PROT/R 2'b11  ROM patch enable.This Bit will programed by FT if ROM patch do not needed*/
#define SEC_ROM_PATCH_PG(x)     (((u8)((x) & 0x00000003) << 0))
#define SEC_GET_ROM_PATCH_PG(x) ((u8)(((x >> 0) & 0x00000003)))
#define SEC_BIT_ROM_PATCH_LWE1  ((u8)0x00000001 << 2)           /*!< PROT/R 1'b1  ROM patch write protection(protect Low 256bits) : 500-51f*/
#define SEC_BIT_ROM_PATCH_LWE2  ((u8)0x00000001 << 3)           /*!< PROT/R 1'b1  ROM patch write protection(protect Low 256bits) : 520-53f*/
#define SEC_BIT_ROM_PATCH_LWE3  ((u8)0x00000001 << 4)           /*!< PROT/R 1'b1  ROM patch write protection(protect Low 256bits) : 540-55f*/
#define SEC_BIT_ROM_PATCH_LWE4  ((u8)0x00000001 << 5)           /*!< PROT/R 1'b1  ROM patch write protection(protect Low 256bits): 560-57f*/
#define SEC_BIT_ROM_PATCH_LWE5  ((u8)0x00000001 << 6)           /*!< PROT/R 1'b1  ROM patch write protection(protect low 1Kbits)    :580-5ff*/
#define SEC_BIT_ROM_PATCH_HWE   ((u8)0x00000001 << 7)           /*!< PROT/R 1'b1  ROM patch write protection (protect High 2Kbits)*/
/** @} */

/** @defgroup SEC_CFG3
  * @brief
  * @{
  */
#define SEC_BIT_RMA_SWD_PWD_R_PROTECTION_EN ((u16)0x00000001 << 0)           /*!< PROT 1'b1  */
#define SEC_BIT_RMA_SWD_PWD_W_FORBIDDEN_EN  ((u16)0x00000001 << 1)           /*!< PROT 1'b1  */
#define SEC_BIT_RMA_PK_W_FORBIDDEN_EN       ((u16)0x00000001 << 2)           /*!< PROT 1'b1  */
#define SEC_MASK_OTP_RSVD0                  ((u16)0x0000001F << 3)           /*!< PROT 5'h1f  */
#define SEC_OTP_RSVD0(x)                    (((u16)((x) & 0x0000001F) << 3))
#define SEC_GET_OTP_RSVD0(x)                ((u16)(((x >> 3) & 0x0000001F)))
/** @} */

/** @defgroup SEC_PKKEY1_RMA_PK_0
  * @brief
  * @{
  */
#define SEC_MASK_KEY_RMA_PK_0   ((u32)0xFFFFFFFF << 0)           /*!< PROT/R 32'hffffffff  */
#define SEC_KEY_RMA_PK_0(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_KEY_RMA_PK_0(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SEC_PKKEY1_RMA_PK_1
  * @brief
  * @{
  */
#define SEC_MASK_KEY_RMA_PK_1   ((u32)0xFFFFFFFF << 0)           /*!< PROT/R 32'hffffffff  */
#define SEC_KEY_RMA_PK_1(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_KEY_RMA_PK_1(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SEC_PKKEY1_RMA_PK_2
  * @brief
  * @{
  */
#define SEC_MASK_KEY_RMA_PK_2   ((u32)0xFFFFFFFF << 0)           /*!< PROT/R 32'hffffffff  */
#define SEC_KEY_RMA_PK_2(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_KEY_RMA_PK_2(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SEC_PKKEY1_RMA_PK_3
  * @brief
  * @{
  */
#define SEC_MASK_KEY_RMA_PK_3   ((u32)0xFFFFFFFF << 0)           /*!< PROT/R 32'hffffffff  */
#define SEC_KEY_RMA_PK_3(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_KEY_RMA_PK_3(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SEC_PKKEY1_RMA_PK_4
  * @brief
  * @{
  */
#define SEC_MASK_KEY_RMA_PK_4   ((u32)0xFFFFFFFF << 0)           /*!< PROT/R 32'hffffffff  */
#define SEC_KEY_RMA_PK_4(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_KEY_RMA_PK_4(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SEC_PKKEY1_RMA_PK_5
  * @brief
  * @{
  */
#define SEC_MASK_KEY_RMA_PK_5   ((u32)0xFFFFFFFF << 0)           /*!< PROT/R 32'hffffffff  */
#define SEC_KEY_RMA_PK_5(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_KEY_RMA_PK_5(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SEC_PKKEY1_RMA_PK_6
  * @brief
  * @{
  */
#define SEC_MASK_KEY_RMA_PK_6   ((u32)0xFFFFFFFF << 0)           /*!< PROT/R 32'hffffffff  */
#define SEC_KEY_RMA_PK_6(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_KEY_RMA_PK_6(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SEC_PKKEY1_RMA_PK_7
  * @brief
  * @{
  */
#define SEC_MASK_KEY_RMA_PK_7   ((u32)0xFFFFFFFF << 0)           /*!< PROT/R 32'hffffffff  */
#define SEC_KEY_RMA_PK_7(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SEC_GET_KEY_RMA_PK_7(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/*==========SEC Register Address Definition==========*/
#define SEC_OTP_SYSCFG0              0x100
#define SEC_OTP_SYSCFG1              0x104
#define SEC_OTP_SYSCFG2              0x108
#define SEC_OTP_SYSCFG3              0x10C
#define SEC_OTP_HWCFG                0x11C
#define SEC_NEWKEY_HUK_0             0x310
#define SEC_NEWKEY_HUK_1             0x314
#define SEC_NEWKEY_HUK_2             0x318
#define SEC_NEWKEY_HUK_3             0x31C
#define SEC_PKKEY_PK1_0              0x320
#define SEC_PKKEY_PK1_1              0x324
#define SEC_PKKEY_PK1_2              0x328
#define SEC_PKKEY_PK1_3              0x32C
#define SEC_PKKEY_PK1_4              0x330
#define SEC_PKKEY_PK1_5              0x334
#define SEC_PKKEY_PK1_6              0x338
#define SEC_PKKEY_PK1_7              0x33C
#define SEC_PKKEY_PK2_0              0x340
#define SEC_PKKEY_PK2_1              0x344
#define SEC_PKKEY_PK2_2              0x348
#define SEC_PKKEY_PK2_3              0x34C
#define SEC_PKKEY_PK2_4              0x350
#define SEC_PKKEY_PK2_5              0x354
#define SEC_PKKEY_PK2_6              0x358
#define SEC_PKKEY_PK2_7              0x35C
#define SEC_CFG0                     0x360
#define SEC_CFG1                     0x364
#define SEC_CFG2                     0x368
#define SEC_OTA_ADDR                 0x36C
#define SEC_BOOT_VER0                0x378
#define SEC_BOOT_VER1                0x37C
#define SEC_RMA                      0x700
#define SEC_ROM_PATCH                0x701
#define SEC_CFG3                     0x702
#define SEC_PKKEY1_RMA_PK_0          0x720
#define SEC_PKKEY1_RMA_PK_1          0x724
#define SEC_PKKEY1_RMA_PK_2          0x728
#define SEC_PKKEY1_RMA_PK_3          0x72C
#define SEC_PKKEY1_RMA_PK_4          0x730
#define SEC_PKKEY1_RMA_PK_5          0x734
#define SEC_PKKEY1_RMA_PK_6          0x738
#define SEC_PKKEY1_RMA_PK_7          0x73C
#define SEC_OTP_DUMMY                0x740

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your defination here
/*==========SWR Calibration Register Address Definition==========*/
#define		SEC_VOL_AUDIO_VREF_1V8_F		0x7DE
#define		SEC_VOL_AUDIO_VREF_1V8_S		0x7DF
#define		SEC_VOL_SWR_PFM_125		0x7E0
#define		SEC_VOL_SWR_PWM_135		0x7E1
#define		SEC_VOL_SWR_PFM_09		0x7E2
#define		SEC_VOL_SWR_PWM_10		0x7E3
#define		SEC_VOL_AON_LDO_09		0x7E4
#define		SEC_VOL_CORE_LDO_10		0x7E5
#define		SEC_VOL_ANA_LDO_180		0x7E6
/** @} */


#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

#endif

/** @} */

/** @} */