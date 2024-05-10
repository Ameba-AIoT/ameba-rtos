/** @file
 * Copyright (c) 2018-2022, Arm Limited or its affiliates. All rights reserved.
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

#include "val_driver_service_apis.h"
#define DATA_VALUE  0x1111
#define BUFFER_SIZE 4

#if SPEC_VERSION == 11

#define DRIVER_UART_INTR_SIG DRIVER_UART_INTR_SIG_SIGNAL

#endif

uint32_t g_psa_rot_data = DATA_VALUE;

int32_t driver_test_psa_eoi_with_non_intr_signal(void);
int32_t driver_test_psa_eoi_with_unasserted_signal(void);
int32_t driver_test_psa_eoi_with_multiple_signals(void);
int32_t driver_test_irq_routing(void);
void driver_test_isolation_psa_rot_data_rd(psa_msg_t *msg);
void driver_test_isolation_psa_rot_data_wr(psa_msg_t *msg);
void driver_test_isolation_psa_rot_stack_rd(psa_msg_t *msg);
void driver_test_isolation_psa_rot_stack_wr(psa_msg_t *msg);
void driver_test_isolation_psa_rot_heap_rd(psa_msg_t *msg);
void driver_test_isolation_psa_rot_heap_wr(psa_msg_t *msg);
void driver_test_isolation_psa_rot_mmio_rd(psa_msg_t *msg);
void driver_test_isolation_psa_rot_mmio_wr(psa_msg_t *msg);

void driver_main(void)
{
    psa_signal_t          signals = 0;
    psa_msg_t             msg = {0};
    int32_t               data = 0;
    uart_fn_type_t        uart_fn;
    nvmem_param_t         nvmem_param;
    driver_test_fn_id_t   driver_test_fn_id;
    char                  string[256] = {0};
    uint8_t               buffer[256] = {0};
    wd_param_t            wd_param;
    addr_t                uart_base;

    /* Initialised driver mmio space */
    if (val_init_driver_memory())
        TEST_PANIC();

    while (1)
    {
        val_status_t fn_status = VAL_STATUS_SUCCESS;
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);

        /* Service Print functionality */
        if (signals & DRIVER_UART_SIGNAL)
        {
            psa_get(DRIVER_UART_SIGNAL, &msg);
            switch (msg.type)
            {
                case PSA_IPC_CONNECT:
                    psa_reply(msg.handle, PSA_SUCCESS);
                    break;
                case PSA_IPC_CALL:
                    if ((msg.in_size[0] > sizeof(uart_fn))
                        && (msg.in_size[1] > sizeof(string))
                        && (msg.in_size[2] > sizeof(data)))
                    {
                        /* buffer overflow */
                        psa_reply(msg.handle, VAL_STATUS_ERROR);
                        break;
                    }
                    else
                    {
                        psa_read(msg.handle, 0, &uart_fn, msg.in_size[0]);
                    }

                    if (uart_fn == UART_INIT)
                    {
                        /* arg2=uart_base */
                        psa_read(msg.handle, 1, &uart_base, msg.in_size[1]);
                        fn_status = val_uart_init_sf(uart_base);
                    }
                    else
                    {
                        /* arg2=string, arg3=DATA */
                        psa_read(msg.handle, 1, &string, msg.in_size[1]);
                        psa_read(msg.handle, 2, &data, msg.in_size[2]);
                        fn_status = val_print_sf(string, data);
                    }
                    if (VAL_ERROR(fn_status))
                    {
                        psa_reply(msg.handle, VAL_STATUS_ERROR);
                    }
                    else
                    {
                        psa_reply(msg.handle, PSA_SUCCESS);
                    }
                    break;
                case PSA_IPC_DISCONNECT:
                    psa_reply(msg.handle, PSA_SUCCESS);
                    break;
            }
        }

        /* Service Watchdog functionality */
        else if (signals & DRIVER_WATCHDOG_SIGNAL)
        {
            psa_get(DRIVER_WATCHDOG_SIGNAL, &msg);
            switch (msg.type)
            {
                case PSA_IPC_CALL:
                    if (msg.in_size[0] > sizeof(wd_param))
                    {
                        /* buffer overflow */
                        psa_reply(msg.handle, VAL_STATUS_ERROR);
                        val_print_sf("msg.in_size[0] buffer overflow\n", 0);
                        break;
                    }
                    else
                    {
                        psa_read(msg.handle, 0, &wd_param, msg.in_size[0]);
                    }

                    if (wd_param.wd_fn_type == WD_INIT_SEQ)
                    {
                        fn_status = val_wd_timer_init_sf(wd_param.wd_base_addr,
                                                         wd_param.wd_time_us,
                                                         wd_param.wd_timer_tick_us);
                    }
                    else if (wd_param.wd_fn_type == WD_ENABLE_SEQ)
                    {
                        fn_status = val_wd_timer_enable_sf(wd_param.wd_base_addr);
                    }
                    else if (wd_param.wd_fn_type == WD_DISABLE_SEQ)
                    {
                        fn_status = val_wd_timer_disable_sf(wd_param.wd_base_addr);
                    }
                    else if (wd_param.wd_fn_type == WD_STATUS_SEQ)
                    {
                        fn_status = val_is_wd_timer_enabled_sf(wd_param.wd_base_addr);
                    }
                    if (fn_status == VAL_STATUS_SUCCESS)
                    {
                        psa_reply(msg.handle, PSA_SUCCESS);
                    }
                    else
                    {
                        psa_reply(msg.handle, VAL_STATUS_ERROR);
                    }
                    break;
                case PSA_IPC_CONNECT:
                case PSA_IPC_DISCONNECT:
                    psa_reply(msg.handle, PSA_SUCCESS);
                    break;
            }
        }

         /* Service NVMEM functionality */
        else if (signals & DRIVER_NVMEM_SIGNAL)
        {
            psa_get(DRIVER_NVMEM_SIGNAL, &msg);
            switch (msg.type)
            {
                case PSA_IPC_CALL:
                    if (msg.in_size[0] > sizeof(nvmem_param))
                    {
                        /* buffer overflow */
                        psa_reply(msg.handle, VAL_STATUS_ERROR);
                        val_print_sf("msg.in_size[0] buffer overflow\n", 0);
                        break;
                    }
                    else
                    {
                        psa_read(msg.handle, 0, &nvmem_param, msg.in_size[0]);
                    }

                    if (nvmem_param.nvmem_fn_type == NVMEM_READ)
                    {
                        fn_status = val_nvmem_read_sf(nvmem_param.base,
                                                      nvmem_param.offset,
                                                      buffer,
                                                      nvmem_param.size);
                        psa_write(msg.handle, 0, (const void*) buffer, msg.out_size[0]);
                    }
                    else
                    {
                        if (msg.in_size[1] > sizeof(buffer))
                        {
                            /* buffer overflow */
                            fn_status = VAL_STATUS_ERROR;
                            val_print_sf("msg.in_size[1] buffer overflow\n", 0);
                        }
                        else
                        {
                            psa_read(msg.handle, 1, (void*) buffer, msg.in_size[1]);
                            fn_status = val_nvmem_write_sf(nvmem_param.base,
                                                           nvmem_param.offset,
                                                           buffer,
                                                           nvmem_param.size);
                        }
                    }
                    if (fn_status == VAL_STATUS_SUCCESS)
                    {
                        psa_reply(msg.handle, PSA_SUCCESS);
                    }
                    else
                    {
                        psa_reply(msg.handle, VAL_STATUS_ERROR);
                    }
                    break;
                case PSA_IPC_CONNECT:
                case PSA_IPC_DISCONNECT:
                    psa_reply(msg.handle, PSA_SUCCESS);
                    break;
            }
        }
        /* SID reserved for Driver test functions */
        else if (signals & DRIVER_TEST_SIGNAL)
        {
            psa_get(DRIVER_TEST_SIGNAL, &msg);
            switch (msg.type)
            {
                case PSA_IPC_CALL:
                    if (msg.in_size[0] > sizeof(driver_test_fn_id_t))
                    {
                        /* buffer overflow */
                        psa_reply(msg.handle, VAL_STATUS_ERROR);
                        val_print_sf("msg.in_size[0] buffer overflow\n", 0);
                        break;
                    }
                    else
                    {
                        psa_read(msg.handle, 0, &driver_test_fn_id, msg.in_size[0]);
                    }

                    switch (driver_test_fn_id)
                    {
                        case TEST_PSA_EOI_WITH_NON_INTR_SIGNAL:
                             driver_test_psa_eoi_with_non_intr_signal();
                             psa_reply(msg.handle, VAL_STATUS_ERROR);
                             break;
                        case TEST_PSA_EOI_WITH_UNASSERTED_SIGNAL:
                             driver_test_psa_eoi_with_unasserted_signal();
                             psa_reply(msg.handle, VAL_STATUS_ERROR);
                             break;
                        case TEST_PSA_EOI_WITH_MULTIPLE_SIGNALS:
                             driver_test_psa_eoi_with_multiple_signals();
                             psa_reply(msg.handle, VAL_STATUS_ERROR);
                             break;
                        case TEST_INTR_SERVICE:
                             if (driver_test_irq_routing() != VAL_STATUS_SUCCESS)
                                 psa_reply(msg.handle, VAL_STATUS_ERROR);
                             else
                                 psa_reply(msg.handle, PSA_SUCCESS);
                             break;
                        case TEST_ISOLATION_PSA_ROT_DATA_RD:
                             driver_test_isolation_psa_rot_data_rd(&msg);
                             break;
                        case TEST_ISOLATION_PSA_ROT_DATA_WR:
                             driver_test_isolation_psa_rot_data_wr(&msg);
                             break;
                        case TEST_ISOLATION_PSA_ROT_STACK_RD:
                             driver_test_isolation_psa_rot_stack_rd(&msg);
                             break;
                        case TEST_ISOLATION_PSA_ROT_STACK_WR:
                             driver_test_isolation_psa_rot_stack_wr(&msg);
                             break;
                        case TEST_ISOLATION_PSA_ROT_HEAP_RD:
                             driver_test_isolation_psa_rot_heap_rd(&msg);
                             break;
                        case TEST_ISOLATION_PSA_ROT_HEAP_WR:
                             driver_test_isolation_psa_rot_heap_wr(&msg);
                             break;
                        case TEST_ISOLATION_PSA_ROT_MMIO_RD:
                             driver_test_isolation_psa_rot_mmio_rd(&msg);
                             break;
                        case TEST_ISOLATION_PSA_ROT_MMIO_WR:
                             driver_test_isolation_psa_rot_mmio_wr(&msg);
                             break;
                    }
                    break;
                case PSA_IPC_CONNECT:
                case PSA_IPC_DISCONNECT:
                    psa_reply(msg.handle, PSA_SUCCESS);
                    break;
            }
        }
        else
        {
            val_print_sf("Unexpected signal value=0x%x. Entering into infinite loop\n",
                         signals);
            TEST_PANIC();
        }
    }
}

