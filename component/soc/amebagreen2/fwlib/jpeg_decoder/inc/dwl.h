/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __DWL_H__
#define __DWL_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "basetype.h"
#include "decapicommon.h"


#define DWL_OK                      0
#define DWL_ERROR                  -1

#define DWL_HW_WAIT_OK              DWL_OK
#define DWL_HW_WAIT_ERROR           DWL_ERROR
#define DWL_HW_WAIT_TIMEOUT         1

#define DWL_CLIENT_TYPE_H264_DEC         1U
#define DWL_CLIENT_TYPE_MPEG4_DEC        2U
#define DWL_CLIENT_TYPE_JPEG_DEC         3U
#define DWL_CLIENT_TYPE_PP               4U
#define DWL_CLIENT_TYPE_VC1_DEC          5U
#define DWL_CLIENT_TYPE_MPEG2_DEC        6U
#define DWL_CLIENT_TYPE_VP6_DEC          7U
#define DWL_CLIENT_TYPE_AVS_DEC          8U
#define DWL_CLIENT_TYPE_RV_DEC           9U
#define DWL_CLIENT_TYPE_VP8_DEC          10U

/* Linear memory area descriptor */
typedef struct DWLLinearMem {
	u32 *virtualAddress;
	g1_addr_t busAddress;
	u32 size;
} DWLLinearMem_t;

/* DWLInitParam is used to pass parameters when initializing the DWL */
typedef struct DWLInitParam {
	u32 clientType;
} DWLInitParam_t;

/* Hardware configuration description, same as in top API */
typedef struct DecHwConfig_ DWLHwConfig_t;

typedef struct DWLHwFuseStatus {
	u32 h264SupportFuse;     /* HW supports h.264 */
	u32 mpeg4SupportFuse;    /* HW supports MPEG-4 */
	u32 mpeg2SupportFuse;    /* HW supports MPEG-2 */
	u32 sorensonSparkSupportFuse;   /* HW supports Sorenson Spark */
	u32 jpegSupportFuse;     /* HW supports JPEG */
	u32 vp6SupportFuse;      /* HW supports VP6 */
	u32 vp7SupportFuse;      /* HW supports VP6 */
	u32 vp8SupportFuse;      /* HW supports VP6 */
	u32 vc1SupportFuse;      /* HW supports VC-1 Simple */
	u32 jpegProgSupportFuse; /* HW supports Progressive JPEG */
	u32 ppSupportFuse;       /* HW supports post-processor */
	u32 ppConfigFuse;        /* HW post-processor functions bitmask */
	u32 maxDecPicWidthFuse;  /* Maximum video decoding width supported  */
	u32 maxPpOutPicWidthFuse; /* Maximum output width of Post-Processor */
	u32 refBufSupportFuse;   /* HW supports reference picture buffering */
	u32 avsSupportFuse;      /* one of the AVS values defined above */
	u32 rvSupportFuse;       /* one of the REAL values defined above */
	u32 mvcSupportFuse;
	u32 customMpeg4SupportFuse; /* Fuse for custom MPEG-4 */
} DWLHwFuseStatus_t;

/* HW ID retrieving, static implementation */
u32 DWLReadAsicID(void);

/* HW configuration retrieving, static implementation */
void DWLReadAsicConfig(DWLHwConfig_t *pHwCfg);

/* Return number of ASIC cores, static implementation */
u32 DWLReadAsicCoreCount(void);

/* DWL initialization and release */
const void *DWLInit(DWLInitParam_t *param);
i32 DWLRelease(const void *instance);

/* HW sharing */
i32 DWLReserveHw(const void *instance, i32 *coreID);
i32 DWLReserveHwPipe(const void *instance, i32 *coreID);
void DWLReleaseHw(const void *instance, i32 coreID);

/* Frame buffers memory */
i32 DWLMallocRefFrm(const void *instance, u32 size, DWLLinearMem_t *info);
void DWLFreeRefFrm(const void *instance, DWLLinearMem_t *info);

/* SW/HW shared memory */
i32 DWLMallocLinear(const void *instance, u32 size, DWLLinearMem_t *info);
void DWLFreeLinear(const void *instance, DWLLinearMem_t *info);

/* D-Cache coherence */
void DWLDCacheRangeFlush(const void *instance, DWLLinearMem_t *info);   /* NOT in use */
void DWLDCacheRangeRefresh(const void *instance, DWLLinearMem_t *info);     /* NOT in use */

/* Register access */
void DWLWriteReg(const void *instance, i32 coreID, u32 offset, u32 value);
u32 DWLReadReg(const void *instance, i32 coreID, u32 offset);

void DWLWriteRegAll(const void *instance, const u32 *table, u32 size);  /* NOT in use */
void DWLReadRegAll(const void *instance, u32 *table, u32 size);     /* NOT in use */

/* HW starting/stopping */
void DWLEnableHW(const void *instance, i32 coreID, u32 offset, u32 value);
void DWLDisableHW(const void *instance, i32 coreID, u32 offset, u32 value);

/* HW synchronization */
i32 DWLWaitHwReady(const void *instance, i32 coreID, u32 timeout);

typedef void DWLIRQCallbackFn(void *arg, i32 coreID);

void DWLSetIRQCallback(const void *instance, i32 coreID,
					   DWLIRQCallbackFn *pCallbackFn, void *arg);

/* SW/SW shared memory */
void *DWLmalloc(u32 n);
void DWLfree(void *p);
void *DWLcalloc(u32 n, u32 s);
void *DWLmemcpy(void *d, const void *s, u32 n);
void *DWLmemset(void *d, i32 c, u32 n);

#ifdef __cplusplus
}
#endif

#endif                       /* __DWL_H__ */
