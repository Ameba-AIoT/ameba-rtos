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

/*
 * to convert physic address from fw to virtual address in linux system.
 */
__iomem void *llhw_ipc_fw_phy_to_virt(phys_addr_t p)
{
	struct axi_data *axi = paxi_data_global;
	struct resource *res = axi->fw_psram_res;

	/* Route SRAM-window phys first; DDR boards keep fw_psram only (RSWLANDIOT-16775) */
	if (axi->fw_sram_res &&
		p >= axi->fw_sram_res->start && p <= axi->fw_sram_res->end) {
		return (__iomem void *)((p - axi->fw_sram_res->start) + axi->fw_sram_start);
	}

	if (p < res->start || p > res->end) {
		WARN_ONCE(1, "whc: fw ptr 0x%llx out of PSRAM/SRAM window\n", (unsigned long long)p);
		return NULL;
	}

	return (__iomem void *)((p - res->start) + axi->fw_psram_start);
}

void *rtw_malloc(size_t size, dma_addr_t *paddr)
{
	return dma_alloc_coherent(global_idev.pwhc_dev, size, paddr, GFP_KERNEL);
}

void rtw_mfree(size_t size, void *vaddr, dma_addr_t paddr)
{
	dma_free_coherent(global_idev.pwhc_dev, size, vaddr, paddr);
}