/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "basetype.h"
#include "dwl_defs.h"
#include "dwl.h"

static const char *const TAG = "DWL";
static u32 dwlShadowRegs[MAX_ASIC_CORES][154];
static unsigned int multicorebase[HXDEC_MAX_CORES] = {0xdeadbeef};
static hx170dec_t hx170dec_data; /* dynamic allocation? */
static rtos_mutex_t dec_lock;
static rtos_mutex_t pp_lock;
static rtos_sema_t pp_wait_sema;
static rtos_sema_t dec_wait_sema;
static int hx170_init_flag = 0;
static const int DecHwId[] = {
	0x8190,
	0x8170,
	0x9170,
	0x9190,
	0x6731,
	0x6e64
};

void PrintIrqType(u32 isPP, u32 coreID, u32 status)
{
	if (isPP) {
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "PP[%d] IRQ %s\n", coreID,
				 status & PP_IRQ_RDY ? "READY" : "BUS ERROR");
	} else {
		if (status & DEC_IRQ_ABORT) {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "DEC[%d] IRQ ABORT\n", coreID);
		} else if (status & DEC_IRQ_RDY) {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "DEC[%d] IRQ READY\n", coreID);
		} else if (status & DEC_IRQ_BUS) {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "DEC[%d] IRQ BUS ERROR\n", coreID);
		} else if (status & DEC_IRQ_BUFFER) {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "DEC[%d] IRQ BUFFER\n", coreID);
		} else if (status & DEC_IRQ_ASO) {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "DEC[%d] IRQ ASO\n", coreID);
		} else if (status & DEC_IRQ_ERROR) {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "DEC[%d] IRQ STREAM ERROR\n", coreID);
		} else if (status & DEC_IRQ_SLICE) {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "DEC[%d] IRQ SLICE\n", coreID);
		} else if (status & DEC_IRQ_TIMEOUT) {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "DEC[%d] IRQ TIMEOUT\n", coreID);
		} else {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "DEC[%d] IRQ UNKNOWN 0x%08x\n", coreID, status);
		}
	}
}

static void ReleaseDecoder(hx170dec_t *dev, int core)
{
	u32 status;
	u32 counter = 0;

	status = HAL_READ32(dev->hwregs[0], HX170_IRQ_STAT_DEC_OFF);

	/* make sure HW is disabled */
	if (status & HX170_DEC_E) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "DEC still enabled -> reset\n");

		while (status & HX170_DEC_E) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Killed, wait for HW finish\n");
			status = HAL_READ32(dev->hwregs[0], HX170_IRQ_STAT_DEC_OFF);
			DelayMs(1000);
			if (++counter > 5) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Killed, timeout\n", core);
				break;
			}
		}

		HAL_WRITE32(dev->hwregs[core], HX170_IRQ_STAT_DEC_OFF, 0);
	}

	rtos_mutex_give(dec_lock);
}

static void ReleasePostProcessor(hx170dec_t *dev, int core)
{
	u32 status = HAL_READ32(dev->hwregs[core], HX170_IRQ_STAT_PP_OFF);

	/* make sure HW is disabled */
	if (status & HX170_PP_E) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "PP still enabled -> reset\n");

		/* disable IRQ */
		status |= HX170_PP_IRQ_DISABLE;

		/* disable postprocessor */
		status &= (~HX170_PP_E);
		HAL_WRITE32(dev->hwregs[core], HX170_IRQ_STAT_PP_OFF, 0x10);
	}

	rtos_mutex_give(pp_lock);
}

