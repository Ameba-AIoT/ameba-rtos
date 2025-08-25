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

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/** @defgroup PINMUX_Pin_Name_definitions
 * @note: Pin_Name = (((port)<<5)|(pin))
 * @{
 */
#define _PA_0           (0x00)  //0x0
#define _PA_1           (0x01)  //0x4
#define _PA_2           (0x02)  //0x8
#define _PA_3           (0x03)  //0xC
#define _PA_4           (0x04)  //0x10
#define _PA_5           (0x05)  //0x14
#define _PA_6           (0x06)  //0x18
#define _PA_7           (0x07)  //0x1C
#define _PA_8           (0x08)  //0x20
#define _PA_9           (0x09)  //0x24
#define _PA_10          (0x0A)  //0x28
#define _PA_11          (0x0B)  //0x2C
#define _PA_12          (0x0C)  //0x30
#define _PA_13          (0x0D)  //0x34
#define _PA_14          (0x0E)  //0x38
#define _PA_15          (0x0F)  //0x3C
#define _PA_16          (0x10)  //0x40
#define _PA_17          (0x11)  //0x44
#define _PA_18          (0x12)  //0x48
#define _PA_19          (0x13)  //0x4C
#define _PA_20          (0x14)  //0x50
#define _PA_21          (0x15)  //0x54
#define _PA_22          (0x16)  //0x58
#define _PA_23          (0x17)  //0x5C
#define _PA_24          (0x18)  //0x60
#define _PA_25          (0x19)  //0x64
#define _PA_26          (0x1A)  //0x68
#define _PA_27          (0x1B)  //0x6C
#define _PA_28          (0x1C)  //0x70
#define _PA_29          (0x1D)  //0x74
#define _PA_30          (0x1E)  //0x78
#define _PA_31          (0x1F)  //0x7C

#define _PB_0           (0x20)  //0x80
#define _PB_1           (0x21)  //0x84
#define _PB_2           (0x22)  //0x88
#define _PB_3           (0x23)  //0x8C
#define _PB_4           (0x24)  //0x90
#define _PB_5           (0x25)  //0x94
#define _PB_6           (0x26)  //0x98
#define _PB_7           (0x27)  //0x9C
#define _PB_8           (0x28)  //0xA0
#define _PB_9           (0x29)  //0xA4
#define _PB_10          (0x2A)  //0xA8
#define _PB_11          (0x2B)  //0xAC
#define _PB_12          (0x2C)  //0xB0
#define _PB_13          (0x2D)  //0xB4
#define _PB_14          (0x2E)  //0xB8
#define _PB_15          (0x2F)  //0xBC
#define _PB_16          (0x30)  //0xC0
#define _PB_17          (0x31)  //0xC4
#define _PB_18          (0x32)  //0xC8
#define _PB_19          (0x33)  //0xCC
#define _PB_20          (0x34)  //0xD0
#define _PB_21          (0x35)  //0xD4
#define _PB_22          (0x36)  //0xD8
#define _PB_23          (0x37)  //0xDC
#define _PB_24          (0x38)  //0xE0
#define _PB_25          (0x39)  //0xE4
#define _PB_26          (0x3A)  //0xE8
#define _PB_27          (0x3B)  //0xEC
#define _PB_28          (0x3C)  //0xF0
#define _PB_29          (0x3D)  //0xF4
#define _PB_30          (0x3E)  //0xF8
#define _PB_31          (0x3F)  //0xFC

#define _PC_0           (0x40)  //0x100
#define _PC_1           (0x41)  //0x104
#define _PC_2           (0x42)  //0x108
#define _PC_3           (0x43)  //0x10C
#define _PC_4           (0x44)  //0x110
#define _PC_5           (0x45)  //0x114
#define _PC_6           (0x46)  //0x118
#define _PC_7           (0x47)  //0x11C
#define _PC_8           (0x48)  //0x120
#define _PC_9           (0x49)  //0x124
#define _PC_10          (0x4A)  //0x128
#define _PC_11          (0x4B)  //0x12C
#define _PC_12          (0x4C)  //0x130
#define _PC_13          (0x4D)  //0x134
#define _PC_14          (0x4E)  //0x138
#define _PC_15          (0x4F)  //0x13C
#define _PC_16          (0x50)  //0x140
#define _PC_17          (0x51)  //0x144
#define _PC_18          (0x52)  //0x148
#define _PC_19          (0x53)  //0x14C
#define _PC_20          (0x54)  //0x150
#define _PC_21          (0x55)  //0x154
#define _PC_22          (0x56)  //0x158
#define _PC_23          (0x57)  //0x15C
#define _PC_24          (0x58)  //0x160
#define _PC_25          (0x59)  //0x164
#define _PC_26          (0x5A)  //0x168
#define _PC_27          (0x5B)  //0x16C
#define _PC_28          (0x5C)  //0x170
#define _PC_29          (0x5D)  //0x174

