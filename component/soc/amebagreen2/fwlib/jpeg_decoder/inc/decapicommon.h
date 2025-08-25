/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*!\file
 * \brief Common Decoder API definitions.
 *
 * Common definitions used by all decoders
 *
 */

#ifndef __DECAPICOMMON_H__
#define __DECAPICOMMON_H__

/*! \addtogroup common Common definitions
 *  @{
 */

#include "basetype.h"

/** Maximum number of cores supported in multi-core configuration */
#define MAX_ASIC_CORES                  4

/*! \defgroup ASICCFG Hardware configuration
 *  @{
 */

/** MPEG4 decoding not supported */
#define MPEG4_NOT_SUPPORTED             (u32)(0x00) /**<\hideinitializer */
/** MPEG4 simple profile decoding supported */
#define MPEG4_SIMPLE_PROFILE            (u32)(0x01) /**<\hideinitializer */
/** MPEG4 advanced simple profile decoding supported */
#define MPEG4_ADVANCED_SIMPLE_PROFILE   (u32)(0x02) /**<\hideinitializer */
/** MPEG4 custom decoding not supported */
#define MPEG4_CUSTOM_NOT_SUPPORTED      (u32)(0x00) /**<\hideinitializer */
/** MPEG4 custom feature 1 decoding supported */
#define MPEG4_CUSTOM_FEATURE_1          (u32)(0x01) /**<\hideinitializer */

