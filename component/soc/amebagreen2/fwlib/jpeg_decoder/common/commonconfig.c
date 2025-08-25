/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "commonconfig.h"
#include "dwl.h"
#include "deccfg.h"
#include "regdrv.h"

/*------------------------------------------------------------------------------
    2. External identifiers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    4. Module indentifiers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

   5.1  Function name: SetCommonConfigRegs

        Purpose:

        Input:

        Output:

------------------------------------------------------------------------------*/

void SetCommonConfigRegs(u32 *pRegs)
{

	/* set parameters defined in deccfg.h */
	SetDecRegister(pRegs, HWIF_DEC_OUT_ENDIAN,
				   DEC_X170_OUTPUT_PICTURE_ENDIAN);

	SetDecRegister(pRegs, HWIF_DEC_IN_ENDIAN,
				   DEC_X170_INPUT_DATA_ENDIAN);

	SetDecRegister(pRegs, HWIF_DEC_STRENDIAN_E,
				   DEC_X170_INPUT_STREAM_ENDIAN);

	SetDecRegister(pRegs, HWIF_DEC_MAX_BURST,
				   DEC_X170_BUS_BURST_LENGTH);

	if ((DWLReadAsicID() >> 16) == 0x8170U) {
		SetDecRegister(pRegs, HWIF_PRIORITY_MODE,
					   DEC_X170_ASIC_SERVICE_PRIORITY);
	} else {
		SetDecRegister(pRegs, HWIF_DEC_SCMD_DIS,
					   DEC_X170_SCMD_DISABLE);
	}

#if (DEC_X170_APF_DISABLE)
	SetDecRegister(pRegs, HWIF_DEC_ADV_PRE_DIS, 1);
	SetDecRegister(pRegs, HWIF_APF_THRESHOLD, 0);
#else
	{
		u32 apfTmpThreshold = 0;

		SetDecRegister(pRegs, HWIF_DEC_ADV_PRE_DIS, 0);

		if (DEC_X170_REFBU_SEQ) {
			apfTmpThreshold = DEC_X170_REFBU_NONSEQ / DEC_X170_REFBU_SEQ;
		} else {
			apfTmpThreshold = DEC_X170_REFBU_NONSEQ;
		}

		if (apfTmpThreshold > 63) {
			apfTmpThreshold = 63;
		}

		SetDecRegister(pRegs, HWIF_APF_THRESHOLD, apfTmpThreshold);
	}
#endif


	SetDecRegister(pRegs, HWIF_DEC_LATENCY,
				   DEC_X170_LATENCY_COMPENSATION);

	SetDecRegister(pRegs, HWIF_DEC_DATA_DISC_E,
				   DEC_X170_DATA_DISCARD_ENABLE);

	SetDecRegister(pRegs, HWIF_DEC_OUTSWAP32_E,
				   DEC_X170_OUTPUT_SWAP_32_ENABLE);

	SetDecRegister(pRegs, HWIF_DEC_INSWAP32_E,
				   DEC_X170_INPUT_DATA_SWAP_32_ENABLE);

	SetDecRegister(pRegs, HWIF_DEC_STRSWAP32_E,
				   DEC_X170_INPUT_STREAM_SWAP_32_ENABLE);

#if( DEC_X170_HW_TIMEOUT_INT_ENA  != 0)
	SetDecRegister(pRegs, HWIF_DEC_TIMEOUT_E, 1);
	SetDecRegister(pRegs, HWIF_DEC_TIMEOUT_CYCLES, HANTRODEC_TIMEOUT_OVERRIDE);
#else
	SetDecRegister(pRegs, HWIF_DEC_TIMEOUT_E, 0);
	SetDecRegister(pRegs, HWIF_DEC_TIMEOUT_CYCLES, 0);
#endif
	SetDecRegister(pRegs, HWIF_RD_CACHE_E, HANTRODEC_RD_L2CACHE_E);

#if( DEC_X170_INTERNAL_CLOCK_GATING != 0)
	SetDecRegister(pRegs, HWIF_DEC_CLK_GATE_E, 1);
#else
	SetDecRegister(pRegs, HWIF_DEC_CLK_GATE_E, 0);
#endif

#if( DEC_X170_USING_IRQ  == 0)
	SetDecRegister(pRegs, HWIF_DEC_IRQ_DIS, 1);
#else
	SetDecRegister(pRegs, HWIF_DEC_IRQ_DIS, 0);
#endif

	SetDecRegister(pRegs, HWIF_SERV_MERGE_DIS,
				   DEC_X170_SERVICE_MERGE_DISABLE);

	/* set AXI RW IDs */
	SetDecRegister(pRegs, HWIF_DEC_AXI_RD_ID,
				   (DEC_X170_AXI_ID_R & 0xFFU));
	SetDecRegister(pRegs, HWIF_DEC_AXI_WR_ID,
				   (DEC_X170_AXI_ID_W & 0xFFU));

}
