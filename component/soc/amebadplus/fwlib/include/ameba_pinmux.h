/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_PINMUX_H_
#define _AMEBA_PINMUX_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  *		-Every GPIO pin can be set to some function based on pinmux spec.
  *		-Every GPIO pin can set internal pull-up, pull-down based on pinmux spec.
  *
  *====================================================================
  * Pad Control Spec.
  *====================================================================
  *		-[31:18]		reserved
  *		-[17]			pull down resistor enable when system is in sleep
  *		-[16]			pull up resistor enable when system is in sleep
  *		-[13]			srew rate control
  *		-[12]			schmitt trigger enable
  *		-[11]			pad driving strength
  *		-[10]			pull resistor selection
  *		-[9]			pull down resistor enable when system is in active
  *		-[8]			pull up resistor enable when system is in active
  *		-[7:5]			reserved for function id extend
  *		-[4:0]			function id
  *
  *****************************************************************************************
  * How to use Pinmux
  *****************************************************************************************
  *		1. Set the Internal pad function type for  each pin using the follwoing function:
  *			Pinmux_Config(u8 PinName, u32 PinFunc)
  *
  *		2. Set the Internal pad pull type for each pin using the follwoing function:
  *			PAD_PullCtrl(u8 PinName, u8 PullType)
  *			PAD_SleepPullCtrl(u8 PinName, u8 PullType);
  *
  *****************************************************************************************
  * @endverbatim
  */

/** @defgroup PIN
  * @brief PIN driver modules
  * @{
  */

/** @defgroup PINMUX
  * @brief PINMUX driver modules
  * @{
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup PINMUX_Exported_Constants PINMUX Exported Constants
  * @{
  */

/** @defgroup PINMUX_Pin_Name_definitions
  * @note: Pin_Name = (((port)<<5)|(pin))
  * @{
  */
#define _PA_0		(0x00)	//0x00
#define _PA_1		(0x01)	//0x04
#define _PA_2		(0x02)	//0x08
#define _PA_3		(0x03)	//0x0C
#define _PA_4		(0x04)	//0x10
#define _PA_5		(0x05)	//0x14
#define _PA_6		(0x06)	//0x18
#define _PA_7		(0x07)	//0x1C
#define _PA_8		(0x08)	//0x20
#define _PA_9		(0x09)	//0x24
#define _PA_10		(0x0A)	//0x28
#define _PA_11		(0x0B)	//0x2C
#define _PA_12		(0x0C)	//0x30
#define _PA_13		(0x0D)	//0x34
#define _PA_14		(0x0E)	//0x38
#define _PA_15		(0x0F)	//0x3C
#define _PA_16		(0x10)	//0x40
#define _PA_17		(0x11)	//0x44
#define _PA_18		(0x12)	//0x48
#define _PA_19		(0x13)	//0x4C
#define _PA_20		(0x14)	//0x50
#define _PA_21		(0x15)	//0x54
#define _PA_22		(0x16)	//0x58
#define _PA_23		(0x17)	//0x5C
#define _PA_24		(0x18)	//0x60
#define _PA_25		(0x19)	//0x64
#define _PA_26		(0x1A)	//0x68
#define _PA_27		(0x1B)	//0x6C
#define _PA_28		(0x1C)	//0x70
#define _PA_29		(0x1D)	//0x74
#define _PA_30		(0x1E)	//0x78
#define _PA_31		(0x1F)	//0x7C

#define _PB_0		(0x20)	//0x80
#define _PB_1		(0x21)	//0x84
#define _PB_2		(0x22)	//0x88
#define _PB_3		(0x23)	//0x8C
#define _PB_4		(0x24)	//0x90
#define _PB_5		(0x25)	//0x94
#define _PB_6		(0x26)	//0x98
#define _PB_7		(0x27)	//0x9C
#define _PB_8		(0x28)	//0xA0
#define _PB_9		(0x29)	//0xA4
#define _PB_10		(0x2A)	//0xA8
#define _PB_11		(0x2B)	//0xAC
#define _PB_12		(0x2C)	//0xB0
#define _PB_13		(0x2D)	//0xB4
#define _PB_14		(0x2E)	//0xB8
#define _PB_15		(0x2F)	//0xBc
#define _PB_16		(0x30)	//0xC0
#define _PB_17		(0x31)	//0xC4
#define _PB_18		(0x32)	//0xC8
#define _PB_19		(0x33)	//0xCC
#define _PB_20		(0x34)	//0xD0
#define _PB_21		(0x35)	//0xD4
#define _PB_22		(0x36)	//0xD8
#define _PB_23		(0x37)	//0xDC
#define _PB_24		(0x38)	//0xE0
#define _PB_25		(0x39)	//0xE4
#define _PB_26		(0x3A)	//0xE8
#define _PB_27		(0x3B)	//0xEC
#define _PB_28		(0x3C)	//0xF0
#define _PB_29		(0x3D)	//0xF4
#define _PB_30		(0x3E)	//0xF8
#define _PB_31		(0x3F)	//0xFC

