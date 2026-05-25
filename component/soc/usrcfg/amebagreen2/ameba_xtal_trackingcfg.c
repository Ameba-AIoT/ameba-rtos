/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "basic_types.h"

/* S-curve: thermal_diff V.S. FreqError(ppm * 1000), y = C3 * x^3 + C2 * x^2 + C1 * x + C0,
   s_coef[4] is coefficient(C3, C2, C1, C0) * 1000 */
const s32 s_coef[4] = {198, 354, -294570, -1185900};

/* Cap sensitivity: cap Index V.S. FreqError(ppm),
	y1 = a1 * x^2 + b1 * x + c1,   0 <= x < 40;
	y2 = a2 * x^2 + b2 * x + c2,  40 <= x < 64;
	y3 = a3 * x^2 + b3 * x + c3,  64 <= x < 105;
	y4 = a4 * x^2 + b4 * x + c4, 105 <= x < 127;
    seg_slope[4][3] is coefficient(a, b, c) * 10000 */
const s32 seg_slope[4][3] = {
	{0,     4677,  -638110},
	{0,     8194,  -774810},
	{264,  -26683,  385370},
	{2925, -601850, 31501000},
};
