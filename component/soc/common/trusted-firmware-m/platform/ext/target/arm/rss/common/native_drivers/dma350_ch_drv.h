/*
 * Copyright (c) 2022-2023 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __DMA350_CH_DRV_H
#define __DMA350_CH_DRV_H

#include "dma350_regdef.h"

/* For __STATIC_INLINE and __DMB */
#include "cmsis_compiler.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* DMA350_CH state definitions */
#define DMA350_CH_INITIALIZED (1UL << 0)

#define DMA350_CH_TMPLTCFG_SRCTMPLTSIZE_MAX 0x1FUL
#define DMA350_CH_TMPLTCFG_DESTMPLTSIZE_MAX 0x1FUL

#define DMA350_CH_CTRL_RESET_VALUE        0x00200200
#define DMA350_CH_INTREN_RESET_VALUE      0x00000000
#define DMA350_CH_LINKADDR_RESET_VALUE    0x00000000
#define DMA350_CH_DESTRANSCFG_RESET_VALUE 0x000F0400
#define DMA350_CH_SRCTRANSCFG_RESET_VALUE 0x000F0400
#define DMA350_CH_AUTOCFG_RESET_VALUE     0x00000000

#define DMA350_CMDLINK_REGCLEAR_SET     (0x1UL)
#define DMA350_CMDLINK_INTREN_SET       (0x1UL << 2)
#define DMA350_CMDLINK_CTRL_SET         (0x1UL << 3)
#define DMA350_CMDLINK_SRC_ADDR_SET     (0x1UL << 4)
#define DMA350_CMDLINK_SRC_ADDRHI_SET   (0x1UL << 5)
#define DMA350_CMDLINK_DES_ADDR_SET     (0x1UL << 6)
#define DMA350_CMDLINK_DES_ADDRHI_SET   (0x1UL << 7)
#define DMA350_CMDLINK_XSIZE_SET        (0x1UL << 8)
#define DMA350_CMDLINK_XSIZEHI_SET      (0x1UL << 9)
#define DMA350_CMDLINK_SRCTRANSCFG_SET  (0x1UL << 10)
#define DMA350_CMDLINK_DESTRANSCFG_SET  (0x1UL << 11)
#define DMA350_CMDLINK_XADDRINC_SET     (0x1UL << 12)
#define DMA350_CMDLINK_YADDRSTRIDE_SET  (0x1UL << 13)
#define DMA350_CMDLINK_FILLVAL_SET      (0x1UL << 14)
#define DMA350_CMDLINK_YSIZE_SET        (0x1UL << 15)
#define DMA350_CMDLINK_TMPLTCFG_SET     (0x1UL << 16)
#define DMA350_CMDLINK_SRCTMPLT_SET     (0x1UL << 17)
#define DMA350_CMDLINK_DESTMPLT_SET     (0x1UL << 18)
#define DMA350_CMDLINK_SRCTRIGINCFG_SET (0x1UL << 19)
#define DMA350_CMDLINK_DESTRIGINCFG_SET (0x1UL << 20)
#define DMA350_CMDLINK_TRIGOUTCFG_SET   (0x1UL << 21)
#define DMA350_CMDLINK_GPOEN0_SET       (0x1UL << 22)
#define DMA350_CMDLINK_GPOVAL0_SET      (0x1UL << 24)
#define DMA350_CMDLINK_STREAMINTCFG_SET (0x1UL << 26)
#define DMA350_CMDLINK_LINKATTR_SET     (0x1UL << 28)
#define DMA350_CMDLINK_AUTOCFG_SET      (0x1UL << 29)
#define DMA350_CMDLINK_LINKADDR_SET     (0x1UL << 30)
#define DMA350_CMDLINK_LINKADDRHI_SET   (0x1UL << 31)

#define DMA350_CMD_1D_BASIC(_CH, _SRCADDR, _DESADDR, _SRCXSIZE, _DSTXSIZE)     \
    {                                                                          \
        DMA350_CMDLINK_SRC_ADDR_SET | DMA350_CMDLINK_DES_ADDR_SET |            \
            DMA350_CMDLINK_XSIZE_SET,                                          \
            (uint32_t)(_SRCADDR), (uint32_t)(_DESADDR),                        \
            ((_SRCXSIZE) << 16U) | (_DSTXSIZE),                                \
    }

/* Update a register (REG) at position (POS) with value (VAL).
 * Affected bits are defined by a mask (MSK) */
#define SET_FIELD(REG, VAL, POS, MSK)                                          \
    do {                                                                       \
        REG = (REG & ~MSK) | (((uint32_t)(VAL) << POS) & MSK);                 \
    } while (0)

/* DMA350 DMA Channel error enumeration types */
enum dma350_ch_error_t {
    DMA350_CH_ERR_NONE = 0,      /*!< No error */
    DMA350_CH_ERR_IIDR_MISMATCH, /*!< Error: DMA350 driver does not support this
                                  *   implementer of the hardware */
    DMA350_CH_ERR_AIDR_MISMATCH, /*!< Error: DMA350 driver does not support this
                                  *   architecture revision */
    DMA350_CH_ERR_INVALID_ARG,   /*!< Error: invalid input argument */
    DMA350_CH_ERR_INVALID_CMD,   /*!< Error: invalid command */
    DMA350_CH_ERR_NOT_READY,     /*!< Error: DMA350 not ready */
    DMA350_CH_ERR_UNSUP_CH,      /*!< Error: DMA350 Channel not supported */
};

/* ARM DMA350 DMA Channel device configuration structure */
struct dma350_ch_dev_cfg_t {
    DMACH_TypeDef *const ch_base; /*!< DMA350 DMA Channel base address */
    const uint8_t channel;        /*!< DMA350 DMA Channel number */
};

/* ARM DMA350 DMA Channel device data structure */
struct dma350_ch_dev_data_t {
    uint32_t state; /*!< Indicates if the DMA350 Channel driver
                     *   is initialized and enabled */
};

/* ARM DMA350 DMA Channel device structure */
struct dma350_ch_dev_t {
    const struct dma350_ch_dev_cfg_t cfg; /*!< DMA350 DMA Channel Config */
    struct dma350_ch_dev_data_t data;     /*!< DMA350 DMA Channel Data */
};

/* ARM DMA350 DMA Channel commands */
enum dma350_ch_cmd_t {
    DMA350_CH_CMD_ENABLECMD = DMA_CH_CMD_ENABLECMD,
    DMA350_CH_CMD_CLEARCMD = DMA_CH_CMD_CLEARCMD,
    DMA350_CH_CMD_DISABLECMD = DMA_CH_CMD_DISABLECMD,
    DMA350_CH_CMD_STOPCMD = DMA_CH_CMD_STOPCMD,
    DMA350_CH_CMD_PAUSECMD = DMA_CH_CMD_PAUSECMD,
    DMA350_CH_CMD_RESUMECMD = DMA_CH_CMD_RESUMECMD,
    DMA350_CH_CMD_SRCSWTRIGINREQ = DMA_CH_CMD_SRCSWTRIGINREQ,
    DMA350_CH_CMD_SRCSWTRIGINREQ_LAST =
        DMA_CH_CMD_SRCSWTRIGINREQ | DMA_CH_CMD_SRCSWTRIGINTYPE_0,
    DMA350_CH_CMD_SRCSWTRIGINREQ_BLOCK =
        DMA_CH_CMD_SRCSWTRIGINREQ | DMA_CH_CMD_SRCSWTRIGINTYPE_1,
    DMA350_CH_CMD_SRCSWTRIGINREQ_BLOCK_LAST = DMA_CH_CMD_SRCSWTRIGINREQ |
                                              DMA_CH_CMD_SRCSWTRIGINTYPE_1 |
                                              DMA_CH_CMD_SRCSWTRIGINTYPE_0,
    DMA350_CH_CMD_DESSWTRIGINREQ = DMA_CH_CMD_DESSWTRIGINREQ,
    DMA350_CH_CMD_DESSWTRIGINREQ_LAST =
        DMA_CH_CMD_DESSWTRIGINREQ | DMA_CH_CMD_DESSWTRIGINTYPE_0,
    DMA350_CH_CMD_DESSWTRIGINREQ_BLOCK =
        DMA_CH_CMD_DESSWTRIGINREQ | DMA_CH_CMD_DESSWTRIGINTYPE_1,
    DMA350_CH_CMD_DESSWTRIGINREQ_BLOCK_LAST = DMA_CH_CMD_DESSWTRIGINREQ |
                                              DMA_CH_CMD_DESSWTRIGINTYPE_1 |
                                              DMA_CH_CMD_DESSWTRIGINTYPE_0,
    DMA350_CH_CMD_SWTRIGOUTACK = DMA_CH_CMD_SWTRIGOUTACK
};

/* ARM DMA350 DMA Channel status bits */
enum dma350_ch_stat_t {
    DMA350_CH_STAT_DONE = DMA_CH_STATUS_STAT_DONE,
    DMA350_CH_STAT_ERR = DMA_CH_STATUS_STAT_ERR,
    DMA350_CH_STAT_DISABLED = DMA_CH_STATUS_STAT_DISABLED,
    DMA350_CH_STAT_STOPPED = DMA_CH_STATUS_STAT_STOPPED,
    DMA350_CH_STAT_SRCTRIGINWAIT = DMA_CH_STATUS_STAT_SRCTRIGINWAIT,
    DMA350_CH_STAT_DESTRIGINWAIT = DMA_CH_STATUS_STAT_DESTRIGINWAIT,
    DMA350_CH_STAT_TRIGOUTACKWAIT = DMA_CH_STATUS_STAT_TRIGOUTACKWAIT,
    DMA350_CH_STAT_ALL =
        DMA_CH_STATUS_STAT_DONE | DMA_CH_STATUS_STAT_ERR |
        DMA_CH_STATUS_STAT_DISABLED | DMA_CH_STATUS_STAT_STOPPED |
        DMA_CH_STATUS_STAT_SRCTRIGINWAIT | DMA_CH_STATUS_STAT_DESTRIGINWAIT |
        DMA_CH_STATUS_STAT_TRIGOUTACKWAIT
};

/* ARM DMA350 DMA Channel interrupt bits */
enum dma350_ch_intr_t {
    DMA350_CH_INTREN_DONE = DMA_CH_INTREN_INTREN_DONE,
    DMA350_CH_INTREN_ERR = DMA_CH_INTREN_INTREN_ERR,
    DMA350_CH_INTREN_DISABLED = DMA_CH_INTREN_INTREN_DISABLED,
    DMA350_CH_INTREN_STOPPED = DMA_CH_INTREN_INTREN_STOPPED,
    DMA350_CH_INTREN_SRCTRIGINWAIT = DMA_CH_INTREN_INTREN_SRCTRIGINWAIT,
    DMA350_CH_INTREN_DESTRIGINWAIT = DMA_CH_INTREN_INTREN_DESTRIGINWAIT,
    DMA350_CH_INTREN_TRIGOUTACKWAIT = DMA_CH_INTREN_INTREN_TRIGOUTACKWAIT,
    DMA350_CH_INTREN_ALL =
        DMA_CH_INTREN_INTREN_DONE | DMA_CH_INTREN_INTREN_ERR |
        DMA_CH_INTREN_INTREN_DISABLED | DMA_CH_INTREN_INTREN_STOPPED |
        DMA_CH_INTREN_INTREN_SRCTRIGINWAIT |
        DMA_CH_INTREN_INTREN_DESTRIGINWAIT | DMA_CH_INTREN_INTREN_TRIGOUTACKWAIT
};

/* ARM DMA350 DMA Channel Transfer size */
enum dma350_ch_transize_t {
    DMA350_CH_TRANSIZE_8BITS = 0,
    DMA350_CH_TRANSIZE_16BITS = DMA_CH_CTRL_TRANSIZE_0,
    DMA350_CH_TRANSIZE_32BITS = DMA_CH_CTRL_TRANSIZE_1,
    DMA350_CH_TRANSIZE_64BITS = DMA_CH_CTRL_TRANSIZE_1 | DMA_CH_CTRL_TRANSIZE_0,
    DMA350_CH_TRANSIZE_128BITS = DMA_CH_CTRL_TRANSIZE_2,
    DMA350_CH_TRANSIZE_256BITS =
        DMA_CH_CTRL_TRANSIZE_2 | DMA_CH_CTRL_TRANSIZE_0,
    DMA350_CH_TRANSIZE_512BITS =
        DMA_CH_CTRL_TRANSIZE_2 | DMA_CH_CTRL_TRANSIZE_1,
    DMA350_CH_TRANSIZE_1024BITS =
        DMA_CH_CTRL_TRANSIZE_2 | DMA_CH_CTRL_TRANSIZE_1 | DMA_CH_CTRL_TRANSIZE_0
};

/* ARM DMA350 DMA Channel XTYPE */
enum dma350_ch_xtype_t {
    DMA350_CH_XTYPE_DISABLE = 0,
    DMA350_CH_XTYPE_CONTINUE = DMA_CH_CTRL_XTYPE_0,
    DMA350_CH_XTYPE_WRAP = DMA_CH_CTRL_XTYPE_1,
    DMA350_CH_XTYPE_FILL = DMA_CH_CTRL_XTYPE_1 | DMA_CH_CTRL_XTYPE_0
};

/* ARM DMA350 DMA Channel YTYPE */
enum dma350_ch_ytype_t {
    DMA350_CH_YTYPE_DISABLE = 0,
    DMA350_CH_YTYPE_CONTINUE = DMA_CH_CTRL_YTYPE_0,
    DMA350_CH_YTYPE_WRAP = DMA_CH_CTRL_YTYPE_1,
    DMA350_CH_YTYPE_FILL = DMA_CH_CTRL_YTYPE_1 | DMA_CH_CTRL_YTYPE_0
};

