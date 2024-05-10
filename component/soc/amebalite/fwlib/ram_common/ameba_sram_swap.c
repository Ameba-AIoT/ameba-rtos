/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

NON_DRAM_TEXT_SECTION
void SOCPS_memswap(void *dst0, void *src0, u32 len0)
{
	char *dst = (char *) dst0;
	char *src = (char *) src0;
	u32 len = len0;

	char temp;

	while (len--) {
		temp = *dst;
		*dst = *src;
		*src = temp;
		dst++;
		src++;
	}
	DCache_Clean((u32)dst0, len0);
	DCache_Clean((u32)src0, len0);
}

NON_DRAM_TEXT_SECTION
void SOCPS_Kr4_StartTbl_Backup(void)
{
#ifndef CONFIG_SOC_SOLO
	SOCPS_memswap(__kr4_boot_entry_addr__, __kr4_start_table_backup_addr__, sizeof(RAM_FUNCTION_START_TABLE));
	/* Kr4 start table bacukp status done */
	RRAM_DEV->kr4_BackupTbl_Done = 1;
#endif
}

NON_DRAM_TEXT_SECTION
void SOCPS_Kr4_StartTbl_Restore(void)
{
#ifndef CONFIG_SOC_SOLO
	SOCPS_memswap(__kr4_boot_entry_addr__, __kr4_start_table_backup_addr__, sizeof(RAM_FUNCTION_START_TABLE));
	/* Kr4 start table bacukp status reset */
	RRAM_DEV->kr4_BackupTbl_Done = 0;
#endif
}