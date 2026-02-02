// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#ifndef __RTW_LLHW_MEM_H__
#define __RTW_LLHW_MEM_H__

struct axi_data {
	struct platform_device	*pdev;

	/* AXI MEM map */
	struct resource *wifi_reg_res; /* shared mem resource in dts */
	unsigned long wifi_reg_end; /* shared mem end */
	unsigned long wifi_reg_start; /* shared mem start */

	/* np MEM map */
	struct resource *fw_mem_res; /* firmware mem resource in dts */
	unsigned long fw_mem_end; /* firmware mem end */
	unsigned long fw_mem_start; /* firmware mem start */
};

extern struct axi_data *paxi_data_global;

/*
 * to convert physic address from fw to virtual address in linux system.
 */
static inline __iomem void *llhw_ipc_fw_phy_to_virt(phys_addr_t p)
{
	struct axi_data *axi = paxi_data_global;
	struct resource *res = axi->fw_mem_res;

	return (__iomem void *)((p - res->start) + axi->fw_mem_start);
}

/*
 * to read wifi registers in linux system.
 */
static inline u32 llhw_ipc_wifi_reg_read32(int reg)
{
	return readl((u8 *)paxi_data_global->wifi_reg_start + reg);
}

#endif /* __RTW_LLHW_MEM_H__ */