static void ReadCoreFuse(const unsigned int *io, DWLHwFuseStatus_t *pHwFuseSts)
{
	u32 configReg, fuseReg, fuseRegPp;

	/* Decoder configuration */
	configReg = io[HX170DEC_SYNTH_CFG];

	/* Decoder fuse configuration */
	fuseReg = io[HX170DEC_FUSE_CFG];

	pHwFuseSts->h264SupportFuse = (fuseReg >> DWL_H264_FUSE_E) & 0x01U;
	pHwFuseSts->mpeg4SupportFuse = (fuseReg >> DWL_MPEG4_FUSE_E) & 0x01U;
	pHwFuseSts->mpeg2SupportFuse = (fuseReg >> DWL_MPEG2_FUSE_E) & 0x01U;
	pHwFuseSts->sorensonSparkSupportFuse =
		(fuseReg >> DWL_SORENSONSPARK_FUSE_E) & 0x01U;
	pHwFuseSts->jpegSupportFuse = (fuseReg >> DWL_JPEG_FUSE_E) & 0x01U;
	pHwFuseSts->vp6SupportFuse = (fuseReg >> DWL_VP6_FUSE_E) & 0x01U;
	pHwFuseSts->vc1SupportFuse = (fuseReg >> DWL_VC1_FUSE_E) & 0x01U;
	pHwFuseSts->jpegProgSupportFuse = (fuseReg >> DWL_PJPEG_FUSE_E) & 0x01U;
	pHwFuseSts->rvSupportFuse = (fuseReg >> DWL_RV_FUSE_E) & 0x01U;
	pHwFuseSts->avsSupportFuse = (fuseReg >> DWL_AVS_FUSE_E) & 0x01U;
	pHwFuseSts->vp7SupportFuse = (fuseReg >> DWL_VP7_FUSE_E) & 0x01U;
	pHwFuseSts->vp8SupportFuse = (fuseReg >> DWL_VP8_FUSE_E) & 0x01U;
	pHwFuseSts->customMpeg4SupportFuse = (fuseReg >> DWL_CUSTOM_MPEG4_FUSE_E) & 0x01U;
	pHwFuseSts->mvcSupportFuse = (fuseReg >> DWL_MVC_FUSE_E) & 0x01U;

	/* check max. decoder output width */
	if (fuseReg & 0x10000U) {
		pHwFuseSts->maxDecPicWidthFuse = 4096;
	} else if (fuseReg & 0x8000U) {
		pHwFuseSts->maxDecPicWidthFuse = 1920;
	} else if (fuseReg & 0x4000U) {
		pHwFuseSts->maxDecPicWidthFuse = 1280;
	} else if (fuseReg & 0x2000U) {
		pHwFuseSts->maxDecPicWidthFuse = 720;
	} else if (fuseReg & 0x1000U) {
		pHwFuseSts->maxDecPicWidthFuse = 352;
	}

	pHwFuseSts->refBufSupportFuse = (fuseReg >> DWL_REF_BUFF_FUSE_E) & 0x01U;

	/* Pp configuration */
	configReg = io[HX170PP_SYNTH_CFG];

	if ((configReg >> DWL_PP_E) & 0x01U) {
		/* Pp fuse configuration */
		fuseRegPp = io[HX170PP_FUSE_CFG];

		if ((fuseRegPp >> DWL_PP_FUSE_E) & 0x01U) {
			pHwFuseSts->ppSupportFuse = 1;

			/* check max. pp output width */
			if (fuseRegPp & 0x10000U) {
				pHwFuseSts->maxPpOutPicWidthFuse = 4096;
			} else if (fuseRegPp & 0x8000U) {
				pHwFuseSts->maxPpOutPicWidthFuse = 1920;
			} else if (fuseRegPp & 0x4000U) {
				pHwFuseSts->maxPpOutPicWidthFuse = 1280;
			} else if (fuseRegPp & 0x2000U) {
				pHwFuseSts->maxPpOutPicWidthFuse = 720;
			} else if (fuseRegPp & 0x1000U) {
				pHwFuseSts->maxPpOutPicWidthFuse = 352;
			}

			pHwFuseSts->ppConfigFuse = fuseRegPp;
		} else {
			pHwFuseSts->ppSupportFuse = 0;
			pHwFuseSts->maxPpOutPicWidthFuse = 0;
			pHwFuseSts->ppConfigFuse = 0;
		}
	}
}

