/*------------------------------------------------------------------------------
--         Copyright (c) 2015, VeriSilicon Inc. All rights reserved           --
--         Copyright (c) 2011-2014, Google Inc. All rights reserved.          --
--         Copyright (c) 2007-2010, Hantro OY. All rights reserved.           --
--                                                                            --
-- This software is confidential and proprietary and may be used only as      --
--   expressly authorized by VeriSilicon in a written licensing agreement.    --
--                                                                            --
--         This entire notice must be reproduced on all copies                --
--                       and may not be removed.                              --
--                                                                            --
--------------------------------------------------------------------------------
-- Redistribution and use in source and binary forms, with or without         --
-- modification, are permitted provided that the following conditions are met:--
--   * Redistributions of source code must retain the above copyright notice, --
--       this list of conditions and the following disclaimer.                --
--   * Redistributions in binary form must reproduce the above copyright      --
--       notice, this list of conditions and the following disclaimer in the  --
--       documentation and/or other materials provided with the distribution. --
--   * Neither the names of Google nor the names of its contributors may be   --
--       used to endorse or promote products derived from this software       --
--       without specific prior written permission.                           --
--------------------------------------------------------------------------------
-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"--
-- AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE  --
-- IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE --
-- ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE  --
-- LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR        --
-- CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF       --
-- SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS   --
-- INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN    --
-- CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)    --
-- ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE --
-- POSSIBILITY OF SUCH DAMAGE.                                                --
--------------------------------------------------------------------------------
------------------------------------------------------------------------------*/


#ifndef __DEC_X170_CFG_H__
#define __DEC_X170_CFG_H__

/* predefined values of HW system parameters. DO NOT ALTER! */
#define DEC_X170_LITTLE_ENDIAN                      1
#define DEC_X170_BIG_ENDIAN                         0

#define DEC_X170_BUS_BURST_LENGTH_UNDEFINED         0
#define DEC_X170_BUS_BURST_LENGTH_4                 4
#define DEC_X170_BUS_BURST_LENGTH_8                 8
#define DEC_X170_BUS_BURST_LENGTH_16                16

#define DEC_X170_ASIC_SERVICE_PRIORITY_DEFAULT      0
#define DEC_X170_ASIC_SERVICE_PRIORITY_WR_1         1
#define DEC_X170_ASIC_SERVICE_PRIORITY_WR_2         2
#define DEC_X170_ASIC_SERVICE_PRIORITY_RD_1         3
#define DEC_X170_ASIC_SERVICE_PRIORITY_RD_2         4

#define DEC_X170_OUTPUT_FORMAT_RASTER_SCAN          0
#define DEC_X170_OUTPUT_FORMAT_TILED                1

/* end of predefined values */

/* now what we use */

#ifndef DEC_X170_USING_IRQ
/* Control IRQ generation by decoder hardware */
#define DEC_X170_USING_IRQ                          1
#endif

#ifndef DEC_X170_ASIC_SERVICE_PRIORITY
/* hardware intgernal prioriy scheme. better left unchanged */
#define DEC_X170_ASIC_SERVICE_PRIORITY   DEC_X170_ASIC_SERVICE_PRIORITY_DEFAULT
#endif

#ifndef DEC_X170_SCMD_DISABLE
/* AXI single command multiple data disable not set */
#define DEC_X170_SCMD_DISABLE                       0
#endif

#ifndef DEC_X170_APF_DISABLE
/* Advanced prefetch disable flag. If disable flag is set, product shall
 * operate akin to 9190 and earlier products. */
#define DEC_X170_APF_DISABLE                        0
#endif

#ifndef DEC_X170_BUS_BURST_LENGTH
/* how long are the hardware data bursts; better left unchanged    */
#define DEC_X170_BUS_BURST_LENGTH         DEC_X170_BUS_BURST_LENGTH_16
#endif

#ifndef DEC_X170_INPUT_STREAM_ENDIAN
/* this should match the system endianess, so that Decoder reads      */
/* the input stream in the right order                                */
#define DEC_X170_INPUT_STREAM_ENDIAN      DEC_X170_LITTLE_ENDIAN
#endif

#ifndef DEC_X170_OUTPUT_PICTURE_ENDIAN
/* this should match the system endianess, so that Decoder writes     */
/* the output pixel data in the right order                           */
#define DEC_X170_OUTPUT_PICTURE_ENDIAN    DEC_X170_LITTLE_ENDIAN
#endif

#ifndef DEC_X170_LATENCY_COMPENSATION
/* compensation for bus latency; values up to 63 */
#define DEC_X170_LATENCY_COMPENSATION               0
#endif

#ifndef DEC_X170_INTERNAL_CLOCK_GATING
/* clock is gated from decoder structures that are not used */
#define DEC_X170_INTERNAL_CLOCK_GATING              0
#endif

#ifndef DEC_X170_OUTPUT_FORMAT
/* Decoder output picture format in external memory: Raster-scan or     */
/*macroblock tiled i.e. macroblock data written in consecutive addresses */
#define DEC_X170_OUTPUT_FORMAT            DEC_X170_OUTPUT_FORMAT_RASTER_SCAN
#endif

#ifndef DEC_X170_DATA_DISCARD_ENABLE
#define DEC_X170_DATA_DISCARD_ENABLE                0
#endif

/* Decoder output data swap for 32bit words*/
#ifndef DEC_X170_OUTPUT_SWAP_32_ENABLE
#define DEC_X170_OUTPUT_SWAP_32_ENABLE              1
#endif

/* Decoder input data swap(excluding stream data) for 32bit words*/
#ifndef DEC_X170_INPUT_DATA_SWAP_32_ENABLE
#define DEC_X170_INPUT_DATA_SWAP_32_ENABLE          1
#endif

