/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_DEBUGTIMER_H_
#define _AMEBA_DEBUGTIMER_H_

/* AUTO_GEN_START */

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup DEBUGTIMER
* @brief DEBUGTIMER driver modules
* @{
*/

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup DEBUGTIMER_Register_Definitions DEBUGTIMER Register Definitions
 * @{
 *****************************************************************************/

/** @defgroup DBGT_CTRL
 * @brief Debug Timer control register
 * @{
 **/
#define DBGT_BIT_ENABLE          ((u32)0x00000001 << 0)          /*!<R/W 1h  Debug timer enable. 1: The counter is enabled. 0: The counter is disabled. */
/** @} */

/** @defgroup DBGT_CRV
 * @brief Debug Timer count register
 * @{
 **/
#define DBGT_MASK_CRV            ((u32)0xFFFFFFFF << 0)          /*!<R/WOP 0h  Continue to count from 0 once it's reached 32'hFFFFFFFF. */
#define DBGT_CRV(x)              (((u32)((x) & 0xFFFFFFFF) << 0))
#define DBGT_GET_CRV(x)          ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup DBGT_ATOM_INC
 * @brief Debug Timer atom read increase register
 * @{
 **/
#define DBGT_MASK_ATOM_INC       ((u32)0xFFFFFFFF << 0)          /*!<RP 0h  Each read will increase 1 and return to the increased value to bus. */
#define DBGT_ATOM_INC(x)         (((u32)((x) & 0xFFFFFFFF) << 0))
#define DBGT_GET_ATOM_INC(x)     ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup DBGT_ATOM
 * @brief Debug Timer atom register
 * @{
 **/
#define DBGT_MASK_ATOM           ((u32)0xFFFFFFFF << 0)          /*!<R/WOP 0h  Return the last increased value of atom counter. */
#define DBGT_ATOM(x)             (((u32)((x) & 0xFFFFFFFF) << 0))
#define DBGT_GET_ATOM(x)         ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup DBGT_SCRATCH
 * @brief Debug Timer scratch register
 * @{
 **/
#define DBGT_MASK_SCRATCH        ((u32)0xFFFFFFFF << 0)          /*!<R/W 0h  Reserved */
#define DBGT_SCRATCH(x)          (((u32)((x) & 0xFFFFFFFF) << 0))
#define DBGT_GET_SCRATCH(x)      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */
/** @} */



/* Exported types --------------------------------------------------------*/

/** @defgroup DEBUGTIMER_Exported_Types DEBUGTIMER Exported Types
  * @{
*****************************************************************************/


/**
 * @brief DEBUGTIMER Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t DBGT_CTRL;                              /*!< DEBUG TIMER CONTROL REGISTER,  Address offset: 0x000 */
	__IO uint32_t DBGT_CRV;                               /*!< DEBUG TIMER COUNT REGISTER,  Address offset: 0x004 */
	__IO uint32_t DBGT_ATOM_INC;                          /*!< DEBUG TIMER ATOM READ INCREASE REGISTER,  Address offset: 0x008 */
	__IO uint32_t DBGT_ATOM;                              /*!< DEBUG TIMER ATOM REGISTER,  Address offset: 0x00C */
	__IO uint32_t DBGT_SCRATCH;                           /*!< DEBUG TIMER SCRATCH REGISTER,  Address offset: 0x010 */
	__IO uint32_t DBGT_DBGTIM_DUMMY;                      /*!< DEBUG TIMER DUMMY REGISTER,  Address offset: 0x014 */
} DEBUGTIMER_TypeDef;
/** @} */
/* AUTO_GEN_END */

/* MANUAL_GEN_START */

//Please add your defination here

/* Exported constants --------------------------------------------------------*/
/** @defgroup DEBUGTIMER_Exported_Constants DEBUGTIMER Exported Constants
  * @{
  */

/** @defgroup DEBUGTIMER_Clock_Source
  * @{
  */
#define  DTIM_CLK_XTAL  0
#define  DTIM_CLK_32K    1
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
  * @brief  Enables or disables the DebugTimer peripheral
  *   The debug timer is a free run timer works at 1MHZ.
  * @param  NewState: new state of the Debug timer peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
__STATIC_INLINE void DTimer_Cmd(BOOL Newstatus)
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
  * @param  DTim_speed: Speed of debugtimer counter.
  *   @arg 0: OSC32KHz.
  *   @arg 1: 1MHz, which is divided form XTAL40MHz.
  * @retval None
  */
__STATIC_INLINE void DTimer_CLK(u8 DTim_speed)
{
	u32 Temp;

	Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_GRP0);

	if (DTim_speed == 1) {
		/*1M clock,the counter increase 1every 1us*/
		Temp &= ~LSYS_BIT_CKSL_DTIM;
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_GRP0, Temp);

		Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_GRP1);
		Temp &= ~LSYS_MASK_CKD_XTAL_DTIM;
		Temp |= LSYS_CKD_XTAL_DTIM(0x27);
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_GRP1, Temp);
	} else if (DTim_speed == 0) {
		/*32K clock,the counter increase 1every 32us*/
		Temp |= LSYS_BIT_CKSL_DTIM;
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_GRP0, Temp);
	}
}


/**
  * @brief  Get the Debug timer current Counter.
  * @param  None
  * @retval current counter
  */
__STATIC_INLINE u32 DTimestamp_Get(void)
{
	DEBUGTIMER_TypeDef *DTimer = DTIMER_DEV;

	return DTimer->DBGT_CRV;
}

/**
  * @brief  Set the Debug timer current Counter.
  * @param  NewVaule: new debug counter.
  * @retval current counter
  */
__STATIC_INLINE void DTimestamp_Set(u32 NewVaule)
{
	DEBUGTIMER_TypeDef *DTimer = DTIMER_DEV;

	DTimer->DBGT_CRV = NewVaule;
}

/**
  * @brief  Get the Debug timer atom value.
  *   The read action won't trigger Atom increase
  * @param  None
  * @retval atom counter
  */
__STATIC_INLINE u32 DTimer_AtomGet(void)
{
	DEBUGTIMER_TypeDef *DTimer = DTIMER_DEV;

	return DTimer->DBGT_ATOM;
}

/**
  * @brief  Get the Debug timer atom value + 1.
  *   The read action will trigger Atom increase
  * @param  None
  * @retval atom counter
  */
__STATIC_INLINE u32 DTimer_AtomIncGet(void)
{
	DEBUGTIMER_TypeDef *DTimer = DTIMER_DEV;

	return DTimer->DBGT_ATOM_INC;
}

/**
  * @brief  Set the Debug timer atom value .
  * @param  New value
  * @retval None
  */
__STATIC_INLINE void DTimer_AtomSet(u32 NewVaule)
{
	DEBUGTIMER_TypeDef *DTimer = DTIMER_DEV;

	DTimer->DBGT_ATOM = NewVaule;
}

/**
  * @brief  Get the Debug timer Scratch value.
  * @param  None
  * @retval atom scratch
  */
__STATIC_INLINE u32 DTimer_ScratchGet(void)
{
	DEBUGTIMER_TypeDef *DTimer = DTIMER_DEV;

	return DTimer->DBGT_SCRATCH;
}

/**
  * @brief  Set the Debug timer Scratch value .
  * @param  New value
  * @retval None
  */
__STATIC_INLINE void DTimer_ScratchSet(u32 NewVaule)
{

	DEBUGTIMER_TypeDef *DTimer = DTIMER_DEV;

	DTimer->DBGT_SCRATCH = NewVaule;
}
/** @} */

/* end of the function group */

/* MANUAL_GEN_END */

/** @} */

/** @} */

#endif
