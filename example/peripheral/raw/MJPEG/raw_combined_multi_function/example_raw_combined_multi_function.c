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


#define MASK1_WIDTH				48
#define MASK1_HEIGHT			96
#define MASK2_WIDTH				64
#define MASK2_HEIGHT			100

#define FRAME_BUFFER_WIDTH 	1024
#define FRAME_BUFFER_HEIGHT 	768

void combined_multi_function(void)
{
	JpegDecInst jpegInst = NULL;
	JpegDecRet jpegRet;
	JpegDecImageInfo imageInfo;
	JpegDecInput jpegIn;
	JpegDecOutput jpegOut;
	JpegDecLinearMem input;
	PPInst pp = NULL;
	PPResult ppRet;
	PPConfig pPpConf;
	u32 *frame_buffer;

	_memset(&jpegIn, 0, sizeof(JpegDecInput));
	_memset(&jpegOut, 0, sizeof(JpegDecOutput));
	_memset(&imageInfo, 0, sizeof(JpegDecImageInfo));
	_memset(&pPpConf, 0, sizeof(PPConfig));
	frame_buffer = (u32 *)rtos_mem_malloc(FRAME_BUFFER_WIDTH * FRAME_BUFFER_HEIGHT * 4);

	for (int i = 0; i < FRAME_BUFFER_WIDTH * FRAME_BUFFER_HEIGHT; i++) {
		frame_buffer[i] = 0x009FE4D4;
	}

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
	/* Pointer to the input JPEG */
	jpegIn.streamBuffer.pVirtualAddress = (u32 *) input.pVirtualAddress;
	jpegIn.streamBuffer.busAddress = input.busAddress;
	jpegIn.streamLength = jpeg_422_len; /* JPEG length in bytes */
	jpegIn.bufferSize = 0; /* input buffering (0 == not used) */

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

	// set input
	pPpConf.ppInImg.width = imageInfo.outputWidth;
	pPpConf.ppInImg.height = imageInfo.outputHeight;
	pPpConf.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_2_SEMIPLANAR;
	pPpConf.ppInImg.videoRange = 1;

	//set output
	pPpConf.ppOutImg.pixFormat = PP_PIX_FMT_RGB32;
	pPpConf.ppOutRgb.rgbTransform = PP_YCBCR2RGB_TRANSFORM_BT_709;
	pPpConf.ppOutImg.bufferBusAddr = (u32) frame_buffer;;

	pPpConf.ppOutImg.width = 640;
	pPpConf.ppOutImg.height = 400;

	// set crop
	pPpConf.ppInCrop.enable = 1;
	pPpConf.ppInCrop.originX = 96;
	pPpConf.ppInCrop.originY = 96;
	pPpConf.ppInCrop.width = 480;
	pPpConf.ppInCrop.height = 360;

	// set brightness
	pPpConf.ppOutRgb.brightness = 100;

	// set pip
	pPpConf.ppOutFrmBuffer.enable = 1;
	pPpConf.ppOutFrmBuffer.frameBufferWidth = FRAME_BUFFER_WIDTH;
	pPpConf.ppOutFrmBuffer.frameBufferHeight = FRAME_BUFFER_HEIGHT;
	pPpConf.ppOutFrmBuffer.writeOriginX = 100;
	pPpConf.ppOutFrmBuffer.writeOriginY = 100;

	//set mask1
	pPpConf.ppOutMask1.enable = 1;
	pPpConf.ppOutMask1.originX = 16;
	pPpConf.ppOutMask1.originY = 32;
	pPpConf.ppOutMask1.width = 48;
	pPpConf.ppOutMask1.height = 48;
	pPpConf.ppOutMask1.alphaBlendEna = 1;
	pPpConf.ppOutMask1.blendComponentBase = (u32)mask1_data;
	pPpConf.ppOutMask1.blendOriginX = 0;
	pPpConf.ppOutMask1.blendOriginY = 10;
	pPpConf.ppOutMask1.blendWidth = MASK1_WIDTH;
	pPpConf.ppOutMask1.blendHeight = MASK1_HEIGHT;

	//set mask2
	pPpConf.ppOutMask2.enable = 1;
	pPpConf.ppOutMask2.originX = 300;
	pPpConf.ppOutMask2.originY = 300;
	pPpConf.ppOutMask2.width = MASK2_WIDTH;
	pPpConf.ppOutMask2.height = MASK2_HEIGHT;

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
	rtos_mem_free((void *)frame_buffer);
	rtos_task_delete(NULL);
}

int example_raw_combined_multi_function(void)
{
	if (rtos_task_create(NULL, "RAW_COMBINED_MULTI_FUNCTION_TASK", (rtos_task_t)combined_multi_function, NULL, (4096), (1)) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Cannot create RAW_COMBINED_MULTI_FUNCTION_TASK\n");
	}

	return 0;
}


