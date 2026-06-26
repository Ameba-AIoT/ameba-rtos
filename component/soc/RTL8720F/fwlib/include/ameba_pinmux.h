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
*		-[13]			slew rate control
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
*		1. Set the Internal pad function type for  each pin using the following function:
*			Pinmux_Config(u8 PinName, u32 PinFunc)
*
*		2. Set the Internal pad pull type for each pin using the following function:
*			PAD_PullCtrl(u8 PinName, u8 PullType)
*			PAD_SleepPullCtrl(u8 PinName, u8 PullType);
*
*****************************************************************************************
* @endverbatim
*/

/** @defgroup PIN PIN
 * @brief PIN driver modules
 * @{
 */


/** @defgroup PINMUX PINMUX
 * @brief PINMUX driver modules
 * @{
 */

/// @cond
/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/** @defgroup PINMUX_Pin_Name_definitions PINMUX Pin Name Definitions
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

#define _PNC           (0xFFFF)
#define PIN_TOTAL_NUM  (0x25)
/** @} */

/** @defgroup PINMUX_Function_definitions PINMUX Function Definitions
  * @{
  */
#define PINMUX_FUNCTION_GPIO                0
#define PINMUX_FUNCTION_LOG_UART            1
#define PINMUX_FUNCTION_SPIC0_FLASH         2
#define PINMUX_FUNCTION_SPIC1_FLASH         3
#define PINMUX_FUNCTION_SPIC1_PSRAM         4
#define PINMUX_FUNCTION_ADC                 5
#define PINMUX_FUNCTION_SPI0                6
#define PINMUX_FUNCTION_SPI1                7
#define PINMUX_FUNCTION_SWD                 8
#define PINMUX_FUNCTION_SDIO_DEVICE         9
#define PINMUX_FUNCTION_SIC                 10
#define PINMUX_FUNCTION_DEBUG               11
#define PINMUX_FUNCTION_I2S0_BCLK           32
#define PINMUX_FUNCTION_I2S0_MCLK           33
#define PINMUX_FUNCTION_I2S0_WS             34
#define PINMUX_FUNCTION_I2S0_DIN            35
#define PINMUX_FUNCTION_I2S0_DOUT           36
#define PINMUX_FUNCTION_SPI0_CLK            37
#define PINMUX_FUNCTION_SPI0_MISO           38
#define PINMUX_FUNCTION_SPI0_MOSI           39
#define PINMUX_FUNCTION_SPI0_CS             40
#define PINMUX_FUNCTION_SPI1_CLK            41
#define PINMUX_FUNCTION_SPI1_MISO           42
#define PINMUX_FUNCTION_SPI1_MOSI           43
#define PINMUX_FUNCTION_SPI1_CS             44
#define PINMUX_FUNCTION_TIM4_PWM0           45
#define PINMUX_FUNCTION_TIM4_PWM1           46
#define PINMUX_FUNCTION_TIM4_PWM2           47
#define PINMUX_FUNCTION_TIM4_PWM3           48
#define PINMUX_FUNCTION_TIM5_PWM0           49
#define PINMUX_FUNCTION_TIM5_PWM1           50
#define PINMUX_FUNCTION_TIM5_PWM2           51
#define PINMUX_FUNCTION_TIM5_PWM3           52
#define PINMUX_FUNCTION_TIM5_PWM4           53
#define PINMUX_FUNCTION_TIM5_PWM5           54
#define PINMUX_FUNCTION_PWM_TIM4_TRIG       55
#define PINMUX_FUNCTION_PWM_TIM5_TRIG       56
#define PINMUX_FUNCTION_PWM_TIM6_TRIG       57
#define PINMUX_FUNCTION_PWM_TIM5_BRAKE      58
#define PINMUX_FUNCTION_I2C0_SCL            59
#define PINMUX_FUNCTION_I2C0_SDA            60
#define PINMUX_FUNCTION_I2C1_SCL            61
#define PINMUX_FUNCTION_I2C1_SDA            62
#define PINMUX_FUNCTION_SWD_CLK             63
#define PINMUX_FUNCTION_SWD_DAT             64
#define PINMUX_FUNCTION_UART0_TXD           65
#define PINMUX_FUNCTION_UART0_RXD           66
#define PINMUX_FUNCTION_UART0_CTS           67
#define PINMUX_FUNCTION_UART0_RTS           68
#define PINMUX_FUNCTION_UART1_TXD           69
#define PINMUX_FUNCTION_UART1_RXD           70
#define PINMUX_FUNCTION_UART1_CTS           71
#define PINMUX_FUNCTION_UART1_RTS           72
#define PINMUX_FUNCTION_UART2_TXD           73
#define PINMUX_FUNCTION_UART2_RXD           74
#define PINMUX_FUNCTION_IR_TX               75
#define PINMUX_FUNCTION_IR_RX               76
#define PINMUX_FUNCTION_ANT_SEL_P           77
#define PINMUX_FUNCTION_ANT_SEL_N           78
#define PINMUX_FUNCTION_GNT_WL              79
#define PINMUX_FUNCTION_GNT_BT              80
#define PINMUX_FUNCTION_BT_CLK_REQ          81
#define PINMUX_FUNCTION_WLAN_ACT            82
#define PINMUX_FUNCTION_BT_ACT              83
#define PINMUX_FUNCTION_BT_STE              84
#define PINMUX_FUNCTION_BT_CK               85
#define PINMUX_FUNCTION_ZB_REQ              86
#define PINMUX_FUNCTION_ZB_GRANT            87
#define PINMUX_FUNCTION_ZB_PRI              88
/** @} */

