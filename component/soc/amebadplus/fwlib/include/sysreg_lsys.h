/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SYSREG_LSYS_H_
#define _SYSREG_LSYS_H_

/* AUTO_GEN_START */

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup LSYS
* @brief LSYS driver modules
* @{
*/

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup LSYS_Register_Definitions LSYS Register Definitions
 * @{
 *****************************************************************************/

/** @defgroup REG_LSYS_PWC
 * @brief
 * @{
 **/
#define LSYS_MASK_PSW_OUT_BT                       ((u32)0x00000003 << 10)          /*!<R 0  BT domain power switch status 1: power on 0: power off Note:there are two group */
#define LSYS_PSW_OUT_BT(x)                         (((u32)((x) & 0x00000003) << 10))
#define LSYS_GET_PSW_OUT_BT(x)                     ((u32)(((x >> 10) & 0x00000003)))
#define LSYS_MASK_PSW_OUT_SOC                      ((u32)0x00000003 << 8)          /*!<R 0  SOC domain power switch status 1: power on 0: power off Note:there are two group */
#define LSYS_PSW_OUT_SOC(x)                        (((u32)((x) & 0x00000003) << 8))
#define LSYS_GET_PSW_OUT_SOC(x)                    ((u32)(((x >> 8) & 0x00000003)))
#define LSYS_BIT_PWC_SOC_REQ                       ((u32)0x00000001 << 7)          /*!<R 0  pwc soc req arb result */
#define LSYS_PWC_SOC_REQ(x)                        ((u32)(((x) & 0x00000001) << 7))
#define LSYS_GET_PWC_SOC_REQ(x)                    ((u32)(((x >> 7) & 0x00000001)))
#define LSYS_MASK_PWC_SOC_ST                       ((u32)0x00000007 << 4)          /*!<R 0  pwc soc arb result state */
#define LSYS_PWC_SOC_ST(x)                         (((u32)((x) & 0x00000007) << 4))
#define LSYS_GET_PWC_SOC_ST(x)                     ((u32)(((x >> 4) & 0x00000007)))
#define LSYS_BIT_PWC_BT                            ((u32)0x00000001 << 1)          /*!<R/W 0  This is BT function total power cut control 1: Enable BT power cut 0: Disable */
#define LSYS_PWC_BT(x)                             ((u32)(((x) & 0x00000001) << 1))
#define LSYS_GET_PWC_BT(x)                         ((u32)(((x >> 1) & 0x00000001)))
#define LSYS_BIT_POW_ON_SOC                        ((u32)0x00000001 << 0)          /*!<R/W 0  1:SYS PMC request to enable HP SOC power cut and diable ISO 0: SYS PMC request to disable HP SOC power cut and enable ISO */
#define LSYS_POW_ON_SOC(x)                         ((u32)(((x) & 0x00000001) << 0))
#define LSYS_GET_POW_ON_SOC(x)                     ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_ISO
 * @brief
 * @{
 **/
#define LSYS_BIT_ISO_BT                            ((u32)0x00000001 << 1)          /*!<R/W 1  1: isolate signals from PD_LP_BT to other power domain 0: release iso */
#define LSYS_ISO_BT(x)                             ((u32)(((x) & 0x00000001) << 1))
#define LSYS_GET_ISO_BT(x)                         ((u32)(((x >> 1) & 0x00000001)))
#define LSYS_BIT_ISO_SOC_REQ                       ((u32)0x00000001 << 0)          /*!<R 0  isolate signals from PD_HP_SOC to other power domain. It is arb result */
#define LSYS_ISO_SOC_REQ(x)                        ((u32)(((x) & 0x00000001) << 0))
#define LSYS_GET_ISO_SOC_REQ(x)                    ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_FEN_GRP0
 * @brief
 * @{
 **/
#define APBPeriph_HTIM0                            ((u32)(0  << 30) | (0x00000001 << 25))  /*!<R/W 0  1: enable TIMER 14 function 0: disable */
#define APBPeriph_HTIM1                            ((u32)(0  << 30) | (0x00000001 << 26))  /*!<R/W 0  1: enable TIMER 14 function 0: disable */
#define APBPeriph_PWM0                             ((u32)(0  << 30) | (0x00000001 << 23))  /*!<R/W 0  1: enable PWM TIMER function 0: disable [1]:pwm [0]:pulse */
#define APBPeriph_PWM1                             ((u32)(0  << 30) | (0x00000001 << 24))  /*!<R/W 0  1: enable PWM TIMER function 0: disable [1]:pwm [0]:pulse */
#define APBPeriph_SDIO                             ((u32)(0  << 30) | (0x00000001 << 22))  /*!<R/W 0  1: enable sdio device function 0: disable */
#define APBPeriph_SPORT0                           ((u32)(0  << 30) | (0x00000001 << 20))  /*!<R/W 0  1: enable SPORT function 0: disable */
#define APBPeriph_SPORT1                           ((u32)(0  << 30) | (0x00000001 << 21))  /*!<R/W 0  1: enable SPORT function 0: disable */
#define APBPeriph_AC                               ((u32)(0  << 30) | (0x00000001 << 19))  /*!<R/W 0  1: enable audio codec digital ip active function 0: disable */
#define APBPeriph_TRNG                             ((u32)(0  << 30) | (0x00000001 << 18))  /*!<R/W 0  1: enable TRNG function 0: disable */
#define APBPeriph_LEDC                             ((u32)(0  << 30) | (0x00000001 << 16))  /*!<R/W 0  1: enable LEDC function 0: disable */
#define APBPeriph_SPI0                             ((u32)(0  << 30) | (0x00000001 << 14))  /*!<R/W 0  1: enable SPI1 function 0: disable */
#define APBPeriph_SPI1                             ((u32)(0  << 30) | (0x00000001 << 15))  /*!<R/W 0  1: enable SPI1 function 0: disable */
#define APBPeriph_PSRAM                            ((u32)(0  << 30) | (0x00000001 << 13))  /*!<R/W 0  1: enable PSRAM function 0: disable */
#define APBPeriph_KSCAN                            ((u32)(0  << 30) | (0x00000001 << 11))  /*!<R/W 0  Flash kscan function enable 1: enable sce 0: disable */
#define APBPeriph_FLASH                            ((u32)(0  << 30) | (0x00000001 << 10))  /*!<R/W 0  Flash spic function enable 1: enable flash 0: disable flash */
#define APBPeriph_SHA                              ((u32)(0  << 30) | (0x00000001 << 9))   /*!<R/W 0  1: enable IPSEC SHA function 0: disable */
#define APBPeriph_AES                              ((u32)(0  << 30) | (0x00000001 << 8))   /*!<R/W 0  1: enable IPSEC AES function 0: disable */
#define APBPeriph_LX                               ((u32)(0  << 30) | (0x00000001 << 7))   /*!<R/W 0  1: enable LX1 system function 0: disable */
#define APBPeriph_DMAC                             ((u32)(0  << 30) | (0x00000001 << 6))   /*!<R/W 0  1: enable GDMA function 0: disable */
#define APBPeriph_IPC                              ((u32)(0  << 30) | (0x00000001 << 5))   /*!<R/W 0  1: enable IPC function 0: reset */
#define APBPeriph_KM4                              ((u32)(0  << 30) | (0x00000001 << 3))   /*!<R/W 0  1: enable KM4 0: reset */
#define APBPeriph_KM0                              ((u32)(0  << 30) | (0x00000001 << 2))   /*!<R/W 0  1: enable KM0 0: reset */
#define APBPeriph_PLFM                             ((u32)(0  << 30) | (0x00000001 << 1))   /*!<R/W 0  1: enable platform 0: reset */
#define APBPeriph_SOC                              ((u32)(0  << 30) | (0x00000001 << 0))   /*!<R/W 0  1: enable hs soc sytem 0: disable hs soc system (Note: used as soc power on reset) */
/** @} */

/** @defgroup REG_LSYS_FEN_GRP1
 * @brief
 * @{
 **/
#define APBPeriph_QSPI                             ((u32)(1  << 30) | (0x00000001 << 29))  /*!<R/W 0  1: enable qspi function 0: disable */
#define APBPeriph_IRDA                             ((u32)(1  << 30) | (0x00000001 << 28))  /*!<R/W 0  1: enable irda function 0: disable */
#define APBPeriph_PTIM0                            ((u32)(1  << 30) | (0x00000001 << 25))  /*!<R/W 0  pmc_timer function enable 1: enable 0: disable */
#define APBPeriph_PTIM1                            ((u32)(1  << 30) | (0x00000001 << 26))  /*!<R/W 0  pmc_timer function enable 1: enable 0: disable */
#define APBPeriph_ADC                              ((u32)(1  << 30) | (0x00000001 << 23))  /*!<R/W 0  lp adc function enable 1: enable 0: disable */
#define APBPeriph_USB                              ((u32)(1  << 30) | (0x00000001 << 22))  /*!<R/W 0  usb core function enable 1: enable 0: disable */
#define APBPeriph_DTIM                             ((u32)(1  << 30) | (0x00000001 << 20))  /*!<R/W 1  Debug timer function enable 1: enable 0: disable */
#define APBPeriph_LTIM0                            ((u32)(1  << 30) | (0x00000001 << 12))  /*!<R/W 8'h0  Basic Timer0-7 function enable 1: enable 0: disable */
#define APBPeriph_LTIM1                            ((u32)(1  << 30) | (0x00000001 << 13))  /*!<R/W 8'h0  Basic Timer0-7 function enable 1: enable 0: disable */
#define APBPeriph_LTIM2                            ((u32)(1  << 30) | (0x00000001 << 14))  /*!<R/W 8'h0  Basic Timer0-7 function enable 1: enable 0: disable */
#define APBPeriph_LTIM3                            ((u32)(1  << 30) | (0x00000001 << 15))  /*!<R/W 8'h0  Basic Timer0-7 function enable 1: enable 0: disable */
#define APBPeriph_LTIM4                            ((u32)(1  << 30) | (0x00000001 << 16))  /*!<R/W 8'h0  Basic Timer0-7 function enable 1: enable 0: disable */
#define APBPeriph_LTIM5                            ((u32)(1  << 30) | (0x00000001 << 17))  /*!<R/W 8'h0  Basic Timer0-7 function enable 1: enable 0: disable */
#define APBPeriph_LTIM6                            ((u32)(1  << 30) | (0x00000001 << 18))  /*!<R/W 8'h0  Basic Timer0-7 function enable 1: enable 0: disable */
#define APBPeriph_LTIM7                            ((u32)(1  << 30) | (0x00000001 << 19))  /*!<R/W 8'h0  Basic Timer0-7 function enable 1: enable 0: disable */
#define APBPeriph_I2C0                             ((u32)(1  << 30) | (0x00000001 << 10))  /*!<R/W 0  I2C1 function enable 1: enable 0: disable */
#define APBPeriph_I2C1                             ((u32)(1  << 30) | (0x00000001 << 11))  /*!<R/W 0  I2C1 function enable 1: enable 0: disable */
#define APBPeriph_UART0                            ((u32)(1  << 30) | (0x00000001 << 6))   /*!<R/W 0  Uart function enable (Note uart2 if for BT) 1: enable 0: disable */
#define APBPeriph_UART1                            ((u32)(1  << 30) | (0x00000001 << 7))   /*!<R/W 0  Uart function enable (Note uart2 if for BT) 1: enable 0: disable */
#define APBPeriph_UART2                            ((u32)(1  << 30) | (0x00000001 << 8))   /*!<R/W 0  Uart function enable (Note uart2 if for BT) 1: enable 0: disable */
#define APBPeriph_LOGUART                          ((u32)(1  << 30) | (0x00000001 << 5))   /*!<R/W 0  Uartlog function enable 1: enable 0: disable */
#define APBPeriph_GPIO                             ((u32)(1  << 30) | (0x00000001 << 4))   /*!<R/W 0  SGPIO function enable 1: enable 0: disable */
#define APBPeriph_SIC                              ((u32)(1  << 30) | (0x00000001 << 3))   /*!<R/W 0  1: enable SIC function 0: reset */
#define APBPeriph_WLAFE                            ((u32)(1  << 30) | (0x00000001 << 2))   /*!<R/W 0  WL RFAFE control function enable 1: enable 0: disable */
#define APBPeriph_BTON                             ((u32)(1  << 30) | (0x00000001 << 1))   /*!<R/W 0  BTON function enable 1: enable 0: disable */
#define APBPeriph_WLON                             ((u32)(1  << 30) | (0x00000001 << 0))   /*!<R/W 0  WLON function enable 1: enable 0: disable */
/** @} */

/** @defgroup REG_LSYS_CKE_GRP0
 * @brief
 * @{
 **/
#define APBPeriph_HP_CLOCK                         ((u32)(0  << 30) | (0x00000001 << 28))  /*!<R/W 1  1: Enable HP clock 0: Disable */
#define APBPeriph_LP_CLOCK                         ((u32)(0  << 30) | (0x00000001 << 27))  /*!<R/W 1  1: Enable LP clock 0: Disable */
#define APBPeriph_HTIM0_CLOCK                      ((u32)(0  << 30) | (0x00000001 << 25))  /*!<R/W 0  1: Enable HS basic TIMER clock 0: Disable */
#define APBPeriph_HTIM1_CLOCK                      ((u32)(0  << 30) | (0x00000001 << 26))  /*!<R/W 0  1: Enable HS basic TIMER clock 0: Disable */
#define APBPeriph_PWM0_CLOCK                       ((u32)(0  << 30) | (0x00000001 << 23))  /*!<R/W 0  1: Enable PWM TIMER clock 0: Disable [1] : pwm timer [0] : pulse timer */
#define APBPeriph_PWM1_CLOCK                       ((u32)(0  << 30) | (0x00000001 << 24))  /*!<R/W 0  1: Enable PWM TIMER clock 0: Disable [1] : pwm timer [0] : pulse timer */
#define APBPeriph_SDIO_CLOCK                       ((u32)(0  << 30) | (0x00000001 << 22))  /*!<R/W 0  1: Enable sdio clock 0: Disable */
#define APBPeriph_SPORT0_CLOCK                     ((u32)(0  << 30) | (0x00000001 << 20))  /*!<R/W 0  1: Enable sport1 clock 0: Disable */
#define APBPeriph_SPORT1_CLOCK                     ((u32)(0  << 30) | (0x00000001 << 21))  /*!<R/W 0  1: Enable sport1 clock 0: Disable */
#define APBPeriph_AC_CLOCK                         ((u32)(0  << 30) | (0x00000001 << 19))  /*!<R/W 0  1: Enable audio codec clock 0: Disable */
#define APBPeriph_TRNG_CLOCK                       ((u32)(0  << 30) | (0x00000001 << 18))  /*!<R/W 0  1: Enable trng bus clock 0: Disable */
#define APBPeriph_LEDC_CLOCK                       ((u32)(0  << 30) | (0x00000001 << 16))  /*!<R/W 0  1: Enable ledc bus clock 0: Disable */
#define APBPeriph_SPI0_CLOCK                       ((u32)(0  << 30) | (0x00000001 << 14))  /*!<R/W 0  1: Enable SPI1 clock 0: Disable */
#define APBPeriph_SPI1_CLOCK                       ((u32)(0  << 30) | (0x00000001 << 15))  /*!<R/W 0  1: Enable SPI1 clock 0: Disable */
#define APBPeriph_PSRAM_CLOCK                      ((u32)(0  << 30) | (0x00000001 << 13))  /*!<R/W 0  1: Enable PSRAM bus clock 0: Disable */
#define APBPeriph_SRAM_CLOCK                       ((u32)(0  << 30) | (0x00000001 << 12))  /*!<R/W 1  1: Enable hp sram clock 0: Disable */
#define APBPeriph_KSCAN_CLOCK                      ((u32)(0  << 30) | (0x00000001 << 11))  /*!<R/W 0  1: Enable kscan clock */
#define APBPeriph_FLASH_CLOCK                      ((u32)(0  << 30) | (0x00000001 << 10))  /*!<R/W 0  1: Enable SPIC source clock */
#define APBPeriph_SHA_CLOCK                        ((u32)(0  << 30) | (0x00000001 << 9))   /*!<R/W 0  1: Enable ipsec SHA clock 0: Disable */
#define APBPeriph_AES_CLOCK                        ((u32)(0  << 30) | (0x00000001 << 8))   /*!<R/W 0  1: Enable ipsec AES clock 0: Disable */
#define APBPeriph_LX_CLOCK                         ((u32)(0  << 30) | (0x00000001 << 7))   /*!<R/W 0  1: Enable lx1 bus clock 0: Disable */
#define APBPeriph_DMAC_CLOCK                       ((u32)(0  << 30) | (0x00000001 << 6))   /*!<R/W 0  1: Enable GDMA0 clock 0: Disable */
#define APBPeriph_IPC_CLOCK                        ((u32)(0  << 30) | (0x00000001 << 5))   /*!<R/W 0  1: Enable IPC clock 0: Disable */
#define APBPeriph_KM4_CLOCK                        ((u32)(0  << 30) | (0x00000001 << 4))   /*!<R/W 0  1: Enable hp KM4 clock 0: Disable */
#define APBPeriph_KM0_CLOCK                        ((u32)(0  << 30) | (0x00000001 << 3))   /*!<R/W 0  1: Enable lp KM0 clock 0: Disable */
#define APBPeriph_HPERI_CLOCK                      ((u32)(0  << 30) | (0x00000001 << 2))   /*!<R/W 1  1: Enable peri_hclk clock all 0: Disable */
#define APBPeriph_LPERI_CLOCK                      ((u32)(0  << 30) | (0x00000001 << 1))   /*!<R/W 1  1: Enable peri_lclk clock all 0: Disable */
#define APBPeriph_LPON_CLOCK                       ((u32)(0  << 30) | (0x00000001 << 0))   /*!<R/W 0  1.Enable lpon clk 0: Disable Note: this bit is controlled HW PMC, SW shall not modify */
/** @} */

/** @defgroup REG_LSYS_CKE_GRP1
 * @brief
 * @{
 **/
#define APBPeriph_QSPI_CLOCK                       ((u32)(1  << 30) | (0x00000001 << 29))  /*!<R/W 0  1: Enable qsip clock 0: Disable */
#define APBPeriph_IRDA_CLOCK                       ((u32)(1  << 30) | (0x00000001 << 28))  /*!<R/W 0  1: Enable irda clock 0: Disable */
#define APBPeriph_WMAC_CLOCK                       ((u32)(1  << 30) | (0x00000001 << 27))  /*!<R/W 0  1: Enable WMAC AXI and AHB bus clock 0: Disable */
#define APBPeriph_PTIM0_CLOCK                      ((u32)(1  << 30) | (0x00000001 << 25))  /*!<R/W 0  1: Enable pmc timer clock 0: Disable */
#define APBPeriph_PTIM1_CLOCK                      ((u32)(1  << 30) | (0x00000001 << 26))  /*!<R/W 0  1: Enable pmc timer clock 0: Disable */
#define APBPeriph_CTC_CLOCK                        ((u32)(1  << 30) | (0x00000001 << 24))  /*!<R/W 0  1: Enable Captouch clock 0: Disable */
#define APBPeriph_ADC_CLOCK                        ((u32)(1  << 30) | (0x00000001 << 23))  /*!<R/W 0  1: Enable LS ADC clock 0: Disable */
#define APBPeriph_USB_CLOCK                        ((u32)(1  << 30) | (0x00000001 << 22))  /*!<R/W 0  1: Enable usb core clock 0: Disable */
#define APBPeriph_DTIM_CLOCK                       ((u32)(1  << 30) | (0x00000001 << 20))  /*!<R/W 1  1: Enable LS dtim clk 0: Disable */
#define APBPeriph_LTIM0_CLOCK                      ((u32)(1  << 30) | (0x00000001 << 12))  /*!<R/W 8'h00  LS basic Timer0-7 clk enable 0: Disable */
#define APBPeriph_LTIM1_CLOCK                      ((u32)(1  << 30) | (0x00000001 << 13))  /*!<R/W 8'h00  LS basic Timer0-7 clk enable 0: Disable */
#define APBPeriph_LTIM2_CLOCK                      ((u32)(1  << 30) | (0x00000001 << 14))  /*!<R/W 8'h00  LS basic Timer0-7 clk enable 0: Disable */
#define APBPeriph_LTIM3_CLOCK                      ((u32)(1  << 30) | (0x00000001 << 15))  /*!<R/W 8'h00  LS basic Timer0-7 clk enable 0: Disable */
#define APBPeriph_LTIM4_CLOCK                      ((u32)(1  << 30) | (0x00000001 << 16))  /*!<R/W 8'h00  LS basic Timer0-7 clk enable 0: Disable */
#define APBPeriph_LTIM5_CLOCK                      ((u32)(1  << 30) | (0x00000001 << 17))  /*!<R/W 8'h00  LS basic Timer0-7 clk enable 0: Disable */
#define APBPeriph_LTIM6_CLOCK                      ((u32)(1  << 30) | (0x00000001 << 18))  /*!<R/W 8'h00  LS basic Timer0-7 clk enable 0: Disable */
#define APBPeriph_LTIM7_CLOCK                      ((u32)(1  << 30) | (0x00000001 << 19))  /*!<R/W 8'h00  LS basic Timer0-7 clk enable 0: Disable */
#define APBPeriph_I2C0_CLOCK                       ((u32)(1  << 30) | (0x00000001 << 10))  /*!<R/W 0  1: Enable HS I2C0 clock 0: Disable [1]: i2c1 [0]: i2c0 */
#define APBPeriph_I2C1_CLOCK                       ((u32)(1  << 30) | (0x00000001 << 11))  /*!<R/W 0  1: Enable HS I2C0 clock 0: Disable [1]: i2c1 [0]: i2c0 */
#define APBPeriph_AIPC_CLOCK                       ((u32)(1  << 30) | (0x00000001 << 9))   /*!<R/W 0  1: Enable aipc clock 0: Disable Note:HW has auto-gating function , sw needn't set it unless hw failed */
#define APBPeriph_UART0_CLOCK                      ((u32)(1  << 30) | (0x00000001 << 6))   /*!<R/W 0  1: Enable Uart clock 0: Disable */
#define APBPeriph_UART1_CLOCK                      ((u32)(1  << 30) | (0x00000001 << 7))   /*!<R/W 0  1: Enable Uart clock 0: Disable */
#define APBPeriph_UART2_CLOCK                      ((u32)(1  << 30) | (0x00000001 << 8))   /*!<R/W 0  1: Enable Uart clock 0: Disable */
#define APBPeriph_LOGUART_CLOCK                    ((u32)(1  << 30) | (0x00000001 << 5))   /*!<R/W 0  1: Enable Uartlog clock 0: Disable */
#define APBPeriph_GPIO_CLOCK                       ((u32)(1  << 30) | (0x00000001 << 4))   /*!<R/W 0  1: Enable GPIO clock 0: Disable */
#define APBPeriph_SIC_CLOCK                        ((u32)(1  << 30) | (0x00000001 << 3))   /*!<R/W 0  1: Enable SIC clock 0: Disable */
#define APBPeriph_WDG_CLOCK                        ((u32)(1  << 30) | (0x00000001 << 2))   /*!<R/W 1  1: Enable wdg bus clock 0: Disable */
#define APBPeriph_BTON_CLOCK                       ((u32)(1  << 30) | (0x00000001 << 1))   /*!<R/W 0  1: Enable BTON clock 0: Disable */
#define APBPeriph_WLON_CLOCK                       ((u32)(1  << 30) | (0x00000001 << 0))   /*!<R/W 0  1: Enable WLON clock 0: Disable */
/** @} */

/** @defgroup REG_LSYS_CKE_GRP2
 * @brief
 * @{
 **/
#define LSYS_BIT_CKE_RET                           ((u32)0x00000001 << 1)          /*!<R/W 1'b0  1.Enable Some IP's clock who has retention function when pmc state change 0: Disable Note: this bit is controlled HW PMC, SW shall not modify */
#define LSYS_CKE_RET(x)                            ((u32)(((x) & 0x00000001) << 1))
#define LSYS_GET_CKE_RET(x)                        ((u32)(((x >> 1) & 0x00000001)))
#define LSYS_BIT_CKE_OSC4M_SOC                     ((u32)0x00000001 << 0)          /*!<R/W 1'b0  1.Enable 4M clk path for soc clk 0: Disable Note: this bit is controlled HW PMC, SW shall not modify */
#define LSYS_CKE_OSC4M_SOC(x)                      ((u32)(((x) & 0x00000001) << 0))
#define LSYS_GET_CKE_OSC4M_SOC(x)                  ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_CKSL_GRP0
 * @brief
 * @{
 **/
#define LSYS_BIT_CKSL_LOGUART                      ((u32)0x00000001 << 29)          /*!<R/W 0  Select div_loguart's input clock (div_loguart's output clock is loguart trx clock ) 0: clock is xtal(source is dicided by cksl_loguart_xtal) 1: clock is osc4M */
#define LSYS_CKSL_LOGUART(x)                       ((u32)(((x) & 0x00000001) << 29))
#define LSYS_GET_CKSL_LOGUART(x)                   ((u32)(((x >> 29) & 0x00000001)))
#define LSYS_MASK_CKSL_UART                        ((u32)0x00000007 << 25)          /*!<R/W 0  Select div_uart[n]'s input clock (div_uart[n]'s output clock is uart[n] rx clock). (n=0,1,2) 0: clock is xtal(source is dicided by cksl_uart_xtal) 1: clock is osc4M */
#define LSYS_CKSL_UART(x)                          (((u32)((x) & 0x00000007) << 25))
#define LSYS_GET_CKSL_UART(x)                      ((u32)(((x >> 25) & 0x00000007)))
#define LSYS_BIT_CKSL_LOGUART_XTAL                 ((u32)0x00000001 << 24)          /*!<R/W 0  loguart_xtal clk select 0:clk_xtal_digi 1:clk_xtal_digi_div2 */
#define LSYS_CKSL_LOGUART_XTAL(x)                  ((u32)(((x) & 0x00000001) << 24))
#define LSYS_GET_CKSL_LOGUART_XTAL(x)              ((u32)(((x >> 24) & 0x00000001)))
#define LSYS_MASK_CKSL_UART_XTAL                   ((u32)0x00000007 << 20)          /*!<R/W 0  uart xtal clk select, cksl_uart_xtal[n](n=0,1,2) 0: clk_xtal_digi 1:clk_xtal_digi_div2 */
#define LSYS_CKSL_UART_XTAL(x)                     (((u32)((x) & 0x00000007) << 20))
#define LSYS_GET_CKSL_UART_XTAL(x)                 ((u32)(((x >> 20) & 0x00000007)))
#define LSYS_MASK_CKSL_SPORT                       ((u32)0x00000003 << 18)          /*!<R/W 2'b11  sport clock selection 0: equal to audio codec main clock (40M) 1: from cpu/dsp pll , after pll selection(cksl_i2s_pll) */
#define LSYS_CKSL_SPORT(x)                         (((u32)((x) & 0x00000003) << 18))
#define LSYS_GET_CKSL_SPORT(x)                     ((u32)(((x >> 18) & 0x00000003)))
#define LSYS_BIT_CKSL_OTPC                         ((u32)0x00000001 << 14)          /*!<R/W 0h  otpc clock selection 0: osc4m 1: xtal40m Note: this bit will be set by pmc auto, when SW use otp, it must be set to 1 already */
#define LSYS_CKSL_OTPC(x)                          ((u32)(((x) & 0x00000001) << 14))
#define LSYS_GET_CKSL_OTPC(x)                      ((u32)(((x >> 14) & 0x00000001)))
#define LSYS_BIT_CKSL_IRDA                         ((u32)0x00000001 << 13)          /*!<R/W 0h  IRDA clock selection 0: xtal 40m 1: pll divider out */
#define LSYS_CKSL_IRDA(x)                          ((u32)(((x) & 0x00000001) << 13))
#define LSYS_GET_CKSL_IRDA(x)                      ((u32)(((x >> 13) & 0x00000001)))
#define LSYS_BIT_CKSL_ADC4M                        ((u32)0x00000001 << 12)          /*!<R/W 0  lp adc 4m clk selection 0: from osc4m 1: divider from xtal */
#define LSYS_CKSL_ADC4M(x)                         ((u32)(((x) & 0x00000001) << 12))
#define LSYS_GET_CKSL_ADC4M(x)                     ((u32)(((x >> 12) & 0x00000001)))
#define LSYS_BIT_CKSL_DTIM                         ((u32)0x00000001 << 11)          /*!<R/W 0  dtim clock selection 0: xtal 1: 32k for sleep */
#define LSYS_CKSL_DTIM(x)                          ((u32)(((x) & 0x00000001) << 11))
#define LSYS_GET_CKSL_DTIM(x)                      ((u32)(((x >> 11) & 0x00000001)))
#define LSYS_BIT_CKSL_CTC                          ((u32)0x00000001 << 10)          /*!<R/W 0h  CTC REG clock selection 0: lbus clock 1: ctc ip clock */
#define LSYS_CKSL_CTC(x)                           ((u32)(((x) & 0x00000001) << 10))
#define LSYS_GET_CKSL_CTC(x)                       ((u32)(((x >> 10) & 0x00000001)))
#define LSYS_BIT_CKSL_ADC                          ((u32)0x00000001 << 9)          /*!<R/W 0  ADC INT clock selection 0: lbus clock 1: adc ip clock */
#define LSYS_CKSL_ADC(x)                           ((u32)(((x) & 0x00000001) << 9))
#define LSYS_GET_CKSL_ADC(x)                       ((u32)(((x >> 9) & 0x00000001)))
#define LSYS_BIT_CKSL_GPIO                         ((u32)0x00000001 << 8)          /*!<R/W 0  GPIO INT clock selecton: 0: lbus clock 1: 32.768K clock from SDM */
#define LSYS_CKSL_GPIO(x)                          ((u32)(((x) & 0x00000001) << 8))
#define LSYS_GET_CKSL_GPIO(x)                      ((u32)(((x >> 8) & 0x00000001)))
#define LSYS_BIT_CKSL_PTIM                         ((u32)0x00000001 << 7)          /*!<R/W 0  PMC Timer INT clock selecton: 0: lbus clock 1: 32.768K clock from SDM */
#define LSYS_CKSL_PTIM(x)                          ((u32)(((x) & 0x00000001) << 7))
#define LSYS_GET_CKSL_PTIM(x)                      ((u32)(((x >> 7) & 0x00000001)))
#define LSYS_BIT_CKSL_PSRAM                        ((u32)0x00000001 << 6)          /*!<R/W 1  psram clock select 0: xtal 40m 1: pll divider out */
#define LSYS_CKSL_PSRAM(x)                         ((u32)(((x) & 0x00000001) << 6))
#define LSYS_GET_CKSL_PSRAM(x)                     ((u32)(((x >> 6) & 0x00000001)))
#define LSYS_BIT_CKSL_SPIC                         ((u32)0x00000001 << 4)          /*!<R/W 0  spic clock select 0: xtal 40m 1: pll divider out */
#define LSYS_CKSL_SPIC(x)                          ((u32)(((x) & 0x00000001) << 4))
#define LSYS_GET_CKSL_SPIC(x)                      ((u32)(((x >> 4) & 0x00000001)))
#define LSYS_BIT_CKSL_HPERI                        ((u32)0x00000001 << 3)          /*!<R/W 0  hperi clock seletion 0: xtal 40m 1: pll divider out */
#define LSYS_CKSL_HPERI(x)                         ((u32)(((x) & 0x00000001) << 3))
#define LSYS_GET_CKSL_HPERI(x)                     ((u32)(((x >> 3) & 0x00000001)))
#define LSYS_BIT_CKSL_HP                           ((u32)0x00000001 << 2)          /*!<R/W 0h  HP clock select,used for KM4 and sram 0: xtal 40m 1: pll divider out */
#define LSYS_CKSL_HP(x)                            ((u32)(((x) & 0x00000001) << 2))
#define LSYS_GET_CKSL_HP(x)                        ((u32)(((x >> 2) & 0x00000001)))
#define LSYS_MASK_CKSL_LP                          ((u32)0x00000003 << 0)          /*!<R/W 0h  LP clock select, used for KM0 0: osc4m 1: xtal_40m 2: NotValid 3: pll divider out */
#define LSYS_CKSL_LP(x)                            (((u32)((x) & 0x00000003) << 0))
#define LSYS_GET_CKSL_LP(x)                        ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup REG_LSYS_CKSL_GRP1
 * @brief
 * @{
 **/
#define LSYS_MASK_CKSL_XTAL                        ((u32)0x0000000F << 0)          /*!<R/W 0  xtal selection, Default=40MHz No use,only support 40M */
#define LSYS_CKSL_XTAL(x)                          (((u32)((x) & 0x0000000F) << 0))
#define LSYS_GET_CKSL_XTAL(x)                      ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_LSYS_CKD_GRP0
 * @brief
 * @{
 **/
#define LSYS_MASK_CKD_QSPI                         ((u32)0x00000007 << 28)          /*!<R/WPD 1h  qspi clock divider, it is based on pll 0: div1 1: div2 2: div3 3/other: divider by this value + 1 */
#define LSYS_CKD_QSPI(x)                           (((u32)((x) & 0x00000007) << 28))
#define LSYS_GET_CKD_QSPI(x)                       ((u32)(((x >> 28) & 0x00000007)))
#define LSYS_MASK_CKD_UTMIFS                       ((u32)0x0000000F << 20)          /*!<R/WPD 0x7  usb full-speed 48M clock divider, it is based on cpu pll output 0: not valid 1: div2 2: div3 3/other: divider by this value + 1 */
#define LSYS_CKD_UTMIFS(x)                         (((u32)((x) & 0x0000000F) << 20))
#define LSYS_GET_CKD_UTMIFS(x)                     ((u32)(((x >> 20) & 0x0000000F)))
#define LSYS_MASK_CKD_HP_MBIST                     ((u32)0x00000003 << 18)          /*!<R/WPD 0h  hp(km4) clock divider, it is based on pll 0: div1 1: div2 2: div3 3/other: divider by this value + 1 Note: this divider just for HW mbist test , SW don’t use it */
#define LSYS_CKD_HP_MBIST(x)                       (((u32)((x) & 0x00000003) << 18))
#define LSYS_GET_CKD_HP_MBIST(x)                   ((u32)(((x >> 18) & 0x00000003)))
#define LSYS_MASK_CKD_SRAM                         ((u32)0x00000003 << 16)          /*!<R/WPD 0h  sram clock divider, it is based on hp_clk 0: div1 1: div2 2: div3 3/other: divider by this value + 1 */
#define LSYS_CKD_SRAM(x)                           (((u32)((x) & 0x00000003) << 16))
#define LSYS_GET_CKD_SRAM(x)                       ((u32)(((x >> 16) & 0x00000003)))
#define LSYS_MASK_CKD_IRDA                         ((u32)0x00000007 << 12)          /*!<R/WPD 3h  irda clock divider, it is based on sys pll 0 : not valid 1: div2 2: div3 3/other: divider by this value + 1 */
#define LSYS_CKD_IRDA(x)                           (((u32)((x) & 0x00000007) << 12))
#define LSYS_GET_CKD_IRDA(x)                       ((u32)(((x >> 12) & 0x00000007)))
#define LSYS_MASK_CKD_HPERI                        ((u32)0x00000007 << 8)          /*!<R/WPD 3h  peri_hclk clock divider, it is based on sys pll 0 : not valid 1: div2 2: div3 3/other: divider by this value + 1 */
#define LSYS_CKD_HPERI(x)                          (((u32)((x) & 0x00000007) << 8))
#define LSYS_GET_CKD_HPERI(x)                      ((u32)(((x >> 8) & 0x00000007)))
#define LSYS_MASK_CKD_LP                           ((u32)0x0000000F << 4)          /*!<R/WPD 3h  KM0 pll clock divider 0: div1 1: div2 2: div3 3/other: divider by this value + 1 */
#define LSYS_CKD_LP(x)                             (((u32)((x) & 0x0000000F) << 4))
#define LSYS_GET_CKD_LP(x)                         ((u32)(((x >> 4) & 0x0000000F)))
#define LSYS_MASK_CKD_HP                           ((u32)0x00000007 << 0)          /*!<R/WPD 1h  hp(km4) clock divider, it is based on pll 0: div1 1: div2 2: div3 3/other: divider by this value + 1 */
#define LSYS_CKD_HP(x)                             (((u32)((x) & 0x00000007) << 0))
#define LSYS_GET_CKD_HP(x)                         ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_LSYS_CKD_GRP1
 * @brief
 * @{
 **/
#define LSYS_MASK_CKD_UART2                        ((u32)0x0000000F << 28)          /*!<R/WPD 0h  uart2 rx divider, source select by cksl_uart N:divider by N+1 1:divider by 2 0:divider by 1 Note: If uart need lps wake up when using xtal source , divider must set to 9 , and using xtal_20M input,when using OSC4M source , divider must set 1. (it is constrained by STA) */
#define LSYS_CKD_UART2(x)                          (((u32)((x) & 0x0000000F) << 28))
#define LSYS_GET_CKD_UART2(x)                      ((u32)(((x >> 28) & 0x0000000F)))
#define LSYS_MASK_CKD_UART1                        ((u32)0x0000000F << 24)          /*!<R/WPD 0h  uart1 rx divider, source select by cksl_uart N:divider by N+1 1:divider by 2 0:divider by 1 Note: If uart need lps wake up when using xtal source , divider must set to 9 , and using xtal_20M input,when using OSC4M source , divider must set 1. (it is constrained by STA) */
#define LSYS_CKD_UART1(x)                          (((u32)((x) & 0x0000000F) << 24))
#define LSYS_GET_CKD_UART1(x)                      ((u32)(((x >> 24) & 0x0000000F)))
#define LSYS_MASK_CKD_UART0                        ((u32)0x0000000F << 20)          /*!<R/WPD 0h  uart0 rx divider, source select by cksl_uart N:divider by N+1 1:divider by 2 0:divider by 1 Note: If uart need lps wake up when using xtal source , divider must set to 9 , and using xtal_20M input,when using OSC4M source , divider must set 1. (it is constrained by STA) */
#define LSYS_CKD_UART0(x)                          (((u32)((x) & 0x0000000F) << 20))
#define LSYS_GET_CKD_UART0(x)                      ((u32)(((x >> 20) & 0x0000000F)))
#define LSYS_MASK_CKD_LOGUART                      ((u32)0x0000000F << 16)          /*!<R/WPD 0h  loguart rx divider, source select by cksl_uart N:divider by N+1 1:divider by 2 0:divider by 1 Note: If uart need lps wake up when using xtal source , divider must set to 9 , and using xtal_20M input,when using OSC4M source , divider must set 1. (it is constrained by STA) */
#define LSYS_CKD_LOGUART(x)                        (((u32)((x) & 0x0000000F) << 16))
#define LSYS_GET_CKD_LOGUART(x)                    ((u32)(((x >> 16) & 0x0000000F)))
#define LSYS_MASK_CKD_XTAL_ADC                     ((u32)0x0000000F << 12)          /*!<R/WPD 9h  xtal 4m divider for adc Default is /10 which output 4M */
#define LSYS_CKD_XTAL_ADC(x)                       (((u32)((x) & 0x0000000F) << 12))
#define LSYS_GET_CKD_XTAL_ADC(x)                   ((u32)(((x >> 12) & 0x0000000F)))
#define LSYS_MASK_CKD_XTAL_DTIM                    ((u32)0x0000003F << 6)          /*!<R/WPD 6'd39  xtal 1m divider for dtim Default is /40 which output 4M */
#define LSYS_CKD_XTAL_DTIM(x)                      (((u32)((x) & 0x0000003F) << 6))
#define LSYS_GET_CKD_XTAL_DTIM(x)                  ((u32)(((x >> 6) & 0x0000003F)))
#define LSYS_MASK_CKD_XTAL_HTIM                    ((u32)0x0000003F << 0)          /*!<R/WPD 6'd39  xtal 1m divider for htim Default is /40 which output 4M */
#define LSYS_CKD_XTAL_HTIM(x)                      (((u32)((x) & 0x0000003F) << 0))
#define LSYS_GET_CKD_XTAL_HTIM(x)                  ((u32)(((x >> 0) & 0x0000003F)))
/** @} */

/** @defgroup REG_LSYS_SYSTEM_CFG0
 * @brief
 * @{
 **/
#define LSYS_BIT_ENG_MODE                          ((u32)0x00000001 << 31)          /*!<R 0  Indicate PKG_ENG_SEL pad value 0: normal package; 1: engineering mode */
#define LSYS_ENG_MODE(x)                           ((u32)(((x) & 0x00000001) << 31))
#define LSYS_GET_ENG_MODE(x)                       ((u32)(((x >> 31) & 0x00000001)))
#define LSYS_BIT_PLL_VALID                         ((u32)0x00000001 << 30)          /*!<R 0  Indicator pll is ready */
#define LSYS_PLL_VALID(x)                          ((u32)(((x) & 0x00000001) << 30))
#define LSYS_GET_PLL_VALID(x)                      ((u32)(((x >> 30) & 0x00000001)))
#define LSYS_BIT_XTAL_VALID                        ((u32)0x00000001 << 29)          /*!<R 0  Indicator xtal 40M is ready */
#define LSYS_XTAL_VALID(x)                         ((u32)(((x) & 0x00000001) << 29))
#define LSYS_GET_XTAL_VALID(x)                     ((u32)(((x >> 29) & 0x00000001)))
#define LSYS_MASK_PTRP_ICFG                        ((u32)0x0000000F << 20)          /*!<R 0  Trapped ICFG data value 0: normall mode XXXX: the other mode (Test Mode) */
#define LSYS_PTRP_ICFG(x)                          (((u32)((x) & 0x0000000F) << 20))
#define LSYS_GET_PTRP_ICFG(x)                      ((u32)(((x >> 20) & 0x0000000F)))
#define LSYS_BIT_PTRP_UARTDL                       ((u32)0x00000001 << 19)          /*!<R 0  Power on trapped uart download value 0: Normal boot 1: uartdownload boot TODO: it may be overwrited by efuse autoload */
#define LSYS_PTRP_UARTDL(x)                        ((u32)(((x) & 0x00000001) << 19))
#define LSYS_GET_PTRP_UARTDL(x)                    ((u32)(((x >> 19) & 0x00000001)))
#define LSYS_BIT_PTRP_IBOOT                        ((u32)0x00000001 << 18)          /*!<R 0  Internal or external flash boot select */
#define LSYS_PTRP_IBOOT(x)                         ((u32)(((x) & 0x00000001) << 18))
#define LSYS_GET_PTRP_IBOOT(x)                     ((u32)(((x >> 18) & 0x00000001)))
#define LSYS_BIT_PTRP_TMODE                        ((u32)0x00000001 << 17)          /*!<R 0  0: normal mode 1: test mode */
#define LSYS_PTRP_TMODE(x)                         ((u32)(((x) & 0x00000001) << 17))
#define LSYS_GET_PTRP_TMODE(x)                     ((u32)(((x >> 17) & 0x00000001)))
#define LSYS_BIT_PTRP_OTPBYP                       ((u32)0x00000001 << 16)          /*!<R 0  1: force efuse autoload fail */
#define LSYS_PTRP_OTPBYP(x)                        ((u32)(((x) & 0x00000001) << 16))
#define LSYS_GET_PTRP_OTPBYP(x)                    ((u32)(((x >> 16) & 0x00000001)))
#define LSYS_MASK_HW_CHIP_TYPE                     ((u32)0x00000003 << 8)          /*!<R 0  Chip type 00: ASIC 01: FPGA 10: PALADIUM 11: RTL_SIM */
#define LSYS_HW_CHIP_TYPE(x)                       (((u32)((x) & 0x00000003) << 8))
#define LSYS_GET_HW_CHIP_TYPE(x)                   ((u32)(((x >> 8) & 0x00000003)))
#define LSYS_MASK_HW_CPU_ID                        ((u32)0x00000003 << 6)          /*!<R 0  Different CPU reading this register will return different value. If read by KM0: return 0 If read by KM4: return 1 Other will return 2 */
#define LSYS_HW_CPU_ID(x)                          (((u32)((x) & 0x00000003) << 6))
#define LSYS_GET_HW_CPU_ID(x)                      ((u32)(((x >> 6) & 0x00000003)))
/** @} */

/** @defgroup REG_LSYS_PLAT_CTRL
 * @brief
 * @{
 **/
#define LSYS_MASK_PPE_CLK_OPT                      ((u32)0x00000003 << 28)          /*!<R/W 0  1X: always ON 01: always OFF 00: controlled by internal ICG */
#define LSYS_PPE_CLK_OPT(x)                        (((u32)((x) & 0x00000003) << 28))
#define LSYS_GET_PPE_CLK_OPT(x)                    ((u32)(((x >> 28) & 0x00000003)))
#define LSYS_BIT_SPI1_MST                          ((u32)0x00000001 << 27)          /*!<R/W 0h  1: SPI1 used as master 0: SPI1 used as slaver */
#define LSYS_SPI1_MST(x)                           ((u32)(((x) & 0x00000001) << 27))
#define LSYS_GET_SPI1_MST(x)                       ((u32)(((x >> 27) & 0x00000001)))
#define LSYS_BIT_SPI0_MST                          ((u32)0x00000001 << 26)          /*!<R/W 0h  1: SPI0 used as master 0: SPI0 used as slaver */
#define LSYS_SPI0_MST(x)                           ((u32)(((x) & 0x00000001) << 26))
#define LSYS_GET_SPI0_MST(x)                       ((u32)(((x >> 26) & 0x00000001)))
#define LSYS_BIT_SPIC_SPDUP_SIM                    ((u32)0x00000001 << 25)          /*!<R/W 0  1: SPIC will wait 256 spic_clk cycles before ready 0: SPIC will wait 2^16 spic_clk cycles before ready */
#define LSYS_SPIC_SPDUP_SIM(x)                     ((u32)(((x) & 0x00000001) << 25))
#define LSYS_GET_SPIC_SPDUP_SIM(x)                 ((u32)(((x >> 25) & 0x00000001)))
#define LSYS_BIT_PSRAM_SPDUP_SIM                   ((u32)0x00000001 << 24)          /*!<R/W 0  psramc simulatin speedup enable. Nouse because config form not specified . */
#define LSYS_PSRAM_SPDUP_SIM(x)                    ((u32)(((x) & 0x00000001) << 24))
#define LSYS_GET_PSRAM_SPDUP_SIM(x)                ((u32)(((x >> 24) & 0x00000001)))
#define LSYS_BIT_QSPI_SPDUP_SIM                    ((u32)0x00000001 << 23)          /*!<R/W 0  qspi simulation speedup enable */
#define LSYS_QSPI_SPDUP_SIM(x)                     ((u32)(((x) & 0x00000001) << 23))
#define LSYS_GET_QSPI_SPDUP_SIM(x)                 ((u32)(((x >> 23) & 0x00000001)))
#define LSYS_MASK_GDMA0_CCM                        ((u32)0x00000003 << 21)          /*!<R/W 0h  GDMA0 clock control mode */
#define LSYS_GDMA0_CCM(x)                          (((u32)((x) & 0x00000003) << 21))
#define LSYS_GET_GDMA0_CCM(x)                      ((u32)(((x >> 21) & 0x00000003)))
#define LSYS_BIT_PPE_GLB_SUS                       ((u32)0x00000001 << 20)          /*!<R/W 0  Assert this bit to suspend PPE, active high */
#define LSYS_PPE_GLB_SUS(x)                        ((u32)(((x) & 0x00000001) << 20))
#define LSYS_GET_PPE_GLB_SUS(x)                    ((u32)(((x >> 20) & 0x00000001)))
#define LSYS_BIT_SHARE_BT_MEM                      ((u32)0x00000001 << 19)          /*!<R/W 0  1: Enable KM4 to share BT memory */
#define LSYS_SHARE_BT_MEM(x)                       ((u32)(((x) & 0x00000001) << 19))
#define LSYS_GET_SHARE_BT_MEM(x)                   ((u32)(((x >> 19) & 0x00000001)))
#define LSYS_BIT_SHARE_WL_MEM                      ((u32)0x00000001 << 18)          /*!<R/W 0  1: Enable KM4 to share WL memory */
#define LSYS_SHARE_WL_MEM(x)                       ((u32)(((x) & 0x00000001) << 18))
#define LSYS_GET_SHARE_WL_MEM(x)                   ((u32)(((x >> 18) & 0x00000001)))
#define LSYS_BIT_KM0_RETENTION_MODE                ((u32)0x00000001 << 17)          /*!<R/W 0  1: Enable, when KM0 reset, cache will not be cleared 0: Disable, when KM0 reset, cache will be cleared by HW */
#define LSYS_KM0_RETENTION_MODE(x)                 ((u32)(((x) & 0x00000001) << 17))
#define LSYS_GET_KM0_RETENTION_MODE(x)             ((u32)(((x >> 17) & 0x00000001)))
#define LSYS_BIT_KM4_RETENTION_MODE                ((u32)0x00000001 << 16)          /*!<R/W 0  1: Enable, when KM4 reset, cache will not be cleared 0: Disable, when KM4 reset, cache will be cleared by HW */
#define LSYS_KM4_RETENTION_MODE(x)                 ((u32)(((x) & 0x00000001) << 16))
#define LSYS_GET_KM4_RETENTION_MODE(x)             ((u32)(((x >> 16) & 0x00000001)))
#define LSYS_BIT_SYS_RETENTION_SL                  ((u32)0x00000001 << 15)          /*!<R/W 1  S/L control signal to retention DFFs at PD_SOC, direct to SL pin, default 1 to prevent leakage 0: normal mode 1: retention data 1->0: restore data */
#define LSYS_SYS_RETENTION_SL(x)                   ((u32)(((x) & 0x00000001) << 15))
#define LSYS_GET_SYS_RETENTION_SL(x)               ((u32)(((x >> 15) & 0x00000001)))
#define LSYS_MASK_HS_SRAM2_RD_WEIGHT               ((u32)0x00000007 << 8)          /*!<R/W 2h  Excuted read command number continuously if both read and write command reached This is for bt/wl share_ram */
#define LSYS_HS_SRAM2_RD_WEIGHT(x)                 (((u32)((x) & 0x00000007) << 8))
#define LSYS_GET_HS_SRAM2_RD_WEIGHT(x)             ((u32)(((x >> 8) & 0x00000007)))
#define LSYS_MASK_HS_SRAM0_RD_WEIGHT               ((u32)0x00000007 << 2)          /*!<R/W 2h  Excuted read command number continuously if both read and write command reached This is for hs_ram 512K */
#define LSYS_HS_SRAM0_RD_WEIGHT(x)                 (((u32)((x) & 0x00000007) << 2))
#define LSYS_GET_HS_SRAM0_RD_WEIGHT(x)             ((u32)(((x >> 2) & 0x00000007)))
#define LSYS_BIT_HUK_PROT                          ((u32)0x00000001 << 0)          /*!<R/WH 0  0: only cpu can read HUK key 1: no one can read HUK key This bit will be latch to high when it was write to 1 until reset again . */
#define LSYS_HUK_PROT(x)                           ((u32)(((x) & 0x00000001) << 0))
#define LSYS_GET_HUK_PROT(x)                       ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_PLAT_STATUS
 * @brief
 * @{
 **/
#define LSYS_BIT_INIT_XO_RANGE_EN                  ((u32)0x00000001 << 16)          /*!<R/W 0   */
#define LSYS_INIT_XO_RANGE_EN(x)                   ((u32)(((x) & 0x00000001) << 16))
#define LSYS_GET_INIT_XO_RANGE_EN(x)               ((u32)(((x >> 16) & 0x00000001)))
#define LSYS_BIT_KM0_RST_HALT                      ((u32)0x00000001 << 15)          /*!<R/W 1   */
#define LSYS_KM0_RST_HALT(x)                       ((u32)(((x) & 0x00000001) << 15))
#define LSYS_GET_KM0_RST_HALT(x)                   ((u32)(((x >> 15) & 0x00000001)))
#define LSYS_BIT_KM4_RST_HALT                      ((u32)0x00000001 << 14)          /*!<R/W 0   */
#define LSYS_KM4_RST_HALT(x)                       ((u32)(((x) & 0x00000001) << 14))
#define LSYS_GET_KM4_RST_HALT(x)                   ((u32)(((x >> 14) & 0x00000001)))
#define LSYS_BIT_KM0_DBGRESTARTED_M                ((u32)0x00000001 << 7)          /*!<R 0  KM0 debug restart status */
#define LSYS_KM0_DBGRESTARTED_M(x)                 ((u32)(((x) & 0x00000001) << 7))
#define LSYS_GET_KM0_DBGRESTARTED_M(x)             ((u32)(((x >> 7) & 0x00000001)))
#define LSYS_BIT_KM0_DBG_MODE                      ((u32)0x00000001 << 6)          /*!<R 0  KM0 debug status */
#define LSYS_KM0_DBG_MODE(x)                       ((u32)(((x) & 0x00000001) << 6))
#define LSYS_GET_KM0_DBG_MODE(x)                   ((u32)(((x >> 6) & 0x00000001)))
#define LSYS_BIT_KM0_LOCKUP_M                      ((u32)0x00000001 << 5)          /*!<R 0  KM0 lockup status */
#define LSYS_KM0_LOCKUP_M(x)                       ((u32)(((x) & 0x00000001) << 5))
#define LSYS_GET_KM0_LOCKUP_M(x)                   ((u32)(((x >> 5) & 0x00000001)))
#define LSYS_BIT_KM0_SLP_MODE                      ((u32)0x00000001 << 4)          /*!<R 0  KM0 sleep status */
#define LSYS_KM0_SLP_MODE(x)                       ((u32)(((x) & 0x00000001) << 4))
#define LSYS_GET_KM0_SLP_MODE(x)                   ((u32)(((x >> 4) & 0x00000001)))
#define LSYS_BIT_KM4_DBGRESTARTED_M                ((u32)0x00000001 << 3)          /*!<R 0  KM4 debug restart status */
#define LSYS_KM4_DBGRESTARTED_M(x)                 ((u32)(((x) & 0x00000001) << 3))
#define LSYS_GET_KM4_DBGRESTARTED_M(x)             ((u32)(((x >> 3) & 0x00000001)))
#define LSYS_BIT_KM4_DBG_MODE                      ((u32)0x00000001 << 2)          /*!<R 0  KM4 halt status */
#define LSYS_KM4_DBG_MODE(x)                       ((u32)(((x) & 0x00000001) << 2))
#define LSYS_GET_KM4_DBG_MODE(x)                   ((u32)(((x >> 2) & 0x00000001)))
#define LSYS_BIT_KM4_LOCKUP_M                      ((u32)0x00000001 << 1)          /*!<R 0  KM4 lockup status */
#define LSYS_KM4_LOCKUP_M(x)                       ((u32)(((x) & 0x00000001) << 1))
#define LSYS_GET_KM4_LOCKUP_M(x)                   ((u32)(((x >> 1) & 0x00000001)))
#define LSYS_BIT_KM4_SLP_MODE                      ((u32)0x00000001 << 0)          /*!<R 0  KM4 sleep status */
#define LSYS_KM4_SLP_MODE(x)                       ((u32)(((x) & 0x00000001) << 0))
#define LSYS_GET_KM4_SLP_MODE(x)                   ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_USB_SDIO_CTRL
 * @brief
 * @{
 **/
#define LSYS_MASK_SS_SCALEDOWN_MODE                ((u32)0x00000003 << 24)          /*!<R/W 0h  use scaled-down timing values for usb, resulting in faster simulations */
#define LSYS_SS_SCALEDOWN_MODE(x)                  (((u32)((x) & 0x00000003) << 24))
#define LSYS_GET_SS_SCALEDOWN_MODE(x)              ((u32)(((x >> 24) & 0x00000003)))
#define LSYS_BIT_SDD_PMUX_FEN                      ((u32)0x00000001 << 16)          /*!<R/W 0  after sdio pinmux is configured, set this bit to 1 */
#define LSYS_SDD_PMUX_FEN(x)                       ((u32)(((x) & 0x00000001) << 16))
#define LSYS_GET_SDD_PMUX_FEN(x)                   ((u32)(((x >> 16) & 0x00000001)))
#define LSYS_BIT_Z0_TUNE_EN                        ((u32)0x00000001 << 3)          /*!<R/W 1  Zo_Tune , 45Ωauto calibration(Port-1 only) 0: Zo_Tune disable 1: Zo_Tune enable */
#define LSYS_Z0_TUNE_EN(x)                         ((u32)(((x) & 0x00000001) << 3))
#define LSYS_GET_Z0_TUNE_EN(x)                     ((u32)(((x >> 3) & 0x00000001)))
#define LSYS_BIT_UA_LV2HV_EN                       ((u32)0x00000001 << 2)          /*!<R/W 0  enable usb comparator l2h level shifter */
#define LSYS_UA_LV2HV_EN(x)                        ((u32)(((x) & 0x00000001) << 2))
#define LSYS_GET_UA_LV2HV_EN(x)                    ((u32)(((x >> 2) & 0x00000001)))
#define LSYS_BIT_PWC_UAHV_EN                       ((u32)0x00000001 << 1)          /*!<R/W 0  enable usb comparator 3.3v power cut */
#define LSYS_PWC_UAHV_EN(x)                        ((u32)(((x) & 0x00000001) << 1))
#define LSYS_GET_PWC_UAHV_EN(x)                    ((u32)(((x >> 1) & 0x00000001)))
#define LSYS_BIT_FS_COMP_EN                        ((u32)0x00000001 << 0)          /*!<R/W 0  enable usb full-speed differential comparator */
#define LSYS_FS_COMP_EN(x)                         ((u32)(((x) & 0x00000001) << 0))
#define LSYS_GET_FS_COMP_EN(x)                     ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_KM4_XO_CTRL0
 * @brief
 * @{
 **/
#define LSYS_MASK_INIT_XO_BASE                     ((u32)0xFFFFFFFF << 0)          /*!<R/W 0   */
#define LSYS_INIT_XO_BASE(x)                       (((u32)((x) & 0xFFFFFFFF) << 0))
#define LSYS_GET_INIT_XO_BASE(x)                   ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_LSYS_KM4_XO_CTRL1
 * @brief
 * @{
 **/
#define LSYS_MASK_INIT_XO_TOP                      ((u32)0xFFFFFFFF << 0)          /*!<R/W 0   */
#define LSYS_INIT_XO_TOP(x)                        (((u32)((x) & 0xFFFFFFFF) << 0))
#define LSYS_GET_INIT_XO_TOP(x)                    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_LSYS_PSRAMC_FLASH_CTRL
 * @brief
 * @{
 **/
#define LSYS_BIT_PSRAMC_FLASH_EN                   ((u32)0x00000001 << 30)          /*!<R/W 0  use psram_spic as flash controller, disable psram_phy */
#define LSYS_PSRAMC_FLASH_EN(x)                    ((u32)(((x) & 0x00000001) << 30))
#define LSYS_GET_PSRAMC_FLASH_EN(x)                ((u32)(((x >> 30) & 0x00000001)))
#define LSYS_BIT_FLASH_FEEDBACK_LOC                ((u32)0x00000001 << 31)          /*!<R/W 0  0: pinmux S0 -> flash_spic, pinmux S1 -> psram_spic(if psramc_flash_en) 1: pinmux S1 -> flash_spic, pinmux S0 -> psram_spic(if psramc_flash_en) */
#define LSYS_FLASH_FEEDBACK_LOC(x)                 ((u32)(((x) & 0x00000001) << 31))
#define LSYS_GET_FLASH_FEEDBACK_LOC(x)             ((u32)(((x >> 31) & 0x00000001)))
#define LSYS_BIT_PSRAM_FRQC_EN                     ((u32)0x00000001 << 27)          /*!<R/W 0  enable psram auto freq change control flow */
#define LSYS_PSRAM_FRQC_EN(x)                      ((u32)(((x) & 0x00000001) << 27))
#define LSYS_GET_PSRAM_FRQC_EN(x)                  ((u32)(((x >> 27) & 0x00000001)))
#define LSYS_BIT_PSRAM_FRQC_DONE                   ((u32)0x00000001 << 26)          /*!<R 0  indicate psram auto freq change is done */
#define LSYS_PSRAM_FRQC_DONE(x)                    ((u32)(((x) & 0x00000001) << 26))
#define LSYS_GET_PSRAM_FRQC_DONE(x)                ((u32)(((x >> 26) & 0x00000001)))
#define LSYS_MASK_PSRAM_FRQC_GRP                   ((u32)0x00000003 << 24)          /*!<R/W 0  select psram auto freq change group */
#define LSYS_PSRAM_FRQC_GRP(x)                     (((u32)((x) & 0x00000003) << 24))
#define LSYS_GET_PSRAM_FRQC_GRP(x)                 ((u32)(((x >> 24) & 0x00000003)))
#define LSYS_MASK_PSRAMC_DPHY_RD_DELAY_CNT         ((u32)0x0000000F << 17)          /*!<R/W 0  spic_dphy output dealy cnt, when set < 3, output data will occur at fourth or fifth axi clock from the rising edge of spi_rx_data_en; when set ≥ 3, data will occur after DPHY_RD_DELAY_CNT + 2 axi clock from the rising edge of spi_rx_data_en */
#define LSYS_PSRAMC_DPHY_RD_DELAY_CNT(x)           (((u32)((x) & 0x0000000F) << 17))
#define LSYS_GET_PSRAMC_DPHY_RD_DELAY_CNT(x)       ((u32)(((x >> 17) & 0x0000000F)))
#define LSYS_BIT_PSRAMC_DPHY_EN                    ((u32)0x00000001 << 16)          /*!<R/W 0  when set, enable spic_dphy function to read data from flash */
#define LSYS_PSRAMC_DPHY_EN(x)                     ((u32)(((x) & 0x00000001) << 16))
#define LSYS_GET_PSRAMC_DPHY_EN(x)                 ((u32)(((x >> 16) & 0x00000001)))
#define LSYS_BIT_PSRAMC_FLASH_CAL_EN               ((u32)0x00000001 << 15)          /*!<R/W 0  Flash calibration enable 1: delay flash sout for calibration 0: bypass flash sout to spic */
#define LSYS_PSRAMC_FLASH_CAL_EN(x)                ((u32)(((x) & 0x00000001) << 15))
#define LSYS_GET_PSRAMC_FLASH_CAL_EN(x)            ((u32)(((x >> 15) & 0x00000001)))
#define LSYS_BIT_PSRAMC_FLASH_DIV_EN               ((u32)0x00000001 << 13)          /*!<R/W 0  1: enable to generate flash clock (no phase shift) divided by 600M pll clock, HW detect this signal's rising edge to start the no phase shift division circuit. */
#define LSYS_PSRAMC_FLASH_DIV_EN(x)                ((u32)(((x) & 0x00000001) << 13))
#define LSYS_GET_PSRAMC_FLASH_DIV_EN(x)            ((u32)(((x >> 13) & 0x00000001)))
#define LSYS_BIT_PSRAMC_FLASH_PS_DIV_EN            ((u32)0x00000001 << 12)          /*!<R/W 0  1: enable to generate flash clock (with phase shift) divided by 600M pll clock, HW detect this signal's rising edge to start the phase shift clock division circuit. */
#define LSYS_PSRAMC_FLASH_PS_DIV_EN(x)             ((u32)(((x) & 0x00000001) << 12))
#define LSYS_GET_PSRAMC_FLASH_PS_DIV_EN(x)         ((u32)(((x >> 12) & 0x00000001)))
#define LSYS_MASK_PSRAMC_FLASH_DIV_INT             ((u32)0x0000000F << 8)          /*!<R/W 0h  Flash clock division ratio, integrate part 0: divided by 1 1: divided by 2 2: divided by 3 3: divided by 4 .. 7: divided by 8 8: divided by 9 9: divided by 10 Note: SPI clock frequency is this divided clock/2 */
#define LSYS_PSRAMC_FLASH_DIV_INT(x)               (((u32)((x) & 0x0000000F) << 8))
#define LSYS_GET_PSRAMC_FLASH_DIV_INT(x)           ((u32)(((x >> 8) & 0x0000000F)))
#define LSYS_MASK_PSRAMC_FLASH_PS_PHASE            ((u32)0x0000001F << 2)          /*!<R/W 0  Flash clock phase shift in units of cpu/dsp pll clock cycles. If selected pll is 500MHz, the step is 2ns. 0: shift 0 step 1: shift 1 step … 19: shift 19 step. Note, this value cannot be set more than FLASH_DIV_INT */
#define LSYS_PSRAMC_FLASH_PS_PHASE(x)              (((u32)((x) & 0x0000001F) << 2))
#define LSYS_GET_PSRAMC_FLASH_PS_PHASE(x)          ((u32)(((x >> 2) & 0x0000001F)))
#define LSYS_BIT_PSRAMC_FLASH_PS_DIV_RDY           ((u32)0x00000001 << 1)          /*!<R 0  Ready flag of Flash clock with phase shift, Read only (input signal) */
#define LSYS_PSRAMC_FLASH_PS_DIV_RDY(x)            ((u32)(((x) & 0x00000001) << 1))
#define LSYS_GET_PSRAMC_FLASH_PS_DIV_RDY(x)        ((u32)(((x >> 1) & 0x00000001)))
#define LSYS_BIT_PSRAMC_FLASH_DIV_RDY              ((u32)0x00000001 << 0)          /*!<R 0  Ready flag of Flash clock, Read only (input signal) */
#define LSYS_PSRAMC_FLASH_DIV_RDY(x)               ((u32)(((x) & 0x00000001) << 0))
#define LSYS_GET_PSRAMC_FLASH_DIV_RDY(x)           ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_I2S0_COMP_STEP
 * @brief
 * @{
 **/
#define LSYS_MASK_COMP_STEP_I2S0                   ((u32)0x03FFFFFF << 0)          /*!<R/W 0x19B4EC7  i2s sdm clock interpolation period . This mains every this value's /N cycles , it will interpolate one /N+1 or /N-1 clock to compensate period error. For example to generate 45.1584M from pll 600M (max precsion is 0.1K, step bit must > 400M/0.1K) 1. set i2s0_sdm_ckd = 12 (N=13 , for 600M/13 = 46.xx M , 600/14 = 42.xx M) 2. Calcurate compensation normalized step : target_per = 1000/45.1584 = 22.14427ns base_per = 13000/600 = 21.66667ns intp_per = 14000/600 = 23.33333ns err_accu = |target_per - base_per| = 0.4776 err_comp = |target_per - intp_per| = 1.18906 normalizaiton = err_accu/err_comp = 0.401667..... Convert the decimal into hexadecimal. change format to unsigned Q0.26 , which is floo( normalizaion * (1<<26)) */
#define LSYS_COMP_STEP_I2S0(x)                     (((u32)((x) & 0x03FFFFFF) << 0))
#define LSYS_GET_COMP_STEP_I2S0(x)                 ((u32)(((x >> 0) & 0x03FFFFFF)))
/** @} */

/** @defgroup REG_LSYS_I2S1_COMP_STEP
 * @brief
 * @{
 **/
#define LSYS_MASK_COMP_STEP_I2S1                   ((u32)0x03FFFFFF << 0)          /*!<R/W 0x763D55  i2s sdm clock interpolation period . This mains every this value's /N cycles , it will interpolate one /N+1 or /N-1 clock to compensate period error. For example to generate 98.304M from pll 600M 1. set i2s0_sdm_ckd = 5 (N=6 , for 600M/6 = 100M , 600/7 = 85.7 M) 2. Calcurate compensation normalized step : target_per = 1000/98.304 = 10.172526ns base_per = 6000/600 = 10ns intp_per = 7000/600 = 11.666667ns err_accu = |target_per - base_per| = 0.172526 err_comp = |target_per - intp_per| = 1.494141 if err_accu<err_comp normalizaiton = err_accu/err_comp = 0.11546835 else normalizaiton =err_comp/ err_accu , and modify comp_sel_i2s ,fen_sdm_i2s Convert the decimal into hexadecimal. change format to unsigned Q0.26 , which is floo( normalizaion * (1<<26)) */
#define LSYS_COMP_STEP_I2S1(x)                     (((u32)((x) & 0x03FFFFFF) << 0))
#define LSYS_GET_COMP_STEP_I2S1(x)                 ((u32)(((x >> 0) & 0x03FFFFFF)))
/** @} */

/** @defgroup REG_LSYS_I2S_SDM_CTRL
 * @brief
 * @{
 **/
#define LSYS_BIT_COMP_SEL_I2S1                     ((u32)0x00000001 << 25)          /*!<R/W 1  Determine to select /N+1 or /N-1 clock when interpolation. when base_per is is closer to target_per ,please select 1. otherwise select 0. 0: select /(N-1) 1: select /(N+1) */
#define LSYS_COMP_SEL_I2S1(x)                      ((u32)(((x) & 0x00000001) << 25))
#define LSYS_GET_COMP_SEL_I2S1(x)                  ((u32)(((x >> 25) & 0x00000001)))
#define LSYS_BIT_FEN_SDM_I2S1                      ((u32)0x00000001 << 24)          /*!<R/W 0  fen enable */
#define LSYS_FEN_SDM_I2S1(x)                       ((u32)(((x) & 0x00000001) << 24))
#define LSYS_GET_FEN_SDM_I2S1(x)                   ((u32)(((x >> 24) & 0x00000001)))
#define LSYS_MASK_CKD_SDM_I2S1                     ((u32)0x0000001F << 16)          /*!<R/W 5'd5  Set a divider to nearest the target frequency . The i2s sdm base divider factor N is this value + 1 */
#define LSYS_CKD_SDM_I2S1(x)                       (((u32)((x) & 0x0000001F) << 16))
#define LSYS_GET_CKD_SDM_I2S1(x)                   ((u32)(((x >> 16) & 0x0000001F)))
#define LSYS_BIT_COMP_SEL_I2S0                     ((u32)0x00000001 << 9)          /*!<R/W 1  Determine to select /N+1 or /N-1 clock when interpolation. when base_per is is closer to target_per ,please select 1. otherwise select 0. 0: select /(N-1) 1: select /(N+1) */
#define LSYS_COMP_SEL_I2S0(x)                      ((u32)(((x) & 0x00000001) << 9))
#define LSYS_GET_COMP_SEL_I2S0(x)                  ((u32)(((x >> 9) & 0x00000001)))
#define LSYS_BIT_FEN_SDM_I2S0                      ((u32)0x00000001 << 8)          /*!<R/W 0  fen enable */
#define LSYS_FEN_SDM_I2S0(x)                       ((u32)(((x) & 0x00000001) << 8))
#define LSYS_GET_FEN_SDM_I2S0(x)                   ((u32)(((x >> 8) & 0x00000001)))
#define LSYS_MASK_CKD_SDM_I2S0                     ((u32)0x0000001F << 0)          /*!<R/W 5'd12  Set a divider to nearest the target frequency . The i2s sdm base divider factor N is this value + 1 */
#define LSYS_CKD_SDM_I2S0(x)                       (((u32)((x) & 0x0000001F) << 0))
#define LSYS_GET_CKD_SDM_I2S0(x)                   ((u32)(((x >> 0) & 0x0000001F)))
/** @} */

/** @defgroup REG_LSYS_SPIC_CTRL
 * @brief
 * @{
 **/
#define LSYS_MASK_DPHY_RD_DELAY_CNT                ((u32)0x0000000F << 17)          /*!<R/W 0  spic_dphy output dealy cnt, when set < 3, output data will occur at fourth or fifth axi clock from the rising edge of spi_rx_data_en; when set ≥ 3, data will occur after DPHY_RD_DELAY_CNT + 2 axi clock from the rising edge of spi_rx_data_en */
#define LSYS_DPHY_RD_DELAY_CNT(x)                  (((u32)((x) & 0x0000000F) << 17))
#define LSYS_GET_DPHY_RD_DELAY_CNT(x)              ((u32)(((x >> 17) & 0x0000000F)))
#define LSYS_BIT_DPHY_EN                           ((u32)0x00000001 << 16)          /*!<R/W 0  when set, enable spic_dphy function to read data from flash */
#define LSYS_DPHY_EN(x)                            ((u32)(((x) & 0x00000001) << 16))
#define LSYS_GET_DPHY_EN(x)                        ((u32)(((x >> 16) & 0x00000001)))
#define LSYS_BIT_FLASH_CAL_EN                      ((u32)0x00000001 << 15)          /*!<R/W 0  Flash calibration enable 1: delay flash sout for calibration 0: bypass flash sout to spic */
#define LSYS_FLASH_CAL_EN(x)                       ((u32)(((x) & 0x00000001) << 15))
#define LSYS_GET_FLASH_CAL_EN(x)                   ((u32)(((x >> 15) & 0x00000001)))
#define LSYS_BIT_FLASH_DIV_EN                      ((u32)0x00000001 << 13)          /*!<R/W 0  1: enable to generate flash clock (no phase shift) divided by 400M pll clock, HW detect this signal's rising edge to start the no phase shift division circuit. */
#define LSYS_FLASH_DIV_EN(x)                       ((u32)(((x) & 0x00000001) << 13))
#define LSYS_GET_FLASH_DIV_EN(x)                   ((u32)(((x >> 13) & 0x00000001)))
#define LSYS_BIT_FLASH_PS_DIV_EN                   ((u32)0x00000001 << 12)          /*!<R/W 0  1: enable to generate flash clock (with phase shift) divided by 600M pll clock, HW detect this signal's rising edge to start the phase shift clock division circuit. */
#define LSYS_FLASH_PS_DIV_EN(x)                    ((u32)(((x) & 0x00000001) << 12))
#define LSYS_GET_FLASH_PS_DIV_EN(x)                ((u32)(((x >> 12) & 0x00000001)))
#define LSYS_MASK_FLASH_DIV_INT                    ((u32)0x0000000F << 8)          /*!<R/W 1h  Flash clock division ratio, integrate part 0: not support 1: divided by 2 2: divided by 3 3: divided by 4 .. 7: divided by 8 8: divided by 9 9: divided by 10 Note: SPI clock frequency is this divided clock/2 */
#define LSYS_FLASH_DIV_INT(x)                      (((u32)((x) & 0x0000000F) << 8))
#define LSYS_GET_FLASH_DIV_INT(x)                  ((u32)(((x >> 8) & 0x0000000F)))
#define LSYS_MASK_FLASH_PS_PHASE                   ((u32)0x0000001F << 2)          /*!<R/W 0  Flash clock phase shift in units of cpu/dsp pll clock cycles. If selected pll is 500MHz, the step is 2ns. 0: shift 0 step 1: shift 1 step … 19: shift 19 step. Note, this value cannot be set more than FLASH_DIV_INT */
#define LSYS_FLASH_PS_PHASE(x)                     (((u32)((x) & 0x0000001F) << 2))
#define LSYS_GET_FLASH_PS_PHASE(x)                 ((u32)(((x >> 2) & 0x0000001F)))
#define LSYS_BIT_FLASH_PS_DIV_RDY                  ((u32)0x00000001 << 1)          /*!<R 0  Ready flag of Flash clock with phase shift, Read only (input signal) */
#define LSYS_FLASH_PS_DIV_RDY(x)                   ((u32)(((x) & 0x00000001) << 1))
#define LSYS_GET_FLASH_PS_DIV_RDY(x)               ((u32)(((x >> 1) & 0x00000001)))
#define LSYS_BIT_FLASH_DIV_RDY                     ((u32)0x00000001 << 0)          /*!<R 0  Ready flag of Flash clock, Read only (input signal) */
#define LSYS_FLASH_DIV_RDY(x)                      ((u32)(((x) & 0x00000001) << 0))
#define LSYS_GET_FLASH_DIV_RDY(x)                  ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_SW_RST_CTRL
 * @brief
 * @{
 **/
#define LSYS_BIT_KM4_SYS_RST                       ((u32)0x00000001 << 31)          /*!<R/WE/EC 0  Only when REG_SW_RST_TRIG is 0x96969696 , this reg can be write */
#define LSYS_KM4_SYS_RST(x)                        ((u32)(((x) & 0x00000001) << 31))
#define LSYS_GET_KM4_SYS_RST(x)                    ((u32)(((x >> 31) & 0x00000001)))
#define LSYS_BIT_KM0_SYS_RST                       ((u32)0x00000001 << 30)          /*!<R/WE/EC 0  Only when REG_SW_RST_TRIG is 0x96969696 , this reg can be write */
#define LSYS_KM0_SYS_RST(x)                        ((u32)(((x) & 0x00000001) << 30))
#define LSYS_GET_KM0_SYS_RST(x)                    ((u32)(((x >> 30) & 0x00000001)))
#define LSYS_BIT_PERIALL_RST_EN                    ((u32)0x00000001 << 1)          /*!<R/W 0  1. warm reset request by one cpu will reset all peri and local cpu 0. warm reset request by one cpu will only reset local cpu Note : peri rst will use sysrst mst also */
#define LSYS_PERIALL_RST_EN(x)                     ((u32)(((x) & 0x00000001) << 1))
#define LSYS_GET_PERIALL_RST_EN(x)                 ((u32)(((x >> 1) & 0x00000001)))
#define LSYS_BIT_OTHERCPU_RST_EN                   ((u32)0x00000001 << 0)          /*!<R/W 0  1. warm reset request by one cpu will reset other cpu's core and local cpu 0. warm reset request by one cpu will only reset local cpu */
#define LSYS_OTHERCPU_RST_EN(x)                    ((u32)(((x) & 0x00000001) << 0))
#define LSYS_GET_OTHERCPU_RST_EN(x)                ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_SW_RST_TRIG
 * @brief
 * @{
 **/
#define LSYS_MASK_TRIG_RST                         ((u32)0xFFFFFFFF << 0)          /*!<R/WPD 0  System reset trigger register Write 0x96969696 to enable the write of reset control Write 0x69696969 will trigger the system if REG_SW_RST_CTRL set . */
#define LSYS_TRIG_RST(x)                           (((u32)((x) & 0xFFFFFFFF) << 0))
#define LSYS_GET_TRIG_RST(x)                       ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_LSYS_BOOT_REASON_SW
 * @brief
 * @{
 **/
#define LSYS_MASK_GDMA_CHANNELS                    ((u32)0x000000FF << 24)          /*!<R/W 0  Indicate the usage status of every GDMA channel. */
#define LSYS_GDMA_CHANNELS(x)                      (((u32)((x) & 0x000000FF) << 24))
#define LSYS_GET_GDMA_CHANNELS(x)                  ((u32)(((x >> 24) & 0x000000FF)))
/** @} */

/** @defgroup REG_LSYS_BOOT_CFG
 * @brief
 * @{
 **/
#define LSYS_BIT_BOOT_KM0_RUN                      ((u32)0x00000001 << 31)          /*!<R/W 0   */
#define LSYS_BOOT_KM0_RUN(x)                       ((u32)(((x) & 0x00000001) << 31))
#define LSYS_GET_BOOT_KM0_RUN(x)                   ((u32)(((x >> 31) & 0x00000001)))
#define LSYS_BIT_BOOT_WAKE_FROM_PS_LS              ((u32)0x00000001 << 30)          /*!<R/W 0   */
#define LSYS_BOOT_WAKE_FROM_PS_LS(x)               ((u32)(((x) & 0x00000001) << 30))
#define LSYS_GET_BOOT_WAKE_FROM_PS_LS(x)           ((u32)(((x >> 30) & 0x00000001)))
#define LSYS_BIT_BOOT_WAKE_FROM_PS_HS              ((u32)0x00000001 << 29)          /*!<R/W 0   */
#define LSYS_BOOT_WAKE_FROM_PS_HS(x)               ((u32)(((x) & 0x00000001) << 29))
#define LSYS_GET_BOOT_WAKE_FROM_PS_HS(x)           ((u32)(((x >> 29) & 0x00000001)))
#define LSYS_BIT_BOOT_PATCH_FUNC0                  ((u32)0x00000001 << 28)          /*!<R/W 0   */
#define LSYS_BOOT_PATCH_FUNC0(x)                   ((u32)(((x) & 0x00000001) << 28))
#define LSYS_GET_BOOT_PATCH_FUNC0(x)               ((u32)(((x >> 28) & 0x00000001)))
#define LSYS_BIT_BOOT_PATCH_FUNC1                  ((u32)0x00000001 << 27)          /*!<R/W 0   */
#define LSYS_BOOT_PATCH_FUNC1(x)                   ((u32)(((x) & 0x00000001) << 27))
#define LSYS_GET_BOOT_PATCH_FUNC1(x)               ((u32)(((x >> 27) & 0x00000001)))
#define LSYS_MASK_BOOT_RSVD_SW                     ((u32)0x000007FF << 16)          /*!<R/W 0   */
#define LSYS_BOOT_RSVD_SW(x)                       (((u32)((x) & 0x000007FF) << 16))
#define LSYS_GET_BOOT_RSVD_SW(x)                   ((u32)(((x >> 16) & 0x000007FF)))
#define LSYS_MASK_ROM_VERSION_SW                   ((u32)0x0000FFFF << 0)          /*!<R/W 0   */
#define LSYS_ROM_VERSION_SW(x)                     (((u32)((x) & 0x0000FFFF) << 0))
#define LSYS_GET_ROM_VERSION_SW(x)                 ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup REG_LSYS_SW_PATCH
 * @brief
 * @{
 **/
#define LSYS_MASK_SW_PATCH_SADDR                   ((u32)0xFFFFFFFF << 0)          /*!<R/W 0  SW patch code start addr. OTP physical addr 0x500-0x6FF will autoload to this addr . */
#define LSYS_SW_PATCH_SADDR(x)                     (((u32)((x) & 0xFFFFFFFF) << 0))
#define LSYS_GET_SW_PATCH_SADDR(x)                 ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_LSYS_CHIP_INFO
 * @brief
 * @{
 **/
#define LSYS_MASK_CHIP_INFO_EN                     ((u32)0x0000000F << 28)          /*!<R/W 0  Enable Internal Chip Information display.(Don't move 0x74 address , for code is not auto-gen) 0xA: enable. Others: disable. Note: CHIP_INFO can be read only if CHIP_INFO_EN[3:0]=0xA, otherwise the read result will return 0x0. */
#define LSYS_CHIP_INFO_EN(x)                       (((u32)((x) & 0x0000000F) << 28))
#define LSYS_GET_CHIP_INFO_EN(x)                   ((u32)(((x >> 28) & 0x0000000F)))
#define LSYS_MASK_RL_VER                           ((u32)0x0000000F << 16)          /*!<RMSK1 0  CHIP cut verison : 0: A-cut 1: B-cut … */
#define LSYS_RL_VER(x)                             (((u32)((x) & 0x0000000F) << 16))
#define LSYS_GET_RL_VER(x)                         ((u32)(((x >> 16) & 0x0000000F)))
#define LSYS_MASK_RL_NO                            ((u32)0x0000FFFF << 0)          /*!<RMSK1 0x6845  CHIP RL NUMBER : 0x6845 */
#define LSYS_RL_NO(x)                              (((u32)((x) & 0x0000FFFF) << 0))
#define LSYS_GET_RL_NO(x)                          ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup REG_LSYS_BT_32K_CTRL0
 * @brief
 * @{
 **/
#define LSYS_MASK_XTAL_CLK_FREQ_NM                 ((u32)0x0007FFFF << 0)          /*!<R/W 0x4C4B4  used for BT 32K clock. indicate the frequence of XTAL Normal Mode, default is 312.5KHz */
#define LSYS_XTAL_CLK_FREQ_NM(x)                   (((u32)((x) & 0x0007FFFF) << 0))
#define LSYS_GET_XTAL_CLK_FREQ_NM(x)               ((u32)(((x >> 0) & 0x0007FFFF)))
/** @} */

/** @defgroup REG_LSYS_BT_32K_CTRL1
 * @brief
 * @{
 **/
#define LSYS_MASK_XTAL_CLK_FREQ_LPS                ((u32)0x0007FFFF << 0)          /*!<R/W 0x4C4B4  used for BT 32K clock. indicate the frequence of XTAL LPS Mode, default is 312.5KHz */
#define LSYS_XTAL_CLK_FREQ_LPS(x)                  (((u32)((x) & 0x0007FFFF) << 0))
#define LSYS_GET_XTAL_CLK_FREQ_LPS(x)              ((u32)(((x >> 0) & 0x0007FFFF)))
/** @} */

/** @defgroup REG_LSYS_BT_CTRL0
 * @brief
 * @{
 **/
#define LSYS_MASK_BT_SWR_STS_NM                    ((u32)0x00000007 << 29)          /*!<R/W 101  SET SWR state when BT in active mode 000: RSVD 001: state1 0.7/PC/PFM 010: state2 0.8/PC/PFM 011: state3 0.9/PC/PFM 100: state4 0.9/SLP/PFM 101: state5 0.9/SLP/PWM 110: state6 0.9/NORM/PWM 111: state7 1.0/NORM/PWM */
#define LSYS_BT_SWR_STS_NM(x)                      (((u32)((x) & 0x00000007) << 29))
#define LSYS_GET_BT_SWR_STS_NM(x)                  ((u32)(((x >> 29) & 0x00000007)))
#define LSYS_BIT_BTWL_USE_LOGUART_PAD_ST           ((u32)0x00000001 << 28)          /*!<R   The state of xor of WL_USE_REQ and BT_USE_REQ. 0: loguart use loguart trx pad 1: bt hci use loguart trx pad */
#define LSYS_BTWL_USE_LOGUART_PAD_ST(x)            ((u32)(((x) & 0x00000001) << 28))
#define LSYS_GET_BTWL_USE_LOGUART_PAD_ST(x)        ((u32)(((x >> 28) & 0x00000001)))
#define LSYS_BIT_WL_MP_STATUS                      ((u32)0x00000001 << 27)          /*!<R/W 0  Wi-Fi MP status */
#define LSYS_WL_MP_STATUS(x)                       ((u32)(((x) & 0x00000001) << 27))
#define LSYS_GET_WL_MP_STATUS(x)                   ((u32)(((x >> 27) & 0x00000001)))
#define LSYS_BIT_BT_USE_EXT_BT                     ((u32)0x00000001 << 26)          /*!<R/W 0  1: select external BT 0: select internal BT */
#define LSYS_BT_USE_EXT_BT(x)                      ((u32)(((x) & 0x00000001) << 26))
#define LSYS_GET_BT_USE_EXT_BT(x)                  ((u32)(((x >> 26) & 0x00000001)))
#define LSYS_BIT_WL_USE_REQ                        ((u32)0x00000001 << 25)          /*!<R/W 0  who uses loguart trx pad depends on the xor of WL_USE_REQ and BT_USE_REQ, wifi can send use request by change the value of WL_USE_REQ */
#define LSYS_WL_USE_REQ(x)                         ((u32)(((x) & 0x00000001) << 25))
#define LSYS_GET_WL_USE_REQ(x)                     ((u32)(((x >> 25) & 0x00000001)))
#define LSYS_BIT_FORCE_LOGUART_USE_LOGUART_PAD_B   ((u32)0x00000001 << 24)          /*!<R/W 0  Force wifi soc to use loguart pad mandatory if necessary 0: loguart use loguart trx pad 1: who uses loguart trx pad depends on BTWL_USE_LOGUART_PAD_ST */
#define LSYS_FORCE_LOGUART_USE_LOGUART_PAD_B(x)    ((u32)(((x) & 0x00000001) << 24))
#define LSYS_GET_FORCE_LOGUART_USE_LOGUART_PAD_B(x) ((u32)(((x >> 24) & 0x00000001)))
#define LSYS_MASK_BT_XTAL_MODE_NM                  ((u32)0x00000007 << 21)          /*!<R/W 100  SET XTAL state when BT in active mode 000: XTAL OFF 001: XTAL LPS, gating XTAL 40M 010: XTAL LPS, enable XTAL 40M 011: XTAL NM mode 100: XTAL HP mode */
#define LSYS_BT_XTAL_MODE_NM(x)                    (((u32)((x) & 0x00000007) << 21))
#define LSYS_GET_BT_XTAL_MODE_NM(x)                ((u32)(((x >> 21) & 0x00000007)))
#define LSYS_BIT_BT_EN_TST_SI_CTRL_RFC             ((u32)0x00000001 << 20)          /*!<R/W 1  进入Test Mode 8 , 且同時 BT_EN_TST_SI_CTRL_RFC=1 , 才將RFC 端的SI 3 wire交由專線BT_SI_*_TST 控制； 若I2C 想要access RFC,设置BT_EN_TST_SI_CTRL_RFC=0 或者 將BT_SI_ENB_TST pull high(如果BT_SI_ENB_TST 有出 PIN) */
#define LSYS_BT_EN_TST_SI_CTRL_RFC(x)              ((u32)(((x) & 0x00000001) << 20))
#define LSYS_GET_BT_EN_TST_SI_CTRL_RFC(x)          ((u32)(((x >> 20) & 0x00000001)))
#define LSYS_BIT_BT_USE_EXT_UART                   ((u32)0x00000001 << 19)          /*!<R/W 0  1: BT use external uart 0: BT use internal uart */
#define LSYS_BT_USE_EXT_UART(x)                    ((u32)(((x) & 0x00000001) << 19))
#define LSYS_GET_BT_USE_EXT_UART(x)                ((u32)(((x >> 19) & 0x00000001)))
#define LSYS_BIT_BT_CTRL_RFC_SEL_SI                ((u32)0x00000001 << 18)          /*!<R/W 0  Normal mode 或者Test Mode 2,通过此寄存器选择RFC的控制端。 BT_CTRL_RFC_SEL_SI = 1,將RFC 端的SI 3 wire交由專線BT_SI_*_TST 控制；BT_CTRL_RFC_SEL_SI = 0,將RFC 端的SI 3 wire交由 I2C 控制。 BT_CTRL_RFC_SEL_SI = 1时,也可通过将BT_SI_ENB_TST pull high,交由PI控制 */
#define LSYS_BT_CTRL_RFC_SEL_SI(x)                 ((u32)(((x) & 0x00000001) << 18))
#define LSYS_GET_BT_CTRL_RFC_SEL_SI(x)             ((u32)(((x >> 18) & 0x00000001)))
#define LSYS_BIT_BOOT_RSVD_BT_SW1                  ((u32)0x00000001 << 17)          /*!<R/W 0  RSVD for sotfware */
#define LSYS_BOOT_RSVD_BT_SW1(x)                   ((u32)(((x) & 0x00000001) << 17))
#define LSYS_GET_BOOT_RSVD_BT_SW1(x)               ((u32)(((x >> 17) & 0x00000001)))
#define LSYS_BIT_BOOT_RSVD_BT_SW2                  ((u32)0x00000001 << 16)          /*!<R/W 0  RSVD for sotfware */
#define LSYS_BOOT_RSVD_BT_SW2(x)                   ((u32)(((x) & 0x00000001) << 16))
#define LSYS_GET_BOOT_RSVD_BT_SW2(x)               ((u32)(((x >> 16) & 0x00000001)))
#define LSYS_BIT_BT_WAKE_HOST_POL                  ((u32)0x00000001 << 15)          /*!<R/W 1  1:BT_WAKE_HOST to CPU and pinmux 0:pinmux to CPU for external trig */
#define LSYS_BT_WAKE_HOST_POL(x)                   ((u32)(((x) & 0x00000001) << 15))
#define LSYS_GET_BT_WAKE_HOST_POL(x)               ((u32)(((x >> 15) & 0x00000001)))
#define LSYS_BIT_HOST_WAKE_BT_POL                  ((u32)0x00000001 << 14)          /*!<R/W 1  1:HOST_WAKE_BT to BT and pinmux 0:pinmux to BT for external trig */
#define LSYS_HOST_WAKE_BT_POL(x)                   ((u32)(((x) & 0x00000001) << 14))
#define LSYS_GET_HOST_WAKE_BT_POL(x)               ((u32)(((x >> 14) & 0x00000001)))
#define LSYS_BIT_HOST_WAKE_BT                      ((u32)0x00000001 << 13)          /*!<R/W 0  CPU set this bit to 1 to wake BT */
#define LSYS_HOST_WAKE_BT(x)                       ((u32)(((x) & 0x00000001) << 13))
#define LSYS_GET_HOST_WAKE_BT(x)                   ((u32)(((x >> 13) & 0x00000001)))
#define LSYS_MASK_BT_XTAL_MODE_LPS                 ((u32)0x00000007 << 10)          /*!<R/W 1  SET XTAL state when BT in LPS mode 000: XTAL OFF 001: XTAL LPS, gating XTAL 40M 010: XTAL LPS, enable XTAL 40M 011: XTAL NM mode 100: XTAL HP mode */
#define LSYS_BT_XTAL_MODE_LPS(x)                   (((u32)((x) & 0x00000007) << 10))
#define LSYS_GET_BT_XTAL_MODE_LPS(x)               ((u32)(((x >> 10) & 0x00000007)))
#define LSYS_BIT_BT_I2C_TEST_EN                    ((u32)0x00000001 << 9)          /*!<R/W 0  SW control BT enter I2C test mode */
#define LSYS_BT_I2C_TEST_EN(x)                     ((u32)(((x) & 0x00000001) << 9))
#define LSYS_GET_BT_I2C_TEST_EN(x)                 ((u32)(((x >> 9) & 0x00000001)))
#define LSYS_MASK_BT_MEM_PWRC_LPS                  ((u32)0x00000007 << 6)          /*!<R/W 3'b010  BT DMEM/BFMEM power mode control in SLEEP. Setted before chip goes to SLEEP.Used to controll power mode of BT memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. 000 - active 001 - light sleep 010 - deep sleep 100 - shut down */
#define LSYS_BT_MEM_PWRC_LPS(x)                    (((u32)((x) & 0x00000007) << 6))
#define LSYS_GET_BT_MEM_PWRC_LPS(x)                ((u32)(((x >> 6) & 0x00000007)))
#define LSYS_BIT_SET_BT_OFF_PARAM                  ((u32)0x00000001 << 5)          /*!<R/W 0  level -> pulse. Write to 1 to disable XTAL and SWR, before BT OFF. Before Write to 1 or after BT OFF,need to write to 0 */
#define LSYS_SET_BT_OFF_PARAM(x)                   ((u32)(((x) & 0x00000001) << 5))
#define LSYS_GET_SET_BT_OFF_PARAM(x)               ((u32)(((x >> 5) & 0x00000001)))
#define LSYS_BIT_BT_SYS_BTDBG_SEL                  ((u32)0x00000001 << 4)          /*!<R/W 0  0: select debug port from BTTOP 1: debug_port[31:18] from bt_misc; else from BTTOP */
#define LSYS_BT_SYS_BTDBG_SEL(x)                   ((u32)(((x) & 0x00000001) << 4))
#define LSYS_GET_BT_SYS_BTDBG_SEL(x)               ((u32)(((x >> 4) & 0x00000001)))
#define LSYS_BIT_DSWR_REF_BT                       ((u32)0x00000001 << 3)          /*!<R/W 1  core swr parameter refer BT status */
#define LSYS_DSWR_REF_BT(x)                        ((u32)(((x) & 0x00000001) << 3))
#define LSYS_GET_DSWR_REF_BT(x)                    ((u32)(((x >> 3) & 0x00000001)))
#define LSYS_MASK_BT_SWR_STS_LPS                   ((u32)0x00000007 << 0)          /*!<R/W 001  SET SWR state when BT in LPS mode 000: RSVD 001: state1 0.7/PC/PFM 010: state2 0.8/PC/PFM 011: state3 0.9/PC/PFM 100: state4 0.9/SLP/PFM 101: state5 0.9/SLP/PWM 110: state6 0.9/NORM/PWM 111: state7 1.0/NORM/PWM */
#define LSYS_BT_SWR_STS_LPS(x)                     (((u32)((x) & 0x00000007) << 0))
#define LSYS_GET_BT_SWR_STS_LPS(x)                 ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_LSYS_BT_CTRL1
 * @brief
 * @{
 **/
#define LSYS_MASK_BT32_DIV_FREQ                    ((u32)0x00007FFF << 17)          /*!<R/W 0x7D00  indicate the frequence of BT32K, default is 32KHz */
#define LSYS_BT32_DIV_FREQ(x)                      (((u32)((x) & 0x00007FFF) << 17))
#define LSYS_GET_BT32_DIV_FREQ(x)                  ((u32)(((x >> 17) & 0x00007FFF)))
#define LSYS_BIT_CKSL_BT_XTAL32K                   ((u32)0x00000001 << 16)          /*!<R/W 0x1  select BT 32K source 1: XTAL 0: OSC */
#define LSYS_CKSL_BT_XTAL32K(x)                    ((u32)(((x) & 0x00000001) << 16))
#define LSYS_GET_CKSL_BT_XTAL32K(x)                ((u32)(((x >> 16) & 0x00000001)))
#define LSYS_BIT_BT_CKSL_CKANA                     ((u32)0x00000001 << 15)          /*!<R/W 0x1  1: enable osc4m when BT enter LPS 0: switch bt_ana_clk from osc4m to ana_100k when BT enter LPS */
#define LSYS_BT_CKSL_CKANA(x)                      ((u32)(((x) & 0x00000001) << 15))
#define LSYS_GET_BT_CKSL_CKANA(x)                  ((u32)(((x >> 15) & 0x00000001)))
#define LSYS_MASK_BT_PMC_STATE                     ((u32)0x00007FFF << 0)          /*!<R 0  indicate bt_misc PMC current state, for debug */
#define LSYS_BT_PMC_STATE(x)                       (((u32)((x) & 0x00007FFF) << 0))
#define LSYS_GET_BT_PMC_STATE(x)                   ((u32)(((x >> 0) & 0x00007FFF)))
/** @} */

/** @defgroup REG_LSYS_AIP_CTRL0
 * @brief
 * @{
 **/
#define LSYS_BIT_CK_CAL_START                      ((u32)0x00000001 << 31)          /*!<R/W/ES 0x0  Start clk calibration . This bit will auto-set to 0 when cal done . */
#define LSYS_CK_CAL_START(x)                       ((u32)(((x) & 0x00000001) << 31))
#define LSYS_GET_CK_CAL_START(x)                   ((u32)(((x >> 31) & 0x00000001)))
#define LSYS_MASK_CK_CAL_SEL                       ((u32)0x00000003 << 16)          /*!<R/W 0x0  Calibration clock selection . The refrence clock is xtal clock. 0: OSC4M 1: AON100K 2: AON128K for SDM */
#define LSYS_CK_CAL_SEL(x)                         (((u32)((x) & 0x00000003) << 16))
#define LSYS_GET_CK_CAL_SEL(x)                     ((u32)(((x >> 16) & 0x00000003)))
#define LSYS_MASK_CK_CAL_RPT                       ((u32)0x0000FFFF << 0)          /*!<R   Clock calibration result */
#define LSYS_CK_CAL_RPT(x)                         (((u32)((x) & 0x0000FFFF) << 0))
#define LSYS_GET_CK_CAL_RPT(x)                     ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup REG_LSYS_MON_CTRL
 * @brief
 * @{
 **/
#define LSYS_BIT_MSK_MONITOR                       ((u32)0x00000001 << 4)          /*!<R/W 0x0  ISR mask , if 1 , cpu will not receive interrup , although isr_monitor is assert */
#define LSYS_MSK_MONITOR(x)                        ((u32)(((x) & 0x00000001) << 4))
#define LSYS_GET_MSK_MONITOR(x)                    ((u32)(((x >> 4) & 0x00000001)))
#define LSYS_BIT_EN_MONITOR                        ((u32)0x00000001 << 1)          /*!<R/W 0x0  Indicates whether enable the monitor function. When The monitor FIFO capture write event, it will trigger the sys illegal write interrupt, the interrupt source will auto clear when read out all the data~ */
#define LSYS_EN_MONITOR(x)                         ((u32)(((x) & 0x00000001) << 1))
#define LSYS_GET_EN_MONITOR(x)                     ((u32)(((x >> 1) & 0x00000001)))
#define LSYS_BIT_ISR_MONITOR                       ((u32)0x00000001 << 0)          /*!<RW1C 0x0  Indicates the monitor FIFO has more */
#define LSYS_ISR_MONITOR(x)                        ((u32)(((x) & 0x00000001) << 0))
#define LSYS_GET_ISR_MONITOR(x)                    ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_MON_WREG0
 * @brief
 * @{
 **/
#define LSYS_MASK_ILL_WADDR                        ((u32)0x00003FFF << 0)          /*!<RP 0x0  Illegal write addr. every read ,it will update waddr and wdata */
#define LSYS_ILL_WADDR(x)                          (((u32)((x) & 0x00003FFF) << 0))
#define LSYS_GET_ILL_WADDR(x)                      ((u32)(((x >> 0) & 0x00003FFF)))
/** @} */

/** @defgroup REG_LSYS_MON_WREG1
 * @brief
 * @{
 **/
#define LSYS_MASK_ILL_WDATA                        ((u32)0xFFFFFFFF << 0)          /*!<R 0x0  Illegal write value */
#define LSYS_ILL_WDATA(x)                          (((u32)((x) & 0xFFFFFFFF) << 0))
#define LSYS_GET_ILL_WDATA(x)                      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_LSYS_DW_GPIO_CTRL
 * @brief
 * @{
 **/
#define LSYS_MASK_GPIO_DIVCNT_PB                   ((u32)0x0000007F << 8)          /*!<R/W 0  divider for dw gpiob debounce clk */
#define LSYS_GPIO_DIVCNT_PB(x)                     (((u32)((x) & 0x0000007F) << 8))
#define LSYS_GET_GPIO_DIVCNT_PB(x)                 ((u32)(((x >> 8) & 0x0000007F)))
#define LSYS_MASK_GPIO_DIVCNT_PA                   ((u32)0x0000007F << 0)          /*!<R/W 0  divider for dw gpioa debounce clk */
#define LSYS_GPIO_DIVCNT_PA(x)                     (((u32)((x) & 0x0000007F) << 0))
#define LSYS_GET_GPIO_DIVCNT_PA(x)                 ((u32)(((x >> 0) & 0x0000007F)))
/** @} */

/** @defgroup REG_LSYS_LXBUS_DBG0
 * @brief
 * @{
 **/
#define LSYS_MASK_TIMEOUT_ADDR_MST                 ((u32)0xFFFFFFFF << 0)          /*!<R 0  lx sys bus addr debug out */
#define LSYS_TIMEOUT_ADDR_MST(x)                   (((u32)((x) & 0xFFFFFFFF) << 0))
#define LSYS_GET_TIMEOUT_ADDR_MST(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_LSYS_LXBUS_DBG1
 * @brief
 * @{
 **/
#define LSYS_MASK_TIMEOUT_ADDR_SLV                 ((u32)0xFFFFFFFF << 0)          /*!<R 0  lx sys bus addr debug out */
#define LSYS_TIMEOUT_ADDR_SLV(x)                   (((u32)((x) & 0xFFFFFFFF) << 0))
#define LSYS_GET_TIMEOUT_ADDR_SLV(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_LSYS_LXBUS_DBG_CTRL
 * @brief
 * @{
 **/
#define LSYS_BIT_LX1_LOCK_OK                       ((u32)0x00000001 << 12)          /*!<R 0  lock lx1 bus lock ok */
#define LSYS_LX1_LOCK_OK(x)                        ((u32)(((x) & 0x00000001) << 12))
#define LSYS_GET_LX1_LOCK_OK(x)                    ((u32)(((x >> 12) & 0x00000001)))
#define LSYS_BIT_LX1_LOCK_BUS                      ((u32)0x00000001 << 8)          /*!<R/W 0  lock lx1 bus for debug */
#define LSYS_LX1_LOCK_BUS(x)                       ((u32)(((x) & 0x00000001) << 8))
#define LSYS_GET_LX1_LOCK_BUS(x)                   ((u32)(((x >> 8) & 0x00000001)))
#define LSYS_MASK_BUSTIMEOUT                       ((u32)0x00000003 << 4)          /*!<R/W 0  lx sys bus arb timeout thresh */
#define LSYS_BUSTIMEOUT(x)                         (((u32)((x) & 0x00000003) << 4))
#define LSYS_GET_BUSTIMEOUT(x)                     ((u32)(((x >> 4) & 0x00000003)))
#define LSYS_BIT_ENBUSTIMEOUT                      ((u32)0x00000001 << 0)          /*!<R/W 0  lx sys bus arb timeout enable */
#define LSYS_ENBUSTIMEOUT(x)                       ((u32)(((x) & 0x00000001) << 0))
#define LSYS_GET_ENBUSTIMEOUT(x)                   ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_FPGA_CLK_INFO
 * @brief
 * @{
 **/
#define LSYS_MASK_FPGA_KM0_CLK                     ((u32)0x000000FF << 24)          /*!<R 0  FPGA KM0 clock */
#define LSYS_FPGA_KM0_CLK(x)                       (((u32)((x) & 0x000000FF) << 24))
#define LSYS_GET_FPGA_KM0_CLK(x)                   ((u32)(((x >> 24) & 0x000000FF)))
#define LSYS_MASK_FPGA_KM4_CLK                     ((u32)0x000000FF << 16)          /*!<R 0  FPGA KM4 clock */
#define LSYS_FPGA_KM4_CLK(x)                       (((u32)((x) & 0x000000FF) << 16))
#define LSYS_GET_FPGA_KM4_CLK(x)                   ((u32)(((x >> 16) & 0x000000FF)))
#define LSYS_MASK_FPGA_PSRAM_CLK                   ((u32)0x000000FF << 8)          /*!<R 0  FPGA PSRAM clock */
#define LSYS_FPGA_PSRAM_CLK(x)                     (((u32)((x) & 0x000000FF) << 8))
#define LSYS_GET_FPGA_PSRAM_CLK(x)                 ((u32)(((x >> 8) & 0x000000FF)))
#define LSYS_MASK_FPGA_SPIC_CLK                    ((u32)0x000000FF << 0)          /*!<R 0  FPGA SPIC clock */
#define LSYS_FPGA_SPIC_CLK(x)                      (((u32)((x) & 0x000000FF) << 0))
#define LSYS_GET_FPGA_SPIC_CLK(x)                  ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup REG_LSYS_DSS_CTRL
 * @brief
 * @{
 **/
#define LSYS_MASK_DSS_RSTN                         ((u32)0x00000003 << 28)          /*!<R/W 0   */
#define LSYS_DSS_RSTN(x)                           (((u32)((x) & 0x00000003) << 28))
#define LSYS_GET_DSS_RSTN(x)                       ((u32)(((x >> 28) & 0x00000003)))
#define LSYS_BIT_DSS_WIRE_SEL                      ((u32)0x00000001 << 25)          /*!<R/W 0   */
#define LSYS_DSS_WIRE_SEL(x)                       ((u32)(((x) & 0x00000001) << 25))
#define LSYS_GET_DSS_WIRE_SEL(x)                   ((u32)(((x >> 25) & 0x00000001)))
#define LSYS_BIT_DSS_SPEED_EN                      ((u32)0x00000001 << 24)          /*!<R/W 0   */
#define LSYS_DSS_SPEED_EN(x)                       ((u32)(((x) & 0x00000001) << 24))
#define LSYS_GET_DSS_SPEED_EN(x)                   ((u32)(((x >> 24) & 0x00000001)))
#define LSYS_MASK_DSS_RO_SEL                       ((u32)0x00000007 << 20)          /*!<R/W 0   */
#define LSYS_DSS_RO_SEL(x)                         (((u32)((x) & 0x00000007) << 20))
#define LSYS_GET_DSS_RO_SEL(x)                     ((u32)(((x >> 20) & 0x00000007)))
#define LSYS_MASK_DSS_DATA_IN                      ((u32)0x000FFFFF << 0)          /*!<R/W 0   */
#define LSYS_DSS_DATA_IN(x)                        (((u32)((x) & 0x000FFFFF) << 0))
#define LSYS_GET_DSS_DATA_IN(x)                    ((u32)(((x >> 0) & 0x000FFFFF)))
/** @} */

/** @defgroup REG_LSYS_DSS_OUT
 * @brief
 * @{
 **/
#define LSYS_BIT_DSS_OUT_SEL                       ((u32)0x00000001 << 24)          /*!<R/W 0  0: select lvt dss 1: select uhvt dss */
#define LSYS_DSS_OUT_SEL(x)                        ((u32)(((x) & 0x00000001) << 24))
#define LSYS_GET_DSS_OUT_SEL(x)                    ((u32)(((x >> 24) & 0x00000001)))
#define LSYS_BIT_DSS_WSORT_GO                      ((u32)0x00000001 << 21)          /*!<R 0   */
#define LSYS_DSS_WSORT_GO(x)                       ((u32)(((x) & 0x00000001) << 21))
#define LSYS_GET_DSS_WSORT_GO(x)                   ((u32)(((x >> 21) & 0x00000001)))
#define LSYS_BIT_DSS_READY                         ((u32)0x00000001 << 20)          /*!<R 0   */
#define LSYS_DSS_READY(x)                          ((u32)(((x) & 0x00000001) << 20))
#define LSYS_GET_DSS_READY(x)                      ((u32)(((x >> 20) & 0x00000001)))
#define LSYS_MASK_DSS_DATA_OUT                     ((u32)0x000FFFFF << 0)          /*!<R 0   */
#define LSYS_DSS_DATA_OUT(x)                       (((u32)((x) & 0x000FFFFF) << 0))
#define LSYS_GET_DSS_DATA_OUT(x)                   ((u32)(((x >> 0) & 0x000FFFFF)))
/** @} */

/** @defgroup REG_LSYS_UPS_CTRL
 * @brief
 * @{
 **/
#define LSYS_MASK_UPS_SRC_SEL                      ((u32)0x0000000F << 28)          /*!<R/W 0  UPS trigger source select: 000: none 001: gpio trigger 010: comparator output 011: audio zcd_0 100: audio zcd_1 101: audio zcd_2 */
#define LSYS_UPS_SRC_SEL(x)                        (((u32)((x) & 0x0000000F) << 28))
#define LSYS_GET_UPS_SRC_SEL(x)                    ((u32)(((x >> 28) & 0x0000000F)))
#define LSYS_BIT_UPS_OUT_PWM_TRIG                  ((u32)0x00000001 << 21)          /*!<R/W 0  Enable for output trigger to pwm */
#define LSYS_UPS_OUT_PWM_TRIG(x)                   ((u32)(((x) & 0x00000001) << 21))
#define LSYS_GET_UPS_OUT_PWM_TRIG(x)               ((u32)(((x >> 21) & 0x00000001)))
#define LSYS_BIT_UPS_OUT_TIM_EN                    ((u32)0x00000001 << 20)          /*!<R/W 0  Enable for output en signal to basic timer */
#define LSYS_UPS_OUT_TIM_EN(x)                     ((u32)(((x) & 0x00000001) << 20))
#define LSYS_GET_UPS_OUT_TIM_EN(x)                 ((u32)(((x >> 20) & 0x00000001)))
#define LSYS_BIT_UPS_IN_TIM_OF                     ((u32)0x00000001 << 19)          /*!<R/W 0  Enable for basic timer overflow input */
#define LSYS_UPS_IN_TIM_OF(x)                      ((u32)(((x) & 0x00000001) << 19))
#define LSYS_GET_UPS_IN_TIM_OF(x)                  ((u32)(((x >> 19) & 0x00000001)))
#define LSYS_BIT_UPS_RVS_EN                        ((u32)0x00000001 << 18)          /*!<R/W 0  Invert polarity for UPS input 0: posedge polarity 1: negedge polarity */
#define LSYS_UPS_RVS_EN(x)                         ((u32)(((x) & 0x00000001) << 18))
#define LSYS_GET_UPS_RVS_EN(x)                     ((u32)(((x >> 18) & 0x00000001)))
#define LSYS_MASK_UPS_DBC_CNT                      ((u32)0x0000FFFF << 2)          /*!<R/W 0  Debounce count for UPS funciton */
#define LSYS_UPS_DBC_CNT(x)                        (((u32)((x) & 0x0000FFFF) << 2))
#define LSYS_GET_UPS_DBC_CNT(x)                    ((u32)(((x >> 2) & 0x0000FFFF)))
/** @} */

/** @defgroup REG_LSYS_SYSRST_MSK0
 * @brief
 * @{
 **/
#define LSYS_BIT_SYSRST_MSK_QSPI                   ((u32)0x00000001 << 31)          /*!<R/W 0  1: mask qspi reset when sys reset happen 0: disable */
#define LSYS_SYSRST_MSK_QSPI(x)                    ((u32)(((x) & 0x00000001) << 31))
#define LSYS_GET_SYSRST_MSK_QSPI(x)                ((u32)(((x >> 31) & 0x00000001)))
#define LSYS_BIT_SYSRST_MSK_IRDA                   ((u32)0x00000001 << 30)          /*!<R/W 0  1: mask irda reset when sys reset happen 0: disable */
#define LSYS_SYSRST_MSK_IRDA(x)                    ((u32)(((x) & 0x00000001) << 30))
#define LSYS_GET_SYSRST_MSK_IRDA(x)                ((u32)(((x >> 30) & 0x00000001)))
#define LSYS_MASK_SYSRST_MSK_HTIM                  ((u32)0x00000003 << 25)          /*!<R/W 0  1: mask HP TIMER14 reset when sys reset happen 0: disable */
#define LSYS_SYSRST_MSK_HTIM(x)                    (((u32)((x) & 0x00000003) << 25))
#define LSYS_GET_SYSRST_MSK_HTIM(x)                ((u32)(((x >> 25) & 0x00000003)))
#define LSYS_MASK_SYSRST_MSK_PWM                   ((u32)0x00000003 << 23)          /*!<R/W 0  1: mask HP TIMER_PWM reset when sys reset happen 0: disable */
#define LSYS_SYSRST_MSK_PWM(x)                     (((u32)((x) & 0x00000003) << 23))
#define LSYS_GET_SYSRST_MSK_PWM(x)                 ((u32)(((x >> 23) & 0x00000003)))
#define LSYS_BIT_SYSRST_MSK_SDIO                   ((u32)0x00000001 << 22)          /*!<R/W 0  1: mask sdio reset when sys reset happen 0: disable */
#define LSYS_SYSRST_MSK_SDIO(x)                    ((u32)(((x) & 0x00000001) << 22))
#define LSYS_GET_SYSRST_MSK_SDIO(x)                ((u32)(((x >> 22) & 0x00000001)))
#define LSYS_MASK_SYSRST_MSK_SPORT                 ((u32)0x00000003 << 20)          /*!<R/W 0  1: mask SPORT1 reset when sys reset happen 0: disable */
#define LSYS_SYSRST_MSK_SPORT(x)                   (((u32)((x) & 0x00000003) << 20))
#define LSYS_GET_SYSRST_MSK_SPORT(x)               ((u32)(((x >> 20) & 0x00000003)))
#define LSYS_BIT_SYSRST_MSK_AC                     ((u32)0x00000001 << 19)          /*!<R/W 0  1: mask audio codec active reset when sys reset happen 0: disable */
#define LSYS_SYSRST_MSK_AC(x)                      ((u32)(((x) & 0x00000001) << 19))
#define LSYS_GET_SYSRST_MSK_AC(x)                  ((u32)(((x >> 19) & 0x00000001)))
#define LSYS_BIT_SYSRST_MSK_TRNG                   ((u32)0x00000001 << 18)          /*!<R/W 0  1: mask TRNG reset when sys reset happen 0: disable */
#define LSYS_SYSRST_MSK_TRNG(x)                    ((u32)(((x) & 0x00000001) << 18))
#define LSYS_GET_SYSRST_MSK_TRNG(x)                ((u32)(((x >> 18) & 0x00000001)))
#define LSYS_BIT_SYSRST_MSK_LEDC                   ((u32)0x00000001 << 16)          /*!<R/W 0  1: mask LEDC reset when sys reset happen 0: disable */
#define LSYS_SYSRST_MSK_LEDC(x)                    ((u32)(((x) & 0x00000001) << 16))
#define LSYS_GET_SYSRST_MSK_LEDC(x)                ((u32)(((x >> 16) & 0x00000001)))
#define LSYS_MASK_SYSRST_MSK_SPI                   ((u32)0x00000003 << 14)          /*!<R/W 0  1: mask SPI1 reset when sys reset happen 0: disable */
#define LSYS_SYSRST_MSK_SPI(x)                     (((u32)((x) & 0x00000003) << 14))
#define LSYS_GET_SYSRST_MSK_SPI(x)                 ((u32)(((x >> 14) & 0x00000003)))
#define LSYS_BIT_SYSRST_MSK_PSRAM                  ((u32)0x00000001 << 13)          /*!<R/W 0  1: mask PSRAM reset when sys reset happen 0: disable */
#define LSYS_SYSRST_MSK_PSRAM(x)                   ((u32)(((x) & 0x00000001) << 13))
#define LSYS_GET_SYSRST_MSK_PSRAM(x)               ((u32)(((x >> 13) & 0x00000001)))
#define LSYS_BIT_SYSRST_MSK_KSCAN                  ((u32)0x00000001 << 11)          /*!<R/W 0  mask kscan when sys reset happen mask 1: mask 0: disable kscan */
#define LSYS_SYSRST_MSK_KSCAN(x)                   ((u32)(((x) & 0x00000001) << 11))
#define LSYS_GET_SYSRST_MSK_KSCAN(x)               ((u32)(((x >> 11) & 0x00000001)))
#define LSYS_BIT_SYSRST_MSK_SPIC                   ((u32)0x00000001 << 10)          /*!<R/W 0  mask Flash spic reset when sys reset happen mask 1: mask flash 0: disable flash */
#define LSYS_SYSRST_MSK_SPIC(x)                    ((u32)(((x) & 0x00000001) << 10))
#define LSYS_GET_SYSRST_MSK_SPIC(x)                ((u32)(((x >> 10) & 0x00000001)))
#define LSYS_BIT_SYSRST_MSK_SHA                    ((u32)0x00000001 << 9)          /*!<R/W 0  1: mask IPSEC SHA reset when sys reset happen 0: disable */
#define LSYS_SYSRST_MSK_SHA(x)                     ((u32)(((x) & 0x00000001) << 9))
#define LSYS_GET_SYSRST_MSK_SHA(x)                 ((u32)(((x >> 9) & 0x00000001)))
#define LSYS_BIT_SYSRST_MSK_AES                    ((u32)0x00000001 << 8)          /*!<R/W 0  1: mask IPSEC AES reset when sys reset happen 0: disable */
#define LSYS_SYSRST_MSK_AES(x)                     ((u32)(((x) & 0x00000001) << 8))
#define LSYS_GET_SYSRST_MSK_AES(x)                 ((u32)(((x >> 8) & 0x00000001)))
#define LSYS_BIT_SYSRST_MSK_LX                     ((u32)0x00000001 << 7)          /*!<R/W 0  1: mask LX reset when sys reset happen 0: disable */
#define LSYS_SYSRST_MSK_LX(x)                      ((u32)(((x) & 0x00000001) << 7))
#define LSYS_GET_SYSRST_MSK_LX(x)                  ((u32)(((x >> 7) & 0x00000001)))
#define LSYS_BIT_SYSRST_MSK_DMAC                   ((u32)0x00000001 << 6)          /*!<R/W 0  1: mask GDMA reset when sys reset happen 0: disable */
#define LSYS_SYSRST_MSK_DMAC(x)                    ((u32)(((x) & 0x00000001) << 6))
#define LSYS_GET_SYSRST_MSK_DMAC(x)                ((u32)(((x >> 6) & 0x00000001)))
#define LSYS_BIT_SYSRST_MSK_USB                    ((u32)0x00000001 << 4)          /*!<R/W 0  1: mask usb reset when sys reset happen 0: reset */
#define LSYS_SYSRST_MSK_USB(x)                     ((u32)(((x) & 0x00000001) << 4))
#define LSYS_GET_SYSRST_MSK_USB(x)                 ((u32)(((x >> 4) & 0x00000001)))
#define LSYS_BIT_SYSRST_MSK_KM4                    ((u32)0x00000001 << 3)          /*!<R/W 0  1: mask KM4 reset when sys reset happen 0: reset */
#define LSYS_SYSRST_MSK_KM4(x)                     ((u32)(((x) & 0x00000001) << 3))
#define LSYS_GET_SYSRST_MSK_KM4(x)                 ((u32)(((x >> 3) & 0x00000001)))
#define LSYS_BIT_SYSRST_MSK_KM0                    ((u32)0x00000001 << 2)          /*!<R/W 0  1: mask KM0 reset when sys reset happen 0: reset */
#define LSYS_SYSRST_MSK_KM0(x)                     ((u32)(((x) & 0x00000001) << 2))
#define LSYS_GET_SYSRST_MSK_KM0(x)                 ((u32)(((x >> 2) & 0x00000001)))
#define LSYS_BIT_SYSRST_MSK_PLFM                   ((u32)0x00000001 << 1)          /*!<R/W 0  1: mask platform 0: reset */
#define LSYS_SYSRST_MSK_PLFM(x)                    ((u32)(((x) & 0x00000001) << 1))
#define LSYS_GET_SYSRST_MSK_PLFM(x)                ((u32)(((x >> 1) & 0x00000001)))
#define LSYS_BIT_SYSRST_MSK_HSOC                   ((u32)0x00000001 << 0)          /*!<R/W 1  1: mask HP_SOC system reset when sys reset happen 0: reset */
#define LSYS_SYSRST_MSK_HSOC(x)                    ((u32)(((x) & 0x00000001) << 0))
#define LSYS_GET_SYSRST_MSK_HSOC(x)                ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_SYSRST_MSK1
 * @brief
 * @{
 **/
#define LSYS_MASK_SYSRST_MSK_PTIM                  ((u32)0x00000003 << 25)          /*!<R/W 0  1: mask pmc timer reset when sys reset happen 0: disable */
#define LSYS_SYSRST_MSK_PTIM(x)                    (((u32)((x) & 0x00000003) << 25))
#define LSYS_GET_SYSRST_MSK_PTIM(x)                ((u32)(((x >> 25) & 0x00000003)))
#define LSYS_BIT_SYSRST_MSK_ADC                    ((u32)0x00000001 << 23)          /*!<R/W 0  1: mask LS ADC reset when sys reset happen 0: reset */
#define LSYS_SYSRST_MSK_ADC(x)                     ((u32)(((x) & 0x00000001) << 23))
#define LSYS_GET_SYSRST_MSK_ADC(x)                 ((u32)(((x >> 23) & 0x00000001)))
#define LSYS_BIT_SYSRST_MSK_IPC                    ((u32)0x00000001 << 22)          /*!<R/W 0  1: mask IPC reset when sys reset happen 0: reset */
#define LSYS_SYSRST_MSK_IPC(x)                     ((u32)(((x) & 0x00000001) << 22))
#define LSYS_GET_SYSRST_MSK_IPC(x)                 ((u32)(((x >> 22) & 0x00000001)))
#define LSYS_BIT_SYSRST_MSK_WDG                    ((u32)0x00000001 << 21)          /*!<R/W 0  1: mask LS WDG 1~4 reset when sys reset happen 0: reset */
#define LSYS_SYSRST_MSK_WDG(x)                     ((u32)(((x) & 0x00000001) << 21))
#define LSYS_GET_SYSRST_MSK_WDG(x)                 ((u32)(((x >> 21) & 0x00000001)))
#define LSYS_BIT_SYSRST_MSK_DTIM                   ((u32)0x00000001 << 20)          /*!<R/W 0  1: mask LP dtim reset when sys reset happen mask 0: reset */
#define LSYS_SYSRST_MSK_DTIM(x)                    ((u32)(((x) & 0x00000001) << 20))
#define LSYS_GET_SYSRST_MSK_DTIM(x)                ((u32)(((x >> 20) & 0x00000001)))
#define LSYS_MASK_SYSRST_MSK_LTIM                  ((u32)0x000000FF << 12)          /*!<R/W 0  1: mask LP basic TIMER0-7 reset when sys reset happen 0: disable */
#define LSYS_SYSRST_MSK_LTIM(x)                    (((u32)((x) & 0x000000FF) << 12))
#define LSYS_GET_SYSRST_MSK_LTIM(x)                ((u32)(((x >> 12) & 0x000000FF)))
#define LSYS_MASK_SYSRST_MSK_I2C                   ((u32)0x00000003 << 10)          /*!<R/W 0  1: mask HS I2C1 reset when sys reset happen 0: reset */
#define LSYS_SYSRST_MSK_I2C(x)                     (((u32)((x) & 0x00000003) << 10))
#define LSYS_GET_SYSRST_MSK_I2C(x)                 ((u32)(((x >> 10) & 0x00000003)))
#define LSYS_MASK_SYSRST_MSK_UART                  ((u32)0x00000007 << 6)          /*!<R/W 0  1: mask UART reset when sys reset happen 0: disable */
#define LSYS_SYSRST_MSK_UART(x)                    (((u32)((x) & 0x00000007) << 6))
#define LSYS_GET_SYSRST_MSK_UART(x)                ((u32)(((x >> 6) & 0x00000007)))
#define LSYS_BIT_SYSRST_MSK_LOGUART                ((u32)0x00000001 << 5)          /*!<R/W 0  1: mask LOGUART reset when sys reset happen 0: disable */
#define LSYS_SYSRST_MSK_LOGUART(x)                 ((u32)(((x) & 0x00000001) << 5))
#define LSYS_GET_SYSRST_MSK_LOGUART(x)             ((u32)(((x >> 5) & 0x00000001)))
#define LSYS_BIT_SYSRST_MSK_GPIO                   ((u32)0x00000001 << 4)          /*!<R/W 0  1: mask LS GPIO reset when sys reset happen 0: reset */
#define LSYS_SYSRST_MSK_GPIO(x)                    ((u32)(((x) & 0x00000001) << 4))
#define LSYS_GET_SYSRST_MSK_GPIO(x)                ((u32)(((x >> 4) & 0x00000001)))
#define LSYS_BIT_SYSRST_MSK_WLAFE_CTRL             ((u32)0x00000001 << 2)          /*!<R/W 0  1: mask WL RFAFE control circuit, shall be 1 when mask WL or BT use WL RFAFE 0: reset */
#define LSYS_SYSRST_MSK_WLAFE_CTRL(x)              ((u32)(((x) & 0x00000001) << 2))
#define LSYS_GET_SYSRST_MSK_WLAFE_CTRL(x)          ((u32)(((x >> 2) & 0x00000001)))
#define LSYS_BIT_SYSRST_MSK_BTON                   ((u32)0x00000001 << 1)          /*!<R/W 0  mask BTON reset when sys reset happen 1: mask 0: disable */
#define LSYS_SYSRST_MSK_BTON(x)                    ((u32)(((x) & 0x00000001) << 1))
#define LSYS_GET_SYSRST_MSK_BTON(x)                ((u32)(((x >> 1) & 0x00000001)))
#define LSYS_BIT_SYSRST_MSK_WLON                   ((u32)0x00000001 << 0)          /*!<R/W 0  mask WLON reset when sys reset happen mask 1: mask 0: disable */
#define LSYS_SYSRST_MSK_WLON(x)                    ((u32)(((x) & 0x00000001) << 0))
#define LSYS_GET_SYSRST_MSK_WLON(x)                ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_DUMMY_AIP_CTRL1
 * @brief
 * @{
 **/
#define LSYS_MASK_AIP_CTRL_RSVD1                   ((u32)0x000000FF << 0)          /*!<R/W 8'h0F  reserved for aip control */
#define LSYS_AIP_CTRL_RSVD1(x)                     (((u32)((x) & 0x000000FF) << 0))
#define LSYS_GET_AIP_CTRL_RSVD1(x)                 ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup REG_LSYS_DUMMY_AIP_INFO1
 * @brief
 * @{
 **/
#define LSYS_MASK_AIP_INFO_RSVD1                   ((u32)0x000000FF << 0)          /*!<R 0  reserved for aip info indication */
#define LSYS_AIP_INFO_RSVD1(x)                     (((u32)((x) & 0x000000FF) << 0))
#define LSYS_GET_AIP_INFO_RSVD1(x)                 ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/*==========LSYS Register Address Definition==========*/
#define REG_LSYS_PWC                                 0x0200
#define REG_LSYS_ISO                                 0x0202
#define REG_LSYS_FEN_GRP0                            0x0204
#define REG_LSYS_FEN_GRP1                            0x0208
#define REG_LSYS_CKE_GRP0                            0x020C
#define REG_LSYS_CKE_GRP1                            0x0210
#define REG_LSYS_CKE_GRP2                            0x0214
#define REG_LSYS_CKSL_GRP0                           0x0218
#define REG_LSYS_CKSL_GRP1                           0x021C
#define REG_LSYS_CKD_GRP0                            0x0220
#define REG_LSYS_CKD_GRP1                            0x0224
#define REG_LSYS_SYSTEM_CFG0                         0x0228
#define REG_LSYS_SYSTEM_CFG1                         0x022C
#define REG_LSYS_PLAT_CTRL                           0x0230
#define REG_LSYS_PLAT_STATUS                         0x0234
#define REG_LSYS_PLAT_INFO                           0x0238
#define REG_LSYS_USB_SDIO_CTRL                       0x023C
#define REG_LSYS_KM4_XO_CTRL0                        0x0240
#define REG_LSYS_KM4_XO_CTRL1                        0x0244
#define REG_LSYS_HIFI_CTRL0                          0x0248
#define REG_LSYS_PSRAMC_FLASH_CTRL                   0x024C
#define REG_LSYS_I2S0_COMP_STEP                      0x0250
#define REG_LSYS_I2S1_COMP_STEP                      0x0254
#define REG_LSYS_I2S_SDM_CTRL                        0x0258
#define REG_LSYS_SPIC_CTRL                           0x025C
#define REG_LSYS_SW_RST_CTRL                         0x0260
#define REG_LSYS_SW_RST_TRIG                         0x0264
#define REG_LSYS_BOOT_REASON_SW                      0x0268
#define REG_LSYS_BOOT_CFG                            0x026C
#define REG_LSYS_SW_PATCH                            0x0270
#define REG_LSYS_CHIP_INFO                           0x0274
#define REG_LSYS_BT_32K_CTRL0                        0x0278
#define REG_LSYS_BT_32K_CTRL1                        0x027C
#define REG_LSYS_BT_CTRL0                            0x0280
#define REG_LSYS_BT_CTRL1                            0x0284
#define REG_LSYS_AIP_CTRL0                           0x0288
#define REG_LSYS_MON_CTRL                            0x0290
#define REG_LSYS_MON_WREG0                           0x0294
#define REG_LSYS_MON_WREG1                           0x0298
#define REG_LSYS_DW_GPIO_CTRL                        0x02A4
#define REG_LSYS_LXBUS_DBG0                          0x02A8
#define REG_LSYS_LXBUS_DBG1                          0x02AC
#define REG_LSYS_LXBUS_DBG_CTRL                      0x02B0
#define REG_LSYS_FPGA_CLK_INFO                       0x02B4
#define REG_LSYS_DSS_CTRL                            0x02B8
#define REG_LSYS_DSS_OUT                             0x02BC
#define REG_LSYS_UPS_CTRL                            0x02C4
#define REG_LSYS_ECO_CTRL                            0x02CC
#define REG_LSYS_SYSRST_MSK0                         0x02D0
#define REG_LSYS_SYSRST_MSK1                         0x02D4
#define REG_LSYS_FLASH_PARA_ADDR                     0x02F0
#define REG_LSYS_DUMMY_090                           0x02F4
#define REG_LSYS_DUMMY_AIP_CTRL1                     0x02F8
#define REG_LSYS_DUMMY_AIP_INFO1                     0x02FC

/** @} */

/* AUTO_GEN_END */

/* MANUAL_GEN_START */
//Please add your defination here

enum CHIP_TYPE {
	CHIP_TYPE_ASIC_POSTSIM = 0,
	CHIP_TYPE_FPGA = 1,
	CHIP_TYPE_PALADIUM = 2,
	CHIP_TYPE_RTLSIM_PRESIM = 3
};

enum CORE_Vol_TYPE {
	CORE_VOL_0P9 = 0,
	CORE_VOL_1P0 = 1,
};

/*******************  Macro definition for Port M0 Master ID ********************/
#define SYSM0_MASTER_SIC                                       0
#define SYSM0_MASTER_OTPC                                    1
#define SYSM0_MASTER_KM0                                      2

/*****************************Macro definition for OTP SYSCFG1**************/
#define LSYS_BIT_DIS_BOOT_DELAY       ((u32)0x00000001 << 0)

/*****************************Macro definition for SHARE MEMORY**************/
#define SHARE_MEM_BT_E0_ADDRESS       			0x20080000
#define SHARE_MEM_BT_E1_ADDRESS       			0x20090000
#define SHARE_MEM_WIFI_TXPKTBUF_ADDRESS			0x200C0000
#define SHARE_MEM_WIFI_RXPKTBUF_ADDRESS			0x200C6000


/**************************************************************************//**
 * @defgroup AMEBA_RRAM
 * @{
 * @brief AMEBA_RRAM Declaration
 * @ Total size of retention memory is 512Byte,Half retention mem resvd for system
 * @ the Max space for RRAM_TypeDef is 0x80, user can alloc space from RRAM_USER_RSVD
 *****************************************************************************/
typedef struct {
	u8 RRAM_SYS_RSVD[256];/* resvd for system */
	u8 AP_WAKEUP_STATUS;
	u32 Memory_Type;
	u8 CORE_Vol_Flag;
	u8 SWRCALIB_AON_LDO_09;
	u8 SWRCALIB_PWM_135;
	u8 SWRCALIB_CORE_LDO_10;
	u8 SWRCALIB_PWM_10;
	__IO u32 psram_backup[6];

	u8 RRAM_USER_RSVD[222];
} RRAM_TypeDef;

Compile_Assert(sizeof(RRAM_TypeDef) != 512, "RRAM_TypeDef size shall be 512");
/** @} */

/* MANUAL_GEN_END */

#endif


/** @} */

/** @} */
