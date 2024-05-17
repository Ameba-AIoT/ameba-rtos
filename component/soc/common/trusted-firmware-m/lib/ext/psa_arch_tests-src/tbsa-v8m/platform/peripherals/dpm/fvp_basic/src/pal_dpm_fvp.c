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

#include "pal_dpm_fvp.h"

/**
    @brief    - This function will set the debug permission based on the input arg
    @param    - index          : DPM index
                access_ns      : TRUE  - allow debug access only for non-secure address
                                 FALSE - allow debug access to both secure and non-secure addresses
    @return   - error status
**/
int pal_dpm_fvp_set_access_ns_only(uint32_t index, bool_t access_ns)
{
    if (access_ns)
        *(uint32_t *)DAUTHCTRL_REG = 0x1;
    else
        *(uint32_t *)DAUTHCTRL_REG = 0x0;

    return 0;

}