#define UART_LOG_TXD                        _PA_19
#define UART_LOG_RXD                        _PA_20
#define SWD_CLK                             _PA_15
#define SWD_DAT                             _PA_16

#define FLASH_PIN_EXTERNAL_CSN                   _PA_7
#define FLASH_PIN_EXTERNAL_D1                    _PA_8
#define FLASH_PIN_EXTERNAL_D2                    _PA_9
#define FLASH_PIN_EXTERNAL_D0                    _PA_10
#define FLASH_PIN_EXTERNAL_CLK                   _PA_11
#define FLASH_PIN_EXTERNAL_D3                    _PA_12
#define FLASH_PIN_INTERNAL_D2                    _PA_31
#define FLASH_PIN_INTERNAL_CSN                   _PB_0
#define FLASH_PIN_INTERNAL_D1                    _PB_1
#define FLASH_PIN_INTERNAL_D3                    _PB_2
#define FLASH_PIN_INTERNAL_D0                    _PB_3
#define FLASH_PIN_INTERNAL_CLK                   _PB_4

#define PSRAM_START_PIN                 _PA_25
#define PSRAM_END_PIN                   _PB_4

// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */
/// @endcond

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your definition here

#define PSRAM_PIN_CLK                 _PA_25   /*!< PSRAM clock pin assignment. */
#define PSRAM_PIN_DQ4                 _PA_26   /*!< PSRAM data pin DQ4 assignment. */
#define PSRAM_PIN_DQ5                 _PA_27   /*!< PSRAM data pin DQ5 assignment. */
#define PSRAM_PIN_DQ6                 _PA_28   /*!< PSRAM data pin DQ6 assignment. */
#define PSRAM_PIN_DQ7                 _PA_29   /*!< PSRAM data pin DQ7 assignment. */
#define PSRAM_PIN_RWDS                _PA_30   /*!< PSRAM read/write data strobe pin assignment. */
#define PSRAM_PIN_RESETN              _PA_31   /*!< PSRAM reset pin assignment. */
#define PSRAM_PIN_DQ0                 _PB_0    /*!< PSRAM data pin DQ0 assignment. */
#define PSRAM_PIN_DQ1                 _PB_1    /*!< PSRAM data pin DQ1 assignment. */
#define PSRAM_PIN_DQ2                 _PB_2    /*!< PSRAM data pin DQ2 assignment. */
#define PSRAM_PIN_DQ3                 _PB_3    /*!< PSRAM data pin DQ3 assignment. */
#define PSRAM_PIN_CSN                 _PB_4    /*!< PSRAM chip select pin assignment. */

#define XDEBUG_GPIO                         _PA_21  /*!< Debug GPIO pin used by ROM; do not change. */

/* Exported Types --------------------------------------------------------*/

/** @defgroup PINMUX_Port_and_Pin_definitions PINMUX Port and Pin Definitions
 * @{
 */
#define PORT_NUM(pin)		((pin>>5) & 0x03)   /*!< Extract port number from pin name. */
#define PIN_NUM(pin)		(pin & 0x1f)   /*!< Extract pin number from pin name. */
/** @} */

/// @cond
/** @defgroup PINMUX_PinName_definition PINMUX Pin Name Definition
 * @{
 */
#define PinName(port, pin)		(((port) & 0x3) << 5 | ((pin) & 0x1f) << 0)
/** @} */
/// @endcond

/** @defgroup PINMUX_Peripheral_Location_definitions PINMUX Peripheral Location Definitions
 * @note Just used by function PINMUX_Ctrl
 * @{
 */
#define PINMUX_S0		(0)   /*!< Peripheral pinmux location selection 0. */
#define PINMUX_S1		(1)   /*!< Peripheral pinmux location selection 1. */
#define PINMUX_S2		(2)   /*!< Peripheral pinmux location selection 2. */
#define PINMUX_S3		(3)   /*!< Peripheral pinmux location selection 3. */
#define PINMUX_S4		(4)   /*!< Peripheral pinmux location selection 4. */
#define PINMUX_S5		(5)   /*!< Peripheral pinmux location selection 5. */
/** @} */

