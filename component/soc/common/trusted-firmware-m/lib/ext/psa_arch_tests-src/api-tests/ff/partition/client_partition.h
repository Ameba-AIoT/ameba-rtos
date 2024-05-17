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

#ifndef _CLIENT_PART_H_
#define _CLIENT_PART_H_

#include "val_client_defs.h"
#include "val_partition_common.h"

typedef client_test_t (*client_test_list_t);

#include "client_tests_list_declare.inc"

client_test_list_t client_ipc_test_list[] = {
    NULL,
#include "client_tests_list.inc"
    NULL,
};

#endif
