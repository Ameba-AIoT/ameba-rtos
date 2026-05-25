/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SYSREG_LSYS_H_
#define _SYSREG_LSYS_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup LSYS
  * @brief LSYS driver modules
  * @{
  */


/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup LSYS_Register_Definitions LSYS Register Definitions
  * @{
  */

/** @defgroup REG_LSYS_PWC
  * @brief
  * @{
  */
#define LSYS_MASK_PSW_OUT_SOC   ((u16)0x00000003 << 8)           /*!< R 0x0  SOC domain power switch status 1: power on 0: power off Note:there are two group*/
#define LSYS_PSW_OUT_SOC(x)     (((u16)((x) & 0x00000003) << 8))
#define LSYS_GET_PSW_OUT_SOC(x) ((u16)(((x >> 8) & 0x00000003)))
#define LSYS_BIT_PWC_SOC_REQ    ((u16)0x00000001 << 7)           /*!< R 0x0  pwc soc req arb result*/
#define LSYS_PWC_SOC_REQ(x)     ((u16)(((x) & 0x00000001) << 7))
#define LSYS_GET_PWC_SOC_REQ(x) ((u16)(((x >> 7) & 0x00000001)))
#define LSYS_MASK_PWC_SOC_ST    ((u16)0x00000007 << 4)           /*!< R 0x0  pwc soc arb result state*/
#define LSYS_PWC_SOC_ST(x)      (((u16)((x) & 0x00000007) << 4))
#define LSYS_GET_PWC_SOC_ST(x)  ((u16)(((x >> 4) & 0x00000007)))
#define LSYS_BIT_POW_ON_SOC     ((u16)0x00000001 << 0)           /*!< R/W 0x0  1:SYS PMC request to enable HP SOC power cut and diable ISO 0: SYS PMC request to disable HP SOC power cut and enable ISO DD_SEC: S*/
#define LSYS_POW_ON_SOC(x)      ((u16)(((x) & 0x00000001) << 0))
#define LSYS_GET_POW_ON_SOC(x)  ((u16)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_ISO
  * @brief
  * @{
  */
#define LSYS_BIT_FEN_SOC        ((u16)0x00000001 << 8)           /*!< R/W 0x0  1: enable hs soc sytem 0: disable hs soc system  (Note: used as soc power on reset) DD_SEC: S*/
#define LSYS_FEN_SOC(x)         ((u16)(((x) & 0x00000001) << 8))
#define LSYS_GET_FEN_SOC(x)     ((u16)(((x >> 8) & 0x00000001)))
#define LSYS_BIT_ISO_SOC_REQ    ((u16)0x00000001 << 0)           /*!< R 0x0  isolate signals from PD_SOC to other power domain. It is arb result*/
#define LSYS_ISO_SOC_REQ(x)     ((u16)(((x) & 0x00000001) << 0))
#define LSYS_GET_ISO_SOC_REQ(x) ((u16)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_FEN_GRP0
  * @brief
  * @{
  */
#define APBPeriph_IRDA   ((u32)(0 << 30) | (0x00000001<< 29)) /*!< R/W 0x0  1: enable irda module 0: reset No use DD_SEC: bpc_irda*/
#define APBPeriph_PKE    ((u32)(0 << 30) | (0x00000001<< 28)) /*!< R/W 0x0  1: enable pke module 0: reset No use DD_SEC: bpc_pke*/
#define APBPeriph_PWM0   ((u32)(0 << 30) | (0x00000001<< 23)) /*!< R/W 0x0  1: enable PWM TIMER function 0: disable [2:1]:pwm [0]:pulse DD_SEC: bpc_pwm*/
#define APBPeriph_PWM1   ((u32)(0 << 30) | (0x00000001<< 24)) /*!< R/W 0x0  1: enable PWM TIMER function 0: disable [2:1]:pwm [0]:pulse DD_SEC: bpc_pwm*/
#define APBPeriph_PWM2   ((u32)(0 << 30) | (0x00000001<< 25)) /*!< R/W 0x0  1: enable PWM TIMER function 0: disable [2:1]:pwm [0]:pulse DD_SEC: bpc_pwm*/
#define APBPeriph_SDD    ((u32)(0 << 30) | (0x00000001<< 22)) /*!< R/W 0x1  1: enable sdio device function 0: disable DD_SEC: bpc_sdio*/
#define APBPeriph_SPORT  ((u32)(0 << 30) | (0x00000001<< 20)) /*!< R/W 0x0  1: enable SPORT function 0: disable DD_SEC: bpc_audio*/
#define APBPeriph_GMAC   ((u32)(0 << 30) | (0x00000001<< 19)) /*!< R/W 0x0  1: enable ppe module 0: reset No use DD_SEC: bpc_gmac*/
#define APBPeriph_TRNG   ((u32)(0 << 30) | (0x00000001<< 18)) /*!< R/W 0x0  1: enable TRNG function 0: disable DD_SEC: bpc_trng*/
#define APBPeriph_SPI0   ((u32)(0 << 30) | (0x00000001<< 14)) /*!< R/W 0x0  1: enable SPI1 function 0: disable DD_SEC: bpc_spi*/
#define APBPeriph_SPI1   ((u32)(0 << 30) | (0x00000001<< 15)) /*!< R/W 0x0  1: enable SPI1 function 0: disable DD_SEC: bpc_spi*/
#define APBPeriph_PSRAM  ((u32)(0 << 30) | (0x00000001<< 13)) /*!< R/W 0x0  1: enable PSRAM function 0: disable DD_SEC: bpc_psram*/
#define APBPeriph_FLASH  ((u32)(0 << 30) | (0x00000001<< 10)) /*!< R/W 0x0  Flash spic function enable 1: enable flash 0: disable flash DD_SEC: bpc_spic*/
#define APBPeriph_CRYPTO ((u32)(0 << 30) | (0x00000001<< 8))  /*!< R/W 0x0  1: enable IPSEC (AES + HMAC) function 0: disable DD_SEC: bpc_crypto*/
#define APBPeriph_DMAC   ((u32)(0 << 30) | (0x00000001<< 6))  /*!< R/W 0x0  1: enable GDMA function 0: disable DD_SEC: bpc_dmac*/
#define APBPeriph_IPC0   ((u32)(0 << 30) | (0x00000001<< 4))  /*!< R/W 0x0  1: enable IPC function 0: reset DD_SEC: bpc_ipc*/
#define APBPeriph_IPC1   ((u32)(0 << 30) | (0x00000001<< 5))  /*!< R/W 0x0  1: enable IPC function 0: reset DD_SEC: bpc_ipc*/
#define APBPeriph_CPU0   ((u32)(0 << 30) | (0x00000001<< 3))  /*!< R/W 0x0  1: enable cpu0 (KM4TZ) 0: reset DD_SEC: bpc_cpu0*/
#define APBPeriph_CPU1   ((u32)(0 << 30) | (0x00000001<< 2))  /*!< R/W 0x0  1: enable cpu1 (KM4NS) 0: reset DD_SEC: bpc_cpu1*/
#define APBPeriph_PLFM1  ((u32)(0 << 30) | (0x00000001<< 1))  /*!< R/W 0x0  1: enable platform1 0: reset DD_SEC: S*/
#define APBPeriph_PLFM0  ((u32)(0 << 30) | (0x00000001<< 0))  /*!< R/W 0x0  1: enable platform0 0: reset DD_SEC: S*/
/** @} */

/** @defgroup REG_LSYS_FEN_GRP1
  * @brief
  * @{
  */
#define APBPeriph_PTIM0   ((u32)(1 << 30) | (0x00000001<< 25)) /*!< R/W 0x0  pmc_timer function enable 1: enable 0: disable DD_SEC: bpc_ptim*/
#define APBPeriph_PTIM1   ((u32)(1 << 30) | (0x00000001<< 26)) /*!< R/W 0x0  pmc_timer function enable 1: enable 0: disable DD_SEC: bpc_ptim*/
#define APBPeriph_ADC     ((u32)(1 << 30) | (0x00000001<< 23)) /*!< R/W 0x1  lp adc  function enable 1: enable 0: disable DD_SEC: bpc_adc*/
#define APBPeriph_DTIM    ((u32)(1 << 30) | (0x00000001<< 20)) /*!< R/W 0x1  Debug timer function enable 1: enable 0: disable DD_SEC: bpc_dtim*/
#define APBPeriph_LTIM0   ((u32)(1 << 30) | (0x00000001<< 12)) /*!< R/W 4'h0  Basic Timer0-3 function enable for plfm0 1: enable 0: disable DD_SEC: bpc_ltim*/
#define APBPeriph_LTIM1   ((u32)(1 << 30) | (0x00000001<< 13)) /*!< R/W 4'h0  Basic Timer0-3 function enable for plfm0 1: enable 0: disable DD_SEC: bpc_ltim*/
#define APBPeriph_LTIM2   ((u32)(1 << 30) | (0x00000001<< 14)) /*!< R/W 4'h0  Basic Timer0-3 function enable for plfm0 1: enable 0: disable DD_SEC: bpc_ltim*/
#define APBPeriph_LTIM3   ((u32)(1 << 30) | (0x00000001<< 15)) /*!< R/W 4'h0  Basic Timer0-3 function enable for plfm0 1: enable 0: disable DD_SEC: bpc_ltim*/
#define APBPeriph_I2C0    ((u32)(1 << 30) | (0x00000001<< 10)) /*!< R/W 0x0  I2C1  function enable 1: enable 0: disable DD_SEC: bpc_i2c*/
#define APBPeriph_I2C1    ((u32)(1 << 30) | (0x00000001<< 11)) /*!< R/W 0x0  I2C1  function enable 1: enable 0: disable DD_SEC: bpc_i2c*/
#define APBPeriph_UART0   ((u32)(1 << 30) | (0x00000001<< 6))  /*!< R/W 0x0  Uart function enable  1: enable 0: disable DD_SEC: bpc_uart*/
#define APBPeriph_UART1   ((u32)(1 << 30) | (0x00000001<< 7))  /*!< R/W 0x0  Uart function enable  1: enable 0: disable DD_SEC: bpc_uart*/
#define APBPeriph_UART2   ((u32)(1 << 30) | (0x00000001<< 8))  /*!< R/W 0x0  Uart function enable  1: enable 0: disable DD_SEC: bpc_uart*/
#define APBPeriph_LOGUART ((u32)(1 << 30) | (0x00000001<< 5))  /*!< R/W 0x0  Uartlog function enable 1: enable 0: disable DD_SEC: bpc_loguart*/
#define APBPeriph_GPIO    ((u32)(1 << 30) | (0x00000001<< 4))  /*!< R/W 0x0  SGPIO  function enable 1: enable 0: disable DD_SEC: bpc_gpio*/
#define APBPeriph_SIC     ((u32)(1 << 30) | (0x00000001<< 3))  /*!< R/W 0x0  1: enable SIC function 0: reset DD_SEC: S*/
#define APBPeriph_WLAFE   ((u32)(1 << 30) | (0x00000001<< 2))  /*!< R/W 0x0  WL RFAFE control function enable 1: enable 0: disable DD_SEC: bpc_wl*/
#define APBPeriph_BTON    ((u32)(1 << 30) | (0x00000001<< 1))  /*!< R/W 0x0  BTON function enable 1: enable 0: disable DD_SEC: bpc_bt*/
#define APBPeriph_WLON    ((u32)(1 << 30) | (0x00000001<< 0))  /*!< R/W 0x0  WLON  function enable 1: enable 0: disable DD_SEC: bpc_wl*/
/** @} */

/** @defgroup REG_LSYS_CKE_GRP0
  * @brief
  * @{
  */
#define APBPeriph_PKE_CLOCK    ((u32)(0 << 30) | (0x00000001<< 29)) /*!< R/W 0x1  1: enable pke clk 0: reset No use DD_SEC: bpc_pke*/
#define APBPeriph_PWM0_CLOCK   ((u32)(0 << 30) | (0x00000001<< 24)) /*!< R/W 0x0  1: Enable PWM TIMER clock 0: Disable [2:1] : pwm timer [0] : pulse timer DD_SEC: bpc_pwm*/
#define APBPeriph_PWM1_CLOCK   ((u32)(0 << 30) | (0x00000001<< 25)) /*!< R/W 0x0  1: Enable PWM TIMER clock 0: Disable [2:1] : pwm timer [0] : pulse timer DD_SEC: bpc_pwm*/
#define APBPeriph_PWM2_CLOCK   ((u32)(0 << 30) | (0x00000001<< 26)) /*!< R/W 0x0  1: Enable PWM TIMER clock 0: Disable [2:1] : pwm timer [0] : pulse timer DD_SEC: bpc_pwm*/
#define APBPeriph_SDD_CLOCK    ((u32)(0 << 30) | (0x00000001<< 22)) /*!< R/W 0x1  1: Enable sdio dev clock 0: Disable DD_SEC: bpc_sdio*/
#define APBPeriph_SPORT_CLOCK  ((u32)(0 << 30) | (0x00000001<< 20)) /*!< R/W 0x0  1: Enable sport clock 0: Disable DD_SEC: bpc_audio*/
#define APBPeriph_TRNG_CLOCK   ((u32)(0 << 30) | (0x00000001<< 18)) /*!< R/W 0x0  1: Enable trng bus clock 0: Disable DD_SEC: bpc_trng*/
#define APBPeriph_SPI0_CLOCK   ((u32)(0 << 30) | (0x00000001<< 14)) /*!< R/W 0x0  1: Enable spi clock 0: Disable DD_SEC: bpc_spi*/
#define APBPeriph_SPI1_CLOCK   ((u32)(0 << 30) | (0x00000001<< 15)) /*!< R/W 0x0  1: Enable spi clock 0: Disable DD_SEC: bpc_spi*/
#define APBPeriph_PSRAM_CLOCK  ((u32)(0 << 30) | (0x00000001<< 13)) /*!< R/W 0x0  1: Enable PSRAM pll divider clock 0: Disable DD_SEC: bpc_psram*/
#define APBPeriph_SRAM_CLOCK   ((u32)(0 << 30) | (0x00000001<< 12)) /*!< R/W 0x1  1: Enable sram clock 0: Disable DD_SEC: S*/
#define APBPeriph_FLASH_CLOCK  ((u32)(0 << 30) | (0x00000001<< 10)) /*!< R/W 0x0  1: Enable FLASH pll divider clock 0: Disable DD_SEC: bpc_spic*/
#define APBPeriph_CRYPTO_CLOCK ((u32)(0 << 30) | (0x00000001<< 8))  /*!< R/W 0x0  1: Enable ipsec (AES + HMAC) clock 0: Disable DD_SEC: bpc_crypto*/
#define APBPeriph_OTPC_CLOCK   ((u32)(0 << 30) | (0x00000001<< 7))  /*!< R/W 0x0  1: Enable OTPC clock 0: Disable DD_SEC: bpc_otpc*/
#define APBPeriph_DMAC_CLOCK   ((u32)(0 << 30) | (0x00000001<< 6))  /*!< R/W 0x0  1: Enable GDMA0 clock 0: Disable DD_SEC: bpc_dmac*/
#define APBPeriph_IPC_CLOCK    ((u32)(0 << 30) | (0x00000001<< 5))  /*!< R/W 0x1  1: Enable IPC clock 0: Disable DD_SEC: bpc_ipc*/
#define APBPeriph_CPU0_CLOCK   ((u32)(0 << 30) | (0x00000001<< 4))  /*!< R/W 0x0  1: Enable hp KM4 cpu0 clock 0: Disable DD_SEC: bpc_cpu0*/
#define APBPeriph_CPU1_CLOCK   ((u32)(0 << 30) | (0x00000001<< 3))  /*!< R/W 0x0  1: Enable lp  KM4 cpu1 clock 0: Disable DD_SEC: bpc_cpu1*/
#define APBPeriph_HPERI_CLOCK  ((u32)(0 << 30) | (0x00000001<< 2))  /*!< R/W 0x1  1: Enable HPERI pll divider clock 0: Disable DD_SEC: S*/
#define APBPeriph_LPERI_CLOCK  ((u32)(0 << 30) | (0x00000001<< 1))  /*!< R/W 0x1  1: Enable peri_lclk  clock all 0: Disable DD_SEC: S*/
#define APBPeriph_LPON_CLOCK   ((u32)(0 << 30) | (0x00000001<< 0))  /*!< R/W 0x0  1.Enable lpon clk 0: Disable Note: this bit is controlled HW PMC, SW shall not modify DD_SEC: S*/
/** @} */

/** @defgroup REG_LSYS_CKE_GRP1
  * @brief
  * @{
  */
#define APBPeriph_IRDA_CLOCK    ((u32)(1 << 30) | (0x00000001<< 28)) /*!< R/W 0x0  1: Enable irda clock 0: Disable DD_SEC: bpc_irda*/
#define APBPeriph_WMAC_CLOCK    ((u32)(1 << 30) | (0x00000001<< 27)) /*!< R/W 0x0  1: Enable WMAC AXI and AHB bus clock 0: Disable DD_SEC: bpc_wl*/
#define APBPeriph_PTIM0_CLOCK   ((u32)(1 << 30) | (0x00000001<< 25)) /*!< R/W 2'b11  1: Enable pmc timer clock 0: Disable DD_SEC: bpc_ptim*/
#define APBPeriph_PTIM1_CLOCK   ((u32)(1 << 30) | (0x00000001<< 26)) /*!< R/W 2'b11  1: Enable pmc timer clock 0: Disable DD_SEC: bpc_ptim*/
#define APBPeriph_ADC_CLOCK     ((u32)(1 << 30) | (0x00000001<< 24)) /*!< R/W 0x1  1: Enable LS ADC clock 0: Disable DD_SEC: bpc_adc*/
#define APBPeriph_DTIM_CLOCK    ((u32)(1 << 30) | (0x00000001<< 21)) /*!< R/W 0x1  1: Enable LS dtim clk 0: Disable DD_SEC: bpc_dtim*/
#define APBPeriph_LTIM0_CLOCK   ((u32)(1 << 30) | (0x00000001<< 12)) /*!< R/W 0x0  LS basic Timer0-3 clk enable 0: Disable DD_SEC: bpc_ltim*/
#define APBPeriph_LTIM1_CLOCK   ((u32)(1 << 30) | (0x00000001<< 13)) /*!< R/W 0x0  LS basic Timer0-3 clk enable 0: Disable DD_SEC: bpc_ltim*/
#define APBPeriph_LTIM2_CLOCK   ((u32)(1 << 30) | (0x00000001<< 14)) /*!< R/W 0x0  LS basic Timer0-3 clk enable 0: Disable DD_SEC: bpc_ltim*/
#define APBPeriph_LTIM3_CLOCK   ((u32)(1 << 30) | (0x00000001<< 15)) /*!< R/W 0x0  LS basic Timer0-3 clk enable 0: Disable DD_SEC: bpc_ltim*/
#define APBPeriph_I2C0_CLOCK    ((u32)(1 << 30) | (0x00000001<< 10)) /*!< R/W 0x0  1: Enable HS I2C0 clock 0: Disable [1]: i2c1 [0]: i2c0 DD_SEC: bpc_i2c*/
#define APBPeriph_I2C1_CLOCK    ((u32)(1 << 30) | (0x00000001<< 11)) /*!< R/W 0x0  1: Enable HS I2C0 clock 0: Disable [1]: i2c1 [0]: i2c0 DD_SEC: bpc_i2c*/
#define APBPeriph_UART0_CLOCK   ((u32)(1 << 30) | (0x00000001<< 6))  /*!< R/W 0x0  1: Enable Uart clock  0: Disable DD_SEC: bpc_uart*/
#define APBPeriph_UART1_CLOCK   ((u32)(1 << 30) | (0x00000001<< 7))  /*!< R/W 0x0  1: Enable Uart clock  0: Disable DD_SEC: bpc_uart*/
#define APBPeriph_UART2_CLOCK   ((u32)(1 << 30) | (0x00000001<< 8))  /*!< R/W 0x0  1: Enable Uart clock  0: Disable DD_SEC: bpc_uart*/
#define APBPeriph_LOGUART_CLOCK ((u32)(1 << 30) | (0x00000001<< 5))  /*!< R/W 0x0  1: Enable Uartlog clock 0: Disable DD_SEC: bpc_loguart*/
#define APBPeriph_GPIO_CLOCK    ((u32)(1 << 30) | (0x00000001<< 4))  /*!< R/W 0x0  1: Enable GPIO clock 0: Disable DD_SEC: bpc_gpio*/
#define APBPeriph_SIC_CLOCK     ((u32)(1 << 30) | (0x00000001<< 3))  /*!< R/W 0x0  1: Enable SIC clock 0: Disable DD_SEC: S*/
#define APBPeriph_WDG_CLOCK     ((u32)(1 << 30) | (0x00000001<< 2))  /*!< R/W 0x1  1: Enable wdg bus clock 0: Disable DD_SEC: bpc_wdg*/
#define APBPeriph_BTON_CLOCK    ((u32)(1 << 30) | (0x00000001<< 1))  /*!< R/W 0x0  1: Enable BTON clock 0: Disable DD_SEC: bpc_bt*/
#define APBPeriph_WLON_CLOCK    ((u32)(1 << 30) | (0x00000001<< 0))  /*!< R/W 0x0  1: Enable WLON clock 0: Disable DD_SEC: bpc_wl*/
/** @} */

/** @defgroup REG_LSYS_CKE_GRP2
  * @brief
  * @{
  */
#define LSYS_BIT_CKE_BT_32K       ((u32)0x00000001 << 13)           /*!< R/W 1'b1  1.Enable bt 32k enable 0: Disable DD_SEC: bpc_bt*/
#define LSYS_CKE_BT_32K(x)        ((u32)(((x) & 0x00000001) << 13))
#define LSYS_GET_CKE_BT_32K(x)    ((u32)(((x >> 13) & 0x00000001)))
#define LSYS_BIT_CKE_SYS_32K      ((u32)0x00000001 << 12)           /*!< R/W 1'b1  1.Enable system/wifi 32k enable 0: Disable DD_SEC: S*/
#define LSYS_CKE_SYS_32K(x)       ((u32)(((x) & 0x00000001) << 12))
#define LSYS_GET_CKE_SYS_32K(x)   ((u32)(((x >> 12) & 0x00000001)))
#define LSYS_BIT_CKE_RET          ((u32)0x00000001 << 1)            /*!< R/W 1'b0  1.Enable some IP's clock who has retention function when pmc state change 0: Gating all retention IP's clock for retention operation needed Note: this bit is controlled HW PMC, SW shall not modify DD_SEC: S*/
#define LSYS_CKE_RET(x)           ((u32)(((x) & 0x00000001) << 1))
#define LSYS_GET_CKE_RET(x)       ((u32)(((x >> 1) & 0x00000001)))
#define LSYS_BIT_CKE_OSC2M_SOC    ((u32)0x00000001 << 0)            /*!< R/W 1'b0  1.Enable 2M clk path for soc clk 0: Disable Note: this bit is controlled HW PMC, SW shall not modify DD_SEC: S*/
#define LSYS_CKE_OSC2M_SOC(x)     ((u32)(((x) & 0x00000001) << 0))
#define LSYS_GET_CKE_OSC2M_SOC(x) ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_CKSL_GRP0
  * @brief  Name: SPORT / IRDA / PSRAM / SPIC / HPERI / OTPC / LPERI / HP
  * @{
  */
#define CKSL_SPORT_XTAL_40M 0x0
#define CKSL_SPORT_PLL_SDM_OUTPUT 0x1
#define CKSL_IRDA_XTAL 0x0
#define CKSL_IRDA_SYS_PLL 0x1
#define CKSL_IRDA_USB_PLL 0x3
#define CKSL_PSRAM_XTAL 0x0
#define CKSL_PSRAM_SYS_PLL 0x1
#define CKSL_PSRAM_USB_PLL 0x3
#define CKSL_SPIC_XTAL 0x0
#define CKSL_SPIC_SYS_PLL 0x1
#define CKSL_SPIC_USB_PLL 0x3
#define CKSL_HPERI_XTAL 0x0
#define CKSL_HPERI_SYS_PLL 0x1
#define CKSL_HPERI_USB_PLL 0x3
#define CKSL_OTPC_OSC4M 0x0
#define CKSL_OTPC_XTAL 0x1
#define CKSL_LPERI_OSC4M 0x0
#define CKSL_LPERI_XTAL_40M 0x1
#define CKSL_HP_XTAL 0x0
#define CKSL_HP_SYS_PLL 0x1
#define CKSL_HP_USB_PLL 0x3
#define LSYS_MASK_CKSL_SPORT   ((u32)0x00000001 << 18)           /*!< R/W 1'b0  i2s clock select ##0: xtal 40M ##1: pll sdm output DD_SEC: bpc_audio*/
#define LSYS_CKSL_SPORT(x)     ((u32)(((x) & 0x00000001) << 18))
#define REG_LSYS_CKSL_SPORT    REG_LSYS_CKSL_GRP0
#define LSYS_GET_CKSL_SPORT(x) ((u32)(((x >> 18) & 0x00000001)))
#define LSYS_MASK_CKSL_IRDA    ((u32)0x00000003 << 10)           /*!< R/W 2'b01  irda clock select ##x0: xtal ##01: sys_pll ##11: usb_pll DD_SEC: bpc_irda*/
#define LSYS_CKSL_IRDA(x)      (((u32)((x) & 0x00000003) << 10))
#define REG_LSYS_CKSL_IRDA     REG_LSYS_CKSL_GRP0
#define LSYS_GET_CKSL_IRDA(x)  ((u32)(((x >> 10) & 0x00000003)))
#define LSYS_MASK_CKSL_PSRAM   ((u32)0x00000003 << 8)            /*!< R/W 2'b01  psram clock select ##x0: xtal ##01: sys_pll ##11: usb_pll DD_SEC: bpc_psram*/
#define LSYS_CKSL_PSRAM(x)     (((u32)((x) & 0x00000003) << 8))
#define REG_LSYS_CKSL_PSRAM    REG_LSYS_CKSL_GRP0
#define LSYS_GET_CKSL_PSRAM(x) ((u32)(((x >> 8) & 0x00000003)))
#define LSYS_MASK_CKSL_SPIC    ((u32)0x00000003 << 6)            /*!< R/W 2'b10  spic clock select ##x0: xtal ##01: sys_pll ##11: usb_pll DD_SEC: bpc_spic*/
#define LSYS_CKSL_SPIC(x)      (((u32)((x) & 0x00000003) << 6))
#define REG_LSYS_CKSL_SPIC     REG_LSYS_CKSL_GRP0
#define LSYS_GET_CKSL_SPIC(x)  ((u32)(((x >> 6) & 0x00000003)))
#define LSYS_MASK_CKSL_HPERI   ((u32)0x00000003 << 4)            /*!< R/W 2'b10  hperi clock seletion ##x0: xtal ##01: sys_pll ##11: usb_pll DD_SEC: bpc_cpu0*/
#define LSYS_CKSL_HPERI(x)     (((u32)((x) & 0x00000003) << 4))
#define REG_LSYS_CKSL_HPERI    REG_LSYS_CKSL_GRP0
#define LSYS_GET_CKSL_HPERI(x) ((u32)(((x >> 4) & 0x00000003)))
#define LSYS_MASK_CKSL_OTPC    ((u32)0x00000001 << 3)            /*!< R/W 0h  otpc clock selection ##0: osc4m ##1: xtal Note: this bit will be set by pmc auto, when SW use otp,  it will be set to 1 DD_SEC: bpc_otpc*/
#define LSYS_CKSL_OTPC(x)      ((u32)(((x) & 0x00000001) << 3))
#define REG_LSYS_CKSL_OTPC     REG_LSYS_CKSL_GRP0
#define LSYS_GET_CKSL_OTPC(x)  ((u32)(((x >> 3) & 0x00000001)))
#define LSYS_MASK_CKSL_LPERI   ((u32)0x00000001 << 2)            /*!< R/W 0h  LPERI clock select ##0: osc4m ##1: xtal_40m DD_SEC: bpc_cpu0*/
#define LSYS_CKSL_LPERI(x)     ((u32)(((x) & 0x00000001) << 2))
#define REG_LSYS_CKSL_LPERI    REG_LSYS_CKSL_GRP0
#define LSYS_GET_CKSL_LPERI(x) ((u32)(((x >> 2) & 0x00000001)))
#define LSYS_MASK_CKSL_HP      ((u32)0x00000003 << 0)            /*!< R/W 2'b10  HP clock select,used for KM4 and sram ##x0: xtal ##01: sys_pll ##11: usb_pll DD_SEC: bpc_cpu0*/
#define LSYS_CKSL_HP(x)        (((u32)((x) & 0x00000003) << 0))
#define REG_LSYS_CKSL_HP       REG_LSYS_CKSL_GRP0
#define LSYS_GET_CKSL_HP(x)    ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup REG_LSYS_CKSL_GRP1
  * @brief  Name: LTIM5 / LTIM4 / LTIM3 / LTIM2 / LTIM1 / LTIM0 / BT_32K / SYS_32K / LOGUART / UART2 / UART1 / UART0 / ADC4M / DTIM / ADC / GPIO / GPIO4M
  * @{
  */
#define CKSL_LTIM5_SDM32K 0x0
#define CKSL_LTIM5_XTAL 0x1
#define CKSL_LTIM4_SDM32K 0x0
#define CKSL_LTIM4_XTAL 0x1
#define CKSL_LTIM3_SDM32K 0x0
#define CKSL_LTIM3_XTAL 0x1
#define CKSL_LTIM2_SDM32K 0x0
#define CKSL_LTIM2_XTAL 0x1
#define CKSL_LTIM1_SDM32K 0x0
#define CKSL_LTIM1_XTAL 0x1
#define CKSL_LTIM0_SDM32K 0x0
#define CKSL_LTIM0_XTAL 0x1
#define CKSL_BT_32K_OSC131K 0x0
#define CKSL_BT_32K_XTAL312P5K 0x1
#define CKSL_SYS_32K_OSC131K 0x0
#define CKSL_SYS_32K_XTAL312P5K 0x1
#define CKSL_LOGUART_OSC4M 0x0
#define CKSL_LOGUART_XTAL 0x1
#define CKSL_UART2_OSC2M 0x0
#define CKSL_UART2_XTAL 0x1
#define CKSL_UART1_OSC2M 0x0
#define CKSL_UART1_XTAL 0x1
#define CKSL_UART0_OSC2M 0x0
#define CKSL_UART0_XTAL 0x1
#define CKSL_ADC4M_OSC4M 0x0
#define CKSL_ADC4M_XTAL 0x1
#define CKSL_DTIM_SDM32K 0x0
#define CKSL_DTIM_XTAL 0x1
#define CKSL_ADC_BUS_CLK 0x0
#define CKSL_ADC_IP_CLK 0x1
#define CKSL_GPIO_BUS_CLK 0x0
#define CKSL_GPIO_IP_CLK 0x1
#define CKSL_GPIO4M_SDM32K 0x0
#define CKSL_GPIO4M_OSC2M 0x1
#define LSYS_MASK_CKSL_LTIM5     ((u32)0x00000001 << 25)           /*!< R/W 0x0  LS timer 5 clock source select ##0: sdm32k ##1: xtal Note:using XTAL divider out , which can be 40M/N  (N=2~63) DD_SEC: bpc_ltim*/
#define LSYS_CKSL_LTIM5(x)       ((u32)(((x) & 0x00000001) << 25))
#define REG_LSYS_CKSL_LTIM5      REG_LSYS_CKSL_GRP1
#define LSYS_GET_CKSL_LTIM5(x)   ((u32)(((x >> 25) & 0x00000001)))
#define LSYS_MASK_CKSL_LTIM4     ((u32)0x00000001 << 24)           /*!< R/W 0x0  LS timer 4 clock source select ##0: sdm32k ##1: xtal Note:using XTAL divider out , which can be 40M/N  (N=2~63) DD_SEC: bpc_ltim*/
#define LSYS_CKSL_LTIM4(x)       ((u32)(((x) & 0x00000001) << 24))
#define REG_LSYS_CKSL_LTIM4      REG_LSYS_CKSL_GRP1
#define LSYS_GET_CKSL_LTIM4(x)   ((u32)(((x >> 24) & 0x00000001)))
#define LSYS_MASK_CKSL_LTIM3     ((u32)0x00000001 << 23)           /*!< R/W 0x0  LS timer 3 clock source select ##0: sdm32k ##1: xtal Note:using XTAL divider out , which can be 40M/N  (N=2~63) DD_SEC: bpc_ltim*/
#define LSYS_CKSL_LTIM3(x)       ((u32)(((x) & 0x00000001) << 23))
#define REG_LSYS_CKSL_LTIM3      REG_LSYS_CKSL_GRP1
#define LSYS_GET_CKSL_LTIM3(x)   ((u32)(((x >> 23) & 0x00000001)))
#define LSYS_MASK_CKSL_LTIM2     ((u32)0x00000001 << 22)           /*!< R/W 0x0  LS timer 2 clock source select ##0: sdm32k ##1: xtal Note:using XTAL divider out , which can be 40M/N  (N=2~63) DD_SEC: bpc_ltim*/
#define LSYS_CKSL_LTIM2(x)       ((u32)(((x) & 0x00000001) << 22))
#define REG_LSYS_CKSL_LTIM2      REG_LSYS_CKSL_GRP1
#define LSYS_GET_CKSL_LTIM2(x)   ((u32)(((x >> 22) & 0x00000001)))
#define LSYS_MASK_CKSL_LTIM1     ((u32)0x00000001 << 21)           /*!< R/W 0x0  LS timer 1 clock source select ##0: sdm32k ##1: xtal Note:using XTAL divider out , which can be 40M/N  (N=2~63) DD_SEC: bpc_ltim*/
#define LSYS_CKSL_LTIM1(x)       ((u32)(((x) & 0x00000001) << 21))
#define REG_LSYS_CKSL_LTIM1      REG_LSYS_CKSL_GRP1
#define LSYS_GET_CKSL_LTIM1(x)   ((u32)(((x >> 21) & 0x00000001)))
#define LSYS_MASK_CKSL_LTIM0     ((u32)0x00000001 << 20)           /*!< R/W 0x0  LS timer 0 clock source select ##0: sdm32k ##1: xtal Note:using XTAL divider out , which can be 40M/N  (N=2~63) DD_SEC: bpc_ltim*/
#define LSYS_CKSL_LTIM0(x)       ((u32)(((x) & 0x00000001) << 20))
#define REG_LSYS_CKSL_LTIM0      REG_LSYS_CKSL_GRP1
#define LSYS_GET_CKSL_LTIM0(x)   ((u32)(((x >> 20) & 0x00000001)))
#define LSYS_MASK_CKSL_BT_32K    ((u32)0x00000001 << 17)           /*!< R/W 0x1  BT 32k clk source selection ##0:  osc131k ##1:  xtal312p5k DD_SEC: bpc_bt*/
#define LSYS_CKSL_BT_32K(x)      ((u32)(((x) & 0x00000001) << 17))
#define REG_LSYS_CKSL_BT_32K     REG_LSYS_CKSL_GRP1
#define LSYS_GET_CKSL_BT_32K(x)  ((u32)(((x >> 17) & 0x00000001)))
#define LSYS_MASK_CKSL_SYS_32K   ((u32)0x00000001 << 16)           /*!< R/W 0x0  system/wifi 32k clk source selection ##0:  osc131k ##1:  xtal312p5k DD_SEC: bpc_cpu0*/
#define LSYS_CKSL_SYS_32K(x)     ((u32)(((x) & 0x00000001) << 16))
#define REG_LSYS_CKSL_SYS_32K    REG_LSYS_CKSL_GRP1
#define LSYS_GET_CKSL_SYS_32K(x) ((u32)(((x >> 16) & 0x00000001)))
#define LSYS_MASK_CKSL_LOGUART   ((u32)0x00000001 << 14)           /*!< R/W 0x1  Select loguart's input clock ##0: osc4m ##1: xtal DD_SEC: bpc_loguart*/
#define LSYS_CKSL_LOGUART(x)     ((u32)(((x) & 0x00000001) << 14))
#define REG_LSYS_CKSL_LOGUART    REG_LSYS_CKSL_GRP1
#define LSYS_GET_CKSL_LOGUART(x) ((u32)(((x >> 14) & 0x00000001)))
#define LSYS_MASK_CKSL_UART2     ((u32)0x00000001 << 12)           /*!< R/W 0x1  Select uart's input rx clock ##0: osc2m ##1: xtal Note:tx input clock is always xtal , and divided in uart IP DD_SEC: bpc_uart*/
#define LSYS_CKSL_UART2(x)       ((u32)(((x) & 0x00000001) << 12))
#define REG_LSYS_CKSL_UART2      REG_LSYS_CKSL_GRP1
#define LSYS_GET_CKSL_UART2(x)   ((u32)(((x >> 12) & 0x00000001)))
#define LSYS_MASK_CKSL_UART1     ((u32)0x00000001 << 11)           /*!< R/W 0x1  Select uart's input rx clock ##0: osc2m ##1: xtal Note:tx input clock is always xtal , and divided in uart IP DD_SEC: bpc_uart*/
#define LSYS_CKSL_UART1(x)       ((u32)(((x) & 0x00000001) << 11))
#define REG_LSYS_CKSL_UART1      REG_LSYS_CKSL_GRP1
#define LSYS_GET_CKSL_UART1(x)   ((u32)(((x >> 11) & 0x00000001)))
#define LSYS_MASK_CKSL_UART0     ((u32)0x00000001 << 10)           /*!< R/W 0x1  Select uart's input rx clock ##0: osc2m ##1: xtal Note:tx input clock is always xtal , and divided in uart IP DD_SEC: bpc_uart*/
#define LSYS_CKSL_UART0(x)       ((u32)(((x) & 0x00000001) << 10))
#define REG_LSYS_CKSL_UART0      REG_LSYS_CKSL_GRP1
#define LSYS_GET_CKSL_UART0(x)   ((u32)(((x >> 10) & 0x00000001)))
#define LSYS_MASK_CKSL_ADC4M     ((u32)0x00000001 << 9)            /*!< R/W 0x0  lp adc 4m clk selection ##0:  osc4m ##1:  xtal Note:when select xtal , it must set divider DD_SEC: bpc_adc*/
#define LSYS_CKSL_ADC4M(x)       ((u32)(((x) & 0x00000001) << 9))
#define REG_LSYS_CKSL_ADC4M      REG_LSYS_CKSL_GRP1
#define LSYS_GET_CKSL_ADC4M(x)   ((u32)(((x >> 9) & 0x00000001)))
#define LSYS_MASK_CKSL_DTIM      ((u32)0x00000001 << 8)            /*!< R/W 0x1  dtim clock selection  ##0: sdm32k ##1: xtal Note:when select to xtal, it will divided to 1M fixed DD_SEC: bpc_dtim*/
#define LSYS_CKSL_DTIM(x)        ((u32)(((x) & 0x00000001) << 8))
#define REG_LSYS_CKSL_DTIM       REG_LSYS_CKSL_GRP1
#define LSYS_GET_CKSL_DTIM(x)    ((u32)(((x >> 8) & 0x00000001)))
#define LSYS_MASK_CKSL_ADC       ((u32)0x00000001 << 6)            /*!< R/W 0x0  ADC INT clock selection ##0: bus_clk ##1: ip_clk DD_SEC: bpc_adc*/
#define LSYS_CKSL_ADC(x)         ((u32)(((x) & 0x00000001) << 6))
#define REG_LSYS_CKSL_ADC        REG_LSYS_CKSL_GRP1
#define LSYS_GET_CKSL_ADC(x)     ((u32)(((x >> 6) & 0x00000001)))
#define LSYS_MASK_CKSL_GPIO      ((u32)0x00000001 << 5)            /*!< R/W 0x0  GPIO INT clock selecton: ##0: bus_clk ##1: ip_clk DD_SEC: bpc_gpio*/
#define LSYS_CKSL_GPIO(x)        ((u32)(((x) & 0x00000001) << 5))
#define REG_LSYS_CKSL_GPIO       REG_LSYS_CKSL_GRP1
#define LSYS_GET_CKSL_GPIO(x)    ((u32)(((x >> 5) & 0x00000001)))
#define LSYS_MASK_CKSL_GPIO4M    ((u32)0x00000001 << 3)            /*!< R/W 0x0  gpio ip using 2m clk selection ##0:  sdm32k ##1:  osc2m DD_SEC: bpc_gpio*/
#define LSYS_CKSL_GPIO4M(x)      ((u32)(((x) & 0x00000001) << 3))
#define REG_LSYS_CKSL_GPIO4M     REG_LSYS_CKSL_GRP1
#define LSYS_GET_CKSL_GPIO4M(x)  ((u32)(((x >> 3) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_CKD_GRP0
  * @brief  Name: XTAL_LTIM / XTAL_ADC
  * @{
  */
#define LSYS_MASK_CKD_XTAL_LTIM   ((u32)0x0000003F << 16)           /*!< R/WPD 6'd39  LS timer xtal clock divider when cksl_ltim = 1 0: not support 1: div2 n: divider is n+1 DIV_MIN:1 DD_SEC: bpc_ltim*/
#define LSYS_CKD_XTAL_LTIM(x)     (((u32)((x) & 0x0000003F) << 16))
#define REG_LSYS_CKD_XTAL_LTIM    REG_LSYS_CKD_GRP0
#define LSYS_GET_CKD_XTAL_LTIM(x) ((u32)(((x >> 16) & 0x0000003F)))
#define LSYS_MASK_CKD_XTAL_ADC    ((u32)0x0000000F << 8)            /*!< R/WPD 0h  xtal 4m divider for adc  Default is /1 which output 40M DD_SEC: bpc_adc*/
#define LSYS_CKD_XTAL_ADC(x)      (((u32)((x) & 0x0000000F) << 8))
#define REG_LSYS_CKD_XTAL_ADC     REG_LSYS_CKD_GRP0
#define LSYS_GET_CKD_XTAL_ADC(x)  ((u32)(((x >> 8) & 0x0000000F)))
/** @} */

/** @defgroup REG_LSYS_CKD_TIMER_GRP1
  * @brief  Name: XTAL_DTIM
  * @{
  */
#define LSYS_MASK_CKD_XTAL_DTIM   ((u32)0x0000003F << 16)           /*!< R/WPD 6'd39  xtal 1m divider for dtim  Default is /40 which output 1M DD_SEC: bpc_dtim*/
#define LSYS_CKD_XTAL_DTIM(x)     (((u32)((x) & 0x0000003F) << 16))
#define REG_LSYS_CKD_XTAL_DTIM    REG_LSYS_CKD_TIMER_GRP1
#define LSYS_GET_CKD_XTAL_DTIM(x) ((u32)(((x >> 16) & 0x0000003F)))
/** @} */

/** @defgroup REG_LSYS_CKD_FEN_XTAL
  * @brief  Name: XTAL_LTIM / XTAL_DTIM / XTAL_ADC
  * @{
  */
#define LSYS_BIT_FEN_XTAL_LTIM    ((u32)0x00000001 << 2)           /*!< R/W 1h  enable xtal divider for ltim[5:0] use 1:enable 0:disable DD_SEC: bpc_ltim*/
#define LSYS_FEN_XTAL_LTIM(x)     ((u32)(((x) & 0x00000001) << 2))
#define REG_LSYS_CKD_FEN_XTAL_LTIM REG_LSYS_CKD_FEN_XTAL
#define LSYS_GET_FEN_XTAL_LTIM(x) ((u32)(((x >> 2) & 0x00000001)))
#define LSYS_BIT_FEN_XTAL_DTIM    ((u32)0x00000001 << 1)           /*!< R/W 1h  enable xtal divider for dtim use 1:enable 0:disable DD_SEC: bpc_dtim*/
#define LSYS_FEN_XTAL_DTIM(x)     ((u32)(((x) & 0x00000001) << 1))
#define REG_LSYS_CKD_FEN_XTAL_DTIM REG_LSYS_CKD_FEN_XTAL
#define LSYS_GET_FEN_XTAL_DTIM(x) ((u32)(((x >> 1) & 0x00000001)))
#define LSYS_BIT_FEN_XTAL_ADC     ((u32)0x00000001 << 0)           /*!< R/W 1h  enable xtal divider for adc use 1:enable 0:disable DD_SEC: bpc_adc*/
#define LSYS_FEN_XTAL_ADC(x)      ((u32)(((x) & 0x00000001) << 0))
#define REG_LSYS_CKD_FEN_XTAL_ADC REG_LSYS_CKD_FEN_XTAL
#define LSYS_GET_FEN_XTAL_ADC(x)  ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_CKD_FEN_SYS_PLL
  * @brief  Name: SYS_PLL_PSRAM / SYS_PLL_SPIC / SYS_PLL_HPERI / SYS_PLL_HP
  * @{
  */
#define LSYS_BIT_FEN_SYS_PLL_PSRAM    ((u32)0x00000001 << 3)           /*!< R/W 1h  sys_pll_divider for psram enable 1:enable 0:disable DD_SEC: bpc_psram*/
#define LSYS_FEN_SYS_PLL_PSRAM(x)     ((u32)(((x) & 0x00000001) << 3))
#define REG_LSYS_CKD_FEN_SYS_PLL_PSRAM REG_LSYS_CKD_FEN_SYS_PLL
#define LSYS_GET_FEN_SYS_PLL_PSRAM(x) ((u32)(((x >> 3) & 0x00000001)))
#define LSYS_BIT_FEN_SYS_PLL_SPIC     ((u32)0x00000001 << 2)           /*!< R/W 1h  sys_pll_divider for spic enable 1:enable 0:disable DD_SEC: bpc_spic*/
#define LSYS_FEN_SYS_PLL_SPIC(x)      ((u32)(((x) & 0x00000001) << 2))
#define REG_LSYS_CKD_FEN_SYS_PLL_SPIC REG_LSYS_CKD_FEN_SYS_PLL
#define LSYS_GET_FEN_SYS_PLL_SPIC(x)  ((u32)(((x >> 2) & 0x00000001)))
#define LSYS_BIT_FEN_SYS_PLL_HPERI    ((u32)0x00000001 << 1)           /*!< R/W 1h  sys_pll_divider for hperi enable 1:enable 0:disable DD_SEC: S*/
#define LSYS_FEN_SYS_PLL_HPERI(x)     ((u32)(((x) & 0x00000001) << 1))
#define REG_LSYS_CKD_FEN_SYS_PLL_HPERI REG_LSYS_CKD_FEN_SYS_PLL
#define LSYS_GET_FEN_SYS_PLL_HPERI(x) ((u32)(((x >> 1) & 0x00000001)))
#define LSYS_BIT_FEN_SYS_PLL_HP       ((u32)0x00000001 << 0)           /*!< R/W 1h  sys_pll_divider for hp enable 1:enable 0:disable DD_SEC: S*/
#define LSYS_FEN_SYS_PLL_HP(x)        ((u32)(((x) & 0x00000001) << 0))
#define REG_LSYS_CKD_FEN_SYS_PLL_HP   REG_LSYS_CKD_FEN_SYS_PLL
#define LSYS_GET_FEN_SYS_PLL_HP(x)    ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_FEN_USB_PLL
  * @brief
  * @{
  */
#define LSYS_BIT_FEN_USB_PLL_PSRAM    ((u32)0x00000001 << 3)           /*!< R/W 1h  usb_pll_divider for psram enable 1:enable 0:disable DD_SEC: bpc_psram*/
#define LSYS_FEN_USB_PLL_PSRAM(x)     ((u32)(((x) & 0x00000001) << 3))
#define LSYS_GET_FEN_USB_PLL_PSRAM(x) ((u32)(((x >> 3) & 0x00000001)))
#define LSYS_BIT_FEN_USB_PLL_SPIC     ((u32)0x00000001 << 2)           /*!< R/W 1h  usb_pll_divider for spic enable 1:enable 0:disable DD_SEC: bpc_spic*/
#define LSYS_FEN_USB_PLL_SPIC(x)      ((u32)(((x) & 0x00000001) << 2))
#define LSYS_GET_FEN_USB_PLL_SPIC(x)  ((u32)(((x >> 2) & 0x00000001)))
#define LSYS_BIT_FEN_USB_PLL_HPERI    ((u32)0x00000001 << 1)           /*!< R/W 1h  usb_pll_divider for hperi enable 1:enable 0:disable DD_SEC: S*/
#define LSYS_FEN_USB_PLL_HPERI(x)     ((u32)(((x) & 0x00000001) << 1))
#define LSYS_GET_FEN_USB_PLL_HPERI(x) ((u32)(((x >> 1) & 0x00000001)))
#define LSYS_BIT_FEN_USB_PLL_HP       ((u32)0x00000001 << 0)           /*!< R/W 1h  usb_pll_divider for hp enable 1:enable 0:disable DD_SEC: S*/
#define LSYS_FEN_USB_PLL_HP(x)        ((u32)(((x) & 0x00000001) << 0))
#define LSYS_GET_FEN_USB_PLL_HP(x)    ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_CKD_SYS_PLL_GRP0
  * @brief  Name: SYS_PLL_PSRAM / SYS_PLL_SPIC / SYS_PLL_HPERI / SYS_PLL_HP
  * @{
  */
#define LSYS_MASK_CKD_SYS_PLL_PSRAM   ((u32)0x0000000F << 12)           /*!< R/WPD 0h  sys_pll divider for psram 0: Not support 1: div2 ... DIV_MIN:1 DD_SEC: bpc_psram*/
#define LSYS_CKD_SYS_PLL_PSRAM(x)     (((u32)((x) & 0x0000000F) << 12))
#define REG_LSYS_CKD_SYS_PLL_PSRAM    REG_LSYS_CKD_SYS_PLL_GRP0
#define LSYS_GET_CKD_SYS_PLL_PSRAM(x) ((u32)(((x >> 12) & 0x0000000F)))
#define LSYS_MASK_CKD_SYS_PLL_SPIC    ((u32)0x0000000F << 8)            /*!< R/WPD 1h  sys_pll divider for spic 0: Not support 1: div2 ... DIV_MIN:1 DD_SEC: bpc_spic*/
#define LSYS_CKD_SYS_PLL_SPIC(x)      (((u32)((x) & 0x0000000F) << 8))
#define REG_LSYS_CKD_SYS_PLL_SPIC     REG_LSYS_CKD_SYS_PLL_GRP0
#define LSYS_GET_CKD_SYS_PLL_SPIC(x)  ((u32)(((x >> 8) & 0x0000000F)))
#define LSYS_MASK_CKD_SYS_PLL_HPERI   ((u32)0x0000000F << 4)            /*!< R/WPD 3h  sys_pll divider for hperi 0: Not support 1: div2 ... DIV_MIN:1 DD_SEC: S*/
#define LSYS_CKD_SYS_PLL_HPERI(x)     (((u32)((x) & 0x0000000F) << 4))
#define REG_LSYS_CKD_SYS_PLL_HPERI    REG_LSYS_CKD_SYS_PLL_GRP0
#define LSYS_GET_CKD_SYS_PLL_HPERI(x) ((u32)(((x >> 4) & 0x0000000F)))
#define LSYS_MASK_CKD_SYS_PLL_HP      ((u32)0x0000000F << 0)            /*!< R/WPD 1h  sys_pll divider for hp 0: Not support 1: div2 ... DIV_MIN:1 DD_SEC: S*/
#define LSYS_CKD_SYS_PLL_HP(x)        (((u32)((x) & 0x0000000F) << 0))
#define REG_LSYS_CKD_SYS_PLL_HP       REG_LSYS_CKD_SYS_PLL_GRP0
#define LSYS_GET_CKD_SYS_PLL_HP(x)    ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_LSYS_CKD_USB_PLL_GRP0
  * @brief  Name: USB_PLL_PSRAM / USB_PLL_SPIC / USB_PLL_HPERI / USB_PLL_HP
  * @{
  */
#define LSYS_MASK_CKD_USB_PLL_PSRAM   ((u32)0x0000000F << 12)           /*!< R/WPD 2h  usb_pll divider for psram 0: div1 1: div2 ... DIV_MIN:0 DD_SEC: bpc_psram*/
#define LSYS_CKD_USB_PLL_PSRAM(x)     (((u32)((x) & 0x0000000F) << 12))
#define REG_LSYS_CKD_USB_PLL_PSRAM    REG_LSYS_CKD_USB_PLL_GRP0
#define LSYS_GET_CKD_USB_PLL_PSRAM(x) ((u32)(((x >> 12) & 0x0000000F)))
#define LSYS_MASK_CKD_USB_PLL_SPIC    ((u32)0x0000000F << 8)            /*!< R/WPD 3h  usb_pll divider for spic 0: div1 1: div2 ... DIV_MIN:0 DD_SEC: bpc_spic*/
#define LSYS_CKD_USB_PLL_SPIC(x)      (((u32)((x) & 0x0000000F) << 8))
#define REG_LSYS_CKD_USB_PLL_SPIC     REG_LSYS_CKD_USB_PLL_GRP0
#define LSYS_GET_CKD_USB_PLL_SPIC(x)  ((u32)(((x >> 8) & 0x0000000F)))
#define LSYS_MASK_CKD_USB_PLL_HPERI   ((u32)0x0000000F << 4)            /*!< R/WPD 9h  usb_pll divider for hperi 0: div1 1: div2 ... DD_SEC: S*/
#define LSYS_CKD_USB_PLL_HPERI(x)     (((u32)((x) & 0x0000000F) << 4))
#define REG_LSYS_CKD_USB_PLL_HPERI    REG_LSYS_CKD_USB_PLL_GRP0
#define LSYS_GET_CKD_USB_PLL_HPERI(x) ((u32)(((x >> 4) & 0x0000000F)))
#define LSYS_MASK_CKD_USB_PLL_HP      ((u32)0x0000000F << 0)            /*!< R/WPD 3h  usb_pll divider for hp 0: div1 1: div2 ... DIV_MIN:0 DD_SEC: S*/
#define LSYS_CKD_USB_PLL_HP(x)        (((u32)((x) & 0x0000000F) << 0))
#define REG_LSYS_CKD_USB_PLL_HP       REG_LSYS_CKD_USB_PLL_GRP0
#define LSYS_GET_CKD_USB_PLL_HP(x)    ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_LSYS_SW_RST_CTRL
  * @brief
  * @{
  */
#define LSYS_BIT_KM4TZ_SYS_RST         ((u32)0x00000001 << 31)           /*!< R/WE/EC 0x0  Only when REG_SW_RST_TRIG is 0x96969696 , this reg can be write DD_SEC: bpc_swrst*/
#define LSYS_KM4TZ_SYS_RST(x)          ((u32)(((x) & 0x00000001) << 31))
#define LSYS_GET_KM4TZ_SYS_RST(x)      ((u32)(((x >> 31) & 0x00000001)))
#define LSYS_BIT_KM4NS_SYS_RST         ((u32)0x00000001 << 30)           /*!< R/WE/EC 0x0  Only when REG_SW_RST_TRIG is 0x96969696 , this reg can be write DD_SEC: bpc_swrst*/
#define LSYS_KM4NS_SYS_RST(x)          ((u32)(((x) & 0x00000001) << 30))
#define LSYS_GET_KM4NS_SYS_RST(x)      ((u32)(((x >> 30) & 0x00000001)))
#define LSYS_BIT_PLFM1_SYS_RST         ((u32)0x00000001 << 29)           /*!< R/WE/EC 0x0  Only when REG_SW_RST_TRIG is 0x96969696 , this reg can be write DD_SEC: bpc_swrst*/
#define LSYS_PLFM1_SYS_RST(x)          ((u32)(((x) & 0x00000001) << 29))
#define LSYS_GET_PLFM1_SYS_RST(x)      ((u32)(((x >> 29) & 0x00000001)))
#define LSYS_BIT_SYSRST_MSK_CKGEN      ((u32)0x00000001 << 28)           /*!< R/W 0x1  ckgen_syson global reset 1: mask ckgen_syson reset when sys reset happen  0: reset by sys reset DD_SEC: bpc_swrst*/
#define LSYS_SYSRST_MSK_CKGEN(x)       ((u32)(((x) & 0x00000001) << 28))
#define LSYS_GET_SYSRST_MSK_CKGEN(x)   ((u32)(((x >> 28) & 0x00000001)))
#define LSYS_BIT_SYSRST_MSK_PWM        ((u32)0x00000001 << 27)           /*!< R/W 0x1  pwm global reset 1: mask pwm[2:0] reset when sys reset happen  0: reset by sys reset DD_SEC: bpc_swrst*/
#define LSYS_SYSRST_MSK_PWM(x)         ((u32)(((x) & 0x00000001) << 27))
#define LSYS_GET_SYSRST_MSK_PWM(x)     ((u32)(((x >> 27) & 0x00000001)))
#define LSYS_BIT_SYSRST_MSK_GPIO       ((u32)0x00000001 << 26)           /*!< R/W 0x1  gpio global reset 1: mask gpio reset when sys reset happen  0: reset by sys reset DD_SEC: bpc_swrst*/
#define LSYS_SYSRST_MSK_GPIO(x)        ((u32)(((x) & 0x00000001) << 26))
#define LSYS_GET_SYSRST_MSK_GPIO(x)    ((u32)(((x >> 26) & 0x00000001)))
#define LSYS_BIT_SRST1_GLB_RST_EN      ((u32)0x00000001 << 25)           /*!< R/W 0x1  1.system reset1  will reset all syson IP and local CPU1(km4ns) 0.system reset0  will reset local CPU1(km4ns) DD_SEC: bpc_swrst*/
#define LSYS_SRST1_GLB_RST_EN(x)       ((u32)(((x) & 0x00000001) << 25))
#define LSYS_GET_SRST1_GLB_RST_EN(x)   ((u32)(((x >> 25) & 0x00000001)))
#define LSYS_BIT_SRST0_GLB_RST_EN      ((u32)0x00000001 << 24)           /*!< R/W 0x1  1.system reset0  will reset all syson IP and local CPU0(km4tz) 0.system reset0  will reset local CPU0(km4tz) DD_SEC: bpc_swrst*/
#define LSYS_SRST0_GLB_RST_EN(x)       ((u32)(((x) & 0x00000001) << 24))
#define LSYS_GET_SRST0_GLB_RST_EN(x)   ((u32)(((x >> 24) & 0x00000001)))
#define LSYS_MASK_SYS_GLBRST_TIMEOUT   ((u32)0x000000FF << 16)           /*!< R/W 0xff  If system glb rst en is disable , system reset will reset its local cpu only . but if bus is hang , it will trigger global reset also .  HW will wait local CPU's status , if it is not in IDLE in a threshold time, it will trigger global reset . Note : This value must set > 1 , it is base on 4M clock cycle . DD_SEC: bpc_swrst*/
#define LSYS_SYS_GLBRST_TIMEOUT(x)     (((u32)((x) & 0x000000FF) << 16))
#define LSYS_GET_SYS_GLBRST_TIMEOUT(x) ((u32)(((x >> 16) & 0x000000FF)))
#define LSYS_MASK_WDG_GLBRST_TIMEOUT   ((u32)0x000000FF << 8)            /*!< R/W 0xf  If wdg's glb rst en is disable , wdg reset will reset its local cpu only . but if bus is hang , it will trigger global reset also .  HW will wait local CPU's status , if it is not in IDLE in a threshold time, it will trigger global reset . Note : This value must set > 1 , it is base on 32k clock cycle . DD_SEC: bpc_swrst*/
#define LSYS_WDG_GLBRST_TIMEOUT(x)     (((u32)((x) & 0x000000FF) << 8))
#define LSYS_GET_WDG_GLBRST_TIMEOUT(x) ((u32)(((x >> 8) & 0x000000FF)))
#define LSYS_BIT_WDG2_GLB_RST_EN       ((u32)0x00000001 << 6)            /*!< R/W 0x1  1.wdg2 will reset all syson IP and local CPU 0.wdg2 will reset local CPU and itself Note: wdg2 's local CPU is CPU0 DD_SEC: bpc_swrst*/
#define LSYS_WDG2_GLB_RST_EN(x)        ((u32)(((x) & 0x00000001) << 6))
#define LSYS_GET_WDG2_GLB_RST_EN(x)    ((u32)(((x >> 6) & 0x00000001)))
#define LSYS_BIT_WDG1_GLB_RST_EN       ((u32)0x00000001 << 5)            /*!< R/W 0x1  1.wdg1 will reset all syson IP and local CPU 0.wdg1 will reset local CPU and itself Note: wdg1 's local CPU is CPU0 DD_SEC: bpc_swrst*/
#define LSYS_WDG1_GLB_RST_EN(x)        ((u32)(((x) & 0x00000001) << 5))
#define LSYS_GET_WDG1_GLB_RST_EN(x)    ((u32)(((x >> 5) & 0x00000001)))
#define LSYS_BIT_WDG0_GLB_RST_EN       ((u32)0x00000001 << 4)            /*!< R/W 0x1  1.wdg0 will reset all syson IP and local CPU 0.wdg0 will reset local CPU and itself Note: wdg0 's local CPU is CPU1 DD_SEC: bpc_swrst*/
#define LSYS_WDG0_GLB_RST_EN(x)        ((u32)(((x) & 0x00000001) << 4))
#define LSYS_GET_WDG0_GLB_RST_EN(x)    ((u32)(((x >> 4) & 0x00000001)))
#define LSYS_BIT_PERIALL_RST_EN        ((u32)0x00000001 << 1)            /*!< R/W 0x1  1. warm reset request by one cpu will reset all peri and local cpu 0. warm reset request by one cpu will only reset local cpu Note : peri rst will use sysrst mst also DD_SEC: bpc_swrst*/
#define LSYS_PERIALL_RST_EN(x)         ((u32)(((x) & 0x00000001) << 1))
#define LSYS_GET_PERIALL_RST_EN(x)     ((u32)(((x >> 1) & 0x00000001)))
#define LSYS_BIT_OTHERCPU_RST_EN       ((u32)0x00000001 << 0)            /*!< R/W 0x1  1. warm reset request by one cpu will reset other cpu's core and local cpu 0. warm reset request by one cpu will only reset local cpu DD_SEC: bpc_swrst*/
#define LSYS_OTHERCPU_RST_EN(x)        ((u32)(((x) & 0x00000001) << 0))
#define LSYS_GET_OTHERCPU_RST_EN(x)    ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_SW_RST_TRIG
  * @brief
  * @{
  */
#define LSYS_MASK_TRIG_RST_P1   ((u32)0x0000FFFF << 16)           /*!< R/WPD 0x0  System reset trigger register Write 0x96969696 to enable the write of reset control Write 0x69696969 will trigger the system if REG_SW_RST_CTRL set . DD_SEC: bpc_swrst*/
#define LSYS_TRIG_RST_P1(x)     (((u32)((x) & 0x0000FFFF) << 16))
#define LSYS_GET_TRIG_RST_P1(x) ((u32)(((x >> 16) & 0x0000FFFF)))
#define LSYS_MASK_TRIG_RST      ((u32)0x0000FFFF << 0)            /*!< R/WPD 0x0  System reset trigger register Write 0x96969696 to enable the write of reset control Write 0x69696969 will trigger the system if REG_SW_RST_CTRL set . DD_SEC: bpc_swrst*/
#define LSYS_TRIG_RST(x)        (((u32)((x) & 0x0000FFFF) << 0))
#define LSYS_GET_TRIG_RST(x)    ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup REG_LSYS_BOOT_CFG
  * @brief
  * @{
  */
#define LSYS_BIT_BOOT_CPU1_RUN           ((u32)0x00000001 << 31)           /*!< R/W 0x0  */
#define LSYS_BOOT_CPU1_RUN(x)            ((u32)(((x) & 0x00000001) << 31))
#define LSYS_GET_BOOT_CPU1_RUN(x)        ((u32)(((x >> 31) & 0x00000001)))
#define LSYS_BIT_BOOT_WAKE_FROM_PS_LS    ((u32)0x00000001 << 30)           /*!< R/W 0x0  */
#define LSYS_BOOT_WAKE_FROM_PS_LS(x)     ((u32)(((x) & 0x00000001) << 30))
#define LSYS_GET_BOOT_WAKE_FROM_PS_LS(x) ((u32)(((x >> 30) & 0x00000001)))
#define LSYS_BIT_BOOT_WAKE_FROM_PS_HS    ((u32)0x00000001 << 29)           /*!< R/W 0x0   DD_SEC: bpc_cpu0*/
#define LSYS_BOOT_WAKE_FROM_PS_HS(x)     ((u32)(((x) & 0x00000001) << 29))
#define LSYS_GET_BOOT_WAKE_FROM_PS_HS(x) ((u32)(((x >> 29) & 0x00000001)))
#define LSYS_BIT_BOOT_PATCH_FUNC0        ((u32)0x00000001 << 28)           /*!< R/W 0x0   DD_SEC: S*/
#define LSYS_BOOT_PATCH_FUNC0(x)         ((u32)(((x) & 0x00000001) << 28))
#define LSYS_GET_BOOT_PATCH_FUNC0(x)     ((u32)(((x >> 28) & 0x00000001)))
#define LSYS_MASK_BOOT_RSVD_SW           ((u32)0x00000FFF << 16)           /*!< R/W 0x0  */
#define LSYS_BOOT_RSVD_SW(x)             (((u32)((x) & 0x00000FFF) << 16))
#define LSYS_GET_BOOT_RSVD_SW(x)         ((u32)(((x >> 16) & 0x00000FFF)))
#define LSYS_MASK_ROM_VERSION_SW         ((u32)0x0000FFFF << 0)            /*!< R/W 0x0   DD_SEC: S*/
#define LSYS_ROM_VERSION_SW(x)           (((u32)((x) & 0x0000FFFF) << 0))
#define LSYS_GET_ROM_VERSION_SW(x)       ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup REG_LSYS_SW_PATCH
  * @brief
  * @{
  */
#define LSYS_MASK_SW_PATCH_SADDR   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  SW patch code start addr. OTP physical addr 0x500-0x6FF will autoload to this addr . DD_SEC: S*/
#define LSYS_SW_PATCH_SADDR(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define LSYS_GET_SW_PATCH_SADDR(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_LSYS_CHIP_INFO
  * @brief
  * @{
  */
#define LSYS_MASK_CHIP_INFO_EN   ((u32)0x0000000F << 28)           /*!< R/W 0x0  Enable Internal Chip Information display.(Don't move 0x74 address , for code is not auto-gen) 0xA: enable. Others: disable. Note: CHIP_INFO can be read only if CHIP_INFO_EN[3:0]=0xA, otherwise the read result will return 0x0.*/
#define LSYS_CHIP_INFO_EN(x)     (((u32)((x) & 0x0000000F) << 28))
#define LSYS_GET_CHIP_INFO_EN(x) ((u32)(((x >> 28) & 0x0000000F)))
#define LSYS_MASK_RL_VER         ((u32)0x0000000F << 16)           /*!< RMSK1 0x0  CHIP cut verison : 0:  A-cut 1:  B-cut ...*/
#define LSYS_RL_VER(x)           (((u32)((x) & 0x0000000F) << 16))
#define LSYS_GET_RL_VER(x)       ((u32)(((x >> 16) & 0x0000000F)))
#define LSYS_MASK_RL_NO          ((u32)0x0000FFFF << 0)            /*!< RMSK1 0x6955  CHIP RL NUMBER :  0x6955 Note: this field also used by FPGA or Simulation . 0xFFFF: FPGA  0XFFFE: PXP 0xFFF0: IMG Simulation 0 (when simulation , disable loguart for speedup, disable calibration)*/
#define LSYS_RL_NO(x)            (((u32)((x) & 0x0000FFFF) << 0))
#define LSYS_GET_RL_NO(x)        ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup REG_LSYS_SW_SWD_CTRL
  * @brief
  * @{
  */
#define LSYS_BIT_SW_SWD_SPNIDEN    ((u32)0x00000001 << 3)           /*!< R/W 0x0   DD_SEC: S*/
#define LSYS_SW_SWD_SPNIDEN(x)     ((u32)(((x) & 0x00000001) << 3))
#define LSYS_GET_SW_SWD_SPNIDEN(x) ((u32)(((x >> 3) & 0x00000001)))
#define LSYS_BIT_SW_SWD_SPIDEN     ((u32)0x00000001 << 2)           /*!< R/W 0x0   DD_SEC: S*/
#define LSYS_SW_SWD_SPIDEN(x)      ((u32)(((x) & 0x00000001) << 2))
#define LSYS_GET_SW_SWD_SPIDEN(x)  ((u32)(((x >> 2) & 0x00000001)))
#define LSYS_BIT_SW_SWD_NIDEN      ((u32)0x00000001 << 1)           /*!< R/W 0x0   DD_SEC: S*/
#define LSYS_SW_SWD_NIDEN(x)       ((u32)(((x) & 0x00000001) << 1))
#define LSYS_GET_SW_SWD_NIDEN(x)   ((u32)(((x >> 1) & 0x00000001)))
#define LSYS_BIT_SW_SWD_DBGEN      ((u32)0x00000001 << 0)           /*!< R/W 0x0   DD_SEC: S*/
#define LSYS_SW_SWD_DBGEN(x)       ((u32)(((x) & 0x00000001) << 0))
#define LSYS_GET_SW_SWD_DBGEN(x)   ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_FPGA_CLK_INFO
  * @brief
  * @{
  */
#define LSYS_MASK_FPGA_KM4TZ_CLK   ((u32)0x000000FF << 24)           /*!< R 0x0  FPGA KM4TZ clock*/
#define LSYS_FPGA_KM4TZ_CLK(x)     (((u32)((x) & 0x000000FF) << 24))
#define LSYS_GET_FPGA_KM4TZ_CLK(x) ((u32)(((x >> 24) & 0x000000FF)))
#define LSYS_MASK_FPGA_KM4NS_CLK   ((u32)0x000000FF << 16)           /*!< R 0x0  FPGA KM4NS clock*/
#define LSYS_FPGA_KM4NS_CLK(x)     (((u32)((x) & 0x000000FF) << 16))
#define LSYS_GET_FPGA_KM4NS_CLK(x) ((u32)(((x >> 16) & 0x000000FF)))
#define LSYS_MASK_FPGA_PSRAM_CLK   ((u32)0x000000FF << 8)            /*!< R 0x0  FPGA PSRAM clock*/
#define LSYS_FPGA_PSRAM_CLK(x)     (((u32)((x) & 0x000000FF) << 8))
#define LSYS_GET_FPGA_PSRAM_CLK(x) ((u32)(((x >> 8) & 0x000000FF)))
#define LSYS_MASK_FPGA_SPIC_CLK    ((u32)0x000000FF << 0)            /*!< R 0x0  FPGA SPIC clock*/
#define LSYS_FPGA_SPIC_CLK(x)      (((u32)((x) & 0x000000FF) << 0))
#define LSYS_GET_FPGA_SPIC_CLK(x)  ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup REG_LSYS_KM4_XO_CTRL0
  * @brief
  * @{
  */
#define LSYS_MASK_INIT_XO_BASE   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  */
#define LSYS_INIT_XO_BASE(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define LSYS_GET_INIT_XO_BASE(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_LSYS_KM4_XO_CTRL1
  * @brief
  * @{
  */
#define LSYS_MASK_INIT_XO_TOP   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  */
#define LSYS_INIT_XO_TOP(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define LSYS_GET_INIT_XO_TOP(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_LSYS_SEC_BPC_CTRL
  * @brief
  * @{
  */
#define LSYS_BIT_BPC_BOR        ((u32)0x00000001 << 31)           /*!< R/W 0x1  Select IP controller source : bor 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_BOR(x)         ((u32)(((x) & 0x00000001) << 31))
#define LSYS_GET_BPC_BOR(x)     ((u32)(((x >> 31) & 0x00000001)))
#define LSYS_BIT_BPC_CHIPEN     ((u32)0x00000001 << 30)           /*!< R/W 0x1  Select IP controller source : chipen 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_CHIPEN(x)      ((u32)(((x) & 0x00000001) << 30))
#define LSYS_GET_BPC_CHIPEN(x)  ((u32)(((x >> 30) & 0x00000001)))
#define LSYS_BIT_BPC_RTC        ((u32)0x00000001 << 29)           /*!< R/W 0x1  Select IP controller source : rtc 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_RTC(x)         ((u32)(((x) & 0x00000001) << 29))
#define LSYS_GET_BPC_RTC(x)     ((u32)(((x >> 29) & 0x00000001)))
#define LSYS_BIT_BPC_ATIM       ((u32)0x00000001 << 28)           /*!< R/W 0x1  Select IP controller source : atim 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_ATIM(x)        ((u32)(((x) & 0x00000001) << 28))
#define LSYS_GET_BPC_ATIM(x)    ((u32)(((x >> 28) & 0x00000001)))
#define LSYS_BIT_BPC_AUDIO      ((u32)0x00000001 << 25)           /*!< R/W 0x1  Select IP controller source : audio 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_AUDIO(x)       ((u32)(((x) & 0x00000001) << 25))
#define LSYS_GET_BPC_AUDIO(x)   ((u32)(((x >> 25) & 0x00000001)))
#define LSYS_BIT_BPC_GMAC       ((u32)0x00000001 << 23)           /*!< R/W 0x1  Select IP controller source : gmac 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_GMAC(x)        ((u32)(((x) & 0x00000001) << 23))
#define LSYS_GET_BPC_GMAC(x)    ((u32)(((x >> 23) & 0x00000001)))
#define LSYS_BIT_BPC_LOGUART    ((u32)0x00000001 << 22)           /*!< R/W 0x1  Select IP controller source : loguart 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_LOGUART(x)     ((u32)(((x) & 0x00000001) << 22))
#define LSYS_GET_BPC_LOGUART(x) ((u32)(((x >> 22) & 0x00000001)))
#define LSYS_BIT_BPC_UART       ((u32)0x00000001 << 21)           /*!< R/W 0x1  Select IP controller source : uart 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_UART(x)        ((u32)(((x) & 0x00000001) << 21))
#define LSYS_GET_BPC_UART(x)    ((u32)(((x >> 21) & 0x00000001)))
#define LSYS_BIT_BPC_ADC        ((u32)0x00000001 << 20)           /*!< R/W 0x1  Select IP controller source : adc4m 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_ADC(x)         ((u32)(((x) & 0x00000001) << 20))
#define LSYS_GET_BPC_ADC(x)     ((u32)(((x >> 20) & 0x00000001)))
#define LSYS_BIT_BPC_DTIM       ((u32)0x00000001 << 19)           /*!< R/W 0x1  Select IP controller source : dtim 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_DTIM(x)        ((u32)(((x) & 0x00000001) << 19))
#define LSYS_GET_BPC_DTIM(x)    ((u32)(((x >> 19) & 0x00000001)))
#define LSYS_BIT_BPC_IPC        ((u32)0x00000001 << 18)           /*!< R/W 0x1  Select IP controller source : ipc 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_IPC(x)         ((u32)(((x) & 0x00000001) << 18))
#define LSYS_GET_BPC_IPC(x)     ((u32)(((x >> 18) & 0x00000001)))
#define LSYS_BIT_BPC_WDG        ((u32)0x00000001 << 17)           /*!< R/W 0x1  Select IP controller source : wdg 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_WDG(x)         ((u32)(((x) & 0x00000001) << 17))
#define LSYS_GET_BPC_WDG(x)     ((u32)(((x >> 17) & 0x00000001)))
#define LSYS_BIT_BPC_GPIO       ((u32)0x00000001 << 16)           /*!< R/W 0x1  Select IP controller source : gpio 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_GPIO(x)        ((u32)(((x) & 0x00000001) << 16))
#define LSYS_GET_BPC_GPIO(x)    ((u32)(((x >> 16) & 0x00000001)))
#define LSYS_BIT_BPC_PTIM       ((u32)0x00000001 << 15)           /*!< R/W 0x1  Select IP controller source : ptim 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_PTIM(x)        ((u32)(((x) & 0x00000001) << 15))
#define LSYS_GET_BPC_PTIM(x)    ((u32)(((x >> 15) & 0x00000001)))
#define LSYS_BIT_BPC_I2C        ((u32)0x00000001 << 14)           /*!< R/W 0x1  Select IP controller source : i2c 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_I2C(x)         ((u32)(((x) & 0x00000001) << 14))
#define LSYS_GET_BPC_I2C(x)     ((u32)(((x >> 14) & 0x00000001)))
#define LSYS_BIT_BPC_SPI        ((u32)0x00000001 << 13)           /*!< R/W 0x1  Select IP controller source : spi 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_SPI(x)         ((u32)(((x) & 0x00000001) << 13))
#define LSYS_GET_BPC_SPI(x)     ((u32)(((x >> 13) & 0x00000001)))
#define LSYS_BIT_BPC_IRDA       ((u32)0x00000001 << 12)           /*!< R/W 0x1  Select IP controller source : irda 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_IRDA(x)        ((u32)(((x) & 0x00000001) << 12))
#define LSYS_GET_BPC_IRDA(x)    ((u32)(((x >> 12) & 0x00000001)))
#define LSYS_BIT_BPC_OTPC       ((u32)0x00000001 << 11)           /*!< R/W 0x1  Select IP controller source : otpc 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_OTPC(x)        ((u32)(((x) & 0x00000001) << 11))
#define LSYS_GET_BPC_OTPC(x)    ((u32)(((x >> 11) & 0x00000001)))
#define LSYS_BIT_BPC_PKE        ((u32)0x00000001 << 7)            /*!< R/W 0x1  Select IP controller source : pke 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_PKE(x)         ((u32)(((x) & 0x00000001) << 7))
#define LSYS_GET_BPC_PKE(x)     ((u32)(((x >> 7) & 0x00000001)))
#define LSYS_BIT_BPC_TRNG       ((u32)0x00000001 << 6)            /*!< R/W 0x1  Select IP controller source : trng 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_TRNG(x)        ((u32)(((x) & 0x00000001) << 6))
#define LSYS_GET_BPC_TRNG(x)    ((u32)(((x >> 6) & 0x00000001)))
#define LSYS_BIT_BPC_CRYPTO     ((u32)0x00000001 << 5)            /*!< R/W 0x1  Select IP controller source : crypto 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_CRYPTO(x)      ((u32)(((x) & 0x00000001) << 5))
#define LSYS_GET_BPC_CRYPTO(x)  ((u32)(((x >> 5) & 0x00000001)))
#define LSYS_BIT_BPC_PWM        ((u32)0x00000001 << 4)            /*!< R/W 0x1  Select IP controller source : pwm 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_PWM(x)         ((u32)(((x) & 0x00000001) << 4))
#define LSYS_GET_BPC_PWM(x)     ((u32)(((x >> 4) & 0x00000001)))
#define LSYS_BIT_BPC_LTIM       ((u32)0x00000001 << 3)            /*!< R/W 0x1  Select IP controller source : ltim 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_LTIM(x)        ((u32)(((x) & 0x00000001) << 3))
#define LSYS_GET_BPC_LTIM(x)    ((u32)(((x >> 3) & 0x00000001)))
#define LSYS_BIT_BPC_DMAC       ((u32)0x00000001 << 2)            /*!< R/W 0x1  Select IP controller source : dmac 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_DMAC(x)        ((u32)(((x) & 0x00000001) << 2))
#define LSYS_GET_BPC_DMAC(x)    ((u32)(((x >> 2) & 0x00000001)))
#define LSYS_BIT_BPC_SDIO       ((u32)0x00000001 << 1)            /*!< R/W 0x1  Select IP controller source : sdio 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_SDIO(x)        ((u32)(((x) & 0x00000001) << 1))
#define LSYS_GET_BPC_SDIO(x)    ((u32)(((x >> 1) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_SEC_PPC_CTRL
  * @brief
  * @{
  */
#define LSYS_BIT_BPC_BT       ((u32)0x00000001 << 31)           /*!< R/W 0x1  Select IP controller source : bt 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_BT(x)        ((u32)(((x) & 0x00000001) << 31))
#define LSYS_GET_BPC_BT(x)    ((u32)(((x >> 31) & 0x00000001)))
#define LSYS_BIT_BPC_WL       ((u32)0x00000001 << 30)           /*!< R/W 0x1  Select IP controller source : wl 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_WL(x)        ((u32)(((x) & 0x00000001) << 30))
#define LSYS_GET_BPC_WL(x)    ((u32)(((x >> 30) & 0x00000001)))
#define LSYS_BIT_BPC_PSRAM    ((u32)0x00000001 << 29)           /*!< R/W 0x1  Select IP controller source : psram 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_PSRAM(x)     ((u32)(((x) & 0x00000001) << 29))
#define LSYS_GET_BPC_PSRAM(x) ((u32)(((x >> 29) & 0x00000001)))
#define LSYS_BIT_BPC_SPIC     ((u32)0x00000001 << 28)           /*!< R/W 0x1  Select IP controller source : spic 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_SPIC(x)      ((u32)(((x) & 0x00000001) << 28))
#define LSYS_GET_BPC_SPIC(x)  ((u32)(((x >> 28) & 0x00000001)))
#define LSYS_BIT_BPC_CPU1     ((u32)0x00000001 << 27)           /*!< R/W 0x1  Select IP controller source : cpu1 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_CPU1(x)      ((u32)(((x) & 0x00000001) << 27))
#define LSYS_GET_BPC_CPU1(x)  ((u32)(((x >> 27) & 0x00000001)))
#define LSYS_BIT_BPC_CPU0     ((u32)0x00000001 << 26)           /*!< R/W 0x1  Select IP controller source : cpu0 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_CPU0(x)      ((u32)(((x) & 0x00000001) << 26))
#define LSYS_GET_BPC_CPU0(x)  ((u32)(((x >> 26) & 0x00000001)))
#define LSYS_BIT_BPC_SWRST    ((u32)0x00000001 << 25)           /*!< R/W 0x1  Select IP controller source : rst 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_SWRST(x)     ((u32)(((x) & 0x00000001) << 25))
#define LSYS_GET_BPC_SWRST(x) ((u32)(((x >> 25) & 0x00000001)))
#define LSYS_BIT_BPC_PMC      ((u32)0x00000001 << 24)           /*!< R/W 0x1  Select IP controller source : pmc 1:control by nonsec zone 0:control by secure zone DD_SEC: S*/
#define LSYS_BPC_PMC(x)       ((u32)(((x) & 0x00000001) << 24))
#define LSYS_GET_BPC_PMC(x)   ((u32)(((x >> 24) & 0x00000001)))
#define LSYS_MASK_PPC_WDG     ((u32)0x00000003 << 12)           /*!< R/W 0x3  IP slave port R/W secure ctrl [0]: wr prot ctrl ,   0: secure only , 1: both [1]: rd prot ctrl , 0:secure only , 1:both DD_SEC: S*/
#define LSYS_PPC_WDG(x)       (((u32)((x) & 0x00000003) << 12))
#define LSYS_GET_PPC_WDG(x)   ((u32)(((x >> 12) & 0x00000003)))
#define LSYS_MASK_PPC_MEM     ((u32)0x00000003 << 8)            /*!< R/W 0x3  IP slave port R/W secure ctrl [0]: wr prot ctrl ,   0: secure only , 1: both [1]: rd prot ctrl , 0:secure only , 1:both DD_SEC: S*/
#define LSYS_PPC_MEM(x)       (((u32)((x) & 0x00000003) << 8))
#define LSYS_GET_PPC_MEM(x)   ((u32)(((x >> 8) & 0x00000003)))
#define LSYS_MASK_PPC_PAD     ((u32)0x00000003 << 6)            /*!< R/W 0x3  IP slave port R/W secure ctrl [0]: wr prot ctrl ,   0: secure only , 1: both [1]: rd prot ctrl , 0:secure only , 1:both Note:this control both of pinmux and ledc, for they are in same reset domain. DD_SEC: S*/
#define LSYS_PPC_PAD(x)       (((u32)((x) & 0x00000003) << 6))
#define LSYS_GET_PPC_PAD(x)   ((u32)(((x >> 6) & 0x00000003)))
#define LSYS_MASK_PPC_RTC     ((u32)0x00000003 << 4)            /*!< R/W 0x3  IP slave port R/W secure ctrl [0]: wr prot ctrl ,   0: secure only , 1: both [1]: rd prot ctrl , 0:secure only , 1:both DD_SEC: S*/
#define LSYS_PPC_RTC(x)       (((u32)((x) & 0x00000003) << 4))
#define LSYS_GET_PPC_RTC(x)   ((u32)(((x >> 4) & 0x00000003)))
#define LSYS_MASK_PPC_SDM     ((u32)0x00000003 << 2)            /*!< R/W 0x3  IP slave port R/W secure ctrl [0]: wr prot ctrl ,   0: secure only , 1: both [1]: rd prot ctrl , 0:secure only , 1:both DD_SEC: S*/
#define LSYS_PPC_SDM(x)       (((u32)((x) & 0x00000003) << 2))
#define LSYS_GET_PPC_SDM(x)   ((u32)(((x >> 2) & 0x00000003)))
#define LSYS_MASK_PPC_AIP     ((u32)0x00000003 << 0)            /*!< R/W 0x3  IP slave port R/W secure ctrl [0]: wr prot ctrl ,   0: secure only , 1: both [1]: rd prot ctrl , 0:secure only , 1:both DD_SEC: S*/
#define LSYS_PPC_AIP(x)       (((u32)((x) & 0x00000003) << 0))
#define LSYS_GET_PPC_AIP(x)   ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup REG_LSYS_CPU0_PC
  * @brief
  * @{
  */
#define LSYS_MASK_KM4TZ_DBG_PC      ((u32)0x3FFFFFFF << 2)           /*!< R 0x0  cpu0 pc value for debug which is pc[31:2]*/
#define LSYS_KM4TZ_DBG_PC(x)        (((u32)((x) & 0x3FFFFFFF) << 2))
#define LSYS_GET_KM4TZ_DBG_PC(x)    ((u32)(((x >> 2) & 0x3FFFFFFF)))
#define LSYS_BIT_KM4TZ_DBG_PC_EN    ((u32)0x00000001 << 0)           /*!< R/W 0x0  debug enable DD_SEC: S*/
#define LSYS_KM4TZ_DBG_PC_EN(x)     ((u32)(((x) & 0x00000001) << 0))
#define LSYS_GET_KM4TZ_DBG_PC_EN(x) ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_CPU1_PC
  * @brief
  * @{
  */
#define LSYS_MASK_KM4NS_DBG_PC      ((u32)0x3FFFFFFF << 2)           /*!< R 0x0  cpu1 pc value for debug which is pc[31:2]*/
#define LSYS_KM4NS_DBG_PC(x)        (((u32)((x) & 0x3FFFFFFF) << 2))
#define LSYS_GET_KM4NS_DBG_PC(x)    ((u32)(((x >> 2) & 0x3FFFFFFF)))
#define LSYS_BIT_KM4NS_DBG_PC_EN    ((u32)0x00000001 << 0)           /*!< R/W 0x0  debug enable*/
#define LSYS_KM4NS_DBG_PC_EN(x)     ((u32)(((x) & 0x00000001) << 0))
#define LSYS_GET_KM4NS_DBG_PC_EN(x) ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_PLAT_CTRL
  * @brief
  * @{
  */
#define LSYS_MASK_CRYPTO_CLK_OPT           ((u32)0x00000003 << 30)           /*!< R/W 0h  1X: always ON 01: always OFF 00: controlled by internal ICG DD_SEC: bpc_crypto*/
#define LSYS_CRYPTO_CLK_OPT(x)             (((u32)((x) & 0x00000003) << 30))
#define LSYS_GET_CRYPTO_CLK_OPT(x)         ((u32)(((x >> 30) & 0x00000003)))
#define LSYS_BIT_KM4NS_SEV_IN              ((u32)0x00000001 << 29)           /*!< R/W 0x0  cpu1 sev input, posedge of this bit will trig cpu1 event*/
#define LSYS_KM4NS_SEV_IN(x)               ((u32)(((x) & 0x00000001) << 29))
#define LSYS_GET_KM4NS_SEV_IN(x)           ((u32)(((x >> 29) & 0x00000001)))
#define LSYS_BIT_KM4TZ_SEV_IN              ((u32)0x00000001 << 28)           /*!< R/W 0x0  cpu0 sev input, posedge of this bit will trig CPU0 event*/
#define LSYS_KM4TZ_SEV_IN(x)               ((u32)(((x) & 0x00000001) << 28))
#define LSYS_GET_KM4TZ_SEV_IN(x)           ((u32)(((x >> 28) & 0x00000001)))
#define LSYS_BIT_SPI1_MST                  ((u32)0x00000001 << 27)           /*!< R/W 0h  1: SPI1 used as master 0: SPI1 used as slaver DD_SEC: bpc_spi*/
#define LSYS_SPI1_MST(x)                   ((u32)(((x) & 0x00000001) << 27))
#define LSYS_GET_SPI1_MST(x)               ((u32)(((x >> 27) & 0x00000001)))
#define LSYS_BIT_SPI0_MST                  ((u32)0x00000001 << 26)           /*!< R/W 0h  1: SPI0 used as master 0: SPI0 used as slaver DD_SEC: bpc_spi*/
#define LSYS_SPI0_MST(x)                   ((u32)(((x) & 0x00000001) << 26))
#define LSYS_GET_SPI0_MST(x)               ((u32)(((x >> 26) & 0x00000001)))
#define LSYS_BIT_SPIC_SPDUP_SIM            ((u32)0x00000001 << 25)           /*!< R/W 0x0  1: SPIC will wait 256 spic_clk cycles before ready 0: SPIC will wait 2^16 spic_clk  cycles before ready*/
#define LSYS_SPIC_SPDUP_SIM(x)             ((u32)(((x) & 0x00000001) << 25))
#define LSYS_GET_SPIC_SPDUP_SIM(x)         ((u32)(((x >> 25) & 0x00000001)))
#define LSYS_BIT_PSRAM_SPDUP_SIM           ((u32)0x00000001 << 24)           /*!< R/W 0x0  psramc simulatin speedup enable. Nouse because config form not specified .*/
#define LSYS_PSRAM_SPDUP_SIM(x)            ((u32)(((x) & 0x00000001) << 24))
#define LSYS_GET_PSRAM_SPDUP_SIM(x)        ((u32)(((x >> 24) & 0x00000001)))
#define LSYS_BIT_QSPI_SPDUP_SIM            ((u32)0x00000001 << 23)           /*!< R/W 0x0  qspi simulation speedup enable*/
#define LSYS_QSPI_SPDUP_SIM(x)             ((u32)(((x) & 0x00000001) << 23))
#define LSYS_GET_QSPI_SPDUP_SIM(x)         ((u32)(((x >> 23) & 0x00000001)))
#define LSYS_MASK_GDMA0_CCM                ((u32)0x00000003 << 21)           /*!< R/W 0h  GDMA0 clock control mode*/
#define LSYS_GDMA0_CCM(x)                  (((u32)((x) & 0x00000003) << 21))
#define LSYS_GET_GDMA0_CCM(x)              ((u32)(((x >> 21) & 0x00000003)))
#define LSYS_BIT_KM4NS_RETENTION_MODE      ((u32)0x00000001 << 17)           /*!< R/W 0x0  1: Enable, when CPU1 reset, cache will not be cleared  0: Disable, when CPU1 reset, cache will be cleared by HW*/
#define LSYS_KM4NS_RETENTION_MODE(x)       ((u32)(((x) & 0x00000001) << 17))
#define LSYS_GET_KM4NS_RETENTION_MODE(x)   ((u32)(((x >> 17) & 0x00000001)))
#define LSYS_BIT_KM4TZ_RETENTION_MODE      ((u32)0x00000001 << 16)           /*!< R/W 0x0  1: Enable, when CPU0 reset, cache will not be cleared  0: Disable, when CPU0 reset, cache will be cleared by HW*/
#define LSYS_KM4TZ_RETENTION_MODE(x)       ((u32)(((x) & 0x00000001) << 16))
#define LSYS_GET_KM4TZ_RETENTION_MODE(x)   ((u32)(((x >> 16) & 0x00000001)))
#define LSYS_BIT_SYS_RETENTION_SAVE        ((u32)0x00000001 << 15)           /*!< R/WA0 0x0  1: save data DD_SEC: S*/
#define LSYS_SYS_RETENTION_SAVE(x)         ((u32)(((x) & 0x00000001) << 15))
#define LSYS_GET_SYS_RETENTION_SAVE(x)     ((u32)(((x >> 15) & 0x00000001)))
#define LSYS_BIT_SYS_RETENTION_LOAD        ((u32)0x00000001 << 14)           /*!< R/WA0 0x0  1: restore data DD_SEC: S*/
#define LSYS_SYS_RETENTION_LOAD(x)         ((u32)(((x) & 0x00000001) << 14))
#define LSYS_GET_SYS_RETENTION_LOAD(x)     ((u32)(((x >> 14) & 0x00000001)))
#define LSYS_BIT_MST_SEC_CTRL_SDIO_BT      ((u32)0x00000001 << 9)            /*!< R/WH 0h  plfm master secure access control . 0: secure property determine by IP self 1: force master to be  nonsec access Note: this bit will sticky to high once it is set to 1 DD_SEC: S*/
#define LSYS_MST_SEC_CTRL_SDIO_BT(x)       ((u32)(((x) & 0x00000001) << 9))
#define LSYS_GET_MST_SEC_CTRL_SDIO_BT(x)   ((u32)(((x >> 9) & 0x00000001)))
#define LSYS_BIT_MST_SEC_CTRL_SDIO_WL      ((u32)0x00000001 << 8)            /*!< R/WH 0h  plfm master secure access control . 0: secure property determine by IP self 1: force master to be  nonsec access Note: this bit will sticky to high once it is set to 1 DD_SEC: S*/
#define LSYS_MST_SEC_CTRL_SDIO_WL(x)       ((u32)(((x) & 0x00000001) << 8))
#define LSYS_GET_MST_SEC_CTRL_SDIO_WL(x)   ((u32)(((x >> 8) & 0x00000001)))
#define LSYS_BIT_MST_SEC_CTRL_AES          ((u32)0x00000001 << 7)            /*!< R/WH 0h  plfm master secure access control . 0: secure property determine by IP self 1: force master to be  nonsec access Note: this bit will sticky to high once it is set to 1 DD_SEC: S*/
#define LSYS_MST_SEC_CTRL_AES(x)           ((u32)(((x) & 0x00000001) << 7))
#define LSYS_GET_MST_SEC_CTRL_AES(x)       ((u32)(((x >> 7) & 0x00000001)))
#define LSYS_BIT_MST_SEC_CTRL_GDMA         ((u32)0x00000001 << 6)            /*!< R/WH 0h  plfm master secure access control . 0: secure property determine by IP self 1: force master to be  nonsec access Note: this bit will sticky to high once it is set to 1 DD_SEC: S*/
#define LSYS_MST_SEC_CTRL_GDMA(x)          ((u32)(((x) & 0x00000001) << 6))
#define LSYS_GET_MST_SEC_CTRL_GDMA(x)      ((u32)(((x >> 6) & 0x00000001)))
#define LSYS_BIT_MST_SEC_CTRL_SIC          ((u32)0x00000001 << 5)            /*!< R/WH 0h  plfm master secure access control . 0: secure property determine by IP self 1: force master to be  nonsec access Note: this bit will sticky to high once it is set to 1 DD_SEC: S*/
#define LSYS_MST_SEC_CTRL_SIC(x)           ((u32)(((x) & 0x00000001) << 5))
#define LSYS_GET_MST_SEC_CTRL_SIC(x)       ((u32)(((x >> 5) & 0x00000001)))
#define LSYS_BIT_MST_SEC_CTRL_OTPC         ((u32)0x00000001 << 4)            /*!< R/WH 0h  plfm master secure access control . 0: secure property determine by IP self 1: force master to be  nonsec access Note: this bit will sticky to high once it is set to 1 DD_SEC: S*/
#define LSYS_MST_SEC_CTRL_OTPC(x)          ((u32)(((x) & 0x00000001) << 4))
#define LSYS_GET_MST_SEC_CTRL_OTPC(x)      ((u32)(((x >> 4) & 0x00000001)))
#define LSYS_BIT_PLFM0_ISOLATION_ENABLE    ((u32)0x00000001 << 1)            /*!< R/W 0h  1:plfm1 -> plfm0 access will be blocked*/
#define LSYS_PLFM0_ISOLATION_ENABLE(x)     ((u32)(((x) & 0x00000001) << 1))
#define LSYS_GET_PLFM0_ISOLATION_ENABLE(x) ((u32)(((x >> 1) & 0x00000001)))
#define LSYS_BIT_HUK_PROT                  ((u32)0x00000001 << 0)            /*!< R/WH 0x0  0: only cpu can read HUK key 1: no one can read HUK key This bit will be latch to high when it was write to 1 until reset again . DD_SEC: S*/
#define LSYS_HUK_PROT(x)                   ((u32)(((x) & 0x00000001) << 0))
#define LSYS_GET_HUK_PROT(x)               ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_PLAT_STATUS
  * @brief
  * @{
  */
#define LSYS_MASK_PKE_ATTR                ((u32)0x000000FF << 24)           /*!< R/W 0x0  pke key secure attribute DD_SEC: s*/
#define LSYS_PKE_ATTR(x)                  (((u32)((x) & 0x000000FF) << 24))
#define LSYS_GET_PKE_ATTR(x)              ((u32)(((x >> 24) & 0x000000FF)))
#define LSYS_MASK_PKE_KEY_MASK            ((u32)0x00000003 << 20)           /*!< R/W 0x0  pke key0 mask mode 00:no mask 01:mask high 128bit 10:mask high 256bit 11:no mask DD_SEC: s*/
#define LSYS_PKE_KEY_MASK(x)              (((u32)((x) & 0x00000003) << 20))
#define LSYS_GET_PKE_KEY_MASK(x)          ((u32)(((x >> 20) & 0x00000003)))
#define LSYS_BIT_INIT_XO_RANGE_EN         ((u32)0x00000001 << 16)           /*!< R/W 0x0   DD_SEC: s*/
#define LSYS_INIT_XO_RANGE_EN(x)          ((u32)(((x) & 0x00000001) << 16))
#define LSYS_GET_INIT_XO_RANGE_EN(x)      ((u32)(((x >> 16) & 0x00000001)))
#define LSYS_BIT_KM4TZ_RST_HALT           ((u32)0x00000001 << 15)           /*!< R/W 0x0   DD_SEC: s*/
#define LSYS_KM4TZ_RST_HALT(x)            ((u32)(((x) & 0x00000001) << 15))
#define LSYS_GET_KM4TZ_RST_HALT(x)        ((u32)(((x >> 15) & 0x00000001)))
#define LSYS_BIT_KM4NS_RST_HALT           ((u32)0x00000001 << 14)           /*!< R/W 0x0   DD_SEC: s*/
#define LSYS_KM4NS_RST_HALT(x)            ((u32)(((x) & 0x00000001) << 14))
#define LSYS_GET_KM4NS_RST_HALT(x)        ((u32)(((x >> 14) & 0x00000001)))
#define LSYS_BIT_KM4TZ_SHARE_CACHE_MEM    ((u32)0x00000001 << 13)           /*!< R/W 0x0  0: km4tz cache memory used as cache 1: km4tz cache memory used as dtcm DD_SEC: s*/
#define LSYS_KM4TZ_SHARE_CACHE_MEM(x)     ((u32)(((x) & 0x00000001) << 13))
#define LSYS_GET_KM4TZ_SHARE_CACHE_MEM(x) ((u32)(((x >> 13) & 0x00000001)))
#define LSYS_BIT_KM4NS_SHARE_CACHE_MEM    ((u32)0x00000001 << 12)           /*!< R/W 0x0  0: km4ns cache memory used as cache 1: km4ns cache memory used as dtcm DD_SEC: s*/
#define LSYS_KM4NS_SHARE_CACHE_MEM(x)     ((u32)(((x) & 0x00000001) << 12))
#define LSYS_GET_KM4NS_SHARE_CACHE_MEM(x) ((u32)(((x >> 12) & 0x00000001)))
#define LSYS_BIT_KM4NS_DBGRESTARTED_M     ((u32)0x00000001 << 7)            /*!< R 0x0  km4ns debug restart status DD_SEC: s*/
#define LSYS_KM4NS_DBGRESTARTED_M(x)      ((u32)(((x) & 0x00000001) << 7))
#define LSYS_GET_KM4NS_DBGRESTARTED_M(x)  ((u32)(((x >> 7) & 0x00000001)))
#define LSYS_BIT_KM4NS_DBG_MODE           ((u32)0x00000001 << 6)            /*!< R 0x0  km4ns halt status DD_SEC: s*/
#define LSYS_KM4NS_DBG_MODE(x)            ((u32)(((x) & 0x00000001) << 6))
#define LSYS_GET_KM4NS_DBG_MODE(x)        ((u32)(((x >> 6) & 0x00000001)))
#define LSYS_BIT_KM4NS_LOCKUP_M           ((u32)0x00000001 << 5)            /*!< R 0x0  km4ns lockup status DD_SEC: s*/
#define LSYS_KM4NS_LOCKUP_M(x)            ((u32)(((x) & 0x00000001) << 5))
#define LSYS_GET_KM4NS_LOCKUP_M(x)        ((u32)(((x >> 5) & 0x00000001)))
#define LSYS_BIT_KM4NS_SLP_MODE           ((u32)0x00000001 << 4)            /*!< R 0x0  km4ns sleep status DD_SEC: s*/
#define LSYS_KM4NS_SLP_MODE(x)            ((u32)(((x) & 0x00000001) << 4))
#define LSYS_GET_KM4NS_SLP_MODE(x)        ((u32)(((x >> 4) & 0x00000001)))
#define LSYS_BIT_KM4TZ_DBGRESTARTED_M     ((u32)0x00000001 << 3)            /*!< R 0x0  km4tz debug restart status DD_SEC: s*/
#define LSYS_KM4TZ_DBGRESTARTED_M(x)      ((u32)(((x) & 0x00000001) << 3))
#define LSYS_GET_KM4TZ_DBGRESTARTED_M(x)  ((u32)(((x >> 3) & 0x00000001)))
#define LSYS_BIT_KM4TZ_DBG_MODE           ((u32)0x00000001 << 2)            /*!< R 0x0  km4tz halt status DD_SEC: s*/
#define LSYS_KM4TZ_DBG_MODE(x)            ((u32)(((x) & 0x00000001) << 2))
#define LSYS_GET_KM4TZ_DBG_MODE(x)        ((u32)(((x >> 2) & 0x00000001)))
#define LSYS_BIT_KM4TZ_LOCKUP_M           ((u32)0x00000001 << 1)            /*!< R 0x0  km4tz lockup status DD_SEC: s*/
#define LSYS_KM4TZ_LOCKUP_M(x)            ((u32)(((x) & 0x00000001) << 1))
#define LSYS_GET_KM4TZ_LOCKUP_M(x)        ((u32)(((x >> 1) & 0x00000001)))
#define LSYS_BIT_KM4TZ_SLP_MODE           ((u32)0x00000001 << 0)            /*!< R 0x0  km4tz sleep status DD_SEC: s*/
#define LSYS_KM4TZ_SLP_MODE(x)            ((u32)(((x) & 0x00000001) << 0))
#define LSYS_GET_KM4TZ_SLP_MODE(x)        ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_SYSTEM_CFG0
  * @brief
  * @{
  */
#define LSYS_BIT_ENG_MODE         ((u32)0x00000001 << 31)           /*!< R 0x0  Indicate PKG_ENG_SEL pad value 0: normal package; 1: engineering mode*/
#define LSYS_ENG_MODE(x)          ((u32)(((x) & 0x00000001) << 31))
#define LSYS_GET_ENG_MODE(x)      ((u32)(((x >> 31) & 0x00000001)))
#define LSYS_BIT_XTAL_VALID       ((u32)0x00000001 << 29)           /*!< R 0x0  Indicator xtal 40M is ready*/
#define LSYS_XTAL_VALID(x)        ((u32)(((x) & 0x00000001) << 29))
#define LSYS_GET_XTAL_VALID(x)    ((u32)(((x >> 29) & 0x00000001)))
#define LSYS_BIT_SYS_PLL_VALID    ((u32)0x00000001 << 28)           /*!< R 0x0  Indicator sys pll is ready*/
#define LSYS_SYS_PLL_VALID(x)     ((u32)(((x) & 0x00000001) << 28))
#define LSYS_GET_SYS_PLL_VALID(x) ((u32)(((x >> 28) & 0x00000001)))
#define LSYS_BIT_PTRP_BOOT_NOR    ((u32)0x00000001 << 25)           /*!< R 0x0  */
#define LSYS_PTRP_BOOT_NOR(x)     ((u32)(((x) & 0x00000001) << 25))
#define LSYS_GET_PTRP_BOOT_NOR(x) ((u32)(((x >> 25) & 0x00000001)))
#define LSYS_BIT_PTRP_BOOT_INT    ((u32)0x00000001 << 24)           /*!< R 0x0  */
#define LSYS_PTRP_BOOT_INT(x)     ((u32)(((x) & 0x00000001) << 24))
#define LSYS_GET_PTRP_BOOT_INT(x) ((u32)(((x >> 24) & 0x00000001)))
#define LSYS_MASK_PTRP_ICFG       ((u32)0x0000000F << 20)           /*!< R 0x0  Trapped ICFG data value 0: normall mode XXXX: the other mode (Test Mode)*/
#define LSYS_PTRP_ICFG(x)         (((u32)((x) & 0x0000000F) << 20))
#define LSYS_GET_PTRP_ICFG(x)     ((u32)(((x >> 20) & 0x0000000F)))
#define LSYS_BIT_PTRP_UARTDL      ((u32)0x00000001 << 19)           /*!< R 0x0  Power on trapped uart download value 0: Normal boot 1: uartdownload boot TODO: it may be overwrited by efuse autoload*/
#define LSYS_PTRP_UARTDL(x)       ((u32)(((x) & 0x00000001) << 19))
#define LSYS_GET_PTRP_UARTDL(x)   ((u32)(((x >> 19) & 0x00000001)))
#define LSYS_BIT_PTRP_TMODE       ((u32)0x00000001 << 17)           /*!< R 0x0  0: normal mode 1: test mode*/
#define LSYS_PTRP_TMODE(x)        ((u32)(((x) & 0x00000001) << 17))
#define LSYS_GET_PTRP_TMODE(x)    ((u32)(((x >> 17) & 0x00000001)))
#define LSYS_BIT_PTRP_OTPBYP      ((u32)0x00000001 << 16)           /*!< R 0x0  1: force efuse autoload fail*/
#define LSYS_PTRP_OTPBYP(x)       ((u32)(((x) & 0x00000001) << 16))
#define LSYS_GET_PTRP_OTPBYP(x)   ((u32)(((x >> 16) & 0x00000001)))
#define LSYS_MASK_HW_CPU_ID       ((u32)0x00000003 << 6)            /*!< R 0x0  Different CPU reading this register will return different value. If read by KR4: return 0 If read by KM4: return 1 Other will return 2*/
#define LSYS_HW_CPU_ID(x)         (((u32)((x) & 0x00000003) << 6))
#define LSYS_GET_HW_CPU_ID(x)     ((u32)(((x >> 6) & 0x00000003)))
/** @} */

/** @defgroup REG_LSYS_I2S_SDM_CTRL
  * @brief
  * @{
  */
#define LSYS_BIT_FEN_SDM_I2S0      ((u32)0x00000001 << 31)           /*!< R/W 0x0  fen enable*/
#define LSYS_FEN_SDM_I2S0(x)       ((u32)(((x) & 0x00000001) << 31))
#define LSYS_GET_FEN_SDM_I2S0(x)   ((u32)(((x >> 31) & 0x00000001)))
#define LSYS_BIT_COMP_SEL_I2S0     ((u32)0x00000001 << 30)           /*!< R/W 0x0  Determine to select /N+1  or /N-1 clock when interpolation. when pll /N-1 is closer to target_per ,please select 1. otherwise select 0.*/
#define LSYS_COMP_SEL_I2S0(x)      ((u32)(((x) & 0x00000001) << 30))
#define LSYS_GET_COMP_SEL_I2S0(x)  ((u32)(((x >> 30) & 0x00000001)))
#define LSYS_MASK_CKD_SDM_I2S0     ((u32)0x0000001F << 24)           /*!< R/W 5'd4  Set a divider to nearest the target frequency . The i2s sdm base divider factor N  is this value + 1*/
#define LSYS_CKD_SDM_I2S0(x)       (((u32)((x) & 0x0000001F) << 24))
#define LSYS_GET_CKD_SDM_I2S0(x)   ((u32)(((x >> 24) & 0x0000001F)))
#define LSYS_MASK_COMP_STEP_I2S0   ((u32)0x00FFFFFF << 0)            /*!< R/W 0x21FB72  i2s sdm clock interpolation period . This mains every this value's /N cycles ,  it will interpolate one /N+1 or /N-1 clock  to compensate period error. For example to generate 98.304M  from pll 480M (max precsion is 1K, step bit must > 480M/1K) 1. target is 98.304M, set i2s0_sdm_ckd = 4 (N=5 ) which is near target 2. set comp_sel is 1, for ckd is bigger than the fractional target (4.88) 2. Calcurate compensation normalized step :     target_per = 1000/98.304 = 10.172526ns     base_per    =  1000/(480/5)   = 10.4166666ns     intp_per     =  10000/(480/4)   = 8.3333333ns     err_accu = |target_per - base_per| = 0.24414     err_comp = |target_per - intp_per| = 1.8391927     normalizaiton = err_accu/err_comp = 0.13274302361..      Convert the decimal into hexadecimal. change format to unsigned Q0.24 , which is  floo( normalizaion * (1<<24))*/
#define LSYS_COMP_STEP_I2S0(x)     (((u32)((x) & 0x00FFFFFF) << 0))
#define LSYS_GET_COMP_STEP_I2S0(x) ((u32)(((x >> 0) & 0x00FFFFFF)))
/** @} */

/** @defgroup REG_LSYS_BT_32K_CTRL0
  * @brief
  * @{
  */
#define LSYS_BIT_BT_MLDO_REQ_TRIG    ((u32)0x00000001 << 31)           /*!< R/W 1'b0  level -> pulse. Write to 1 to update BT analog 1.8V LDO request when BT is active (load BT_mldo_mode_nm). need to write 0 before next request DD_SEC: bpc_bt*/
#define LSYS_BT_MLDO_REQ_TRIG(x)     ((u32)(((x) & 0x00000001) << 31))
#define LSYS_GET_BT_MLDO_REQ_TRIG(x) ((u32)(((x >> 31) & 0x00000001)))
#define LSYS_MASK_BT_MLDO_MODE_LPS   ((u32)0x00000003 << 26)           /*!< R/W 2'b00  mem ldo mode slep option  00:off 01:sleep 10:normal + dummy 11:normal + no dummy DD_SEC: bpc_bt*/
#define LSYS_BT_MLDO_MODE_LPS(x)     (((u32)((x) & 0x00000003) << 26))
#define LSYS_GET_BT_MLDO_MODE_LPS(x) ((u32)(((x >> 26) & 0x00000003)))
#define LSYS_MASK_BT_MLDO_MODE_NM    ((u32)0x00000003 << 24)           /*!< R/W 2'b10  mem ldo mode normal option  00:off 01:sleep 10:normal + dummy 11:normal + no dummy DD_SEC: bpc_bt*/
#define LSYS_BT_MLDO_MODE_NM(x)      (((u32)((x) & 0x00000003) << 24))
#define LSYS_GET_BT_MLDO_MODE_NM(x)  ((u32)(((x >> 24) & 0x00000003)))
#define LSYS_MASK_DWTIM_SRSTN_CHN    ((u32)0x0000007F << 0)            /*!< R/W 0x7F  sw reset for bt dw channel ,active 0 default all channel will release after dw timer function enable DD_SEC: bpc_bt*/
#define LSYS_DWTIM_SRSTN_CHN(x)      (((u32)((x) & 0x0000007F) << 0))
#define LSYS_GET_DWTIM_SRSTN_CHN(x)  ((u32)(((x >> 0) & 0x0000007F)))
/** @} */

/** @defgroup REG_LSYS_BT_KR4_CTRL
  * @brief
  * @{
  */
#define LSYS_BIT_BT_EN_PON_REQ_REGU    ((u32)0x00000001 << 26)           /*!< R/W 0x0  1: enable to change BT regu_sts request during BT active DD_SEC: bpc_bt*/
#define LSYS_BT_EN_PON_REQ_REGU(x)     ((u32)(((x) & 0x00000001) << 26))
#define LSYS_GET_BT_EN_PON_REQ_REGU(x) ((u32)(((x >> 26) & 0x00000001)))
#define LSYS_BIT_BT_EN_PWM_LOAD        ((u32)0x00000001 << 24)           /*!< R/W 0x1  1:when bt enter normal , it will enable pwm high loading option 0:pwm loading change effect by bt DD_SEC: bpc_bt*/
#define LSYS_BT_EN_PWM_LOAD(x)         ((u32)(((x) & 0x00000001) << 24))
#define LSYS_GET_BT_EN_PWM_LOAD(x)     ((u32)(((x >> 24) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_BT_CTRL0
  * @brief
  * @{
  */
#define LSYS_MASK_BT_SWR_STS_NM                     ((u32)0x00000007 << 29)           /*!< R/W 0x110  SET SWR state when BT in active mode 000: RSVD      001: state1  0.7/PC/PFM 010: state2  0.8/PC/PFM  011: state3  0.9/PC/PFM 100: state4  0.9/SLP/PFM   101: state5  0.9/SLP/PWM  110: state6  0.9/NORM/PWM  111: state7  1.0/NORM/PWM DD_SEC: bpc_bt*/
#define LSYS_BT_SWR_STS_NM(x)                       (((u32)((x) & 0x00000007) << 29))
#define LSYS_GET_BT_SWR_STS_NM(x)                   ((u32)(((x >> 29) & 0x00000007)))
#define LSYS_BIT_BTWL_USE_LOGUART_PAD_ST            ((u32)0x00000001 << 28)           /*!< R   The state of xor of WL_USE_REQ and BT_USE_REQ. 0: loguart use loguart trx pad 1: bt hci use loguart trx pad DD_SEC: bpc_bt*/
#define LSYS_BTWL_USE_LOGUART_PAD_ST(x)             ((u32)(((x) & 0x00000001) << 28))
#define LSYS_GET_BTWL_USE_LOGUART_PAD_ST(x)         ((u32)(((x >> 28) & 0x00000001)))
#define LSYS_BIT_WL_MP_STATUS                       ((u32)0x00000001 << 27)           /*!< R/W 0x0  Wi-Fi MP status DD_SEC: bpc_bt*/
#define LSYS_WL_MP_STATUS(x)                        ((u32)(((x) & 0x00000001) << 27))
#define LSYS_GET_WL_MP_STATUS(x)                    ((u32)(((x >> 27) & 0x00000001)))
#define LSYS_BIT_BT_USE_EXT_BT                      ((u32)0x00000001 << 26)           /*!< R/W 0x0  1: select external BT 0: select internal BT DD_SEC: bpc_bt*/
#define LSYS_BT_USE_EXT_BT(x)                       ((u32)(((x) & 0x00000001) << 26))
#define LSYS_GET_BT_USE_EXT_BT(x)                   ((u32)(((x >> 26) & 0x00000001)))
#define LSYS_BIT_WL_USE_REQ                         ((u32)0x00000001 << 25)           /*!< R/W 0x0  who uses loguart  trx pad depends on the xor of WL_USE_REQ and BT_USE_REQ, wifi can send use request by change the value of WL_USE_REQ DD_SEC: bpc_bt*/
#define LSYS_WL_USE_REQ(x)                          ((u32)(((x) & 0x00000001) << 25))
#define LSYS_GET_WL_USE_REQ(x)                      ((u32)(((x >> 25) & 0x00000001)))
#define LSYS_BIT_FORCE_LOGUART_USE_LOGUART_PAD_B    ((u32)0x00000001 << 24)           /*!< R/W 0x0  Force wifi soc to use loguart pad mandatory if necessary 0: loguart use loguart trx pad 1: who uses loguart trx pad depends on BTWL_USE_LOGUART_PAD_ST DD_SEC: bpc_bt*/
#define LSYS_FORCE_LOGUART_USE_LOGUART_PAD_B(x)     ((u32)(((x) & 0x00000001) << 24))
#define LSYS_GET_FORCE_LOGUART_USE_LOGUART_PAD_B(x) ((u32)(((x >> 24) & 0x00000001)))
#define LSYS_MASK_BT_XTAL_MODE_NM                   ((u32)0x00000007 << 21)           /*!< R/W 0x100  SET XTAL state when BT in active mode 000: XTAL OFF 001: XTAL LPS, gating XTAL 40M 010: XTAL LPS, enable XTAL 40M 011: XTAL NM mode 100: XTAL HP mode DD_SEC: bpc_bt*/
#define LSYS_BT_XTAL_MODE_NM(x)                     (((u32)((x) & 0x00000007) << 21))
#define LSYS_GET_BT_XTAL_MODE_NM(x)                 ((u32)(((x >> 21) & 0x00000007)))
#define LSYS_BIT_BT_EN_TST_SI_CTRL_RFC              ((u32)0x00000001 << 20)           /*!< R/W 0x1  进入Test Mode 8 , 且同時 BT_EN_TST_SI_CTRL_RFC=1 , 才將RFC 端的SI  3 wire交由專線BT_SI_*_TST 控制; 若I2C 想要access RFC,设置BT_EN_TST_SI_CTRL_RFC=0 或者 將BT_SI_ENB_TST pull high(如果BT_SI_ENB_TST 有出 PIN) DD_SEC: bpc_bt*/
#define LSYS_BT_EN_TST_SI_CTRL_RFC(x)               ((u32)(((x) & 0x00000001) << 20))
#define LSYS_GET_BT_EN_TST_SI_CTRL_RFC(x)           ((u32)(((x >> 20) & 0x00000001)))
#define LSYS_BIT_BT_USE_EXT_UART                    ((u32)0x00000001 << 19)           /*!< R/W 0x0  1: BT use external uart 0: BT use internal uart DD_SEC: bpc_bt*/
#define LSYS_BT_USE_EXT_UART(x)                     ((u32)(((x) & 0x00000001) << 19))
#define LSYS_GET_BT_USE_EXT_UART(x)                 ((u32)(((x >> 19) & 0x00000001)))
#define LSYS_BIT_BT_CTRL_RFC_SEL_SI                 ((u32)0x00000001 << 18)           /*!< R/W 0x0  Normal mode 或者Test Mode 2,通过此寄存器选择RFC的控制端. BT_CTRL_RFC_SEL_SI = 1,將RFC 端的SI  3 wire交由專線BT_SI_*_TST 控制;BT_CTRL_RFC_SEL_SI = 0,將RFC 端的SI  3 wire交由 I2C 控制. BT_CTRL_RFC_SEL_SI = 1时,也可通过将BT_SI_ENB_TST pull high,交由PI控制 DD_SEC: bpc_bt*/
#define LSYS_BT_CTRL_RFC_SEL_SI(x)                  ((u32)(((x) & 0x00000001) << 18))
#define LSYS_GET_BT_CTRL_RFC_SEL_SI(x)              ((u32)(((x >> 18) & 0x00000001)))
#define LSYS_BIT_BOOT_RSVD_BT_SW1                   ((u32)0x00000001 << 17)           /*!< R/W 0x0  RSVD for sotfware DD_SEC: bpc_bt*/
#define LSYS_BOOT_RSVD_BT_SW1(x)                    ((u32)(((x) & 0x00000001) << 17))
#define LSYS_GET_BOOT_RSVD_BT_SW1(x)                ((u32)(((x >> 17) & 0x00000001)))
#define LSYS_BIT_BOOT_RSVD_BT_SW2                   ((u32)0x00000001 << 16)           /*!< R/W 0x0  RSVD for sotfware DD_SEC: bpc_bt*/
#define LSYS_BOOT_RSVD_BT_SW2(x)                    ((u32)(((x) & 0x00000001) << 16))
#define LSYS_GET_BOOT_RSVD_BT_SW2(x)                ((u32)(((x >> 16) & 0x00000001)))
#define LSYS_MASK_BT_XTAL_MODE_LPS                  ((u32)0x00000007 << 10)           /*!< R/W 0x1  SET XTAL state when BT in LPS mode 000: XTAL OFF 001: XTAL LPS, gating XTAL 40M 010: XTAL LPS, enable XTAL 40M 011: XTAL NM mode 100: XTAL HP mode DD_SEC: bpc_bt*/
#define LSYS_BT_XTAL_MODE_LPS(x)                    (((u32)((x) & 0x00000007) << 10))
#define LSYS_GET_BT_XTAL_MODE_LPS(x)                ((u32)(((x >> 10) & 0x00000007)))
#define LSYS_MASK_BT_MEM_PWRC_LPS                   ((u32)0x00000007 << 6)            /*!< R/W 3'b010  BT DMEM/BFMEM power mode control in SLEEP. Setted before chip goes to SLEEP.Used to controll power mode of BT memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}.   000 - active   001 - light sleep   010 - deep sleep   100 - shut down DD_SEC: bpc_bt*/
#define LSYS_BT_MEM_PWRC_LPS(x)                     (((u32)((x) & 0x00000007) << 6))
#define LSYS_GET_BT_MEM_PWRC_LPS(x)                 ((u32)(((x >> 6) & 0x00000007)))
#define LSYS_BIT_BT_SYS_BTDBG_SEL                   ((u32)0x00000001 << 4)            /*!< R/W 0x0  0: select debug port from BTTOP 1: debug_port[31:18] from bt_misc; else from BTTOP DD_SEC: bpc_bt*/
#define LSYS_BT_SYS_BTDBG_SEL(x)                    ((u32)(((x) & 0x00000001) << 4))
#define LSYS_GET_BT_SYS_BTDBG_SEL(x)                ((u32)(((x >> 4) & 0x00000001)))
#define LSYS_BIT_DSWR_REF_BT                        ((u32)0x00000001 << 3)            /*!< R/W 0x1  core swr parameter refer BT status DD_SEC: bpc_bt*/
#define LSYS_DSWR_REF_BT(x)                         ((u32)(((x) & 0x00000001) << 3))
#define LSYS_GET_DSWR_REF_BT(x)                     ((u32)(((x >> 3) & 0x00000001)))
#define LSYS_MASK_BT_SWR_STS_LPS                    ((u32)0x00000007 << 0)            /*!< R/W 0x010  SET SWR state when BT in LPS mode 000: RSVD      001: state1  0.7/PC/PFM 010: state2  0.8/PC/PFM  011: state3  0.9/PC/PFM 100: state4  0.9/SLP/PFM   101: state5  0.9/SLP/PWM  110: state6  0.9/NORM/PWM  111: state7  1.0/NORM/PWM DD_SEC: bpc_bt*/
#define LSYS_BT_SWR_STS_LPS(x)                      (((u32)((x) & 0x00000007) << 0))
#define LSYS_GET_BT_SWR_STS_LPS(x)                  ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_LSYS_BT_CTRL1
  * @brief
  * @{
  */
#define LSYS_BIT_PON_SDN              ((u32)0x00000001 << 31)           /*!< R/W 0x0  1: SW enable SDN DD_SEC: bpc_bt*/
#define LSYS_PON_SDN(x)               ((u32)(((x) & 0x00000001) << 31))
#define LSYS_GET_PON_SDN(x)           ((u32)(((x >> 31) & 0x00000001)))
#define LSYS_BIT_POF_SDN              ((u32)0x00000001 << 30)           /*!< R/W 0x0  1: SW disable SDN DD_SEC: bpc_bt*/
#define LSYS_POF_SDN(x)               ((u32)(((x) & 0x00000001) << 30))
#define LSYS_GET_POF_SDN(x)           ((u32)(((x >> 30) & 0x00000001)))
#define LSYS_BIT_LPS_SDN              ((u32)0x00000001 << 29)           /*!< R/W 0x0  1: SW trigger SDN to enter LPS DD_SEC: bpc_bt*/
#define LSYS_LPS_SDN(x)               ((u32)(((x) & 0x00000001) << 29))
#define LSYS_GET_LPS_SDN(x)           ((u32)(((x >> 29) & 0x00000001)))
#define LSYS_BIT_SDN_PON_WAIT_ALL     ((u32)0x00000001 << 28)           /*!< R/W 0x0  for speed up, wait swr_rdy, xtal_rdy and pwc_ready DD_SEC: bpc_bt*/
#define LSYS_SDN_PON_WAIT_ALL(x)      ((u32)(((x) & 0x00000001) << 28))
#define LSYS_GET_SDN_PON_WAIT_ALL(x)  ((u32)(((x >> 28) & 0x00000001)))
#define LSYS_BIT_SDN_POF_WAIT_ALL     ((u32)0x00000001 << 27)           /*!< R/W 0x0  for speed up, wait swr_rdy, xtal_rdy and pwc_ready DD_SEC: bpc_bt*/
#define LSYS_SDN_POF_WAIT_ALL(x)      ((u32)(((x) & 0x00000001) << 27))
#define LSYS_GET_SDN_POF_WAIT_ALL(x)  ((u32)(((x >> 27) & 0x00000001)))
#define LSYS_BIT_SDN_ENABLE_LPS_PG    ((u32)0x00000001 << 26)           /*!< R/W 0x0  1: enable LPS PG DD_SEC: bpc_bt*/
#define LSYS_SDN_ENABLE_LPS_PG(x)     ((u32)(((x) & 0x00000001) << 26))
#define LSYS_GET_SDN_ENABLE_LPS_PG(x) ((u32)(((x >> 26) & 0x00000001)))
#define LSYS_BIT_SDN_LPS_CLK_SW       ((u32)0x00000001 << 25)           /*!< R/W 0x1  1: enable clk_tsf auto switch when enter/quit LPS (clk_40m and clk_32k) and gated 40M 0: disable switch clk_tsf, need modify lps_xtal_mode DD_SEC: bpc_bt*/
#define LSYS_SDN_LPS_CLK_SW(x)        ((u32)(((x) & 0x00000001) << 25))
#define LSYS_GET_SDN_LPS_CLK_SW(x)    ((u32)(((x >> 25) & 0x00000001)))
#define LSYS_BIT_BT_CKSL_CKANA        ((u32)0x00000001 << 15)           /*!< R/W 0x1  1: enable osc4m when BT enter LPS 0: switch bt_ana_clk from osc4m to ana_100k when BT enter LPS DD_SEC: bpc_bt*/
#define LSYS_BT_CKSL_CKANA(x)         ((u32)(((x) & 0x00000001) << 15))
#define LSYS_GET_BT_CKSL_CKANA(x)     ((u32)(((x >> 15) & 0x00000001)))
#define LSYS_MASK_BT_PMC_STATE        ((u32)0x00007FFF << 0)            /*!< R 0x0  indicate bt_misc PMC current state, for debug DD_SEC: bpc_bt*/
#define LSYS_BT_PMC_STATE(x)          (((u32)((x) & 0x00007FFF) << 0))
#define LSYS_GET_BT_PMC_STATE(x)      ((u32)(((x >> 0) & 0x00007FFF)))
/** @} */

/** @defgroup REG_LSYS_DW_GPIO_CTRL
  * @brief
  * @{
  */
#define LSYS_BIT_GPIO_REG_OPT      ((u32)0x00000001 << 31)           /*!< R/W 0x1  gpio data out/oe register function option 0: DR/DDR reg function are same as DW GPIO spec 1: DR/DDR reg  func changes , 0:not toggle , 1:toggle current value DD_SEC: bpc_gpio*/
#define LSYS_GPIO_REG_OPT(x)       ((u32)(((x) & 0x00000001) << 31))
#define LSYS_GET_GPIO_REG_OPT(x)   ((u32)(((x >> 31) & 0x00000001)))
#define LSYS_MASK_GPIO_DIVCNT_PC   ((u32)0x0000007F << 16)           /*!< R/W 0x0  divider for dw gpioc debounce clk DD_SEC: bpc_gpio*/
#define LSYS_GPIO_DIVCNT_PC(x)     (((u32)((x) & 0x0000007F) << 16))
#define LSYS_GET_GPIO_DIVCNT_PC(x) ((u32)(((x >> 16) & 0x0000007F)))
#define LSYS_MASK_GPIO_DIVCNT_PB   ((u32)0x0000007F << 8)            /*!< R/W 0x0  divider for dw gpiob debounce clk DD_SEC: bpc_gpio*/
#define LSYS_GPIO_DIVCNT_PB(x)     (((u32)((x) & 0x0000007F) << 8))
#define LSYS_GET_GPIO_DIVCNT_PB(x) ((u32)(((x >> 8) & 0x0000007F)))
#define LSYS_MASK_GPIO_DIVCNT_PA   ((u32)0x0000007F << 0)            /*!< R/W 0x0  divider for dw gpioa debounce clk DD_SEC: bpc_gpio*/
#define LSYS_GPIO_DIVCNT_PA(x)     (((u32)((x) & 0x0000007F) << 0))
#define LSYS_GET_GPIO_DIVCNT_PA(x) ((u32)(((x >> 0) & 0x0000007F)))
/** @} */

/** @defgroup REG_LSYS_SDIO_CTRL
  * @brief
  * @{
  */
#define LSYS_BIT_SDIO_BT_EN             ((u32)0x00000001 << 8)           /*!< R/W 0x1  enable sdio bt function(fn2) DD_SEC: bpc_sdio*/
#define LSYS_SDIO_BT_EN(x)              ((u32)(((x) & 0x00000001) << 8))
#define LSYS_GET_SDIO_BT_EN(x)          ((u32)(((x >> 8) & 0x00000001)))
#define LSYS_BIT_SDD_UPDATE_FW          ((u32)0x00000001 << 7)           /*!< R/WE 0x1  enable updating firmware via sdio DD_SEC: bpc_sdio*/
#define LSYS_SDD_UPDATE_FW(x)           ((u32)(((x) & 0x00000001) << 7))
#define LSYS_GET_SDD_UPDATE_FW(x)       ((u32)(((x >> 7) & 0x00000001)))
#define LSYS_BIT_SDD_AHBM_NONSEC        ((u32)0x00000001 << 6)           /*!< R/WH 0x0  sdio wifi ahb mster 0: sec, 1: non-sec, DD_SEC: bpc_sdio*/
#define LSYS_SDD_AHBM_NONSEC(x)         ((u32)(((x) & 0x00000001) << 6))
#define LSYS_GET_SDD_AHBM_NONSEC(x)     ((u32)(((x >> 6) & 0x00000001)))
#define LSYS_BIT_SDD_FN2_AHBM_NONSEC    ((u32)0x00000001 << 5)           /*!< R/WH 0x0  sdio bt ahb mster 0: sec, 1: non-sec, DD_SEC: bpc_sdio*/
#define LSYS_SDD_FN2_AHBM_NONSEC(x)     ((u32)(((x) & 0x00000001) << 5))
#define LSYS_GET_SDD_FN2_AHBM_NONSEC(x) ((u32)(((x >> 5) & 0x00000001)))
#define LSYS_BIT_SDD_INT_SO_POL         ((u32)0x00000001 << 1)           /*!< R/W 0x0  polarity control for sd_int_so(sideband intr signal) DD_SEC: bpc_sdio*/
#define LSYS_SDD_INT_SO_POL(x)          ((u32)(((x) & 0x00000001) << 1))
#define LSYS_GET_SDD_INT_SO_POL(x)      ((u32)(((x >> 1) & 0x00000001)))
#define LSYS_BIT_SDD_PMUX_FEN           ((u32)0x00000001 << 0)           /*!< R/W 0x0  after sdio pinmux is configured, set this bit to 1 DD_SEC: bpc_sdio*/
#define LSYS_SDD_PMUX_FEN(x)            ((u32)(((x) & 0x00000001) << 0))
#define LSYS_GET_SDD_PMUX_FEN(x)        ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_SIC_DBNC
  * @brief
  * @{
  */
#define LSYS_MASK_SIC_DBNC_CYC   ((u32)0x0000001F << 4)           /*!< R/W 5'd2  SIC clk/data input debounce cycle , base on 4M clock*/
#define LSYS_SIC_DBNC_CYC(x)     (((u32)((x) & 0x0000001F) << 4))
#define LSYS_GET_SIC_DBNC_CYC(x) ((u32)(((x >> 4) & 0x0000001F)))
#define LSYS_BIT_SIC_DBNC_FEN    ((u32)0x00000001 << 0)           /*!< R/W 0x0  SIC clk/data input debounce enable*/
#define LSYS_SIC_DBNC_FEN(x)     ((u32)(((x) & 0x00000001) << 0))
#define LSYS_GET_SIC_DBNC_FEN(x) ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_SPIC_CTRL
  * @brief
  * @{
  */
#define LSYS_BIT_FLASH_PG_DO_CTRL     ((u32)0x00000001 << 24)           /*!< R/W 0x0  1: disable spic do output enable DD_SEC: S*/
#define LSYS_FLASH_PG_DO_CTRL(x)      ((u32)(((x) & 0x00000001) << 24))
#define LSYS_GET_FLASH_PG_DO_CTRL(x)  ((u32)(((x >> 24) & 0x00000001)))
#define LSYS_MASK_DPHY_RD_DELAY_CNT   ((u32)0x0000000F << 17)           /*!< R/W 0x0  spic_dphy output dealy cnt, when set < 3, output data will occur at fourth or fifth axi clock from the rising edge of spi_rx_data_en; when set >= 3, data  will occur after  DPHY_RD_DELAY_CNT + 2 axi clock from the rising edge of spi_rx_data_en DD_SEC: S*/
#define LSYS_DPHY_RD_DELAY_CNT(x)     (((u32)((x) & 0x0000000F) << 17))
#define LSYS_GET_DPHY_RD_DELAY_CNT(x) ((u32)(((x >> 17) & 0x0000000F)))
#define LSYS_BIT_DPHY_EN              ((u32)0x00000001 << 16)           /*!< R/W 0x0  when set, enable spic_dphy function to read data from flash DD_SEC: S*/
#define LSYS_DPHY_EN(x)               ((u32)(((x) & 0x00000001) << 16))
#define LSYS_GET_DPHY_EN(x)           ((u32)(((x >> 16) & 0x00000001)))
#define LSYS_BIT_FLASH_CAL_EN         ((u32)0x00000001 << 15)           /*!< R/W 0x0  Flash calibration enable 1: delay flash sout for calibration 0: bypass flash sout to spic DD_SEC: S*/
#define LSYS_FLASH_CAL_EN(x)          ((u32)(((x) & 0x00000001) << 15))
#define LSYS_GET_FLASH_CAL_EN(x)      ((u32)(((x >> 15) & 0x00000001)))
#define LSYS_BIT_ECC_ICG_DISABLE      ((u32)0x00000001 << 12)           /*!< R/W 1h  Disable NAND ECC auto icg to save power 1:disable , no gating 0:enable DD_SEC: S*/
#define LSYS_ECC_ICG_DISABLE(x)       ((u32)(((x) & 0x00000001) << 12))
#define LSYS_GET_ECC_ICG_DISABLE(x)   ((u32)(((x >> 12) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_PSRAMC_FLASH_CTRL
  * @brief
  * @{
  */
#define LSYS_BIT_FLASH_FEEDBACK_LOC          ((u32)0x00000001 << 31)           /*!< R/W 0x0  0: pinmux S0 -> flash_spic, pinmux S1 -> psram_spic(if psramc_flash_en) 1: pinmux S1 -> flash_spic, pinmux S0 -> psram_spic(if psramc_flash_en) Note: S0 mean MCM group(default boot), S1 mean External group DD_SEC: S*/
#define LSYS_FLASH_FEEDBACK_LOC(x)           ((u32)(((x) & 0x00000001) << 31))
#define LSYS_GET_FLASH_FEEDBACK_LOC(x)       ((u32)(((x >> 31) & 0x00000001)))
#define LSYS_BIT_PSRAMC_FLASH_EN             ((u32)0x00000001 << 30)           /*!< R/W 0x0  use psram_spic as flash controller, disable psram_phy DD_SEC: S*/
#define LSYS_PSRAMC_FLASH_EN(x)              ((u32)(((x) & 0x00000001) << 30))
#define LSYS_GET_PSRAMC_FLASH_EN(x)          ((u32)(((x >> 30) & 0x00000001)))
#define LSYS_BIT_PSRAM_FRQC_EN               ((u32)0x00000001 << 27)           /*!< R/W 0x0  enable psram auto freq change control flow DD_SEC: bpc_pmc*/
#define LSYS_PSRAM_FRQC_EN(x)                ((u32)(((x) & 0x00000001) << 27))
#define LSYS_GET_PSRAM_FRQC_EN(x)            ((u32)(((x >> 27) & 0x00000001)))
#define LSYS_BIT_PSRAM_FRQC_DONE             ((u32)0x00000001 << 26)           /*!< R 0x0  indicate psram auto freq change is done DD_SEC: bpc_pmc*/
#define LSYS_PSRAM_FRQC_DONE(x)              ((u32)(((x) & 0x00000001) << 26))
#define LSYS_GET_PSRAM_FRQC_DONE(x)          ((u32)(((x >> 26) & 0x00000001)))
#define LSYS_MASK_PSRAM_FRQC_GRP             ((u32)0x00000003 << 24)           /*!< R/W 0x0  select psram auto freq change group DD_SEC: bpc_pmc*/
#define LSYS_PSRAM_FRQC_GRP(x)               (((u32)((x) & 0x00000003) << 24))
#define LSYS_GET_PSRAM_FRQC_GRP(x)           ((u32)(((x >> 24) & 0x00000003)))
#define LSYS_MASK_PSRAMC_DPHY_RD_DELAY_CNT   ((u32)0x0000000F << 17)           /*!< R/W 0x0  spic_dphy output dealy cnt, when set < 3, output data will occur at fourth or fifth axi clock from the rising edge of spi_rx_data_en; when set >= 3, data  will occur after  DPHY_RD_DELAY_CNT + 2 axi clock from the rising edge of spi_rx_data_en DD_SEC: S*/
#define LSYS_PSRAMC_DPHY_RD_DELAY_CNT(x)     (((u32)((x) & 0x0000000F) << 17))
#define LSYS_GET_PSRAMC_DPHY_RD_DELAY_CNT(x) ((u32)(((x >> 17) & 0x0000000F)))
#define LSYS_BIT_PSRAMC_DPHY_EN              ((u32)0x00000001 << 16)           /*!< R/W 0x0  when set, enable spic_dphy function to read data from flash DD_SEC: S*/
#define LSYS_PSRAMC_DPHY_EN(x)               ((u32)(((x) & 0x00000001) << 16))
#define LSYS_GET_PSRAMC_DPHY_EN(x)           ((u32)(((x >> 16) & 0x00000001)))
#define LSYS_BIT_PSRAMC_FLASH_CAL_EN         ((u32)0x00000001 << 15)           /*!< R/W 0x0  Flash calibration enable 1: delay flash sout for calibration 0: bypass flash sout to spic DD_SEC: S*/
#define LSYS_PSRAMC_FLASH_CAL_EN(x)          ((u32)(((x) & 0x00000001) << 15))
#define LSYS_GET_PSRAMC_FLASH_CAL_EN(x)      ((u32)(((x >> 15) & 0x00000001)))
/** @} */

/** @defgroup REG_LSYS_FPGA_SET
  * @brief
  * @{
  */
#define LSYS_MASK_FPGA_SET   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  USED for FPGA*/
#define LSYS_FPGA_SET(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define LSYS_GET_FPGA_SET(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_LSYS_DUMMY_10
  * @brief
  * @{
  */
#define LSYS_BIT_OTPC_DSLP_TRIG         ((u32)0x00000001 << 4)           /*!< WA0 0x0  It can trig OTP to accelerate power-off.*/
#define LSYS_OTPC_DSLP_TRIG(x)          ((u32)(((x) & 0x00000001) << 4))
#define LSYS_GET_OTPC_DSLP_TRIG(x)      ((u32)(((x >> 4) & 0x00000001)))
#define LSYS_BIT_CP_GPIOB_MAP           ((u32)0x00000001 << 2)           /*!< R/W 0x0  Only used when CP test , if 1 ,  PB[4:0] will use PA gpio controller for inout testing.*/
#define LSYS_CP_GPIOB_MAP(x)            ((u32)(((x) & 0x00000001) << 2))
#define LSYS_GET_CP_GPIOB_MAP(x)        ((u32)(((x >> 2) & 0x00000001)))
#define LSYS_BIT_DBG_SCAN_BYP           ((u32)0x00000001 << 1)           /*!< R 0x0  Used for scan byp point connect to scan chain. HW used only*/
#define LSYS_DBG_SCAN_BYP(x)            ((u32)(((x) & 0x00000001) << 1))
#define LSYS_GET_DBG_SCAN_BYP(x)        ((u32)(((x >> 1) & 0x00000001)))
#define LSYS_BIT_PLFM1_RESET_CONFIRM    ((u32)0x00000001 << 0)           /*!< R/W 0x0  cpu0 write this bit to 1 to confirm plfm1 reset enable.*/
#define LSYS_PLFM1_RESET_CONFIRM(x)     ((u32)(((x) & 0x00000001) << 0))
#define LSYS_GET_PLFM1_RESET_CONFIRM(x) ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/*==========LSYS Register Address Definition==========*/
#define REG_LSYS_PWC                        0x200
#define REG_LSYS_ISO                        0x202
#define REG_LSYS_FEN_GRP0                   0x204
#define REG_LSYS_FEN_GRP1                   0x208
#define REG_LSYS_CKE_GRP0                   0x210
#define REG_LSYS_CKE_GRP1                   0x214
#define REG_LSYS_CKE_GRP2                   0x218
#define REG_LSYS_CKSL_GRP0                  0x220
#define REG_LSYS_CKSL_GRP1                  0x224
#define REG_LSYS_CKD_GRP0                   0x228
#define REG_LSYS_CKD_TIMER_GRP1             0x234
#define REG_LSYS_CKD_FEN_XTAL               0x240
#define REG_LSYS_CKD_FEN_SYS_PLL            0x244
#define REG_LSYS_FEN_USB_PLL                0x248
#define REG_LSYS_CKD_SYS_PLL_GRP0           0x250
#define REG_LSYS_CKD_USB_PLL_GRP0           0x258
#define REG_LSYS_SW_RST_CTRL                0x260
#define REG_LSYS_SW_RST_TRIG                0x264
#define REG_LSYS_BOOT_REASON_SW             0x268
#define REG_LSYS_BOOT_CFG                   0x26C
#define REG_LSYS_SW_PATCH                   0x270
#define REG_LSYS_CHIP_INFO                  0x274
#define REG_LSYS_SW_SWD_CTRL                0x278
#define REG_LSYS_FPGA_CLK_INFO              0x27C
#define REG_LSYS_KM4_XO_CTRL0               0x280
#define REG_LSYS_KM4_XO_CTRL1               0x284
#define REG_LSYS_SEC_BPC_CTRL               0x290
#define REG_LSYS_SEC_PPC_CTRL               0x294
#define REG_LSYS_CPU0_PC                    0x298
#define REG_LSYS_CPU1_PC                    0x29C
#define REG_LSYS_PLAT_CTRL                  0x2A0
#define REG_LSYS_PLAT_STATUS                0x2A4
#define REG_LSYS_SYSTEM_CFG0                0x2A8
#define REG_LSYS_I2S_SDM_CTRL               0x2AC
#define REG_LSYS_BT_32K_CTRL0               0x2B0
#define REG_LSYS_BT_KR4_CTRL                0x2B4
#define REG_LSYS_BT_CTRL0                   0x2B8
#define REG_LSYS_BT_CTRL1                   0x2BC
#define REG_LSYS_DW_GPIO_CTRL               0x2C4
#define REG_LSYS_SDIO_CTRL                  0x2C8
#define REG_LSYS_SIC_DBNC                   0x2CC
#define REG_LSYS_SPIC_CTRL                  0x2D0
#define REG_LSYS_PSRAMC_FLASH_CTRL          0x2D4
#define REG_LSYS_FPGA_SET                   0x2D8
#define REG_LSYS_SOFT_DUMMY                 0x2DC
#define REG_LSYS_DUMMY_P1                   0x2EC
#define REG_LSYS_BOOT_ADDR_KM4TZ            0x2F0
#define REG_LSYS_BOOT_ADDR_KM4NS            0x2F4
#define REG_LSYS_DUMMY_09                   0x2F8
#define REG_LSYS_DUMMY_10                   0x2FC

/** @} */


/**
* @brief  Set Periph Clock Source
* Supported Periph_Name and Clk_Source listed as flows:
*
* Periph_Name: Clk_Source_1 / Clk_Source_2 / Clk_Source_3
* -------------------------------------------------------------
* ADC4M: OSC4M / XTAL
* ADC: BUS_CLK / IP_CLK
* BT_32K: OSC131K / XTAL312P5K
* DTIM: SDM32K / XTAL
* GPIO4M: SDM32K / OSC2M
* GPIO: BUS_CLK / IP_CLK
* HP: XTAL / SYS_PLL / USB_PLL
* HPERI: XTAL / SYS_PLL / USB_PLL
* IRDA: XTAL / SYS_PLL / USB_PLL
* LOGUART: OSC4M / XTAL
* LPERI: OSC4M / XTAL_40M
* LTIM0: SDM32K / XTAL
* LTIM1: SDM32K / XTAL
* LTIM2: SDM32K / XTAL
* LTIM3: SDM32K / XTAL
* LTIM4: SDM32K / XTAL
* LTIM5: SDM32K / XTAL
* OTPC: OSC4M / XTAL
* PSRAM: XTAL / SYS_PLL / USB_PLL
* SPIC: XTAL / SYS_PLL / USB_PLL
* SPORT: XTAL_40M / PLL_SDM_OUTPUT
* SYS_32K: OSC131K / XTAL312P5K
* UART0: OSC2M / XTAL
* UART1: OSC2M / XTAL
* UART2: OSC2M / XTAL
* -------------------------------------------------------------
*
* @param  Periph_Name:
* @param  Clk_Source:
*/
#define RCC_PeriphClockSourceSet(Periph_Name, Clk_Source) do {          \
    u32 Temp = 0;                                                       \
    Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_##Periph_Name);   \
    Temp &= ~(LSYS_MASK_CKSL_##Periph_Name);                             \
    Temp |= LSYS_CKSL_##Periph_Name(CKSL_##Periph_Name##_##Clk_Source); \
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_##Periph_Name, Temp);   \
} while(0)

/**
* @brief  Get Periph Clock Source
* Supported Periph_Name listed as flows
*
* Periph_Name
* -------------------------------------------------------------
* ADC4M / ADC / BT_32K / DTIM / GPIO4M / GPIO / HP / HPERI
* IRDA / LOGUART / LPERI / LTIM0 / LTIM1 / LTIM2 / LTIM3 / LTIM4
* LTIM5 / OTPC / PSRAM / SPIC / SPORT / SYS_32K / UART0 / UART1
* UART2
* -------------------------------------------------------------
*
* @param  Periph_Name: valid option listed above
*/
#define RCC_PeriphClockSourceGet(Periph_Name) \
	(LSYS_GET_CKSL_##Periph_Name(HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_##Periph_Name)))

/**
* @brief  Set Periph Clock Divider
* Supported Div_Name and supported Div listed as flows:
*
*  Div_Name: min_div - max_div
* -------------------------------------------------------------
* SYS_PLL_HP: 2 - 16
* SYS_PLL_HPERI: 2 - 16
* SYS_PLL_PSRAM: 2 - 16
* SYS_PLL_SPIC: 2 - 16
* USB_PLL_HP: 1 - 16
* USB_PLL_HPERI: 10
* USB_PLL_PSRAM: 1 - 16
* USB_PLL_SPIC: 1 - 16
* XTAL_ADC: 1
* XTAL_DTIM: 40
* XTAL_LTIM: 2 - 64
* -------------------------------------------------------------
* @param  Div_Name: As listed in the table above, brief "Name" at group REG_LSYS_CKD_XXX
* @param Div: 1->div1 or not support
*              2->div2
*              n->divn
*/
#define RCC_PeriphClockDividerSet(Div_Name, Div) do {               \
    u32 Temp = 0;                                                   \
    Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_##Div_Name);   \
    Temp &= ~(LSYS_MASK_CKD_##Div_Name);                            \
    Temp |= LSYS_CKD_##Div_Name(Div - 1);                           \
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_##Div_Name, Temp);   \
} while(0)

/**
* @brief  Set Periph Clock Divider
* Supported Div_Name and supported Div listed as flows:
*
*  Div_Name: min_div - max_div
* -------------------------------------------------------------
* SYS_PLL_HP / SYS_PLL_HPERI / SYS_PLL_PSRAM / SYS_PLL_SPIC / USB_PLL_HP / USB_PLL_HPERI / USB_PLL_PSRAM / USB_PLL_SPIC
* XTAL_ADC / XTAL_DTIM / XTAL_LTIM
* -------------------------------------------------------------
* @param  Div_Name: As listed in the table above, brief "Name" at group REG_LSYS_CKD_XXX
*/
#define RCC_PeriphClockDividerGet(Div_Name) \
	(LSYS_GET_CKD_##Div_Name(HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_##Div_Name)) + 1)

/**
* @brief  Enable / Disable Periph Clock Divider
* Supported DivFen_Name listed as flows:
* -------------------------------------------------------------
* SYS_PLL_HP / SYS_PLL_HPERI / SYS_PLL_PSRAM / SYS_PLL_SPIC / XTAL_ADC / XTAL_DTIM / XTAL_LTIM
* -------------------------------------------------------------
* @param  DivFen_Name: As listed in the table above, brief "Name" at group REG_LSYS_CKD_FEN_XXX
* @param  IS_ENABLE: 1 enable
* 					  0 disable
*/
#define RCC_PeriphClockDividerFENSet(DivFen_Name, IS_ENABLE) do {          \
    u32 Temp = 0;                                                          \
    Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_FEN_##DivFen_Name);   \
    Temp &= ~(LSYS_BIT_FEN_##DivFen_Name);                                 \
    Temp |= LSYS_FEN_##DivFen_Name(IS_ENABLE);                             \
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_FEN_##DivFen_Name, Temp);   \
} while(0)

/**
* @brief  Get Periph Clock Divider status
* Supported DivFen_Name listed as flows:
* -------------------------------------------------------------
* SYS_PLL_HP / SYS_PLL_HPERI / SYS_PLL_PSRAM / SYS_PLL_SPIC / XTAL_ADC / XTAL_DTIM / XTAL_LTIM
* -------------------------------------------------------------
* @param  DivFen_Name: As listed in the table above, brief "Name" at group REG_LSYS_CKD_FEN_XXX
*/
#define RCC_PeriphClockDividerFENGet(DivFen_Name, IS_ENABLE) \
	(LSYS_GET_FEN_##DivFen_Name(HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_FEN_##DivFen_Name)))

// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif

//Please add your defination here

/**
 * By default both USB_PLL(2'b01) / SYS_PLL(2'b11) are on then there will be no problem with switching.
 * If SYS_PLL(2'b11) is not enabled, there will be no intermediate state issues when switching between USB_PLL and XTAL(2'b00).
 *
 * When disable USB_PLL(2'b01), user can only use SYS_PLL and XTAL.
 * When switch between XTAL(2'b00 / 2'b10) and SYS_PLL(2'b11), in order to avoid the intermediate USB_PLL state (2'b01) in the writing process,
 * Set the state of XTAL to 2'b10 to avoid modifying two bits at the same time which causing an intermediate state.
 */
#ifdef USB_PLL_DISABLED

#undef  CKSL_SDH_XTAL
#define CKSL_SDH_XTAL 0x2
#undef  CKSL_I2S_XTAL
#define CKSL_I2S_XTAL 0x2
#undef  CKSL_GMAC_EXT50M
#define CKSL_GMAC_EXT50M 0x2
#undef  CKSL_VO_XTAL
#define CKSL_VO_XTAL 0x2
#undef  CKSL_CAN_XTAL
#define CKSL_CAN_XTAL 0x2
#undef  CKSL_SHPERI_XTAL
#define CKSL_SHPERI_XTAL 0x2
#undef  CKSL_PSRAM_XTAL
#define CKSL_PSRAM_XTAL 0x2
#undef  CKSL_SPIC_XTAL
#define CKSL_SPIC_XTAL 0x2
#undef  CKSL_HPERI_XTAL
#define CKSL_HPERI_XTAL 0x2
#undef  CKSL_HP_XTAL
#define CKSL_HP_XTAL 0x2

#endif

enum CHIP_TYPE {
	CHIP_TYPE_ASIC = 0,
	CHIP_TYPE_FPGA = 1,
	CHIP_TYPE_PALADIUM = 2,
	CHIP_TYPE_SIM = 3,
};

enum CORE_Vol_TYPE {
	CORE_VOL_0P9 = 0,
	CORE_VOL_1P0 = 1,
};

struct CLK_Info_Backup {
	u32	USBPLL_CLK;
	u32	SYSPLL_CLK;
	u8 Vol_Type;
	u8 CPU_CKD;
	u8 hperi_ckd;
	u8 psramc_ckd;
	u8 spic_ckd;
} __attribute__((packed, aligned(1)));

#define IS_SOC_CORE_VOLTAGE(vol) 	(((vol) == CORE_VOL_0P9) || ((vol) == CORE_VOL_1P0))
/**************************************************************************//**
 * @defgroup AMEBA_GBSS
 * @{
 * @brief AMEBA_GBSS Declaration
 * @ Total size of global bss is 64Byte used to image1/2 global bss.
 *****************************************************************************/
typedef struct {
	__IO u8 CHIP_INFO;
	__IO u8 PSRAM_TYPE;
	__IO u8 PSRAM_DQ;
	__IO u8 PSRAM_RSVD;
	__IO u32 psram_backup[6];
	struct CLK_Info_Backup clk_info_bk;
	__IO u8 AP_WAKEUP_STATUS;

	u8 GBSS_SYS_RSVD[22];/* resvd for system */

	u8 GBSS_USER_RSVD[64];/* resvd for user */
} GBSS_TypeDef;

Compile_Assert(sizeof(GBSS_TypeDef) == 128, "GBSS_TypeDef size shall be 128");

/** @} */

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */
#ifndef RTL8720F_TODO
#define REG_LSYS_BOOT_ADDR_TZ 	REG_LSYS_FLASH_PARA_ADDR0 // RL7005 0x2F0
#define REG_LSYS_BOOT_ADDR_NS 	REG_LSYS_FLASH_PARA_ADDR1 // RL7005 0x2F4
#endif

#endif


/** @} */

/** @} */
