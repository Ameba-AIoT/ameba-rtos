/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * \file dma350_address_remap.c
 * \brief Dma350 library does the remap of TCM memory address aliases,
 * and the definition of those addresses are needed as the dma350 sees
 * these addresses differently as the core.
 */

#include "dma350_lib.h"

static const struct dma350_remap_range_t dma350_address_remap_list[] = {
    {.begin = 0x00000000, .end = 0x00007FFF, .offset = 0x0A000000},
    {.begin = 0x10000000, .end = 0x10007FFF, .offset = 0x0A000000},
    {.begin = 0x20000000, .end = 0x20007FFF, .offset = 0x04000000},
    {.begin = 0x30000000, .end = 0x30007FFF, .offset = 0x04000000}};
const struct dma350_remap_list_t dma350_address_remap = {
    .size = sizeof(dma350_address_remap_list) /
            sizeof(dma350_address_remap_list[0]),
    .map = dma350_address_remap_list};
