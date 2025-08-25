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

void JPEG_standalone_slice(void)
{
	JpegDecInst jpegInst;
	JpegDecRet jpegRet;
	JpegDecImageInfo imageInfo;
	JpegDecInput jpegIn;
	JpegDecOutput jpegOut;
	u32 slicemb = 10;

	/* Set all configuartion to 0 */
	_memset(&jpegIn, 0, sizeof(JpegDecInput));
	_memset(&jpegOut, 0, sizeof(JpegDecOutput));
	_memset(&imageInfo, 0, sizeof(JpegDecImageInfo));

	/* enable MJPEG clock and low level driver */
	RCC_PeriphClockCmd(APBPeriph_MJPEG, APBPeriph_MJPEG_CLOCK, ENABLE);
	hx170dec_init();

	jpegRet = JpegDecInit(&jpegInst);
	if (jpegRet != JPEGDEC_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "JpegDecInit error: %d\n", jpegRet);
		goto end;
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "JpegDecInit OK\n");
	}

	/* Pointer to the input JPEG */
	jpegIn.streamBuffer.pVirtualAddress = (u32 *) jpeg_422;
	jpegIn.streamBuffer.busAddress = (u32)jpeg_422;
	jpegIn.streamLength = jpeg_422_len; /* JPEG length in bytes */
	jpegIn.bufferSize = 0; /* input buffering (0 == not used) */

	jpegRet = JpegDecGetImageInfo(jpegInst, &jpegIn, &imageInfo);
	if (jpegRet != JPEGDEC_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "JpegDecGetImageInfo error: %d\n", jpegRet);
		goto end;
	}

	/* Slice mode */
	u32 slicemb_total = imageInfo.outputHeight / 8;           /* total macro block number. For this picture, it is 480/8 = 60 */
	jpegIn.sliceMbSet = slicemb_total / slicemb;          /* It will output in 6 separate times. */

	do {
		/* slice decode */
		jpegRet = JpegDecDecode(jpegInst, &jpegIn, &jpegOut);
		if (jpegRet == JPEGDEC_FRAME_READY) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "JpegDecDecode OK\n");
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "Y data addr: %x, CbCr data addr: %x\n", jpegOut.outputPictureY.pVirtualAddress, jpegOut.outputPictureCbCr.pVirtualAddress);
			goto end;
		} else if (jpegRet == JPEGDEC_SLICE_READY) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "JPEGDEC_SLICE_READY\n");
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "Y data addr: %x, CbCr data addr: %x\n", jpegOut.outputPictureY.pVirtualAddress, jpegOut.outputPictureCbCr.pVirtualAddress);
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "JpegDecDecode error: %d\n", jpegRet);
			goto end;
		}
	} while (jpegRet != JPEGDEC_FRAME_READY);

end:
	JpegDecRelease(jpegInst);
	rtos_task_delete(NULL);
}

int example_raw_jpegdec_standalone_slice(void)
{
	if (rtos_task_create(NULL, "RAW_MJPEG_STANDALONE_SLICE_TASK", (rtos_task_t)JPEG_standalone_slice, NULL, (4096), (1)) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Cannot create RAW_MJPEG_STANDALONE_SLICE_TASK\n");
	}

	return 0;
}

