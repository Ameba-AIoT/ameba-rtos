/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_DEBUGTIMER_H_
#define _AMEBA_DEBUGTIMER_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup DEBUGTIMER Debug Timer
  * @brief DEBUGTIMER driver modules
  * @{
  */

/// @cond
/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup DEBUGTIMER_Register_Definitions DEBUGTIMER Register Definitions
  * @{
  */

/** @defgroup DBGT_CTRL
  * @brief debugtimer control register
  * @{
  */
#define DBGT_MASK_DIV      ((u32)0x00000FFF << 4)           /*!< R/W 28h  The default value can used to divide 40M Xtal to 1M*/
#define DBGT_DIV(x)        (((u32)((x) & 0x00000FFF) << 4))
#define DBGT_GET_DIV(x)    ((u32)(((x >> 4) & 0x00000FFF)))
#define DBGT_MASK_CLKSEL   ((u32)0x00000007 << 1)           /*!< R/W 0h  Clock source select 1: Select 32K as clock source. 0: Select Xtal as clock source. 2~7: Reserved*/
#define DBGT_CLKSEL(x)     (((u32)((x) & 0x00000007) << 1))
#define DBGT_GET_CLKSEL(x) ((u32)(((x >> 1) & 0x00000007)))
#define DBGT_BIT_ENABLE    ((u32)0x00000001 << 0)           /*!< R/W 1h  Debug timer enable. 1: The counter is enabled. 0: The counter is disabled.*/
/** @} */

/** @defgroup DBGT_CRV
  * @brief debugtimer count register
  * @{
  */
#define DBGT_MASK_CRV   ((u32)0xFFFFFFFF << 0)           /*!< WOP 0h  Continuing to count from 0 once it's reached 32'hFFFFFFFF*/
#define DBGT_CRV(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define DBGT_GET_CRV(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup DBGT_ATOM_INC
  * @brief debugtimer atom read increase
  * @{
  */
#define DBGT_MASK_ATOM_INC   ((u32)0xFFFFFFFF << 0)           /*!< RP 0h  Each read will increase 1 and return to the increased value to bus.*/
#define DBGT_ATOM_INC(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define DBGT_GET_ATOM_INC(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup DBGT_ATOM
  * @brief debugtimer atom
  * @{
  */
#define DBGT_MASK_ATOM   ((u32)0xFFFFFFFF << 0)           /*!< WOP 0h  Return the last increased value of atom counter*/
#define DBGT_ATOM(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define DBGT_GET_ATOM(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup DBGT_SCRATCH
  * @brief debugtimer scratch
  * @{
  */
#define DBGT_MASK_SCRATCH   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0h  Reserved for user use*/
#define DBGT_SCRATCH(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define DBGT_GET_SCRATCH(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup DEBUGTIMER_Exported_Types DEBUGTIMER Exported Types
  * @{
  */

/** @brief DEBUGTIMER Register Declaration
  */

typedef struct {
	__IO uint32_t DBGT_CTRL    ;  /*!< DEBUGTIMER CONTROL REGISTER,  Address offset:0x000 */
	__O  uint32_t DBGT_CRV     ;  /*!< DEBUGTIMER COUNT REGISTER,  Address offset:0x004 */
	__I  uint32_t DBGT_ATOM_INC;  /*!< DEBUGTIMER ATOM READ INCREASE Register,  Address offset:0x008 */
	__O  uint32_t DBGT_ATOM    ;  /*!< DEBUGTIMER ATOM Register,  Address offset:0x00C */
	__IO uint32_t DBGT_SCRATCH ;  /*!< DEBUGTIMER SCRATCH Register,  Address offset:0x010 */
} DEBUGTIMER_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */
/// @endcond

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your defination here

/* Exported constants --------------------------------------------------------*/
/** @defgroup DEBUGTIMER_Exported_Constants DEBUGTIMER Exported Constants
  * @{
  */

/** @defgroup DEBUGTIMER_Clock_Source Debug Timer Clock Source
  * @{
  */
#define  DTIM_CLK_XTAL  0   /*!< Select XTAL as clock source */
#define  DTIM_CLK_32K   1   /*!< Select 32K as clock source */
/**
  * @}
  */

/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/
/** @defgroup DEBUGTIMER_Exported_Functions DEBUGTIMER Exported Functions
  * @{
  */

/**
  * @brief  Enable or disable the Debug Timer peripheral
  *   The debug timer is a free-running timer that works at 1 MHz.
  * @param  Newstatus New state of the Debug Timer peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  */
__STATIC_INLINE void DTimer_Cmd(bool Newstatus)
{
	DEBUGTIMER_TypeDef *DTimer = DTIMER_DEV;

	if (Newstatus == ENABLE) {
		DTimer->DBGT_CTRL |= DBGT_BIT_ENABLE;
	} else {
		DTimer->DBGT_CTRL &= ~DBGT_BIT_ENABLE;
	}

}

/**
  * @brief  Select clock for timestamp, which works at 1MHz by default.
  * @param  DTim_speed Speed of debugtimer counter.
  *   @arg 0: OSC32KHz.
  *   @arg 1: 1MHz, which is divided from XTAL40MHz.
  */
__STATIC_INLINE void DTimer_CLK(u8 DTim_speed)
{
	DEBUGTIMER_TypeDef *DTimer = DTIMER_DEV;
	u8 xtal = 40;
	u32 Temp;

	//assert_param(DTim_speed <= 40);

	if (DTim_speed == 0) {
		/*32K clock,the counter increase 1every 32us*/
		Temp = DTimer->DBGT_CTRL;
		Temp &= ~DBGT_MASK_CLKSEL;
		Temp |= DBGT_CLKSEL(DTIM_CLK_32K);

		DTimer->DBGT_CTRL = Temp;
	} else if (DTim_speed == 1)  {

		//if ((xtal % DTim_speed) != 0) {
		//	DBG_8195A("inaccurate speed selected\n");
		//}

		Temp = DTimer->DBGT_CTRL;
		Temp &= ~DBGT_MASK_CLKSEL;
		Temp |= DBGT_CLKSEL(DTIM_CLK_XTAL);
		Temp |= DBGT_DIV(xtal / DTim_speed);

		DTimer->DBGT_CTRL = Temp;
	}

}


/**
  * @brief  Get the Debug Timer u32 current counter.
  * @return Current u32 counter value.
  */
__STATIC_INLINE u32 DTimestamp_Get(void)
{
	DEBUGTIMER_TypeDef *DTimer = DTIMER_DEV;

	return DTimer->DBGT_CRV;
}

/**
  * @brief  Set the Debug Timer current counter.
  * @param  NewValue New counter value.
  */
__STATIC_INLINE void DTimestamp_Set(u32 NewValue)
{
	DEBUGTIMER_TypeDef *DTimer = DTIMER_DEV;

	DTimer->DBGT_CRV = NewValue;
}

/**
  * @brief  Get the Debug Timer atom counter value.
  *   Reading this register does not increment the atom counter.
  * @return Current atom counter value.
  */
__STATIC_INLINE u32 DTimer_AtomGet(void)
{
	DEBUGTIMER_TypeDef *DTimer = DTIMER_DEV;

	return DTimer->DBGT_ATOM;
}

/**
  * @brief  Get the Debug Timer atom counter value and trigger auto-increment.
  *   Reading this register auto-increments the atom counter by 1.
  * @return Atom counter value after auto-increment.
  */
__STATIC_INLINE u32 DTimer_AtomIncGet(void)
{
	DEBUGTIMER_TypeDef *DTimer = DTIMER_DEV;

	return DTimer->DBGT_ATOM_INC;
}

/**
  * @brief  Set the Debug Timer atom counter value.
  * @param  NewValue New atom counter value.
  */
__STATIC_INLINE void DTimer_AtomSet(u32 NewValue)
{
	DEBUGTIMER_TypeDef *DTimer = DTIMER_DEV;

	DTimer->DBGT_ATOM = NewValue;
}

/**
  * @brief  Get the Debug Timer scratch register value.
  * @return Current scratch register value.
  */
__STATIC_INLINE u32 DTimer_ScratchGet(void)
{
	DEBUGTIMER_TypeDef *DTimer = DTIMER_DEV;

	return DTimer->DBGT_SCRATCH;
}

/**
  * @brief  Set the Debug Timer scratch register value.
  * @param  NewValue New scratch register value.
  */
__STATIC_INLINE void DTimer_ScratchSet(u32 NewValue)
{

	DEBUGTIMER_TypeDef *DTimer = DTIMER_DEV;

	DTimer->DBGT_SCRATCH = NewValue;
}
/** @} */

/* end of the function group */

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

/** @} */

/** @} */


#endif
