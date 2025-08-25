/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba.h"

void RtlFlashProgram(void);

IMAGE1_ENTRY_SECTION
RAM_FUNCTION_START_TABLE RamStartTable = {
	.RamStartFun = RtlFlashProgram,
	.RamWakeupFun = RtlFlashProgram,
	.RamPatchFun0 = RtlFlashProgram,
	.RamPatchFun1 = RtlFlashProgram,
	.RamPatchFun2 = RtlFlashProgram
};
