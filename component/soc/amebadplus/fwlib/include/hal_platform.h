/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _HAL_PLATFORM_
#define _HAL_PLATFORM_

#define ROMVERSION			0x01 /* ACUT is 0x01, BCUT is 0x02 ... */
#define ROMVERSION_SUB		0 /* ROM sub version */
#define ROMINFORMATION		(ROMVERSION)

#define HAL_READ32(base, addr)				rtk_le32_to_cpu(*((volatile u32*)(base + addr)))
#define HAL_WRITE32(base, addr, value32)		((*((volatile u32*)(base + addr))) = rtk_cpu_to_le32(value32))
#define HAL_READ16(base, addr)				rtk_le16_to_cpu(*((volatile u16*)(base + addr)))
#define HAL_WRITE16(base, addr, value)		((*((volatile u16*)(base + addr))) = rtk_cpu_to_le16(value))
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
  *	-Channel count:		6(KM0)/18(KM4)
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
#define SYSTEM_CTRL_BASE		0x41008000


/**************************************************************************//**
 * @defgroup AmebaD2_Address_Map AmebaD2 HS/LP Address Map
 * @{
 *****************************************************************************/
#define SPI_FLASH_BASE           0x08000000        /* ID:S0, Inter. Type:AXI4, Top Address:0xFFFFFFF, Size(KB):128M, Clk Domain:SPIC_CLK */
#define HP_SRAM0_BASE            0x20000000        /* ID:S1, Inter. Type:AXI4, Top Address:0x2007FFFF, Size(KB):512K, Clk Domain:SRAM_CLK */
#define WIFI_REG_BASE            0x40000000        /* ID:S3-0, Inter. Type:AHB, Top Address:0x4007FFFF, Size(KB):512K, Clk Domain:PERI_HCLK */
#define WIFI_RF_REG_BASE         0x40003C00        /* ID:S3-0, Inter. Type:AHB, Top Address:0x4007FFFF, Size(KB):512K, Clk Domain:PERI_HCLK */
#define BT_REG_BASE              0x40080000        /* ID:S3-1, Inter. Type:AHB, Top Address:0x400FFFFF, Size(KB):512K, Clk Domain:BT_SYS_CLK */
#define AES_REG_BASE             0x40100000        /* ID:S3-2, Inter. Type:APB4, Top Address:0x4010FFFF, Size(KB):64K, Clk Domain:PERI_HCLK */
#define SHA_REG_BASE             0x40108000        /* ID:S3-2, Inter. Type:APB4, Top Address:0x4010FFFF, Size(KB):64K, Clk Domain:PERI_HCLK */
#define GDMA0_REG_BASE           0x40110000        /* ID:S3-3, Inter. Type:APB4, Top Address:0x40117FFF, Size(KB):32K, Clk Domain:PERI_HCLK */
#define PPE_REG_BASE             0x40118000        /* ID:S3-4, Inter. Type:APB4, Top Address:0x4011FFFF, Size(KB):32K, Clk Domain:QSPI_CFG_CLK */
#define SDIO_REG_BASE            0x40120000        /* ID:S3-5, Inter. Type:AHB, Top Address:0x40123FFF, Size(KB):16K, Clk Domain:PERI_HCLK */
#define SPI0_REG_BASE            0x40124000        /* ID:S3-6, Inter. Type:APB4, Top Address:0x40124FFF, Size(KB):4K, Clk Domain:PERI_HCLK */
#define SPI1_REG_BASE            0x40125000        /* ID:S3-7, Inter. Type:APB4, Top Address:0x40125FFF, Size(KB):4K, Clk Domain:PERI_HCLK */
#define PSRAM_PHY_REG_BASE       0x40126000        /* ID:S3-8, Inter. Type:APB4, Top Address:0x40126FFF, Size(KB):4K, Clk Domain:PSRAM_CLK */
#define PSRAM_REG_BASE           0x40127000        /* ID:S3-9, Inter. Type:AXI4, Top Address:0x40127FFF, Size(KB):4K, Clk Domain:PSRAM_CLK */
#define SPI_FLASH_CTRL_BASE      0x40128000        /* ID:S3-10, Inter. Type:AXI4, Top Address:0x40128FFF, Size(KB):4K, Clk Domain:SPIC_CLK */
#define QSPI_REG_BASE            0x40129000        /* ID:S3-11, Inter. Type:AXI4, Top Address:0x40129FFF, Size(KB):4K, Clk Domain:QSPI_CLK */
#define SPORT0_REG_BASE          0x4012A000        /* ID:S3-12, Inter. Type:AHB, Top Address:0x4012AFFF, Size(KB):4K, Clk Domain:PERI_HCLK */
#define SPORT1_REG_BASE          0x4012B000        /* ID:S3-13, Inter. Type:AHB, Top Address:0x4012BFFF, Size(KB):4K, Clk Domain:PERI_HCLK */
#define USB_REG_BASE             0x40140000        /* ID:S3-14, Inter. Type:AHB, Top Address:0x4017FFFF, Size(KB):256K, Clk Domain:PERI_HCLK */
#define OTPC_REG_BASE            0x41000000        /* ID:S4-0, Inter. Type:APB4, Top Address:0x41007FFF, Size(KB):32K, Clk Domain:PERI_LCLK */
#define SYSTEM_CTRL_BASE         0x41008000        /* ID:S4-1, Inter. Type:APB4, Top Address:0x4100BFFF, Size(KB):16K, Clk Domain:PERI_LCLK */
#define UART0_REG_BASE           0x4100C000        /* ID:S4-2, Inter. Type:APB4, Top Address:0x4100CFFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define UART1_REG_BASE           0x4100D000        /* ID:S4-3, Inter. Type:APB4, Top Address:0x4100DFFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define UART2_REG_BASE           0x4100E000        /* ID:S4-4, Inter. Type:APB4, Top Address:0x4100EFFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define UARTLOG_REG_BASE         0x4100F000        /* ID:S4-5, Inter. Type:APB4, Top Address:0x4100FFFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define GPIO_REG_BASE            0x41010000        /* ID:S4-6, Inter. Type:APB4, Top Address:0x41010FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define ADC_REG_BASE             0x41011000        /* ID:S4-7, Inter. Type:APB4, Top Address:0x41011FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define CMP_REG_BASE             0x41011800        /* ID:S4-7, Inter. Type:APB4, Top Address:0x41011FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define CTC_REG_BASE             0x41012000        /* ID:S4-8, Inter. Type:APB4, Top Address:0x41012FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define KSCAN_REG_BASE           0x41013000        /* ID:S4-9, Inter. Type:APB4, Top Address:0x41013FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define IPC0_REG_BASE            0x41014000        /* ID:S4-10, Inter. Type:APB4, Top Address:0x41014FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define IPC1_REG_BASE            0x41014080        /* ID:S4-10, Inter. Type:APB4, Top Address:0x41014FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define IPC_SEMA_BASE			 0x41014200        /* ID:S4-10, Inter. Type:APB4, Top Address:0x41014FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define DEBUGTIMER_REG_BASE      0x41015000        /* ID:S4-11, Inter. Type:APB4, Top Address:0x41015FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define PMC_TIMER_REG_BASE       0x41016000        /* ID:S4-12, Inter. Type:APB4, Top Address:0x41016FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER0_REG_BASE          0x41017000        /* ID:S4-13, Inter. Type:APB4, Top Address:0x41017FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER1_REG_BASE          0x41017200        /* ID:S4-13, Inter. Type:APB4, Top Address:0x41017FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER2_REG_BASE          0x41017400        /* ID:S4-13, Inter. Type:APB4, Top Address:0x41017FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER3_REG_BASE          0x41017600        /* ID:S4-13, Inter. Type:APB4, Top Address:0x41017FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER4_REG_BASE          0x41017800        /* ID:S4-13, Inter. Type:APB4, Top Address:0x41017FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER5_REG_BASE          0x41017A00        /* ID:S4-13, Inter. Type:APB4, Top Address:0x41017FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER6_REG_BASE          0x41017C00        /* ID:S4-13, Inter. Type:APB4, Top Address:0x41017FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER7_REG_BASE          0x41017E00        /* ID:S4-13, Inter. Type:APB4, Top Address:0x41017FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER8_REG_BASE          0x41100000        /* ID:S5-0, Inter. Type:APB4, Top Address:0x41100FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER9_REG_BASE          0x41100200        /* ID:S5-0, Inter. Type:APB4, Top Address:0x41100FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER10_REG_BASE         0x41100400        /* ID:S5-0, Inter. Type:APB4, Top Address:0x41100FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER11_REG_BASE         0x41100600        /* ID:S5-0, Inter. Type:APB4, Top Address:0x41100FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TRNG_REG_BASE            0x41101000        /* ID:S5-1, Inter. Type:APB4, Top Address:0x41101FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define RXI300_REG_BASE          0x41102000        /* ID:S5-2, Inter. Type:APB4, Top Address:0x41103FFF, Size(KB):8K, Clk Domain:PERI_LCLK */
#define RSIP_REG_BASE            0x41104000        /* ID:S5-3, Inter. Type:APB4, Top Address:0x41104FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define LEDC_REG_BASE            0x41105000        /* ID:S5-4, Inter. Type:APB4, Top Address:0x41105FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define AUDIO_REG_BASE           0x41106000        /* ID:S5-5, Inter. Type:APB4, Top Address:0x41106FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define IR_REG_BASE              0x41107000        /* ID:S5-6, Inter. Type:APB4, Top Address:0x41107FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define I2C0_REG_BASE            0x41108000        /* ID:S5-7, Inter. Type:APB4, Top Address:0x41109FFF, Size(KB):8K, Clk Domain:PERI_LCLK */
#define I2C1_REG_BASE            0x4110A000        /* ID:S5-8, Inter. Type:APB4, Top Address:0x4110BFFF, Size(KB):8K, Clk Domain:PERI_LCLK */
#define PSRAM_BASE               0x60000000        /* ID:S6, Inter. Type:AXI4, Top Address:0x6FFFFFFF, Size(KB):256M, Clk Domain:PSRAM_CLK */
#define PINMUX_REG_BASE          0x41008800
#define RTC_BASE                 0x41008A00
#define IWDG_REG_BASE            0x41008C00
#define WDG0_REG_BASE            0x41008D00
#define WDG1_REG_BASE_S          0x51008D40
#define WDG2_REG_BASE            0x41008D80
#define RETENTION_RAM_BASE       0x41008E00
#define SDM_REG_BASE             0x41009000