static void ReadCoreConfig(const unsigned int *io, DWLHwConfig_t *pHwCfg)
{
	u32 configReg;
	const unsigned int asicID = io[0];

	/* Decoder configuration */
	configReg = io[HX170DEC_SYNTH_CFG];

	pHwCfg->h264Support = (configReg >> DWL_H264_E) & 0x3U;
	/* check jpeg */
	pHwCfg->jpegSupport = (configReg >> DWL_JPEG_E) & 0x01U;
	if (pHwCfg->jpegSupport && ((configReg >> DWL_PJPEG_E) & 0x01U)) {
		pHwCfg->jpegSupport = JPEG_PROGRESSIVE;
	}
	pHwCfg->mpeg4Support = (configReg >> DWL_MPEG4_E) & 0x3U;
	pHwCfg->vc1Support = (configReg >> DWL_VC1_E) & 0x3U;
	pHwCfg->mpeg2Support = (configReg >> DWL_MPEG2_E) & 0x01U;
	pHwCfg->sorensonSparkSupport = (configReg >> DWL_SORENSONSPARK_E) & 0x01U;
#ifndef DWL_REFBUFFER_DISABLE
	pHwCfg->refBufSupport = (configReg >> DWL_REF_BUFF_E) & 0x01U;
#else
	pHwCfg->refBufSupport = 0;
#endif
	pHwCfg->vp6Support = (configReg >> DWL_VP6_E) & 0x01U;
#ifdef DEC_X170_APF_DISABLE
	if (DEC_X170_APF_DISABLE) {
		pHwCfg->tiledModeSupport = 0;
	}
#endif /* DEC_X170_APF_DISABLE */

	pHwCfg->maxDecPicWidth = configReg & 0x07FFU;

	/* 2nd Config register */
	configReg = io[HX170DEC_SYNTH_CFG_2];
	if (pHwCfg->refBufSupport) {
		if ((configReg >> DWL_REF_BUFF_ILACE_E) & 0x01U) {
			pHwCfg->refBufSupport |= 2;
		}
		if ((configReg >> DWL_REF_BUFF_DOUBLE_E) & 0x01U) {
			pHwCfg->refBufSupport |= 4;
		}
	}

	pHwCfg->customMpeg4Support = (configReg >> DWL_MPEG4_CUSTOM_E) & 0x01U;
	pHwCfg->vp7Support = (configReg >> DWL_VP7_E) & 0x01U;
	pHwCfg->vp8Support = (configReg >> DWL_VP8_E) & 0x01U;
	pHwCfg->avsSupport = (configReg >> DWL_AVS_E) & 0x01U;

	/* JPEG extensions */
	if (((asicID >> 16) >= 0x8190U) || ((asicID >> 16) == 0x6731U) ||
		((asicID >> 16) == 0x6e64U)) {
		pHwCfg->jpegESupport = (configReg >> DWL_JPEG_EXT_E) & 0x01U;
	} else {
		pHwCfg->jpegESupport = JPEG_EXT_NOT_SUPPORTED;
	}

	if (((asicID >> 16) >= 0x9170U) || ((asicID >> 16) == 0x6731U) ||
		((asicID >> 16) == 0x6e64U)) {
		pHwCfg->rvSupport = (configReg >> DWL_RV_E) & 0x03U;
	} else {
		pHwCfg->rvSupport = RV_NOT_SUPPORTED;
	}

	pHwCfg->mvcSupport = (configReg >> DWL_MVC_E) & 0x03U;
	pHwCfg->webpSupport = (configReg >> DWL_WEBP_E) & 0x01U;
	pHwCfg->tiledModeSupport = (configReg >> DWL_DEC_TILED_L) & 0x03U;
	pHwCfg->maxDecPicWidth += ((configReg >> DWL_DEC_PIC_W_EXT) & 0x03U) << 11;

	pHwCfg->ecSupport = (configReg >> DWL_EC_E) & 0x03U;
	pHwCfg->strideSupport = (configReg >> DWL_STRIDE_E) & 0x01U;
	pHwCfg->fieldDpbSupport = (configReg >> DWL_FIELD_DPB_E) & 0x01U;
	pHwCfg->avsPlusSupport = (configReg >> DWL_AVS_PLUS_E) & 0x01U;
	pHwCfg->addr64Support = (configReg >> DWL_64BIT_ENV_E) & 0x01U;

	if (pHwCfg->refBufSupport && (((asicID >> 16) == 0x6731U) ||
								  ((asicID >> 16) == 0x6e64U))) {
		pHwCfg->refBufSupport |= 8; /* enable HW support for offset */
	}

	/* Pp configuration */
	configReg = io[HX170PP_SYNTH_CFG];

	if ((configReg >> DWL_PP_E) & 0x01U) {
		pHwCfg->ppSupport = 1;
		/* Theoretical max range 0...8191; actual 48...4096 */
		pHwCfg->maxPpOutPicWidth = configReg & 0x1FFFU;
		/*pHwCfg->ppConfig = (configReg >> DWL_CFG_E) & 0x0FU; */
		pHwCfg->ppConfig = configReg;
	} else {
		pHwCfg->ppSupport = 0;
		pHwCfg->maxPpOutPicWidth = 0;
		pHwCfg->ppConfig = 0;
	}

	/* check the HW version */
	if (((asicID >> 16) >= 0x8190U) || ((asicID >> 16) == 0x6731U) ||
		((asicID >> 16) == 0x6e64U)) {
		u32 deInterlace;
		u32 alphaBlend;
		u32 deInterlaceFuse;
		u32 alphaBlendFuse;
		DWLHwFuseStatus_t hwFuseSts;

		/* check fuse status */
		ReadCoreFuse(io, &hwFuseSts);

		/* Maximum decoding width supported by the HW */
		if (pHwCfg->maxDecPicWidth > hwFuseSts.maxDecPicWidthFuse) {
			pHwCfg->maxDecPicWidth = hwFuseSts.maxDecPicWidthFuse;
		}
		/* Maximum output width of Post-Processor */
		if (pHwCfg->maxPpOutPicWidth > hwFuseSts.maxPpOutPicWidthFuse) {
			pHwCfg->maxPpOutPicWidth = hwFuseSts.maxPpOutPicWidthFuse;
		}
		/* h264 */
		if (!hwFuseSts.h264SupportFuse) {
			pHwCfg->h264Support = H264_NOT_SUPPORTED;
		}
		/* mpeg-4 */
		if (!hwFuseSts.mpeg4SupportFuse) {
			pHwCfg->mpeg4Support = MPEG4_NOT_SUPPORTED;
		}
		/* custom mpeg-4 */
		if (!hwFuseSts.customMpeg4SupportFuse) {
			pHwCfg->customMpeg4Support = MPEG4_CUSTOM_NOT_SUPPORTED;
		}
		/* jpeg (baseline && progressive) */
		if (!hwFuseSts.jpegSupportFuse) {
			pHwCfg->jpegSupport = JPEG_NOT_SUPPORTED;
		}
		if ((pHwCfg->jpegSupport == JPEG_PROGRESSIVE) &&
			!hwFuseSts.jpegProgSupportFuse) {
			pHwCfg->jpegSupport = JPEG_BASELINE;
		}
		/* mpeg-2 */
		if (!hwFuseSts.mpeg2SupportFuse) {
			pHwCfg->mpeg2Support = MPEG2_NOT_SUPPORTED;
		}
		/* vc-1 */
		if (!hwFuseSts.vc1SupportFuse) {
			pHwCfg->vc1Support = VC1_NOT_SUPPORTED;
		}
		/* vp6 */
		if (!hwFuseSts.vp6SupportFuse) {
			pHwCfg->vp6Support = VP6_NOT_SUPPORTED;
		}
		/* vp7 */
		if (!hwFuseSts.vp7SupportFuse) {
			pHwCfg->vp7Support = VP7_NOT_SUPPORTED;
		}
		/* vp8 */
		if (!hwFuseSts.vp8SupportFuse) {
			pHwCfg->vp8Support = VP8_NOT_SUPPORTED;
		}
		/* webp */
		if (!hwFuseSts.vp8SupportFuse) {
			pHwCfg->webpSupport = WEBP_NOT_SUPPORTED;
		}
		/* pp */
		if (!hwFuseSts.ppSupportFuse) {
			pHwCfg->ppSupport = PP_NOT_SUPPORTED;
		}
		/* check the pp config vs fuse status */
		if ((pHwCfg->ppConfig & 0xFC000000) &&
			((hwFuseSts.ppConfigFuse & 0xF0000000) >> 5)) {
			/* config */
			deInterlace = ((pHwCfg->ppConfig & PP_DEINTERLACING) >> 25);
			alphaBlend = ((pHwCfg->ppConfig & PP_ALPHA_BLENDING) >> 24);
			/* fuse */
			deInterlaceFuse =
				(((hwFuseSts.ppConfigFuse >> 5) & PP_DEINTERLACING) >> 25);
			alphaBlendFuse =
				(((hwFuseSts.ppConfigFuse >> 5) & PP_ALPHA_BLENDING) >> 24);

			/* check if */
			if (deInterlace && !deInterlaceFuse) {
				pHwCfg->ppConfig &= 0xFD000000;
			}
			if (alphaBlend && !alphaBlendFuse) {
				pHwCfg->ppConfig &= 0xFE000000;
			}
		}
		/* sorenson */
		if (!hwFuseSts.sorensonSparkSupportFuse) {
			pHwCfg->sorensonSparkSupport = SORENSON_SPARK_NOT_SUPPORTED;
		}
		/* ref. picture buffer */
		if (!hwFuseSts.refBufSupportFuse) {
			pHwCfg->refBufSupport = REF_BUF_NOT_SUPPORTED;
		}

		/* rv */
		if (!hwFuseSts.rvSupportFuse) {
			pHwCfg->rvSupport = RV_NOT_SUPPORTED;
		}
		/* avs */
		if (!hwFuseSts.avsSupportFuse) {
			pHwCfg->avsSupport = AVS_NOT_SUPPORTED;
		}
		/* mvc */
		if (!hwFuseSts.mvcSupportFuse) {
			pHwCfg->mvcSupport = MVC_NOT_SUPPORTED;
		}
	}
}

