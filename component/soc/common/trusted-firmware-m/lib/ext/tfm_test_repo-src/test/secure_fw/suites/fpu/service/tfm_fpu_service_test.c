/*
 * Copyright (c) 2021-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "psa/client.h"
#include "psa/service.h"
#include "psa_manifest/tfm_fpu_service_test.h"
#include "tfm_api.h"
#include "tfm_hal_isolation.h"
#include "tfm_sp_log.h"
#include "tfm_plat_test.h"
#include "fpu_tests_common.h"
#include "utilities.h"

/*
 * Description: Service handler for checking FP register.
 * Expectation: FP callee registers should be restored correctly in service.
 */
static psa_status_t fpu_service_check_fp_register(const psa_msg_t *msg)
{
    psa_status_t status = PSA_SUCCESS;

#if TFM_SP_FPU_SERVICE_TEST_MODEL_SFN == 1
    return status;
#elif TFM_SP_FPU_SERVICE_TEST_MODEL_IPC == 1
    uint32_t fp_callee_buffer[NR_FP_CALLEE_REG] = {0};
    const uint32_t fp_clear_callee_content[NR_FP_CALLEE_REG] = {0};
    const uint32_t expecting_callee_content[NR_FP_CALLEE_REG] = {
        0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
        0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
    };

    switch (msg->type) {
    case PSA_IPC_CONNECT:
        break;
    case PSA_IPC_CALL:
        dump_fp_callee(fp_callee_buffer);
        if (!memcmp(fp_callee_buffer, fp_clear_callee_content,
                    FP_CALLEE_BUF_SIZE)) {
            populate_callee_fp_regs(expecting_callee_content);
        } else {
            status = PSA_ERROR_GENERIC_ERROR;
        }
        break;
    case PSA_IPC_DISCONNECT:
        dump_fp_callee(fp_callee_buffer);
        if ((!memcmp(fp_callee_buffer, expecting_callee_content,
                     FP_CALLEE_BUF_SIZE))) {
            populate_callee_fp_regs(fp_clear_callee_content);
        }
        break;
    default:
        psa_panic();
        break;
    }
    psa_reply(msg->handle, status);
#endif /* TFM_SP_FPU_SERVICE_TEST_MODEL_SFN == 1 */
}

/* Service handler for trigger NS interrupt. */
static psa_status_t fpu_service_check_fp_register_after_ns_inturrept(
                    const psa_msg_t *msg)
{
    psa_status_t status = PSA_SUCCESS;
    uint32_t fp_caller_buffer[NR_FP_CALLER_REG] = {0};
    uint32_t fp_callee_buffer[NR_FP_CALLEE_REG] = {0};
    const uint32_t expecting_caller_content[NR_FP_CALLER_REG] = {
        0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
        0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF
    };
    const uint32_t expecting_callee_content[NR_FP_CALLEE_REG] = {
        0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
        0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF
    };

    uintptr_t func_table[] = {
        (uintptr_t)populate_caller_fp_regs, (uintptr_t)expecting_caller_content,
        (uintptr_t)populate_callee_fp_regs, (uintptr_t)expecting_callee_content,
        (uintptr_t)fpu_interrupt_trigger, (uintptr_t)TFM_FPU_NS_TEST_IRQ,
        (uintptr_t)dump_fp_caller, (uintptr_t)fp_caller_buffer,
        (uintptr_t)dump_fp_callee, (uintptr_t)fp_callee_buffer
    };
    uintptr_t func_return[ARRAY_SIZE(func_table)/2] = {0};


    switch (msg->type) {
    case PSA_IPC_CONNECT:
        break;
    case PSA_IPC_CALL:
        fp_func_jump_template(func_table, func_return, ARRAY_SIZE(func_table)/2);

        if (memcmp(fp_caller_buffer, expecting_caller_content,
                   FP_CALLER_BUF_SIZE)) {
            TEST_LOG("FP caller registers are not correctly retored!");
            status = PSA_ERROR_GENERIC_ERROR;
        } else {
            if (memcmp(fp_callee_buffer, expecting_callee_content,
                       FP_CALLEE_BUF_SIZE)) {
                TEST_LOG("FP callee registers are not correctly retored!");
                status = PSA_ERROR_GENERIC_ERROR;
            }
        }
        break;
    case PSA_IPC_DISCONNECT:
        break;
    default:
        psa_panic();
        break;
    }
#if TFM_SP_FPU_SERVICE_TEST_MODEL_SFN == 1
    return status;
#elif TFM_SP_FPU_SERVICE_TEST_MODEL_IPC == 1
    psa_reply(msg->handle, status);
#endif /* TFM_SP_FPU_SERVICE_TEST_MODEL_SFN == 1 */
}

#if TFM_SP_FPU_SERVICE_TEST_MODEL_SFN == 1

psa_status_t tfm_fpu_check_fp_callee_register_sfn(const psa_msg_t* msg) {
    return fpu_service_check_fp_register(msg);
}

psa_status_t tfm_fpu_test_ns_preempt_s_sfn(const psa_msg_t* msg) {
    return fpu_service_check_fp_register_after_ns_inturrept(msg);
}

#elif TFM_SP_FPU_SERVICE_TEST_MODEL_IPC == 1
/* FP service partition main thread. */
void fpu_service_test_main()
{
    uint32_t signals = 0;
    psa_msg_t msg;

    while (1) {
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);
        if (signals & TFM_FPU_CHECK_FP_CALLEE_REGISTER_SIGNAL) {
            psa_get(TFM_FPU_CHECK_FP_CALLEE_REGISTER_SIGNAL, &msg);
            fpu_service_check_fp_register(&msg);
        } else if (signals & TFM_FPU_TEST_NS_PREEMPT_S_SIGNAL) {
            psa_get(TFM_FPU_TEST_NS_PREEMPT_S_SIGNAL, &msg);
            fpu_service_check_fp_register_after_ns_inturrept(&msg);
        } else {
            psa_panic();
        }
    }
}
#endif /* TFM_SP_FPU_SERVICE_TEST_MODEL_SFN == 1 */
