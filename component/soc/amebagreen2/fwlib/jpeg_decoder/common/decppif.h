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

#ifndef _PPDEC_H_
#define _PPDEC_H_

#include "basetype.h"

/* PP input types (picStruct) */
/* Frame or top field */
#define DECPP_PIC_FRAME_OR_TOP_FIELD                       0U
/* Bottom field only */
#define DECPP_PIC_BOT_FIELD                                1U
/* top and bottom is separate locations */
#define DECPP_PIC_TOP_AND_BOT_FIELD                        2U
/* top and bottom interleaved */
#define DECPP_PIC_TOP_AND_BOT_FIELD_FRAME                  3U
/* interleaved top only */
#define DECPP_PIC_TOP_FIELD_FRAME                          4U
/* interleaved bottom only */
#define DECPP_PIC_BOT_FIELD_FRAME                          5U

/* Control interface between decoder and pp */
/* decoder writes, pp read-only */

typedef struct DecPpInterface_ {
	enum {
		DECPP_IDLE = 0,
		DECPP_RUNNING,  /* PP was started */
		DECPP_PIC_READY, /* PP has finished a picture */
		DECPP_PIC_NOT_FINISHED /* PP still processing a picture */
	} ppStatus; /* Decoder keeps track of what it asked the pp to do */

	enum {
		MULTIBUFFER_UNINIT = 0, /* buffering mode not yet decided */
		MULTIBUFFER_DISABLED,   /* Single buffer legacy mode */
		MULTIBUFFER_SEMIMODE,   /* enabled but full pipel cannot be used */
		MULTIBUFFER_FULLMODE    /* enabled and full pipeline successful */
	} multiBufStat;
	g1_addr_t inputBusLuma;
	g1_addr_t inputBusChroma;
	g1_addr_t bottomBusLuma;
	g1_addr_t bottomBusChroma;
	u32 picStruct;           /* structure of input picture */
	u32 topField;
	u32 inwidth;
	u32 inheight;
	u32 usePipeline;
	u32 littleEndian;
	u32 wordSwap;
	u32 croppedW;
	u32 croppedH;

	u32 bufferIndex;         /* multibuffer, where to put PP output */
	u32 displayIndex;        /* multibuffer, next picture in display order */
	u32 prevAnchorDisplayIndex;

	/* VC-1 */
	u32 vc1AdvEnable;
	u32 rangeRed;
	u32 rangeMapYEnable;
	u32 rangeMapYCoeff;
	u32 rangeMapCEnable;
	u32 rangeMapCCoeff;

	u32 tiledInputMode;
	u32 progressiveSequence;

	u32 lumaStride;
	u32 chromaStride;

} DecPpInterface;

/* Decoder asks with this struct information about pp setup */
/* pp writes, decoder read-only */

typedef struct DecPpQuery_ {
	/* Dec-to-PP direction */
	u32 tiledMode;

	/* PP-to-Dec direction */
	u32 pipelineAccepted;    /* PP accepts pipeline */
	u32 deinterlace;         /* Deinterlace feature used */
	u32 multiBuffer;         /* multibuffer PP output enabled */
	u32 ppConfigChanged;     /* PP config changed after previous output */
} DecPpQuery;

#endif