int32_t driver_test_psa_eoi_with_non_intr_signal(void)
{
    /* Setting boot.state before test check */
    if (val_driver_private_set_boot_flag_fn(BOOT_EXPECTED_NS))
    {
        val_print_sf("\tFailed to set boot flag before check\n", 0);
        return VAL_STATUS_ERROR;
    }

    /* psa_eoi should panic as signal is non-interrupt signal*/
    psa_eoi(PSA_DOORBELL);

    /* Control shouldn't have reached here */
    val_print_sf("\tCheck for psa_eoi(non_intr_sig) failed\n", 0);

    /* Resetting boot.state to catch unwanted reboot */
    if (val_driver_private_set_boot_flag_fn(BOOT_EXPECTED_BUT_FAILED))
    {
        val_print_sf("\tFailed to set boot flag after check\n", 0);
        return VAL_STATUS_ERROR;
    }

    return VAL_STATUS_SPM_FAILED;
}

int32_t driver_test_psa_eoi_with_unasserted_signal(void)
{
    /* Setting boot.state before test check */
    if (val_driver_private_set_boot_flag_fn(BOOT_EXPECTED_NS))
    {
        val_print_sf("\tFailed to set boot flag before check\n", 0);
        return VAL_STATUS_ERROR;
    }

    /* psa_eoi should panic as DRIVER_UART_INTR_SIG is unasserted */
    psa_eoi(DRIVER_UART_INTR_SIG);

    /* Control shouldn't have reached here */
    val_print_sf("\tCheck for psa_eoi(multiple_signals) failed\n", 0);

    /* Resetting boot.state to catch unwanted reboot */
    if (val_driver_private_set_boot_flag_fn(BOOT_EXPECTED_BUT_FAILED))
    {
        val_print_sf("\tFailed to set boot flag after check\n", 0);
        return VAL_STATUS_ERROR;
    }

    return VAL_STATUS_SPM_FAILED;
}