/** @defgroup Flash_Pad_Location_definitions Flash Pad Location Definitions
 * @note Just used by function Pinmux_BootSpicCtrl / Pinmux_ComboSpicCtrl
 * @{
 */
#define PINMUX_EXTERNAL		(0)   /*!< Use external flash pad location. */
#define PINMUX_INTERNAL		(1)   /*!< Use internal flash pad location. */
/** @} */

/** @} */

_LONG_CALL_ void Pinmux_UartLogCtrl(UNUSED_WARN_DIS u32  PinLocation, bool   Operation);
_LONG_CALL_ void Pinmux_BootSpicCtrl(u32  PinLocation, bool Operation);
_LONG_CALL_ void Pinmux_ComboSpicCtrl(u32  PinLocation, bool Operation);

/** @defgroup PINMUX_Exported_Functions PINMUX Exported Functions
 * @{
 */
_LONG_CALL_ void Pinmux_Config(u8 PinName, u32 PinFunc);
_LONG_CALL_ u32 Pinmux_ConfigGet(u8 PinName);
_LONG_CALL_ void Pinmux_Swdoff(void);
/** @} */

/** @defgroup PINMUX_Register_Declaration PINMUX Register Declaration
 * @brief PINMUX register structure declaration
 * @{
 */
typedef struct {
	__IO uint32_t REG_GPIOx[PIN_TOTAL_NUM];			/*!< Pad control register */
} PINMUX_TypeDef;
/** @} */

/** @} */


/** @defgroup PAD PAD
 * @brief PAD driver modules
 * @{
 */

/** @defgroup PAD_Exported_Constants PAD Exported Constants
 * @{
 */

/** @defgroup PAD_Pull_Resistor_definitions PAD Pull Resistor Definitions
 * @{
 */
#define PAD_Resistor_LARGE		0x00 /*!< PAD large pull resistor. */
#define PAD_Resistor_SMALL		0x01 /*!< PAD small pull resistor. */
/** @} */


/** @defgroup PINMUX_PAD_DrvStrength_definitions PINMUX PAD Drive Strength Definitions
 * @{
 */
#define PAD_DRV_ABILITITY_LOW			(0)   /*!< PAD drive strength lowest level. */
#define PAD_DRV_ABILITITY_MID_LOW		(1)   /*!< PAD drive strength mid-low level. */
#define PAD_DRV_ABILITITY_MID_HIGH		(2)   /*!< PAD drive strength mid-high level. */
#define PAD_DRV_ABILITITY_HIGH			(3)   /*!< PAD drive strength highest level. */
/** @} */

/** @defgroup PINMUX_PAD_SlewRate_definitions PINMUX PAD Slew Rate Definitions
 * @{
 */
#define PAD_SlewRate_Fast				(0)   /*!< PAD output slew rate fast. */
#define PAD_SlewRate_Slow				(1)   /*!< PAD output slew rate slow. */
/** @} */

/** @} */

_LONG_CALL_ void PAD_SpicCtrl(u32  PinLocation);

/** @defgroup PAD_Exported_Functions PAD Exported Functions
 * @{
 */
_LONG_CALL_ void PAD_CMD(u8 PinName, u8 NewStatus);
_LONG_CALL_ void PAD_DrvStrength(u8 PinName, u32 DrvStrength);
_LONG_CALL_ void PAD_SchmitCtrl(u8 PinName, u32 NewState);
_LONG_CALL_ void PAD_SlewRateCtrl(u8 PinName, u32 NewState);
_LONG_CALL_ void PAD_PullCtrl(u8 PinName, u8 PullType);
_LONG_CALL_ void PAD_SleepPullCtrl(u8 PinName, u8 PullType);
_LONG_CALL_ void PAD_ResistorCtrl(u8 PinName, u8 RType);
_LONG_CALL_ void PAD_InputCtrl(u8 PinName, u32 NewState);
/** @} */

/** @} */

/** @} */

/// @cond
/** @} */
/// @endcond


/* PINMUX_TESTID_TRAP_ICFG_definitions */
#define TESTID_MCM_FLASH_PG1B_ICFG			3   /*!< Test ID for MCM flash PG1B ICFG trap. */
#define TESTID_FORCE_USE_EXTERNAL_PAD_ICFG		  	4   /*!< Test ID to force use of external pad ICFG trap. */
#define TESTID_FLASH_PROBE_EXTERNAL_PAD_ICFG			8   /*!< Test ID for flash probe on external pad ICFG trap. */
#define TESTID_FLASH_PROBE_INTERNAL_PAD_ICFG			9   /*!< Test ID for flash probe on internal pad ICFG trap. */

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

#endif   //_AMEBA_PINMUX_H_
