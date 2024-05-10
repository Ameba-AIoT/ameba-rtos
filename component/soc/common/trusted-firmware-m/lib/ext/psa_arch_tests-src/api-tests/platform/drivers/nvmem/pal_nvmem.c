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

#include "pal_nvmem.h"

/**
    @brief    - Writes into given non-volatile address.
    @param    - base    : Base address of non-volatile memory
                offset  : Offset
                buffer  : Pointer to source address
                size    : Number of bytes
    @return   - 1/0
**/
int nvmem_write(addr_t base, uint32_t offset, void *buffer, int size)
{
    int b_cnt;

    if (buffer == NULL) {
        return 0;
    }

    for (b_cnt = 0; b_cnt < size; b_cnt++)
    {
        /* If flash interface just emulated over SRAM, use simple memory write */
        *(uint8_t *)(base + offset + b_cnt) = *((uint8_t *)buffer + b_cnt);
    }

    return 1;
}

/**
    @brief    - Reads from given non-volatile address.
    @param    - base    : Base address of non-volatile memory
                offset  : Offset
                buffer  : Pointer to source address
                size    : Number of bytes
    @return   - 1/0
**/
int nvmem_read(addr_t base, uint32_t offset, void *buffer, int size)
{
    int b_cnt;

    if (buffer == NULL) {
        return 0;
    }

    for (b_cnt = 0; b_cnt < size; b_cnt++)
    {
        /* If flash interface just emulated over SRAM, use simple memory read */
        *((uint8_t *)buffer + b_cnt) = *(uint8_t *)(base + offset + b_cnt);
    }

    return 1;
}