int32_t driver_test_psa_eoi_with_multiple_signals(void)
{
    psa_irq_enable(DRIVER_UART_INTR_SIG);
    /*
     * To test psa_eoi for multiple signals, one of signal should asserted first.
     * Otherwise, check can false pass with psa_eoi_with_unasserted_signal.
     * Assert interrupt signal assigned to driver partition
     */
    val_generate_interrupt();

    /* Wait for DRIVER_UART_INTR_SIG signal */
    if (psa_wait(DRIVER_UART_INTR_SIG, PSA_BLOCK) & DRIVER_UART_INTR_SIG)
    {
        /* Received DRIVER_UART_INTR_SIG signal, now process it */
        val_disable_interrupt();
    }
    else
    {
        /* didn't receive DRIVER_UART_INTR_SIG signal, however process it */
        val_disable_interrupt();
        val_print_sf("\tFailed to receive irq signal\n", 0);
        return VAL_STATUS_SPM_FAILED;
    }

    /* Setting boot.state before test check */
    if (val_driver_private_set_boot_flag_fn(BOOT_EXPECTED_NS))
    {
        val_print_sf("\tFailed to set boot flag before check\n", 0);
        return VAL_STATUS_ERROR;
    }

    /* psa_eoi should panic as irq_signal is provided with multiple signals */
    psa_eoi(DRIVER_UART_INTR_SIG|DRIVER_TEST_SIGNAL);

    /* Control shouldn't have reached here */
    val_print_sf("\tCheck for psa_eoi(multiple_signals) failed\n", 0);

    /* Resetting boot.state to catch unwanted reboot */
    if (val_driver_private_set_boot_flag_fn(BOOT_EXPECTED_BUT_FAILED))
    {
        val_print_sf("\tFailed to set boot flag after check\n", 0);
        return VAL_STATUS_ERROR;
    }

    return VAL_STATUS_SPM_FAILED;
}