/* ARM DMA350 DMA Channel Automatic register reload type */
enum dma350_ch_regreloadtype_t {
    DMA350_CH_REGRELOADTYPE_DISABLE = 0,
    DMA350_CH_REGRELOADTYPE_SRC_DES_SIZE = DMA_CH_CTRL_REGRELOADTYPE_0,
    DMA350_CH_REGRELOADTYPE_SRC_ADDR_SRC_DES_SIZE =
        DMA_CH_CTRL_REGRELOADTYPE_1 | DMA_CH_CTRL_REGRELOADTYPE_0,
    DMA350_CH_REGRELOADTYPE_DES_ADDR_SRC_DES_SIZE =
        DMA_CH_CTRL_REGRELOADTYPE_2 | DMA_CH_CTRL_REGRELOADTYPE_0,
    DMA350_CH_REGRELOADTYPE_SRC_DES_ADDR_AND_SIZE =
        DMA_CH_CTRL_REGRELOADTYPE_2 | DMA_CH_CTRL_REGRELOADTYPE_1 |
        DMA_CH_CTRL_REGRELOADTYPE_0
};

/* ARM DMA350 DMA Channel Done type */
enum dma350_ch_donetype_t {
    DMA350_CH_DONETYPE_NONE = 0,
    DMA350_CH_DONETYPE_END_OF_CMD = DMA_CH_CTRL_DONETYPE_0,
    DMA350_CH_DONETYPE_END_OF_AUTORESTART =
        DMA_CH_CTRL_DONETYPE_1 | DMA_CH_CTRL_DONETYPE_0
};

/* ARM DMA350 DMA Channel Source Trigger Input Type */
enum dma350_ch_srctrigintype_t {
    DMA350_CH_SRCTRIGINTYPE_SOFTWARE_ONLY = 0,
    DMA350_CH_SRCTRIGINTYPE_HW = DMA_CH_SRCTRIGINCFG_SRCTRIGINTYPE_1,
    DMA350_CH_SRCTRIGINTYPE_INTERNAL = DMA_CH_SRCTRIGINCFG_SRCTRIGINTYPE_1 |
                                       DMA_CH_SRCTRIGINCFG_SRCTRIGINTYPE_0
};

/* ARM DMA350 DMA Channel Source Trigger Input Mode */
enum dma350_ch_srctriginmode_t {
    DMA350_CH_SRCTRIGINMODE_CMD = 0,
    DMA350_CH_SRCTRIGINMODE_DMA_FLOW_CTRL = DMA_CH_SRCTRIGINCFG_SRCTRIGINMODE_1,
    DMA350_CH_SRCTRIGINMODE_PERIPH_FLOW_CTRL =
        DMA_CH_SRCTRIGINCFG_SRCTRIGINMODE_1 |
        DMA_CH_SRCTRIGINCFG_SRCTRIGINMODE_0
};

/* ARM DMA350 DMA Channel Destination Trigger Input Type */
enum dma350_ch_destrigintype_t {
    DMA350_CH_DESTRIGINTYPE_SOFTWARE_ONLY = 0,
    DMA350_CH_DESTRIGINTYPE_HW = DMA_CH_DESTRIGINCFG_DESTRIGINTYPE_1,
    DMA350_CH_DESTRIGINTYPE_INTERNAL = DMA_CH_DESTRIGINCFG_DESTRIGINTYPE_1 |
                                       DMA_CH_DESTRIGINCFG_DESTRIGINTYPE_0
};

/* ARM DMA350 DMA Channel Destination Trigger Input Mode */
enum dma350_ch_destriginmode_t {
    DMA350_CH_DESTRIGINMODE_CMD = 0,
    DMA350_CH_DESTRIGINMODE_DMA_FLOW_CTRL = DMA_CH_DESTRIGINCFG_DESTRIGINMODE_1,
    DMA350_CH_DESTRIGINMODE_PERIPH_FLOW_CTRL =
        DMA_CH_DESTRIGINCFG_DESTRIGINMODE_1 |
        DMA_CH_DESTRIGINCFG_DESTRIGINMODE_0
};

/* ARM DMA350 DMA Channel Trigger Output Type */
enum dma350_ch_trigouttype_t {
    DMA350_CH_TRIGOUTTYPE_SOFTWARE_ONLY = 0,
    DMA350_CH_TRIGOUTTYPE_HW = DMA_CH_TRIGOUTCFG_TRIGOUTTYPE_1,
    DMA350_CH_TRIGOUTTYPE_INTERNAL =
        DMA_CH_TRIGOUTCFG_TRIGOUTTYPE_1 | DMA_CH_TRIGOUTCFG_TRIGOUTTYPE_0
};

/* ARM DMA350 DMA Channel Stream Interface operation Type */
enum dma350_ch_streamtype_t {
    DMA350_CH_STREAMTYPE_IN_OUT = 0,
    DMA350_CH_STREAMTYPE_OUT_ONLY = DMA_CH_STREAMINTCFG_STREAMTYPE_0,
    DMA350_CH_STREAMTYPE_IN_ONLY = DMA_CH_STREAMINTCFG_STREAMTYPE_1
};

/* ARM DMA350 DMA Channel Status Type */
union dma350_ch_status_t {
    struct {
        uint32_t INTR_DONE:1;            /*!< bit:      0 INTR_DONE */
        uint32_t INTR_ERR:1;             /*!< bit:      1 INTR_ERR */
        uint32_t INTR_DISABLED:1;        /*!< bit:      2 INTR_DISABLED */
        uint32_t INTR_STOPPED:1;         /*!< bit:      3 INTR_STOPPED */
        uint32_t RESERVED0:4;            /*!< bit:  4.. 7 RESERVED0[ 3:0] */
        uint32_t INTR_SRCTRIGINWAIT:1;   /*!< bit:      8 INTR_SRCTRIGINWAIT */
        uint32_t INTR_DESTRIGINWAIT:1;   /*!< bit:      9 INTR_DESTRIGINWAIT */
        uint32_t INTR_TRIGOUTACKWAIT:1;  /*!< bit:     10 INTR_TRIGOUTACKWAIT */
        uint32_t RESERVED1:5;            /*!< bit: 11..15 RESERVED1[ 4:0] */
        uint32_t STAT_DONE:1;            /*!< bit:     16 STAT_DONE */
        uint32_t STAT_ERR:1;             /*!< bit:     17 STAT_ERR */
        uint32_t STAT_DISABLED:1;        /*!< bit:     18 STAT_DISABLED */
        uint32_t STAT_STOPPED:1;         /*!< bit:     19 STAT_STOPPED */
        uint32_t STAT_PAUSED:1;          /*!< bit:     20 STAT_PAUSED */
        uint32_t STAT_RESUMEWAIT:1;      /*!< bit:     21 STAT_RESUMEWAIT */
        uint32_t RESERVED2:2;            /*!< bit: 22..23 RESERVED2[ 1:0] */
        uint32_t STAT_SRCTRIGINWAIT:1;   /*!< bit:     24 STAT_SRCTRIGINWAIT */
        uint32_t STAT_DESTRIGINWAIT:1;   /*!< bit:     25 STAT_DESTRIGINWAIT */
        uint32_t STAT_TRIGOUTACKWAIT:1;  /*!< bit:     26 STAT_TRIGOUTACKWAIT */
        uint32_t RESERVED3:5;            /*!< bit: 27..31 RESERVED3[ 4:0] */
    } b;                                 /*!< Structure used for bit access */
    uint32_t w;                          /*!< Type used for word access */
};

/* ARM DMA350 DMA Command link register structure */
/* Note: Field order must match the order of the bits in the header */
struct dma350_cmdlink_reg_t {
    /* Note: REGCLEAR (Bit 0) has no associated field and Bit 1 is reserved */
    uint32_t intren;       /* Bit 2  */
    uint32_t ctrl;         /* Bit 3  */
    uint32_t srcaddr;      /* Bit 4  */
    uint32_t srcaddrhi;    /* Bit 5  */
    uint32_t desaddr;      /* Bit 6  */
    uint32_t desaddrhi;    /* Bit 7  */
    uint32_t xsize;        /* Bit 8  */
    uint32_t xsizehi;      /* Bit 9  */
    uint32_t srctranscfg;  /* Bit 10 */
    uint32_t destranscfg;  /* Bit 11 */
    uint32_t xaddrinc;     /* Bit 12 */
    uint32_t yaddrstride;  /* Bit 13 */
    uint32_t fillval;      /* Bit 14 */
    uint32_t ysize;        /* Bit 15 */
    uint32_t tmpltcfg;     /* Bit 16 */
    uint32_t srctmplt;     /* Bit 17 */
    uint32_t destmplt;     /* Bit 18 */
    uint32_t srctrigincfg; /* Bit 19 */
    uint32_t destrigincfg; /* Bit 20 */
    uint32_t trigoutcfg;   /* Bit 21 */
    uint32_t gpoen0;       /* Bit 22 */
    uint32_t reserved0;    /* Bit 23 */
    uint32_t gpoval0;      /* Bit 24 */
    uint32_t reserved1;    /* Bit 25 */
    uint32_t streamintcfg; /* Bit 26 */
    uint32_t reserved2;    /* Bit 27 */
    uint32_t linkattr;     /* Bit 28 */
    uint32_t autocfg;      /* Bit 29 */
    uint32_t linkaddr;     /* Bit 30 */
    uint32_t linkaddrhi;   /* Bit 31 */
};

/* ARM DMA350 DMA Command link generator config structure */
struct dma350_cmdlink_gencfg_t {
    uint32_t header;
    struct dma350_cmdlink_reg_t cfg;
};

/**
 * \brief Initializes DMA350 DMA Channel
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return Returns error code as specified in \ref dma350_ch_error_t
 *
 * \note This function doesn't check if dev is NULL.
 */
enum dma350_ch_error_t dma350_ch_init(struct dma350_ch_dev_t *dev);

/**
 * \brief Checks if DMA350 device is initialised
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return Returns true if device is initialized
 *
 * \note This function doesn't check if dev is NULL.
 */
__STATIC_INLINE
bool dma350_ch_is_init(const struct dma350_ch_dev_t *dev);

