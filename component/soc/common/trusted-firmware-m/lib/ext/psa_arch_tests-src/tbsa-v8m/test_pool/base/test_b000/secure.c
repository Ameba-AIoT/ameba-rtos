/** @file
 * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
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

/**
  Publish these functions to the external world as associated to this test ID
**/
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_BASE_BASE, 0),
                  CREATE_TEST_TITLE("CPUID Register access from secure and non-secure world"),
                  CREATE_REF_TAG("No reference tag"),
                  entry_hook,
                  test_payload,
                  exit_hook);

void
entry_hook(tbsa_val_api_t *val)
{
    tbsa_test_init_t init = {
                             .bss_start      = &__tbsa_test_bss_start__,
                             .bss_end        = &__tbsa_test_bss_end__
                            };

    val->test_initialize(&init);

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void
test_payload(tbsa_val_api_t *val)
{
    uint32_t id;
    cpuid_t  cpuid;
    cpuid_t  cpuid_ns;

    val->mem_reg_read(CPUID, &id);
    cpuid.id = id;
    val->mem_reg_read(CPUID_NS, &id);
    cpuid_ns.id = id;

    val->print(PRINT_DEBUG, "-----From Secure World-----\n",    0);
    val->print(PRINT_DEBUG, "Secure view of CPUID: 0x%x\n",     cpuid.id);
    val->print(PRINT_DEBUG, "Implementer\t: 0x%x\n",            cpuid.id_s.imp);
    val->print(PRINT_DEBUG, "Variant\t\t: 0x%x\n",              cpuid.id_s.var);
    val->print(PRINT_DEBUG, "Architecture\t: 0x%x\n",           cpuid.id_s.arc);
    val->print(PRINT_DEBUG, "PartNo\t\t: 0x%x\n",               cpuid.id_s.par);
    val->print(PRINT_DEBUG, "Revision\t: 0x%x\n",               cpuid.id_s.rev);
    val->print(PRINT_DEBUG, "\n",                               0);
    val->print(PRINT_DEBUG, "Non-Secure view of CPUID: 0x%x\n", cpuid_ns.id);
    val->print(PRINT_DEBUG, "Implementer\t: 0x%x\n",            cpuid_ns.id_s.imp);
    val->print(PRINT_DEBUG, "Variant\t\t: 0x%x\n",              cpuid_ns.id_s.var);
    val->print(PRINT_DEBUG, "Architecture\t: 0x%x\n",           cpuid_ns.id_s.arc);
    val->print(PRINT_DEBUG, "PartNo\t\t: 0x%x\n",               cpuid_ns.id_s.par);
    val->print(PRINT_DEBUG, "Revision\t: 0x%x\n",               cpuid_ns.id_s.rev);
    val->print(PRINT_DEBUG, "\n",                               0);

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void
exit_hook(tbsa_val_api_t *val)
{
}
