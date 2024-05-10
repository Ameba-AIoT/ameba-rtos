/** @file
 * Copyright (c) 2018-2019, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

#include "val_test_common.h"

#define   TIMER_VALUE_US  1 * 10
#define   KEY_SIZE        32            /*Size in Bytes*/

tbsa_val_api_t        *g_val;
uint8_t               g_key[KEY_SIZE] = {0}, g_key_save[KEY_SIZE] = {0};
soc_peripheral_hdr_t  *soc_peripheral_hdr;
soc_peripheral_desc_t *soc_peripheral_desc = NULL;
bool_t                s_timer_present = 0;
int                   g_exception_taken = 0, g_reinit_taken = 0;
clocks_desc_t         *clocks_desc;
uint32_t              g_timer = TIMER_VALUE_US;

/*  Publish these functions to the external world as associated to this test ID */
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_CRYPTO_BASE, 1),
                  CREATE_TEST_TITLE("Check key operations cannot be interrupted"),
                  CREATE_REF_TAG("R010/R020_TBSA_KEY"),
                  entry_hook,
                  test_payload,
                  exit_hook);

void entry_hook(tbsa_val_api_t *val)
{
    tbsa_test_init_t init = {
                             .bss_start      = &__tbsa_test_bss_start__,
                             .bss_end        = &__tbsa_test_bss_end__
                            };

    val->test_initialize(&init);

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

tbsa_status_t reinit_timer(tbsa_val_api_t *val)
{
    tbsa_status_t status;

    for(int i=0; i<KEY_SIZE; i++)
        g_key_save[i] = 0;

    g_timer = g_timer - 1;
    status = val->interrupt_disable(EXCP_NUM_EXT_INT(soc_peripheral_desc->intr_id));
    if (val->err_check_set(TEST_CHECKPOINT_7, status)) {
        return status;
    }

    status = val->timer_disable(soc_peripheral_desc->base);
    if (val->err_check_set(TEST_CHECKPOINT_8, status)) {
        return status;
    }

    /* Reinit the timer */
    status = val->timer_init(soc_peripheral_desc->base, g_timer , ((clocks_desc->sys_freq)/1000000));
    if (val->err_check_set(TEST_CHECKPOINT_9, status)) {
        return status;
    }

    status = val->interrupt_enable(EXCP_NUM_EXT_INT(soc_peripheral_desc->intr_id));
    if (val->err_check_set(TEST_CHECKPOINT_A, status)) {
        return status;
    }

    status = val->timer_enable(soc_peripheral_desc->base);
    if (val->err_check_set(TEST_CHECKPOINT_B, status)) {
        return status;
    }

    g_exception_taken = 0;
    return status;
}

static void timer_isr(void)
{
    uint32_t key_sum=0, i;

    g_exception_taken++;
    for(i=0; i<KEY_SIZE; i++)
        key_sum += g_key[i];

    if (key_sum == 0) {
        g_val->timer_interrupt_clear(soc_peripheral_desc->base);
    } else {
        /* Disable the interrupt only if occured 5 or more times */
        if(g_exception_taken >= 5)
        {
            g_val->timer_disable(soc_peripheral_desc->base);
            g_val->interrupt_disable(EXCP_NUM_EXT_INT(soc_peripheral_desc->intr_id));
            for(i=0; i<KEY_SIZE; i++)
                g_key_save[i] = g_key[i];
        }
    }
    return;
}

tbsa_status_t program_timer(tbsa_val_api_t *val)
{
    uint32_t index = 0;
    tbsa_status_t status;

    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL, 0, 0),
                                    (uint8_t **)&soc_peripheral_hdr,
                                    (uint32_t *)sizeof(soc_peripheral_hdr_t));
    if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
        return status;
    }

    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_CLOCKS, CLOCKS_SYS_FREQ, 0),
                                    (uint8_t **)&clocks_desc,
                                    (uint32_t *)sizeof(clocks_desc_t));
    if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
        return status;
    }

    /* Check we have at least one trusted timer */
    while (index < soc_peripheral_hdr->num) {
        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL, SOC_PERIPHERAL_TIMER, index),
                                        (uint8_t **)&soc_peripheral_desc,
                                        (uint32_t *)sizeof(soc_peripheral_desc_t));
        if (val->err_check_set(TEST_CHECKPOINT_4, status)) {
            return status;
        }

        if (soc_peripheral_desc->attribute != SECURE_PROGRAMMABLE) {
            index++;
            continue;
        }
        s_timer_present=1;

        status = val->interrupt_setup_handler(EXCP_NUM_EXT_INT(soc_peripheral_desc->intr_id), 0, timer_isr);
        if (val->err_check_set(TEST_CHECKPOINT_5, status)) {
            return status;
        }

        break;
    }
    if (s_timer_present)
        return TBSA_STATUS_SUCCESS;
    else
        return TBSA_STATUS_NOT_FOUND;

}

void test_payload(tbsa_val_api_t *val)
{
    tbsa_status_t status;
    g_val = val;
    uint32_t i, timeout = 1000;

    status = val->crypto_set_base_addr(SECURE_PROGRAMMABLE);
    if (val->err_check_set(TEST_CHECKPOINT_1, status)) {
        return;
    }

    status = program_timer(val);
    if (val->err_check_set(TEST_CHECKPOINT_6, status)) {
        return;
    }

    /* Check if we receive at least 3 interrupts before the atomic
     * key generation is done. If not reinit the timer and retry
     * until we receive the expected number of exceptions. Reinit
     * is done only 3 times if the number of exceptions received
     * is zero.
     */
    while(g_exception_taken < 5) {
        if (g_reinit_taken < 4) {
            if (g_exception_taken == 0)
                g_reinit_taken++;

            status = reinit_timer(val);
            if (val->err_check_set(TEST_CHECKPOINT_C, status)) {
                return;
            }

            for(i=0; i<KEY_SIZE; i++) {
               g_key[i] = 0;
               g_key_save[i] = 0;
            }
            status = val->crypto_key_generate(g_key, AES, KEY_SIZE);
            if (val->err_check_set(TEST_CHECKPOINT_D, status)) {
                return;
            }

        } else {
            val->set_status(RESULT_SKIP(1));
            val->print(PRINT_ERROR, "\n\r\tUnable to receive any interrupts", 0);
            return;
        }
    }

    while (--timeout);

    for(i=0; i<KEY_SIZE; i++) {
        if(g_key[i] != g_key_save[i]) {
            val->err_check_set(TEST_CHECKPOINT_E, TBSA_STATUS_ERROR);
            val->print(PRINT_ERROR, "\n\r\tKey generation from secure state was interrupted", 0);
            return;
        } else {
            val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
        }
    }

    return;
}

void exit_hook(tbsa_val_api_t *val)
{
    if (soc_peripheral_desc != NULL) {
        val->interrupt_disable(EXCP_NUM_EXT_INT(soc_peripheral_desc->intr_id));
        val->timer_disable(soc_peripheral_desc->base);
    }
}