#define SPI_FLASH_BASE_S         0x18000000        /* ID:S0, Inter. Type:AXI4, Top Address:0xFFFFFFF, Size(KB):128M, Clk Domain:SPIC_CLK */
#define HP_SRAM0_BASE_S          0x30000000        /* ID:S1, Inter. Type:AXI4, Top Address:0x2007FFFF, Size(KB):512K, Clk Domain:SRAM_CLK */
#define WIFI_REG_BASE_S          0x50000000        /* ID:S3-0, Inter. Type:AHB, Top Address:0x4007FFFF, Size(KB):512K, Clk Domain:PERI_HCLK */
#define WIFI_RF_REG_BASE_S       0x50003C00        /* ID:S3-0, Inter. Type:AHB, Top Address:0x4007FFFF, Size(KB):512K, Clk Domain:PERI_HCLK */
#define BT_REG_BASE_S            0x50080000        /* ID:S3-1, Inter. Type:AHB, Top Address:0x400FFFFF, Size(KB):512K, Clk Domain:BT_SYS_CLK */
#define AES_REG_BASE_S           0x50100000        /* ID:S3-2, Inter. Type:APB4, Top Address:0x4010FFFF, Size(KB):64K, Clk Domain:PERI_HCLK */
#define SHA_REG_BASE_S           0x50108000        /* ID:S3-2, Inter. Type:APB4, Top Address:0x4010FFFF, Size(KB):64K, Clk Domain:PERI_HCLK */
#define GDMA0_REG_BASE_S         0x50110000        /* ID:S3-3, Inter. Type:APB4, Top Address:0x40117FFF, Size(KB):32K, Clk Domain:PERI_HCLK */
#define PPE_REG_BASE_S           0x50118000        /* ID:S3-4, Inter. Type:APB4, Top Address:0x4011FFFF, Size(KB):32K, Clk Domain:QSPI_CFG_CLK */
#define SDIO_REG_BASE_S          0x50120000        /* ID:S3-5, Inter. Type:AHB, Top Address:0x40123FFF, Size(KB):16K, Clk Domain:PERI_HCLK */
#define SPI0_REG_BASE_S          0x50124000        /* ID:S3-6, Inter. Type:APB4, Top Address:0x40124FFF, Size(KB):4K, Clk Domain:PERI_HCLK */
#define SPI1_REG_BASE_S          0x50125000        /* ID:S3-7, Inter. Type:APB4, Top Address:0x40125FFF, Size(KB):4K, Clk Domain:PERI_HCLK */
#define PSRAM_PHY_REG_BASE_S     0x50126000        /* ID:S3-8, Inter. Type:APB4, Top Address:0x40126FFF, Size(KB):4K, Clk Domain:PSRAM_CLK */
#define PSRAM_REG_BASE_S         0x50127000        /* ID:S3-9, Inter. Type:AXI4, Top Address:0x40127FFF, Size(KB):4K, Clk Domain:PSRAM_CLK */
#define SPI_FLASH_CTRL_BASE_S    0x50128000        /* ID:S3-10, Inter. Type:AXI4, Top Address:0x40128FFF, Size(KB):4K, Clk Domain:SPIC_CLK */
#define QSPI_REG_BASE_S          0x50129000        /* ID:S3-11, Inter. Type:AXI4, Top Address:0x40129FFF, Size(KB):4K, Clk Domain:QSPI_CLK */
#define SPORT0_REG_BASE_S        0x5012A000        /* ID:S3-12, Inter. Type:AHB, Top Address:0x4012AFFF, Size(KB):4K, Clk Domain:PERI_HCLK */
#define SPORT1_REG_BASE_S        0x5012B000        /* ID:S3-13, Inter. Type:AHB, Top Address:0x4012BFFF, Size(KB):4K, Clk Domain:PERI_HCLK */
#define USB_REG_BASE_S           0x50140000        /* ID:S3-14, Inter. Type:AHB, Top Address:0x4017FFFF, Size(KB):256K, Clk Domain:PERI_HCLK */
#define OTPC_REG_BASE_S          0x51000000        /* ID:S4-0, Inter. Type:APB4, Top Address:0x41007FFF, Size(KB):32K, Clk Domain:PERI_LCLK */
#define SYSTEM_CTRL_BASE_S       0x51008000        /* ID:S4-1, Inter. Type:APB4, Top Address:0x4100BFFF, Size(KB):16K, Clk Domain:PERI_LCLK */
#define UART0_REG_BASE_S         0x5100C000        /* ID:S4-2, Inter. Type:APB4, Top Address:0x4100CFFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define UART1_REG_BASE_S         0x5100D000        /* ID:S4-3, Inter. Type:APB4, Top Address:0x4100DFFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define UART2_REG_BASE_S         0x5100E000        /* ID:S4-4, Inter. Type:APB4, Top Address:0x4100EFFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define UARTLOG_REG_BASE_S       0x5100F000        /* ID:S4-5, Inter. Type:APB4, Top Address:0x4100FFFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define GPIO_REG_BASE_S          0x51010000        /* ID:S4-6, Inter. Type:APB4, Top Address:0x41010FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define ADC_REG_BASE_S           0x51011000        /* ID:S4-7, Inter. Type:APB4, Top Address:0x41011FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define CMP_REG_BASE_S           0x51011800        /* ID:S4-7, Inter. Type:APB4, Top Address:0x41011FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define CTC_REG_BASE_S           0x51012000        /* ID:S4-8, Inter. Type:APB4, Top Address:0x41012FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define KSCAN_REG_BASE_S         0x51013000        /* ID:S4-9, Inter. Type:APB4, Top Address:0x41013FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define IPC0_REG_BASE_S          0x51014000        /* ID:S4-10, Inter. Type:APB4, Top Address:0x41014FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define IPC1_REG_BASE_S          0x51014080        /* ID:S4-10, Inter. Type:APB4, Top Address:0x41014FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define IPC_SEMA_BASE_S          0x51014200        /* ID:S4-10, Inter. Type:APB4, Top Address:0x41014FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define DEBUGTIMER_REG_BASE_S    0x51015000        /* ID:S4-11, Inter. Type:APB4, Top Address:0x41015FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define PMC_TIMER_REG_BASE_S     0x51016000        /* ID:S4-12, Inter. Type:APB4, Top Address:0x41016FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER0_REG_BASE_S        0x51017000        /* ID:S4-13, Inter. Type:APB4, Top Address:0x41017FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER1_REG_BASE_S        0x51017200        /* ID:S4-13, Inter. Type:APB4, Top Address:0x41017FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER2_REG_BASE_S        0x51017400        /* ID:S4-13, Inter. Type:APB4, Top Address:0x41017FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER3_REG_BASE_S        0x51017600        /* ID:S4-13, Inter. Type:APB4, Top Address:0x41017FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER4_REG_BASE_S        0x51017800        /* ID:S4-13, Inter. Type:APB4, Top Address:0x41017FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER5_REG_BASE_S        0x51017A00        /* ID:S4-13, Inter. Type:APB4, Top Address:0x41017FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER6_REG_BASE_S        0x51017C00        /* ID:S4-13, Inter. Type:APB4, Top Address:0x41017FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER7_REG_BASE_S        0x51017E00        /* ID:S4-13, Inter. Type:APB4, Top Address:0x41017FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER8_REG_BASE_S        0x51100000        /* ID:S5-0, Inter. Type:APB4, Top Address:0x41100FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER9_REG_BASE_S        0x51100200        /* ID:S5-0, Inter. Type:APB4, Top Address:0x41100FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER10_REG_BASE_S       0x51100400        /* ID:S5-0, Inter. Type:APB4, Top Address:0x41100FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TIMER11_REG_BASE_S       0x51100600        /* ID:S5-0, Inter. Type:APB4, Top Address:0x41100FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define TRNG_REG_BASE_S          0x51101000        /* ID:S5-1, Inter. Type:APB4, Top Address:0x41101FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define RXI300_REG_BASE_S        0x51102000        /* ID:S5-2, Inter. Type:APB4, Top Address:0x41103FFF, Size(KB):8K, Clk Domain:PERI_LCLK */
#define MPC1_BASE_S              0x51102200        /* ID:S5-2, Inter. Type:APB4, Top Address:0x41103FFF, Size(KB):8K, Clk Domain:PERI_LCLK */
#define MPC2_BASE_S              0x51102300        /* ID:S5-2, Inter. Type:APB4, Top Address:0x41103FFF, Size(KB):8K, Clk Domain:PERI_LCLK */
#define PPC_BASE_S               0x51102400        /* ID:S5-2, Inter. Type:APB4, Top Address:0x41103FFF, Size(KB):8K, Clk Domain:PERI_LCLK */
#define RSIP_REG_BASE_S          0x51104000        /* ID:S5-3, Inter. Type:APB4, Top Address:0x41104FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define LEDC_REG_BASE_S          0x51105000        /* ID:S5-4, Inter. Type:APB4, Top Address:0x41105FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define AUDIO_REG_BASE_S         0x51106000        /* ID:S5-5, Inter. Type:APB4, Top Address:0x41106FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define IR_REG_BASE_S            0x51107000        /* ID:S5-6, Inter. Type:APB4, Top Address:0x41107FFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define I2C0_REG_BASE_S          0x51108000        /* ID:S5-7, Inter. Type:APB4, Top Address:0x41109FFF, Size(KB):8K, Clk Domain:PERI_LCLK */
#define I2C1_REG_BASE_S          0x5110A000        /* ID:S5-8, Inter. Type:APB4, Top Address:0x4110BFFF, Size(KB):8K, Clk Domain:PERI_LCLK */
#define PSRAM_BASE_S             0x70000000        /* ID:S6, Inter. Type:AXI4, Top Address:0x6FFFFFFF, Size(KB):256M, Clk Domain:PSRAM_CLK */

