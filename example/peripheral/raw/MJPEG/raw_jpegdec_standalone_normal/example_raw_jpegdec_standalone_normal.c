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


void JPEG_standalone_normal(void)
{
	JpegDecInst jpegInst;
	JpegDecRet jpegRet;
	JpegDecImageInfo imageInfo;
	JpegDecInput jpegIn;
	JpegDecOutput jpegOut;
	JpegDecLinearMem input;

	/* Set all configuartion to 0 */
	_memset(&jpegIn, 0, sizeof(JpegDecInput));
	_memset(&jpegOut, 0, sizeof(JpegDecOutput));
	_memset(&imageInfo, 0, sizeof(JpegDecImageInfo));
	_memset(&input, 0, sizeof(JpegDecLinearMem));

	/* enable MJPEG clock and low level driver */
	RCC_PeriphClockCmd(APBPeriph_MJPEG, APBPeriph_MJPEG_CLOCK, ENABLE);
	hx170dec_init();

	/* Init JPEG decoder instance */
	jpegRet = JpegDecInit(&jpegInst);
	if (jpegRet != JPEGDEC_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "JpegDecInit error: %d\n", jpegRet);
		goto end;
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "JpegDecInit OK\n");
	}

	input.pVirtualAddress = (u32 *)jpeg_422;
	input.busAddress = (u32) jpeg_422;
	jpegIn.streamBuffer.pVirtualAddress = (u32 *) input.pVirtualAddress;
	jpegIn.streamBuffer.busAddress = input.busAddress;
	jpegIn.streamLength = jpeg_422_len; /* JPEG length in bytes */
	jpegIn.bufferSize = 0; /* input buffering (0 == not used) */

	/* Get image information of the JPEG image */
	jpegRet = JpegDecGetImageInfo(jpegInst, &jpegIn, &imageInfo);
	if (jpegRet != JPEGDEC_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "JpegDecGetImageInfo error: %d\n", jpegRet);
		goto end;
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "JpegDecGetImageInfo OK\n");
	}

	/* Decode JPEG */
	jpegRet = JpegDecDecode(jpegInst, &jpegIn, &jpegOut);
	if (jpegRet != JPEGDEC_FRAME_READY) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "JpegDecDecode error: %d\n", jpegRet);
		goto end;
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "JpegDecDecode OK\n");
	}

	/* check the output data address */
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "Y data addr: %x, CbCr data addr: %x\n", jpegOut.outputPictureY.pVirtualAddress, jpegOut.outputPictureCbCr.pVirtualAddress);

end:
	/* Release JPEG Decoder */
	JpegDecRelease(jpegInst);
	rtos_task_delete(NULL);
}

int example_raw_jpegdec_standalone_normal(void)
{
	if (rtos_task_create(NULL, "RAW_MJPEG_STANDALONE_NORMAL_TASK", (rtos_task_t)JPEG_standalone_normal, NULL, (4096), (1)) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Cannot create RAW_MJPEG_STANDALONE_NORMAL_TASK\n");
	}

	return 0;
}


