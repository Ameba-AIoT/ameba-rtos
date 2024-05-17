/*
 * Copyright (c) 2018-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef __TFM_ARCH_V8M_H__
#define __TFM_ARCH_V8M_H__

#include <stdint.h>
#include <stdbool.h>

#include "cmsis_compiler.h"
#include "tfm_core_trustzone.h"
#include "utilities.h"

#define EXC_RETURN_RES1                         (0x1FFFFUL << 7)

/* Initial EXC_RETURN value in LR when a thread is loaded at the first time */
#define EXC_RETURN_THREAD_PSP                                   \
        EXC_RETURN_PREFIX | EXC_RETURN_RES1 |                   \
        EXC_RETURN_S | EXC_RETURN_DCRS |                        \
        EXC_RETURN_FTYPE | EXC_RETURN_MODE |                    \
        EXC_RETURN_SPSEL | EXC_RETURN_ES

#define EXC_RETURN_THREAD_MSP                                   \
        EXC_RETURN_PREFIX | EXC_RETURN_RES1 |                   \
        EXC_RETURN_S | EXC_RETURN_DCRS |                        \
        EXC_RETURN_FTYPE | EXC_RETURN_MODE |                    \
        EXC_RETURN_ES

#define EXC_RETURN_HANDLER                                      \
        EXC_RETURN_PREFIX | EXC_RETURN_RES1 |                   \
        EXC_RETURN_S | EXC_RETURN_DCRS |                        \
        EXC_RETURN_FTYPE | EXC_RETURN_ES

/* Exception numbers */
#define EXC_NUM_THREAD_MODE                     (0)
#define EXC_NUM_SVCALL                          (11)
#define EXC_NUM_PENDSV                          (14)

#define SCB_ICSR_ADDR                    (0xE000ED04)
#define SCB_ICSR_PENDSVSET_BIT           (0x10000000)

/* Disable NS exceptions by setting NS PRIMASK to 1 */
#define TFM_NS_EXC_DISABLE()    __TZ_set_PRIMASK_NS(1)
/* Enable NS exceptions by setting NS PRIMASK to 0 */
#define TFM_NS_EXC_ENABLE()     __TZ_set_PRIMASK_NS(0)

extern uint64_t __STACK_SEAL;

/**
 * \brief Check whether Secure or Non-secure stack is used to restore stack
 *        frame on exception return.
 *
 * \param[in] lr            LR register containing the EXC_RETURN value.
 *
 * \retval true             Secure stack is used to restore stack frame on
 *                          exception return.
 * \retval false            Non-secure stack is used to restore stack frame on
 *                          exception return.
 */
__STATIC_INLINE bool is_return_secure_stack(uint32_t lr)
{
    return (lr & EXC_RETURN_S) ? true : false;
}

/**
 * \brief Check whether the default stacking rules apply, or whether the
 *        Additional state context, also known as callee registers,
 *        are already on the stack.
 *        DCRS bit is only present from V8M and above.
 *        If DCRS is 1 then Stack contains:
 *        r0, r1, r2, r3, r12, r14 (lr), the return address and xPSR
 *
 *        If DCRS is 0 then the stack contains the following too before
 *        the caller-saved registers:
 *        Integrity signature, res, r4, r5, r6, r7, r8, r9, r10, r11
 *
 * \param[in] lr            LR register containing the EXC_RETURN value.
 *
 * \retval true             Default rules for stacking the Additional state
 *                          context registers followed.
 * \retval false            Stacking of the Additional state context
 *                          registers skipped.
 */
__STATIC_INLINE bool is_default_stacking_rules_apply(uint32_t lr)
{
    return (lr & EXC_RETURN_DCRS) ? true : false;
}

/**
 * \brief Check whether the stack frame for this exception has space allocated
 *        for Floating Point(FP) state information.
 *
 * \param[in] lr            LR register containing the EXC_RETURN value.
 *
 * \retval true             The stack allocates space for FP information
 * \retval false            The stack doesn't allocate space for FP information
 */
__STATIC_INLINE bool is_stack_alloc_fp_space(uint32_t lr)
{
    return (lr & EXC_RETURN_FTYPE) ? false : true;
}

/**
 * \brief Set PSPLIM register.
 *
 * \param[in] psplim        Register value to be written into PSPLIM.
 */
__STATIC_INLINE void tfm_arch_set_psplim(uint32_t psplim)
{
    __set_PSPLIM(psplim);
}

/**
 * \brief Set MSP limit value.
 *
 * \param[in] msplim        MSP limit value to be written.
 */
__STATIC_INLINE void tfm_arch_set_msplim(uint32_t msplim)
{
    __set_MSPLIM(msplim);
}

/**
 * \brief Seal the thread stack.
 *
 * This function must be called only when the caller is using MSP.
 *
 * \param[in] stk        Thread stack address.
 *
 * \retval stack         Updated thread stack address.
 */
__STATIC_INLINE uintptr_t arch_seal_thread_stack(uintptr_t stk)
{
    SPM_ASSERT((stk & 0x7) == 0);
    stk -= TFM_STACK_SEALED_SIZE;

    *((uint32_t *)stk)       = TFM_STACK_SEAL_VALUE;
    *((uint32_t *)(stk + 4)) = TFM_STACK_SEAL_VALUE;

    return stk;
}

/**
 * \brief Check MSP sealing.
 *
 * Sealing must be done in the Reset_Handler() on a 8 byte region
 * (__STACK_SEAL) defined in the linker scripts.
 * (It is a CMSIS recommendation)
 *
 */
__STATIC_INLINE void tfm_arch_check_msp_sealing(void)
{
    SPM_ASSERT(*(uint64_t *)(&__STACK_SEAL) == __TZ_STACK_SEAL_VALUE);
}

#endif
