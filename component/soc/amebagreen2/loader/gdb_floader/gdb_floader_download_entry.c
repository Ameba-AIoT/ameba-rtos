/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba.h"

void Gdb_Floader_Program(void);

IMAGE1_ENTRY_SECTION
RAM_FUNCTION_START_TABLE RamStartTable = {
	.RamStartFun  = Gdb_Floader_Program,
	.RamWakeupFun = Gdb_Floader_Program,
	.RamPatchFun0 = Gdb_Floader_Program,
	.RamPatchFun1 = Gdb_Floader_Program,
	.RamPatchFun2 = Gdb_Floader_Program
};