static void ResetAsic(hx170dec_t *dev)
{
	u32 i;
	int j;
	u32 status;

	for (j = 0; j < dev->cores; j++) {
		status = HAL_READ32(dev->hwregs[j], HX170_IRQ_STAT_DEC_OFF);

		if (status & HX170_DEC_E) {
			/* abort with IRQ disabled */
			status = HX170_DEC_ABORT | HX170_DEC_IRQ_DISABLE;
			HAL_WRITE32(dev->hwregs[j], HX170_IRQ_STAT_DEC_OFF, status);
		}

		/* reset PP */
		HAL_WRITE32(dev->hwregs[j], HX170_IRQ_STAT_PP_OFF, 0);

		for (i = 4; i < dev->iosize; i += 4) {
			HAL_WRITE32(dev->hwregs[j], i, 0);
		}
	}
}

void dump_regs(hx170dec_t *dev)
{
	u32 i;
	int c;

	RTK_LOGS(TAG, RTK_LOG_INFO, "Reg Dump Start\n");
	for (c = 0; c < dev->cores; c++) {
		for (i = 0; i < dev->iosize; i += 4 * 4) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "\toffset %04X: %08X  %08X  %08X  %08X\n", i,
					 HAL_READ32(dev->hwregs[c], i),
					 HAL_READ32(dev->hwregs[c], i + 4),
					 HAL_READ32(dev->hwregs[c], i + 16),
					 HAL_READ32(dev->hwregs[c], i + 24));
		}
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "Reg Dump End\n");
}

static int CheckHwId(hx170dec_t *dev)
{
	int hwid;
	int i;
	size_t numHw = sizeof(DecHwId) / sizeof(*DecHwId);

	int found = 0;

	for (i = 0; i < dev->cores; i++) {
		if (dev->hwregs[i] != NULL) {
			hwid = HAL_READ32(dev->hwregs[i], 0);
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "hx170dec: Core %d HW ID=0x%08lx\n", i, hwid);
			hwid = (hwid >> 16) & 0xFFFF; /* product version only */

			while (numHw--) {
				if (hwid == DecHwId[numHw]) {
					RTK_LOGS(TAG, RTK_LOG_DEBUG, "hx170dec: Supported HW found at 0x%16lx\n",
							 multicorebase[i]);
					found++;
					break;
				}
			}
			if (!found) {
				RTK_LOGS(TAG, RTK_LOG_DEBUG, "hx170dec: Unknown HW found at 0x%16lx\n",
						 multicorebase[i]);
				return DWL_OK;
			}
			found = 0;
			numHw = sizeof(DecHwId) / sizeof(*DecHwId);
		}
	}

	return 1;
}

static int SetIO(void)
{
	if (multicorebase[0] != 0xdeadbeef) {
		hx170dec_data.hwregs[0] = (u8 *)multicorebase[0];
	}

	if (hx170dec_data.hwregs[0] == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "hx170dec: failed to ioremap HW regs\n");
		return DWL_ERROR;
	}

	hx170dec_data.cores = 1;
	/* check for correct HW */
	if (!CheckHwId(&hx170dec_data)) {
		return DWL_ERROR;
	}
	return DWL_OK;
}

u32 hx170dec_isr(void *dev_id)
{
	unsigned int handled = 0;
	int i;
	volatile u8 *hwregs;
	hx170dec_t *dev = (hx170dec_t *) dev_id;
	u32 irq_status_dec;
	u32 irq_status_pp;

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "hx170dec_isr\n");

	for (i = 0; i < dev->cores; i++) {
		volatile u8 *hwregs = dev->hwregs[i];

		/* interrupt status register read */
		irq_status_dec = HAL_READ32(hwregs, HX170_IRQ_STAT_DEC_OFF);

		if (irq_status_dec & HX170_DEC_IRQ) {
			/* clear dec IRQ */
			irq_status_dec &= (~HX170_DEC_IRQ);
			HAL_WRITE32(hwregs, HX170_IRQ_STAT_DEC_OFF, irq_status_dec);

			RTK_LOGS(TAG, RTK_LOG_DEBUG, "decoder IRQ received! core %d\n", i);

			rtos_sema_give(dec_wait_sema);
			handled++;
		}
	}

	/* check PP also */
	hwregs = dev->hwregs[0];
	irq_status_pp = HAL_READ32(hwregs, HX170_IRQ_STAT_PP_OFF);

	if (irq_status_pp & HX170_PP_IRQ) {
		/* clear pp IRQ */
		irq_status_pp &= (~HX170_PP_IRQ);
		HAL_WRITE32(hwregs, HX170_IRQ_STAT_PP_OFF, irq_status_pp);

		RTK_LOGS(TAG, RTK_LOG_DEBUG, "post-processor IRQ received!\n");

		rtos_sema_give(pp_wait_sema);
		handled++;
	}

	if (!handled) {
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "IRQ received, but not x170's!\n");
	}

	return DWL_OK;
}