#define _PNC           (0xFFFF)
#define PIN_TOTAL_NUM  (0x5E)
/** @} */

/** @defgroup PINMUX_Function_definitions
 * @{
 */
#define PINMUX_FUNCTION_GPIO                0
#define PINMUX_FUNCTION_LOG_UART            1
#define PINMUX_FUNCTION_SPIC0_FLASH         2
#define PINMUX_FUNCTION_SPIC1_FLASH         3
#define PINMUX_FUNCTION_SPIC1_PSRAM         4
#define PINMUX_FUNCTION_ADC                 5
#define PINMUX_FUNCTION_CAP_TOUCH           5
#define PINMUX_FUNCTION_TSSI                6
#define PINMUX_FUNCTION_USB                 7
#define PINMUX_FUNCTION_RMII                8
#define PINMUX_FUNCTION_SPI0                9
#define PINMUX_FUNCTION_SPI1                10
#define PINMUX_FUNCTION_SWD                 11
#define PINMUX_FUNCTION_SDIO_MST            12
#define PINMUX_FUNCTION_SDIO_SLV            13
#define PINMUX_FUNCTION_SIC                 14
#define PINMUX_FUNCTION_BT_IO               15
#define PINMUX_FUNCTION_BT                  16
#define PINMUX_FUNCTION_DEBUG               17
#define PINMUX_FUNCTION_I2S0_BCLK           32
#define PINMUX_FUNCTION_I2S0_MCLK           33
#define PINMUX_FUNCTION_I2S0_WS             34
#define PINMUX_FUNCTION_I2S0_DIO0           35
#define PINMUX_FUNCTION_I2S0_DIO1           36
#define PINMUX_FUNCTION_I2S0_DIO2           37
#define PINMUX_FUNCTION_I2S0_DIO3           38
#define PINMUX_FUNCTION_LCD_RGB_DCLK        39
#define PINMUX_FUNCTION_LCD_MCU_WR          39
#define PINMUX_FUNCTION_LCD_RGB_DE          40
#define PINMUX_FUNCTION_LCD_MCU_CSX         40
#define PINMUX_FUNCTION_LCD_RGB_HSYNC       41
#define PINMUX_FUNCTION_LCD_MCU_RD          41
#define PINMUX_FUNCTION_LCD_RGB_TE          42
#define PINMUX_FUNCTION_LCD_MCU_TE          42
#define PINMUX_FUNCTION_LCD_RGB_VSYNC       43
#define PINMUX_FUNCTION_LCD_MCU_VSYNC       43
#define PINMUX_FUNCTION_LCD_MCU_DCX         44
#define PINMUX_FUNCTION_LCD_D0              45
#define PINMUX_FUNCTION_LCD_D1              46
#define PINMUX_FUNCTION_LCD_D2              47
#define PINMUX_FUNCTION_LCD_D3              48
#define PINMUX_FUNCTION_LCD_D4              49
#define PINMUX_FUNCTION_LCD_D5              50
#define PINMUX_FUNCTION_LCD_D6              51
#define PINMUX_FUNCTION_LCD_D7              52
#define PINMUX_FUNCTION_LCD_D8              53
#define PINMUX_FUNCTION_LCD_D9              54
#define PINMUX_FUNCTION_LCD_D10             55
#define PINMUX_FUNCTION_LCD_D11             56
#define PINMUX_FUNCTION_LCD_D12             57
#define PINMUX_FUNCTION_LCD_D13             58
#define PINMUX_FUNCTION_LCD_D14             59
#define PINMUX_FUNCTION_LCD_D15             60
#define PINMUX_FUNCTION_LCD_D16             61
#define PINMUX_FUNCTION_LCD_D17             62
#define PINMUX_FUNCTION_LCD_D18             63
#define PINMUX_FUNCTION_LCD_D19             64
#define PINMUX_FUNCTION_LCD_D20             65
#define PINMUX_FUNCTION_LCD_D21             66
#define PINMUX_FUNCTION_LCD_D22             67
#define PINMUX_FUNCTION_LCD_D23             68
#define PINMUX_FUNCTION_SD_M_CLK            69
#define PINMUX_FUNCTION_SD_M_CMD            70
#define PINMUX_FUNCTION_SD_M_D0             71
#define PINMUX_FUNCTION_SD_M_D1             72
#define PINMUX_FUNCTION_SD_M_D2             73
#define PINMUX_FUNCTION_SD_M_D3             74
#define PINMUX_FUNCTION_SPI0_CLK            75
#define PINMUX_FUNCTION_SPI0_MISO           76
#define PINMUX_FUNCTION_SPI0_MOSI           77
#define PINMUX_FUNCTION_SPI0_CS             78
#define PINMUX_FUNCTION_SPI1_CLK            79
#define PINMUX_FUNCTION_SPI1_MISO           80
#define PINMUX_FUNCTION_SPI1_MOSI           81
#define PINMUX_FUNCTION_SPI1_CS             82
#define PINMUX_FUNCTION_SWD_CLK             83
#define PINMUX_FUNCTION_SWD_DAT             84
#define PINMUX_FUNCTION_DMIC_CLK            85
#define PINMUX_FUNCTION_DMIC_DATA           86
#define PINMUX_FUNCTION_LED_SCL             87
#define PINMUX_FUNCTION_LED_SDA             88
#define PINMUX_FUNCTION_I2C0_SCL            89
#define PINMUX_FUNCTION_I2C0_SDA            90
#define PINMUX_FUNCTION_I2C1_SCL            91
#define PINMUX_FUNCTION_I2C1_SDA            92
#define PINMUX_FUNCTION_RMII_MDIO           93
#define PINMUX_FUNCTION_RMII_MDC            94
#define PINMUX_FUNCTION_UART0_TXD           95
#define PINMUX_FUNCTION_UART0_RXD           96
#define PINMUX_FUNCTION_UART0_CTS           97
#define PINMUX_FUNCTION_UART0_RTS           98
#define PINMUX_FUNCTION_UART1_TXD           99
#define PINMUX_FUNCTION_UART1_RXD           100
#define PINMUX_FUNCTION_UART2_TXD           101
#define PINMUX_FUNCTION_UART2_RXD           102
#define PINMUX_FUNCTION_UART3_TXD           103
#define PINMUX_FUNCTION_UART3_RXD           104
#define PINMUX_FUNCTION_UART3_CTS           105
#define PINMUX_FUNCTION_UART3_RTS           106
#define PINMUX_FUNCTION_A2C0_TX             107
#define PINMUX_FUNCTION_A2C0_RX             108
#define PINMUX_FUNCTION_A2C1_TX             109
#define PINMUX_FUNCTION_A2C1_RX             110
#define PINMUX_FUNCTION_TIM4_PWM0           111
#define PINMUX_FUNCTION_TIM4_PWM1           112
#define PINMUX_FUNCTION_TIM4_PWM2           113
#define PINMUX_FUNCTION_TIM4_PWM3           114
#define PINMUX_FUNCTION_TIM5_PWM0           115
#define PINMUX_FUNCTION_TIM5_PWM1           116
#define PINMUX_FUNCTION_TIM5_PWM2           117
#define PINMUX_FUNCTION_TIM5_PWM3           118
#define PINMUX_FUNCTION_TIM6_PWM0           119
#define PINMUX_FUNCTION_TIM6_PWM1           120
#define PINMUX_FUNCTION_TIM6_PWM2           121
#define PINMUX_FUNCTION_TIM6_PWM3           122
#define PINMUX_FUNCTION_TIM7_PWM0           123
#define PINMUX_FUNCTION_TIM7_PWM1           124
#define PINMUX_FUNCTION_TIM7_PWM2           125
#define PINMUX_FUNCTION_TIM7_PWM3           126
#define PINMUX_FUNCTION_PWM_TIM4_TRIG       127
#define PINMUX_FUNCTION_PWM_TIM5_TRIG       128
#define PINMUX_FUNCTION_PWM_TIM6_TRIG       129
#define PINMUX_FUNCTION_PWM_TIM7_TRIG       130
#define PINMUX_FUNCTION_CAPT_TIM8_TRIG      131
#define PINMUX_FUNCTION_IR_TX               132
#define PINMUX_FUNCTION_IR_RX               133
#define PINMUX_FUNCTION_ANT_SEL_P           134
#define PINMUX_FUNCTION_ANT_SEL_N           135
#define PINMUX_FUNCTION_TRSW_P              136
#define PINMUX_FUNCTION_TRSW_N              137
#define PINMUX_FUNCTION_PA_EN0              138
#define PINMUX_FUNCTION_LNA_EN0             139
#define PINMUX_FUNCTION_PA_EN1              140
#define PINMUX_FUNCTION_LNA_EN1             141
#define PINMUX_FUNCTION_BT_CLK_REQ          142
#define PINMUX_FUNCTION_WLAN_ACT            143
#define PINMUX_FUNCTION_BT_ACT              144
#define PINMUX_FUNCTION_BT_STE              145
#define PINMUX_FUNCTION_BT_CK               146
#define PINMUX_FUNCTION_ZB_REQ              147
#define PINMUX_FUNCTION_ZB_GRANT            148
#define PINMUX_FUNCTION_ZB_PRI              149
#define PINMUX_FUNCTION_BT_ANT_SW0          150
#define PINMUX_FUNCTION_BT_ANT_SW1          151
#define PINMUX_FUNCTION_BT_ANT_SW2          152
#define PINMUX_FUNCTION_BT_ANT_SW3          153
#define PINMUX_FUNCTION_EXT_CLK50M_IN       154
#define PINMUX_FUNCTION_EXT_CLK_OUT         155
#define PINMUX_FUNCTION_UART_LOG_RXD        156
#define PINMUX_FUNCTION_UART_LOG_TXD        157
#define PINMUX_FUNCTION_SIC_CLK             158
#define PINMUX_FUNCTION_SIC_DAT             159
#define PINMUX_FUNCTION_BT_FWLOG            160
#define PINMUX_FUNCTION_BT_I2C_SDA          161
#define PINMUX_FUNCTION_BT_I2C_SCL          162
/** @} */

