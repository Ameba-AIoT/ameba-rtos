/** @file
 * Copyright (c) 2021, Arm Limited or its affiliates. All rights reserved.
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

#include <stdint.h>

int32_t val_entry(void);

/**
    @brief    - PSA C main function, used for generating tgt_dev_apis_stdc test binaries.
    @param    - argc    : the number of command line arguments.
                argv    : array containing command line arguments.
    @return   - error status
**/
int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return val_entry();
}