/** @} End of group AmebaD_Address_Map */

#define RTC_BASE_S               0x51008A00
#define PINMUX_REG_BASE_S        0x51008800

#define SDM_REG_BASE_S           0x51009000

#define AON_BACKUP_BASE          0x410080E0
#define PMC_BASE                 0x41008300
#define DATA_FLASH_BASE          0x60000000

/**************************************************************************//**
 * @defgroup AmebaD_Peripheral_Declaration  AmebaD HS/LP Peripheral Declarations
 * @{
 *****************************************************************************/
#define RRAM_DEV			((RRAM_TypeDef			*) RETENTION_RAM_BASE)		/*retention mem */

#define UART0_DEV			((UART_TypeDef			*) UART0_REG_BASE)		/*hp uart0 */
#define UART1_DEV			((UART_TypeDef			*) UART1_REG_BASE)		/*hp uart1 */
#define UART2_DEV			((UART_TypeDef			*) UART2_REG_BASE)		/*hp uart2 */
#define UART0_DEV_S			((UART_TypeDef			*) UART0_REG_BASE_S)		/*hp uart0 secure */
#define UART1_DEV_S			((UART_TypeDef			*) UART1_REG_BASE_S)		/*hp uart1 secure*/
#define UART2_DEV_S			((UART_TypeDef			*) UART2_REG_BASE_S)		/*hp uart2 secrue*/