#define H264_NOT_SUPPORTED              (u32)(0x00) /**<\hideinitializer */
#define H264_BASELINE_PROFILE           (u32)(0x01) /**<\hideinitializer */
#define H264_MAIN_PROFILE               (u32)(0x02) /**<\hideinitializer */
#define H264_HIGH_PROFILE               (u32)(0x03) /**<\hideinitializer */
#define VC1_NOT_SUPPORTED               (u32)(0x00) /**<\hideinitializer */
#define VC1_SIMPLE_PROFILE              (u32)(0x01) /**<\hideinitializer */
#define VC1_MAIN_PROFILE                (u32)(0x02) /**<\hideinitializer */
#define VC1_ADVANCED_PROFILE            (u32)(0x03) /**<\hideinitializer */
#define MPEG2_NOT_SUPPORTED             (u32)(0x00) /**<\hideinitializer */
#define MPEG2_MAIN_PROFILE              (u32)(0x01) /**<\hideinitializer */
#define JPEG_NOT_SUPPORTED              (u32)(0x00) /**<\hideinitializer */
#define JPEG_BASELINE                   (u32)(0x01) /**<\hideinitializer */
#define JPEG_PROGRESSIVE                (u32)(0x02) /**<\hideinitializer */
#define PP_NOT_SUPPORTED                (u32)(0x00) /**<\hideinitializer */
#define PP_SUPPORTED                    (u32)(0x01) /**<\hideinitializer */
#define PP_TILED_4X4                    (u32)(0x20000000) /**<\hideinitializer */
#define PP_DITHERING                    (u32)(0x10000000) /**<\hideinitializer */
#define PP_SCALING                      (u32)(0x0C000000) /**<\hideinitializer */
#define PP_DEINTERLACING                (u32)(0x02000000) /**<\hideinitializer */
#define PP_ALPHA_BLENDING               (u32)(0x01000000) /**<\hideinitializer */
#define PP_OUTP_ENDIAN                  (u32)(0x00040000) /**<\hideinitializer */
#define PP_TILED_INPUT                  (u32)(0x0000C000) /**<\hideinitializer */
#define PP_PIX_ACC_OUTPUT               (u32)(0x40000000) /**<\hideinitializer */
#define PP_ABLEND_CROP                  (u32)(0x80000000) /**<\hideinitializer */
#define SORENSON_SPARK_NOT_SUPPORTED    (u32)(0x00) /**<\hideinitializer */
#define SORENSON_SPARK_SUPPORTED        (u32)(0x01) /**<\hideinitializer */
#define VP6_NOT_SUPPORTED               (u32)(0x00) /**<\hideinitializer */
#define VP6_SUPPORTED                   (u32)(0x01) /**<\hideinitializer */
#define VP7_NOT_SUPPORTED               (u32)(0x00) /**<\hideinitializer */
#define VP7_SUPPORTED                   (u32)(0x01) /**<\hideinitializer */
#define VP8_NOT_SUPPORTED               (u32)(0x00) /**<\hideinitializer */
#define VP8_SUPPORTED                   (u32)(0x01) /**<\hideinitializer */
#define REF_BUF_NOT_SUPPORTED           (u32)(0x00) /**<\hideinitializer */
#define REF_BUF_SUPPORTED               (u32)(0x01) /**<\hideinitializer */
#define REF_BUF_INTERLACED              (u32)(0x02) /**<\hideinitializer */
#define REF_BUF_DOUBLE                  (u32)(0x04) /**<\hideinitializer */
#define REF_BUF_OFFSET                  (u32)(0x08) /**<\hideinitializer */
#define TILED_NOT_SUPPORTED             (u32)(0x00) /**<\hideinitializer */
#define TILED_8x4_SUPPORTED             (u32)(0x01) /**<\hideinitializer */
#define AVS_NOT_SUPPORTED               (u32)(0x00) /**<\hideinitializer */
#define AVS_SUPPORTED                   (u32)(0x01) /**<\hideinitializer */
#define JPEG_EXT_NOT_SUPPORTED          (u32)(0x00) /**<\hideinitializer */
#define JPEG_EXT_SUPPORTED              (u32)(0x01) /**<\hideinitializer */
#define RV_NOT_SUPPORTED                (u32)(0x00) /**<\hideinitializer */
#define RV_SUPPORTED                    (u32)(0x01) /**<\hideinitializer */
#define MVC_NOT_SUPPORTED               (u32)(0x00) /**<\hideinitializer */
#define MVC_SUPPORTED                   (u32)(0x01) /**<\hideinitializer */
#define WEBP_NOT_SUPPORTED              (u32)(0x00) /**<\hideinitializer */
#define WEBP_SUPPORTED                  (u32)(0x01) /**<\hideinitializer */
#define EC_NOT_SUPPORTED                (u32)(0x00) /**<\hideinitializer */
#define EC_SUPPORTED                    (u32)(0x01) /**<\hideinitializer */
#define STRIDE_NOT_SUPPORTED            (u32)(0x00) /**<\hideinitializer */
#define STRIDE_SUPPORTED                (u32)(0x01) /**<\hideinitializer */
#define FIELD_DPB_NOT_SUPPORTED         (u32)(0x00) /**<\hideinitializer */
#define FIELD_DPB_SUPPORTED             (u32)(0x01) /**<\hideinitializer */
#define AVS_PLUS_NOT_SUPPORTED          (u32)(0x00) /**<\hideinitializer */
#define AVS_PLUS_SUPPORTED              (u32)(0x01) /**<\hideinitializer */
#define ADDR64_ENV_NOT_SUPPORTED        (u32)(0x00) /**<\hideinitializer */
#define ADDR64_ENV_SUPPORTED            (u32)(0x01) /**<\hideinitializer */

/*! @} - end gorup ASICCFG */

/*! \defgroup FUSE Hardware fuse configuration
 *  @{
 */
