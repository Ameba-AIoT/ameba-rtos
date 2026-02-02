/**
 * *****************************************************************************
 * @file    ssl_rom_to_ram_map.c
 * @author
 * @version V1.0.0
 * @date    2023-09-06
 * @brief   rom mbedtls will use ssl_function_map structure function pointer to map calloc/free
 * *****************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */
#include "basic_types.h"
#include <section_config.h>
#include "ssl_rom_to_ram_map.h"

/* only compile it in rom */
#ifdef CONFIG_BUILD_ROM
HAL_ROM_BSS_SECTION
ssl_rom_to_ram_map_t ssl_function_map;
#else
ssl_rom_to_ram_map_t ssl_function_map;
#endif

extern void _memset(void *m, int c, size_t n);

void mbedtls_platform_zeroize(void *buf, size_t len)
{
    _memset(buf, 0, len);
}

void reverse_bytes(uint8_t *buf, size_t len) {
    for (size_t i = 0; i < len/2; i++) {
        uint8_t tmp = buf[i];
        buf[i] = buf[len-1-i];
        buf[len-1-i] = tmp;
    }
}


