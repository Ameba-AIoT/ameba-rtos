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

/** @defgroup RD_WRR0
  * @brief Read weight round-robin configuration 0
  * @{
  */
#define RXI300_MASK_RD_CPU_PSRAM_FN   ((u32)0x0000FFFF << 16)           /*!< R/W 0x4924  */
#define RXI300_RD_CPU_PSRAM_FN(x)     (((u32)((x) & 0x0000FFFF) << 16))
#define RXI300_GET_RD_CPU_PSRAM_FN(x) ((u32)(((x >> 16) & 0x0000FFFF)))
#define RXI300_MASK_RD_CPU_SPIC_FN    ((u32)0x0000FFFF << 0)            /*!< R/W 0x4924  */
#define RXI300_RD_CPU_SPIC_FN(x)      (((u32)((x) & 0x0000FFFF) << 0))
#define RXI300_GET_RD_CPU_SPIC_FN(x)  ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup RD_WRR1
  * @brief Read weight round-robin configuration 1
  * @{
  */
#define RXI300_MASK_RD_PLFM1_FN      ((u32)0x0000FFFF << 16)           /*!< R/W 0x4924  */
#define RXI300_RD_PLFM1_FN(x)        (((u32)((x) & 0x0000FFFF) << 16))
#define RXI300_GET_RD_PLFM1_FN(x)    ((u32)(((x >> 16) & 0x0000FFFF)))
#define RXI300_MASK_RD_CPU_SRAM_FN   ((u32)0x0000FFFF << 0)            /*!< R/W 0x4924  */
#define RXI300_RD_CPU_SRAM_FN(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define RXI300_GET_RD_CPU_SRAM_FN(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup RD_WRR2
  * @brief Read weight round-robin configuration 2
  * @{
  */
#define RXI300_MASK_RD_HPERI_SRAM_FN   ((u32)0x000000FF << 24)           /*!< R/W 0x55  */
#define RXI300_RD_HPERI_SRAM_FN(x)     (((u32)((x) & 0x000000FF) << 24))
#define RXI300_GET_RD_HPERI_SRAM_FN(x) ((u32)(((x >> 24) & 0x000000FF)))
#define RXI300_MASK_RD_PSRAM_FN        ((u32)0x000000FF << 16)           /*!< R/W 0x55  */
#define RXI300_RD_PSRAM_FN(x)          (((u32)((x) & 0x000000FF) << 16))
#define RXI300_GET_RD_PSRAM_FN(x)      ((u32)(((x >> 16) & 0x000000FF)))
#define RXI300_MASK_RD_SPIC_FN         ((u32)0x000000FF << 8)            /*!< R/W 0x55  */
#define RXI300_RD_SPIC_FN(x)           (((u32)((x) & 0x000000FF) << 8))
#define RXI300_GET_RD_SPIC_FN(x)       ((u32)(((x >> 8) & 0x000000FF)))
#define RXI300_MASK_RD_SRAM_FN         ((u32)0x000000FF << 0)            /*!< R/W 0x55  */
#define RXI300_RD_SRAM_FN(x)           (((u32)((x) & 0x000000FF) << 0))
#define RXI300_GET_RD_SRAM_FN(x)       ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup RD_WRR3
  * @brief Read weight round-robin configuration 3
  * @{
  */
#define RXI300_MASK_RD_GDMA_FN   ((u32)0x000000FF << 0)           /*!< R/W 0x55  */
#define RXI300_RD_GDMA_FN(x)     (((u32)((x) & 0x000000FF) << 0))
#define RXI300_GET_RD_GDMA_FN(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup WR_WRR0
  * @brief Write weight round-robin configuration 0
  * @{
  */
#define RXI300_MASK_WR_CPU_PSRAM_FN   ((u32)0x0000FFFF << 16)           /*!< R/W 0x4924  */
#define RXI300_WR_CPU_PSRAM_FN(x)     (((u32)((x) & 0x0000FFFF) << 16))
#define RXI300_GET_WR_CPU_PSRAM_FN(x) ((u32)(((x >> 16) & 0x0000FFFF)))
#define RXI300_MASK_WR_CPU_SPIC_FN    ((u32)0x0000FFFF << 0)            /*!< R/W 0x4924  */
#define RXI300_WR_CPU_SPIC_FN(x)      (((u32)((x) & 0x0000FFFF) << 0))
#define RXI300_GET_WR_CPU_SPIC_FN(x)  ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup WR_WRR1
  * @brief Write weight round-robin configuration 1
  * @{
  */
#define RXI300_MASK_WR_PLFM1_FN      ((u32)0x0000FFFF << 16)           /*!< R/W 0x4924  */
#define RXI300_WR_PLFM1_FN(x)        (((u32)((x) & 0x0000FFFF) << 16))
#define RXI300_GET_WR_PLFM1_FN(x)    ((u32)(((x >> 16) & 0x0000FFFF)))
#define RXI300_MASK_WR_CPU_SRAM_FN   ((u32)0x0000FFFF << 0)            /*!< R/W 0x4924  */
#define RXI300_WR_CPU_SRAM_FN(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define RXI300_GET_WR_CPU_SRAM_FN(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup WR_WRR2
  * @brief Write weight round-robin configuration 2
  * @{
  */
#define RXI300_MASK_WR_HPERI_SRAM_FN   ((u32)0x000000FF << 24)           /*!< R/W 0x55  */
#define RXI300_WR_HPERI_SRAM_FN(x)     (((u32)((x) & 0x000000FF) << 24))
#define RXI300_GET_WR_HPERI_SRAM_FN(x) ((u32)(((x >> 24) & 0x000000FF)))
#define RXI300_MASK_WR_PSRAM_FN        ((u32)0x000000FF << 16)           /*!< R/W 0x55  */
#define RXI300_WR_PSRAM_FN(x)          (((u32)((x) & 0x000000FF) << 16))
#define RXI300_GET_WR_PSRAM_FN(x)      ((u32)(((x >> 16) & 0x000000FF)))
#define RXI300_MASK_WR_SPIC_FN         ((u32)0x000000FF << 8)            /*!< R/W 0x55  */
#define RXI300_WR_SPIC_FN(x)           (((u32)((x) & 0x000000FF) << 8))
#define RXI300_GET_WR_SPIC_FN(x)       ((u32)(((x >> 8) & 0x000000FF)))
#define RXI300_MASK_WR_SRAM_FN         ((u32)0x000000FF << 0)            /*!< R/W 0x55  */
#define RXI300_WR_SRAM_FN(x)           (((u32)((x) & 0x000000FF) << 0))
#define RXI300_GET_WR_SRAM_FN(x)       ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup WR_WRR3
  * @brief Write weight round-robin configuration 3
  * @{
  */
#define RXI300_MASK_SRAM_RD_WEIGHT   ((u32)0x000001FF << 8)           /*!< R/W 0x2  */
#define RXI300_SRAM_RD_WEIGHT(x)     (((u32)((x) & 0x000001FF) << 8))
#define RXI300_GET_SRAM_RD_WEIGHT(x) ((u32)(((x >> 8) & 0x000001FF)))
#define RXI300_MASK_WR_GDMA_FN       ((u32)0x000000FF << 0)           /*!< R/W 0x55  */
#define RXI300_WR_GDMA_FN(x)         (((u32)((x) & 0x000000FF) << 0))
#define RXI300_GET_WR_GDMA_FN(x)     ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup PLD0
  * @brief Error log payload register0
  * @{
  */
#define RXI300_MASK_ERR_PLD0   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  */
#define RXI300_ERR_PLD0(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_ERR_PLD0(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup PLD1
  * @brief Error log payload register1
  * @{
  */
#define RXI300_MASK_ERR_PLD1   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  */
#define RXI300_ERR_PLD1(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_ERR_PLD1(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ID
  * @brief Error log Id information register
  * @{
  */
#define RXI300_MASK_ERR_ID   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  */
#define RXI300_ERR_ID(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_ERR_ID(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ADR0
  * @brief Error log low 32-bit address register
  * @{
  */
#define RXI300_MASK_ERR_ADR0   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  */
#define RXI300_ERR_ADR0(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_ERR_ADR0(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ADR1
  * @brief Error log high 32-bit address register
  * @{
  */
#define RXI300_MASK_ERR_ADR1   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  */
#define RXI300_ERR_ADR1(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_ERR_ADR1(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup CODE
  * @brief Error log error code register
  * @{
  */
#define RXI300_MASK_ERR_CODE   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  */
#define RXI300_ERR_CODE(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_ERR_CODE(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup INT_CLR
  * @brief Interrupt clear register
  * @{
  */
#define RXI300_BIT_ERR_INTR_CLR ((u32)0x00000001 << 0) /*!< R/W 0x0  Write 0x1 to clear interrupt and error log, self return to 0x0*/
/** @} */

/** @defgroup TIMEOUT_GUARD_EN
  * @brief Timeout guardian enable register
  * @{
  */
#define RXI300_BIT_TMO_GUARDIAN_EN_PLFM1_MA ((u32)0x00000001 << 1) /*!< R/W 0x0  */
#define RXI300_BIT_TMO_GUARDIAN_EN_CPU1_MA  ((u32)0x00000001 << 0) /*!< R/W 0x0  */
/** @} */

/** @defgroup TIMEOUT_CLEAR_N
  * @brief Timeout clear register
  * @{
  */
#define RXI300_BIT_TMO_CLR_SW_RST_N_PLFM1_MA ((u32)0x00000001 << 1) /*!< R/W 0x1  */
#define RXI300_BIT_TMO_CLR_SW_RST_N_CPU1_MA  ((u32)0x00000001 << 0) /*!< R/W 0x1  */
/** @} */

/** @defgroup TIMEOUT_MON_CFG0
  * @brief Timeout monitor config register
  * @{
  */
#define RXI300_MASK_TMO_GRADE_SELECT_CPU1_MA    ((u32)0x0000000F << 16)           /*!< R/W 0x0  */
#define RXI300_TMO_GRADE_SELECT_CPU1_MA(x)      (((u32)((x) & 0x0000000F) << 16))
#define RXI300_GET_TMO_GRADE_SELECT_CPU1_MA(x)  ((u32)(((x >> 16) & 0x0000000F)))
#define RXI300_MASK_TMO_THRESHOLD_OUT_CPU1_MA   ((u32)0x000000FF << 0)            /*!< R/W 0xff  */
#define RXI300_TMO_THRESHOLD_OUT_CPU1_MA(x)     (((u32)((x) & 0x000000FF) << 0))
#define RXI300_GET_TMO_THRESHOLD_OUT_CPU1_MA(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup TIMEOUT_MON_CFG1
  * @brief Timeout monitor config register
  * @{
  */
#define RXI300_MASK_TMO_GRADE_SELECT_PLFM1_MA    ((u32)0x0000000F << 16)           /*!< R/W 0x0  */
#define RXI300_TMO_GRADE_SELECT_PLFM1_MA(x)      (((u32)((x) & 0x0000000F) << 16))
#define RXI300_GET_TMO_GRADE_SELECT_PLFM1_MA(x)  ((u32)(((x >> 16) & 0x0000000F)))
#define RXI300_MASK_TMO_THRESHOLD_OUT_PLFM1_MA   ((u32)0x000000FF << 0)            /*!< R/W 0xff  */
#define RXI300_TMO_THRESHOLD_OUT_PLFM1_MA(x)     (((u32)((x) & 0x000000FF) << 0))
#define RXI300_GET_TMO_THRESHOLD_OUT_PLFM1_MA(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup PPC1_REG
  * @brief PPC configure register 1
  * @{
  */
#define RXI300_BIT_SPIC_USER_MODE            ((u32)0x00000001 << 17) /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_PSRAM_SPIC_USERMODE       ((u32)0x00000001 << 16) /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_PSRAM_PHY                 ((u32)0x00000001 << 15) /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_I2C1                      ((u32)0x00000001 << 14) /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_I2C0                      ((u32)0x00000001 << 13) /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_TIMER_4_8_PULSE_PWM_TIMER ((u32)0x00000001 << 12) /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_UART_LOG                  ((u32)0x00000001 << 11) /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_UART2                     ((u32)0x00000001 << 10) /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_UART1                     ((u32)0x00000001 << 9)  /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_UART0                     ((u32)0x00000001 << 8)  /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_SPI1                      ((u32)0x00000001 << 7)  /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_SPI0                      ((u32)0x00000001 << 6)  /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_GDMA0_CFG                 ((u32)0x00000001 << 5)  /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_PKE_CFG                   ((u32)0x00000001 << 4)  /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_SPORT0_I2S                ((u32)0x00000001 << 3)  /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_SDIO_DEVICE_CFG           ((u32)0x00000001 << 2)  /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_SDN_CFG                   ((u32)0x00000001 << 1)  /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_WIFI_CFG                  ((u32)0x00000001 << 0)  /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
/** @} */

/** @defgroup PPC2_REG
  * @brief PPC configure register 2
  * @{
  */
#define RXI300_BIT_LALU_KEY_MANAGE  ((u32)0x00000001 << 19) /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_AES_SHA_DMA      ((u32)0x00000001 << 18) /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_SHA_CFG          ((u32)0x00000001 << 17) /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_AES_CFG          ((u32)0x00000001 << 16) /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_IR               ((u32)0x00000001 << 12) /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_RSIP             ((u32)0x00000001 << 11) /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_RXI300           ((u32)0x00000001 << 10) /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_TRNG_PORT1_PORT2 ((u32)0x00000001 << 9)  /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_TIMER_0_3_BASIC  ((u32)0x00000001 << 8)  /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_PMC_TIMER_0_1    ((u32)0x00000001 << 7)  /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_DBG_TIMER        ((u32)0x00000001 << 6)  /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_IPC_P1           ((u32)0x00000001 << 5)  /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_IPC              ((u32)0x00000001 << 4)  /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_ADC_ADC_COMP     ((u32)0x00000001 << 3)  /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_GPIO_A_B_C       ((u32)0x00000001 << 2)  /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_SYSON            ((u32)0x00000001 << 1)  /*!< R/W 0x1  1: Non-secure attribution 0: Secure attribution*/
#define RXI300_BIT_OTPC_CFG         ((u32)0x00000001 << 0)  /*!< R/W 0x0  1: Non-secure attribution 0: Secure attribution*/
/** @} */

/** @defgroup PPC_LOCK
  * @brief PPC Lock register
  * @{
  */
#define RXI300_BIT_LOCK ((u32)0x00000001 << 0) /*!< R/W 0x0  PPC lock register. When writing 1'b1 to this nit, PPC will be locked. Note: It can not be written to 1'b0 again until system reset to prevent modification. When PPC is locked, you can not program PPC registers unless system reset.*/
/** @} */

/** @defgroup MST_EN
  * @brief Maser enable register
  * @{
  */
#define RXI300_BIT_TIMEOUT_TRIGGER_N    ((u32)0x00000001 << 1) /*!< R/W 0x1  Master enable signal, all master agent of plfm1-to-plfm0 are: 0: enable  1: disable*/
#define RXI300_BIT_PLFM1_MASTER_DISABLE ((u32)0x00000001 << 0) /*!< R/W 0x0  1: disable all master agent from plfm1 to plfm0*/
/** @} */

/** @defgroup MST_TIMEOUT_STATUS
  * @brief Maser timeout status register
  * @{
  */
#define RXI300_BIT_TMO_TRIG_PLFM1  ((u32)0x00000001 << 5) /*!< RO 0x0  Status of timeout monitor (PLFM1). The timeout is: 0: disable 1: enable*/
#define RXI300_BIT_TMO_TRIG_CPU1   ((u32)0x00000001 << 4) /*!< RO 0x0  Status of timeout monitor (CPU1). The timeout is: 0: disable 1: enable*/
#define RXI300_BIT_MA_STATUS_PLFM1 ((u32)0x00000001 << 1) /*!< RO 0x1  Status of master agent (PLFM1). The master is: 0: enable 1: disable*/
#define RXI300_BIT_MA_STATUS_CPU1  ((u32)0x00000001 << 0) /*!< RO 0x1  Status of master agent (CPU1). The master is: 0: enable 1: disable*/
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
#define RXI300_MASK_IDAU_WRITE_CTRL    ((u32)0x0000000F << 16)           /*!< R/W 0h  Region entry enable After setting WRITE_CTRL bits as 1, corresponding regions is set as write-forbidden.Note that if WRITE_CTRL bits are enabled, the region remains secure even if ba*i/la*i is set. [16]: enable entry for ba0/la0 0: disable  1: enable                                                                                                                                                                                                                                                     [17]: enable entry for ba1/la1 0: disable  1: enable                                                                                                                                                                                                                                              [18]: enable entry for ba2/la2 0: disable  1: enable [19]: enable entry for ba3/la3 0: disable  1: enable*/
#define RXI300_IDAU_WRITE_CTRL(x)      (((u32)((x) & 0x0000000F) << 16))
#define RXI300_GET_IDAU_WRITE_CTRL(x)  ((u32)(((x >> 16) & 0x0000000F)))
#define RXI300_MASK_IDAU_SECURE_CTRL   ((u32)0x0000000F << 0)            /*!< R/W 0h  Region entry enable After setting SECURE_CTRL bits as 1, corresponding regions is set as non-secure.Note that if SECURE_CTRL bits are enabled, the region remains secure even if ba*i/la*i is set. [0]: enable entry for ba0/la0 0: disable  1: enable                                                                                                                                                                                                                                                 [1]: enable entry for ba1/la1 0: disable  1: enable                                                                                                                                                                                                                                                  [2]: enable entry for ba2/la2 0: disable  1: enable [3]: enable entry for ba3/la3 0: disable  1: enable*/
#define RXI300_IDAU_SECURE_CTRL(x)     (((u32)((x) & 0x0000000F) << 0))
#define RXI300_GET_IDAU_SECURE_CTRL(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup IDAU_LOCK
  * @brief Lock control register in IDAU
  * @{
  */
#define RXI300_BIT_IDAU_LOCK ((u32)0x00000001 << 0) /*!< R/W 0h  Lock write transfer for all registers in IDAU (including lock register itself) until system reset if it is ebabled. 0: disable lock 1: enable lock*/
/** @} */

/** @defgroup IDAU2_BAx
  * @brief Base address register x in IDAU
  * @{
  */
#define RXI300_MASK_IDAU2_BAx   ((u32)0x0000FFFF << 12)           /*!< R/W 0h  It's the base address of region x which can be set as non-secure.                                                                                                       [11:0]: reserved [27:12]: base address of region x                                                                                                                                                                                                [31:28]: reserved*/
#define RXI300_IDAU2_BAx(x)     (((u32)((x) & 0x0000FFFF) << 12))
#define RXI300_GET_IDAU2_BAx(x) ((u32)(((x >> 12) & 0x0000FFFF)))
/** @} */

/** @defgroup IDAU2_LAx
  * @brief Top address register x in IDAU
  * @{
  */
#define RXI300_MASK_IDAU2_LAx   ((u32)0x0000FFFF << 12)           /*!< R/W FFFFh  It's the top address of region x which can be set as non-secure. [11:0]: reserved [27:12]: top address of region x                                                                                                                                                                                                [31:28]: reserved*/
#define RXI300_IDAU2_LAx(x)     (((u32)((x) & 0x0000FFFF) << 12))
#define RXI300_GET_IDAU2_LAx(x) ((u32)(((x >> 12) & 0x0000FFFF)))
/** @} */

/** @defgroup IDAU2_CTRL
  * @brief Enable control register in IDAU
  * @{
  */
#define RXI300_MASK_IDAU2_WRITE_CTRL    ((u32)0x0000000F << 16)           /*!< R/W 0h  Region entry enable After setting WRITE_CTRL bits as 1, corresponding regions is set as write-forbidden.Note that if WRITE_CTRL bits are enabled, the region remains secure even if ba*i/la*i is set. [16]: enable entry for ba0/la0 0: disable  1: enable                                                                                                                                                                                                                                                     [17]: enable entry for ba1/la1 0: disable  1: enable                                                                                                                                                                                                                                              [18]: enable entry for ba2/la2 0: disable  1: enable [19]: enable entry for ba3/la3 0: disable  1: enable*/
#define RXI300_IDAU2_WRITE_CTRL(x)      (((u32)((x) & 0x0000000F) << 16))
#define RXI300_GET_IDAU2_WRITE_CTRL(x)  ((u32)(((x >> 16) & 0x0000000F)))
#define RXI300_MASK_IDAU2_SECURE_CTRL   ((u32)0x0000000F << 0)            /*!< R/W 0h  Region entry enable After setting SECURE_CTRL bits as 1, corresponding regions is set as non-secure.Note that if SECURE_CTRL bits are enabled, the region remains secure even if ba*i/la*i is set. [0]: enable entry for ba0/la0 0: disable  1: enable                                                                                                                                                                                                                                                 [1]: enable entry for ba1/la1 0: disable  1: enable                                                                                                                                                                                                                                                  [2]: enable entry for ba2/la2 0: disable  1: enable [3]: enable entry for ba3/la3 0: disable  1: enable*/
#define RXI300_IDAU2_SECURE_CTRL(x)     (((u32)((x) & 0x0000000F) << 0))
#define RXI300_GET_IDAU2_SECURE_CTRL(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup IDAU2_LOCK
  * @brief Lock control register in IDAU
  * @{
  */
#define RXI300_BIT_IDAU2_LOCK ((u32)0x00000001 << 0) /*!< R/W 0h  Lock write transfer for all registers in IDAU (including lock register itself) until system reset if it is ebabled. 0: disable lock 1: enable lock*/
/** @} */

/** @defgroup IDAU3_BAx
  * @brief Base address register x in IDAU
  * @{
  */
#define RXI300_MASK_IDAU3_BAx   ((u32)0x0000FFFF << 12)           /*!< R/W 0h  It's the base address of region x which can be set as non-secure.                                                                                                       [11:0]: reserved [27:12]: base address of region x                                                                                                                                                                                                [31:28]: reserved*/
#define RXI300_IDAU3_BAx(x)     (((u32)((x) & 0x0000FFFF) << 12))
#define RXI300_GET_IDAU3_BAx(x) ((u32)(((x >> 12) & 0x0000FFFF)))
/** @} */

/** @defgroup IDAU3_LAx
  * @brief Top address register x in IDAU
  * @{
  */
#define RXI300_MASK_IDAU3_LAx   ((u32)0x0000FFFF << 12)           /*!< R/W FFFFh  It's the top address of region x which can be set as non-secure. [11:0]: reserved [27:12]: top address of region x                                                                                                                                                                                                [31:28]: reserved*/
#define RXI300_IDAU3_LAx(x)     (((u32)((x) & 0x0000FFFF) << 12))
#define RXI300_GET_IDAU3_LAx(x) ((u32)(((x >> 12) & 0x0000FFFF)))
/** @} */

/** @defgroup IDAU3_CTRL
  * @brief Enable control register in IDAU
  * @{
  */
#define RXI300_MASK_IDAU3_WRITE_CTRL    ((u32)0x0000000F << 16)           /*!< R/W 0h  Region entry enable After setting WRITE_CTRL bits as 1, corresponding regions is set as write-forbidden.Note that if WRITE_CTRL bits are enabled, the region remains secure even if ba*i/la*i is set. [16]: enable entry for ba0/la0 0: disable  1: enable                                                                                                                                                                                                                                                     [17]: enable entry for ba1/la1 0: disable  1: enable                                                                                                                                                                                                                                              [18]: enable entry for ba2/la2 0: disable  1: enable [19]: enable entry for ba3/la3 0: disable  1: enable*/
#define RXI300_IDAU3_WRITE_CTRL(x)      (((u32)((x) & 0x0000000F) << 16))
#define RXI300_GET_IDAU3_WRITE_CTRL(x)  ((u32)(((x >> 16) & 0x0000000F)))
#define RXI300_MASK_IDAU3_SECURE_CTRL   ((u32)0x0000000F << 0)            /*!< R/W 0h  Region entry enable After setting SECURE_CTRL bits as 1, corresponding regions is set as non-secure.Note that if SECURE_CTRL bits are enabled, the region remains secure even if ba*i/la*i is set. [0]: enable entry for ba0/la0 0: disable  1: enable                                                                                                                                                                                                                                                 [1]: enable entry for ba1/la1 0: disable  1: enable                                                                                                                                                                                                                                                  [2]: enable entry for ba2/la2 0: disable  1: enable [3]: enable entry for ba3/la3 0: disable  1: enable*/
#define RXI300_IDAU3_SECURE_CTRL(x)     (((u32)((x) & 0x0000000F) << 0))
#define RXI300_GET_IDAU3_SECURE_CTRL(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup IDAU3_LOCK
  * @brief Lock control register in IDAU
  * @{
  */
#define RXI300_BIT_IDAU3_LOCK ((u32)0x00000001 << 0) /*!< R/W 0h  Lock write transfer for all registers in IDAU (including lock register itself) until system reset if it is ebabled. 0: disable lock 1: enable lock*/
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup RXI300_Exported_Types RXI300 Exported Types
  * @{
  */

/** @brief RXI300 Port
  */
typedef struct {
	__IO uint32_t PPC1_REG;  /*!< PPC CONFIGURE REGISTER 1 Register */
	__IO uint32_t PPC2_REG;  /*!< PPC CONFIGURE REGISTER 2 Register */
	__IO uint32_t PPC_LOCK;  /*!< PPC LOCK REGISTER */
} PPC_TypeDef;

/** @brief RXI300 Port
  */
typedef struct {
	__IO uint32_t IDAU_BAx;  /*!< BASE ADDRESS REGISTER x IN IDAU Register */
	__IO uint32_t IDAU_LAx;  /*!< TOP ADDRESS REGISTER x IN IDAU Register */
} MPC_EntryTypeDef;

/** @brief RXI300 Register Declaration
  */

typedef struct {
	__IO uint32_t RD_WRR0                       ;  /*!< READ WEIGHT ROUND-ROBIN CONFIGURATION 0 Register,  Address offset:0x000 */
	__IO uint32_t RD_WRR1                       ;  /*!< READ WEIGHT ROUND-ROBIN CONFIGURATION 1 Register,  Address offset:0x004 */
	__IO uint32_t RD_WRR2                       ;  /*!< READ WEIGHT ROUND-ROBIN CONFIGURATION 2 Register,  Address offset:0x008 */
	__IO uint32_t RD_WRR3                       ;  /*!< READ WEIGHT ROUND-ROBIN CONFIGURATION 3 Register,  Address offset:0x00C */
	__IO uint32_t WR_WRR0                       ;  /*!< WRITE WEIGHT ROUND-ROBIN CONFIGURATION 0 Register,  Address offset:0x010 */
	__IO uint32_t WR_WRR1                       ;  /*!< WRITE WEIGHT ROUND-ROBIN CONFIGURATION 1 Register,  Address offset:0x014 */
	__IO uint32_t WR_WRR2                       ;  /*!< WRITE WEIGHT ROUND-ROBIN CONFIGURATION 2 Register,  Address offset:0x018 */
	__IO uint32_t WR_WRR3                       ;  /*!< WRITE WEIGHT ROUND-ROBIN CONFIGURATION 3 Register,  Address offset:0x01C */
	__IO uint32_t RSVD0[56]                     ;  /*!< Reserved,  Address offset:0x020-0x0FF */
	__IO uint32_t PLD0                          ;  /*!< ERROR LOG PAYLOAD REGISTER0 Register,  Address offset:0x100 */
	__IO uint32_t PLD1                          ;  /*!< ERROR LOG PAYLOAD REGISTER1 Register,  Address offset:0x104 */
	__IO uint32_t ID                            ;  /*!< ERROR LOG ID INFORMATION REGISTER,  Address offset:0x108 */
	__IO uint32_t ADR0                          ;  /*!< ERROR LOG LOW 32-BIT ADDRESS REGISTER,  Address offset:0x10C */
	__IO uint32_t ADR1                          ;  /*!< ERROR LOG HIGH 32-BIT ADDRESS REGISTER,  Address offset:0x110 */
	__IO uint32_t RSVD1[7]                      ;  /*!< Reserved,  Address offset:0x114-0x12F */
	__IO uint32_t CODE                          ;  /*!< ERROR LOG ERROR CODE REGISTER,  Address offset:0x130 */
	__IO uint32_t RSVD2[2]                      ;  /*!< Reserved,  Address offset:0x134-0x13B */
	__IO uint32_t INT_CLR                       ;  /*!< INTERRUPT CLEAR REGISTER,  Address offset:0x13C */
	__IO uint32_t RSVD3[112]                    ;  /*!< Reserved,  Address offset:0x140-0x2FF */
	__IO uint32_t TIMEOUT_GUARD_EN              ;  /*!< TIMEOUT GUARDIAN ENABLE REGISTER,  Address offset:0x300 */
	__IO uint32_t RSVD4[15]                     ;  /*!< Reserved,  Address offset:0x304-0x33F */
	__IO uint32_t TIMEOUT_CLEAR_N               ;  /*!< TIMEOUT CLEAR REGISTER,  Address offset:0x340 */
	__IO uint32_t RSVD5[15]                     ;  /*!< Reserved,  Address offset:0x344-0x37F */
	__IO uint32_t TIMEOUT_MON_CFG0              ;  /*!< TIMEOUT MONITOR CONFIG REGISTER,  Address offset:0x380 */
	__IO uint32_t TIMEOUT_MON_CFG1              ;  /*!< TIMEOUT MONITOR CONFIG REGISTER,  Address offset:0x384 */
	__IO uint32_t RSVD6[30]                     ;  /*!< Reserved,  Address offset:0x388-0x3FF */
	PPC_TypeDef PPC                             ;  /*!< RXI300 IP have 1 ports,  Address offset:0x400-0x408 */
	__IO uint32_t MST_EN                        ;  /*!< MASER ENABLE REGISTER,  Address offset:0x40C */
	__IO uint32_t RSVD7[4]                      ;  /*!< Reserved,  Address offset:0x410-0x41F */
	__I  uint32_t MST_TIMEOUT_STATUS            ;  /*!< MASER TIMEOUT STATUS REGISTER,  Address offset:0x420 */
	__IO uint32_t RSVD8[247]                    ;  /*!< Reserved,  Address offset:0x424-0x7FF */
	MPC_EntryTypeDef MPC1_Entry[4]              ;  /*!< RXI300 IP have 4 ports,  Address offset:0x800-0x81C */
	__IO uint32_t RSVD9[8]                      ;  /*!< Reserved,  Address offset:0x820-0x83F */
	__IO uint32_t IDAU_CTRL                     ;  /*!< ENABLE CONTROL REGISTER IN IDAU Register,  Address offset:0x840 */
	__IO uint32_t IDAU_LOCK                     ;  /*!< LOCK CONTROL REGISTER IN IDAU Register,  Address offset:0x844 */
	__IO uint32_t RSVD10[46]                    ;  /*!< Reserved,  Address offset:0x848-0x8FF */
	MPC_EntryTypeDef MPC2_Entry[4]              ;  /*!< RXI300 IP have 4 ports,  Address offset:0x900-0x91C */
	__IO uint32_t RSVD11[8]                     ;  /*!< Reserved,  Address offset:0x920-0x93F */
	__IO uint32_t IDAU2_CTRL                    ;  /*!< ENABLE CONTROL REGISTER IN IDAU Register,  Address offset:0x940 */
	__IO uint32_t IDAU2_LOCK                    ;  /*!< LOCK CONTROL REGISTER IN IDAU Register,  Address offset:0x944 */
	__IO uint32_t RSVD12[46]                    ;  /*!< Reserved,  Address offset:0x948-0x9FF */
	MPC_EntryTypeDef MPC3_Entry[4]              ;  /*!< RXI300 IP have 4 ports,  Address offset:0xA00-0xA1C */
	__IO uint32_t RSVD13[8]                     ;  /*!< Reserved,  Address offset:0xA20-0xA3F */
	__IO uint32_t IDAU3_CTRL                    ;  /*!< ENABLE CONTROL REGISTER IN IDAU Register,  Address offset:0xA40 */
	__IO uint32_t IDAU3_LOCK                    ;  /*!< LOCK CONTROL REGISTER IN IDAU Register,  Address offset:0xA44 */
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