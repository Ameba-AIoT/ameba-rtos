/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "basic_types.h"

/* S-curve: thermal_diff V.S. FreqError(ppm * 1000), y = C3 * x^3 + C2 * x^2 + C1 * x + C0,
   s_coef[4] is coefficient(C3, C2, C1, C0) * 1000 */
const s32 s_coef[4] = {246, 2252, -376372, 64619};

/* Cap sensitivity: cap Index V.S. FreqError(ppm),
	y1 = a1 * x + b1,   0 <= x < 40;
	y2 = a2 * x + b2,  40 <= x < 80;
	y3 = a3 * x + b3,  80 <= x <= 127;
    seg_slope[3] is coefficient(a1, a2, a3) * 10000 */
const s32 seg_slope[3] = {5000, 8000, 15000};
