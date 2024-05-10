/*
 * Copyright (c) 2018-2023, Arm Limited. All rights reserved.
 * Copyright (c) 2022-2023 Cypress Semiconductor Corporation (an Infineon
 * company) or an affiliate of Cypress Semiconductor Corporation. All rights
 * reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <inttypes.h>
#include "compiler_ext_defs.h"
#include "config_spm.h"
#include "security_defs.h"
#include "spm.h"
#include "svc_num.h"
#include "tfm_hal_device_header.h"
#include "tfm_arch.h"
#include "tfm_svcalls.h"
#include "utilities.h"

#if !defined(__ARM_ARCH_8M_BASE__)
#error "Unsupported ARM Architecture."
#endif

/* Delcaraction flag to control the scheduling logic in PendSV. */
uint32_t scheduler_lock = SCHEDULER_UNLOCKED;

/* IAR Specific */
#if defined(__ICCARM__)

#if CONFIG_TFM_SPM_BACKEND_IPC == 1
#pragma required = ipc_schedule
#endif

#pragma required = scheduler_lock
#pragma required = tfm_core_svc_handler

#if CONFIG_TFM_PSA_API_CROSS_CALL == 1

#pragma required = cross_call_entering_c
#pragma required = cross_call_exiting_c

#endif /* CONFIG_TFM_PSA_API_CROSS_CALL == 1 */

#endif

#if CONFIG_TFM_PSA_API_CROSS_CALL == 1

__naked void arch_non_preempt_call(uintptr_t fn_addr, uintptr_t frame_addr,
                                   uint32_t stk_base, uint32_t stk_limit)
{
    __asm volatile(
#if !defined(__ICCARM__)
        ".syntax unified                                \n"
#endif
        "   push   {r4-r6, lr}                          \n"
        "   cpsid  i                                    \n"
        "   isb                                         \n"
        "   mov    r4, r2                               \n"
        "   cmp    r2, #0                               \n"
        "   beq    v8b_lock_sched                       \n"
        "   mrs    r5, psplim                           \n"/* To caller stack */
        "   movs   r4, #0                               \n"
        "   msr    psplim, r4                           \n"
        "   mov    r4, sp                               \n"
        "   mov    sp, r2                               \n"
        "   msr    psplim, r3                           \n"
        "v8b_lock_sched:                                \n"/* To lock sched */
        "   ldr    r2, =scheduler_lock                  \n"
        "   movs   r3, #"M2S(SCHEDULER_LOCKED)"         \n"
        "   str    r3, [r2, #0]                         \n"
        "   cpsie  i                                    \n"
        "   isb                                         \n"
        "   mov    r6, r1                               \n"
        "   bl     cross_call_entering_c                \n"
        "   cpsid  i                                    \n"
        "   isb                                         \n"
        "   mov    r1, r6                               \n"
        "   bl     cross_call_exiting_c                 \n"
        "   cmp    r4, #0                               \n"
        "   beq    v8b_release_sched                    \n"
        "   movs   r3, #0                               \n"/* To callee stack */
        "   msr    psplim, r3                           \n"
        "   mov    sp, r4                               \n"
        "   msr    psplim, r5                           \n"
        "v8b_release_sched:                             \n"
        "   ldr    r2, =scheduler_lock                  \n"/* To unlock sched */
        "   movs   r3, #"M2S(SCHEDULER_UNLOCKED)"       \n"
        "   str    r3, [r2, #0]                         \n"
        "   cpsie  i                                    \n"
        "   isb                                         \n"
        "   pop    {r4-r6, pc}                          \n"
    );
}

#endif /* CONFIG_TFM_PSA_API_CROSS_CALL == 1 */