int hx170dec_init(void)
{
	int result;

	if (hx170_init_flag != 0) {
		return DWL_OK;
	}

	hx170_init_flag = 1;

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "hx170dec init\n");

	multicorebase[0] = MJPEG_REG_BASE;

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "hx170dec: Init single core at 0x%16lx IRQ=%i\n",
			 multicorebase[0], DEC_IRQ);

	hx170dec_data.iosize = DEC_IO_SIZE;
	hx170dec_data.irq = DEC_IRQ;
	hx170dec_data.hwregs[0] = 0;

	result = SetIO();
	if (result < 0) {
		goto err;
	}

	rtos_mutex_create(&dec_lock);
	rtos_mutex_create(&pp_lock);
	rtos_sema_create(&dec_wait_sema, 0, RTOS_SEMA_MAX_COUNT);
	rtos_sema_create(&pp_wait_sema, 0, RTOS_SEMA_MAX_COUNT);

	/* reset hardware */
	ResetAsic(&hx170dec_data);

	InterruptRegister((IRQ_FUN)hx170dec_isr, DEC_IRQ, (u32) &hx170dec_data, 3);
	InterruptEn(DEC_IRQ, 3);

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "hx170dec init OK\n");

	return DWL_OK;
err:
	RTK_LOGS(TAG, RTK_LOG_ERROR, "hx170dec init fail\n");
	hx170_init_flag = 0;
	return result;
}

void hx170dec_deinit(void)
{
	hx170dec_t *dev = &hx170dec_data;

	if (hx170_init_flag == 0) {
		return;
	}

	hx170_init_flag = 0;

	/* reset hardware */
	ResetAsic(dev);

	/* free the IRQ */
	if (dev->irq != -1) {
		InterruptUnRegister(dev->irq);
	}

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "hx170dec deinit\n");
}

static int DecFlushRegs(hx170dec_t *dev, struct core_desc *core)
{
	int i;
	u32 id = core->id;

	/* write dec regs but the status reg[1] to hardware */
	/* both original and extended regs need to be written */
	for (i = 2; i <= HANTRO_DEC_ORG_LAST_REG; i++) {
		HAL_WRITE32(dev->hwregs[id], i * 4,  core->regs[i]);
	}

	/* write the status register, which may start the decoder */
	HAL_WRITE32(dev->hwregs[id], 4, core->regs[1]);

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "flushed registers on core %d\n", id);

	return DWL_OK;
}

static int DecRefreshRegs(hx170dec_t *dev, struct core_desc *core)
{
	int i;
	u32 id = core->id;

	/* user has to know exactly what they are asking for */
	if (core->size != (HANTRO_DEC_ORG_REGS * 4)) {
		return -DWL_ERROR;
	}

	/* read all registers from hardware */
	/* both original and extended regs need to be read */
	for (i = 0; i <= HANTRO_DEC_ORG_LAST_REG; i++) {
		core->regs[i] = HAL_READ32(dev->hwregs[id], i * 4);
	}

	return DWL_OK;
}

static int PPFlushRegs(hx170dec_t *dev, struct core_desc *core)
{
	u32 id = core->id;
	u32 i;

	/* write all regs but the status reg[1] to hardware */
	/* both original and extended regs need to be written */
	for (i = HANTRO_PP_ORG_FIRST_REG + 1; i <= HANTRO_PP_ORG_LAST_REG; i++) {
		HAL_WRITE32(dev->hwregs[id], i * 4, core->regs[i]);
	}

	/* write the stat reg, which may start the PP */
	HAL_WRITE32(
		dev->hwregs[id], HANTRO_PP_ORG_FIRST_REG * 4, core->regs[HANTRO_PP_ORG_FIRST_REG]);

	return DWL_OK;
}

static int PPRefreshRegs(hx170dec_t *dev, struct core_desc *core)
{
	int i;
	u32 id = core->id;

	/* user has to know exactly what they are asking for */
	if (core->size != (HANTRO_PP_ORG_REGS * 4)) {
		return DWL_ERROR;
	}

	/* read all registers from hardware */
	/* both original and extended regs need to be read */
	for (i = HANTRO_PP_ORG_FIRST_REG; i <= HANTRO_PP_ORG_LAST_REG; i++) {
		core->regs[i] = HAL_READ32(dev->hwregs[id], i * 4);
	}

	return DWL_OK;
}

int WaitPPReadyAndRefreshRegs(hx170dec_t *dev, struct core_desc *core)
{
	u32 id = core->id;

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "wait_event_interruptible PP[%d]\n", id);

	rtos_sema_take(pp_wait_sema, RTOS_MAX_TIMEOUT);

	/* refresh registers */
	return PPRefreshRegs(dev, core);
}

int WaitDecReadyAndRefreshRegs(hx170dec_t *dev, struct core_desc *core)
{
	u32 id = core->id;

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "wait_event_interruptible DEC[%d]\n", id);
	rtos_sema_take(dec_wait_sema, RTOS_MAX_TIMEOUT);

	/* refresh registers */
	return DecRefreshRegs(dev, core);
}