#define H264_NOT_SUPPORTED_FUSE             (u32)(0x00) /**<\hideinitializer */
#define H264_FUSE_ENABLED                   (u32)(0x01) /**<\hideinitializer */
#define MPEG4_NOT_SUPPORTED_FUSE            (u32)(0x00) /**<\hideinitializer */
#define MPEG4_FUSE_ENABLED                  (u32)(0x01) /**<\hideinitializer */
#define MPEG2_NOT_SUPPORTED_FUSE            (u32)(0x00) /**<\hideinitializer */
#define MPEG2_FUSE_ENABLED                  (u32)(0x01) /**<\hideinitializer */
#define SORENSON_SPARK_NOT_SUPPORTED_FUSE   (u32)(0x00) /**<\hideinitializer */
#define SORENSON_SPARK_ENABLED              (u32)(0x01) /**<\hideinitializer */
#define JPEG_NOT_SUPPORTED_FUSE             (u32)(0x00) /**<\hideinitializer */
#define JPEG_FUSE_ENABLED                   (u32)(0x01) /**<\hideinitializer */
#define VP6_NOT_SUPPORTED_FUSE              (u32)(0x00) /**<\hideinitializer */
#define VP6_FUSE_ENABLED                    (u32)(0x01) /**<\hideinitializer */
#define VP7_NOT_SUPPORTED_FUSE              (u32)(0x00) /**<\hideinitializer */
#define VP7_FUSE_ENABLED                    (u32)(0x01) /**<\hideinitializer */
#define VP8_NOT_SUPPORTED_FUSE              (u32)(0x00) /**<\hideinitializer */
#define VP8_FUSE_ENABLED                    (u32)(0x01) /**<\hideinitializer */
#define VC1_NOT_SUPPORTED_FUSE              (u32)(0x00) /**<\hideinitializer */
#define VC1_FUSE_ENABLED                    (u32)(0x01) /**<\hideinitializer */
#define JPEG_PROGRESSIVE_NOT_SUPPORTED_FUSE (u32)(0x00) /**<\hideinitializer */
#define JPEG_PROGRESSIVE_FUSE_ENABLED       (u32)(0x01) /**<\hideinitializer */
#define REF_BUF_NOT_SUPPORTED_FUSE          (u32)(0x00) /**<\hideinitializer */
#define REF_BUF_FUSE_ENABLED                (u32)(0x01) /**<\hideinitializer */
#define AVS_NOT_SUPPORTED_FUSE              (u32)(0x00) /**<\hideinitializer */
#define AVS_FUSE_ENABLED                    (u32)(0x01) /**<\hideinitializer */
#define RV_NOT_SUPPORTED_FUSE               (u32)(0x00) /**<\hideinitializer */
#define RV_FUSE_ENABLED                     (u32)(0x01) /**<\hideinitializer */
#define MVC_NOT_SUPPORTED_FUSE              (u32)(0x00) /**<\hideinitializer */
#define MVC_FUSE_ENABLED                    (u32)(0x01) /**<\hideinitializer */

#define PP_NOT_SUPPORTED_FUSE               (u32)(0x00) /**<\hideinitializer */
#define PP_FUSE_ENABLED                     (u32)(0x01) /**<\hideinitializer */
#define PP_FUSE_DEINTERLACING_ENABLED       (u32)(0x40000000) /**<\hideinitializer */
#define PP_FUSE_ALPHA_BLENDING_ENABLED      (u32)(0x20000000) /**<\hideinitializer */
#define MAX_PP_OUT_WIDHT_1920_FUSE_ENABLED  (u32)(0x00008000) /**<\hideinitializer */
#define MAX_PP_OUT_WIDHT_1280_FUSE_ENABLED  (u32)(0x00004000) /**<\hideinitializer */
#define MAX_PP_OUT_WIDHT_720_FUSE_ENABLED   (u32)(0x00002000) /**<\hideinitializer */
#define MAX_PP_OUT_WIDHT_352_FUSE_ENABLED   (u32)(0x00001000) /**<\hideinitializer */

/*! @} - end group FUSE */

/*! \struct DecHwConfig_
 * Hardware configuration.
 * \typedef DecHwConfig
 * A typename for DecHwConfig_.
 */
