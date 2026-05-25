/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "basic_types.h"

/* S-curve coefficient * 1000, y = C3 * x^3 + C2 * x^2 + C1 * x + C0 */
const s32 s_coef[4] = {217, 1922, -337400, 348714};

/* cap Index V.S. FreqError(ppm) coefficient *1000, y = a * x^2 + b * x + c */
const s32 seg_slope[4][3] = {
	{0,     5392,  -624780},
	{0,     9280,  -775650},
	{270,  -26026,  381760},
	{839, -135730, 5656400},
};
