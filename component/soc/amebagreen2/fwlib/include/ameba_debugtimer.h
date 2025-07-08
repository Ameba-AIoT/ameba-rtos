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

/** @defgroup DEBUGTIMER
  * @brief DEBUGTIMER driver modules
  * @{
  */

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
#define DBGT_BIT_CRV_SET ((u32)0x00000001 << 1) /*!< WA0 0h  set dbgt_crv0/1 to  dbgtcnt*/
#define DBGT_BIT_ENABLE  ((u32)0x00000001 << 0) /*!< R/W 1h  Debug timer enable. 1: The counter is enabled. 0: The counter is disabled.*/
/** @} */

/** @defgroup DBGT_CRV0
  * @brief debugtimer count register
  * @{
  */
#define DBGT_MASK_CRV0   ((u32)0xFFFFFFFF << 0)           /*!< R/WOP 0h  dbgtcnt[31:0].Continuing to count from 0 once it's reached 32'Hffffffff*/
#define DBGT_CRV0(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define DBGT_GET_CRV0(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup DBGT_CRV1
  * @brief debugtimer count register
  * @{
  */
#define DBGT_MASK_CRV1   ((u32)0xFFFFFFFF << 0)           /*!< R/WOP 0h  dbgtcnt[63:32].Continuing to count from 0 once it's reached 32'hFFFFFFFF*/
#define DBGT_CRV1(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define DBGT_GET_CRV1(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
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
#define DBGT_MASK_ATOM   ((u32)0xFFFFFFFF << 0)           /*!< R/WOP 0h  Return the last increased value of atom counter*/
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
	__IO uint32_t DBGT_CTRL        ;  /*!< DEBUGTIMER CONTROL REGISTER,  Address offset:0x000 */
	__IO uint32_t DBGT_CRV0        ;  /*!< DEBUGTIMER COUNT REGISTER,  Address offset:0x004 */
	__IO uint32_t DBGT_CRV1        ;  /*!< DEBUGTIMER COUNT REGISTER,  Address offset:0x008 */
	__I  uint32_t DBGT_ATOM_INC    ;  /*!< DEBUGTIMER ATOM READ INCREASE Register,  Address offset:0x00C */
	__IO uint32_t DBGT_ATOM        ;  /*!< DEBUGTIMER ATOM Register,  Address offset:0x010 */
	__IO uint32_t DBGT_SCRATCH     ;  /*!< DEBUGTIMER SCRATCH Register,  Address offset:0x014 */
	__IO uint32_t DBGT_DBGTIM_DUMMY;  /*!< DEBUG TIMERDUMMY REGISTER,  Address offset:0x018 */
} DEBUGTIMER_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
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

/** @} */

/** @} */


/* group: DEBUGTIMER_Exported_Functions */
/* Since all the members in the group are __STATIC_INLINE which will not be extracted to the doxygen doc,
  original comment has been deleted to avoid there is nothing displayed under the group. */

/**
  * @brief  Enables or disables the DebugTimer peripheral
  *   The debug timer is a free run timer works at 1MHZ.
  * @param  NewState: new state of the Debug timer peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
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
  * @brief  Get the u32 passed time.
  * @param  None
  * @retval current counter
  */
__STATIC_INLINE u32 DTimesPassedtime32_Get(u32 OldTime)
{
	DEBUGTIMER_TypeDef *DTimer = DTIMER_DEV;

	if (DTimer->DBGT_CRV0 < OldTime) {
		return (DTimer->DBGT_CRV0 - OldTime + 0xFFFFFFFF + 1);
	} else {
		return (DTimer->DBGT_CRV0 - OldTime);
	}

}

/**
  * @brief  Get the u64 passed time.
  * @param  None
  * @retval current counter
  */
__STATIC_INLINE u64 DTimesPassedtime64_Get(u64 OldTime)
{
	DEBUGTIMER_TypeDef *DTimer = DTIMER_DEV;

	return ((((u64)DTimer->DBGT_CRV1 << 32) | DTimer->DBGT_CRV0) - OldTime);
}

/**
  * @brief  Get the Debug timer u32 current Counter.
  * @param  None
  * @retval current counter
  */
__STATIC_INLINE u32 DTimestamp_Get(void)
{
	DEBUGTIMER_TypeDef *DTimer = DTIMER_DEV;

	return DTimer->DBGT_CRV0;
}

/**
  * @brief  Get the Debug timer u64 current Counter.
  * @param  None
  * @retval current counter
  */
__STATIC_INLINE u64 DTimestamp64_Get(void)
{
	DEBUGTIMER_TypeDef *DTimer = DTIMER_DEV;

	return (((u64)DTimer->DBGT_CRV1 << 32) | DTimer->DBGT_CRV0);
}

/**
  * @brief  Set the Debug timer current Counter.
  * @param  NewVaule: new debug counter.
  * @retval current counter
  */
__STATIC_INLINE void DTimestamp_Set(u64 NewVaule)
{
	DEBUGTIMER_TypeDef *DTimer = DTIMER_DEV;

	DTimer->DBGT_CRV0 = (u32)(NewVaule & (u64)0xFFFFFFFF);
	DTimer->DBGT_CRV1 = (u32)((NewVaule & ((u64)0xFFFFFFFF << 32)) >> 32);
	/*set DBGT_CRV0 and DBGT_CRV1 to dbgtcnt.*/
	DTimer->DBGT_CTRL |= DBGT_BIT_CRV_SET;
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
/* end of the function group */

/* MANUAL_GEN_END */

#endif