// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <rtw_cfg80211_fullmac.h>

void *rtw_malloc(size_t size, dma_addr_t *paddr)
{
	return dmam_alloc_coherent(global_idev.fullmac_dev, size, paddr, GFP_KERNEL);
}

void rtw_mfree(size_t size, void *vaddr, dma_addr_t paddr)
{
	dma_free_coherent(global_idev.fullmac_dev, size, vaddr, paddr);
}