int32_t driver_test_irq_routing(void)
{

    psa_signal_t signals = 0;
    psa_irq_enable(DRIVER_UART_INTR_SIG);

    /* Assert interrupt signal assigned to driver partition */
    val_generate_interrupt();

    /* Wait for DRIVER_UART_INTR_SIG signal */
    signals = psa_wait(DRIVER_UART_INTR_SIG, PSA_BLOCK);


    if (signals & DRIVER_UART_INTR_SIG)
    {
        /* Received DRIVER_UART_INTR_SIG signal, now process it */
        val_disable_interrupt();

        /* A signal remains active until it is processed by psa_eoi */
        if ((psa_wait(DRIVER_UART_INTR_SIG, PSA_BLOCK) & DRIVER_UART_INTR_SIG) == 0)
        {
            val_print_sf("\tIrq signal got de-activate before psa_eoi()\n", 0);
            return VAL_STATUS_SPM_FAILED;
        }

        /* Perform end of interrupt */
        psa_eoi(DRIVER_UART_INTR_SIG);

        /* Irq signal should be de-activated now */
        if (psa_wait(DRIVER_UART_INTR_SIG, PSA_POLL) & DRIVER_UART_INTR_SIG)
        {
            val_print_sf("\tIrq signal didn't get de-activate after psa_eoi()\n", 0);
            return VAL_STATUS_SPM_FAILED;
        }

        return VAL_STATUS_SUCCESS;
    }
    else
    {
        /* didn't receive DRIVER_UART_INTR_SIG signal, however process it */
        val_disable_interrupt();
        val_print_sf("\tFailed to receive irq signal, signals=0x%x\n", signals);
        return VAL_STATUS_SPM_FAILED;
    }
}

static int32_t process_call_request(psa_signal_t sig, psa_msg_t *msg)
{
    val_status_t res = VAL_STATUS_ERROR;
    psa_signal_t signals;

wait:
    signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);
    if (signals & sig)
    {
        if (psa_get(sig, msg) != PSA_SUCCESS)
        {
            goto wait;
        }

        if ((msg->type < PSA_IPC_CALL) || (msg->handle <= 0))
        {
            val_print_sf("\tpsa_get failed for request message\n", 0);
            res = VAL_STATUS_ERROR;
        }
        else
        {
            res = VAL_STATUS_SUCCESS;
        }
    }
    else
    {
        val_print_sf("\tpsa_wait returned with invalid signal value = 0x%x\n", signals);
        res = VAL_STATUS_ERROR;
    }
    return res;
}

