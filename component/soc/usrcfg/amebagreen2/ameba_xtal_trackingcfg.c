/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "wifi_api.h"

/* S-curve coefficient * 1000, y = C3 * x^3 + C2 * x^2 + C1 * x + C0 */
const s32 s_coef[4] = {198, 354, -294570, -1185900};

/* cap Index V.S. FreqError(ppm*1000) coefficient, y = a * x^2 + b * x + c */
const s32 seg_slope[4][3] = {
	{0,     4677,  -638110},
	{0,     8194,  -774810},
	{264,  -26683,  385370},
	{2925, -601850, 31501000},
};