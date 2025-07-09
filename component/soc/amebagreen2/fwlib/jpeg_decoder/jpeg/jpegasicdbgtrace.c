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

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/
#include "dwl.h"
#include "jpegregdrv.h"
#include "jpegasicdbgtrace.h"
#include "basetype.h"
#include "jpegdeccontainer.h"
#include <stdio.h>
#include <assert.h>

#ifdef JPEGDEC_PP_TRACE
#include "pphwregdrv.h"
#endif /* #ifdef JPEGDEC_PP_TRACE */

#ifdef JPEGDEC_ASIC_TRACE
static u32 sliceCounter = 0;
#endif /* #ifdef JPEGDEC_ASIC_TRACE */

/*------------------------------------------------------------------------------
    2. External compiler flags
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    4. Local function prototypes
------------------------------------------------------------------------------*/
#ifdef JPEGDEC_ASIC_TRACE
/*------------------------------------------------------------------------------
    5. Functions
------------------------------------------------------------------------------*/
void DumpJPEGCtrlReg(u32 *regBase, FILE *fd)
{
	assert(fd);

	fprintf(fd,
			"------------------ picture = %d ------------------------------------\n",
			sliceCounter);
	fprintf(fd, "%-10d decoding mode (0=h264, 1=mpeg4, 2=h263, 3=jpeg)\n",
			GetJpegDecodingMode(regBase));
	fprintf(fd, "%-10d RLC mode enable (0=HW decodes VLC)\n",
			GetJpegDecRlcModeDisable(regBase));
	fprintf(fd, "%-10d Picture width in macro blocks\n",
			GetJpegDecPictureWidth(regBase));
	fprintf(fd, "%-10d Picture height in macro blocks\n",
			GetJpegDecPictureHeight(regBase));
	fprintf(fd, "%-10d JPEG sampling format\n",
			GetJpegDecDecodingInputFormat(regBase));
	fprintf(fd, "%-10d amount of quantization tables\n",
			GetJpegDecAmountOfQpTables(regBase));
	/*fprintf(fd,"%-10d amount of rlc/pic\n",sliceCounter); */
	fprintf(fd,
			"%-10d Defines which VLC table should be used for decoding AC for Cr\n",
			GetJpegDecAcTableSelectorBitCr(regBase));
	fprintf(fd,
			"%-10d Defines which VLC table should be used for decoding AC for Cb\n",
			GetJpegDecAcTableSelectorBitCb(regBase));
	fprintf(fd,
			"%-10d Defines which VLC table should be used for decoding DC for Cr\n",
			GetJpegDecDcTableSelectorBitCr(regBase));
	fprintf(fd,
			"%-10d Defines which VLC table should be used for decoding DC for Cb\n",
			GetJpegDecDcTableSelectorBitCb(regBase));
	fprintf(fd,
			"%-10d Defines which VLC table should be used for decoding QP for Cr\n",
			GetJpegDecQpTableSelectorBitCr(regBase));
	fprintf(fd,
			"%-10d Defines which VLC table should be used for decoding QP for Cb\n",
			GetJpegDecQpTableSelectorBitCb(regBase));
	fprintf(fd, "%-10d AC code words in table 1 of length 16\n",
			GetJpegDecAc1Len16(regBase));
	fprintf(fd, "%-10d AC code words in table 1 of length 15\n",
			GetJpegDecAc1Len15(regBase));
	fprintf(fd, "%-10d AC code words in table 1 of length 14\n",
			GetJpegDecAc1Len14(regBase));
	fprintf(fd, "%-10d AC code words in table 1 of length 13\n",
			GetJpegDecAc1Len13(regBase));
	fprintf(fd, "%-10d AC code words in table 1 of length 12\n",
			GetJpegDecAc1Len12(regBase));
	fprintf(fd, "%-10d AC code words in table 1 of length 11\n",
			GetJpegDecAc1Len11(regBase));
	fprintf(fd, "%-10d AC code words in table 1 of length 10\n",
			GetJpegDecAc1Len10(regBase));
	fprintf(fd, "%-10d AC code words in table 1 of length 9\n",
			GetJpegDecAc1Len9(regBase));
	fprintf(fd, "%-10d AC code words in table 1 of length 8\n",
			GetJpegDecAc1Len8(regBase));
	fprintf(fd, "%-10d AC code words in table 1 of length 7\n",
			GetJpegDecAc1Len7(regBase));
	fprintf(fd, "%-10d AC code words in table 1 of length 6\n",
			GetJpegDecAc1Len6(regBase));
	fprintf(fd, "%-10d AC code words in table 1 of length 5\n",
			GetJpegDecAc1Len5(regBase));
	fprintf(fd, "%-10d AC code words in table 1 of length 4\n",
			GetJpegDecAc1Len4(regBase));
	fprintf(fd, "%-10d AC code words in table 1 of length 3\n",
			GetJpegDecAc1Len3(regBase));
	fprintf(fd, "%-10d AC code words in table 1 of length 2\n",
			GetJpegDecAc1Len2(regBase));
	fprintf(fd, "%-10d AC code words in table 1 of length 1\n",
			GetJpegDecAc1Len1(regBase));
	fprintf(fd, "%-10d AC code words in table 2 of length 16\n",
			GetJpegDecAc2Len16(regBase));
	fprintf(fd, "%-10d AC code words in table 2 of length 15\n",
			GetJpegDecAc2Len15(regBase));
	fprintf(fd, "%-10d AC code words in table 2 of length 14\n",
			GetJpegDecAc2Len14(regBase));
	fprintf(fd, "%-10d AC code words in table 2 of length 13\n",
			GetJpegDecAc2Len13(regBase));
	fprintf(fd, "%-10d AC code words in table 2 of length 12\n",
			GetJpegDecAc2Len12(regBase));
	fprintf(fd, "%-10d AC code words in table 2 of length 11\n",
			GetJpegDecAc2Len11(regBase));
	fprintf(fd, "%-10d AC code words in table 2 of length 10\n",
			GetJpegDecAc2Len10(regBase));
	fprintf(fd, "%-10d AC code words in table 2 of length 9\n",
			GetJpegDecAc2Len9(regBase));
	fprintf(fd, "%-10d AC code words in table 2 of length 8\n",
			GetJpegDecAc2Len8(regBase));
	fprintf(fd, "%-10d AC code words in table 2 of length 7\n",
			GetJpegDecAc2Len7(regBase));
	fprintf(fd, "%-10d AC code words in table 2 of length 6\n",
			GetJpegDecAc2Len6(regBase));
	fprintf(fd, "%-10d AC code words in table 2 of length 5\n",
			GetJpegDecAc2Len5(regBase));
	fprintf(fd, "%-10d AC code words in table 2 of length 4\n",
			GetJpegDecAc2Len4(regBase));
	fprintf(fd, "%-10d AC code words in table 2 of length 3\n",
			GetJpegDecAc2Len3(regBase));
	fprintf(fd, "%-10d AC code words in table 2 of length 2\n",
			GetJpegDecAc2Len2(regBase));
	fprintf(fd, "%-10d AC code words in table 2 of length 1\n",
			GetJpegDecAc2Len1(regBase));
	fprintf(fd, "%-10d DC code words in table 1 of length 16\n",
			GetJpegDecDc1Len16(regBase));
	fprintf(fd, "%-10d DC code words in table 1 of length 15\n",
			GetJpegDecDc1Len15(regBase));
	fprintf(fd, "%-10d DC code words in table 1 of length 14\n",
			GetJpegDecDc1Len14(regBase));
	fprintf(fd, "%-10d DC code words in table 1 of length 13\n",
			GetJpegDecDc1Len13(regBase));
	fprintf(fd, "%-10d DC code words in table 1 of length 12\n",
			GetJpegDecDc1Len12(regBase));
	fprintf(fd, "%-10d DC code words in table 1 of length 11\n",
			GetJpegDecDc1Len11(regBase));
	fprintf(fd, "%-10d DC code words in table 1 of length 10\n",
			GetJpegDecDc1Len10(regBase));
	fprintf(fd, "%-10d DC code words in table 1 of length 9\n",
			GetJpegDecDc1Len9(regBase));
	fprintf(fd, "%-10d DC code words in table 1 of length 8\n",
			GetJpegDecDc1Len8(regBase));
	fprintf(fd, "%-10d DC code words in table 1 of length 7\n",
			GetJpegDecDc1Len7(regBase));
	fprintf(fd, "%-10d DC code words in table 1 of length 6\n",
			GetJpegDecDc1Len6(regBase));
	fprintf(fd, "%-10d DC code words in table 1 of length 5\n",
			GetJpegDecDc1Len5(regBase));
	fprintf(fd, "%-10d DC code words in table 1 of length 4\n",
			GetJpegDecDc1Len4(regBase));
	fprintf(fd, "%-10d DC code words in table 1 of length 3\n",
			GetJpegDecDc1Len3(regBase));
	fprintf(fd, "%-10d DC code words in table 1 of length 2\n",
			GetJpegDecDc1Len2(regBase));
	fprintf(fd, "%-10d DC code words in table 1 of length 1\n",
			GetJpegDecDc1Len1(regBase));
	fprintf(fd, "%-10d DC code words in table 2 of length 16\n",
			GetJpegDecDc2Len16(regBase));
	fprintf(fd, "%-10d DC code words in table 2 of length 15\n",
			GetJpegDecDc2Len15(regBase));
	fprintf(fd, "%-10d DC code words in table 2 of length 14\n",
			GetJpegDecDc2Len14(regBase));
	fprintf(fd, "%-10d DC code words in table 2 of length 13\n",
			GetJpegDecDc2Len13(regBase));
	fprintf(fd, "%-10d DC code words in table 2 of length 12\n",
			GetJpegDecDc2Len12(regBase));
	fprintf(fd, "%-10d DC code words in table 2 of length 11\n",
			GetJpegDecDc2Len11(regBase));
	fprintf(fd, "%-10d DC code words in table 2 of length 10\n",
			GetJpegDecDc2Len10(regBase));
	fprintf(fd, "%-10d DC code words in table 2 of length 9\n",
			GetJpegDecDc2Len9(regBase));
	fprintf(fd, "%-10d DC code words in table 2 of length 8\n",
			GetJpegDecDc2Len8(regBase));
	fprintf(fd, "%-10d DC code words in table 2 of length 7\n",
			GetJpegDecDc2Len7(regBase));
	fprintf(fd, "%-10d DC code words in table 2 of length 6\n",
			GetJpegDecDc2Len6(regBase));
	fprintf(fd, "%-10d DC code words in table 2 of length 5\n",
			GetJpegDecDc2Len5(regBase));
	fprintf(fd, "%-10d DC code words in table 2 of length 4\n",
			GetJpegDecDc2Len4(regBase));
	fprintf(fd, "%-10d DC code words in table 2 of length 3\n",
			GetJpegDecDc2Len3(regBase));
	fprintf(fd, "%-10d DC code words in table 2 of length 2\n",
			GetJpegDecDc2Len2(regBase));
	fprintf(fd, "%-10d DC code words in table 2 of length 1\n",
			GetJpegDecDc2Len1(regBase));

	sliceCounter++;
}