#define _PNC		(0xFFFF)
/** @} */

/** @defgroup PINMUX_Port_and_Pin_definitions
  * @{
  */
#define PORT_NUM(pin)		((pin>>5) & 0x03)
#define PIN_NUM(pin)		(pin & 0x1f)
/** @} */

/** @defgroup PINMUX_Function_definitions
  * @{
  */
#define PINMUX_FUNCTION_GPIO                               0
#define PINMUX_FUNCTION_LOG_UART                           1
#define PINMUX_FUNCTION_UART                               1
#define PINMUX_FUNCTION_SPIC0_FLASH                        2
#define PINMUX_FUNCTION_SPIC1_FLASH                        3
#define PINMUX_FUNCTION_SPIC1_PSRAM                        4
#define PINMUX_FUNCTION_OSPI                               5
#define PINMUX_FUNCTION_QSPI                               5
#define PINMUX_FUNCTION_ADC                                6
#define PINMUX_FUNCTION_CAP_TOUCH                          6
#define PINMUX_FUNCTION_SIC                                7
#define PINMUX_FUNCTION_SPI                                8
#define PINMUX_FUNCTION_SWD                                9
#define PINMUX_FUNCTION_SDIO                               10
#define PINMUX_FUNCTION_ANT_DIV                            11
#define PINMUX_FUNCTION_EXT_BT                             12
#define PINMUX_FUNCTION_BT_IO                              13
#define PINMUX_FUNCTION_BT                                 14
#define PINMUX_FUNCTION_EXT_ZIGBEE                         15
#define PINMUX_FUNCTION_TIMER                              16
#define PINMUX_FUNCTION_USB                                17
#define PINMUX_FUNCTION_DEBUG                              18
#define PINMUX_FUNCTION_UART0_TXD                          19
#define PINMUX_FUNCTION_UART0_RXD                          20
#define PINMUX_FUNCTION_UART0_CTS                          21
#define PINMUX_FUNCTION_UART0_RTS                          22
#define PINMUX_FUNCTION_UART1_TXD                          23
#define PINMUX_FUNCTION_UART1_RXD                          24
#define PINMUX_FUNCTION_UART2_TXD                          25
#define PINMUX_FUNCTION_UART2_RXD                          26
#define PINMUX_FUNCTION_UART2_CTS                          27
#define PINMUX_FUNCTION_UART2_RTS                          28
#define PINMUX_FUNCTION_SPI1_CLK                           29
#define PINMUX_FUNCTION_SPI1_MISO                          30
#define PINMUX_FUNCTION_SPI1_MOSI                          31
#define PINMUX_FUNCTION_SPI1_CS                            32
#define PINMUX_FUNCTION_LEDC                               33
#define PINMUX_FUNCTION_I2S0_MCLK                          34
#define PINMUX_FUNCTION_I2S0_BCLK                          35
#define PINMUX_FUNCTION_I2S0_WS                            36
#define PINMUX_FUNCTION_I2S0_DIO0                          37
#define PINMUX_FUNCTION_I2S0_DIO1                          38
#define PINMUX_FUNCTION_I2S0_DIO2                          39
#define PINMUX_FUNCTION_I2S0_DIO3                          40
#define PINMUX_FUNCTION_I2S1_MCLK                          41
#define PINMUX_FUNCTION_I2S1_BCLK                          42
#define PINMUX_FUNCTION_I2S1_WS                            43
#define PINMUX_FUNCTION_I2S1_DIO0                          44
#define PINMUX_FUNCTION_I2S1_DIO1                          45
#define PINMUX_FUNCTION_I2S1_DIO2                          46
#define PINMUX_FUNCTION_I2S1_DIO3                          47
#define PINMUX_FUNCTION_I2C0_SCL                           48
#define PINMUX_FUNCTION_I2C0_SDA                           49
#define PINMUX_FUNCTION_I2C1_SCL                           50
#define PINMUX_FUNCTION_I2C1_SDA                           51
#define PINMUX_FUNCTION_PWM0                               52
#define PINMUX_FUNCTION_PWM1                               53
#define PINMUX_FUNCTION_PWM2                               54
#define PINMUX_FUNCTION_PWM3                               55
#define PINMUX_FUNCTION_PWM4                               56
#define PINMUX_FUNCTION_PWM5                               57
#define PINMUX_FUNCTION_PWM6                               58
#define PINMUX_FUNCTION_PWM7                               59
#define PINMUX_FUNCTION_BT_UART_TXD                        60
#define PINMUX_FUNCTION_BT_UART_RTS                        61
#define PINMUX_FUNCTION_DMIC_CLK                           62
#define PINMUX_FUNCTION_DMIC_DATA                          63
#define PINMUX_FUNCTION_IR_TX                              64
#define PINMUX_FUNCTION_IR_RX                              65
#define PINMUX_FUNCTION_KEY_ROW0                           66
#define PINMUX_FUNCTION_KEY_ROW1                           67
#define PINMUX_FUNCTION_KEY_ROW2                           68
#define PINMUX_FUNCTION_KEY_ROW3                           69
#define PINMUX_FUNCTION_KEY_ROW4                           70
#define PINMUX_FUNCTION_KEY_ROW5                           71
#define PINMUX_FUNCTION_KEY_ROW6                           72
#define PINMUX_FUNCTION_KEY_ROW7                           73
#define PINMUX_FUNCTION_KEY_COL0                           74
#define PINMUX_FUNCTION_KEY_COL1                           75
#define PINMUX_FUNCTION_KEY_COL2                           76
#define PINMUX_FUNCTION_KEY_COL3                           77
#define PINMUX_FUNCTION_KEY_COL4                           78
#define PINMUX_FUNCTION_KEY_COL5                           79
#define PINMUX_FUNCTION_KEY_COL6                           80
#define PINMUX_FUNCTION_KEY_COL7                           81
/** @} */