void driver_test_isolation_psa_rot_data_rd(psa_msg_t *msg)
{
    uint32_t *addr = &g_psa_rot_data;

    /* Send PSA RoT data address - global variable */
    psa_write(msg->handle, 0, (void *) &addr, sizeof(addr_t));
    psa_reply(msg->handle, PSA_SUCCESS);
}

void driver_test_isolation_psa_rot_data_wr(psa_msg_t *msg)
{
    uint32_t *addr = &g_psa_rot_data;

    /* Send PSA RoT data address - global variable */
    psa_write(msg->handle, 0, (void *) &addr, sizeof(addr_t));

    /* Setting boot.state before test check */
    if (val_driver_private_set_boot_flag_fn(BOOT_EXPECTED_ON_SECOND_CHECK))
    {
       val_print_sf("\tFailed to set boot flag before check\n", 0);
       psa_reply(msg->handle, -2);
    }
    psa_reply(msg->handle, PSA_SUCCESS);

    /* Process second call request */
    if (VAL_ERROR(process_call_request(DRIVER_TEST_SIGNAL, msg)))
    {
        psa_reply(msg->handle, -2);
        return;
    }

    /* Resetting boot.state to catch unwanted reboot */
    if (val_driver_private_set_boot_flag_fn(BOOT_EXPECTED_BUT_FAILED))
    {
        val_print_sf("\tFailed to set boot flag after check\n", 0);
        psa_reply(msg->handle, -2);
        return;
    }

    /* Reached here means there could be write succeed or ignored */
    if (g_psa_rot_data == DATA_VALUE)
    {
        psa_reply(msg->handle, PSA_SUCCESS);
    }
    else
    {
        val_print_sf("\tExpected write to fault but it didn't\n", 0);
        psa_reply(msg->handle, -2);
    }
}

void driver_test_isolation_psa_rot_stack_rd(psa_msg_t *msg)
{
    uint32_t l_psa_rot_data = DATA_VALUE;
    uint32_t *addr = &l_psa_rot_data;

    /* Send PSA RoT stack address - local variable */
    psa_write(msg->handle, 0, (void *) &addr, sizeof(addr_t));
    psa_reply(msg->handle, PSA_SUCCESS);

    /* Dummy print to avoid compiler optimisation on local variable */
    val_print_sf("\tStack data 0x%x\n", (int)l_psa_rot_data);
}

void driver_test_isolation_psa_rot_stack_wr(psa_msg_t *msg)
{
    uint32_t l_psa_rot_data = DATA_VALUE;
    uint32_t *addr = &l_psa_rot_data;

    /* Send PSA RoT stack address - local variable */
    psa_write(msg->handle, 0, (void *) &addr, sizeof(addr_t));

    /* Setting boot.state before test check */
    if (val_driver_private_set_boot_flag_fn(BOOT_EXPECTED_ON_SECOND_CHECK))
    {
       val_print_sf("\tFailed to set boot flag before check\n", 0);
       psa_reply(msg->handle, -2);
    }
    psa_reply(msg->handle, PSA_SUCCESS);

    /* Process second call request */
    if (VAL_ERROR(process_call_request(DRIVER_TEST_SIGNAL, msg)))
    {
        psa_reply(msg->handle, -2);
        return;
    }

    /* Resetting boot.state to catch unwanted reboot */
    if (val_driver_private_set_boot_flag_fn(BOOT_EXPECTED_BUT_FAILED))
    {
        val_print_sf("\tFailed to set boot flag after check\n", 0);
        psa_reply(msg->handle, -2);
        return;
    }

    /* Reached here means there could be write succeed or ignored */
    if (l_psa_rot_data == DATA_VALUE)
    {
        psa_reply(msg->handle, PSA_SUCCESS);
    }
    else
    {
        val_print_sf("\tExpected write to fault but it didn't\n", 0);
        psa_reply(msg->handle, -2);
    }
}

void driver_test_isolation_psa_rot_heap_rd(psa_msg_t *msg)
{
#ifdef SP_HEAP_MEM_SUPP
    uint8_t         *buffer;

    buffer = (uint8_t *)malloc(sizeof(uint8_t) * BUFFER_SIZE);
    memset((uint8_t *)buffer, (uint8_t)DATA_VALUE, BUFFER_SIZE);

    /* Send PSA RoT heap address */
    psa_write(msg->handle, 0, (void *) &buffer, BUFFER_SIZE);
    psa_reply(msg->handle, PSA_SUCCESS);
    free(buffer);
#else
    (void)msg;
#endif
}

