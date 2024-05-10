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

#include "val_target.h"
#include "target_database.h"

/* Use raw print for driver partition */
#ifdef DRIVER_PARTITION_INCLUDE
#define val_print(x, y, z)                        \
       do {                                       \
           if (x >= VERBOSE)                      \
              val_print_sf(y,z);                  \
       } while(0)
#else
__UNUSED STATIC_DECLARE val_status_t val_print
                        (print_verbosity_t verbosity, char *string, int32_t data);
#endif

/**
    @brief    - Returns the base address of target configuration block database.
    @param    - blob     : Populates the base address
    @return   - val_status_t
**/
STATIC_DECLARE val_status_t val_target_cfg_get_next(void **blob)
{
    val_status_t        status = VAL_STATUS_SUCCESS;
    target_cfg_hdr_t    *hdr;
    uint32_t            size;

    if (*blob == NULL)
    {
        *blob = (void *) &target_database;
        hdr = *blob;

        /* Sanity check signature and version here */
       if ((hdr->version != 1) || (hdr->size == 0))
       {
            val_print(PRINT_ERROR, "\tTarget config database Error. \n", 0);
            return VAL_STATUS_ERROR;
        }
        hdr++;
        *blob = hdr;  // skip the header. start with the first record.
        return status;
    }

    size = (((cfg_type_t *)*blob)->size) & 0xFFFFFF;
    if (size)
    {
        *blob = (void *)((uint8_t *)*blob + size);
        return VAL_STATUS_SUCCESS;
    }
    return VAL_STATUS_ERROR;
}

/**
    @brief    - This function checks for the given configuration ID with the block in
                target configuration database.
    @param    - cfg_id   : Configuration ID of a block
              - data     : Returns block base address
              - size     : Block size
    @return   - val_status_t
**/
STATIC_DECLARE val_status_t val_target_get_cfg_blob(cfg_id_t cfg_id, uint8_t **data, uint32_t *size)
{
    val_status_t    status;
    void            *config_blob = NULL;

    (void)size;

    val_print(PRINT_INFO, "\tInput id is %x \n", cfg_id);
    do
    {

        status = val_target_cfg_get_next(&config_blob);

        if (VAL_ERROR(status))
        {
            break;
        }

        if (((cfg_type_t *)config_blob)->cfg_id == cfg_id)
        {
            *data  = (uint8_t *)config_blob;
            status = VAL_STATUS_SUCCESS;
            break;
        }
        else if (((((cfg_type_t *)config_blob)->cfg_id & VAL_TEST_MAJOR_GROUP_MASK) == \
                (cfg_id & VAL_TEST_MAJOR_GROUP_MASK)) && \
                !(((cfg_type_t *)config_blob)->cfg_id & \
                (VAL_TEST_MINOR_GROUP_MASK | VAL_TEST_CFG_INSTANCE_MASK)))
        {
           config_blob = (void *)((uint8_t *)config_blob + sizeof(memory_hdr_t));
           if (((cfg_type_t *)config_blob)->cfg_id == cfg_id)
           {
               *data  = (uint8_t *)config_blob;
               status = VAL_STATUS_SUCCESS;
               break;
           }
        }
        else if (((cfg_type_t *)config_blob)->cfg_id == VAL_TEST_INVALID_CFG_ID)
        {
                status = VAL_STATUS_NOT_FOUND;
                break;
        }
    } while(1);

    return status;
}

/**

  @brief  - This function returns the data associated with a given
           config ID.
  @param  -  size  - if the input size is less than the data size to
           returned, the size is updated with the actual data size and
           error is returned.
  @return - data contains the information of type specific to a
            config id.
**/
STATIC_DECLARE val_status_t val_target_get_config(cfg_id_t cfg_id, uint8_t **data, uint32_t *size)
{
    val_status_t status;

    if ((cfg_id < (cfg_id_t)TARGET_MIN_CFG_ID) || (cfg_id > (cfg_id_t)TARGET_MAX_CFG_ID))
    {
        val_print(PRINT_ERROR, "\tInvalid Target data config ID = %x \n", cfg_id);
        return VAL_STATUS_INSUFFICIENT_SIZE;
    }

    status = val_target_get_cfg_blob(cfg_id, data, size);

    if (VAL_ERROR(status))
    {
        val_print(PRINT_ERROR, "\tGet Config failed with status = %x", status);
        val_print(PRINT_ERROR, " for cfg_id = %x\n", cfg_id);
        return status;
    }
    return VAL_STATUS_SUCCESS;
}

