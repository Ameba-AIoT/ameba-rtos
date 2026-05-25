/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_RESET_H_
#define _AMEBA_RESET_H_

/* AUTO_GEN_START */


/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your defination here

#define KM4NS_CPU_ID 1
#define KM4TZ_CPU_ID 0

#define SYS_RESET_KEY 0x9696
#define SYS_RESET_TRIG 0x6969

extern u32 BOOT_Reason(void);
extern void System_Reset(void);
extern void Platform1_Reset(void);


/**
  * @brief  Get Current CPU dynamiclly
  * @param  None
  * @retval CPU ID.
  *		 1: KM4NS_CPU_ID
  *		 0: KM4TZ_CPU_ID
  */
__STATIC_INLINE u32 SYS_CPUID(void)
{
	u32 temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_SYSTEM_CFG0);

	return LSYS_GET_HW_CPU_ID(temp);
}

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

#endif
