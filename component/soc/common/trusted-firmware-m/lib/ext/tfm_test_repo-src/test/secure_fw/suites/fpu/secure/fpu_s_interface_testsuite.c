/*
 * Copyright (c) 2021-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "fpu_s_tests.h"
#include "../fpu_tests_common.h"
#include "tfm_psa_call_pack.h"
#include "utilities.h"

/* Test FP context protection after psa calls. */
static void tfm_fpu_test_fp_protection_s_psa_call(struct test_result_t *ret);

/* Test reliability of FP context protection after psa calls by loops. */
static void tfm_fpu_test_fp_protection_s_psa_call_loop(
                                                    struct test_result_t *ret);

/* Test FP caller and callee protection after secure interrupt. */
static void tfm_fpu_test_fp_protection_s_after_s_interrupt(
                                                    struct test_result_t *ret);

static struct test_t fpu_s_tests[] = {
    {
        &tfm_fpu_test_clear_client_fp_data, "TFM_S_FPU_TEST_1001",
        "Clear FP registers in FPU client partition"
    },
    {
        &tfm_fpu_test_fp_protection_s_psa_call_loop, "TFM_S_FPU_TEST_1002",
        "Test reliability of FP context protection after psa calls"
    },
    {
        &tfm_fpu_test_fp_protection_s_after_s_interrupt, "TFM_S_FPU_TEST_1003",
        "Test FP context protection in S thread after S interrupt"
    },
};

void register_testsuite_s_fpu_interface(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(fpu_s_tests) / sizeof(fpu_s_tests[0]));

    set_testsuite("FPU secure interface test (TFM_S_FPU_TEST_1XXX)",
                  fpu_s_tests, list_size, p_test_suite);
}

/*
 * Description: Directly jump to tfm_psa_call_pack from S side.
 * Expectation: Return psa_status_t status code.
 */
__attribute__((naked)) static uint32_t fpu_s_call(uintptr_t psa_call_param)
{
        __asm volatile(
#if !defined(__ICCARM__)
        ".syntax unified                          \n"
#endif
        "   push    {r4-r6, lr}                   \n"
        "   mov     r4, r0                        \n"
        /* Load params of tfm_psa_call_pack into r0~r3. */
        "   ldr     r0, [r4], #4                  \n"  /* psa handle */
        "   ldr     r1, [r4], #4                  \n"  /* ctrl_param */
        "   ldr     r2, [r4], #4                  \n"  /* in_vec */
        "   ldr     r3, [r4]                      \n"  /* out_vec */
        "   ldr     r5, ="M2S(tfm_psa_call_pack)" \n"
        "   blx     r5                            \n"
        "   pop     {r4-r6, pc}                   \n"
    );
}

/*
 * Description: Test FP context protection after psa calls. Change FP callee
 * registers in FPU client/service partition separately, then check FP callee
 * registers after psa calls.
 * Expectation: FP callee registers in FPU client/service partition should be
 * saved and restored correctly.
 */
static void tfm_fpu_test_fp_protection_s_psa_call(struct test_result_t *ret)
{
    psa_handle_t handle;
    uint32_t param;
    uint32_t psa_call_param[PSA_CALL_PARAM_LEN] = {0};
    uint32_t fp_callee_buffer[NR_FP_CALLEE_REG] = {0};
    const uint32_t expecting_callee_content[NR_FP_CALLEE_REG] = {
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
        0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF
    };

    uintptr_t func_table[] = {
        (uintptr_t)populate_callee_fp_regs, (uintptr_t)expecting_callee_content,
        (uintptr_t)fpu_s_call, (uintptr_t)psa_call_param,
        (uintptr_t)dump_fp_callee, (uintptr_t)fp_callee_buffer
    };
    uintptr_t func_return[ARRAY_SIZE(func_table)/2] = {0};

    ret->val = TEST_PASSED;

    handle = psa_connect(TFM_FPU_CHECK_FP_CALLEE_REGISTER_SID,
                         TFM_FPU_CHECK_FP_CALLEE_REGISTER_VERSION);
    if (!PSA_HANDLE_IS_VALID(handle)) {
        TEST_FAIL("PSA Connect fail!");
        return;
    }

    param = PARAM_PACK(PSA_IPC_CALL, 0, 0);
    psa_call_param[0] = (uint32_t)handle;
    psa_call_param[1] = param;

    fp_func_jump_template(func_table, func_return, ARRAY_SIZE(func_table)/2);

    if ((psa_status_t)func_return[1] != PSA_SUCCESS) {
        TEST_FAIL("FP callee registers check fail in service!");
    }

    if (memcmp(fp_callee_buffer, expecting_callee_content,
               FP_CALLEE_BUF_SIZE)) {
        TEST_FAIL("FP callee registers are not correctly restored in client!");
    }

    psa_close(handle);
}

/*
 * Description: Test reliability of FP context protection after psa calls by
 * loops. Change FP callee registers in FPU client/service partition separately,
 * then check FP callee registers after psa calls.
 * Expectation: FP callee registers in FPU client/service partition should be
 * saved and restored correctly.
 */
static void tfm_fpu_test_fp_protection_s_psa_call_loop(
                                                    struct test_result_t *ret)
{
    uint32_t itr;

    for (itr = 0; itr < LOOP_ITERATIONS; itr++) {
        TEST_LOG("  > Iteration %d of %d\r", itr + 1, LOOP_ITERATIONS);

        tfm_fpu_test_fp_protection_s_psa_call(ret);
    }
}

/*
 * Description: Secure client trigger a secure interrupt and check caller and
 * callee registers.
 * Expectation: FP register in secure client should be restored after S
 * interrupt.
 */
static void tfm_fpu_test_fp_protection_s_after_s_interrupt(
                                                struct test_result_t *ret)
{
    uint32_t fp_caller_buffer[NR_FP_CALLER_REG] = {0};
    uint32_t fp_callee_buffer[NR_FP_CALLEE_REG] = {0};
    const uint32_t expecting_caller_content[NR_FP_CALLER_REG] = {
        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
        0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF
    };
    const uint32_t expecting_callee_content[NR_FP_CALLEE_REG] = {
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
        0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF
    };

    ret->val = TEST_PASSED;

    uintptr_t func_table[] = {
        (uintptr_t)populate_caller_fp_regs, (uintptr_t)expecting_caller_content,
        (uintptr_t)populate_callee_fp_regs, (uintptr_t)expecting_callee_content,
        (uintptr_t)fpu_interrupt_trigger, (uintptr_t)TFM_FPU_S_TEST_IRQ,
        (uintptr_t)dump_fp_caller, (uintptr_t)fp_caller_buffer,
        (uintptr_t)dump_fp_callee, (uintptr_t)fp_callee_buffer
    };
    uintptr_t func_return[ARRAY_SIZE(func_table)/2] = {0};

    fp_func_jump_template(func_table, func_return, ARRAY_SIZE(func_table)/2);

    if (memcmp(fp_caller_buffer, expecting_caller_content,
               FP_CALLER_BUF_SIZE)) {
        TEST_FAIL("FP caller registers are not correctly retored in client!");
    }

    if (memcmp(fp_callee_buffer, expecting_callee_content,
               FP_CALLEE_BUF_SIZE)) {
        TEST_FAIL("FP callee registers are not correctly retored in client!");
    }
}