void HexDumpJPEGCtrlReg(u32 *regBase, FILE *fd)
{
	assert(fd);

	fprintf(fd, "%08x\n", GetJpegDecodingMode(regBase));
	fprintf(fd, "%08x\n", GetJpegDecRlcModeDisable(regBase));
	fprintf(fd, "%08x\n", GetJpegDecPictureWidth(regBase));
	fprintf(fd, "%08x\n", GetJpegDecPictureHeight(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDecodingInputFormat(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAmountOfQpTables(regBase));
	/*fprintf(fd,"%08x\n",sliceCounter); */
	fprintf(fd, "%08x\n", GetJpegDecAcTableSelectorBitCr(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAcTableSelectorBitCb(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDcTableSelectorBitCr(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDcTableSelectorBitCb(regBase));
	fprintf(fd, "%08x\n", GetJpegDecQpTableSelectorBitCr(regBase));
	fprintf(fd, "%08x\n", GetJpegDecQpTableSelectorBitCb(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc1Len16(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc1Len15(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc1Len14(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc1Len13(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc1Len12(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc1Len11(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc1Len10(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc1Len9(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc1Len8(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc1Len7(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc1Len6(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc1Len5(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc1Len4(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc1Len3(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc1Len2(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc1Len1(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc2Len16(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc2Len15(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc2Len14(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc2Len13(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc2Len12(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc2Len11(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc2Len10(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc2Len9(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc2Len8(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc2Len7(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc2Len6(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc2Len5(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc2Len4(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc2Len3(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc2Len2(regBase));
	fprintf(fd, "%08x\n", GetJpegDecAc2Len1(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc1Len16(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc1Len15(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc1Len14(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc1Len13(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc1Len12(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc1Len11(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc1Len10(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc1Len9(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc1Len8(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc1Len7(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc1Len6(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc1Len5(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc1Len4(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc1Len3(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc1Len2(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc1Len1(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc2Len16(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc2Len15(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc2Len14(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc2Len13(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc2Len12(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc2Len11(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc2Len10(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc2Len9(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc2Len8(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc2Len7(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc2Len6(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc2Len5(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc2Len4(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc2Len3(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc2Len2(regBase));
	fprintf(fd, "%08x\n", GetJpegDecDc2Len1(regBase));
}

void HexDumpJPEGTables(u32 *regBase, JpegDecContainer *pJpegDecCont,
					   FILE *fd)
{
	u32 i, j = 0;
	u32 amountOfParam = 0;
	u32 count = 0;
	u32 tableWord = 0;
	u32 tableWord2 = 0;
	u8 tableByte[8] = { 0 };
	u32 *pTables = NULL;

	assert(fd);

	/* pointer to table memory */
	pTables = pJpegDecCont->frame.pTableBase.virtualAddress;

	/* calculate amount of table variables */
	/* qp */
	amountOfParam = (pJpegDecCont->info.amountOfQTables * 64);
	/* huffman */
	for (i = 0; i < 16; i++) {
		/* ac tables */
		amountOfParam += pJpegDecCont->vlc.acTable0.bits[i];
		amountOfParam += pJpegDecCont->vlc.acTable1.bits[i];
		/* dc tables */
		amountOfParam += pJpegDecCont->vlc.dcTable0.bits[i];
		amountOfParam += pJpegDecCont->vlc.dcTable1.bits[i];
	}

	/* round up to 64-bit memory */
	if ((JPEGDEC_TABLE_SIZE % 8) != 0) {
		do {
			count++;
		} while (((JPEGDEC_TABLE_SIZE + count) % 8) != 0);
	}

	/* write data to trace */
	for (i = 0; i < ((JPEGDEC_TABLE_SIZE + count) / 4); i++) {
		tableWord = pTables[i];
		tableWord2 = pTables[i + 1];

		tableByte[0] = ((tableWord & 0xFF000000) >> 24);
		tableByte[1] = ((tableWord & 0x00FF0000) >> 16);
		tableByte[2] = ((tableWord & 0x0000FF00) >> 8);
		tableByte[3] = ((tableWord & 0x000000FF) >> 0);
		tableByte[4] = ((tableWord2 & 0xFF000000) >> 24);
		tableByte[5] = ((tableWord2 & 0x00FF0000) >> 16);
		tableByte[6] = ((tableWord2 & 0x0000FF00) >> 8);
		tableByte[7] = ((tableWord2 & 0x000000FF) >> 0);
		i++;

		if (i >= (JPEGDEC_TABLE_SIZE / 4)) {
			tableByte[4] = 0;
			tableByte[5] = 0;
			tableByte[6] = 0;
			tableByte[7] = 0;

		}

		if (i < (JPEGDEC_TABLE_SIZE / 4)) {
			for (j = 0; j < 8; j++) {
				fprintf(fd, "%02x ", tableByte[j]);
			}

			if ((i) % 1 == 0) {
				fprintf(fd, "\n");
			}
		} else {
			for (j = 0; j < 4; j++) {
				fprintf(fd, "%02x ", tableByte[j]);
			}

			for (j = 0; j < count; j++) {
				fprintf(fd, "0  ");
				i++;
			}
		}
	}
	fprintf(fd, "\n");
}

void HexDumpRegs(u32 *regBase, FILE *fd)
{
	i32 i;

	assert(fd);

	fprintf(fd, "\n");

	for (i = 0; i < (DEC_X170_REGISTERS); i++) {
		fprintf(fd, "Offset %02x: %08x\n", i, regBase[i]);
	}
	fprintf(fd, "\n");
}

void PrintJPEGReg(u32 *regBase)
{
	u32 reg, regs = 0;

	for (reg = 0; reg < DEC_X170_REGISTERS; reg++) {
		regs = regBase[reg];
		printf("swreg%-10d  0x%08x\n", reg, regs);
	}
	printf("\n");
}

#endif /* #ifdef JPEGDEC_ASIC_TRACE */

#ifdef JPEGDEC_PP_TRACE

void ppRegDump(const u32 *regBase)
{

	printf("GetPpInFormat \t%d\n", GetPpInFormat(regBase));
	printf("GetPpOutFormat(  ) %d\n", GetPpOutFormat(regBase));
	printf("GetPpOutHeight \t%d\n", GetPpOutHeight(regBase));
	printf("GetPpOutWidth \t%d\n", GetPpOutWidth(regBase));
	printf("GetPpCtrlDecPipeline \t%d\n", GetPpCtrlDecPipeline(regBase));
	printf("GetPpCtrlIrqDisabled \t%d\n", GetPpCtrlIrqDisabled(regBase));
	printf("GetPpCtrlIrqStat \t%d\n", GetPpCtrlIrqStat(regBase));
	printf("GetPpCtrlIrqStat \t%d\n", GetPpCtrlIrqStat(regBase));

	printf("GetPpInHeight \t%d\n", GetPpInHeight(regBase));
	printf("GetPpInWidth \t%d\n", GetPpInWidth(regBase));

	printf("GetPpFrameBufferWidth(  ) %d\n", GetPpFrameBufferWidth(regBase));
	printf("GetPpOutLumaOrRgbAddr \t%x\n", GetPpOutLumaOrRgbAddr(regBase));
	printf("GetPpOutChromaAddr \t%x\n", GetPpOutChromaAddr(regBase));
	printf("GetPpOutEndianess \t%x\n", GetPpOutEndianess(regBase));
	printf("GetPpInEndianess \t%d\n", GetPpInEndianess(regBase));

	printf("GetPpInLumAddr \t%x\n", GetPpInLumAddr(regBase));
	printf("GetPpInCbAddr \t%x\n", GetPpInCbAddr(regBase));
	printf("GetPpInCrAddr \t%x\n", GetPpInCrAddr(regBase));

	printf("GetPpInYOffset \t%d\n", GetPpInYOffset(regBase));
	printf("GetPpInXOffset \t%d\n", GetPpInXOffset(regBase));
	printf("GetPpInCroppedWidth \t%d\n", GetPpInCroppedWidth(regBase));
	printf("GetPpInEndianess \t%d\n", GetPpInEndianess(regBase));

	printf("GetPpCtrlAmbaBurstLength \t%d\n",
		   GetPpCtrlAmbaBurstLength(regBase));

	printf("GetPpContrastThr1 \t%d\n", GetPpContrastThr1(regBase));
	printf("GetPpContrastThr1 \t%d\n", GetPpContrastThr1(regBase));
	printf("GetPpContrastThr2 \t%d\n", GetPpContrastThr2(regBase));
	printf("GetPpContrastOffset1 \t%d\n", GetPpContrastOffset1(regBase));
	printf("GetPpContrastOffset2 \t%d\n", GetPpContrastOffset2(regBase));

	printf("\n");
}

#endif /* #ifdef JPEGDEC_PP_TRACE */