#if CONFIG_TFM_SPM_BACKEND_IPC == 1
__attribute__((naked)) void PendSV_Handler(void)
{
    __ASM volatile(
#if !defined(__ICCARM__)
        ".syntax unified                                \n"
#endif
        "   movs    r0, #"M2S(EXC_RETURN_S)"            \n"
        "   mov     r1, lr                              \n"
#if CONFIG_TFM_SCHEDULE_WHEN_NS_INTERRUPTED == 0
        "   tst     r0, r1                              \n" /* NS interrupted */
        "   beq     v8b_pendsv_exit                     \n" /* No schedule */
#endif
        "   push    {r0, lr}                            \n" /* Save R0, LR */
        "   bl      ipc_schedule                        \n"
        "   pop     {r2, r3}                            \n"
        "   mov     lr, r3                              \n"
        "   cmp     r0, r1                              \n" /* curr, next ctx */
        "   beq     v8b_pendsv_exit                     \n" /* No schedule */
        "   cpsid   i                                   \n"
        "   mrs     r2, psp                             \n"
        "   mov     r3, lr                              \n"
        "   push    {r0, r1}                            \n"
        "   movs    r1, #"M2S(EXC_RETURN_DCRS)"         \n" /* Check DCRS */
        "   ands    r1, r3                              \n"
        "   beq     v8b_pendsv_callee_saved             \n" /* Skip saving callee */
        "   subs    r2, #32                             \n" /* For r4-r7 */
        "   stm     r2!, {r4-r7}                        \n" /* Save r4-r7 */
        "   mov     r4, r8                              \n"
        "   mov     r5, r9                              \n"
        "   mov     r6, r10                             \n"
        "   mov     r7, r11                             \n"
        "   stm     r2!, {r4-r7}                        \n" /* Save r8-r11 */
        "   subs    r2, #40                             \n" /* Set SP to top
                                                             * With two more dummy data for
                                                             * reserved additional state context,
                                                             * integrity signature offset
                                                             */
        "v8b_pendsv_callee_saved:                       \n"
        "   pop     {r0, r1}                            \n"
        "   stm     r0!, {r2, r3}                       \n" /* Save curr ctx */
        "   ldm     r1!, {r2, r3}                       \n" /* Load next ctx */
        "   mov     lr, r3                              \n"
        "   push    {r0, r1}                            \n"
        "   movs    r1, #"M2S(EXC_RETURN_DCRS)"         \n" /* Check DCRS */
        "   ands    r1, r3                              \n"
        "   beq     v8b_pendsv_callee_loaded            \n" /* Skip loading callee */
        "   adds    r2, #24                             \n" /* Pop dummy data for
                                                             * reserved additional state context,
                                                             * integrity signature offset,
                                                             * r8-r11
                                                             */
        "   ldm     r2!, {r4-r7}                        \n"
        "   mov     r8, r4                              \n"
        "   mov     r9, r5                              \n"
        "   mov     r10, r6                             \n"
        "   mov     r11, r7                             \n"
        "   subs    r2, #32                             \n" /* Pop r4-r7 */
        "   ldm     r2!, {r4-r7}                        \n"
        "   adds    r2, #16                             \n" /* Pop r4-r11 end */
        "v8b_pendsv_callee_loaded:                      \n"
        "   pop     {r0, r1}                            \n"
        "   ldr     r3, [r1]                            \n"
        "   msr     psp, r2                             \n"
        "   msr     psplim, r3                          \n"
        "   cpsie   i                                   \n"
        "v8b_pendsv_exit:                               \n"
        "   bx      lr                                  \n"
    );
}
#endif