/**
 * \brief Sets source address[31:0] of channel
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] src_addr    source address, where to copy from
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_src(struct dma350_ch_dev_t *dev, uint32_t src_addr);

/**
 * \brief Sets destination address[31:0] of channel
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] des_addr    destination address, where to copy to
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_des(struct dma350_ch_dev_t *dev, uint32_t des_addr);

/**
 * \brief Sets Channel Priority
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] chprio        Number of priority
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_set_chprio(struct dma350_ch_dev_t *dev, uint8_t chprio);

/**
 * \brief Sets number of copies in the x dimension
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] src_xsize   number of source elements in the x dimension
 * \param[in] des_xsize   number of destination elements in the x dimension
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_xsize16(struct dma350_ch_dev_t *dev, uint16_t src_xsize,
                           uint16_t des_xsize);

/**
 * \brief Sets number of copies in the x dimension
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] src_xsize   number of source elements in the x dimension
 * \param[in] des_xsize   number of destination elements in the x dimension
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_xsize32(struct dma350_ch_dev_t *dev, uint32_t src_xsize,
                           uint32_t des_xsize);

/**
 * \brief Sets number of copies in the y dimension
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] src_ysize   number of source elements in the y dimension
 * \param[in] des_ysize   number of destination elements in the y dimension
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_ysize16(struct dma350_ch_dev_t *dev, uint16_t src_ysize,
                           uint16_t des_ysize);

/**
 * \brief Sets address stride between pixels of adjacent lines
 *
 * \param[in] dev              DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] src_yaddrstride  address stride of source
 * \param[in] des_yaddrstride  address stride of destination
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_yaddrstride(struct dma350_ch_dev_t *dev,
                               uint16_t src_yaddrstride,
                               uint16_t des_yaddrstride);

/**
 * \brief Sets size of each transfer
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] transize    size of each transfer \ref dma350_ch_transize_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_transize(struct dma350_ch_dev_t *dev,
                            enum dma350_ch_transize_t transize);

/**
 * \brief Sets type of operation in the x dimension
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] xtype       type of operation in the x dimension
 *                        \ref dma350_ch_xtype_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_xtype(struct dma350_ch_dev_t *dev,
                         enum dma350_ch_xtype_t xtype);

/**
 * \brief Sets type of operation in the y dimension
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] ytype       type of operation in the y dimension
 *                        \ref dma350_ch_ytype_t
 *
 * \return void
 *
 * \note Set to disabled (default) means 1D copy.
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_ytype(struct dma350_ch_dev_t *dev,
                         enum dma350_ch_ytype_t ytype);

/**
 * \brief Sets type of automatic register reloading
 *
 * \param[in] dev            DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] regreloadtype  type of autoreload \ref dma350_ch_regreloadtype_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_regreloadtype(struct dma350_ch_dev_t *dev,
                                 enum dma350_ch_regreloadtype_t regreloadtype);

/**
 * \brief Configures when STAT_DONE flag is asserted for this command
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] donetype    donetype of command \ref dma350_ch_donetype_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_donetype(struct dma350_ch_dev_t *dev,
                            enum dma350_ch_donetype_t donetype);

/**
 * \brief Enables Done pause use for this command
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_enable_donepause(struct dma350_ch_dev_t *dev);

/**
 * \brief Disables Done pause use for this command
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_disable_donepause(struct dma350_ch_dev_t *dev);

/**
 * \brief Enables Source Trigger Input use for this command
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_enable_srctrigin(struct dma350_ch_dev_t *dev);

/**
 * \brief Disables Source Trigger Input use for this command
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_disable_srctrigin(struct dma350_ch_dev_t *dev);

/**
 * \brief Enables Destination Trigger Input use for this command
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_enable_destrigin(struct dma350_ch_dev_t *dev);

/**
 * \brief Disables Destination Trigger Input use for this command
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_disable_destrigin(struct dma350_ch_dev_t *dev);

/**
 * \brief Enables Trigger Onput use for this command
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_enable_trigout(struct dma350_ch_dev_t *dev);

/**
 * \brief Disables Trigger Onput use for this command
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_disable_trigout(struct dma350_ch_dev_t *dev);

/**
 * \brief Enables GPO use for this command
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_enable_gpo(struct dma350_ch_dev_t *dev);

/**
 * \brief Disables GPO use for this command
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_disable_gpo(struct dma350_ch_dev_t *dev);

/**
 * \brief Enables Stream Interface use for this command
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_enable_stream(struct dma350_ch_dev_t *dev);

/**
 * \brief Disables Stream Interface use for this command
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_disable_stream(struct dma350_ch_dev_t *dev);

/**
 * \brief Sets Source Transfer Memory Attribute and shareability
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] memattr       Attribute field
 * \param[in] shareattr     Shareability attribute field (2 bits)
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_set_srcmemattr(struct dma350_ch_dev_t *dev, uint8_t memattr,
                              uint8_t shareattr);

/**
 * \brief Sets Source Transfer Memory Attribute field[3:0]
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] memattrlo     Attribute field (4 bits)
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_set_srcmemattrlo(struct dma350_ch_dev_t *dev, uint8_t memattrlo);

/**
 * \brief Sets Source Transfer Memory Attribute field[7:4]
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] memattrhi     Attribute field (4 bits)
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_set_srcmemattrhi(struct dma350_ch_dev_t *dev, uint8_t memattrhi);

/**
 * \brief Sets Source Transfer Shareability Attribute
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] shareattr     Shareability attribute field (2 bits)
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_set_srcshareattr(struct dma350_ch_dev_t *dev, uint8_t shareattr);

/**
 * \brief Sets Destination Transfer Memory Attribute and shareability
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] memattr       Attribute field
 * \param[in] shareattr     Shareability attribute field (2 bits)
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_set_desmemattr(struct dma350_ch_dev_t *dev, uint8_t memattr,
                              uint8_t shareattr);

/**
 * \brief Sets Destination Transfer Memory Attribute field[3:0]
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] memattrlo     Attribute field (4 bits)
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_set_desmemattrlo(struct dma350_ch_dev_t *dev, uint8_t memattrlo);

/**
 * \brief Sets Destination Transfer Memory Attribute field[7:4]
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] memattrhi     Attribute field (4 bits)
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_set_desmemattrhi(struct dma350_ch_dev_t *dev, uint8_t memattrhi);

/**
 * \brief Sets Destination Transfer Shareability Attribute
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] shareattr     Attribute field (2 bits)
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_set_desshareattr(struct dma350_ch_dev_t *dev, uint8_t shareattr);

/**
 * \brief Sets source transfer to be secure
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_src_trans_secure(struct dma350_ch_dev_t *dev);

/**
 * \brief Sets source transfer to be non-secure
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_src_trans_nonsecure(struct dma350_ch_dev_t *dev);

/**
 * \brief Sets destination transfer to be secure
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_des_trans_secure(struct dma350_ch_dev_t *dev);

/**
 * \brief Sets destination transfer to be non-secure
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_des_trans_nonsecure(struct dma350_ch_dev_t *dev);

/**
 * \brief Sets source transfer to be privileged
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_src_trans_privileged(struct dma350_ch_dev_t *dev);

/**
 * \brief Sets source transfer to be unprivileged
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_src_trans_unprivileged(struct dma350_ch_dev_t *dev);

/**
 * \brief Sets destination transfer to be privileged
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_des_trans_privileged(struct dma350_ch_dev_t *dev);

/**
 * \brief Sets destination transfer to be unprivileged
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_des_trans_unprivileged(struct dma350_ch_dev_t *dev);

/**
 * \brief Sets Source Max Burst Lenght
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] length        Value of max burst length
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_set_srcmaxburstlen(struct dma350_ch_dev_t *dev, uint8_t length);

/**
 * \brief Sets Destination Max Burst Lenght
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] length        Value of max burst length
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_set_desmaxburstlen(struct dma350_ch_dev_t *dev, uint8_t length);

/**
 * \brief Sets source address increment after each transfer
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] src_xaddr_inc  increment of source address
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_src_xaddr_inc(struct dma350_ch_dev_t *dev,
                                 int16_t src_xaddr_inc);

/**
 * \brief Sets destination address increment after each transfer
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] des_xaddr_inc  increment of destination address
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_des_xaddr_inc(struct dma350_ch_dev_t *dev,
                                 int16_t des_xaddr_inc);

/**
 * \brief Sets source and destination address increment after each transfer
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] src_xaddr_inc  increment of source address
 * \param[in] des_xaddr_inc  increment of destination address
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_xaddr_inc(struct dma350_ch_dev_t *dev, int16_t src_xaddr_inc,
                             int16_t des_xaddr_inc);

/**
 * \brief Sets fill value
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] fill_value  fill value
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_fill_value(struct dma350_ch_dev_t *dev, uint32_t fill_value);

/**
 * \brief Commands a channel of DMA350 DMA.
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] cmd         DMA350 DMA channel command \ref dma350_ch_cmd_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_cmd(struct dma350_ch_dev_t *dev, enum dma350_ch_cmd_t cmd);

/**
 * \brief Gets current command of DMA350 DMA channel.
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return Returns channel command specified in \ref dma350_ch_cmd_t
 *
 * \note This function doesn't check if dev is NULL.
 */
__STATIC_INLINE
enum dma350_ch_cmd_t dma350_ch_get_cmd(struct dma350_ch_dev_t *dev);

/**
 * \brief Enables Interrupt for DMA350 DMA channel
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] intr        Interrupt(s) to enable \ref dma350_ch_intr_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_enable_intr(struct dma350_ch_dev_t *dev,
                           enum dma350_ch_intr_t intr);

/**
 * \brief Disables Interrupt for DMA350 DMA channel
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] intr        Interrupt(s) to disable \ref dma350_ch_intr_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_disable_intr(struct dma350_ch_dev_t *dev,
                            enum dma350_ch_intr_t intr);

/**
 * \brief Get Interrupt status of DMA350 DMA channel
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] intr        Interrupt to check \ref dma350_ch_intr_t
 *
 * \return Returns True if interrupt is active, False otherwise
 *
 * \note Interrupt had to be enabled prior for this function to return True.
 *       This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
bool dma350_ch_is_intr_set(struct dma350_ch_dev_t *dev,
                           enum dma350_ch_intr_t intr);

/**
 * \brief Get Status bit of DMA350 DMA channel
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] stat        Status bit(s) to check \ref dma350_ch_stat_t
 *
 * \return Returns True if status of interrupt is active, False otherwise.
 *
 * \note Status of interrupt is independent from whether the interrupt is
 *       enabled.
 *       This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
bool dma350_ch_is_stat_set(struct dma350_ch_dev_t *dev,
                           enum dma350_ch_stat_t stat);

/**
 * \brief Clear Status bit of DMA350 DMA channel
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] stat        Status bit(s) to clear \ref dma350_ch_stat_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_clear_stat(struct dma350_ch_dev_t *dev,
                          enum dma350_ch_stat_t stat);

/**
 * \brief Set automatic command reload count for DMA350 DMA channel
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] cnt         Number of command reloads
 *
 * \return void
 *
 * \note This command implicitly disables infinite command reload.
 *       This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_autocfg_restart_cnt(struct dma350_ch_dev_t *dev,
                                       uint16_t cnt);

/**
 * \brief Set infinite command reload for DMA350 DMA channel
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return void
 *
 * \note This command implicitly clears command reload count.
 *       Infinite restart can be cleared by setting counter to 0.
 *       This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_autocfg_restart_inf(struct dma350_ch_dev_t *dev);

/**
 * \brief Set source template size of DMA350 DMA channel
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] size        Number of 1s in template minus 1. 0 means disabled.
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_tmplt_src_size(struct dma350_ch_dev_t *dev, uint32_t size);

/**
 * \brief Set destination template size of DMA350 DMA channel
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] size        Number of 1s in template minus 1. 0 means disabled.
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_tmplt_des_size(struct dma350_ch_dev_t *dev, uint32_t size);

/**
 * \brief Set source template of DMA350 DMA channel
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] tmplt       Template of the source side. Bit[0] is ignored, it is
 *                        always 1.
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_tmplt_src(struct dma350_ch_dev_t *dev, uint32_t tmplt);

/**
 * \brief Set destination template of DMA350 DMA channel
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] tmplt       Template of the destination side. Bit[0] is ignored,
 *                        it is always 1.
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
void dma350_ch_set_tmplt_des(struct dma350_ch_dev_t *dev, uint32_t tmplt);

/**
 * \brief Get source template of DMA350 DMA channel
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return Template of the source side
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
uint32_t dma350_ch_get_tmplt_src(struct dma350_ch_dev_t *dev);

/**
 * \brief Get destination template of DMA350 DMA channel
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return Template of the destination side
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
uint32_t dma350_ch_get_tmplt_des(struct dma350_ch_dev_t *dev);

/**
 * \brief Sets Source Trigger Input Select
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] srctriginsel  Input Select value
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_set_srctriginsel(struct dma350_ch_dev_t *dev,
                                uint8_t srctriginsel);

/**
 * \brief Sets Source Trigger Input Type
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] type          Input type \ref dma350_ch_srctrigintype_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_set_srctrigintype(struct dma350_ch_dev_t *dev,
                                 enum dma350_ch_srctrigintype_t type);

/**
 * \brief Sets Source Trigger Input Mode
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] mode          Mode \ref dma350_ch_srctriginmode_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_set_srctriginmode(struct dma350_ch_dev_t *dev,
                                 enum dma350_ch_srctriginmode_t mode);

/**
 * \brief Sets Source Trigger Input Default Transfer Size
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] blksize       Size value
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_set_srctriginblksize(struct dma350_ch_dev_t *dev,
                                    uint8_t blksize);

/**
 * \brief Sets Destination Trigger Input Select
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] destriginsel  Input Select value
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_set_destriginsel(struct dma350_ch_dev_t *dev,
                                uint8_t destriginsel);

/**
 * \brief Sets Destination Trigger Input Type
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] type          Input type \ref dma350_ch_destrigintype_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_set_destrigintype(struct dma350_ch_dev_t *dev,
                                 enum dma350_ch_destrigintype_t type);

/**
 * \brief Sets Destination Trigger Input Mode
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] mode          Mode \ref dma350_ch_destriginmode_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_set_destriginmode(struct dma350_ch_dev_t *dev,
                                 enum dma350_ch_destriginmode_t mode);

/**
 * \brief Sets Destination Trigger Input Default Transfer Size
 *
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] blksize       Size value
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_set_destriginblksize(struct dma350_ch_dev_t *dev,
                                    uint8_t blksize);

/**
 * \brief Sets Trigger Output Select
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] trigoutsel    Select value
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_set_trigoutsel(struct dma350_ch_dev_t *dev, uint8_t trigoutsel);

/**
 * \brief Sets Trigger Output Type
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] type          Output type \ref dma350_ch_trigouttype_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_set_trigouttype(struct dma350_ch_dev_t *dev,
                               enum dma350_ch_trigouttype_t type);

/**
 * \brief Sets which GPO ports 0-31 are enabled to change at the beginning
 *        of current DMA command
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] gpoen0        Enable mask value
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_set_gpoen0(struct dma350_ch_dev_t *dev, uint32_t gpoen0);

/**
 * \brief Sets the value to be driven on the GPO ports 0-31 that are enabled
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] gpoval0       General Purpose Output Value GPO[31:0]
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_set_gpoval0(struct dma350_ch_dev_t *dev, uint32_t gpoval0);

/**
 * \brief Gets the current value of the GPO ports, bits 0-31
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return General Purpose Output Value GPO[31:0]
 */
__STATIC_INLINE
uint32_t dma350_ch_get_gpoval0(struct dma350_ch_dev_t *dev);

/**
 * \brief Sets Stream Interface operation Type
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] type          Type \ref dma350_ch_streamtype_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_set_streamtype(struct dma350_ch_dev_t *dev,
                              enum dma350_ch_streamtype_t type);

/**
 * \brief Sets Link Address Read Transfer Memory Attribute[3:0] field
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] memattrlo     Attribute field
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_set_linkmemattrlo(struct dma350_ch_dev_t *dev,
                                 uint8_t memattrlo);

/**
 * \brief Sets Link Address Read Transfer Memory Attribute[7:4] field
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] memattrlo     Attribute field
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_set_linkmemattrhi(struct dma350_ch_dev_t *dev,
                                 uint8_t memattrhi);

/**
 * \brief Sets Link Address Transfer Shareability Attribute
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] shareattr     Attribute field
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_set_linkshareattr(struct dma350_ch_dev_t *dev,
                                 uint8_t shareattr);

/**
 * \brief Enables Link Address
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_enable_linkaddr(struct dma350_ch_dev_t *dev);

/**
 * \brief Disables Link Address
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_disable_linkaddr(struct dma350_ch_dev_t *dev);

/**
 * \brief Sets Link Address Pointer [31:2]
 *
 * \param[in] dev           DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] linkaddr      Memory location of the destination
 *
 * \return void
 */
__STATIC_INLINE
void dma350_ch_set_linkaddr32(struct dma350_ch_dev_t *dev, uint32_t linkaddr);

/**
 * \brief Checks if DMA350 DMA channel is busy.
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return Returns True if channel is busy, False if ready
 *
 * \note This function doesn't check if dev is NULL.
 */
__STATIC_INLINE
bool dma350_ch_is_busy(struct dma350_ch_dev_t *dev);

/**
 * \brief Checks if DMA350 DMA channel is ready.
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return Returns True if channel is ready, False if busy
 *
 * \note This function doesn't check if dev is NULL.
 */
__STATIC_INLINE
bool dma350_ch_is_ready(struct dma350_ch_dev_t *dev);

/**
 * \brief Gets status of DMA350 DMA channel.
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return Returns status specified in \ref dma350_ch_status_t
 *
 * \note This function doesn't check if dev is NULL.
 */
__STATIC_INLINE
union dma350_ch_status_t dma350_ch_get_status(struct dma350_ch_dev_t *dev);

