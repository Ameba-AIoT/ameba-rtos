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


#ifndef _TBSA_H_
#define _TBSA_H_

#include "val_infra.h"

#define TBSA_ACS_MAJOR_VER    0
#define TBSA_ACS_MINOR_VER    9

uint32_t __tbsa_text_start__;
uint32_t __tbsa_text_end__;
uint32_t __tbsa_bss_start__;
uint32_t __tbsa_bss_end__;
uint32_t __tbsa_data_src_start__;
uint32_t __tbsa_data_start__;
uint32_t __tbsa_data_end__;
uint32_t __tbsa_nsc_entry_points_start__;
uint32_t __tbsa_nsc_entry_points_end__;
uint32_t __tbsa_ns_data_start__;
uint32_t __tbsa_ns_data_end__;
uint32_t __tbsa_ns_text_start__;
uint32_t __tbsa_ns_text_end__;
uint32_t __tbsa_test_s_start__;
uint32_t __tbsa_test_s_end__;
uint32_t __tbsa_test_ns_start__;
uint32_t __tbsa_test_ns_end__;

#endif /* _TBSA_H_ */
