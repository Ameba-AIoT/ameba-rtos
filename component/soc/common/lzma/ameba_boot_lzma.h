/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_BOOT_LZMA_H_
#define _AMEBA_BOOT_LZMA_H_

void bootLzma_buffer_set(u8 *bd_ram_addr);
u8 bootLzma_main_function(u32 read_addr, u32 write_addr_st, u32 write_addr_end);

#endif