/**
 * \brief Waits for DMA350 DMA channel to be ready, return status when done.
 *
 * \param[in] dev         DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return Returns status specified in \ref dma350_ch_status_t
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
union dma350_ch_status_t dma350_ch_wait_status(struct dma350_ch_dev_t *dev);

/**
 * \brief Sets REGCLEAR header bit in the command structure which clears all
 *        previous settings from the channel registers
 *
 * \param[in] cmldink_cfg   Command structure for DMA350 DMA command linking
 *                          feature \ref dma350_cmdlink_gencfg_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_regclear(struct dma350_cmdlink_gencfg_t *cmdlink_cfg);

/**
 * \brief Enables Interrupt for DMA350 DMA channel in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] intr        Interrupt(s) to enable \ref dma350_ch_intr_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_enable_intr(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                enum dma350_ch_intr_t intr);

/**
 * \brief Disables Interrupt for DMA350 DMA channel in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] intr        Interrupt(s) to disable \ref dma350_ch_intr_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_disable_intr(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                 enum dma350_ch_intr_t intr);

/**
 * \brief Sets Transfer Enitity Size in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] transize      size in bytes \ref dma350_ch_transize_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_transize(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                 enum dma350_ch_transize_t transize);

/**
 * \brief Sets Channel Priority in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] chprio        Number of priority
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_chprio(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                               uint8_t chprio);

/**
 * \brief Sets operation type for X direction in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] xtype         type \ref dma350_ch_xtype_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_xtype(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                              enum dma350_ch_xtype_t xtype);

/**
 * \brief Sets operation type for Y direction in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] ytype         type \ref dma350_ch_ytype_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_ytype(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                              enum dma350_ch_ytype_t ytype);

/**
 * \brief Sets Automatic Register reload type in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] regreloadtype Reload type \ref dma350_ch_regreloadtype_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_regreloadtype(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
    enum dma350_ch_regreloadtype_t regreloadtype);

/**
 * \brief Sets when the STAT_DONE status flag is asserted during the command
 *        operation in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] donetype      Done type selection \ref dma350_ch_donetype_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_donetype(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                 enum dma350_ch_donetype_t donetype);

/**
 * \brief Enables the automatic pause request for the current DMA operation
 *        if the STAT_DONE flag is asserted in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_enable_donepause(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg);

/**
 * \brief Disables the automatic pause request for the current DMA operation
 *        if the STAT_DONE flag is asserted in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_disable_donepause(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg);

/**
 * \brief Enables Source Trigger Input use in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_enable_srctrigin(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg);

/**
 * \brief Disables Source Trigger Input use in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_disable_srctrigin(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg);

/**
 * \brief Enables Destination Trigger Input use in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_enable_destrigin(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg);

/**
 * \brief Disables Destination Trigger Input use in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_disable_destrigin(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg);

/**
 * \brief Enables Trigger Output use in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_enable_trigout(struct dma350_cmdlink_gencfg_t *cmdlink_cfg);

/**
 * \brief Disables Trigger Output for use in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_disable_trigout(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg);

/**
 * \brief Enables GPO use in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_enable_gpo(struct dma350_cmdlink_gencfg_t *cmdlink_cfg);

/**
 * \brief Disables GPO use in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_disable_gpo(struct dma350_cmdlink_gencfg_t *cmdlink_cfg);

/**
 * \brief Enables Stream Interface use in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_enable_stream(struct dma350_cmdlink_gencfg_t *cmdlink_cfg);

/**
 * \brief Disables Stream Interface use in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_disable_stream(struct dma350_cmdlink_gencfg_t *cmdlink_cfg);

/**
 * \brief Sets Source Address[31:0] in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] src_addr      Memory location of the source
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_srcaddr32(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                  uint32_t src_addr);

/**
 * \brief Sets Destination Address[31:0] in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] des_addr      Memory location of the destination
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_desaddr32(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                  uint32_t des_addr);

/**
 * \brief Sets the number of data in the command structure units
 *        copied during the DMA command up to 16 bits in the X dimension
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] src_xsize     Source number of transfers
 * \param[in] des_xsize     Destination number of transfers
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_xsize16(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                uint16_t src_xsize, uint16_t des_xsize);

/**
 * \brief Sets the number of data in the command structure units
 *        copied during the DMA command up to 32 bits in the X dimension
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] src_xsize     Source number of transfers
 * \param[in] des_xsize     Destination number of transfers
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_xsize32(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                uint32_t src_xsize, uint32_t des_xsize);

/**
 * \brief Sets Source Transfer Memory Attribute field[3:0]
 *        in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] memattrlo     Attribute field
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_srcmemattrlo(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t memattrlo);

/**
 * \brief Sets Source Transfer Memory Attribute field[7:4]
 *        in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] memattrhi     Attribute field
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_srcmemattrhi(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t memattrhi);

/**
 * \brief Sets Source Transfer Shareability Attribute in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] shareattr     Attribute field
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_srcshareattr(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t shareattr);

/**
 * \brief Sets Destination Transfer Memory Attribute field[3:0] in the command
 *        structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] memattrlo     Attribute field
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_desmemattrlo(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t memattrlo);

/**
 * \brief Sets Destination Transfer Memory Attribute field[7:4] in the command
 *        structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] memattrhi     Attribute field
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_desmemattrhi(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t memattrhi);

/**
 * \brief Sets Destination Transfer Shareability Attribute in the command
 *        structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] shareattr     Attribute field
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_desshareattr(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t shareattr);

/**
 * \brief Sets Source Transfer Attribute to secure in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_src_trans_secure(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg);

/**
 * \brief Sets Source Transfer Attribute to non secure in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_src_trans_nonsecure(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg);

/**
 * \brief Sets Destination Transfer Attribute to secure in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_des_trans_secure(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg);

/**
 * \brief Sets Destination Transfer Attribute to non secure
 *        in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_des_trans_nonsecure(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg);

/**
 * \brief Sets Source Transfer Privilege to privileged
 *        in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_src_trans_privileged(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg);

/**
 * \brief Sets Source Transfer Privilege to unprivileged
 *        in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_src_trans_unprivileged(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg);

/**
 * \brief Sets Destination Transfer Privilege to privileged
 *        in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_des_trans_privileged(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg);

/**
 * \brief Sets Destination Transfer Privilege to unprivileged
 *        in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_des_trans_unprivileged(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg);

/**
 * \brief Sets Source Max Burst Lenght in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] length        Value of max burst length
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_srcmaxburstlen(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t length);

/**
 * \brief Sets Destination Max Burst Lenght in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] length        Value of max burst length
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_desmaxburstlen(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t length);

/**
 * \brief Sets the increment value in the command structure that is used to
 *        update the source and the destination addresses after each
 *        transfered data unit
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] src_xaddrinc  Source X dimension address increment value
 * \param[in] des_xaddrinc  Destination X dimension address increment value
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_xaddrinc(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                 uint16_t src_xaddrinc, uint16_t des_xaddrinc);

/**
 * \brief Sets the increment value in the command structure that is used to
 *        update the source and the destination line base addresses after
 *        each line is transferred in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] src_yaddrstride Source Address Stride value
 * \param[in] des_yaddrstride Destination Address Stride value
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_yaddrstride(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                    uint16_t src_yaddrstride,
                                    uint16_t des_yaddrstride);

/**
 * \brief Sets the value in the command structure to be used to fill the
 *        remaining part of the destination memory area
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] fillval       Fill pattern value
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_fillval(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                uint32_t fillval);

/**
 * \brief Sets the number of data units copied during the DMA command
 *        up to 16 bits in the Y dimension in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] src_ysize   number of source elements in the y dimension
 * \param[in] des_ysize   number of destination elements in the y dimension
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_ysize16(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                uint16_t src_ysize, uint16_t des_ysize);

/**
 * \brief Sets template size in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] scr_tmpltsize Source Template size in numbers
 * \param[in] des_tmpltsize Destination Template size in numbers
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_tmpltsize(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                  uint8_t scr_tmpltsize, uint8_t des_tmpltsize);

/**
 * \brief Sets the template pattern in the command structure
 *        used for reading the source memory area
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] src_tmplt     Source Packing Template
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_srctmplt(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                 uint32_t src_tmplt);

/**
 * \brief Sets the template pattern in the command structure
 *        used for reading the destination memory area
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] des_tmplt     Destination Packing Template
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_destmplt(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                 uint32_t des_tmplt);

/**
 * \brief Sets Source Trigger Input Select in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] srctriginsel  Input Select value
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_srctriginsel(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t srctriginsel);

/**
 * \brief Sets Source Trigger Input Type in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] type          Input type \ref dma350_ch_srctrigintype_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_srctrigintype(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
    enum dma350_ch_srctrigintype_t type);

/**
 * \brief Sets Source Trigger Input Mode in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] mode          Mode \ref dma350_ch_srctriginmode_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_srctriginmode(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
    enum dma350_ch_srctriginmode_t mode);

/**
 * \brief Sets Source Trigger Input Default Transfer Size
 *        in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] blksize       Size value
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_srctriginblksize(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t blksize);

/**
 * \brief Sets Destination Trigger Input Select in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] destriginsel  Input Select value
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_destriginsel(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t destriginsel);

/**
 * \brief Sets Destination Trigger Input Type in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] type          Input type \ref dma350_ch_destrigintype_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_destrigintype(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
    enum dma350_ch_destrigintype_t type);

/**
 * \brief Sets Destination Trigger Input Mode in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] mode          Mode \ref dma350_ch_destriginmode_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_destriginmode(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
    enum dma350_ch_destriginmode_t mode);

/**
 * \brief Sets Destination Trigger Input Default Transfer Size
 *        in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] blksize       Size value
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_destriginblksize(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t blksize);

/**
 * \brief Sets Trigger Output Select in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] trigoutsel    Select value
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_trigoutsel(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                   uint8_t trigoutsel);

/**
 * \brief Sets Trigger Output Type in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] type          Output type \ref dma350_ch_trigouttype_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_trigouttype(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                    enum dma350_ch_trigouttype_t type);

/**
 * \brief Sets which GPO ports 0-31 in the command structure are enabled
 *        to change at the beginning of current DMA command
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] gpoen0        Enable mask value
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_gpoen0(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                               uint32_t gpoen0);

/**
 * \brief Sets the value to be driven on the GPO ports 0-31 thar are enabled
 *        in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] gpoval0       General Purpose Output Value GPO[31:0]
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_gpoval0(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                uint32_t gpoval0);

/**
 * \brief Sets Stream Interface operation Type in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] type          Type \ref dma350_ch_streamtype_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_streamtype(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                   enum dma350_ch_streamtype_t type);

/**
 * \brief Sets Link Address Read Transfer Memory Attribute[3:0] field
 *        in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] memattrlo     Attribute field
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_linkmemattrlo(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t memattrlo);

/**
 * \brief Sets Link Address Read Transfer Memory Attribute[7:4] field
 *        in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] memattrlo     Attribute field
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_linkmemattrhi(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t memattrhi);

/**
 * \brief Sets Link Address Transfer Shareability Attribute
 *        in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] shareattr     Attribute field
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_linkshareattr(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t shareattr);

/**
 * \brief Sets Automatic Command Restart Counter in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] cnt           Counter value
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_cmdrestartcnt(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint16_t cnt);

/**
 * \brief Enables Infinite Automatic Command Restart in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_enable_cmdrestartinfen(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg);

/**
 * \brief Disables Infinite Automatic Command Restart in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_disable_cmdrestartinfen(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg);

/**
 * \brief Enables Link Address in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_enable_linkaddr(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg);

/**
 * \brief Disables Link Address in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_disable_linkaddr(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg);

/**
 * \brief Sets Link Address Pointer [31:2] in the command structure
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[in] linkaddr      Memory location of the destination
 *
 * \return void
 */
__STATIC_INLINE
void dma350_cmdlink_set_linkaddr32(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                   uint32_t linkaddr);

/**
 * \brief Get command from a command structure
 *        that can be fetched by the channel
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 * \param[inout] buffer     Pointer to the buffer to place the generated cmd to
 * \param[in] buffer_end    Pointer to the next memory address after cmd buffer
 *
 * \return next memory address in buffer after generated cmd, or NUll on error
 */
uint32_t *dma350_cmdlink_generate(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                  uint32_t *buffer, uint32_t *buffer_end);

/**
 * \brief Initialize command structure for dma command linking feature
 *
 * \param[in] cmldink_cfg   DMA350 command structure for command linking feature
 *                          \ref dma350_cmdlink_gencfg_t
 *
 * \return void
 */
void dma350_cmdlink_init(struct dma350_cmdlink_gencfg_t *cmdlink_cfg);

__STATIC_INLINE
bool dma350_ch_is_init(const struct dma350_ch_dev_t *dev)
{
    return dev->data.state == DMA350_CH_INITIALIZED;
}

__STATIC_INLINE
void dma350_ch_set_src(struct dma350_ch_dev_t *dev, uint32_t src_addr)
{
    dev->cfg.ch_base->CH_SRCADDR = src_addr;
}

__STATIC_INLINE
void dma350_ch_set_des(struct dma350_ch_dev_t *dev, uint32_t des_addr)
{
    dev->cfg.ch_base->CH_DESADDR = des_addr;
}

__STATIC_INLINE
void dma350_ch_set_xsize16(struct dma350_ch_dev_t *dev, uint16_t src_xsize,
                           uint16_t des_xsize)
{
    dev->cfg.ch_base->CH_XSIZE =
        ((des_xsize & 0x0000FFFFUL) << 16U) | (src_xsize & 0x0000FFFFUL);
    dev->cfg.ch_base->CH_XSIZEHI = 0;
}

__STATIC_INLINE
void dma350_ch_set_xsize32(struct dma350_ch_dev_t *dev, uint32_t src_xsize,
                           uint32_t des_xsize)
{
    dev->cfg.ch_base->CH_XSIZE =
        ((des_xsize & 0x0000FFFFUL) << 16U) | (src_xsize & 0x0000FFFFUL);
    dev->cfg.ch_base->CH_XSIZEHI =
        (des_xsize & 0xFFFF0000UL) | ((src_xsize & 0xFFFF0000UL) >> 16U);
}

__STATIC_INLINE
void dma350_ch_set_ysize16(struct dma350_ch_dev_t *dev, uint16_t src_ysize,
                           uint16_t des_ysize)
{
    dev->cfg.ch_base->CH_YSIZE =
        ((des_ysize & 0x0000FFFFUL) << 16U) | (src_ysize & 0x0000FFFFUL);
}

