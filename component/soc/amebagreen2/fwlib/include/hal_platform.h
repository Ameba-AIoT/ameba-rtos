/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _HAL_PLATFORM_
#define _HAL_PLATFORM_

#ifdef __cplusplus
extern "C" {
#endif


#define ROMVERSION			0x01
#define ROMVERSION_SUB		0 /* ACUT is v1.0, BCUT is v1.1 ... */

#define HAL_READ32(base, addr)				((u32)(*((volatile u32*)(base + addr))))
#define HAL_WRITE32(base, addr, value32)	((*((volatile u32*)(base + addr))) = ((u32)(value32)))
#define HAL_READ16(base, addr)				((u16)(*((volatile u16*)(base + addr))))
#define HAL_WRITE16(base, addr, value)		((*((volatile u16*)(base + addr))) = ((u16)(value)))
#define HAL_READ8(base, addr)				(*((volatile u8*)(base + addr)))
#define HAL_WRITE8(base, addr, value)		((*((volatile u8*)(base + addr))) = value)


#ifdef __cplusplus
#define __I		volatile             /*!< Defines 'read only' permissions                 */
#else
#define __I		volatile const       /*!< Defines 'read only' permissions                 */
#endif
#define __O		volatile             /*!< Defines 'write only' permissions                */
#define __IO	 	volatile             /*!< Defines 'read / write' permissions              */

/**************************************************************************//**
 * @defgroup AmebaD_Outline
 * @{
 *****************************************************************************/

/** @addtogroup AmebaD_Outline
  * @verbatim
  *****************************************************************************************
  * CPU
  *****************************************************************************************
  *	-Clock speed:		125MHz
  *	-Core:				CM4F
  *	-Working votage:	3.3V
  *
  *****************************************************************************************
  * Memory
  *****************************************************************************************
  *	-ROM:				512KB
  *	-RAM:				256KB
  *	-FLash:				128MB
  *
  *****************************************************************************************
  * GPIO
  *****************************************************************************************
  *	-GPIOA:				0~31, IN/OUT/INT
  *	-GPIOB:				0~31, IN/OUT/INT
  *
  *****************************************************************************************
  * Flash
  *****************************************************************************************
  *	-Max. speed:		100MHz
  *	-SPI Mode:			Quad IO mode
  *	-cache:				32K I/D read cache
  *	-XIP:				support executed in place
  *
  *****************************************************************************************
  * Firmware Protection
  *****************************************************************************************
  *	-RSIP(OTF):(Realsil Image Protection)
  *		-Flash Encryption
  *		-key store in OTP EFUSE Area, and can not read
  *	-RDP
  *		-4K RAM read protection
  *		-key store in OTP EFUSE Area, and can not read
  *
  *****************************************************************************************
  * UART
  *****************************************************************************************
  *	-Set count:			2
  *	-IrDA:				support
  *	-Low Power:			support
  *	-Sleep wakeup:		support
  *	-Baud rate:			110~6000000
  *	-GDMA:				support
  *
  *****************************************************************************************
  * UART LOG
  *****************************************************************************************
  *	-Set count:			1
  *	-IrDA:				support
  *	-Low Power:			not support
  *	-Sleep wakeup:		not support
  *	-Baud rate:			110~6000000
  *
  *****************************************************************************************
  * SPI Master
  *****************************************************************************************
  *	-Set count:			1
  *	-Max. Speed:		31.25MHz
  *	-GDMA:				support
  *
  *****************************************************************************************
  * SPI Slave
  *****************************************************************************************
  *	-Set count:			1
  *	-Max. Speed:		31.25MHz
  *	-GDMA:				support
  *
  *****************************************************************************************
  * I2C
  *****************************************************************************************
  *	-Set count:			1
  *	-Speed:				Standard (up to 100 kHz) and Fast (up to 400 kHz) Modes
  *	-Role:				Master & Slave
  *	-Sleep wakeup:		support
  *	-GDMA:				support
  *
  *****************************************************************************************
  * RTC
  *****************************************************************************************
  *	-Set count:			1
  *	-H:					12/24 hour mode
  *	-M:					0~59
  *	-S:					0~59
  *	-D:					0~511
  *	-Alarm:				support D/H/M/S
  *	-Daylight:			support
  *	-Calibration:		support
  *
  *****************************************************************************************
  * PWM
  *****************************************************************************************
  *	-Channel count:		6(NP)/18(AP)
  *	-Max. Resolution:	16bits (customizable)
  *	-Max. Frequency:	40MHz@1-bit mode, 40/(2^x))MHz@x-bit
  *
  *****************************************************************************************
  * Capture
  *****************************************************************************************
  *	-Channel count:		1
  *	-Max. Resolution:	16bits (customizable)
  *	-Max. Frequency:	40MHz@1-bit mode, 40/(2^x))MHz@x-bit
  *	-capture width:		supoort
  *	-capture number:	support
  *
  *****************************************************************************************
  * Timer
  *****************************************************************************************
  *	-Set count:			12
  *	-32KHz timer:		8
  *	-40MHz timer:		4
  *
  *****************************************************************************************
  * I2S
  *****************************************************************************************
  *	-Set count:			1
  *	-Source clk:		45.1584MHz or 98.304MHz(default)
  *	-Sample rate:		8K, 12K, 16K, 24K, 32K, 48K, 64K, 96K, 192K, 384K, 7.35K, 11.025K, 14.7K, 22.05K, 29.4K, 44.1K, 58.8K, 88.2K, 176.4K
  *	-Sample bit:		16 bit, 24 bit
  *	-DMA:				support
  *
  *****************************************************************************************
  * WIFI
  *****************************************************************************************
  *	-Set count:			1
  *	-Mode:				B/G/N(2.4G)
  *	-BW:				20MHz/40MHz
  *	-Ant:				1T/1R
  *
  *****************************************************************************************
  * USB device
  *****************************************************************************************
  *	-Set count:			1
  *	-DMA:				support
  *
  *****************************************************************************************
  * SDIO device
  *****************************************************************************************
  *	-Set count:			1
  *	-GSPI:				support
  *	-DMA:				support
  *
  *****************************************************************************************
  * BACKUP REG
  *****************************************************************************************
  *	-Set count:			4 dwords
  *
  *****************************************************************************************
  * CRYPTO
  *****************************************************************************************
  *	-MD5
  *	-SHA1
  *	-SHA2
  *	-HMAC-MD5
  *	-HMAC-SHA1
  *	-HMAC-SHA2
  *
  *	-AES-CBC
  *	-AES-ECB
  *	-AES-CTR
  *	-AES-CFB
  *	-AES-OFB
  *	-AES-GCM
  *	-3DES-CBC
  *	-3DES-ECB
  *	-DES-CBC
  *	-DES-ECB
  *
  *****************************************************************************************
  * ADC
  *****************************************************************************************
  *	-Set count:			1
  *	-Channel:			4
  *	-Sample rate:		max frequency up to 1MHz per channel, configurable
  *	-Resolution:		12 bit
  *	-signal sampling:	CH1 & CH3: 0 ~ 3V, CH2: 0~5V
  *	-power save:		Support one shot mode
  *	-GDMA:				support
  *
  *****************************************************************************************
  * GDMA
  *****************************************************************************************
  *	-Set count:			2
  *	-Channel:			2*6
  *
  *****************************************************************************************
  * @endverbatim
  */

/* TBD AutoGen */
#define SPI_FLASH_BASE           0x08000000        /* ID:S0, Inter. Type:AXI4, Top Address:0xFFFFFFF, Size(KB):128M, Clk Domain:SPIC_CLK */

/**************************************************************************//**
 * @defgroup AmebaD2_Address_Map AmebaD2 HS/LP Address Map
 * @{
 *****************************************************************************/
/* AUTO_GEN_START */
#define SHARE_TCM_BASE                     0x000F8000        /* ID:S7, Inter. Type:AXI4, Top Address:0x000FFFFF, Size(KB):32K, Clk Domain:CPU_CLK */
#define SRAM_BASE                          0x20000000        /* ID:S2, Inter. Type:AXI4, Top Address:0x200FFFFF, Size(KB):1024K, Clk Domain:SRAM_CLK */
#define HP_SRAM0_BASE                      0x20000000        /* ID:S2, Inter. Type:AXI4, Top Address:0x200FFFFF, Size(KB):1024K, Clk Domain:SRAM_CLK */
#define WIFI_REG_BASE                      0x40000000        /* ID:S3-0, Inter. Type:AHB, Top Address:0x4007FFFF, Size(KB):512K, Clk Domain:PERI_HCLK */
#define WIFI_RF_REG_BASE                   0x40003C00        /* ID:S3-0, Inter. Type:AHB, Top Address:0x4007FFFF, Size(KB):512K, Clk Domain:PERI_HCLK */
#define USB_REG_BASE                       0x40080000        /* ID:S3-1, Inter. Type:AHB, Top Address:0x400BFFFF, Size(KB):256K, Clk Domain:PERI_HCLK */
#define UVC_REG_BASE                       0x400B1000        /* ID:S3-1, Inter. Type:AHB, Top Address:0x400BFFFF, Size(KB):256K, Clk Domain:PERI_HCLK */
#define PKE_REG_BASE                       0x400C0000        /* ID:S3-2, Inter. Type:APB4, Top Address:0x400FFFFF, Size(KB):256K, Clk Domain:PERI_SHCLK */
#define LCDC_REG_BASE                      0x40100000        /* ID:S3-3, Inter. Type:APB4, Top Address:0x40107FFF, Size(KB):32K, Clk Domain:PERI_HCLK */
#define MJPEG_REG_BASE                     0x40108000        /* ID:S3-4, Inter. Type:APB3, Top Address:0x4010FFFF, Size(KB):32K, Clk Domain:PERI_HCLK */
#define SDIO_CCCR_REG_BASE                 0x40110000        /* ID:S3-5, Inter. Type:AHB, Top Address:0x40113FFF, Size(KB):16K, Clk Domain:PERI_HCLK */
#define SDIO_WIFI_REG_BASE                 0x40111000        /* ID:S3-5, Inter. Type:AHB, Top Address:0x40113FFF, Size(KB):16K, Clk Domain:PERI_HCLK */
#define SDIO_BT_REG_BASE                   0x40112000        /* ID:S3-5, Inter. Type:AHB, Top Address:0x40113FFF, Size(KB):16K, Clk Domain:PERI_HCLK */
#define SDIO_LOCAL_REG_BASE                0x40113000        /* ID:S3-5, Inter. Type:AHB, Top Address:0x40113FFF, Size(KB):16K, Clk Domain:PERI_HCLK */
#define SDIO_HOST_CCCR_REG_BASE            0x40114000        /* ID:S3-6, Inter. Type:APB4, Top Address:0x40117FFF, Size(KB):16K, Clk Domain:PERI_HCLK */
#define SDIO_HOST_LOCAL_REG_BASE           0x40117000        /* ID:S3-6, Inter. Type:APB4, Top Address:0x40117FFF, Size(KB):16K, Clk Domain:PERI_HCLK */
#define SPORT0_REG_BASE                    0x40118000        /* ID:S3-7, Inter. Type:AHB, Top Address:0x40118FFF, Size(KB):4K, Clk Domain:PERI_HCLK */
#define RMII_REG_BASE                      0x4011A000        /* ID:S3-8, Inter. Type:AXI4, Top Address:0x4011BFFF, Size(KB):8K, Clk Domain:PERI_HCLK */
#define SPI_FLASH_CTRL_BASE                0x4011D000        /* ID:S3-10, Inter. Type:AXI4, Top Address:0x4011DFFF, Size(KB):4K, Clk Domain:SPIC_CLK */
#define PSRAM_REG_BASE                     0x4011E000        /* ID:S3-11, Inter. Type:AXI4, Top Address:0x4011EFFF, Size(KB):4K, Clk Domain:PSRAM_CLK */
#define PSRAM_PHY_REG_BASE                 0x4011F000        /* ID:S3-12, Inter. Type:APB4, Top Address:0x4011FFFF, Size(KB):4K, Clk Domain:PSRAM_CLK */
#define GDMA0_REG_BASE                     0x40120000        /* ID:S3-13, Inter. Type:APB4, Top Address:0x40120FFF, Size(KB):4K, Clk Domain:PERI_HCLK */
#define SPI0_REG_BASE                      0x40121000        /* ID:S3-14, Inter. Type:APB4, Top Address:0x40121FFF, Size(KB):4K, Clk Domain:PERI_HCLK */
#define SPI1_REG_BASE                      0x40122000        /* ID:S3-15, Inter. Type:APB4, Top Address:0x40122FFF, Size(KB):4K, Clk Domain:PERI_HCLK */
#define AES_REG_BASE                       0x40123000        /* ID:S3-16, Inter. Type:APB4, Top Address:0x40123FFF, Size(KB):4K, Clk Domain:PERI_HCLK */
#define SHA_REG_BASE                       0x40124000        /* ID:S3-17, Inter. Type:APB4, Top Address:0x40124FFF, Size(KB):4K, Clk Domain:PERI_HCLK */
#define AES_SHA_DMA_REG_BASE               0x40125000        /* ID:S3-18, Inter. Type:APB4, Top Address:0x40125FFF, Size(KB):4K, Clk Domain:PERI_HCLK */
#define PPE_REG_BASE                       0x40126000        /* ID:S3-19, Inter. Type:APB4, Top Address:0x40126FFF, Size(KB):4K, Clk Domain:PERI_SHCLK */
#define OTPC_REG_BASE                      0x40800000        /* ID:S4-0, Inter. Type:APB4, Top Address:0x40807FFF, Size(KB):32K, Clk Domain:PERI_LCLK */
#define AES_KEY_REG_BASE                   0x40808000        /* ID:S4-1, Inter. Type:APB4, Top Address:0x40808FFF, Size(KB):4K, Clk Domain:PERI_HCLK */
#define SYSTEM_CTRL_BASE                   0x4080A000        /* ID:S4-2, Inter. Type:APB4, Top Address:0x4080BFFF, Size(KB):8K, Clk Domain:PERI_LCLK */
#define UART0_REG_BASE                     0x4080C000        /* ID:S4-3, Inter. Type:APB4, Top Address:0x4080CFFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define UART1_REG_BASE                     0x4080D000        /* ID:S4-4, Inter. Type:APB4, Top Address:0x4080DFFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define UART2_REG_BASE                     0x4080E000        /* ID:S4-5, Inter. Type:APB4, Top Address:0x4080EFFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define UART3_REG_BASE                     0x4080F000        /* ID:S4-6, Inter. Type:APB4, Top Address:0x4080FFFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define UARTLOG_REG_BASE                   0x40810000        /* ID:S4-7, Inter. Type:APB4, Top Address:0x40810FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define GPIO_REG_BASE                      0x40811000        /* ID:S4-8, Inter. Type:APB4, Top Address:0x40811FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define ADC_REG_BASE                       0x40812000        /* ID:S4-9, Inter. Type:APB4, Top Address:0x40812FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define CMP_REG_BASE                       0x40812800        /* ID:S4-9, Inter. Type:APB4, Top Address:0x40812FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define CTC_REG_BASE                       0x40813000        /* ID:S4-10, Inter. Type:APB4, Top Address:0x40813FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define SHA_KEY_REG_BASE                   0x40814000        /* ID:S4-11, Inter. Type:APB4, Top Address:0x40814FFF, Size(KB):4K, Clk Domain:PERI_HCLK */
#define IPC0_REG_BASE                      0x40815000        /* ID:S4-12, Inter. Type:APB4, Top Address:0x40815FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define IPC1_REG_BASE                      0x40815080        /* ID:S4-12, Inter. Type:APB4, Top Address:0x40815FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define IPC_SEMA_BASE                      0x40815200        /* ID:S4-12, Inter. Type:APB4, Top Address:0x40815FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define THERMAL_REG_BASE                   0x40816000        /* ID:S4-13, Inter. Type:APB4, Top Address:0x40816FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define DEBUGTIMER_REG_BASE                0x40817000        /* ID:S4-14, Inter. Type:APB4, Top Address:0x40817FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define PMC_TIMER_REG_BASE                 0x40818000        /* ID:S4-15, Inter. Type:APB4, Top Address:0x40818FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER0_REG_BASE                    0x40819000        /* ID:S4-16, Inter. Type:APB4, Top Address:0x40819FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER1_REG_BASE                    0x40819200        /* ID:S4-16, Inter. Type:APB4, Top Address:0x40819FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER2_REG_BASE                    0x40819400        /* ID:S4-16, Inter. Type:APB4, Top Address:0x40819FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER3_REG_BASE                    0x40819600        /* ID:S4-16, Inter. Type:APB4, Top Address:0x40819FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER4_REG_BASE                    0x41000000        /* ID:S5-0, Inter. Type:APB4, Top Address:0x41000FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER5_REG_BASE                    0x41000200        /* ID:S5-0, Inter. Type:APB4, Top Address:0x41000FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER6_REG_BASE                    0x41000400        /* ID:S5-0, Inter. Type:APB4, Top Address:0x41000FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER7_REG_BASE                    0x41000600        /* ID:S5-0, Inter. Type:APB4, Top Address:0x41000FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER8_REG_BASE                    0x41000800        /* ID:S5-0, Inter. Type:APB4, Top Address:0x41000FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define UPS_REG_BASE                       0x41000A00        /* ID:S5-0, Inter. Type:APB4, Top Address:0x41000FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TRNG_REG_BASE                      0x41001000        /* ID:S5-1, Inter. Type:APB4, Top Address:0x41001FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define RXI300_REG_BASE                    0x41002000        /* ID:S5-2, Inter. Type:APB4, Top Address:0x41003FFF, Size(KB):8K, Clk Domain:PERI_LCLK */
#define RSIP_REG_BASE                      0x41004000        /* ID:S5-3, Inter. Type:APB4, Top Address:0x41004FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define A2C0_REG_BASE                      0x41005000        /* ID:S5-4, Inter. Type:APB4, Top Address:0x41005FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define A2C1_REG_BASE                      0x41006000        /* ID:S5-5, Inter. Type:APB4, Top Address:0x41006FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define IR_REG_BASE                        0x41007000        /* ID:S5-6, Inter. Type:APB4, Top Address:0x41007FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define I2C0_REG_BASE                      0x41008000        /* ID:S5-7, Inter. Type:APB4, Top Address:0x41008FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define I2C1_REG_BASE                      0x41009000        /* ID:S5-8, Inter. Type:APB4, Top Address:0x41009FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define AUDIO_REG_BASE                     0x4100A000        /* ID:S5-9, Inter. Type:APB4, Top Address:0x4100AFFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define RXI300_OST_REG_BASE                0x41010000        /* ID:S5-10, Inter. Type:APB4, Top Address:0x4101FFFF, Size(KB):64K, Clk Domain:CPU_CLK */
#define PSRAM_BASE                         0x60000000        /* ID:S6, Inter. Type:AXI4, Top Address:0x6FFFFFFF, Size(KB):256M, Clk Domain:PSRAM_CLK */
#define AON_BACKUP_REG_BASE                0x4080A0E0
#define LDO_REG_BASE                       0x4080A100
#define SWR_REG_BASE                       0x4080A180
#define PMC_BASE                           0x4080A300
#define XTAL_REG_BASE                      0x4080A400
#define WLAFE_REG_BASE                     0x4080A480
#define PLL_REG_BASE                       0x4080A500
#define SYSTEM_MEM_CTRL_BASE               0x4080A600
#define PINMUX_REG_BASE                    0x4080A800
#define RTC_REG_BASE                       0x4080AA00
#define IWDG_REG_BASE                      0x4080AC00
#define WDG0_REG_BASE                      0x4080AD00
#define WDG1_REG_BASE_S                    0x5080AD40
#define WDG2_REG_BASE                      0x4080AD80
#define RETENTION_RAM_BASE                 0x4080AE00
#define SDM_REG_BASE                       0x4080B000
#define LEDC_REG_BASE                      0x4080B200
#define CPU_MMU_BASE_S                     0x5080B400

#define SHARE_TCM_BASE_S                   0x100F8000        /* ID:S7, Inter. Type:AXI4, Top Address:0x000FFFFF, Size(KB):32K, Clk Domain:CPU_CLK */
#define HP_SRAM0_BASE_S                    0x30000000        /* ID:S2, Inter. Type:AXI4, Top Address:0x200FFFFF, Size(KB):1024K, Clk Domain:SRAM_CLK */
#define WIFI_REG_BASE_S                    0x50000000        /* ID:S3-0, Inter. Type:AHB, Top Address:0x4007FFFF, Size(KB):512K, Clk Domain:PERI_HCLK */
#define WIFI_RF_REG_BASE_S                 0x50003C00        /* ID:S3-0, Inter. Type:AHB, Top Address:0x4007FFFF, Size(KB):512K, Clk Domain:PERI_HCLK */
#define USB_REG_BASE_S                     0x50080000        /* ID:S3-1, Inter. Type:AHB, Top Address:0x400BFFFF, Size(KB):256K, Clk Domain:PERI_HCLK */
#define UVC_REG_BASE_S                     0x500B1000        /* ID:S3-1, Inter. Type:AHB, Top Address:0x400BFFFF, Size(KB):256K, Clk Domain:PERI_HCLK */
#define PKE_REG_BASE_S                     0x500C0000        /* ID:S3-2, Inter. Type:APB4, Top Address:0x400FFFFF, Size(KB):256K, Clk Domain:PERI_SHCLK */
#define LCDC_REG_BASE_S                    0x50100000        /* ID:S3-3, Inter. Type:APB4, Top Address:0x40107FFF, Size(KB):32K, Clk Domain:PERI_HCLK */
#define MJPEG_REG_BASE_S                   0x50108000        /* ID:S3-4, Inter. Type:APB3, Top Address:0x4010FFFF, Size(KB):32K, Clk Domain:PERI_HCLK */
#define SDIO_CCCR_REG_BASE_S               0x50110000        /* ID:S3-5, Inter. Type:AHB, Top Address:0x40113FFF, Size(KB):16K, Clk Domain:PERI_HCLK */
#define SDIO_WIFI_REG_BASE_S               0x50111000        /* ID:S3-5, Inter. Type:AHB, Top Address:0x40113FFF, Size(KB):16K, Clk Domain:PERI_HCLK */
#define SDIO_BT_REG_BASE_S                 0x50112000        /* ID:S3-5, Inter. Type:AHB, Top Address:0x40113FFF, Size(KB):16K, Clk Domain:PERI_HCLK */
#define SDIO_LOCAL_REG_BASE_S              0x50113000        /* ID:S3-5, Inter. Type:AHB, Top Address:0x40113FFF, Size(KB):16K, Clk Domain:PERI_HCLK */
#define SDIO_HOST_CCCR_REG_BASE_S          0x50114000        /* ID:S3-6, Inter. Type:APB4, Top Address:0x40117FFF, Size(KB):16K, Clk Domain:PERI_HCLK */
#define SDIO_HOST_LOCAL_REG_BASE_S         0x50117000        /* ID:S3-6, Inter. Type:APB4, Top Address:0x40117FFF, Size(KB):16K, Clk Domain:PERI_HCLK */
#define SPORT0_REG_BASE_S                  0x50118000        /* ID:S3-7, Inter. Type:AHB, Top Address:0x40118FFF, Size(KB):4K, Clk Domain:PERI_HCLK */
#define RMII_REG_BASE_S                    0x5011A000        /* ID:S3-8, Inter. Type:AXI4, Top Address:0x4011BFFF, Size(KB):8K, Clk Domain:PERI_HCLK */
#define SPI_FLASH_CTRL_BASE_S              0x5011D000        /* ID:S3-10, Inter. Type:AXI4, Top Address:0x4011DFFF, Size(KB):4K, Clk Domain:SPIC_CLK */
#define PSRAM_REG_BASE_S                   0x5011E000        /* ID:S3-11, Inter. Type:AXI4, Top Address:0x4011EFFF, Size(KB):4K, Clk Domain:PSRAM_CLK */
#define PSRAM_PHY_REG_BASE_S               0x5011F000        /* ID:S3-12, Inter. Type:APB4, Top Address:0x4011FFFF, Size(KB):4K, Clk Domain:PSRAM_CLK */
#define GDMA0_REG_BASE_S                   0x50120000        /* ID:S3-13, Inter. Type:APB4, Top Address:0x40120FFF, Size(KB):4K, Clk Domain:PERI_HCLK */
#define SPI0_REG_BASE_S                    0x50121000        /* ID:S3-14, Inter. Type:APB4, Top Address:0x40121FFF, Size(KB):4K, Clk Domain:PERI_HCLK */
#define SPI1_REG_BASE_S                    0x50122000        /* ID:S3-15, Inter. Type:APB4, Top Address:0x40122FFF, Size(KB):4K, Clk Domain:PERI_HCLK */
#define AES_REG_BASE_S                     0x50123000        /* ID:S3-16, Inter. Type:APB4, Top Address:0x40123FFF, Size(KB):4K, Clk Domain:PERI_HCLK */
#define SHA_REG_BASE_S                     0x50124000        /* ID:S3-17, Inter. Type:APB4, Top Address:0x40124FFF, Size(KB):4K, Clk Domain:PERI_HCLK */
#define AES_SHA_DMA_REG_BASE_S             0x50125000        /* ID:S3-18, Inter. Type:APB4, Top Address:0x40125FFF, Size(KB):4K, Clk Domain:PERI_HCLK */
#define PPE_REG_BASE_S                     0x50126000        /* ID:S3-19, Inter. Type:APB4, Top Address:0x40126FFF, Size(KB):4K, Clk Domain:PERI_SHCLK */
#define OTPC_REG_BASE_S                    0x50800000        /* ID:S4-0, Inter. Type:APB4, Top Address:0x40807FFF, Size(KB):32K, Clk Domain:PERI_LCLK */
#define AES_KEY_REG_BASE_S                 0x50808000        /* ID:S4-1, Inter. Type:APB4, Top Address:0x40808FFF, Size(KB):4K, Clk Domain:PERI_HCLK */
#define SYSTEM_CTRL_BASE_S                 0x5080A000        /* ID:S4-2, Inter. Type:APB4, Top Address:0x4080BFFF, Size(KB):8K, Clk Domain:PERI_LCLK */
#define UART0_REG_BASE_S                   0x5080C000        /* ID:S4-3, Inter. Type:APB4, Top Address:0x4080CFFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define UART1_REG_BASE_S                   0x5080D000        /* ID:S4-4, Inter. Type:APB4, Top Address:0x4080DFFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define UART2_REG_BASE_S                   0x5080E000        /* ID:S4-5, Inter. Type:APB4, Top Address:0x4080EFFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define UART3_REG_BASE_S                   0x5080F000        /* ID:S4-6, Inter. Type:APB4, Top Address:0x4080FFFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define UARTLOG_REG_BASE_S                 0x50810000        /* ID:S4-7, Inter. Type:APB4, Top Address:0x40810FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define GPIO_REG_BASE_S                    0x50811000        /* ID:S4-8, Inter. Type:APB4, Top Address:0x40811FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define ADC_REG_BASE_S                     0x50812000        /* ID:S4-9, Inter. Type:APB4, Top Address:0x40812FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define CMP_REG_BASE_S                     0x50812800        /* ID:S4-9, Inter. Type:APB4, Top Address:0x40812FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define CTC_REG_BASE_S                     0x50813000        /* ID:S4-10, Inter. Type:APB4, Top Address:0x40813FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define SHA_KEY_REG_BASE_S                 0x50814000        /* ID:S4-11, Inter. Type:APB4, Top Address:0x40814FFF, Size(KB):4K, Clk Domain:PERI_HCLK */
#define IPC0_REG_BASE_S                    0x50815000        /* ID:S4-12, Inter. Type:APB4, Top Address:0x40815FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define IPC1_REG_BASE_S                    0x50815080        /* ID:S4-12, Inter. Type:APB4, Top Address:0x40815FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define IPC_SEMA_BASE_S                    0x50815200        /* ID:S4-12, Inter. Type:APB4, Top Address:0x40815FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define THERMAL_REG_BASE_S                 0x50816000        /* ID:S4-13, Inter. Type:APB4, Top Address:0x40816FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define DEBUGTIMER_REG_BASE_S              0x50817000        /* ID:S4-14, Inter. Type:APB4, Top Address:0x40817FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define PMC_TIMER_REG_BASE_S               0x50818000        /* ID:S4-15, Inter. Type:APB4, Top Address:0x40818FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER0_REG_BASE_S                  0x50819000        /* ID:S4-16, Inter. Type:APB4, Top Address:0x40819FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER1_REG_BASE_S                  0x50819200        /* ID:S4-16, Inter. Type:APB4, Top Address:0x40819FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER2_REG_BASE_S                  0x50819400        /* ID:S4-16, Inter. Type:APB4, Top Address:0x40819FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER3_REG_BASE_S                  0x50819600        /* ID:S4-16, Inter. Type:APB4, Top Address:0x40819FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER4_REG_BASE_S                  0x51000000        /* ID:S5-0, Inter. Type:APB4, Top Address:0x41000FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER5_REG_BASE_S                  0x51000200        /* ID:S5-0, Inter. Type:APB4, Top Address:0x41000FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER6_REG_BASE_S                  0x51000400        /* ID:S5-0, Inter. Type:APB4, Top Address:0x41000FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER7_REG_BASE_S                  0x51000600        /* ID:S5-0, Inter. Type:APB4, Top Address:0x41000FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER8_REG_BASE_S                  0x51000800        /* ID:S5-0, Inter. Type:APB4, Top Address:0x41000FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define UPS_REG_BASE_S                     0x51000A00        /* ID:S5-0, Inter. Type:APB4, Top Address:0x41000FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TRNG_REG_BASE_S                    0x51001000        /* ID:S5-1, Inter. Type:APB4, Top Address:0x41001FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define RXI300_REG_BASE_S                  0x51002000        /* ID:S5-2, Inter. Type:APB4, Top Address:0x41003FFF, Size(KB):8K, Clk Domain:PERI_LCLK */
#define MPC1_BASE_S                        0x51002200        /* ID:S5-2, Inter. Type:APB4, Top Address:0x41003FFF, Size(KB):8K, Clk Domain:PERI_LCLK */
#define MPC2_BASE_S                        0x51002300        /* ID:S5-2, Inter. Type:APB4, Top Address:0x41003FFF, Size(KB):8K, Clk Domain:PERI_LCLK */
#define MPC3_BASE_S                        0x51002400        /* ID:S5-2, Inter. Type:APB4, Top Address:0x41003FFF, Size(KB):8K, Clk Domain:PERI_LCLK */
#define PPC_BASE_S                         0x51002500        /* ID:S5-2, Inter. Type:APB4, Top Address:0x41003FFF, Size(KB):8K, Clk Domain:PERI_LCLK */
#define RSIP_REG_BASE_S                    0x51004000        /* ID:S5-3, Inter. Type:APB4, Top Address:0x41004FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define A2C0_REG_BASE_S                    0x51005000        /* ID:S5-4, Inter. Type:APB4, Top Address:0x41005FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define A2C1_REG_BASE_S                    0x51006000        /* ID:S5-5, Inter. Type:APB4, Top Address:0x41006FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define IR_REG_BASE_S                      0x51007000        /* ID:S5-6, Inter. Type:APB4, Top Address:0x41007FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define I2C0_REG_BASE_S                    0x51008000        /* ID:S5-7, Inter. Type:APB4, Top Address:0x41008FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define I2C1_REG_BASE_S                    0x51009000        /* ID:S5-8, Inter. Type:APB4, Top Address:0x41009FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define AUDIO_REG_BASE_S                   0x5100A000        /* ID:S5-9, Inter. Type:APB4, Top Address:0x4100AFFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define RXI300_OST_REG_BASE_S              0x51010000        /* ID:S5-10, Inter. Type:APB4, Top Address:0x4101FFFF, Size(KB):64K, Clk Domain:CPU_CLK */
#define PSRAM_BASE_S                       0x70000000        /* ID:S6, Inter. Type:AXI4, Top Address:0x6FFFFFFF, Size(KB):256M, Clk Domain:PSRAM_CLK */
/* AUTO_GEN_END */
/** @} End of group AmebaD_Address_Map */
#define DATA_FLASH_BASE                    0x60000000
#define PMC_BASE_S                         0x5080A300

#define TZ_IDAU_SEC_OFFSET                 0x10000000
#define KBYTES(x)                          ((x) << 10)
#define MBYTES(x)                          ((x) << 20)

/**************************************************************************//**
 * @defgroup AmebaD_Peripheral_Declaration  AmebaD HS/LP Peripheral Declarations
 * @{
 *****************************************************************************/
#define RRAM_DEV			((RRAM_TypeDef			*) RETENTION_RAM_BASE)		/*retention mem */

#define UART0_DEV			((UART_TypeDef			*) UART0_REG_BASE)		/*hp uart0 */
#define UART1_DEV			((UART_TypeDef			*) UART1_REG_BASE)		/*hp uart1 */
#define UART2_DEV			((UART_TypeDef			*) UART2_REG_BASE)		/*hp uart2 */
#define UART3_DEV			((UART_TypeDef			*) UART3_REG_BASE)		/*hp uart3 */
#define UART0_DEV_S			((UART_TypeDef			*) UART0_REG_BASE_S)		/*hp uart0 secure */
#define UART1_DEV_S			((UART_TypeDef			*) UART1_REG_BASE_S)		/*hp uart1 secure*/
#define UART2_DEV_S			((UART_TypeDef			*) UART2_REG_BASE_S)		/*hp uart2 secrue*/
#define UART3_DEV_S			((UART_TypeDef			*) UART3_REG_BASE_S)		/*hp uart3 secrue*/

#define LOGUART_DEV			((LOGUART_TypeDef		*) UARTLOG_REG_BASE)
#define LOGUART_DEV_S		((LOGUART_TypeDef		*) UARTLOG_REG_BASE_S)

#define SPI0_DEV			((SPI_TypeDef			*) SPI0_REG_BASE)		/*hp spi0 */
#define SPI1_DEV			((SPI_TypeDef			*) SPI1_REG_BASE)		/*hp spi1 */
#define SPI0_DEV_S			((SPI_TypeDef			*) SPI0_REG_BASE_S)		/*hp spi0 secure */
#define SPI1_DEV_S			((SPI_TypeDef			*) SPI1_REG_BASE_S)		/*hp spi1 secure */

#define PSRAMC_DEV			((SPIC_TypeDef			*) PSRAM_REG_BASE)				/*AP */
#define PSRAMPHY_DEV		((PSPHY_TypeDef			*) PSRAM_PHY_REG_BASE)				/*AP */

#define LCDC				((LCDC_TypeDef			*) LCDC_REG_BASE)
#define SPIC				((SPIC_TypeDef			*) SPI_FLASH_CTRL_BASE)
#define SPIC_COMBO			((SPIC_TypeDef			*) PSRAM_REG_BASE)
#define ADC					((ADC_TypeDef			*) ADC_REG_BASE)
#define ADC_S				((ADC_TypeDef			*) ADC_REG_BASE_S)
#define CAPTOUCH_DEV		((CAPTOUCH_TypeDef		*) CTC_REG_BASE)
#define CAPTOUCH_DEV_S		((CAPTOUCH_TypeDef		*) CTC_REG_BASE_S)
#define I2C0_DEV			((I2C_TypeDef			*) I2C0_REG_BASE)		/*lp i2c0 */
#define I2C1_DEV			((I2C_TypeDef			*) I2C1_REG_BASE)		/*hp i2c1 */
#define I2C1_DEV_S			((I2C_TypeDef			*) I2C1_REG_BASE_S)		/*hp i2c1 secure */
#define TIM0				((RTIM_TypeDef			*) TIMER0_REG_BASE)
#define TIM1				((RTIM_TypeDef			*) TIMER1_REG_BASE)
#define TIM2				((RTIM_TypeDef			*) TIMER2_REG_BASE)
#define TIM3				((RTIM_TypeDef			*) TIMER3_REG_BASE)
#define TIM4				((RTIM_TypeDef			*) TIMER4_REG_BASE)
#define TIM5				((RTIM_TypeDef			*) TIMER5_REG_BASE)
#define TIM6				((RTIM_TypeDef			*) TIMER6_REG_BASE)
#define TIM7				((RTIM_TypeDef			*) TIMER7_REG_BASE)
#define TIM8				((RTIM_TypeDef			*) TIMER8_REG_BASE)

#define TIM0_S				((RTIM_TypeDef			*) TIMER0_REG_BASE_S)
#define TIM1_S				((RTIM_TypeDef			*) TIMER1_REG_BASE_S)
#define TIM2_S				((RTIM_TypeDef			*) TIMER2_REG_BASE_S)
#define TIM3_S				((RTIM_TypeDef			*) TIMER3_REG_BASE_S)
#define TIM4_S				((RTIM_TypeDef			*) TIMER4_REG_BASE_S)
#define TIM5_S				((RTIM_TypeDef			*) TIMER5_REG_BASE_S)
#define TIM6_S				((RTIM_TypeDef			*) TIMER6_REG_BASE_S)
#define TIM7_S				((RTIM_TypeDef 			*) TIMER7_REG_BASE_S)
#define TIM8_S				((RTIM_TypeDef			*) TIMER8_REG_BASE_S)

#define PMCTIMER_DEV0		((PMCTIMER_TpyeDef		*) PMC_TIMER_REG_BASE)
#define PMCTIMER_DEV1		((PMCTIMER_TpyeDef		*) (PMC_TIMER_REG_BASE + 0x200))
#define PMCTIMER_DEV0_S		((PMCTIMER_TpyeDef		*) PMC_TIMER_REG_BASE_S)
#define PMCTIMER_DEV1_S		((PMCTIMER_TpyeDef		*) (PMC_TIMER_REG_BASE_S + 0x200))

#define IR_DEV				((IR_TypeDef			*) IR_REG_BASE)
#define IR_DEV_S			((IR_TypeDef			*) IR_REG_BASE_S)

#define RTC_DEV				((RTC_TypeDef			*) RTC_REG_BASE)					/*NP */
#define RTC_DEV_S			((RTC_TypeDef			*) (RTC_REG_BASE + TZ_IDAU_SEC_OFFSET))
#define PINMUX				((PINMUX_TypeDef		*) PINMUX_REG_BASE)			/*NP */
#define PINMUX_S			((PINMUX_TypeDef		*) (PINMUX_REG_BASE + TZ_IDAU_SEC_OFFSET))			/*NP */
#define GPIOA_BASE			((GPIO_TypeDef			*) GPIO_REG_BASE)			/*NP */
#define GPIOB_BASE			((GPIO_TypeDef			*) (GPIO_REG_BASE + 0x400))	/*NP */
#define GPIOC_BASE			((GPIO_TypeDef			*) (GPIO_REG_BASE + 0x800))	/*NP */
#define GPIOA_BASE_S		((GPIO_TypeDef			*) GPIO_REG_BASE_S)
#define GPIOB_BASE_S		((GPIO_TypeDef			*) (GPIO_REG_BASE_S + 0x400))
#define GPIOC_BASE_S		((GPIO_TypeDef			*) (GPIO_REG_BASE_S + 0x800))
#define BACKUP_REG			((BACKUP_REG_TypeDef	*) AON_BACKUP_REG_BASE)			/*AON */

#define AP_MPC1			((MPC_TypeDef			*) MPC1_BASE_S)			/*AP */
#define AP_MPC2			((MPC_TypeDef			*) MPC2_BASE_S)			/*AP */
#define AP_MPC3			((MPC_TypeDef			*) MPC3_BASE_S)			/*AP */
#define AP_PPC				((PPC_TypeDef			*) PPC_BASE_S)			/*AP */

#define IPCAP_DEV 			((IPC_TypeDef 			*) IPC0_REG_BASE)				/*NP */
#define IPCNP_DEV 			((IPC_TypeDef 			*) IPC1_REG_BASE)				/*AP */
#define IPCAP_DEV_S 		((IPC_TypeDef 			*) IPC0_REG_BASE_S)				/*NP Secure */
#define IPCNP_DEV_S 		((IPC_TypeDef 			*) IPC1_REG_BASE_S)				/*AP Secure*/

#define TM_DEV				((THERMAL_TypeDef		*) THERMAL_REG_BASE)
#define TM_DEV_S			((THERMAL_TypeDef		*) THERMAL_REG_BASE_S)

#define GDMA_BASE			((GDMA_TypeDef			*) (GDMA0_REG_BASE))

#define IWDG_DEV			((WDG_TypeDef			*) (IWDG_REG_BASE))
#define CPU1_WDG_DEV		((WDG_TypeDef			*) (WDG0_REG_BASE))
#define CPU0_S_WDG_DEV		((WDG_TypeDef			*) (WDG1_REG_BASE_S))
#define CPU0_NS_WDG_DEV		((WDG_TypeDef			*) (WDG2_REG_BASE))
#define SDM_DEV				((SDM_TypeDef			*) (SDM_REG_BASE))
#define SDM_DEV_S			((SDM_TypeDef			*) (SDM_REG_BASE_S))
#define OTPC_DEV			((OTPC_TypeDef			*) (OTPC_REG_BASE))
#define OTPC_DEV_S			((OTPC_TypeDef			*) (OTPC_REG_BASE_S))
#define DTIMER_DEV			((DEBUGTIMER_TypeDef	*) (DEBUGTIMER_REG_BASE))
#define DTIMER_DEV_S		((DEBUGTIMER_TypeDef	*) (DEBUGTIMER_REG_BASE_S))
#define TRNG_DEV			((TRNG_TypeDef			*) (TRNG_REG_BASE))
#define TRNG_SDEV			((TRNG_TypeDef			*) (TRNG_REG_BASE_S))

#define RSIP_BASE			((RSIP_REG_TypeDef		*) (RSIP_REG_BASE))

#define AUDIO_SPORT0_DEV	((AUDIO_SPORT_TypeDef	*) SPORT0_REG_BASE)
#define AUDIO_SPORT0_DEV_S	((AUDIO_SPORT_TypeDef	*) SPORT0_REG_BASE_S)	/*SPORT0 secure */

#define SDIO_HOST			((SDIOHOST_TypeDef		*) SDIO_HOST_CCCR_REG_BASE)
#define SDIO_WIFI			((SDIO_TypeDef			*) SDIO_WIFI_REG_BASE)
#define SDIO_BT				((SDIO_TypeDef			*) SDIO_BT_REG_BASE)

#define UVC					((UVC_TypeDef			*) UVC_REG_BASE)
#define PPE					((PPE_TypeDef			*) PPE_REG_BASE)

#define A2C0					((A2C_TypeDef			*) A2C0_REG_BASE)
#define A2C1					((A2C_TypeDef			*) A2C1_REG_BASE)
#define A2C0_S				((A2C_TypeDef			*) A2C0_REG_BASE_S)
#define A2C1_S				((A2C_TypeDef			*) A2C1_REG_BASE_S)

/* syson related ip*/
#define LDO_BASE      ((LDO_TypeDef *)  (LDO_REG_BASE))
#define REGU_BASE     ((REGU_TypeDef *)  (LDO_REG_BASE))
#define PLL_BASE      ((PLL_TypeDef *)  (PLL_REG_BASE))
#define SWR_BASE      ((SWR_TypeDef *)  (SWR_REG_BASE))
#define WLAFE_BASE    ((WLAFE_TypeDef *) (WLAFE_REG_BASE))
#define XTAL_BASE     ((XTAL_TypeDef *) (XTAL_REG_BASE))
#define OSC131K_BASE      ((RTC_MISC_TypeDef *)  (RTC_MISC_REG_BASE))
#define PSEUDO_I2C_DEV      ((LEDC_PRO_TypeDef *)  (LEDC_REG_BASE))

/** @} End of group AmebaD_Peripheral_Declaration */
#define MSP_RAM_LP					0x20005000						/* rom used, can not change, RISC-V kept 16-byte aligned */
#define MSPLIM_RAM_HP				0x30000600						/* rom used, can not change */
#define MSP_RAM_HP					0x30001000						/* rom used, can not change */
#define MSP_RAM_HP_NS				0x20003000

#define BOOT_LOADER_START			0x30005000						/* rom used, can not change */
#define BOOT_LOADER_LIMIT			0x30080000						/* rom used, can not change */
#define LOADER_DOWNLOAD_ADDR		(BOOT_LOADER_LIMIT - 64 * 1024)	/* rom used, can not change */

#define FULLMAC_ADDR_START			0x30002400						/* rom used, can not change */
#define FULLMAC_ADDR_END			0x30080000						/* rom used, can not change */

/*BT share mem with system*/
#define SHARE_MEM_BT_E0_ADDRESS		0x20080000
#define SHARE_MEM_BT_E0_LEN			(96 * 1024)	/* size: 96KB */
#define SHARE_MEM_BT_E1_ADDRESS		0x20098000
#define SHARE_MEM_BT_E1_LEN			(56 * 1024)	/* size:56KB */

#if defined(CONFIG_RSICV_CORE_KR4)
#define CONTEXT_SAVE_SIZE	320	/* portCONTEXT_SIZE:66*4 = 288 roundup to 64B aligned */
#elif defined(CONFIG_ARM_CORE_CA32)
#define CONTEXT_SAVE_SIZE	320 /* 15*4 + 32*8: general reg and floating reg */
#elif defined(CONFIG_ARM_CORE_CM4)
#define CONTEXT_SAVE_SIZE	192 /* 15*4 + 16*8: s16~s31 if use float */
#elif defined(CONFIG_ARM_CORE_CM0)
#define CONTEXT_SAVE_SIZE	64	/* not support hw float, 15*4 */
#endif

/** @} End of group AmebaD_Outline */

#ifdef __cplusplus
}
#endif
#endif //_HAL_PLATFORM_
