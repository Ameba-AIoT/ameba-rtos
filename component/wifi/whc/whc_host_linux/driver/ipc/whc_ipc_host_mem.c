// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <whc_host_linux.h>

struct axi_data *paxi_data_global;

void *rtw_malloc(size_t size, dma_addr_t *paddr)
{
	return dma_alloc_coherent(global_idev.pwhc_dev, size, paddr, GFP_KERNEL);
}

void rtw_mfree(size_t size, void *vaddr, dma_addr_t paddr)
{
	dma_free_coherent(global_idev.pwhc_dev, size, vaddr, paddr);
}