#define LOGUART_DEV			((LOGUART_TypeDef		*) UARTLOG_REG_BASE)
#define LOGUART_DEV_S		((LOGUART_TypeDef		*) UARTLOG_REG_BASE_S)

#define SPI0_DEV			((SPI_TypeDef			*) SPI0_REG_BASE)		/*hp spi0 */
#define SPI1_DEV			((SPI_TypeDef			*) SPI1_REG_BASE)		/*hp spi1 */
#define SPI0_DEV_S			((SPI_TypeDef			*) SPI0_REG_BASE_S)		/*hp spi0 secure */
#define SPI1_DEV_S			((SPI_TypeDef			*) SPI1_REG_BASE_S)		/*hp spi1 secure */

#define PSRAMC_DEV			((SPIC_TypeDef			*) PSRAM_REG_BASE)				/*KM4 */
#define PSRAMPHY_DEV		((PSPHY_TypeDef			*) PSRAM_PHY_REG_BASE)				/*KM4 */

#define SPIC				((SPIC_TypeDef			*) SPI_FLASH_CTRL_BASE)
#define SPIC_COMBO			((SPIC_TypeDef			*) PSRAM_REG_BASE)
#define QSPI				((SPIC_TypeDef			*) QSPI_REG_BASE)
#define PPE_LAYER			((PPE_LAYER_TypeDef		*) PPE_REG_BASE)
#define PPE					((PPE_GLOBAL_TypeDef	*)(PPE_REG_BASE + 0x400))
#define ADC					((ADC_TypeDef			*) ADC_REG_BASE)
#define ADC_S				((ADC_TypeDef			*) ADC_REG_BASE_S)
#define CMP_DEV				((CMP_TypeDef			*) CMP_REG_BASE)
#define CMP_DEV_S			((CMP_TypeDef			*) CMP_REG_BASE_S)
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
#define TIM9				((RTIM_TypeDef			*) TIMER9_REG_BASE)
#define TIM10				((RTIM_TypeDef			*) TIMER10_REG_BASE)
#define TIM11				((RTIM_TypeDef			*) TIMER11_REG_BASE)

