/** @file
 * Copyright (c) 2021 Arm Limited or its affiliates. All rights reserved.
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

#include "val_framework.h"
#include "val_interfaces.h"
#include "val_peripherals.h"

/*VAL APIs to be used by test */
const val_api_t val_api = {
    .print                     = val_print,
    .set_status                = val_set_status,
    .get_status                = val_get_status,
    .test_init                 = val_test_init,
    .test_exit                 = val_test_exit,
    .err_check_set             = val_err_check_set,
};

