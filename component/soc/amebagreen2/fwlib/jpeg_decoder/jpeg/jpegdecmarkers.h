/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*------------------------------------------------------------------------------

    Table of contents

    1. Include headers
    2. Module defines
    3. Data types
    4. Function prototypes

------------------------------------------------------------------------------*/

#ifndef JPEGDECMARKERS_H
#define JPEGDECMARKERS_H

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    2. Module defines
------------------------------------------------------------------------------*/

/* JPEG markers, table B.1 page 32 */

enum {
	SOF0 = 0xC0,
	SOF1 = 0xC1,
	SOF2 = 0xC2,
	SOF3 = 0xC3,
	SOF5 = 0xC5,
	SOF6 = 0xC6,
	SOF7 = 0xC7,
	SOF9 = 0xC8,
	SOF10 = 0xCA,
	SOF11 = 0xCB,
	SOF13 = 0xCD,
	SOF14 = 0xCE,
	SOF15 = 0xCF,
	JPG = 0xC8,
	DHT = 0xC4,
	DAC = 0xCC,
	SOI = 0xD8,
	EOI = 0xD9,
	SOS = 0xDA,
	DQT = 0xDB,
	DNL = 0xDC,
	DRI = 0xDD,
	DHP = 0xDE,
	EXP = 0xDF,
	APP0 = 0xE0,
	APP1 = 0xE1,
	APP2 = 0xE2,
	APP3 = 0xE3,
	APP4 = 0xE4,
	APP5 = 0xE5,
	APP6 = 0xE6,
	APP7 = 0xE7,
	APP8 = 0xE8,
	APP9 = 0xE9,
	APP10 = 0xEA,
	APP11 = 0xEB,
	APP12 = 0xEC,
	APP13 = 0xED,
	APP14 = 0xEE,
	APP15 = 0xEF,
	JPG0 = 0xF0,
	JPG1 = 0xF1,
	JPG2 = 0xF2,
	JPG3 = 0xF3,
	JPG4 = 0xF4,
	JPG5 = 0xF5,
	JPG6 = 0xF6,
	JPG7 = 0xF7,
	JPG8 = 0xF8,
	JPG9 = 0xF9,
	JPG10 = 0xFA,
	JPG11 = 0xFB,
	JPG12 = 0xFC,
	JPG13 = 0xFD,
	COM = 0xFE,
	TEM = 0x01,
	RST0 = 0xD0,
	RST1 = 0xD1,
	RST2 = 0xD2,
	RST3 = 0xD3,
	RST4 = 0xD4,
	RST5 = 0xD5,
	RST6 = 0xD6,
	RST7 = 0xD7
};

/*------------------------------------------------------------------------------
    3. Data types
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    4. Function prototypes
------------------------------------------------------------------------------*/

#endif /* #ifdef JPEGDECMARKERS_H */