/*------------------------------------------------------------------------------
    Function name   : DWLInit
    Description     : Initialize a DWL instance

    Return type     : const void * - pointer to a DWL instance

    Argument        : void * param - not in use, application passes NULL
------------------------------------------------------------------------------*/
const void *DWLInit(DWLInitParam_t *param)
{
	hX170dwl_t *dec_dwl;
	dec_dwl = (hX170dwl_t *) rtos_mem_calloc(1, sizeof(hX170dwl_t));
	dec_dwl->numCores = HXDEC_MAX_CORES;

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "DWLInit start\n");

	if (dec_dwl == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "failed to alloc hX170dwl_t struct\n");
		return NULL;
	}

	dec_dwl->clientType = param->clientType;

	switch (dec_dwl->clientType) {
	case DWL_CLIENT_TYPE_JPEG_DEC:
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "JPEG type\n");
		break;
	case DWL_CLIENT_TYPE_PP:
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "PP type\n");
		break;
	default:
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Unknown client type no. %d\n", dec_dwl->clientType);
		goto err;
	}

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "DWLInit SUCCESS\n");

	return dec_dwl;
err:
	RTK_LOGS(TAG, RTK_LOG_ERROR, "DWLInit FAILED\n");
	DWLRelease(dec_dwl);

	return NULL;
}

/*------------------------------------------------------------------------------
    Function name   : DWLRelease
    Description     : Release a DWl instance

    Return type     : i32 - 0 for success or a negative error code

    Argument        : const void * instance - instance to be released
------------------------------------------------------------------------------*/
i32 DWLRelease(const void *instance)
{
	hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;

	assert_param(dec_dwl != NULL);

	rtos_mem_free(dec_dwl);

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "DWLRelease SUCCESS\n");

	return (DWL_OK);
}

/*------------------------------------------------------------------------------
    Function name   : DWLReserveHwPipe
    Description     :
    Return type     : i32
    Argument        : const void *instance
    Argument        : i32 *coreID - ID of the reserved HW core
------------------------------------------------------------------------------*/
i32 DWLReserveHwPipe(const void *instance, i32 *coreID)
{
	hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;
	UNUSED(coreID);

	assert_param(dec_dwl != NULL);
	assert_param(dec_dwl->clientType != DWL_CLIENT_TYPE_PP);

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Try Reserve Pipe\n");

	rtos_mutex_take(dec_lock, RTOS_MAX_TIMEOUT);
	/* reserve PP */
	rtos_mutex_take(pp_lock, RTOS_MAX_TIMEOUT);

	dec_dwl->bPPReserved = 1;

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Reserved Pipe\n");

	return DWL_OK;
}

/*------------------------------------------------------------------------------
    Function name   : DWLReserveHw
    Description     :
    Return type     : i32
    Argument        : const void *instance
    Argument        : i32 *coreID - ID of the reserved HW core
------------------------------------------------------------------------------*/
i32 DWLReserveHw(const void *instance, i32 *coreID)
{
	hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;
	int isPP;
	UNUSED(coreID);

	assert_param(dec_dwl != NULL);

	isPP = dec_dwl->clientType == DWL_CLIENT_TYPE_PP ? 1 : 0;

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Try reserve %s\n", isPP ? "PP" : "DEC");

	if (isPP) {
		rtos_mutex_take(pp_lock, RTOS_MAX_TIMEOUT);
	} else {
		rtos_mutex_take(dec_lock, RTOS_MAX_TIMEOUT);
	}

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Reserved %s\n", isPP ? "PP" : "DEC");

	return DWL_OK;
}

/*------------------------------------------------------------------------------
    Function name   : DWLReleaseHw
    Description     :
    Return type     : void
    Argument        : const void *instance
------------------------------------------------------------------------------*/
void DWLReleaseHw(const void *instance, i32 coreID)
{
	hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;
	int isPP;
	UNUSED(coreID);

	assert_param((u32)coreID < dec_dwl->numCores);
	assert_param(dec_dwl != NULL);

	isPP = dec_dwl->clientType == DWL_CLIENT_TYPE_PP ? 1 : 0;

	if ((u32) coreID >= dec_dwl->numCores) {
		return;
	}

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Release %s\n", isPP ? "PP" : "DEC");

	if (isPP) {
		assert_param(coreID == 0);

		ReleasePostProcessor(&hx170dec_data, coreID);
	} else {
		if (dec_dwl->bPPReserved) {
			/* decoder has reserved PP also => release it */
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "DEC released PP core %d\n", coreID);

			dec_dwl->bPPReserved = 0;

			assert_param(coreID == 0);

			ReleasePostProcessor(&hx170dec_data, coreID);
		}

		ReleaseDecoder(&hx170dec_data, coreID);
	}
}

/*------------------------------------------------------------------------------
    Function name   : DWLReadAsicCoreCount
    Description     : Return the number of hardware cores available
------------------------------------------------------------------------------*/
u32 DWLReadAsicCoreCount(void)
{
	return hx170dec_data.cores;
}

/*------------------------------------------------------------------------------
    Function name   : DWLReadAsicID
    Description     : Read the HW ID. Does not need a DWL instance to run

    Return type     : u32 - the HW ID
------------------------------------------------------------------------------*/
u32 DWLReadAsicID()
{
	u32 id = ~0;
	unsigned int *base;

	base = (unsigned int *) multicorebase[0];
	id =  base[0];

	return id;
}

