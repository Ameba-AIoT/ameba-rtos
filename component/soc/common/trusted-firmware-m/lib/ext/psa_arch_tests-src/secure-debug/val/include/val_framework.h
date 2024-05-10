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


#ifndef _VAL_INFRA_H_
#define _VAL_INFRA_H_

#include "val.h"

/* prototypes */
uint32_t     val_report_status(void);
val_status_t val_set_status(uint32_t status);
uint32_t     val_get_status(void);
val_status_t val_err_check_set(uint32_t checkpoint, val_status_t status);
void         val_test_init(uint32_t test_num, char8_t *desc);
void         val_test_exit(void);

#endif
