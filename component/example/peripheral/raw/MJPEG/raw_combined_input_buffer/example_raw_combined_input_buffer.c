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

#define INPUT_BUFFERING_SIZE	(10*1024)

void combined_inputbuffer(void)
{
	JpegDecInst jpegInst;
	JpegDecRet jpegRet;
	JpegDecImageInfo imageInfo;
	JpegDecInput jpegIn;
	JpegDecOutput jpegOut;
	//JpegDecLinearMem input;
	//input.pVirtualAddress = NULL;
	u8 *inputbuffer;
	u8 *input_bus_address = jpeg_422;
	PPInst pp = NULL;
	PPResult ppRet;
	PPConfig pPpConf;
	u32 streamLen = jpeg_422_len;

	ICache_Invalidate();
	DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);
	Cache_Enable(DISABLE);

	_memset(&jpegIn, 0, sizeof(JpegDecInput));
	_memset(&jpegOut, 0, sizeof(JpegDecOutput));
	_memset(&imageInfo, 0, sizeof(JpegDecImageInfo));
	_memset(&pPpConf, 0, sizeof(PPConfig));

	RCC_PeriphClockCmd(APBPeriph_MJPEG, APBPeriph_MJPEG_CLOCK, ENABLE);
	hx170dec_init();

	inputbuffer = rtos_mem_malloc(INPUT_BUFFERING_SIZE);
	_memcpy((void *)inputbuffer, (void *)input_bus_address, INPUT_BUFFERING_SIZE);
	input_bus_address += INPUT_BUFFERING_SIZE;

	jpegRet = JpegDecInit(&jpegInst);
	if (jpegRet != JPEGDEC_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "JpegDecInit error: %d\n", jpegRet);
		goto end;
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "JpegDecInit OK\n");
	}

	/* Pointer to the input JPEG */
	jpegIn.streamBuffer.pVirtualAddress = (u32 *) inputbuffer;
	jpegIn.streamBuffer.busAddress = (u32)inputbuffer;
	jpegIn.streamLength = jpeg_422_len;
	jpegIn.bufferSize = INPUT_BUFFERING_SIZE;

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
	pPpConf.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_2_SEMIPLANAR;
	pPpConf.ppOutImg.width = imageInfo.outputWidth;
	pPpConf.ppOutImg.height = imageInfo.outputHeight;
	pPpConf.ppOutImg.pixFormat = PP_PIX_FMT_RGB32;
	pPpConf.ppInImg.videoRange = 1;
	pPpConf.ppOutRgb.rgbTransform = PP_YCBCR2RGB_TRANSFORM_BT_709;
	pPpConf.ppOutImg.bufferBusAddr = (u32) rtos_mem_malloc(pPpConf.ppOutImg.width * pPpConf.ppOutImg.height * 4);

	ppRet = PPSetConfig(pp, &pPpConf);
	if (ppRet != PP_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "PPSetConfig error: %d\n", ppRet);
		goto end;
	}

	do {
		/* Decode JPEG */
		jpegRet = JpegDecDecode(jpegInst, &jpegIn, &jpegOut);
		if (jpegRet == JPEGDEC_FRAME_READY) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "JpegDecDecode OK\n");
			goto end;
		} else if (jpegRet == JPEGDEC_STRM_PROCESSED) {
			/* load new data to the input buffer */
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "JPEGDEC_STRM_PROCESSED\n");
			_memcpy(inputbuffer, input_bus_address, INPUT_BUFFERING_SIZE);
			input_bus_address += INPUT_BUFFERING_SIZE;
			/* Pointer to the input JPEG */
			// jpegIn.streamBuffer.pVirtualAddress = inputbuffer;
			// jpegIn.streamBuffer.busAddress = input.busAddress;
			streamLen -= INPUT_BUFFERING_SIZE;
			if (streamLen < INPUT_BUFFERING_SIZE) {
				jpegIn.bufferSize = streamLen;
			}
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "JpegDecDecode error: %d\n", jpegRet);
			goto end;
		}
	} while (jpegRet != JPEGDEC_FRAME_READY);
end:
	PPRelease(pp);
	JpegDecRelease(jpegInst);
	rtos_mem_free((void *)pPpConf.ppOutImg.bufferBusAddr);
	rtos_task_delete(NULL);
}

int example_raw_combined_input_buffer(void)
{
	if (rtos_task_create(NULL, "RAW_MJPEG_COMBINED_INPUTBUFFER_TASK", (rtos_task_t)combined_inputbuffer, NULL, (4096), (1)) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Cannot create RAW_MJPEG_COMBINED_INPUTBUFFER_TASK\n");
	}

	return 0;
}


