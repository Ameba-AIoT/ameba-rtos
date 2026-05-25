/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "basic_types.h"

/* S-curve: thermal_diff V.S. FreqError(ppm * 1000), y = C3 * x^3 + C2 * x^2 + C1 * x + C0,
   s_coef[4] is coefficient(C3, C2, C1, C0) * 1000 */
const s32 s_coef[4] = {205, 287, -286118, 257903};

/* Cap sensitivity: cap Index V.S. FreqError(ppm),
	y1 = a1 * x + b1,   0 <= x < 40;
	y2 = a2 * x + b2,  40 <= x < 75;
	y3 = a3 * x + b3,  75 <= x < 105;
	y4 = a4 * x + b4, 105 <= x < 127;
    seg_slope[4] is coefficient a * 10000 */
const s32 seg_slope[4] = {4423, 6798, 11220, 19738};
