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

#include "val_fuse.h"
#include "pal_interfaces.h"

/**
    @brief    - Read the value of given fuse address
    @param    - addr     : Address of the fuse
              - data     : Buffer to store the data
              - size     : Number of words to be read
    @return   - error status
**/
tbsa_status_t val_fuse_read(addr_t addr, uint32_t *data, uint32_t size)
{
   return pal_fuse_read(addr, data, size);
}

/**
    @brief    - Write the value in given fuse address
    @param    - addr     : Address of the fuse
              - data     : Data to be written
    @return   - error status
**/
tbsa_status_t val_fuse_write(addr_t addr, uint32_t *data, uint32_t size)
{
   return pal_fuse_write(addr, data, size);
}

/**
    @brief    - This function will choose the operation to be performed on a fuse
    @param    - fuse_ops : fuse read/write
                addr     : address of the fuse on which the operation is to be performed
                data     : buffer, which can be used to get the data in case of fuse reads
                           and write the data in case of writes.
                size     : size of the fuse
    @return   - error status
**/
tbsa_status_t val_fuse_ops(fuse_ops_t fuse_ops, addr_t addr, uint32_t *data, uint32_t size)
{
    switch (fuse_ops) {
        case FUSE_READ:
            return val_fuse_read(addr, data, size);
        case FUSE_WRITE:
            return val_fuse_write(addr, data, size);
        default:
            return TBSA_STATUS_INVALID_ARGS;
    }
}

/**
    @brief    - This function will fuse information from targetConfig.
    @param    - fuse_info_desc : the structure which will have the info
                fuse_type      : type of the fuse
                instance       : instance of the fuse
    @return   - error status
**/
tbsa_status_t val_get_fuse_info(fuse_desc_t **fuse_info_desc, fuse_type_t fuse_type, uint32_t instance)
{
    fuse_desc_t    *fuse_desc;
    fuse_hdr_t     *fuse_hdr;
    tbsa_status_t  status;
    uint32_t       index = 0;
    bool_t         fuse_present = 0;

    status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_FUSE, 0, 0),
                          (uint8_t **)&fuse_hdr, (uint32_t *)sizeof(fuse_hdr_t));
    if (status != TBSA_STATUS_SUCCESS)
        return status;

    while (index < fuse_hdr->num) {
        status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_FUSE, FUSE_FUSE, index),
                              (uint8_t **)&fuse_desc, (uint32_t *)sizeof(fuse_desc_t));
        if (status != TBSA_STATUS_SUCCESS)
            return status;

        if ((fuse_desc->type & fuse_type) != fuse_type) {
            index++;
            continue;
        }

        fuse_present = 1;
        break;
    }

    if (fuse_present) {
        *fuse_info_desc = fuse_desc;
        return TBSA_STATUS_SUCCESS;
    }

    return TBSA_STATUS_NOT_FOUND;
}

/**
    @brief    - This function will read the LCS register
    @param    - Buffer to store the LCS value
    @return   - error status
**/
tbsa_status_t val_fuse_get_lcs(uint32_t *pLcs)
{
    return pal_fuse_get_lcs(pLcs);
}

/*
    @brief      - Updates the firmware version for given instance(trusted or non-trusted)
    @param      - instance   : Firmware instance
                - fw_ver_cnt : Firmware version counter
    @return     - tbsa_status_t
*/
tbsa_status_t val_firmware_version_update(uint32_t instance, firmware_version_type_t firmware_version_type, uint32_t fw_ver_cnt)
{
    return pal_firmware_version_update(instance, firmware_version_type, fw_ver_cnt);
}

/*
    @brief    - Reads the firmware version for given instance(trusted or non-trusted)
    @param    - instance : Instance number
    @return   - firmware version counter
*/
uint32_t val_firmware_version_read(uint32_t instance, firmware_version_type_t firmware_version_type)
{
    return pal_firmware_version_read(instance, firmware_version_type);
}