/*------------------------------------------------------------------------------
    Function name   : DWLReadAsicConfig
    Description     : Read HW configuration. Does not need a DWL instance to run

    Return type     : DWLHwConfig_t - structure with HW configuration
------------------------------------------------------------------------------*/
void DWLReadAsicConfig(DWLHwConfig_t *pHwCfg)
{
	const unsigned int *io = NULL;
	unsigned int base;

	base = multicorebase[0];
	io = (unsigned int *) base;

	/* Decoder configuration */
	_memset(pHwCfg, 0, sizeof(*pHwCfg));

	ReadCoreConfig(io, pHwCfg);
}

/*------------------------------------------------------------------------------
    Function name   : DWLMallocRefFrm
    Description     : Allocate a frame buffer (contiguous linear RAM memory)

    Return type     : i32 - 0 for success or a negative error code

    Argument        : const void * instance - DWL instance
    Argument        : u32 size - size in bytes of the requested memory
    Argument        : void *info - place where the allocated memory buffer
                        parameters are returned
------------------------------------------------------------------------------*/
i32 DWLMallocRefFrm(const void *instance, u32 size, DWLLinearMem_t *info)
{
	return DWLMallocLinear(instance, size, info);
}

/*------------------------------------------------------------------------------
    Function name   : DWLFreeRefFrm
    Description     : Release a frame buffer previously allocated with
                        DWLMallocRefFrm.

    Return type     : void

    Argument        : const void * instance - DWL instance
    Argument        : void *info - frame buffer memory information
------------------------------------------------------------------------------*/
void DWLFreeRefFrm(const void *instance, DWLLinearMem_t *info)
{
	DWLFreeLinear(instance, info);
}

/*------------------------------------------------------------------------------
    Function name   : DWLMallocLinear
    Description     : Allocate a contiguous, linear RAM  memory buffer

    Return type     : i32 - 0 for success or a negative error code

    Argument        : const void * instance - DWL instance
    Argument        : u32 size - size in bytes of the requested memory
    Argument        : void *info - place where the allocated memory buffer
                        parameters are returned
------------------------------------------------------------------------------*/
i32 DWLMallocLinear(const void *instance, u32 size, DWLLinearMem_t *info)
{
	UNUSED(instance);

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Malloc Linear\t%8d bytes\n", size);

	info->size = size;
	info->virtualAddress = rtos_mem_malloc(size);
	info->busAddress = (u32)info->virtualAddress;

	if (info->virtualAddress == NULL) {
		return DWL_ERROR;
	}

	return DWL_OK;
}

/*------------------------------------------------------------------------------
    Function name   : DWLFreeLinear
    Description     : Release a linera memory buffer, previously allocated with
                        DWLMallocLinear.

    Return type     : void

    Argument        : const void * instance - DWL instance
    Argument        : void *info - linear buffer memory information
------------------------------------------------------------------------------*/
void DWLFreeLinear(const void *instance, DWLLinearMem_t *info)
{
	UNUSED(instance);
	rtos_mem_free(info->virtualAddress);
	info->size = 0;
}

/*------------------------------------------------------------------------------
    Function name   : DWLWriteReg
    Description     : Write a value to a hardware IO register

    Return type     : void

    Argument        : const void * instance - DWL instance
    Argument        : u32 offset - byte offset of the register to be written
    Argument        : u32 value - value to be written out
------------------------------------------------------------------------------*/

void DWLWriteReg(const void *instance, i32 coreID, u32 offset, u32 value)
{
	hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;

	assert_param(dec_dwl != NULL);
	assert_param(coreID < (i32)dec_dwl->numCores);

	offset = offset / 4;

	dwlShadowRegs[coreID][offset] = value;

	(void)dec_dwl;
}

/*------------------------------------------------------------------------------
    Function name   : DWLReadReg
    Description     : Read the value of a hardware IO register

    Return type     : u32 - the value stored in the register

    Argument        : const void * instance - DWL instance
    Argument        : u32 offset - byte offset of the register to be read
------------------------------------------------------------------------------*/
u32 DWLReadReg(const void *instance, i32 coreID, u32 offset)
{
	hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;
	u32 val;

	assert_param(dec_dwl != NULL);
	/*some MSB regs added to support 64bit address accress,
	  so this assert_param should be ingored */
	assert_param(coreID < (i32)dec_dwl->numCores);

	offset = offset / 4;

	val = dwlShadowRegs[coreID][offset];

	(void)dec_dwl;
	return val;
}

/*------------------------------------------------------------------------------
    Function name   : DWLEnableHW
    Description     : Enable hw by writing to register
    Return type     : void
    Argument        : const void * instance - DWL instance
    Argument        : u32 offset - byte offset of the register to be written
    Argument        : u32 value - value to be written out
------------------------------------------------------------------------------*/
void DWLEnableHW(const void *instance, i32 coreID, u32 offset, u32 value)
{
	hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;
	struct core_desc core;
	int isPP;

	assert_param(dec_dwl);

	isPP = dec_dwl->clientType == DWL_CLIENT_TYPE_PP ? 1 : 0;

	DWLWriteReg(dec_dwl, coreID, offset, value);

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "%s enabled by previous DWLWriteReg\n",
			 isPP ? "PP" : "DEC");

	core.id = coreID;
	core.regs = dwlShadowRegs[coreID];
	core.size = isPP ? 41 * 4 : 60 * 4;

	if (isPP) {
		PPFlushRegs(&hx170dec_data, &core);
	} else {
		DecFlushRegs(&hx170dec_data, &core);
	}
}