__STATIC_INLINE
void dma350_ch_set_yaddrstride(struct dma350_ch_dev_t *dev,
                               uint16_t src_yaddrstride,
                               uint16_t des_yaddrstride)
{
    dev->cfg.ch_base->CH_YADDRSTRIDE =
        ((des_yaddrstride & 0x0000FFFFUL) << 16U) |
        (src_yaddrstride & 0x0000FFFFUL);
}

__STATIC_INLINE
void dma350_ch_set_transize(struct dma350_ch_dev_t *dev,
                            enum dma350_ch_transize_t transize)
{
    dev->cfg.ch_base->CH_CTRL =
        (dev->cfg.ch_base->CH_CTRL & (~DMA_CH_CTRL_TRANSIZE_Msk)) |
        (transize & DMA_CH_CTRL_TRANSIZE_Msk);
}

__STATIC_INLINE
void dma350_ch_set_chprio(struct dma350_ch_dev_t *dev, uint8_t chprio)
{
    SET_FIELD(dev->cfg.ch_base->CH_CTRL, chprio, DMA_CH_CTRL_CHPRIO_Pos,
              DMA_CH_CTRL_CHPRIO_Msk);
}

__STATIC_INLINE
void dma350_ch_set_xtype(struct dma350_ch_dev_t *dev,
                         enum dma350_ch_xtype_t xtype)
{
    dev->cfg.ch_base->CH_CTRL =
        (dev->cfg.ch_base->CH_CTRL & (~DMA_CH_CTRL_XTYPE_Msk)) |
        (xtype & DMA_CH_CTRL_XTYPE_Msk);
}

__STATIC_INLINE
void dma350_ch_set_ytype(struct dma350_ch_dev_t *dev,
                         enum dma350_ch_ytype_t ytype)
{
    dev->cfg.ch_base->CH_CTRL =
        (dev->cfg.ch_base->CH_CTRL & (~DMA_CH_CTRL_YTYPE_Msk)) |
        (ytype & DMA_CH_CTRL_YTYPE_Msk);
}

__STATIC_INLINE
void dma350_ch_set_regreloadtype(struct dma350_ch_dev_t *dev,
                                 enum dma350_ch_regreloadtype_t regreloadtype)
{
    dev->cfg.ch_base->CH_CTRL =
        (dev->cfg.ch_base->CH_CTRL & (~DMA_CH_CTRL_REGRELOADTYPE_Msk)) |
        (regreloadtype & DMA_CH_CTRL_REGRELOADTYPE_Msk);
}

__STATIC_INLINE
void dma350_ch_set_donetype(struct dma350_ch_dev_t *dev,
                            enum dma350_ch_donetype_t donetype)
{
    dev->cfg.ch_base->CH_CTRL =
        (dev->cfg.ch_base->CH_CTRL & (~DMA_CH_CTRL_DONETYPE_Msk)) |
        (donetype & DMA_CH_CTRL_DONETYPE_Msk);
}

__STATIC_INLINE
void dma350_ch_enable_donepause(struct dma350_ch_dev_t *dev)
{
    dev->cfg.ch_base->CH_CTRL =
        dev->cfg.ch_base->CH_CTRL | DMA_CH_CTRL_DONEPAUSEEN_Msk;
}

__STATIC_INLINE
void dma350_ch_disable_donepause(struct dma350_ch_dev_t *dev)
{
    dev->cfg.ch_base->CH_CTRL =
        dev->cfg.ch_base->CH_CTRL & (~DMA_CH_CTRL_DONEPAUSEEN_Msk);
}

__STATIC_INLINE
void dma350_ch_enable_srctrigin(struct dma350_ch_dev_t *dev)
{
    dev->cfg.ch_base->CH_CTRL =
        dev->cfg.ch_base->CH_CTRL | DMA_CH_CTRL_USESRCTRIGIN_Msk;
}

__STATIC_INLINE
void dma350_ch_disable_srctrigin(struct dma350_ch_dev_t *dev)
{
    dev->cfg.ch_base->CH_CTRL =
        dev->cfg.ch_base->CH_CTRL & (~DMA_CH_CTRL_USESRCTRIGIN_Msk);
}

__STATIC_INLINE
void dma350_ch_enable_destrigin(struct dma350_ch_dev_t *dev)
{
    dev->cfg.ch_base->CH_CTRL =
        dev->cfg.ch_base->CH_CTRL | DMA_CH_CTRL_USEDESTRIGIN_Msk;
}

__STATIC_INLINE
void dma350_ch_disable_destrigin(struct dma350_ch_dev_t *dev)
{
    dev->cfg.ch_base->CH_CTRL =
        dev->cfg.ch_base->CH_CTRL & (~DMA_CH_CTRL_USEDESTRIGIN_Msk);
}

__STATIC_INLINE
void dma350_ch_enable_trigout(struct dma350_ch_dev_t *dev)
{
    dev->cfg.ch_base->CH_CTRL =
        dev->cfg.ch_base->CH_CTRL | DMA_CH_CTRL_USETRIGOUT_Msk;
}

__STATIC_INLINE
void dma350_ch_disable_trigout(struct dma350_ch_dev_t *dev)
{
    dev->cfg.ch_base->CH_CTRL =
        dev->cfg.ch_base->CH_CTRL & (~DMA_CH_CTRL_USETRIGOUT_Msk);
}

__STATIC_INLINE
void dma350_ch_enable_gpo(struct dma350_ch_dev_t *dev)
{
    dev->cfg.ch_base->CH_CTRL =
        dev->cfg.ch_base->CH_CTRL | DMA_CH_CTRL_USEGPO_Msk;
}

__STATIC_INLINE
void dma350_ch_disable_gpo(struct dma350_ch_dev_t *dev)
{
    dev->cfg.ch_base->CH_CTRL =
        dev->cfg.ch_base->CH_CTRL & (~DMA_CH_CTRL_USEGPO_Msk);
}

__STATIC_INLINE
void dma350_ch_enable_stream(struct dma350_ch_dev_t *dev)
{
    dev->cfg.ch_base->CH_CTRL =
        dev->cfg.ch_base->CH_CTRL | DMA_CH_CTRL_USESTREAM_Msk;
}

__STATIC_INLINE
void dma350_ch_disable_stream(struct dma350_ch_dev_t *dev)
{
    dev->cfg.ch_base->CH_CTRL =
        dev->cfg.ch_base->CH_CTRL & (~DMA_CH_CTRL_USESTREAM_Msk);
}

__STATIC_INLINE
void dma350_ch_set_srcmemattr(struct dma350_ch_dev_t *dev, uint8_t memattr,
                              uint8_t shareattr)
{
    dev->cfg.ch_base->CH_SRCTRANSCFG =
        /* Only set Lo, Hi, and Share attributes */
        (dev->cfg.ch_base->CH_SRCTRANSCFG &
         ((~DMA_CH_SRCTRANSCFG_SRCMEMATTRLO_Msk) |
          (~DMA_CH_SRCTRANSCFG_SRCMEMATTRHI_Msk) |
          (~DMA_CH_SRCTRANSCFG_SRCSHAREATTR_Msk))
         /* memattr already has Lo and Hi values in correct order */
         ) |
        ((memattr & 0x000000FFUL) << DMA_CH_SRCTRANSCFG_SRCMEMATTRLO_Pos)
        /* Add shareability attribute */
        | ((shareattr & 0x00000003UL) << DMA_CH_SRCTRANSCFG_SRCSHAREATTR_Pos);
}

__STATIC_INLINE
void dma350_ch_set_srcmemattrlo(struct dma350_ch_dev_t *dev, uint8_t memattrlo)
{
    SET_FIELD(dev->cfg.ch_base->CH_SRCTRANSCFG, memattrlo,
              DMA_CH_SRCTRANSCFG_SRCMEMATTRLO_Pos,
              DMA_CH_SRCTRANSCFG_SRCMEMATTRLO_Msk);
}

__STATIC_INLINE
void dma350_ch_set_srcmemattrhi(struct dma350_ch_dev_t *dev, uint8_t memattrhi)
{
    SET_FIELD(dev->cfg.ch_base->CH_SRCTRANSCFG, memattrhi,
              DMA_CH_SRCTRANSCFG_SRCMEMATTRHI_Pos,
              DMA_CH_SRCTRANSCFG_SRCMEMATTRHI_Msk);
}

__STATIC_INLINE
void dma350_ch_set_srcshareattr(struct dma350_ch_dev_t *dev, uint8_t shareattr)
{
    SET_FIELD(dev->cfg.ch_base->CH_SRCTRANSCFG, shareattr,
              DMA_CH_SRCTRANSCFG_SRCSHAREATTR_Pos,
              DMA_CH_SRCTRANSCFG_SRCSHAREATTR_Msk);
}

__STATIC_INLINE
void dma350_ch_set_desmemattr(struct dma350_ch_dev_t *dev, uint8_t memattr,
                              uint8_t shareattr)
{
    dev->cfg.ch_base->CH_DESTRANSCFG =
        /* Only set Lo, Hi, and Share attributes */
        (dev->cfg.ch_base->CH_DESTRANSCFG &
         ((~DMA_CH_DESTRANSCFG_DESMEMATTRLO_Msk) |
          (~DMA_CH_DESTRANSCFG_DESMEMATTRHI_Msk) |
          (~DMA_CH_DESTRANSCFG_DESSHAREATTR_Msk))
         /* memattr already has Lo and Hi values in correct order */
         ) |
        ((memattr & 0x000000FFUL) << DMA_CH_DESTRANSCFG_DESMEMATTRLO_Pos)
        /* Add shareability attribute */
        | ((shareattr & 0x00000003UL) << DMA_CH_DESTRANSCFG_DESSHAREATTR_Pos);
}

__STATIC_INLINE
void dma350_ch_set_desmemattrlo(struct dma350_ch_dev_t *dev, uint8_t memattrlo)
{
    SET_FIELD(dev->cfg.ch_base->CH_DESTRANSCFG, memattrlo,
              DMA_CH_DESTRANSCFG_DESMEMATTRLO_Pos,
              DMA_CH_DESTRANSCFG_DESMEMATTRLO_Msk);
}

__STATIC_INLINE
void dma350_ch_set_desmemattrhi(struct dma350_ch_dev_t *dev, uint8_t memattrhi)
{
    SET_FIELD(dev->cfg.ch_base->CH_DESTRANSCFG, memattrhi,
              DMA_CH_DESTRANSCFG_DESMEMATTRHI_Pos,
              DMA_CH_DESTRANSCFG_DESMEMATTRHI_Msk);
}

__STATIC_INLINE
void dma350_ch_set_desshareattr(struct dma350_ch_dev_t *dev, uint8_t shareattr)
{
    SET_FIELD(dev->cfg.ch_base->CH_DESTRANSCFG, shareattr,
              DMA_CH_DESTRANSCFG_DESSHAREATTR_Pos,
              DMA_CH_DESTRANSCFG_DESSHAREATTR_Msk);
}

__STATIC_INLINE
void dma350_ch_set_src_trans_secure(struct dma350_ch_dev_t *dev)
{
    dev->cfg.ch_base->CH_SRCTRANSCFG = dev->cfg.ch_base->CH_SRCTRANSCFG &
                                       (~DMA_CH_SRCTRANSCFG_SRCNONSECATTR_Msk);
}

__STATIC_INLINE
void dma350_ch_set_src_trans_nonsecure(struct dma350_ch_dev_t *dev)
{
    dev->cfg.ch_base->CH_SRCTRANSCFG =
        dev->cfg.ch_base->CH_SRCTRANSCFG | DMA_CH_SRCTRANSCFG_SRCNONSECATTR_Msk;
}

__STATIC_INLINE
void dma350_ch_set_des_trans_secure(struct dma350_ch_dev_t *dev)
{
    dev->cfg.ch_base->CH_DESTRANSCFG = dev->cfg.ch_base->CH_DESTRANSCFG &
                                       (~DMA_CH_DESTRANSCFG_DESNONSECATTR_Msk);
}

__STATIC_INLINE
void dma350_ch_set_des_trans_nonsecure(struct dma350_ch_dev_t *dev)
{
    dev->cfg.ch_base->CH_DESTRANSCFG =
        dev->cfg.ch_base->CH_DESTRANSCFG | DMA_CH_DESTRANSCFG_DESNONSECATTR_Msk;
}

__STATIC_INLINE
void dma350_ch_set_src_trans_privileged(struct dma350_ch_dev_t *dev)
{
    dev->cfg.ch_base->CH_SRCTRANSCFG =
        dev->cfg.ch_base->CH_SRCTRANSCFG | DMA_CH_SRCTRANSCFG_SRCPRIVATTR_Msk;
}

__STATIC_INLINE
void dma350_ch_set_src_trans_unprivileged(struct dma350_ch_dev_t *dev)
{
    dev->cfg.ch_base->CH_SRCTRANSCFG = dev->cfg.ch_base->CH_SRCTRANSCFG &
                                       (~DMA_CH_SRCTRANSCFG_SRCPRIVATTR_Msk);
}

__STATIC_INLINE
void dma350_ch_set_des_trans_privileged(struct dma350_ch_dev_t *dev)
{
    dev->cfg.ch_base->CH_DESTRANSCFG =
        dev->cfg.ch_base->CH_DESTRANSCFG | DMA_CH_DESTRANSCFG_DESPRIVATTR_Msk;
}

__STATIC_INLINE
void dma350_ch_set_des_trans_unprivileged(struct dma350_ch_dev_t *dev)
{
    dev->cfg.ch_base->CH_DESTRANSCFG = dev->cfg.ch_base->CH_DESTRANSCFG &
                                       (~DMA_CH_DESTRANSCFG_DESPRIVATTR_Msk);
}

__STATIC_INLINE
void dma350_ch_set_srcmaxburstlen(struct dma350_ch_dev_t *dev, uint8_t length)
{
    SET_FIELD(dev->cfg.ch_base->CH_SRCTRANSCFG, length,
              DMA_CH_SRCTRANSCFG_SRCMAXBURSTLEN_Pos,
              DMA_CH_SRCTRANSCFG_SRCMAXBURSTLEN_Msk);
}