#define TIM0_S				((RTIM_TypeDef			*) TIMER0_REG_BASE_S)
#define TIM1_S				((RTIM_TypeDef			*) TIMER1_REG_BASE_S)
#define TIM2_S				((RTIM_TypeDef			*) TIMER2_REG_BASE_S)
#define TIM3_S				((RTIM_TypeDef			*) TIMER3_REG_BASE_S)
#define TIM4_S				((RTIM_TypeDef			*) TIMER4_REG_BASE_S)
#define TIM5_S				((RTIM_TypeDef			*) TIMER5_REG_BASE_S)
#define TIM6_S				((RTIM_TypeDef			*) TIMER6_REG_BASE_S)
#define TIM7_S				((RTIM_TypeDef			*) TIMER7_REG_BASE_S)
#define TIM8_S				((RTIM_TypeDef			*) TIMER8_REG_BASE_S)
#define TIM9_S				((RTIM_TypeDef 			*) TIMER9_REG_BASE_S)
#define TIM10_S				((RTIM_TypeDef			*) TIMER10_REG_BASE_S)
#define TIM11_S				((RTIM_TypeDef			*) TIMER11_REG_BASE_S)

#define PMCTIMER_DEV0		((PMCTIMER_TpyeDef		*) PMC_TIMER_REG_BASE)
#define PMCTIMER_DEV1		((PMCTIMER_TpyeDef		*) (PMC_TIMER_REG_BASE + 0x200))
#define PMCTIMER_DEV0_S		((PMCTIMER_TpyeDef		*) PMC_TIMER_REG_BASE_S)
#define PMCTIMER_DEV1_S		((PMCTIMER_TpyeDef		*) (PMC_TIMER_REG_BASE_S + 0x200))