__attribute__((naked)) void SVC_Handler(void)
{
    __ASM volatile(
#if !defined(__ICCARM__)
    ".syntax unified                        \n"
#endif
    "MRS     r0, MSP                        \n"
    "MOV     r1, lr                         \n"
    "MRS     r2, PSP                        \n"
    "MRS     r3, PSPLIM                     \n"
    "PUSH    {r2, r3}                       \n" /* PSP PSPLIM */
    "PUSH    {r1, r2}                       \n" /* Orig_exc_return, dummy */
    "BL      tfm_core_svc_handler           \n"
    "MOV     lr, r0                         \n"
    "POP     {r1, r2}                       \n" /* Orig_exc_return, dummy */
    "MOVS    r2, #8                         \n"
    "ANDS    r0, r2                         \n" /* Mode bit */
    "ANDS    r1, r2                         \n"
    "POP     {r2, r3}                       \n" /* PSP PSPLIM */
    "SUBS    r0, r1                         \n" /* Compare EXC_RETURN values */
    "BGT     to_flih_func                   \n"
    "BLT     from_flih_func                 \n"
    "BX      lr                             \n"
    "to_flih_func:                          \n"
    "PUSH    {r2, r3}                       \n" /* PSP PSPLIM */
    "PUSH    {r0, r1}                       \n"
    "LDR     r0, ="M2S(EXC_RETURN_DCRS)"    \n" /* Check DCRS */
    "MOV     r1, lr                         \n"
    "ANDS    r0, r1                         \n"
    "BEQ     v8b_svc_callee_saved           \n" /* Skip saving callee */
    "PUSH    {r4-r7}                        \n" /* Save callee */
    "MOV     r4, r8                         \n"
    "MOV     r5, r9                         \n"
    "MOV     r6, r10                        \n"
    "MOV     r7, r11                        \n"
    "PUSH    {r4-r7}                        \n"
    "SUB     sp, #8                         \n" /* Dummy data to align SP offset for
                                                 * reserved additional state context,
                                                 * integrity signature
                                                 */
    "v8b_svc_callee_saved:                  \n"
    "POP     {r0, r1}                       \n"
    "LDR     r4, ="M2S(STACK_SEAL_PATTERN)" \n" /* clear r4-r11 */
    "MOV     r5, r4                         \n"
    "MOV     r6, r4                         \n"
    "MOV     r7, r4                         \n"
    "MOV     r8, r4                         \n"
    "MOV     r9, r4                         \n"
    "MOV     r10, r4                        \n"
    "MOV     r11, r4                        \n"
    "PUSH    {r4, r5}                       \n" /* Seal stack before EXC_RET */
    "BX      lr                             \n"
    "from_flih_func:                        \n"
    "POP     {r4, r5}                       \n" /* Seal stack */
    "PUSH    {r0, r1}                       \n"
    "LDR     r0, ="M2S(EXC_RETURN_DCRS)"    \n" /* Check DCRS */
    "MOV     r1, lr                         \n"
    "ANDS    r0, r1                         \n"
    "BEQ     v8b_svc_callee_loaded          \n" /* Skip loading callee */
    "ADD     sp, #8                         \n" /* Dummy data to align SP offset for
                                                 * reserved additional state context,
                                                 * integrity signature
                                                 */
    "POP     {r4-r7}                        \n"
    "MOV     r8, r4                         \n"
    "MOV     r9, r5                         \n"
    "MOV     r10, r6                        \n"
    "MOV     r11, r7                        \n"
    "POP     {r4-r7}                        \n"
    "v8b_svc_callee_loaded:                 \n"
    "POP     {r0, r1}                       \n"
    "POP     {r1, r2}                       \n" /* PSP PSPLIM */
    "BX      lr                             \n"
    );
}

void tfm_arch_set_secure_exception_priorities(void)
{
    uint32_t VECTKEY;
    SCB_Type *scb = SCB;
    uint32_t AIRCR;

    /* Set PRIS flag in AIRCR */
    AIRCR = scb->AIRCR;
    VECTKEY = (~AIRCR & SCB_AIRCR_VECTKEYSTAT_Msk);
    scb->AIRCR = SCB_AIRCR_PRIS_Msk |
                 VECTKEY |
                 (AIRCR & ~SCB_AIRCR_VECTKEY_Msk);

    NVIC_SetPriority(SVCall_IRQn, SVCall_IRQnLVL);
    /*
     * Set secure PendSV priority to the lowest in SECURE state.
     */
    NVIC_SetPriority(PendSV_IRQn, PENDSV_PRIO_FOR_SCHED);
}

#ifdef TFM_FIH_PROFILE_ON
FIH_RET_TYPE(int32_t) tfm_arch_verify_secure_exception_priorities(void)
{
    SCB_Type *scb = SCB;

    if ((scb->AIRCR & SCB_AIRCR_PRIS_Msk) !=  SCB_AIRCR_PRIS_Msk) {
        FIH_RET(FIH_FAILURE);
    }
    fih_delay();
    if ((scb->AIRCR & SCB_AIRCR_PRIS_Msk) !=  SCB_AIRCR_PRIS_Msk) {
        FIH_RET(FIH_FAILURE);
    }
    if (fih_not_eq(fih_int_encode(NVIC_GetPriority(SVCall_IRQn)),
                  fih_int_encode(SVCall_IRQnLVL))) {
        FIH_RET(FIH_FAILURE);
    }
    if (fih_not_eq(fih_int_encode(NVIC_GetPriority(PendSV_IRQn)),
                  fih_int_encode(PENDSV_PRIO_FOR_SCHED))) {
        FIH_RET(FIH_FAILURE);
    }
    FIH_RET(FIH_SUCCESS);
}
#endif

/* There are no coprocessors in Armv8-M Baseline implementations */
void tfm_arch_config_extensions(void)
{
}
