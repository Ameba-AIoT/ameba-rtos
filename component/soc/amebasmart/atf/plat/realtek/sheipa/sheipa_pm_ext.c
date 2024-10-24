/*
 * Copyright (c) 2015, Realtek Semiconductor Corp. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/debug.h>
#include <platform_def.h>


unsigned int plat_sheipa_pm_get_ns_dram0_base(void)
{
	return NS_DRAM0_BASE;
}

unsigned int plat_sheipa_pm_get_ns_dram0_size(void)
{
	return NS_DRAM0_SIZE;
}

unsigned int plat_sheipa_pm_get_trusted_mailbox_base(void)
{
	return PLAT_SHEIPA_TRUSTED_MAILBOX_BASE;
}

unsigned int plat_sheipa_pm_get_hold_base(void)
{
	return PLAT_SHEIPA_HOLD_BASE;
}

unsigned int plat_sheipa_pm_get_warm_boot_base(void)
{
	return PLAT_SHEIPA_WARM_BOOT_BASE;
}