#define LEDC_DEV			((LEDC_TypeDef			*) LEDC_REG_BASE)
#define LEDC_DEV_S			((LEDC_TypeDef			*) LEDC_REG_BASE_S)

#define IR_DEV				((IR_TypeDef			*) IR_REG_BASE)
#define IR_DEV_S			((IR_TypeDef			*) IR_REG_BASE_S)

#define CAPTOUCH_DEV		((CAPTOUCH_TypeDef		*) CTC_REG_BASE)				/*KM0 */
#define RTC_DEV				((RTC_TypeDef			*) RTC_BASE)					/*KM0 */
#define RTC_DEV_S			((RTC_TypeDef			*) RTC_BASE_S)
#define PINMUX				((PINMUX_TypeDef		*) PINMUX_REG_BASE)			/*KM0 */
#define GPIOA_BASE			((GPIO_TypeDef			*) GPIO_REG_BASE)			/*KM0 */
#define GPIOB_BASE			((GPIO_TypeDef			*) (GPIO_REG_BASE + 0x400))	/*KM0 */
#define GPIOA_BASE_S		((GPIO_TypeDef			*) GPIO_REG_BASE_S)
#define GPIOB_BASE_S		((GPIO_TypeDef			*) (GPIO_REG_BASE_S + 0x400))
#define BACKUP_REG			((BACKUP_REG_TypeDef	*) AON_BACKUP_BASE)			/*AON */
#define PINMUX_S			((PINMUX_TypeDef		*) PINMUX_REG_BASE_S)			/*KM0 */

