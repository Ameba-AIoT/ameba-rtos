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

#define YUV_DATA_WIDTH	640
#define YUV_DATA_HEIGHT	480

void PP_standalone_rotation(void)
{
	PPInst pp = NULL;
	PPResult ppRet;
	PPConfig pPpConf;
	u8 *pp_out_addr;
	u8 *pp_int_addr;

	/* enable MJPEG clock and low level driver */
	RCC_PeriphClockCmd(APBPeriph_MJPEG, APBPeriph_MJPEG_CLOCK, ENABLE);
	hx170dec_init();

	pp_int_addr = rtos_mem_malloc(YUV_DATA_WIDTH * YUV_DATA_HEIGHT * 1.5); // YUV 420 data length
	pp_out_addr = rtos_mem_malloc(YUV_DATA_WIDTH * YUV_DATA_HEIGHT * 1.5);

	_memcpy((void *) pp_int_addr, (void *)yuv_data, yuv_len);

	/* init pp instance */
	ppRet = PPInit(&pp);
	if (ppRet == PP_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "PPInit ok\n");
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "PPInit error: %d\n", ppRet);
		goto end;
	}

	/* init pp config */
	ppRet = PPGetConfig(pp, &pPpConf);
	if (ppRet != PP_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "PPGetConfig error: %d\n", ppRet);
		goto end;
	}

	pPpConf.ppInImg.width = YUV_DATA_WIDTH;
	pPpConf.ppInImg.height = YUV_DATA_HEIGHT;
	pPpConf.ppInImg.videoRange = 1;
	pPpConf.ppInImg.bufferBusAddr = (u32) pp_int_addr;
	/* Set the chrominance base address offset in a linear YCbCr 4:2:0 buffer.
	   Chrominance plane will be located directly after the luminance plane */
	pPpConf.ppInImg.bufferCbBusAddr = ((u32)pp_int_addr) + (YUV_DATA_WIDTH * YUV_DATA_HEIGHT);
	pPpConf.ppInImg.bufferCrBusAddr = pPpConf.ppInImg.bufferCbBusAddr + (YUV_DATA_WIDTH * YUV_DATA_HEIGHT) / 4;
	pPpConf.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_0_PLANAR;
	pPpConf.ppOutImg.bufferBusAddr = (u32)pp_out_addr;
	pPpConf.ppOutImg.pixFormat = PP_PIX_FMT_RGB32;
	pPpConf.ppOutRgb.rgbTransform = PP_YCBCR2RGB_TRANSFORM_BT_709;

	// set rotation paramter
	pPpConf.ppInRotation.rotation = PP_ROTATION_RIGHT_90;
	pPpConf.ppOutImg.width = YUV_DATA_HEIGHT;
	pPpConf.ppOutImg.height = YUV_DATA_WIDTH;

	/* set pp config */
	ppRet = PPSetConfig(pp, &pPpConf);
	if (ppRet != PP_OK) {
		/* Handle errors here */
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "PPSetConfig error: %d\n", ppRet);
		goto end;
	}

	/* Get result */
	ppRet = PPGetResult(pp);
	if (ppRet == PP_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "PPGetResult ok\n");
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "PPGetResult error: %d\n", ppRet);
		goto end;
	}

end:
	PPRelease(pp);
	rtos_mem_free((void *)pp_int_addr);
	rtos_mem_free((void *)pp_out_addr);
	rtos_task_delete(NULL);
}

int example_raw_pp_standalone_rotation(void)
{
	if (rtos_task_create(NULL, "RAW_PP_STANDALONE_ROTATION_TASK", (rtos_task_t)PP_standalone_rotation, NULL, (4096), (1)) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Cannot create RAW_PP_STANDALONE_ROTATION_TASK\n");
	}

	return 0;
}


