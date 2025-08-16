/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "os_wrapper.h"
#include "basetype.h"
#include "jpegdecapi.h"
#include "ppapi.h"

#include "jpeg_data.c"

void combined_normal(void)
{
	JpegDecInst jpegInst;
	JpegDecRet jpegRet;
	JpegDecImageInfo imageInfo;
	JpegDecInput jpegIn;
	JpegDecOutput jpegOut;
	JpegDecLinearMem input;
	PPInst pp ;
	PPResult ppRet;
	PPConfig pPpConf;

	_memset(&jpegIn, 0, sizeof(JpegDecInput));
	_memset(&jpegOut, 0, sizeof(JpegDecOutput));
	_memset(&imageInfo, 0, sizeof(JpegDecImageInfo));
	_memset(&pPpConf, 0, sizeof(PPConfig));

	RCC_PeriphClockCmd(APBPeriph_MJPEG, APBPeriph_MJPEG_CLOCK, ENABLE);
	hx170dec_init();

	jpegRet = JpegDecInit(&jpegInst);
	if (jpegRet != JPEGDEC_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "JpegDecInit error: %d\n", jpegRet);
		goto end;
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "JpegDecInit OK\n");
	}

	input.pVirtualAddress = (u32 *) jpeg_422;
	input.busAddress = (u32) jpeg_422;
	jpegIn.streamBuffer.pVirtualAddress = (u32 *) input.pVirtualAddress;
	jpegIn.streamBuffer.busAddress = input.busAddress;
	jpegIn.streamLength = jpeg_422_len;
	jpegIn.bufferSize = 0;

	jpegRet = JpegDecGetImageInfo(jpegInst, &jpegIn, &imageInfo);
	if (jpegRet != JPEGDEC_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "JpegDecGetImageInfo error: %d\n", jpegRet);
		goto end;
	}

	ppRet = PPInit(&pp);
	if (ppRet != PP_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "PPInit error: %d\n", ppRet);
		goto end;
	}

	ppRet = PPDecCombinedModeEnable(pp, jpegInst, PP_PIPELINED_DEC_TYPE_JPEG);
	if (ppRet != PP_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "PPDecCombinedModeEnable error: %d\n", ppRet);
		goto end;
	}

	ppRet = PPGetConfig(pp, &pPpConf);
	if (ppRet != PP_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "PPGetConfig error: %d\n", ppRet);
		goto end;
	}

	pPpConf.ppInImg.width = imageInfo.outputWidth;
	pPpConf.ppInImg.height = imageInfo.outputHeight;
	pPpConf.ppInImg.videoRange = 1;
	pPpConf.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_2_SEMIPLANAR;
	pPpConf.ppOutImg.width = pPpConf.ppInImg.width;
	pPpConf.ppOutImg.height = pPpConf.ppInImg.height;
	pPpConf.ppOutImg.pixFormat = PP_PIX_FMT_RGB32;
	pPpConf.ppOutRgb.rgbTransform = PP_YCBCR2RGB_TRANSFORM_BT_709;
	pPpConf.ppOutImg.bufferBusAddr = (u32) rtos_mem_malloc(pPpConf.ppOutImg.width * pPpConf.ppOutImg.height * 4);

	ppRet = PPSetConfig(pp, &pPpConf);
	if (ppRet != PP_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "PPSetConfig error: %d\n", ppRet);
		goto end;
	}

	jpegRet = JpegDecDecode(jpegInst, &jpegIn, &jpegOut);
	if (jpegRet != JPEGDEC_FRAME_READY) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "JpegDecDecode error: %d\n", jpegRet);
		goto end;
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "JpegDecDecode OK\n");
	}

	ppRet = PPDecCombinedModeDisable(pp, jpegInst);
	if (ppRet != PP_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "PPDecCombinedModeDisable error: %d\n", ppRet);
		goto end;
	}

end:
	PPRelease(pp);
	JpegDecRelease(jpegInst);
	rtos_mem_free((void *)pPpConf.ppOutImg.bufferBusAddr);
	rtos_task_delete(NULL);
}

int example_raw_combined_normal(void)
{
	if (rtos_task_create(NULL, "RAW_COMBINED_NORMAL_TASK", (rtos_task_t)combined_normal, NULL, (4096), (1)) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Cannot create RAW_COMBINED_NORMAL_TASK\n");
	}

	return 0;
}


