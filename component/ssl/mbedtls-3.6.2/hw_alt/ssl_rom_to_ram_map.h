/**
 * *****************************************************************************
 * @file    rom_to_ram_map.h
 * @author  
 * @version V1.0.0
 * @date    2023-09-06
 * @brief   Use function pointer to map calloc/free function to ram 
 * *****************************************************************************
 * @attention
 * 
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 * 
 * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#ifndef SSL_ROM_TO_RAM_MAP_H
#define SSL_ROM_TO_RAM_MAP_H

typedef struct ssl_rom_to_ram_map {
	/* OS interface, printf will map to DiagPrintf */
	void *(*ssl_calloc)(unsigned int, unsigned int);
	void (*ssl_free)(void *);
	long unsigned int (*ssl_printf)(const char *, ...);
	int (*ssl_snprintf)(char *s, size_t n, const char *format, ...);
}ssl_rom_to_ram_map_t;

extern ssl_rom_to_ram_map_t ssl_function_map;

#endif