__STATIC_INLINE
void dma350_ch_set_desmaxburstlen(struct dma350_ch_dev_t *dev, uint8_t length)
{
    SET_FIELD(dev->cfg.ch_base->CH_DESTRANSCFG, length,
              DMA_CH_DESTRANSCFG_DESMAXBURSTLEN_Pos,
              DMA_CH_DESTRANSCFG_DESMAXBURSTLEN_Msk);
}

__STATIC_INLINE
void dma350_ch_set_src_xaddr_inc(struct dma350_ch_dev_t *dev,
                                 int16_t src_xaddr_inc)
{
    dev->cfg.ch_base->CH_XADDRINC =
        (dev->cfg.ch_base->CH_XADDRINC & (~DMA_CH_XADDRINC_SRCXADDRINC_Msk)) |
        ((uint32_t)src_xaddr_inc & 0x0000FFFFUL);
}

__STATIC_INLINE
void dma350_ch_set_des_xaddr_inc(struct dma350_ch_dev_t *dev,
                                 int16_t des_xaddr_inc)
{
    dev->cfg.ch_base->CH_XADDRINC =
        (dev->cfg.ch_base->CH_XADDRINC & (~DMA_CH_XADDRINC_DESXADDRINC_Msk)) |
        (((uint32_t)des_xaddr_inc & 0x0000FFFFUL) << DMA_CH_XADDRINC_DESXADDRINC_Pos);
}

__STATIC_INLINE
void dma350_ch_set_xaddr_inc(struct dma350_ch_dev_t *dev, int16_t src_xaddr_inc,
                             int16_t des_xaddr_inc)
{
    dev->cfg.ch_base->CH_XADDRINC =
        (((uint32_t)des_xaddr_inc & 0x0000FFFFUL) << DMA_CH_XADDRINC_DESXADDRINC_Pos) |
        ((uint32_t)src_xaddr_inc & 0x0000FFFFUL);
}

__STATIC_INLINE
void dma350_ch_set_fill_value(struct dma350_ch_dev_t *dev, uint32_t fill_value)
{
    dev->cfg.ch_base->CH_FILLVAL = fill_value;
}

__STATIC_INLINE
void dma350_ch_set_srctriginsel(struct dma350_ch_dev_t *dev,
                                uint8_t srctriginsel)
{
    SET_FIELD(dev->cfg.ch_base->CH_SRCTRIGINCFG, srctriginsel,
              DMA_CH_SRCTRIGINCFG_SRCTRIGINSEL_Pos,
              DMA_CH_SRCTRIGINCFG_SRCTRIGINSEL_Msk);
}

__STATIC_INLINE
void dma350_ch_set_srctrigintype(struct dma350_ch_dev_t *dev,
                                 enum dma350_ch_srctrigintype_t type)
{
    dev->cfg.ch_base->CH_SRCTRIGINCFG =
        (dev->cfg.ch_base->CH_SRCTRIGINCFG &
         (~DMA_CH_SRCTRIGINCFG_SRCTRIGINTYPE_Msk)) |
        (type & DMA_CH_SRCTRIGINCFG_SRCTRIGINTYPE_Msk);
}

__STATIC_INLINE
void dma350_ch_set_srctriginmode(struct dma350_ch_dev_t *dev,
                                 enum dma350_ch_srctriginmode_t mode)
{
    dev->cfg.ch_base->CH_SRCTRIGINCFG =
        (dev->cfg.ch_base->CH_SRCTRIGINCFG &
         (~DMA_CH_SRCTRIGINCFG_SRCTRIGINMODE_Msk)) |
        (mode & DMA_CH_SRCTRIGINCFG_SRCTRIGINMODE_Msk);
}

__STATIC_INLINE
void dma350_ch_set_srctriginblksize(struct dma350_ch_dev_t *dev,
                                    uint8_t blksize)
{
    SET_FIELD(dev->cfg.ch_base->CH_SRCTRIGINCFG, blksize,
              DMA_CH_SRCTRIGINCFG_SRCTRIGINBLKSIZE_Pos,
              DMA_CH_SRCTRIGINCFG_SRCTRIGINBLKSIZE_Msk);
}

__STATIC_INLINE
void dma350_ch_set_destriginsel(struct dma350_ch_dev_t *dev,
                                uint8_t destriginsel)
{
    SET_FIELD(dev->cfg.ch_base->CH_DESTRIGINCFG, destriginsel,
              DMA_CH_DESTRIGINCFG_DESTRIGINSEL_Pos,
              DMA_CH_DESTRIGINCFG_DESTRIGINSEL_Msk);
}

__STATIC_INLINE
void dma350_ch_set_destrigintype(struct dma350_ch_dev_t *dev,
                                 enum dma350_ch_destrigintype_t type)
{
    dev->cfg.ch_base->CH_DESTRIGINCFG =
        (dev->cfg.ch_base->CH_DESTRIGINCFG &
         (~DMA_CH_DESTRIGINCFG_DESTRIGINTYPE_Msk)) |
        (type & DMA_CH_DESTRIGINCFG_DESTRIGINTYPE_Msk);
}

__STATIC_INLINE
void dma350_ch_set_destriginmode(struct dma350_ch_dev_t *dev,
                                 enum dma350_ch_destriginmode_t mode)
{
    dev->cfg.ch_base->CH_DESTRIGINCFG =
        (dev->cfg.ch_base->CH_DESTRIGINCFG &
         (~DMA_CH_DESTRIGINCFG_DESTRIGINMODE_Msk)) |
        (mode & DMA_CH_DESTRIGINCFG_DESTRIGINMODE_Msk);
}

__STATIC_INLINE
void dma350_ch_set_destriginblksize(struct dma350_ch_dev_t *dev,
                                    uint8_t blksize)
{
    SET_FIELD(dev->cfg.ch_base->CH_DESTRIGINCFG, blksize,
              DMA_CH_DESTRIGINCFG_DESTRIGINBLKSIZE_Pos,
              DMA_CH_DESTRIGINCFG_DESTRIGINBLKSIZE_Msk);
}

__STATIC_INLINE
void dma350_ch_set_trigoutsel(struct dma350_ch_dev_t *dev, uint8_t trigoutsel)
{
    SET_FIELD(dev->cfg.ch_base->CH_TRIGOUTCFG, trigoutsel,
              DMA_CH_TRIGOUTCFG_TRIGOUTSEL_Pos,
              DMA_CH_TRIGOUTCFG_TRIGOUTSEL_Msk);
}

__STATIC_INLINE
void dma350_ch_set_trigouttype(struct dma350_ch_dev_t *dev,
                               enum dma350_ch_trigouttype_t type)
{
    dev->cfg.ch_base->CH_TRIGOUTCFG =
        (dev->cfg.ch_base->CH_TRIGOUTCFG &
         (~DMA_CH_TRIGOUTCFG_TRIGOUTTYPE_Msk)) |
        (type & DMA_CH_TRIGOUTCFG_TRIGOUTTYPE_Msk);
}

__STATIC_INLINE
void dma350_ch_set_gpoen0(struct dma350_ch_dev_t *dev, uint32_t gpoen0)
{
    dev->cfg.ch_base->CH_GPOEN0 = gpoen0;
}

__STATIC_INLINE
void dma350_ch_set_gpoval0(struct dma350_ch_dev_t *dev, uint32_t gpoval0)
{
    dev->cfg.ch_base->CH_GPOVAL0 = gpoval0;
}

__STATIC_INLINE
uint32_t dma350_ch_get_gpoval0(struct dma350_ch_dev_t *dev)
{
    return dev->cfg.ch_base->CH_GPOREAD0;
}

__STATIC_INLINE
void dma350_ch_set_streamtype(struct dma350_ch_dev_t *dev,
                              enum dma350_ch_streamtype_t type)
{
    dev->cfg.ch_base->CH_STREAMINTCFG =
        (dev->cfg.ch_base->CH_STREAMINTCFG &
         (~DMA_CH_STREAMINTCFG_STREAMTYPE_Msk)) |
        (type & DMA_CH_STREAMINTCFG_STREAMTYPE_Msk);
}

__STATIC_INLINE
void dma350_ch_set_linkmemattrlo(struct dma350_ch_dev_t *dev, uint8_t memattrlo)
{
    SET_FIELD(dev->cfg.ch_base->CH_LINKATTR, memattrlo,
              DMA_CH_LINKATTR_LINKMEMATTRLO_Pos,
              DMA_CH_LINKATTR_LINKMEMATTRLO_Msk);
}

__STATIC_INLINE
void dma350_ch_set_linkmemattrhi(struct dma350_ch_dev_t *dev, uint8_t memattrhi)
{
    SET_FIELD(dev->cfg.ch_base->CH_LINKATTR, memattrhi,
              DMA_CH_LINKATTR_LINKMEMATTRHI_Pos,
              DMA_CH_LINKATTR_LINKMEMATTRHI_Msk);
}

__STATIC_INLINE
void dma350_ch_set_linkshareattr(struct dma350_ch_dev_t *dev, uint8_t shareattr)
{
    SET_FIELD(dev->cfg.ch_base->CH_LINKATTR, shareattr,
              DMA_CH_LINKATTR_LINKSHAREATTR_Pos,
              DMA_CH_LINKATTR_LINKSHAREATTR_Msk);
}

__STATIC_INLINE
void dma350_ch_enable_linkaddr(struct dma350_ch_dev_t *dev)
{
    dev->cfg.ch_base->CH_LINKADDR |= DMA_CH_LINKADDR_LINKADDREN_Msk;
}

__STATIC_INLINE
void dma350_ch_disable_linkaddr(struct dma350_ch_dev_t *dev)
{
    dev->cfg.ch_base->CH_LINKADDR &= ~(DMA_CH_LINKADDR_LINKADDREN_Msk);
}

__STATIC_INLINE
void dma350_ch_set_linkaddr32(struct dma350_ch_dev_t *dev, uint32_t linkaddr)
{
    dev->cfg.ch_base->CH_LINKADDR =
        (dev->cfg.ch_base->CH_LINKADDR & (~DMA_CH_LINKADDR_LINKADDR_Msk)) |
        ((uint32_t)linkaddr & DMA_CH_LINKADDR_LINKADDR_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_regclear(struct dma350_cmdlink_gencfg_t *cmdlink_cfg)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_REGCLEAR_SET;
}

__STATIC_INLINE
void dma350_cmdlink_enable_intr(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                enum dma350_ch_intr_t intr)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_INTREN_SET;
    cmdlink_cfg->cfg.intren |= intr;
}

__STATIC_INLINE
void dma350_cmdlink_disable_intr(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                 enum dma350_ch_intr_t intr)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_INTREN_SET;
    cmdlink_cfg->cfg.intren &= (~intr);
}

