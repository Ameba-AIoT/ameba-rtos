/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __PPCFG_H__
#define __PPCFG_H__

/* predefined values of HW system parameters. DO NOT ALTER!      */
#define PP_X170_PICTURE_LITTLE_ENDIAN               1
#define PP_X170_PICTURE_BIG_ENDIAN                  0

#define PP_X170_BUS_BURST_LENGTH_UNDEFINED          0
#define PP_X170_BUS_BURST_LENGTH_4                  4
#define PP_X170_BUS_BURST_LENGTH_8                  8
#define PP_X170_BUS_BURST_LENGTH_16                16

#define PP_X170_DATA_BUS_WIDTH_32                   4
#define PP_X170_DATA_BUS_WIDTH_64                   8
/* end of predefined values */

/* now what we use */

#ifndef PP_X170_USING_IRQ
/* use the HW IRQ or not; set to non-zero to enable                */
/* if set to zero the software will disable the PP IRQ             */
/* generation in the control register                              */
#define PP_X170_USING_IRQ                           1
#endif

#define PP_X170_SWAP_32_WORDS                       1

#ifdef PP_X170_SWAP_32_WORDS
#define PP_X170_SWAP_32_WORDS                       1
#define PP_X170_SWAP_32_WORDS_RGB32                 1
#define PP_X170_SWAP_32_WORDS_RGB16                 1
#else
#define PP_X170_SWAP_32_WORDS                       0
#define PP_X170_SWAP_32_WORDS_RGB32                 0
#define PP_X170_SWAP_32_WORDS_RGB16                 0
#endif

#ifndef PP_X170_SWAP_16_WORDS
#define PP_X170_SWAP_16_WORDS                       0
#endif

#ifndef PP_X170_SWAP_16_WORDS_RGB32
#define PP_X170_SWAP_16_WORDS_RGB32                 0
#endif

#ifndef PP_X170_SWAP_16_WORDS_RGB16
#define PP_X170_SWAP_16_WORDS_RGB16                 1
#endif


#ifndef PP_X170_INPUT_PICTURE_ENDIAN
/* this should match the system endianess, so that PP reads        */
/* the input pixel data in the right order                         */
#define PP_X170_INPUT_PICTURE_ENDIAN PP_X170_PICTURE_LITTLE_ENDIAN
#endif

#ifndef PP_X170_OUTPUT_PICTURE_ENDIAN
/* this should match the system endianess, so that PP writes       */
/* the output pixel data in the right order                        */
#define PP_X170_OUTPUT_PICTURE_ENDIAN PP_X170_PICTURE_LITTLE_ENDIAN
#endif

#ifndef PP_X170_OUTPUT_PICTURE_ENDIAN_RGB32
#define PP_X170_OUTPUT_PICTURE_ENDIAN_RGB32 PP_X170_PICTURE_BIG_ENDIAN
#endif

#ifndef PP_X170_OUTPUT_PICTURE_ENDIAN_RGB16
#define PP_X170_OUTPUT_PICTURE_ENDIAN_RGB16 PP_X170_PICTURE_BIG_ENDIAN
#endif

#ifndef PP_X170_BUS_BURST_LENGTH
/* how long are the hardware data bursts; better left unchanged    */
#define PP_X170_BUS_BURST_LENGTH        PP_X170_BUS_BURST_LENGTH_16
#endif

#ifndef PP_X170_DATA_BUS_WIDTH
/* data bus width of the PP hardware; note that this might not be   */
/* the same with the CPU's data bus width                           */
#define PP_X170_DATA_BUS_WIDTH PP_X170_DATA_BUS_WIDTH_64
#endif

#ifndef PP_X170_LATENCY_COMPENSATION
/* compensation for bus latency; values up to 63 */
#define PP_X170_LATENCY_COMPENSATION                0
#endif

#ifndef PP_X170_INTERNAL_CLOCK_GATING
/* clock is gated from PP structures that are not used */
#define PP_X170_INTERNAL_CLOCK_GATING               0
#endif

#ifndef PP_X170_DATA_DISCARD_ENABLE
#define PP_X170_DATA_DISCARD_ENABLE                 0
#endif

#ifndef PP_X170_SWAP_32_WORDS_INPUT
#define PP_X170_SWAP_32_WORDS_INPUT                 1
#endif

/* AXI bus read and write ID values used by HW. 0 - 255 */
#ifndef PP_X170_AXI_ID_R
#define PP_X170_AXI_ID_R                            0
#endif

#ifndef PP_X170_AXI_ID_W
#define PP_X170_AXI_ID_W                            0
#endif

/* AXI single command multiple data disable not set */
#define PP_X170_SCMD_DISABLE                        0

/* flag to ignore endianness when reading RGB format alpha blend picture.
 * DO NOT ALTER! */
#define PP_X170_IGNORE_ABLEND_ENDIANNESS            0

/* disable fast scaling shortcuts */
#ifndef PP_X170_FAST_VERTICAL_DOWNSCALE_DISABLE
#define PP_X170_FAST_VERTICAL_DOWNSCALE_DISABLE     0
#endif

#ifndef PP_X170_FAST_HORIZONTAL_DOWNSCALE_DISABLE
#define PP_X170_FAST_HORIZONTAL_DOWNSCALE_DISABLE   0
#endif

#endif /* __PPCFG_H__ */
