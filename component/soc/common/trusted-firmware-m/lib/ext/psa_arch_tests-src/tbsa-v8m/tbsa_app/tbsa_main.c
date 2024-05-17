/** @file
 * Copyright (c) 2018-2020, Arm Limited or its affiliates. All rights reserved.
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
#include "tbsa.h"

/**
    @brief    - TBSA C main function, zero out BSS, does VAL init
    @param    - None
    @return   - void
**/
void tbsa_main (void)
{
    tbsa_status_t status;
    test_id_t test_id_prev;

    /* zero bss section */
    memset(&__tbsa_bss_start__, 0, TBSA_REGION_SIZE(&__tbsa_bss_start__, &__tbsa_bss_end__));
    /* Initialize data section */
    memcpy(&__tbsa_data_start__, &__tbsa_data_src_start__, TBSA_REGION_SIZE(&__tbsa_data_start__, &__tbsa_data_end__));

    status = val_infra_init(&test_id_prev);
    if (TBSA_ERROR(status))
    {
        if (status == TBSA_STATUS_UART_INIT_ERROR)
            goto exit;
        val_print(PRINT_ERROR, "\nVal Infra Init failed with error = %x ", status);
        goto exit;
    }
    val_print(PRINT_ALWAYS, "\n\rTBSA Architecture Test Suite",0);
    val_print(PRINT_ALWAYS, "\n\rVersion :\t%d.", TBSA_ACS_MAJOR_VER);
    val_print(PRINT_ALWAYS, "%d", TBSA_ACS_MINOR_VER);

    val_print(PRINT_ALWAYS, "\n", 0);
    val_print(PRINT_ALWAYS, "\n\r+----------------------------------------------------------------+", 0);
    val_print(PRINT_ALWAYS, "\n\r|                       TBSA Memory Layout                       |", 0);
    val_print(PRINT_ALWAYS, "\n\r+----------------------------------------------------------------+", 0);
    val_print(PRINT_ALWAYS, "\n\r| Main App S_Text      : start - 0x%X\t", (uint32_t)&__tbsa_text_start__);
    val_print(PRINT_ALWAYS, "end - 0x%X |", (uint32_t)&__tbsa_text_end__);
    val_print(PRINT_ALWAYS, "\n\r| Main App S_Data      : start - 0x%X\t", (uint32_t)&__tbsa_data_start__);
    val_print(PRINT_ALWAYS, "end - 0x%X |", (uint32_t)&__tbsa_bss_end__);
    val_print(PRINT_ALWAYS, "\n\r| Main App NSC_Text    : start - 0x%X\t", (uint32_t)&__tbsa_nsc_entry_points_start__);
    val_print(PRINT_ALWAYS, "end - 0x%X |", (uint32_t)&__tbsa_nsc_entry_points_end__);
    val_print(PRINT_ALWAYS, "\n\r| Main App NS_Data     : start - 0x%X\t", (uint32_t)&__tbsa_ns_data_start__);
    val_print(PRINT_ALWAYS, "end - 0x%X |", (uint32_t)&__tbsa_ns_data_end__);
    val_print(PRINT_ALWAYS, "\n\r| Main App NS_Text     : start - 0x%X\t", (uint32_t)&__tbsa_ns_text_start__);
    val_print(PRINT_ALWAYS, "end - 0x%X |", (uint32_t)&__tbsa_ns_text_end__);
    val_print(PRINT_ALWAYS, "\n\r| Secure Test          : start - 0x%X\t", (uint32_t)&__tbsa_test_s_start__);
    val_print(PRINT_ALWAYS, "end - 0x%X |", (uint32_t)&__tbsa_test_s_end__);
    val_print(PRINT_ALWAYS, "\n\r| Non-secure Test      : start - 0x%X\t", (uint32_t)&__tbsa_test_ns_start__);
    val_print(PRINT_ALWAYS, "end - 0x%X |", (uint32_t)&__tbsa_test_ns_end__);
    val_print(PRINT_ALWAYS, "\n\r+----------------------------------------------------------------+", 0);


    val_print(PRINT_ALWAYS, "\n\n\r+++ Starting tests +++", 0);
    /* Call the dispatcher routine*/
    tbsa_dispatcher(test_id_prev);
    val_print(PRINT_ALWAYS, "\n\n\rEntering standby\n", 0);

exit:
    while(1) {
        __asm volatile("WFI");
    }
}
