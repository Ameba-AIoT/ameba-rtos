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

/* Exported constants --------------------------------------------------------*/

/** @defgroup PINMUX_Exported_Constants PINMUX Exported Constants
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


#define _PNC           (0xFFFF)
/** @} */

/** @defgroup PINMUX_Function_definitions PINMUX Function Definitions
  * @{
  */
#define PINMUX_FUNCTION_GPIO			(0)
#define PINMUX_FUNCTION_UART			(1)
#define PINMUX_FUNCTION_UART_RTSCTS		(2)
#define PINMUX_FUNCTION_LOGUART			(2)
#define PINMUX_FUNCTION_SPI				(3)
#define PINMUX_FUNCTION_RTC				(4)
#define PINMUX_FUNCTION_IR				(5)
#define PINMUX_FUNCTION_SPIF			(6)
#define PINMUX_FUNCTION_I2C				(7)
#define PINMUX_FUNCTION_SDIOH			(8)
#define PINMUX_FUNCTION_LEDC			(9)
#define PINMUX_FUNCTION_PWM				(10)
#define PINMUX_FUNCTION_SWD				(11)
#define PINMUX_FUNCTION_AUDIO			(12)
#define PINMUX_FUNCTION_I2S0			(13)
#define PINMUX_FUNCTION_I2S1			(13)
#define PINMUX_FUNCTION_I2S2			(14)
#define PINMUX_FUNCTION_I2S3			(15)
#define PINMUX_FUNCTION_SPK				(16)
#define PINMUX_FUNCTION_AUXIN			(16)
#define PINMUX_FUNCTION_DMIC			(17)
#define PINMUX_FUNCTION_CAPTOUCH		(18)
#define PINMUX_FUNCTION_SIC				(19)
#define PINMUX_FUNCTION_MIPI			(20)
#define PINMUX_FUNCTION_USB				(21)
#define PINMUX_FUNCTION_FEM_C			(22)
#define PINMUX_FUNCTION_ANT_SEL			(22)
#define PINMUX_FUNCTION_EXT_ZIGBEE		(23)
#define PINMUX_FUNCTION_BT_UART			(24)
#define PINMUX_FUNCTION_BT_GPIO			(25)
#define PINMUX_FUNCTION_BT_RF			(26)
#define PINMUX_FUNCTION_DBG_BTCOEX_GNT	(27)
#define PINMUX_FUNCTION_TIMINPUT_HS		(28)
#define PINMUX_FUNCTION_DBGPORT			(29)
#define PINMUX_FUNCTION_WAKEUP			(30)

#define PINMUX_FUNCTION_SWD_EXTRA		(32)
#define PINMUX_FUNCTION_SDIO_EXTRA		(33)

#define PINMUX_FUNCTION_I2S2_EXTRA1		(34)
#define PINMUX_FUNCTION_I2S2_EXTRA2		(35)
#define PINMUX_FUNCTION_I2S2_DOUT_EXTRA	(36)

#define PINMUX_FUNCTION_I2S3_EXTRA		(37)
#define PINMUX_FUNCTION_I2S3_DOUT_EXTRA	(38)
#define PINMUX_FUNCTION_I2S3_DIN_EXTRA	(39)
/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */
/// @endcond

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your definition here
/* Exported Types --------------------------------------------------------*/

/** @defgroup PINMUX_Port_and_Pin_definitions PINMUX Port and Pin Definitions
 * @{
 */
#define PORT_NUM(pin)		((pin>>5) & 0x03)   /*!< Extract port number from pin name. */
#define PIN_NUM(pin)		(pin & 0x1f)   /*!< Extract pin number from pin name. */
/** @} */

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

/** @defgroup PINMUX_EXTEND_FUNCTION_definitions PINMUX Extend Function Definitions
 * @{
 */
typedef struct {
	u16 func_id;
	u16 bit_mask;
} PINMUX_FUNCEXT_T;

/** @} */

/** @} */

_LONG_CALL_ void Pinmux_UartLogCtrl(u32  PinLocation, bool   Operation);
_LONG_CALL_ void Pinmux_SpicCtrl(u32  PinLocation, bool Operation);
_LONG_CALL_ void Pinmux_ConfigEXT(u8 PinName, u32 PinFunc, u8 NewStatus);

/** @defgroup PINMUX_Exported_Functions PINMUX Exported Functions
  * @{
  */
_LONG_CALL_ void _Pinmux_Config(u8 PinName, u32 PinFunc);
_LONG_CALL_ u32 Pinmux_ConfigGet(u8 PinName);
_LONG_CALL_ void Pinmux_Swdoff(void);
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

/** @defgroup PINMUX_Special_Pad_definitions PINMUX Special Pad Definitions
 * @{
 */
#define APAD_NAME_START				_PA_18   /*!< First pin of audio shared PAD range. */

#define APAD_NAME_END				_PB_6   /*!< Last pin of audio shared PAD range. */