/* Decoder input stream swap for 32bit words */
#ifndef DEC_X170_INPUT_STREAM_SWAP_32_ENABLE
#define DEC_X170_INPUT_STREAM_SWAP_32_ENABLE        1
#endif

/* Decoder input data endian. Do not modify this! */
#ifndef DEC_X170_INPUT_DATA_ENDIAN
#define DEC_X170_INPUT_DATA_ENDIAN        DEC_X170_BIG_ENDIAN
#endif


/* AXI bus read and write ID values used by HW. 0 - 255 */
#ifndef DEC_X170_AXI_ID_R
#define DEC_X170_AXI_ID_R                           0
#endif

#ifndef DEC_X170_AXI_ID_W
#define DEC_X170_AXI_ID_W                           0
#endif

/* Decoder service merge disable */
#ifndef DEC_X170_SERVICE_MERGE_DISABLE
#define DEC_X170_SERVICE_MERGE_DISABLE              0
#endif

/* Check validity of values */

/* data discard and tiled mode can not be on simultaneously */
#if (DEC_X170_DATA_DISCARD_ENABLE && (DEC_X170_OUTPUT_FORMAT == DEC_X170_OUTPUT_FORMAT_TILED))
#error "Bad value specified: DEC_X170_DATA_DISCARD_ENABLE && (DEC_X170_OUTPUT_FORMAT == DEC_X170_OUTPUT_FORMAT_TILED)"
#endif

#if (DEC_X170_OUTPUT_PICTURE_ENDIAN > 1)
#error "Bad value specified for DEC_X170_OUTPUT_PICTURE_ENDIAN"
#endif

#if (DEC_X170_OUTPUT_FORMAT > 1)
#error "Bad value specified for DEC_X170_OUTPUT_FORMAT"
#endif

#if (DEC_X170_BUS_BURST_LENGTH > 31)
#error "Bad value specified for DEC_X170_AMBA_BURST_LENGTH"
#endif

#if (DEC_X170_ASIC_SERVICE_PRIORITY > 4)
#error "Bad value specified for DEC_X170_ASIC_SERVICE_PRIORITY"
#endif

#if (DEC_X170_LATENCY_COMPENSATION > 63)
#error "Bad value specified for DEC_X170_LATENCY_COMPENSATION"
#endif

#if (DEC_X170_OUTPUT_SWAP_32_ENABLE > 1)
#error "Bad value specified for DEC_X170_OUTPUT_SWAP_32_ENABLE"
#endif

#if (DEC_X170_INPUT_DATA_SWAP_32_ENABLE > 1)
#error "Bad value specified for DEC_X170_INPUT_DATA_SWAP_32_ENABLE"
#endif

#if (DEC_X170_INPUT_STREAM_SWAP_32_ENABLE > 1)
#error "Bad value specified for DEC_X170_INPUT_STREAM_SWAP_32_ENABLE"
#endif

#if (DEC_X170_OUTPUT_SWAP_32_ENABLE > 1)
#error "Bad value specified for DEC_X170_INPUT_DATA_ENDIAN"
#endif

#if (DEC_X170_DATA_DISCARD_ENABLE > 1)
#error "Bad value specified for DEC_X170_DATA_DISCARD_ENABLE"
#endif

#if (DEC_X170_SERVICE_MERGE_DISABLE > 1)
#error "Bad value specified for DEC_X170_SERVICE_MERGE_DISABLE"
#endif

/* Common defines for the decoder */

/* Number registers for the decoder */
#define DEC_X170_REGISTERS          60

#define TOTAL_X170_ORIGIN_REGS      119
#define TOTAL_X170_EXPAND_REGS      36
#define TOTAL_X170_REGISTERS        (TOTAL_X170_ORIGIN_REGS + TOTAL_X170_EXPAND_REGS)
#define DEC_X170_EXPAND_REGS        27

#define DEC_PERF_CYCLE_COUNT        76

/* Max amount of stream */
#define DEC_X170_MAX_STREAM         ((1<<24)-1)

/* Timeout value for the DWLWaitHwReady() call. */
/* Set to -1 for an unspecified value */
#ifndef DEC_X170_TIMEOUT_LENGTH
#define DEC_X170_TIMEOUT_LENGTH     -1
#endif

/* Enable HW internal watchdog timeout IRQ */
#define DEC_X170_HW_TIMEOUT_INT_ENA  1

#ifndef HANTRODEC_TIMEOUT_OVERRIDE
#define HANTRODEC_TIMEOUT_OVERRIDE   0x000FFFFF
#endif
#ifndef HANTRODEC_RD_L2CACHE_E
#define HANTRODEC_RD_L2CACHE_E   0x00000020
#endif

/* Memory wait states for reference buffer */
#define DEC_X170_REFBU_WIDTH        64
#define DEC_X170_REFBU_LATENCY      20
#define DEC_X170_REFBU_NONSEQ       8
#define DEC_X170_REFBU_SEQ          1

/* Check validity of the stream addresses */
#define X170_CHECK_BUS_ADDRESS(d)   ((d) < 64 ? 1 : 0)
#define X170_CHECK_VIRTUAL_ADDRESS(d)   (((void*)(d) < (void*)64) ? 1 : 0)
#define X170_CHECK_BUS_ADDRESS_AGLINED(d) (((d) < 64 || ((d) & (DEC_X170_BUS_BURST_LENGTH_8 - 1))) ? 1 : 0)

/* Max number of macroblock rows to memset/memcpy in partial freeze error
 * concealment */
#define DEC_X170_MAX_EC_COPY_ROWS   8

#endif /* __DEC_X170_CFG_H__ */
