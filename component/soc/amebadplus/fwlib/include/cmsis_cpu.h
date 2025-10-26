/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __CMSIS_CPU_H__
#define __CMSIS_CPU_H__

#include "ameba_vector_table.h"

#if defined (CONFIG_ARM_CORE_CM4)

/* ========  Configuration of Core Peripherals  ================================== */
#define __CM55_REV                0x0001U   /* Core revision r0p1 */
// #define __ARMv81MML_REV           0x0001U   /* Core revision r0p1 */
#define __SAUREGION_PRESENT       1U        /* SAU regions present */
#define __MPU_PRESENT             1U        /* MPU present */
#define __VTOR_PRESENT            1U        /* VTOR present */
#define __NVIC_PRIO_BITS          3U        /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0U        /* Set to 1 if different SysTick Config is used */
#define __FPU_PRESENT             1U        /* FPU present */
#define __FPU_DP                  0U        /* double precision FPU */
#define __DSP_PRESENT             1U        /* DSP extension present */
#define __PMU_PRESENT             1U        /* PMU present */
#define __PMU_NUM_EVENTCNT        4U        /* Number of PMU event counters */
#define __ICACHE_PRESENT          1U        /* Instruction Cache present */
#define __DCACHE_PRESENT          1U        /* Data Cache present */

/* CM33 does not support Cache and PMU */
#include "core_cm55.h"                 /* Processor and core peripherals */
// #include "core_armv81mml.h"
#include "cmsis_ridr.h"

#if defined (__ARM_FEATURE_CMSE) &&  (__ARM_FEATURE_CMSE == 3U)
#include <arm_cmse.h>
#endif /* __ARM_FEATURE_CMSE */

#elif defined (CONFIG_ARM_CORE_CM0)

/* --------  Configuration of the Cortex-M23 Processor and Core Peripherals  ------ */
#define __CM23_REV                0x0000U   /* Core revision r0p1 */
// #define __ARMv8MBL_REV            0x0000U   /* Core revision r0p0 */
#define __MPU_PRESENT             1U        /* MPU present */
#define __SAUREGION_PRESENT       0U        /* SAU regions present */
#define __VTOR_PRESENT            1U        /* VTOR present */
#define __NVIC_PRIO_BITS          2U        /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0U        /* Set to 1 if different SysTick Config is used */
#define __FPU_PRESENT             0U        /* no FPU present */
#define __DSP_PRESENT             0U        /* no DSP extension present */

#include "core_cm23_km0.h"
// #include "core_armv8mbl.h"

/* CM23 Does not support Cache, But Ameba Has it */
#define __ICACHE_PRESENT          1U        /* Instruction Cache present */
#define __DCACHE_PRESENT          1U        /* Data Cache present */
/* ##########################  Cache functions  #################################### */
#if ((defined (__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1U)) || \
     (defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)))
#include "m-profile/armv7m_cachel1.h"
#endif

#endif

#endif /* __CMSIS_CPU_H__ */
