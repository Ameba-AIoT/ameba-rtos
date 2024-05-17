/*
 * Copyright (c) 2021-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>
#include "fpu_tests_common.h"

__attribute__((naked))
void fp_func_jump_template(uintptr_t *func_table, uintptr_t *func_return,
                           uint32_t func_num)
{
    __asm volatile(
        "cmp       r2, #0           \n"
        "beq       jump_exit        \n"
        "push      {r4-r6, lr}      \n"
        "mov       r4, r0           \n"
        "mov       r5, r1           \n"
        "mov       r6, r2           \n"
    "jump_loop:                     \n"
        /* Load func address to r2.
         * Move r4 to the address of func param. */
        "ldr       r2, [r4], #4     \n"
        /* Load func param to r0.
         * Move r4 to the address of next func.*/
        "ldr       r0, [r4], #4     \n"
        /* Jump to target func. */
        "blx       r2               \n"
        /* Store return value of finished func.
         * Move r5 to store return value of next func. */
        "str       r0, [r5], #4     \n"
        "subs      r6, r6, #1       \n"
        "bne       jump_loop        \n"
        "pop       {r4-r6, pc}      \n"
    "jump_exit:                     \n"
        "bx        lr               \n"
    );
}

__attribute__((naked))
void dump_fp_caller(uint32_t *fp_caller_buffer)
{
    __asm volatile(
        "vstm      r0, {s0-s15}     \n"
        "bx        lr               \n"
    );
}

__attribute__((naked))
void dump_fp_callee(uint32_t *fp_callee_buffer)
{
    __asm volatile(
        "vstm      r0, {s16-s31}    \n"
        "bx        lr               \n"
    );
}

__attribute__((naked))
void populate_caller_fp_regs(const uint32_t *expecting_caller_content)
{
    __asm volatile(
        "vldm      r0, {s0-s15}     \n"
        "bx        lr               \n"
    );
}

__attribute__((naked))
void populate_callee_fp_regs(const uint32_t *expecting_callee_content)
{
    __asm volatile(
        "vldm      r0, {s16-s31}    \n"
        "bx        lr               \n"
    );
}

void tfm_fpu_test_clear_client_fp_data(struct test_result_t *ret)
{
    uint32_t fp_caller_buffer[NR_FP_CALLER_REG] = {0};
    uint32_t fp_callee_buffer[NR_FP_CALLEE_REG] = {0};
    const uint32_t expecting_caller_content[NR_FP_CALLER_REG] = {0};
    const uint32_t expecting_callee_content[NR_FP_CALLEE_REG] = {0};

    uintptr_t func_table[] = {
        (uintptr_t)populate_caller_fp_regs, (uintptr_t)expecting_caller_content,
        (uintptr_t)populate_callee_fp_regs, (uintptr_t)expecting_callee_content,
        (uintptr_t)dump_fp_caller, (uintptr_t)fp_caller_buffer,
        (uintptr_t)dump_fp_callee, (uintptr_t)fp_callee_buffer
    };
    uintptr_t func_return[ARRAY_SIZE(func_table)/2] = {0};

    ret->val = TEST_PASSED;

    fp_func_jump_template(func_table, func_return, ARRAY_SIZE(func_table)/2);

    if (memcmp(fp_caller_buffer, expecting_caller_content,
               FP_CALLER_BUF_SIZE)) {
        TEST_FAIL("FP caller registers are not correctly cleared!");
    }

    if (memcmp(fp_callee_buffer, expecting_callee_content,
               FP_CALLEE_BUF_SIZE)) {
        TEST_FAIL("FP callee registers are not correctly cleared!");
    }
}

__attribute__((naked)) uint32_t fpu_interrupt_trigger(uint32_t IRQ_NUM){
    __asm volatile(
        "push    {r7, lr}                  \n"
        /* Software Trigger Interrupt Register address is 0xE000EF00. */
        "ldr     r7, =0xE000EF00           \n"
        "str     r0, [r7]                  \n"
        "dsb     0xF                       \n"
        "pop     {r7, pc}                  \n"
    );
}