typedef struct DecHwConfig_ {
	u32 mpeg4Support;        /**< one of the MPEG4 values defined in \ref ASICCFG */
	u32 customMpeg4Support;  /**< one of the MPEG4 custom values defined in \ref ASICCFG */
	u32 h264Support;         /* one of the H264 values defined above */
	u32 vc1Support;          /* one of the VC1 values defined above */
	u32 mpeg2Support;        /* one of the MPEG2 values defined above */
	u32 jpegSupport;         /* one of the JPEG values defined above */
	u32 jpegProgSupport;     /* one of the Progressive JPEG values defined above */
	u32 maxDecPicWidth;      /* maximum picture width in decoder */
	u32 ppSupport;           /* PP_SUPPORTED or PP_NOT_SUPPORTED */
	u32 ppConfig;            /* Bitwise list of PP function */
	u32 maxPpOutPicWidth;    /* maximum post-processor output picture width */
	u32 sorensonSparkSupport;/* one of the SORENSON_SPARK values defined above */
	u32 refBufSupport;       /* one of the REF_BUF values defined above */
	u32 tiledModeSupport;    /* one of the TILED values defined above */
	u32 vp6Support;          /* one of the VP6 values defined above */
	u32 vp7Support;          /* one of the VP7 values defined above */
	u32 vp8Support;          /* one of the VP8 values defined above */
	u32 avsSupport;          /* one of the AVS values defined above */
	u32 jpegESupport;        /* one of the JPEG EXT values defined above */
	u32 rvSupport;           /* one of the HUKKA values defined above */
	u32 mvcSupport;          /* one of the MVC values defined above */
	u32 webpSupport;         /* one of the WEBP values defined above */
	u32 ecSupport;           /* one of the EC values defined above */
	u32 strideSupport;       /* HW supports separate Y and C strides */
	u32 fieldDpbSupport;     /* HW supports field-mode DPB */
	u32 avsPlusSupport;      /* one of the AVS PLUS values defined above */
	u32 addr64Support;     /* HW supports 64bit bit width */
} DecHwConfig;

/*! \struct DecSwHwBuild_
 *  Build information of the decoder.
 * \typedef DecSwHwBuild
 * A typename for DecSwHwBuild_.
 */
typedef struct DecSwHwBuild_ {
	u32 swBuild;         /**< Software build ID */
	u32 hwBuild;         /**< Hardware build ID */
	DecHwConfig hwConfig[MAX_ASIC_CORES];     /**< Hardware configuration for each core */
} DecSwHwBuild;


/** DPB flags to control reference picture format etc. */
typedef enum {
	/* Reference frame formats */
	DEC_REF_FRM_RASTER_SCAN         = 0x0,
	DEC_REF_FRM_TILED_DEFAULT       = 0x1,

	/* Flag to allow SW to use DPB field ordering on interlaced content */
	DEC_DPB_ALLOW_FIELD_ORDERING    = 0x40000000
} DecDpbFlags;

#define DEC_REF_FRM_FMT_MASK        0x01

/** Modes for storing content into DPB */
typedef enum {
	DEC_DPB_FRAME                    = 0,
	DEC_DPB_INTERLACED_FIELD         = 1
} DecDpbMode;

/* DEPRECATED!!! do not use in new applications! */
#define DEC_DPB_DEFAULT              DEC_DPB_FRAME

/** Output picture format types */
typedef enum {
	DEC_OUT_FRM_RASTER_SCAN          = 0, /** Output picture is in raster scan format. */
	DEC_OUT_FRM_TILED_8X4            = 1  /** Output picture is in 8x4 tiled format. */
} DecOutFrmFormat;

/** Picture coding type */
typedef enum {
	DEC_PIC_TYPE_I           = 0,
	DEC_PIC_TYPE_P           = 1,
	DEC_PIC_TYPE_B           = 2,
	DEC_PIC_TYPE_D           = 3,
	DEC_PIC_TYPE_FI          = 4,
	DEC_PIC_TYPE_BI          = 5
} DecPicCodingType;

/* error handling */
typedef enum {
	DEC_EC_PICTURE_FREEZE            = 0,
	DEC_EC_VIDEO_FREEZE              = 1,
	DEC_EC_PARTIAL_FREEZE            = 2,
	DEC_EC_PARTIAL_IGNORE            = 3
} DecErrorHandling;

int hx170dec_init(void);

/*! @} - end group common*/

#endif /* __DECAPICOMMON_H__ */