void driver_test_isolation_psa_rot_heap_wr(psa_msg_t *msg)
{
#ifdef SP_HEAP_MEM_SUPP
    uint8_t         *buffer;

    buffer = (uint8_t *)malloc(sizeof(uint8_t) * BUFFER_SIZE);
    memset((uint8_t *)buffer, (uint8_t)DATA_VALUE, BUFFER_SIZE);

    /* Send PSA RoT heap address */
    psa_write(msg->handle, 0, (void *) &buffer, BUFFER_SIZE);

    /* Setting boot.state before test check */
    if (val_driver_private_set_boot_flag_fn(BOOT_EXPECTED_ON_SECOND_CHECK))
    {
       val_print_sf("\tFailed to set boot flag before check\n", 0);
       psa_reply(msg->handle, -2);
    }
    psa_reply(msg->handle, PSA_SUCCESS);

    /* Process second call request */
    if (VAL_ERROR(process_call_request(DRIVER_TEST_SIGNAL, msg)))
    {
        psa_reply(msg->handle, -2);
        return;
    }

    /* Resetting boot.state to catch unwanted reboot */
    if (val_driver_private_set_boot_flag_fn(BOOT_EXPECTED_BUT_FAILED))
    {
        val_print_sf("\tFailed to set boot flag after check\n", 0);
        psa_reply(msg->handle, -2);
        return;
    }

    /* Reached here means there could be write succeed or ignored */
    if (buffer[0] == (uint8_t)DATA_VALUE)
    {
        psa_reply(msg->handle, PSA_SUCCESS);
    }
    else
    {
        val_print_sf("\tExpected write to fault but it didn't\n", 0);
        psa_reply(msg->handle, -2);
    }
    free(buffer);
#else
    (void)msg;
#endif
}

void driver_test_isolation_psa_rot_mmio_rd(psa_msg_t *msg)
{
    addr_t psa_rot_mmio_addr;

    if (VAL_ERROR(val_get_driver_mmio_addr(&psa_rot_mmio_addr)))
    {
        psa_reply(msg->handle, -2);
        return;
    }

    /* Send PSA RoT mmio address */
    memset((uint8_t *)&psa_rot_mmio_addr, (uint8_t)DATA_VALUE, sizeof(addr_t));
    psa_write(msg->handle, 0, (void *) &psa_rot_mmio_addr, sizeof(addr_t));
    psa_reply(msg->handle, PSA_SUCCESS);
}

void driver_test_isolation_psa_rot_mmio_wr(psa_msg_t *msg)
{
    addr_t psa_rot_mmio_addr;

    if (VAL_ERROR(val_get_driver_mmio_addr(&psa_rot_mmio_addr)))
    {
        psa_reply(msg->handle, -2);
        return;
    }

    /* Send PSA RoT mmio address */
    memset((uint8_t *)&psa_rot_mmio_addr, (uint8_t)DATA_VALUE, sizeof(addr_t));
    psa_write(msg->handle, 0, (void *) &psa_rot_mmio_addr, sizeof(addr_t));

    /* Setting boot.state before test check */
    if (val_driver_private_set_boot_flag_fn(BOOT_EXPECTED_ON_SECOND_CHECK))
    {
        val_print_sf("\tFailed to set boot flag before check\n", 0);
        psa_reply(msg->handle, -2);
    }
   psa_reply(msg->handle, PSA_SUCCESS);

    /* Process second call request */
    if (VAL_ERROR(process_call_request(DRIVER_TEST_SIGNAL, msg)))
    {
        psa_reply(msg->handle, -2);
        return;
    }

    /* Resetting boot.state to catch unwanted reboot */
    if (val_driver_private_set_boot_flag_fn(BOOT_EXPECTED_BUT_FAILED))
    {
        val_print_sf("\tFailed to set boot flag after check\n", 0);
        psa_reply(msg->handle, -2);
        return;
    }

    /* Reached here means there could be write succeed or ignored */
    if (*(uint32_t *)psa_rot_mmio_addr == (uint32_t)DATA_VALUE)
    {
        psa_reply(msg->handle, PSA_SUCCESS);
    }
    else
    {
        val_print_sf("\tExpected write to fault but it didn't\n", 0);
        psa_reply(msg->handle, -2);
    }
}