#define KM4_MPC1			((MPC_TypeDef			*) MPC1_BASE_S)			/*KM4 */
#define KM4_MPC2			((MPC_TypeDef			*) MPC2_BASE_S)			/*KM4 */
#define KM4_PPC				((PPC_TypeDef			*) PPC_BASE_S)			/*KM4 */

#define IPCKM0_DEV 			((IPC_TypeDef 			*) IPC0_REG_BASE)				/*KR4 */
#define IPCKM4_DEV 			((IPC_TypeDef 			*) IPC1_REG_BASE)				/*KM4 */
#define IPCKM0_DEV_S 		((IPC_TypeDef 			*) IPC0_REG_BASE_S)				/*KR4 Secure */
#define IPCKM4_DEV_S 		((IPC_TypeDef 			*) IPC1_REG_BASE_S)				/*KM4 Secure*/

#define GDMA_BASE			((GDMA_TypeDef			*) (GDMA0_REG_BASE))

#define IWDG_DEV			((WDG_TypeDef			*) (IWDG_REG_BASE))
#define KM0_WDG_DEV			((WDG_TypeDef			*) (WDG0_REG_BASE))
#define KM4_S_WDG_DEV		((WDG_TypeDef			*) (WDG1_REG_BASE_S))
#define KM4_NS_WDG_DEV		((WDG_TypeDef			*) (WDG2_REG_BASE))
#define SDM_DEV				((SDM_TypeDef			*) (SDM_REG_BASE))
#define SDM_DEV_S			((SDM_TypeDef			*) (SDM_REG_BASE_S))
#define OTPC_DEV			((OTPC_TypeDef			*) (OTPC_REG_BASE))
#define OTPC_DEV_S			((OTPC_TypeDef			*) (OTPC_REG_BASE_S))
#define DTIMER_DEV			((DEBUGTIMER_TypeDef	*) (DEBUGTIMER_REG_BASE))
#define DTIMER_DEV_S		((DEBUGTIMER_TypeDef	*) (DEBUGTIMER_REG_BASE_S))
#define TRNG_DEV			((TRNG_TypeDef			*) (TRNG_REG_BASE))
#define TRNG_SDEV			((TRNG_TypeDef			*) (TRNG_REG_BASE_S))