#define UART_LOG_RXD                        _PA_2
#define UART_LOG_TXD                        _PB_20
#define SWD_CLK                             _PA_18
#define SWD_DAT                             _PA_19

// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */

//Please add your defination here
/* Exported Types --------------------------------------------------------*/

/** @defgroup PINMUX_Port_and_Pin_definitions
 * @{
 */
#define PORT_NUM(pin)		((pin>>5) & 0x03)
#define PIN_NUM(pin)		(pin & 0x1f)
/** @} */

/** @defgroup PINMUX_PinName_definition
 * @{
 */
#define PinName(port, pin)		(((port) & 0x3) << 5 | ((pin) & 0x1f) << 0)
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
_LONG_CALL_ void Pinmux_UartLogCtrl(UNUSED_WARN_DIS u32  PinLocation, bool   Operation);
_LONG_CALL_ void Pinmux_BootSpicCtrl(u32  PinLocation, bool Operation);
_LONG_CALL_ void Pinmux_ComboSpicCtrl(u32  PinLocation, bool Operation);
_LONG_CALL_ void Pinmux_Swdoff(void);
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAGREEN2_PINMUX
 * @{
 * @brief AMEBAGREEN2_PINMUX Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t REG_GPIOx[PIN_TOTAL_NUM];			/*!< Pad control register */
} PINMUX_TypeDef;
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
#define PAD_DRV_ABILITITY_LOW				(0)
#define PAD_DRV_ABILITITY_MID_LOW			(1)
#define PAD_DRV_ABILITITY_MID_HIGH			(2)
#define PAD_DRV_ABILITITY_HIGH				(3)
/** @} */

/** @defgroup PINMUX_PAD_SlewRate_definitions
 * @{
 */
#define PAD_SlewRate_Fast				(0)
#define PAD_SlewRate_Slow				(1)
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
#define TESTID_MCM_FLASH_PG1B_ICFG			3
#define TESTID_FORCE_USE_S0_ICFG		  	4
#define TESTID_FLASH_PROBE_G0_ICFG			8
#define TESTID_FLASH_PROBE_G1_ICFG			9
#define TESTID_FLASH_PROBE_G0G1_ICFG		10
/* MANUAL_GEN_END */

#endif   //_AMEBA_PINMUX_H_
