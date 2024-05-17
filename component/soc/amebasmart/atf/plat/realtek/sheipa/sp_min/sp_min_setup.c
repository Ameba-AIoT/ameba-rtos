/*
 * Copyright (c) 2019, Realtek Semiconductor Corp. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <string.h>

#include <platform_def.h>

#include <arch_helpers.h>
#include <common/bl_common.h>
#include <common/debug.h>
#include <drivers/console.h>

#include "../sheipa_private.h"
#include "basic_types.h"
#include "hal_platform.h"
#include "sysreg_hsys.h"
#include "sysreg_lsys.h"

#if RESET_TO_SP_MIN
#error sheipa does not support RESET_TO_SP_MIN
#endif

static entry_point_info_t bl33_image_ep_info;

/*******************************************************************************
 * Return a pointer to the 'entry_point_info' structure of the next image for
 * the security state specified. BL33 corresponds to the non-secure image type
 * while BL32 corresponds to the secure image type. A NULL pointer is returned
 * if the image does not exist.
 ******************************************************************************/
entry_point_info_t *sp_min_plat_get_bl33_ep_info(void)
{
	entry_point_info_t *next_image_info = &bl33_image_ep_info;

	/*
	 * None of the images on the ARM development platforms can have 0x0
	 * as the entrypoint
	 */
	if (next_image_info->pc)
		return next_image_info;
	else
		return NULL;
}

void sp_min_early_platform_setup2(u_register_t arg0, u_register_t arg1,
		u_register_t arg2, u_register_t arg3)
{
	bl_params_t *params_from_bl2 = (bl_params_t *)arg0;

	/* Initialize the console to provide early debug support */
	sheipa_console_init();

	VERBOSE("[sp_min] sheipa console init\n");
	/*
	 * Check params passed from BL2
	 */
	assert(params_from_bl2);
	assert(params_from_bl2->h.type == PARAM_BL_PARAMS);
	assert(params_from_bl2->h.version >= VERSION_2);

	bl_params_node_t *bl_params = params_from_bl2->head;

	/*
	 * Copy BL33 entry point information from BL2's address space.
	 */
	while (bl_params) {
		if (bl_params->image_id == BL33_IMAGE_ID)
			bl33_image_ep_info = *bl_params->ep_info;

		bl_params = bl_params->next_params_info;
	}

	if (!bl33_image_ep_info.pc)
		panic();
}

void sp_min_plat_arch_setup(void)
{
	sheipa_configure_mmu_svc_mon(BL32_BASE, BL32_END - BL32_BASE,
				  BL_CODE_BASE, BL_CODE_END,
				  BL_RO_DATA_BASE, BL_RO_DATA_END
#if USE_COHERENT_MEM
				  , BL_COHERENT_RAM_BASE, BL_COHERENT_RAM_END
#endif
				  );

}

void sp_min_platform_setup(void)
{
	/* Initialize the gic cpu and distributor interfaces */
	plat_sheipa_gic_driver_init();
	plat_sheipa_gic_init();
}

unsigned int plat_get_syscnt_freq2(void)
{
	return SYS_COUNTER_FREQ_IN_TICKS;
}

void sp_min_plat_fiq_handler(uint32_t id)
{
	VERBOSE("[sp_min] interrupt #%d\n", id);
}