#define RSIP_BASE			((RSIP_REG_TypeDef		*) (RSIP_REG_BASE))

#define AUDIO_CODEC_DEV  ((AUDIO_CODEC_TypeDef*) AUDIO_REG_BASE)
#define AUDIO_CODEC_DEV_S  ((AUDIO_CODEC_TypeDef*) AUDIO_REG_BASE_S)       /*audio codec secure */

#define AUDIO_SPORT0_DEV	((AUDIO_SPORT_TypeDef	*) SPORT0_REG_BASE)
#define AUDIO_SPORT1_DEV	((AUDIO_SPORT_TypeDef	*) SPORT1_REG_BASE)
#define AUDIO_SPORT0_DEV_S	((AUDIO_SPORT_TypeDef	*) SPORT0_REG_BASE_S)	/*SPORT0 secure */
#define AUDIO_SPORT1_DEV_S	((AUDIO_SPORT_TypeDef	*) SPORT1_REG_BASE_S)	/*SPORT1 secure */

#define KEYSCAN_DEV 		((KSCAN_TypeDef 		*) KSCAN_REG_BASE)				/*KM4 */
#define KEYSCAN_DEV_S 		((KSCAN_TypeDef 		*) KSCAN_REG_BASE_S)

/** @} End of group AmebaD_Peripheral_Declaration */

#define MSP_RAM_LP					0x20001FFC						/* rom used, can not change */
#define MSPLIM_RAM_HP				0x30009000						/* rom: MSPLIM = 30008000; bootloader/image2 MSPLIM = 30009000 */
#define MSP_RAM_HP					0x30009FFC						/* rom used, can not change */
#define MSP_RAM_HP_NS				0x20004000						/* rom used, can not change */

#define HS_SRAM_S_ADDR_START		HP_SRAM0_BASE_S 				/* rom used, can not change */
#define HS_SRAM_S_ADDR_END			(HP_SRAM0_BASE_S + 0x00FFFFFF)	/* rom used, can not change */

#define HS_BOOT_ADDR_START			(HP_SRAM0_BASE_S +  0x0000A000)		/* rom used, can not change */
#define HS_BOOT_ADDR_END			(HS_BOOT_ADDR_START + 0x00017FFF)	/* rom used, can not change */

#define LS_SRAM_ADDR_START			LP_SRAM_BASE
#define LS_SRAM_ADDR_END			(LP_SRAM_BASE + 0x00020000)

/* margin 512 for lite and 1024 for CA32 */
#if defined(RSICV_CORE_KR4)
#define CONTEXT_SAVE_SIZE	320	/* portCONTEXT_SIZE:66*4 = 288 roundup to 64B aligned */
#elif defined(ARM_CORE_CA32)
#define CONTEXT_SAVE_SIZE	(320 + 1024) /* 15*4 + 32*8: general reg and floating reg */
#elif defined(ARM_CORE_CM4)
#define CONTEXT_SAVE_SIZE	192 /* 15*4 + 16*8: s16~s31 if use float */
#elif defined(ARM_CORE_CM0)
#define CONTEXT_SAVE_SIZE	64	/* not support hw float, 15*4 */
#endif


/** @} End of group AmebaD_Outline */
#endif //_HAL_PLATFORM_