/** @defgroup PINMUX_Peripheral_Location_definitions
  * @note just used by function PINMUX_Ctrl
  * @{
  */
#define PINMUX_S0		(0)
#define PINMUX_S1		(1)
#define PINMUX_S2		(2)
#define PINMUX_S3		(3)
#define PINMUX_S4		(4)
#define PINMUX_S5		(5)
/** @} */

/** @} */

/** @defgroup PINMUX_Exported_Functions PINMUX Exported Functions
  * @{
  */
_LONG_CALL_ void Pinmux_Config(u8 PinName, u32 PinFunc);
_LONG_CALL_ u32 Pinmux_ConfigGet(u8 PinName);
_LONG_CALL_ void Pinmux_UartLogCtrl(u32  PinLocation, BOOL   Operation);
_LONG_CALL_ void Pinmux_BootSpicCtrl(u32  PinLocation, BOOL Operation);
_LONG_CALL_ void Pinmux_ComboSpicCtrl(u32  PinLocation, BOOL Operation);
_LONG_CALL_ void Pinmux_Swdoff(void);
/** @} */

/** @} */


/** @defgroup PAD
  * @brief PAD driver modules
  * @{
  */

/** @defgroup PAD_Exported_Constants PAD Exported Constants
  * @{
  */

/** @defgroup PAD_Pull_Resistor_definitions
  * @{
  */
#define PAD_Resistor_LARGE		0x00 /*!< PAD Resistor LARGE */
#define PAD_Resistor_SMALL		0x01 /*!< PAD Resistor SMALL */
/** @} */

/** @defgroup PINMUX_PAD_DrvStrength_definitions
  * @{
  */
#define PAD_DRV_ABILITITY_LOW			(0)
#define PAD_DRV_ABILITITY_HIGH			(1)
/** @} */

/** @} */


/** @defgroup PAD_Exported_Functions PAD Exported Functions
  * @{
  */

_LONG_CALL_ void PAD_CMD(u8 PinName, u8 NewStatus);
_LONG_CALL_ void PAD_DrvStrength(u8 PinName, u32 DrvStrength);
_LONG_CALL_ void PAD_SchmitCtrl(u8 PinName, u32 NewState);
_LONG_CALL_ void PAD_SlewRateCtrl(u8 PinName, u32 NewState);
_LONG_CALL_ void PAD_PullCtrl(u8 PinName, u8 PullType);
_LONG_CALL_ void PAD_SleepPullCtrl(u8 PinName, u8 PullType);
_LONG_CALL_ void PAD_SpicCtrl(u32  PinLocation);
_LONG_CALL_ void PAD_ResistorCtrl(u8 PinName, u8 RType);
_LONG_CALL_ void PAD_InputCtrl(u8 PinName, u32 NewState);
/** @} */

/** @} */

/** @} */

/** @} */


/* PINMUX_TESTID_TRAP_ICFG_definitions */
#define TESTID_MCM_FLASH_PG1B_ICFG			4
#define TESTID_FORCE_USE_S0_ICFG			5
#define TESTID_FLASH_PROBE_G0_ICFG			8
#define TESTID_FLASH_PROBE_G1_ICFG			9
#define TESTID_FLASH_PROBE_G0G1_ICFG		10

#endif   //_AMEBA_PINMUX_H_