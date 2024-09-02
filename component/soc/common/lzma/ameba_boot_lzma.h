/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifndef _AMEBA_BOOT_LZMA_H_
#define _AMEBA_BOOT_LZMA_H_

void bootLzma_buffer_set(u8 *bd_ram_addr);
u8 bootLzma_main_function(u32 read_addr, u32 write_addr_st, u32 write_addr_end);

#endif