__STATIC_INLINE
void dma350_cmdlink_set_transize(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                 enum dma350_ch_transize_t transize)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_CTRL_SET;
    cmdlink_cfg->cfg.ctrl =
        (cmdlink_cfg->cfg.ctrl & (~DMA_CH_CTRL_TRANSIZE_Msk)) |
        (transize & DMA_CH_CTRL_TRANSIZE_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_chprio(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                               uint8_t chprio)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_CTRL_SET;
    cmdlink_cfg->cfg.ctrl =
        (cmdlink_cfg->cfg.ctrl & (~DMA_CH_CTRL_CHPRIO_Msk)) |
        (((chprio & 0x000000FFUL) << DMA_CH_CTRL_CHPRIO_Pos) &
         DMA_CH_CTRL_CHPRIO_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_xtype(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                              enum dma350_ch_xtype_t xtype)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_CTRL_SET;
    cmdlink_cfg->cfg.ctrl = (cmdlink_cfg->cfg.ctrl & (~DMA_CH_CTRL_XTYPE_Msk)) |
                            (xtype & DMA_CH_CTRL_XTYPE_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_ytype(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                              enum dma350_ch_ytype_t ytype)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_CTRL_SET;
    cmdlink_cfg->cfg.ctrl = (cmdlink_cfg->cfg.ctrl & (~DMA_CH_CTRL_YTYPE_Msk)) |
                            (ytype & DMA_CH_CTRL_YTYPE_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_regreloadtype(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
    enum dma350_ch_regreloadtype_t regreloadtype)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_CTRL_SET;
    cmdlink_cfg->cfg.ctrl =
        (cmdlink_cfg->cfg.ctrl & (~DMA_CH_CTRL_REGRELOADTYPE_Msk)) |
        (regreloadtype & DMA_CH_CTRL_REGRELOADTYPE_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_donetype(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                 enum dma350_ch_donetype_t donetype)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_CTRL_SET;
    cmdlink_cfg->cfg.ctrl =
        (cmdlink_cfg->cfg.ctrl & (~DMA_CH_CTRL_DONETYPE_Msk)) |
        (donetype & DMA_CH_CTRL_DONETYPE_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_enable_donepause(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_CTRL_SET;
    cmdlink_cfg->cfg.ctrl |= DMA_CH_CTRL_DONEPAUSEEN_Msk;
}

__STATIC_INLINE
void dma350_cmdlink_disable_donepause(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_CTRL_SET;
    cmdlink_cfg->cfg.ctrl &= (~DMA_CH_CTRL_DONEPAUSEEN_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_enable_srctrigin(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_CTRL_SET;
    cmdlink_cfg->cfg.ctrl |= DMA_CH_CTRL_USESRCTRIGIN_Msk;
}

__STATIC_INLINE
void dma350_cmdlink_disable_srctrigin(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_CTRL_SET;
    cmdlink_cfg->cfg.ctrl &= (~DMA_CH_CTRL_USESRCTRIGIN_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_enable_destrigin(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_CTRL_SET;
    cmdlink_cfg->cfg.ctrl |= DMA_CH_CTRL_USEDESTRIGIN_Msk;
}

__STATIC_INLINE
void dma350_cmdlink_disable_destrigin(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_CTRL_SET;
    cmdlink_cfg->cfg.ctrl &= (~DMA_CH_CTRL_USEDESTRIGIN_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_enable_trigout(struct dma350_cmdlink_gencfg_t *cmdlink_cfg)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_CTRL_SET;
    cmdlink_cfg->cfg.ctrl |= DMA_CH_CTRL_USETRIGOUT_Msk;
}

__STATIC_INLINE
void dma350_cmdlink_disable_trigout(struct dma350_cmdlink_gencfg_t *cmdlink_cfg)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_CTRL_SET;
    cmdlink_cfg->cfg.ctrl &= (~DMA_CH_CTRL_USETRIGOUT_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_enable_gpo(struct dma350_cmdlink_gencfg_t *cmdlink_cfg)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_CTRL_SET;
    cmdlink_cfg->cfg.ctrl |= DMA_CH_CTRL_USEGPO_Msk;
}

__STATIC_INLINE
void dma350_cmdlink_disable_gpo(struct dma350_cmdlink_gencfg_t *cmdlink_cfg)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_CTRL_SET;
    cmdlink_cfg->cfg.ctrl &= (~DMA_CH_CTRL_USEGPO_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_enable_stream(struct dma350_cmdlink_gencfg_t *cmdlink_cfg)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_CTRL_SET;
    cmdlink_cfg->cfg.ctrl |= DMA_CH_CTRL_USESTREAM_Msk;
}

__STATIC_INLINE
void dma350_cmdlink_disable_stream(struct dma350_cmdlink_gencfg_t *cmdlink_cfg)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_CTRL_SET;
    cmdlink_cfg->cfg.ctrl &= (~DMA_CH_CTRL_USESTREAM_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_srcaddr32(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                  uint32_t src_addr)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_SRC_ADDR_SET;
    cmdlink_cfg->header &= (~DMA350_CMDLINK_SRC_ADDRHI_SET);
    cmdlink_cfg->cfg.srcaddr = src_addr;
}

__STATIC_INLINE
void dma350_cmdlink_set_desaddr32(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                  uint32_t des_addr)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_DES_ADDR_SET;
    cmdlink_cfg->header &= (~DMA350_CMDLINK_DES_ADDRHI_SET);
    cmdlink_cfg->cfg.desaddr = des_addr;
}

__STATIC_INLINE
void dma350_cmdlink_set_xsize16(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                uint16_t src_xsize, uint16_t des_xsize)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_XSIZE_SET;
    cmdlink_cfg->header &= (~DMA350_CMDLINK_XSIZEHI_SET);
    cmdlink_cfg->cfg.xsize = (des_xsize & 0x0000FFFFUL)
                             << DMA_CH_XSIZE_DESXSIZE_Pos;
    cmdlink_cfg->cfg.xsize |= (src_xsize & 0x0000FFFFUL)
                              << DMA_CH_XSIZE_SRCXSIZE_Pos;
}

__STATIC_INLINE
void dma350_cmdlink_set_xsize32(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                uint32_t src_xsize, uint32_t des_xsize)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_XSIZE_SET;
    cmdlink_cfg->header |= DMA350_CMDLINK_XSIZEHI_SET;
    cmdlink_cfg->cfg.xsize = (des_xsize & 0x0000FFFFUL)
                             << DMA_CH_XSIZE_DESXSIZE_Pos;
    cmdlink_cfg->cfg.xsize |= (src_xsize & 0x0000FFFFUL)
                              << DMA_CH_XSIZE_SRCXSIZE_Pos;
    cmdlink_cfg->cfg.xsizehi = (des_xsize & 0xFFFF0000UL);
    cmdlink_cfg->cfg.xsizehi |= (src_xsize & 0xFFFF0000UL) >> 16;
}

__STATIC_INLINE
void dma350_cmdlink_set_srcmemattrlo(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t memattrlo)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_SRCTRANSCFG_SET;
    cmdlink_cfg->cfg.srctranscfg =
        (cmdlink_cfg->cfg.srctranscfg &
         (~DMA_CH_SRCTRANSCFG_SRCMEMATTRLO_Msk)) |
        (((memattrlo & 0x000000FFUL) << DMA_CH_SRCTRANSCFG_SRCMEMATTRLO_Pos) &
         DMA_CH_SRCTRANSCFG_SRCMEMATTRLO_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_srcmemattrhi(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t memattrhi)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_SRCTRANSCFG_SET;
    cmdlink_cfg->cfg.srctranscfg =
        (cmdlink_cfg->cfg.srctranscfg &
         (~DMA_CH_SRCTRANSCFG_SRCMEMATTRHI_Msk)) |
        (((memattrhi & 0x000000FFUL) << DMA_CH_SRCTRANSCFG_SRCMEMATTRHI_Pos) &
         DMA_CH_SRCTRANSCFG_SRCMEMATTRHI_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_srcshareattr(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t shareattr)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_SRCTRANSCFG_SET;
    cmdlink_cfg->cfg.srctranscfg =
        (cmdlink_cfg->cfg.srctranscfg &
         (~DMA_CH_SRCTRANSCFG_SRCSHAREATTR_Msk)) |
        (((shareattr & 0x000000FFUL) << DMA_CH_SRCTRANSCFG_SRCSHAREATTR_Pos) &
         DMA_CH_SRCTRANSCFG_SRCSHAREATTR_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_desmemattrlo(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t memattrlo)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_DESTRANSCFG_SET;
    cmdlink_cfg->cfg.destranscfg =
        (cmdlink_cfg->cfg.destranscfg &
         (~DMA_CH_DESTRANSCFG_DESSHAREATTR_Msk)) |
        (((memattrlo & 0x000000FFUL) << DMA_CH_DESTRANSCFG_DESMEMATTRLO_Pos) &
         DMA_CH_DESTRANSCFG_DESMEMATTRLO_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_desmemattrhi(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t memattrhi)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_DESTRANSCFG_SET;
    cmdlink_cfg->cfg.destranscfg =
        (cmdlink_cfg->cfg.destranscfg &
         (~DMA_CH_DESTRANSCFG_DESMEMATTRHI_Msk)) |
        (((memattrhi & 0x000000FFUL) << DMA_CH_DESTRANSCFG_DESMEMATTRHI_Pos) &
         DMA_CH_DESTRANSCFG_DESMEMATTRHI_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_desshareattr(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t shareattr)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_DESTRANSCFG_SET;
    cmdlink_cfg->cfg.destranscfg =
        (cmdlink_cfg->cfg.destranscfg &
         (~DMA_CH_DESTRANSCFG_DESSHAREATTR_Msk)) |
        (((shareattr & 0x000000FFUL) << DMA_CH_DESTRANSCFG_DESSHAREATTR_Pos) &
         DMA_CH_DESTRANSCFG_DESSHAREATTR_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_src_trans_secure(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_SRCTRANSCFG_SET;
    cmdlink_cfg->cfg.srctranscfg &= (~DMA_CH_SRCTRANSCFG_SRCNONSECATTR_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_src_trans_nonsecure(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg)
{
    cmdlink_cfg->cfg.srctranscfg |= DMA_CH_SRCTRANSCFG_SRCNONSECATTR_Msk;
    if (cmdlink_cfg->cfg.srctranscfg == DMA350_CH_SRCTRANSCFG_RESET_VALUE) {
        cmdlink_cfg->header &= (~DMA350_CMDLINK_SRCTRANSCFG_SET);
    }
}

__STATIC_INLINE
void dma350_cmdlink_set_des_trans_secure(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_DESTRANSCFG_SET;
    cmdlink_cfg->cfg.destranscfg &= (~DMA_CH_DESTRANSCFG_DESNONSECATTR_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_des_trans_nonsecure(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg)
{
    cmdlink_cfg->cfg.destranscfg |= DMA_CH_DESTRANSCFG_DESNONSECATTR_Msk;
    if (cmdlink_cfg->cfg.destranscfg == DMA350_CH_DESTRANSCFG_RESET_VALUE) {
        cmdlink_cfg->header &= (~DMA350_CMDLINK_DESTRANSCFG_SET);
    }
}

__STATIC_INLINE
void dma350_cmdlink_set_src_trans_privileged(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_SRCTRANSCFG_SET;
    cmdlink_cfg->cfg.srctranscfg |= DMA_CH_SRCTRANSCFG_SRCPRIVATTR_Msk;
}

__STATIC_INLINE
void dma350_cmdlink_set_src_trans_unprivileged(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg)
{
    cmdlink_cfg->cfg.srctranscfg &= (~DMA_CH_SRCTRANSCFG_SRCPRIVATTR_Msk);
    if (cmdlink_cfg->cfg.srctranscfg == DMA350_CH_SRCTRANSCFG_RESET_VALUE) {
        cmdlink_cfg->header &= (~DMA350_CMDLINK_SRCTRANSCFG_SET);
    }
}

__STATIC_INLINE
void dma350_cmdlink_set_des_trans_privileged(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_DESTRANSCFG_SET;
    cmdlink_cfg->cfg.destranscfg |= DMA_CH_DESTRANSCFG_DESPRIVATTR_Msk;
}

__STATIC_INLINE
void dma350_cmdlink_set_des_trans_unprivileged(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg)
{
    cmdlink_cfg->cfg.destranscfg &= (~DMA_CH_DESTRANSCFG_DESPRIVATTR_Msk);
    if (cmdlink_cfg->cfg.destranscfg == DMA350_CH_DESTRANSCFG_RESET_VALUE) {
        cmdlink_cfg->header &= (~DMA350_CMDLINK_DESTRANSCFG_SET);
    }
}

__STATIC_INLINE
void dma350_cmdlink_set_srcmaxburstlen(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t length)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_SRCTRANSCFG_SET;
    cmdlink_cfg->cfg.srctranscfg =
        (cmdlink_cfg->cfg.srctranscfg &
         (~DMA_CH_SRCTRANSCFG_SRCMAXBURSTLEN_Msk)) |
        (((length & 0x000000FFUL) << DMA_CH_SRCTRANSCFG_SRCMAXBURSTLEN_Pos) &
         DMA_CH_SRCTRANSCFG_SRCMAXBURSTLEN_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_desmaxburstlen(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t length)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_DESTRANSCFG_SET;
    cmdlink_cfg->cfg.destranscfg =
        (cmdlink_cfg->cfg.destranscfg &
         (~DMA_CH_DESTRANSCFG_DESMAXBURSTLEN_Msk)) |
        (((length & 0x000000FFUL) << DMA_CH_DESTRANSCFG_DESMAXBURSTLEN_Pos) &
         DMA_CH_DESTRANSCFG_DESMAXBURSTLEN_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_xaddrinc(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                 uint16_t src_xaddrinc, uint16_t des_xaddrinc)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_XADDRINC_SET;
    cmdlink_cfg->cfg.xaddrinc = (des_xaddrinc & 0x0000FFFFUL)
                                << DMA_CH_XADDRINC_DESXADDRINC_Pos;
    cmdlink_cfg->cfg.xaddrinc |= (src_xaddrinc & 0x0000FFFFUL)
                                 << DMA_CH_XADDRINC_SRCXADDRINC_Pos;
}

__STATIC_INLINE
void dma350_cmdlink_set_yaddrstride(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                    uint16_t src_yaddrstride,
                                    uint16_t des_yaddrstride)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_YADDRSTRIDE_SET;
    cmdlink_cfg->cfg.yaddrstride = (des_yaddrstride & 0x0000FFFFUL)
                                   << DMA_CH_YADDRSTRIDE_DESYADDRSTRIDE_Pos;
    cmdlink_cfg->cfg.yaddrstride |= (src_yaddrstride & 0x0000FFFFUL)
                                    << DMA_CH_YADDRSTRIDE_SRCYADDRSTRIDE_Pos;
}

__STATIC_INLINE
void dma350_cmdlink_set_fillval(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                uint32_t fillval)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_FILLVAL_SET;
    cmdlink_cfg->cfg.fillval = fillval << DMA_CH_FILLVAL_FILLVAL_Pos;
}

__STATIC_INLINE
void dma350_cmdlink_set_ysize16(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                uint16_t src_ysize, uint16_t des_ysize)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_YSIZE_SET;
    cmdlink_cfg->cfg.ysize = (des_ysize & 0x0000FFFFUL)
                             << DMA_CH_YSIZE_DESYSIZE_Pos;
    cmdlink_cfg->cfg.ysize |= (src_ysize & 0x0000FFFFUL)
                              << DMA_CH_YSIZE_SRCYSIZE_Pos;
}

__STATIC_INLINE
void dma350_cmdlink_set_tmpltsize(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                  uint8_t src_tmpltsize, uint8_t des_tmpltsize)
{
    if (src_tmpltsize <= DMA350_CH_TMPLTCFG_SRCTMPLTSIZE_MAX &&
        des_tmpltsize <= DMA350_CH_TMPLTCFG_DESTMPLTSIZE_MAX) {
        cmdlink_cfg->header |= DMA350_CMDLINK_TMPLTCFG_SET;
        cmdlink_cfg->cfg.tmpltcfg = (des_tmpltsize & 0x000000FFUL)
                                    << DMA_CH_TMPLTCFG_DESTMPLTSIZE_Pos;
        cmdlink_cfg->cfg.tmpltcfg |= (src_tmpltsize & 0x000000FFUL)
                                     << DMA_CH_TMPLTCFG_SRCTMPLTSIZE_Pos;
    }
}

__STATIC_INLINE
void dma350_cmdlink_set_srctmplt(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                 uint32_t src_tmplt)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_SRCTMPLT_SET;
    cmdlink_cfg->cfg.srctmplt = src_tmplt;
}

__STATIC_INLINE
void dma350_cmdlink_set_destmplt(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                 uint32_t des_tmplt)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_DESTMPLT_SET;
    cmdlink_cfg->cfg.destmplt = des_tmplt;
}

__STATIC_INLINE
void dma350_cmdlink_set_srctriginsel(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t srctriginsel)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_SRCTRIGINCFG_SET;
    cmdlink_cfg->cfg.srctrigincfg = (cmdlink_cfg->cfg.srctrigincfg &
                                     (~DMA_CH_SRCTRIGINCFG_SRCTRIGINSEL_Msk)) |
                                    (((srctriginsel & 0x000000FFUL)
                                      << DMA_CH_SRCTRIGINCFG_SRCTRIGINSEL_Pos) &
                                     DMA_CH_SRCTRIGINCFG_SRCTRIGINSEL_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_srctrigintype(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
    enum dma350_ch_srctrigintype_t type)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_SRCTRIGINCFG_SET;
    cmdlink_cfg->cfg.srctrigincfg =
        (cmdlink_cfg->cfg.srctrigincfg &
         (~DMA_CH_SRCTRIGINCFG_SRCTRIGINTYPE_Msk)) |
        (type & DMA_CH_SRCTRIGINCFG_SRCTRIGINTYPE_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_srctriginmode(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
    enum dma350_ch_srctriginmode_t mode)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_SRCTRIGINCFG_SET;
    cmdlink_cfg->cfg.srctrigincfg =
        (cmdlink_cfg->cfg.srctrigincfg &
         (~DMA_CH_SRCTRIGINCFG_SRCTRIGINMODE_Msk)) |
        (mode & DMA_CH_SRCTRIGINCFG_SRCTRIGINMODE_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_srctriginblksize(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t blksize)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_SRCTRIGINCFG_SET;
    cmdlink_cfg->cfg.srctrigincfg =
        (cmdlink_cfg->cfg.srctrigincfg &
         (~DMA_CH_SRCTRIGINCFG_SRCTRIGINBLKSIZE_Msk)) |
        (((blksize & 0x000000FFUL)
          << DMA_CH_SRCTRIGINCFG_SRCTRIGINBLKSIZE_Pos) &
         DMA_CH_SRCTRIGINCFG_SRCTRIGINBLKSIZE_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_destriginsel(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t destriginsel)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_DESTRIGINCFG_SET;
    cmdlink_cfg->cfg.destrigincfg = (cmdlink_cfg->cfg.destrigincfg &
                                     (~DMA_CH_DESTRIGINCFG_DESTRIGINSEL_Msk)) |
                                    (((destriginsel & 0x000000FFUL)
                                      << DMA_CH_DESTRIGINCFG_DESTRIGINSEL_Pos) &
                                     DMA_CH_DESTRIGINCFG_DESTRIGINSEL_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_destrigintype(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
    enum dma350_ch_destrigintype_t type)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_DESTRIGINCFG_SET;
    cmdlink_cfg->cfg.destrigincfg =
        (cmdlink_cfg->cfg.destrigincfg &
         (~DMA_CH_DESTRIGINCFG_DESTRIGINTYPE_Msk)) |
        (type & DMA_CH_DESTRIGINCFG_DESTRIGINTYPE_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_destriginmode(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
    enum dma350_ch_destriginmode_t mode)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_DESTRIGINCFG_SET;
    cmdlink_cfg->cfg.destrigincfg =
        (cmdlink_cfg->cfg.destrigincfg &
         (~DMA_CH_DESTRIGINCFG_DESTRIGINMODE_Msk)) |
        (mode & DMA_CH_DESTRIGINCFG_DESTRIGINMODE_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_destriginblksize(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t blksize)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_DESTRIGINCFG_SET;
    cmdlink_cfg->cfg.destrigincfg =
        (cmdlink_cfg->cfg.destrigincfg &
         (~DMA_CH_DESTRIGINCFG_DESTRIGINBLKSIZE_Msk)) |
        (((blksize & 0x000000FFUL)
          << DMA_CH_DESTRIGINCFG_DESTRIGINBLKSIZE_Pos) &
         DMA_CH_DESTRIGINCFG_DESTRIGINBLKSIZE_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_trigoutsel(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                   uint8_t trigoutsel)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_TRIGOUTCFG_SET;
    cmdlink_cfg->cfg.trigoutcfg =
        (cmdlink_cfg->cfg.trigoutcfg & (~DMA_CH_TRIGOUTCFG_TRIGOUTSEL_Msk)) |
        (((trigoutsel & 0x000000FFUL) << DMA_CH_TRIGOUTCFG_TRIGOUTSEL_Pos) &
         DMA_CH_TRIGOUTCFG_TRIGOUTSEL_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_trigouttype(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                    enum dma350_ch_trigouttype_t type)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_TRIGOUTCFG_SET;
    cmdlink_cfg->cfg.trigoutcfg =
        (cmdlink_cfg->cfg.trigoutcfg & (~DMA_CH_TRIGOUTCFG_TRIGOUTTYPE_Msk)) |
        (type & DMA_CH_TRIGOUTCFG_TRIGOUTTYPE_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_gpoen0(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                               uint32_t gpoen0)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_GPOEN0_SET;
    cmdlink_cfg->cfg.gpoen0 = gpoen0;
}

__STATIC_INLINE
void dma350_cmdlink_set_gpoval0(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                uint32_t gpoval0)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_GPOVAL0_SET;
    cmdlink_cfg->cfg.gpoval0 = gpoval0;
}

__STATIC_INLINE
void dma350_cmdlink_set_streamtype(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                   enum dma350_ch_streamtype_t type)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_STREAMINTCFG_SET;
    cmdlink_cfg->cfg.streamintcfg = (cmdlink_cfg->cfg.streamintcfg &
                                     (~DMA_CH_STREAMINTCFG_STREAMTYPE_Msk)) |
                                    (type & DMA_CH_STREAMINTCFG_STREAMTYPE_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_linkmemattrlo(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t memattrlo)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_LINKATTR_SET;
    cmdlink_cfg->cfg.linkattr =
        (cmdlink_cfg->cfg.linkattr & (~DMA_CH_LINKATTR_LINKMEMATTRLO_Msk)) |
        (((memattrlo & 0x000000FFUL) << DMA_CH_LINKATTR_LINKMEMATTRLO_Pos) &
         DMA_CH_LINKATTR_LINKMEMATTRLO_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_linkmemattrhi(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t memattrhi)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_LINKATTR_SET;
    cmdlink_cfg->cfg.linkattr =
        (cmdlink_cfg->cfg.linkattr & (~DMA_CH_LINKATTR_LINKMEMATTRHI_Msk)) |
        (((memattrhi & 0x000000FFUL) << DMA_CH_LINKATTR_LINKMEMATTRHI_Pos) &
         DMA_CH_LINKATTR_LINKMEMATTRHI_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_linkshareattr(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint8_t shareattr)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_LINKATTR_SET;
    cmdlink_cfg->cfg.linkattr =
        (cmdlink_cfg->cfg.linkattr & (~DMA_CH_LINKATTR_LINKSHAREATTR_Msk)) |
        (((shareattr & 0x000000FFUL) << DMA_CH_LINKATTR_LINKSHAREATTR_Pos) &
         DMA_CH_LINKATTR_LINKSHAREATTR_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_cmdrestartcnt(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg, uint16_t cnt)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_AUTOCFG_SET;
    cmdlink_cfg->cfg.autocfg = (cnt & 0x0000FFFFUL)
                               << DMA_CH_AUTOCFG_CMDRESTARTCNT_Pos;
}

__STATIC_INLINE
void dma350_cmdlink_enable_cmdrestartinfen(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_AUTOCFG_SET;
    cmdlink_cfg->cfg.autocfg |= DMA_CH_AUTOCFG_CMDRESTARTINFEN_Msk;
}

__STATIC_INLINE
void dma350_cmdlink_disable_cmdrestartinfen(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_AUTOCFG_SET;
    cmdlink_cfg->cfg.autocfg &= (~DMA_CH_AUTOCFG_CMDRESTARTINFEN_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_enable_linkaddr(struct dma350_cmdlink_gencfg_t *cmdlink_cfg)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_LINKADDR_SET;
    cmdlink_cfg->cfg.linkaddr |= DMA_CH_LINKADDR_LINKADDREN_Msk;
}

__STATIC_INLINE
void dma350_cmdlink_disable_linkaddr(
    struct dma350_cmdlink_gencfg_t *cmdlink_cfg)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_LINKADDR_SET;
    cmdlink_cfg->cfg.linkaddr &= ~(DMA_CH_LINKADDR_LINKADDREN_Msk);
}

__STATIC_INLINE
void dma350_cmdlink_set_linkaddr32(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                   uint32_t linkaddr)
{
    cmdlink_cfg->header |= DMA350_CMDLINK_LINKADDR_SET;
    cmdlink_cfg->header &= ~(DMA350_CMDLINK_LINKADDRHI_SET);
    cmdlink_cfg->cfg.linkaddr =
        (cmdlink_cfg->cfg.linkaddr & (~DMA_CH_LINKADDR_LINKADDR_Msk)) |
        (linkaddr & DMA_CH_LINKADDR_LINKADDR_Msk);
}

__STATIC_INLINE
void dma350_ch_cmd(struct dma350_ch_dev_t *dev, enum dma350_ch_cmd_t cmd)
{
    /* Wait outstanding CPU writes to avoid race condition with DMA */
    __DMB();
    dev->cfg.ch_base->CH_CMD = cmd;
}

__STATIC_INLINE
enum dma350_ch_cmd_t dma350_ch_get_cmd(struct dma350_ch_dev_t *dev)
{
    return (enum dma350_ch_cmd_t)dev->cfg.ch_base->CH_CMD;
}

__STATIC_INLINE
bool dma350_ch_is_busy(struct dma350_ch_dev_t *dev)
{
    return (dev->cfg.ch_base->CH_CMD & DMA350_CH_CMD_ENABLECMD) != 0;
}

__STATIC_INLINE
bool dma350_ch_is_ready(struct dma350_ch_dev_t *dev)
{
    return (dev->cfg.ch_base->CH_CMD & DMA350_CH_CMD_ENABLECMD) == 0;
}

__STATIC_INLINE
union dma350_ch_status_t dma350_ch_get_status(struct dma350_ch_dev_t *dev)
{
    return (union dma350_ch_status_t) {
        .w = (dev->cfg.ch_base->CH_STATUS)
    };
}

__STATIC_INLINE
void dma350_ch_enable_intr(struct dma350_ch_dev_t *dev,
                           enum dma350_ch_intr_t intr)
{
    dev->cfg.ch_base->CH_INTREN = dev->cfg.ch_base->CH_INTREN | intr;
}

__STATIC_INLINE
void dma350_ch_disable_intr(struct dma350_ch_dev_t *dev,
                            enum dma350_ch_intr_t intr)
{
    dev->cfg.ch_base->CH_INTREN = dev->cfg.ch_base->CH_INTREN & (~intr);
}

#if DMA_CH_STATUS_INTR_DONE != DMA_CH_INTREN_INTREN_DONE ||                    \
    DMA_CH_STATUS_INTR_ERR != DMA_CH_INTREN_INTREN_ERR ||                      \
    DMA_CH_STATUS_INTR_DISABLED != DMA_CH_INTREN_INTREN_DISABLED ||            \
    DMA_CH_STATUS_INTR_STOPPED != DMA_CH_INTREN_INTREN_STOPPED ||              \
    DMA_CH_STATUS_INTR_SRCTRIGINWAIT != DMA_CH_INTREN_INTREN_SRCTRIGINWAIT ||  \
    DMA_CH_STATUS_INTR_DESTRIGINWAIT != DMA_CH_INTREN_INTREN_DESTRIGINWAIT ||  \
    DMA_CH_STATUS_INTR_TRIGOUTACKWAIT != DMA_CH_INTREN_INTREN_TRIGOUTACKWAIT
#error "STATUS_INTR INTREN_INTREN not aligned anymore!"
#endif
__STATIC_INLINE
bool dma350_ch_is_intr_set(struct dma350_ch_dev_t *dev,
                           enum dma350_ch_intr_t intr)
{
    return (dev->cfg.ch_base->CH_STATUS & intr) != 0;
}

__STATIC_INLINE
bool dma350_ch_is_stat_set(struct dma350_ch_dev_t *dev,
                           enum dma350_ch_stat_t stat)
{
    return (dev->cfg.ch_base->CH_STATUS & stat) != 0;
}

__STATIC_INLINE
void dma350_ch_clear_stat(struct dma350_ch_dev_t *dev,
                          enum dma350_ch_stat_t stat)
{
    dev->cfg.ch_base->CH_STATUS = stat;
}

__STATIC_INLINE
void dma350_ch_set_autocfg_restart_cnt(struct dma350_ch_dev_t *dev,
                                       uint16_t cnt)
{
    dev->cfg.ch_base->CH_AUTOCFG = (cnt & DMA_CH_AUTOCFG_CMDRESTARTCNT_Msk);
}

__STATIC_INLINE
void dma350_ch_set_autocfg_restart_inf(struct dma350_ch_dev_t *dev)
{
    dev->cfg.ch_base->CH_AUTOCFG = DMA_CH_AUTOCFG_CMDRESTARTINFEN_Msk;
}

__STATIC_INLINE
void dma350_ch_set_tmplt_src_size(struct dma350_ch_dev_t *dev, uint32_t size)
{
    SET_FIELD(dev->cfg.ch_base->CH_TMPLTCFG, size,
              DMA_CH_TMPLTCFG_SRCTMPLTSIZE_Pos,
              DMA_CH_TMPLTCFG_SRCTMPLTSIZE_Msk);
}

__STATIC_INLINE
void dma350_ch_set_tmplt_des_size(struct dma350_ch_dev_t *dev, uint32_t size)
{
    SET_FIELD(dev->cfg.ch_base->CH_TMPLTCFG, size,
              DMA_CH_TMPLTCFG_DESTMPLTSIZE_Pos,
              DMA_CH_TMPLTCFG_DESTMPLTSIZE_Msk);
}

__STATIC_INLINE
void dma350_ch_set_tmplt_src(struct dma350_ch_dev_t *dev, uint32_t tmplt)
{
    dev->cfg.ch_base->CH_SRCTMPLT = tmplt;
}

__STATIC_INLINE
void dma350_ch_set_tmplt_des(struct dma350_ch_dev_t *dev, uint32_t tmplt)
{
    dev->cfg.ch_base->CH_DESTMPLT = tmplt;
}

__STATIC_INLINE
uint32_t dma350_ch_get_tmplt_src(struct dma350_ch_dev_t *dev)
{
    return dev->cfg.ch_base->CH_SRCTMPLT;
}

__STATIC_INLINE
uint32_t dma350_ch_get_tmplt_des(struct dma350_ch_dev_t *dev)
{
    return dev->cfg.ch_base->CH_DESTMPLT;
}

#ifdef __cplusplus
}
#endif
#endif /* __DMA350_CH_DRV_H */