#define APAD_MIC_PAD_START			_PA_20   /*!< First pin of audio microphone input PAD range. */
#define APAD_MIC_PAD_END			_PA_29   /*!< Last pin of audio microphone input PAD range. */

#define APAD_MIC_BIAS_PAD_START		_PA_30   /*!< First pin of audio microphone bias PAD range. */
#define APAD_MIC_BIAS_PAD_END		_PB_2   /*!< Last pin of audio microphone bias PAD range. */

#define APAD_OUT_PAD_START			_PB_3   /*!< First pin of audio output PAD range. */
#define APAD_OUT_PAD_END			_PB_6   /*!< Last pin of audio output PAD range. */

#define UART_LOG_RXD      			_PB_23   /*!< Default pin for UART log RXD. */
#define UART_LOG_TXD     		    _PB_24   /*!< Default pin for UART log TXD. */

#define SWD_DATA					_PA_13   /*!< Default pin for SWD data signal. */
#define	SWD_CLK						_PA_14   /*!< Default pin for SWD clock signal. */
/** @} */

/** @} */

_LONG_CALL_ void PAD_SpicCtrl(u32  PinLocation);
_LONG_CALL_ u8 PAD_PullCtrlGet(u8 pin_name);

/** @defgroup PAD_Exported_Functions PAD Exported Functions
 * @{
 */
_LONG_CALL_ void PAD_CMD(u8 PinName, u8 NewStatus);
_LONG_CALL_ void PAD_DrvStrength(u8 PinName, u32 DrvStrength);


/**
  *  @brief Set the PAD slew rate control status.
  *  @param PinName: Specify the target pin. This parameter must be one of the values
  *                  defined in @ref PINMUX_Pin_Name_definitions.
  *  @param NewState: Specify the slew rate control status. Refer to the datasheet for detail informations.
  *                   This parameter can be one of the following values:
  *           @arg PAD_SlewRate_Slow
  *           @arg PAD_SlewRate_Fast
  */
_LONG_CALL_ void PAD_SlewRateCtrl(u8 PinName, u32 NewState);

/**
  *  @brief Set the PAD Schmitt trigger control status.
  *  @param PinName Specify the target pin. This parameter must be one of the values
  *                 defined in @ref PINMUX_Pin_Name_definitions.
  *  @param NewState Specify the control status. Refer to the datasheet for detail informations.
  *                  This parameter can be: ENABLE or DISABLE.
  */
_LONG_CALL_ void PAD_SchmitCtrl(u8 PinName, u32 NewState);

_LONG_CALL_ void PAD_PullCtrl(u8 PinName, u8 PullType);
_LONG_CALL_ void PAD_SleepPullCtrl(u8 PinName, u8 PullType);
_LONG_CALL_ void PAD_ResistorCtrl(u8 PinName, u8 RType);

/**
 *  @brief Control digital path input.
 *  @param PinName Value of @ref PINMUX_Pin_Name_definitions.
 *  @param NewState Digital path input status.
 *    @arg ENABLE: Enable digital path input.
 *    @arg DISABLE: Disable digital path input.
 *  @internal
 *  @note Only valid for ADC and CTC pads
 *  @endinternal
 */
__STATIC_INLINE
void PAD_InputCtrl(u8 PinName, u32 NewState)
{
	u32 RTemp, PadShift = 0;

	if (PinName > _PA_8) {
		return;
	}

	RTemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_DUMMY_094);

	PadShift = PinName - _PA_0;

	if (NewState != DISABLE) {
		RTemp |= BIT(PadShift);
	} else {
		RTemp &= (~ BIT(PadShift));
	}

	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_DUMMY_094, RTemp);
}

/**
 *  @brief Control digital path input for Audio share PAD.
 *  @param PinName Pin of Audio share PAD or 0xFF.
 *  @param NewState Digital path input status.
 *    @arg ENABLE: Enable digital path input.
 *    @arg DISABLE: Disable digital path input.
 *  @note
 *        - Only valid for Audio share PADs.
 *        - If PinName is 0xFF, all the pins of Audio share PAD will be controlled.
 */
__STATIC_INLINE
void APAD_InputCtrl(u8 PinName, u32 NewState)
{
	u32 RTemp, PadShift = 0;

	if (PinName == 0xFF) {
		if (NewState != DISABLE) {
			RTemp = 0x1FFFFF;
		} else {
			RTemp = 0x0;
		}
	} else {
		if (PinName < APAD_NAME_START || PinName > APAD_NAME_END) {
			return;
		}

		RTemp = HAL_READ32(PINMUX_REG_BASE, REG_PAD_AUD_PAD_CTRL);
		PadShift = PinName - APAD_NAME_START;
		if (NewState != DISABLE) {
			RTemp |= BIT(PadShift);
		} else {
			RTemp &= ~BIT(PadShift);
		}
	}

	HAL_WRITE32(PINMUX_REG_BASE, REG_PAD_AUD_PAD_CTRL, RTemp);
}
/** @} */

/** @} */

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

#endif   //_AMEBA_PINMUX_H_