/*------------------------------------------------------------------------------
    Function name   : DWLDisableHW
    Description     : Disable hw by writing to register
    Return type     : void
    Argument        : const void * instance - DWL instance
    Argument        : u32 offset - byte offset of the register to be written
    Argument        : u32 value - value to be written out
------------------------------------------------------------------------------*/
void DWLDisableHW(const void *instance, i32 coreID, u32 offset, u32 value)
{
	hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;
	struct core_desc core;
	int isPP;

	assert_param(dec_dwl);

	isPP = dec_dwl->clientType == DWL_CLIENT_TYPE_PP ? 1 : 0;

	DWLWriteReg(dec_dwl, coreID, offset, value);

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "%s %d disabled by previous DWLWriteReg\n",
			 isPP ? "PP" : "DEC", coreID);

	core.id = coreID;
	core.regs = dwlShadowRegs[coreID];
	core.size = isPP ? 41 * 4 : 60 * 4;

	if (isPP) {
		PPRefreshRegs(&hx170dec_data, &core);
	} else {
		DecRefreshRegs(&hx170dec_data, &core);
	}
}

/*------------------------------------------------------------------------------
    Function name   : DWLWaitHwReady
    Description     : Wait until hardware has stopped running.
                      Used for synchronizing software runs with the hardware.
                      The wait could succed, timeout, or fail with an error.

    Return type     : i32 - one of the values DWL_HW_WAIT_OK
                                              DWL_HW_WAIT_TIMEOUT
                                              DWL_HW_WAIT_ERROR

    Argument        : const void * instance - DWL instance
------------------------------------------------------------------------------*/
i32 DWLWaitHwReady(const void *instance, i32 coreID, u32 timeout)
{
	const hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;
	struct core_desc core;
	int isPP;
	i32 ret = DWL_HW_WAIT_OK;
	UNUSED(timeout);
#ifndef DWL_USE_DEC_IRQ
	int max_wait_time = 5000; /* 5s in ms */
#endif

	assert_param(dec_dwl);

	isPP = dec_dwl->clientType == DWL_CLIENT_TYPE_PP ? 1 : 0;

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "%s %d\n", isPP ? "PP" : "DEC", coreID);

	core.id = coreID;
	core.regs = dwlShadowRegs[coreID];
	core.size = isPP ? 41 * 4 : 60 * 4;

#ifdef DWL_USE_DEC_IRQ
	if (isPP) {
		if (WaitPPReadyAndRefreshRegs(&hx170dec_data, &core)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "ioctl HX170DEC_IOCG_*_WAIT failed\n");
			ret = DWL_HW_WAIT_ERROR;
		}
	} else {
		if (WaitDecReadyAndRefreshRegs(&hx170dec_data, &core)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "ioctl HX170DEC_IOCG_*_WAIT failed\n");
			ret = DWL_HW_WAIT_ERROR;
		}
	}
#else /* Polling */

	ret = DWL_HW_WAIT_TIMEOUT;

	do {
		u32 irq_stats;
		const unsigned int usec = 1000; /* 1 ms polling interval */

		if (isPP) {
			PPRefreshRegs(&hx170dec_data, &core);
		} else {
			DecRefreshRegs(&hx170dec_data, &core);
		}

		irq_stats = isPP ? dwlShadowRegs[coreID][HX170_IRQ_STAT_PP] :
					dwlShadowRegs[coreID][HX170_IRQ_STAT_DEC];

		irq_stats = (irq_stats >> 11) & 0xFF;

		if (irq_stats != 0) {
			ret = DWL_HW_WAIT_OK;
			break;
		}

		rtos_time_delay_us(usec);

		max_wait_time--;
	} while (max_wait_time > 0);
#endif

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "%s done\n", isPP ? "PP" : "DEC");
	return ret;
}

/*------------------------------------------------------------------------------
    Function name   : DWLmalloc
    Description     : Allocate a memory block. Same functionality as
                      the ANSI C malloc()

    Return type     : void pointer to the allocated space, or NULL if there
                      is insufficient memory available

    Argument        : u32 n - Bytes to allocate
------------------------------------------------------------------------------*/
void *DWLmalloc(u32 n)
{
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "DWLmalloc\t%8d bytes\n", n);
	return rtos_mem_malloc((size_t) n);
}

/*------------------------------------------------------------------------------
    Function name   : DWLfree
    Description     : Deallocates or frees a memory block. Same functionality as
                      the ANSI C free()

    Return type     : void

    Argument        : void *p - Previously allocated memory block to be freed
------------------------------------------------------------------------------*/
void DWLfree(void *p)
{
	if (p != NULL) {
		rtos_mem_free(p);
	}
}

/*------------------------------------------------------------------------------
    Function name   : DWLcalloc
    Description     : Allocates an array in memory with elements initialized
                      to 0. Same functionality as the ANSI C calloc()

    Return type     : void pointer to the allocated space, or NULL if there
                      is insufficient memory available

}
    Argument        : u32 n - Number of elements
    Argument        : u32 s - Length in bytes of each element.
------------------------------------------------------------------------------*/
void *DWLcalloc(u32 n, u32 s)
{
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "DWLcalloc\t%8d bytes\n", n * s);
	void *p = NULL;

	p = rtos_mem_calloc((n), (s));

	return p;
}

/*------------------------------------------------------------------------------
    Function name   : DWLmemcpy
    Description     : Copies characters between buffers. Same functionality as
                      the ANSI C memcpy()

    Return type     : The value of destination d

    Argument        : void *d - Destination buffer
    Argument        : const void *s - Buffer to copy from
    Argument        : u32 n - Number of bytes to copy
------------------------------------------------------------------------------*/
void *DWLmemcpy(void *d, const void *s, u32 n)
{
	return _memcpy(d, s, (size_t) n);
}

void *DWLmemset(void *d, i32 c, u32 n)
{
	return _memset(d, (int) c, (size_t) n);
}

