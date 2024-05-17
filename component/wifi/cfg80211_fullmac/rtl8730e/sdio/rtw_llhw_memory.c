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
	void *vaddr = kzalloc(size, GFP_KERNEL);

	if (paddr) {
		*paddr = (dma_addr_t) vaddr;
	}

	return vaddr;
}

void rtw_mfree(size_t size, void *vaddr, dma_addr_t paddr)
{
	kfree(vaddr);
}

