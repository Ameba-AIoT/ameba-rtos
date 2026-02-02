/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SOFTWARE_LINUX_DWL_DWL_DEFS_H_
#define SOFTWARE_LINUX_DWL_DWL_DEFS_H_

#ifdef __cplusplus
extern "C" {
#endif

#define DWL_MPEG2_E         31  /* 1 bit */
#define DWL_VC1_E           29  /* 2 bits */
#define DWL_JPEG_E          28  /* 1 bit */
#define DWL_MPEG4_E         26  /* 2 bits */
#define DWL_H264_E          24  /* 2 bits */
#define DWL_VP6_E           23  /* 1 bit */
#define DWL_RV_E            26  /* 2 bits */
#define DWL_VP8_E           23  /* 1 bit */
#define DWL_VP7_E           24  /* 1 bit */
#define DWL_WEBP_E          19  /* 1 bit */
#define DWL_AVS_E           22  /* 1 bit */
#define DWL_PP_E            16  /* 1 bit */

#define DWL_PJPEG_E         22  /* 1 bit */
#define DWL_REF_BUFF_E      20  /* 1 bit */

#define DWL_JPEG_EXT_E          31  /* 1 bit */
#define DWL_REF_BUFF_ILACE_E    30  /* 1 bit */
#define DWL_MPEG4_CUSTOM_E      29  /* 1 bit */
#define DWL_REF_BUFF_DOUBLE_E   28  /* 1 bit */

#define DWL_MVC_E           20  /* 2 bits */

#define DWL_DEC_TILED_L     17  /* 2 bits */
#define DWL_DEC_PIC_W_EXT   14  /* 2 bits */
#define DWL_EC_E            12  /* 2 bits */
#define DWL_STRIDE_E        11  /* 1 bit */
#define DWL_FIELD_DPB_E     10  /* 1 bit */
#define DWL_AVS_PLUS_E       6  /* 1 bit */
#define DWL_64BIT_ENV_E      5  /* 1 bit */

#define DWL_CFG_E           24  /* 4 bits */
#define DWL_PP_IN_TILED_L   14  /* 2 bits */

#define DWL_SORENSONSPARK_E 11  /* 1 bit */

#define DWL_H264_FUSE_E          31 /* 1 bit */
#define DWL_MPEG4_FUSE_E         30 /* 1 bit */
#define DWL_MPEG2_FUSE_E         29 /* 1 bit */
#define DWL_SORENSONSPARK_FUSE_E 28 /* 1 bit */
#define DWL_JPEG_FUSE_E          27 /* 1 bit */
#define DWL_VP6_FUSE_E           26 /* 1 bit */
#define DWL_VC1_FUSE_E           25 /* 1 bit */
#define DWL_PJPEG_FUSE_E         24 /* 1 bit */
#define DWL_CUSTOM_MPEG4_FUSE_E  23 /* 1 bit */
#define DWL_RV_FUSE_E            22 /* 1 bit */
#define DWL_VP7_FUSE_E           21 /* 1 bit */
#define DWL_VP8_FUSE_E           20 /* 1 bit */
#define DWL_AVS_FUSE_E           19 /* 1 bit */
#define DWL_MVC_FUSE_E           18 /* 1 bit */

#define DWL_DEC_MAX_1920_FUSE_E  15 /* 1 bit */
#define DWL_DEC_MAX_1280_FUSE_E  14 /* 1 bit */
#define DWL_DEC_MAX_720_FUSE_E   13 /* 1 bit */
#define DWL_DEC_MAX_352_FUSE_E   12 /* 1 bit */
#define DWL_REF_BUFF_FUSE_E       7 /* 1 bit */


#define DWL_PP_FUSE_E				31  /* 1 bit */
#define DWL_PP_DEINTERLACE_FUSE_E   30  /* 1 bit */
#define DWL_PP_ALPHA_BLEND_FUSE_E   29  /* 1 bit */
#define DWL_PP_MAX_4096_FUSE_E		16  /* 1 bit */
#define DWL_PP_MAX_1920_FUSE_E		15  /* 1 bit */
#define DWL_PP_MAX_1280_FUSE_E		14  /* 1 bit */
#define DWL_PP_MAX_720_FUSE_E		13  /* 1 bit */
#define DWL_PP_MAX_352_FUSE_E		12  /* 1 bit */

#define DWL_DECODER_INT ((DWLReadReg(dec_dwl, HX170DEC_REG_START) >> 11) & 0xFFU)
#define DWL_PP_INT      ((DWLReadReg(dec_dwl, HX170PP_REG_START) >> 11) & 0xFFU)

#define DEC_IRQ_ABORT          (1 << 11)
#define DEC_IRQ_RDY            (1 << 12)
#define DEC_IRQ_BUS            (1 << 13)
#define DEC_IRQ_BUFFER         (1 << 14)
#define DEC_IRQ_ASO            (1 << 15)
#define DEC_IRQ_ERROR          (1 << 16)
#define DEC_IRQ_SLICE          (1 << 17)
#define DEC_IRQ_TIMEOUT        (1 << 18)

#define PP_IRQ_RDY             (1 << 12)
#define PP_IRQ_BUS             (1 << 13)

#define DWL_HW_ENABLE_BIT       0x000001    /* 0th bit */

#define HX170_IRQ_STAT_DEC          1
#define HX170_IRQ_STAT_DEC_OFF      (HX170_IRQ_STAT_DEC * 4)
#define HX170_IRQ_STAT_PP           60
#define HX170_IRQ_STAT_PP_OFF       (HX170_IRQ_STAT_PP * 4)

#define HX170PP_SYNTH_CFG           100
#define HX170PP_SYNTH_CFG_OFF       (HX170PP_SYNTH_CFG * 4)
#define HX170DEC_SYNTH_CFG          50
#define HX170DEC_SYNTH_CFG_OFF      (HX170DEC_SYNTH_CFG * 4)
#define HX170DEC_SYNTH_CFG_2        54
#define HX170DEC_SYNTH_CFG_2_OFF    (HX170DEC_SYNTH_CFG_2 * 4)



#define HX170PP_REG_START       0xF0
#define HX170DEC_REG_START      0x4
#define HX170_MSB_START         (119*4)

#define HX170PP_FUSE_CFG         99
#define HX170DEC_FUSE_CFG        57



#define HX170_DEC_E                 0x01
#define HX170_PP_E                  0x01
#define HX170_DEC_ABORT             0x20
#define HX170_DEC_IRQ_DISABLE       0x10
#define HX170_PP_IRQ_DISABLE        0x10
#define HX170_DEC_IRQ               0x100
#define HX170_PP_IRQ                0x100



#define HANTRO_REG_OFFSET               0
/* Base address of PCI base address translation */
#define HLINA_ADDR_TRANSL_REG            0x20c/4

#define HXDEC_MAX_CORES                 1

#define HANTRO_DEC_ORG_REGS             60
#define HANTRO_PP_ORG_REGS              41

#define HANTRO_DEC_EXT_REGS             27
#define HANTRO_PP_EXT_REGS              9

#define HANTRO_DEC_TOTAL_REGS           (HANTRO_DEC_ORG_REGS + HANTRO_DEC_EXT_REGS)
#define HANTRO_PP_TOTAL_REGS            (HANTRO_PP_ORG_REGS + HANTRO_PP_EXT_REGS)
#define HANTRO_TOTAL_REGS               155

#define HANTRO_DEC_ORG_FIRST_REG        0
#define HANTRO_DEC_ORG_LAST_REG         59
#define HANTRO_DEC_EXT_FIRST_REG        119
#define HANTRO_DEC_EXT_LAST_REG         145

#define HANTRO_PP_ORG_FIRST_REG         60
#define HANTRO_PP_ORG_LAST_REG          100
#define HANTRO_PP_EXT_FIRST_REG         146
#define HANTRO_PP_EXT_LAST_REG          154

#define VP_PB_INT_LT                    30
#define SOCLE_INT                       36

#define HXDEC_NO_IRQ                    -1


/* module defaults */
#ifdef USE_64BIT_ENV
#define DEC_IO_SIZE             ((HANTRO_TOTAL_REGS) * 4) /* bytes */
#else
#define DEC_IO_SIZE             ((HANTRO_DEC_ORG_REGS + HANTRO_PP_ORG_REGS) * 4) /* bytes */
#endif
#define DEC_IRQ                 MJPEG_IRQ


struct core_desc {
	u32 id; /* id of the core */
	u32 *regs; /* pointer to user registers */
	u32 size; /* size of register space */
};


/* wrapper information */
typedef struct hX170dwl {
	u32 clientType;
	u32 numCores;
	u32 bPPReserved;
} hX170dwl_t;

/* here's all the must remember stuff */
typedef struct {
	char *buffer;
	unsigned int iosize;
	volatile u8 *hwregs[HXDEC_MAX_CORES];
	int irq;
	int cores;
} hx170dec_t;

#ifdef __cplusplus
}
#endif

#endif /* SOFTWARE_LINUX_DWL_DWL_DEFS_H_ */
