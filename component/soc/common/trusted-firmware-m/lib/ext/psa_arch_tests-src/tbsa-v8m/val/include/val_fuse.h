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

#ifndef _VAL_FUSE_H_
#define _VAL_FUSE_H_

#include "val_common.h"
#include "val_target.h"
#include "val_infra.h"

typedef enum {
  FUSE_READ=1,
  FUSE_WRITE,
} fuse_ops_t;

tbsa_status_t val_fuse_ops(fuse_ops_t fuse_ops, addr_t addr, uint32_t *data, uint32_t size);
tbsa_status_t val_get_fuse_info(fuse_desc_t **fuse_info_desc, fuse_type_t fuse_type, uint32_t instance);
tbsa_status_t val_fuse_get_lcs(uint32_t *pLcs);

tbsa_status_t val_firmware_version_update(uint32_t instance, firmware_version_type_t firmware_version_type, uint32_t fw_ver_cnt);
uint32_t      val_firmware_version_read  (uint32_t instance, firmware_version_type_t firmware_version_type);

#endif /* _VAL_FUSE_H_ */
