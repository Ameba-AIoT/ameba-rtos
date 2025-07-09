/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SYSREG_MEM_CTRL_H_
#define _SYSREG_MEM_CTRL_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup MEM_CTRL
  * @brief MEM_CTRL driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup MEM_CTRL_Register_Definitions MEM_CTRL Register Definitions
  * @{
  */

/** @defgroup REG_CTRL_MBIST_CTRL_ALL
  * @brief MEMx MBIST control and status register
  * @{
  */
#define CTRL_MASK_MBIST_STA    ((u32)0x0000001F << 16)           /*!< R 5'b0  Total mbist status [16] MBIST DRF_START_PAUSE [17] MBIST NRML_DONE_ALL [18] MBIST NRML_FAIL_ALL [19] MBIST DRF_DONE_ALL [20] MBIST DRF_FAIL_ALL*/
#define CTRL_MBIST_STA(x)      (((u32)((x) & 0x0000001F) << 16))
#define CTRL_GET_MBIST_STA(x)  ((u32)(((x >> 16) & 0x0000001F)))
#define CTRL_MASK_MBIST_CTRL   ((u32)0x000000FF << 0)            /*!< R/W 8'b0  Total mbist control [0] MBIST_CLK_EN_ALL     BIS_CLK enable of all system memory groups. [1] MBIST_RSTB_ALL     Reset controll mbist circuit of all system memory groups. [3:2]MBIST_MSEL_ALL      mbist method selection of all memory groups:     2'b01: enable normal mode mbist     2'b10: enable DRF mode mbist      2'b11: enable normal&DRF mode mbist [4]MBIST_RESUME [5]MBIST_RESUME_PS [6]MBIST_DYN_READ_EN    If MBIST_MSEL_ALL[1]=1 & MBIST_DYN_READ_EN=1, run VDDR mode mbist. 7]MBIST_PHYCLK_STOP*/
#define CTRL_MBIST_CTRL(x)     (((u32)((x) & 0x000000FF) << 0))
#define CTRL_GET_MBIST_CTRL(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup REG_CTRL_MBIST_GRP_EN_CTRL0
  * @brief Used to on-off mbist of memory block in a group.
  * @{
  */
#define CTRL_MASK_GRP_EN_WLK4_E0     ((u32)0x00000003 << 21)           /*!< R/W 0x3  */
#define CTRL_GRP_EN_WLK4_E0(x)       (((u32)((x) & 0x00000003) << 21))
#define CTRL_GET_GRP_EN_WLK4_E0(x)   ((u32)(((x >> 21) & 0x00000003)))
#define CTRL_MASK_GRP_EN_SDIO_E0     ((u32)0x00000003 << 19)           /*!< R/W 0x3  */
#define CTRL_GRP_EN_SDIO_E0(x)       (((u32)((x) & 0x00000003) << 19))
#define CTRL_GET_GRP_EN_SDIO_E0(x)   ((u32)(((x >> 19) & 0x00000003)))
#define CTRL_MASK_GRP_EN_AONPMC_E0   ((u32)0x00000003 << 17)           /*!< R/W 0x3  */
#define CTRL_GRP_EN_AONPMC_E0(x)     (((u32)((x) & 0x00000003) << 17))
#define CTRL_GET_GRP_EN_AONPMC_E0(x) ((u32)(((x >> 17) & 0x00000003)))
#define CTRL_MASK_GRP_EN_HSYS_E1     ((u32)0x000001FF << 8)            /*!< R/W 0x1ff  */
#define CTRL_GRP_EN_HSYS_E1(x)       (((u32)((x) & 0x000001FF) << 8))
#define CTRL_GET_GRP_EN_HSYS_E1(x)   ((u32)(((x >> 8) & 0x000001FF)))
#define CTRL_MASK_GRP_EN_HSYS_E0     ((u32)0x000000FF << 0)            /*!< R/W 0xff  */
#define CTRL_GRP_EN_HSYS_E0(x)       (((u32)((x) & 0x000000FF) << 0))
#define CTRL_GET_GRP_EN_HSYS_E0(x)   ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup REG_CTRL_MBIST_GRP_EN_CTRL1
  * @brief Used to on-off mbist of memory block in a group.
  * @{
  */
#define CTRL_MASK_GRP_EN_MJPEG_E0         ((u32)0x0000007F << 24)           /*!< R/W 0x7f  */
#define CTRL_GRP_EN_MJPEG_E0(x)           (((u32)((x) & 0x0000007F) << 24))
#define CTRL_GET_GRP_EN_MJPEG_E0(x)       ((u32)(((x >> 24) & 0x0000007F)))
#define CTRL_MASK_GRP_EN_KM4NS_CACHE_E0   ((u32)0x0000000F << 20)           /*!< R/W 0xf  */
#define CTRL_GRP_EN_KM4NS_CACHE_E0(x)     (((u32)((x) & 0x0000000F) << 20))
#define CTRL_GET_GRP_EN_KM4NS_CACHE_E0(x) ((u32)(((x >> 20) & 0x0000000F)))
#define CTRL_MASK_GRP_EN_KM4TZ_ITCM0_E0   ((u32)0x0000000F << 16)           /*!< R/W 0xf  */
#define CTRL_GRP_EN_KM4TZ_ITCM0_E0(x)     (((u32)((x) & 0x0000000F) << 16))
#define CTRL_GET_GRP_EN_KM4TZ_ITCM0_E0(x) ((u32)(((x >> 16) & 0x0000000F)))
#define CTRL_MASK_GRP_EN_KM4TZ_CACHE_E0   ((u32)0x0000000F << 12)           /*!< R/W 0xf  */
#define CTRL_GRP_EN_KM4TZ_CACHE_E0(x)     (((u32)((x) & 0x0000000F) << 12))
#define CTRL_GET_GRP_EN_KM4TZ_CACHE_E0(x) ((u32)(((x >> 12) & 0x0000000F)))
#define CTRL_MASK_GRP_EN_BT_E0            ((u32)0x00000003 << 10)           /*!< R/W 0x3  */
#define CTRL_GRP_EN_BT_E0(x)              (((u32)((x) & 0x00000003) << 10))
#define CTRL_GET_GRP_EN_BT_E0(x)          ((u32)(((x >> 10) & 0x00000003)))
#define CTRL_MASK_GRP_EN_WPOFF_E1         ((u32)0x000003FF << 0)            /*!< R/W 0x3ff  */
#define CTRL_GRP_EN_WPOFF_E1(x)           (((u32)((x) & 0x000003FF) << 0))
#define CTRL_GET_GRP_EN_WPOFF_E1(x)       ((u32)(((x >> 0) & 0x000003FF)))
/** @} */

/** @defgroup REG_CTRL_MBIST_GRP_EN_CTRL2
  * @brief Used to on-off mbist of memory block in a group.
  * @{
  */
#define CTRL_MASK_GRP_EN_PPE_E1     ((u32)0x000000FF << 17)           /*!< R/W 0xff  */
#define CTRL_GRP_EN_PPE_E1(x)       (((u32)((x) & 0x000000FF) << 17))
#define CTRL_GET_GRP_EN_PPE_E1(x)   ((u32)(((x >> 17) & 0x000000FF)))
#define CTRL_MASK_GRP_EN_PPE_E0     ((u32)0x000000FF << 9)            /*!< R/W 0xff  */
#define CTRL_GRP_EN_PPE_E0(x)       (((u32)((x) & 0x000000FF) << 9))
#define CTRL_GET_GRP_EN_PPE_E0(x)   ((u32)(((x >> 9) & 0x000000FF)))
#define CTRL_MASK_GRP_EN_CAN_E0     ((u32)0x00000003 << 7)            /*!< R/W 0x3  */
#define CTRL_GRP_EN_CAN_E0(x)       (((u32)((x) & 0x00000003) << 7))
#define CTRL_GET_GRP_EN_CAN_E0(x)   ((u32)(((x >> 7) & 0x00000003)))
#define CTRL_MASK_GRP_EN_MJPEG_E2   ((u32)0x0000007F << 0)            /*!< R/W 0x7f  */
#define CTRL_GRP_EN_MJPEG_E2(x)     (((u32)((x) & 0x0000007F) << 0))
#define CTRL_GET_GRP_EN_MJPEG_E2(x) ((u32)(((x >> 0) & 0x0000007F)))
/** @} */

/** @defgroup REG_CTRL_MBIST_GRP_EN_BT_VIR_E00
  * @brief
  * @{
  */
#define CTRL_MASK_BT_VIR_E0_BIST_EN_19_0   ((u32)0x000FFFFF << 0)           /*!< R/W 0xfffff  */
#define CTRL_BT_VIR_E0_BIST_EN_19_0(x)     (((u32)((x) & 0x000FFFFF) << 0))
#define CTRL_GET_BT_VIR_E0_BIST_EN_19_0(x) ((u32)(((x >> 0) & 0x000FFFFF)))
/** @} */

/** @defgroup REG_CTRL_MBIST_DONE_STA0
  * @brief
  * @{
  */
#define CTRL_MASK_MBIST_DONE_31_0   ((u32)0xFFFFFFFF << 0)           /*!< R 32'b0  */
#define CTRL_MBIST_DONE_31_0(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define CTRL_GET_MBIST_DONE_31_0(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_CTRL_MBIST_START_PULSE0
  * @brief
  * @{
  */
#define CTRL_MASK_MBIST_START_PAUSE_31_0   ((u32)0xFFFFFFFF << 0)           /*!< R 32'b0  */
#define CTRL_MBIST_START_PAUSE_31_0(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define CTRL_GET_MBIST_START_PAUSE_31_0(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_CTRL_MBIST_NORM_FAIL0
  * @brief
  * @{
  */
#define CTRL_MASK_MBIST_NORM_FAIL_31_0   ((u32)0xFFFFFFFF << 0)           /*!< R 32'b0  */
#define CTRL_MBIST_NORM_FAIL_31_0(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define CTRL_GET_MBIST_NORM_FAIL_31_0(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_CTRL_MBIST_NORM_FAIL1
  * @brief
  * @{
  */
#define CTRL_MASK_MBIST_NORM_FAIL_63_32   ((u32)0xFFFFFFFF << 0)           /*!< R 32'b0  */
#define CTRL_MBIST_NORM_FAIL_63_32(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define CTRL_GET_MBIST_NORM_FAIL_63_32(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_CTRL_MBIST_NORM_FAIL2
  * @brief
  * @{
  */
#define CTRL_MASK_MBIST_NORM_FAIL_95_64   ((u32)0xFFFFFFFF << 0)           /*!< R 32'b0  */
#define CTRL_MBIST_NORM_FAIL_95_64(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define CTRL_GET_MBIST_NORM_FAIL_95_64(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_CTRL_MBIST_NORM_FAIL3
  * @brief
  * @{
  */
#define CTRL_MASK_MBIST_NORM_FAIL_99_96   ((u32)0x0000000F << 0)           /*!< R 4'b0  */
#define CTRL_MBIST_NORM_FAIL_99_96(x)     (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_MBIST_NORM_FAIL_99_96(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_MBIST_DRF_FAIL0
  * @brief
  * @{
  */
#define CTRL_MASK_MBIST_DRF_FAIL_31_0   ((u32)0xFFFFFFFF << 0)           /*!< R 32'b0  */
#define CTRL_MBIST_DRF_FAIL_31_0(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define CTRL_GET_MBIST_DRF_FAIL_31_0(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_CTRL_MBIST_DRF_FAIL1
  * @brief
  * @{
  */
#define CTRL_MASK_MBIST_DRF_FAIL_63_32   ((u32)0xFFFFFFFF << 0)           /*!< R 32'b0  */
#define CTRL_MBIST_DRF_FAIL_63_32(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define CTRL_GET_MBIST_DRF_FAIL_63_32(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_CTRL_MBIST_DRF_FAIL2
  * @brief
  * @{
  */
#define CTRL_MASK_MBIST_DRF_FAIL_95_64   ((u32)0xFFFFFFFF << 0)           /*!< R 32'b0  */
#define CTRL_MBIST_DRF_FAIL_95_64(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define CTRL_GET_MBIST_DRF_FAIL_95_64(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_CTRL_MBIST_DRF_FAIL3
  * @brief
  * @{
  */
#define CTRL_MASK_MBIST_DRF_FAIL_99_96   ((u32)0x0000000F << 0)           /*!< R 4'b0  */
#define CTRL_MBIST_DRF_FAIL_99_96(x)     (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_MBIST_DRF_FAIL_99_96(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_BT_VIR_E0_BIST_DONE0
  * @brief
  * @{
  */
#define CTRL_MASK_BT_VIR_E0_MBIST_DONE_19_0   ((u32)0x000FFFFF << 0)           /*!< R 20'b0  */
#define CTRL_BT_VIR_E0_MBIST_DONE_19_0(x)     (((u32)((x) & 0x000FFFFF) << 0))
#define CTRL_GET_BT_VIR_E0_MBIST_DONE_19_0(x) ((u32)(((x >> 0) & 0x000FFFFF)))
/** @} */

/** @defgroup REG_CTRL_BT_VIR_E0_BIST_FAIL0
  * @brief
  * @{
  */
#define CTRL_MASK_BT_VIR_E0_MBIST_FAIL_19_0   ((u32)0x000FFFFF << 0)           /*!< R 20'b0  */
#define CTRL_BT_VIR_E0_MBIST_FAIL_19_0(x)     (((u32)((x) & 0x000FFFFF) << 0))
#define CTRL_GET_BT_VIR_E0_MBIST_FAIL_19_0(x) ((u32)(((x >> 0) & 0x000FFFFF)))
/** @} */

/** @defgroup REG_CTRL_BT_VIR_E0_BIST_DRF_PAUSE0
  * @brief
  * @{
  */
#define CTRL_MASK_BT_VIR_E0_MBIST_DRF_PAUSE_19_0   ((u32)0x000FFFFF << 0)           /*!< R 20'b0  */
#define CTRL_BT_VIR_E0_MBIST_DRF_PAUSE_19_0(x)     (((u32)((x) & 0x000FFFFF) << 0))
#define CTRL_GET_BT_VIR_E0_MBIST_DRF_PAUSE_19_0(x) ((u32)(((x >> 0) & 0x000FFFFF)))
/** @} */

/** @defgroup REG_CTRL_BT_VIR_E0_BIST_RPT
  * @brief
  * @{
  */
#define CTRL_MASK_BT_VIR_E0_BIST_RPT_DOUT   ((u32)0xFFFFFFFF << 0)           /*!< R 32'b0  */
#define CTRL_BT_VIR_E0_BIST_RPT_DOUT(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define CTRL_GET_BT_VIR_E0_BIST_RPT_DOUT(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_CTRL_BT_VIR_E0_RPT_SEL
  * @brief
  * @{
  */
#define CTRL_MASK_BT_VIR_E0_RPT_SEL   ((u32)0x0000000F << 0)           /*!< R/W 4'b0  Select which rom's misr_dataout or bist_status of each memory can be read from bt_vir_e0_bist_rpt*/
#define CTRL_BT_VIR_E0_RPT_SEL(x)     (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_BT_VIR_E0_RPT_SEL(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_MISR_DOUT_SEL
  * @brief
  * @{
  */
#define CTRL_MASK_KM4TZ_ITCM0_E0_MISR_SEL   ((u32)0x00000003 << 5)           /*!< R/W 2'b0  */
#define CTRL_KM4TZ_ITCM0_E0_MISR_SEL(x)     (((u32)((x) & 0x00000003) << 5))
#define CTRL_GET_KM4TZ_ITCM0_E0_MISR_SEL(x) ((u32)(((x >> 5) & 0x00000003)))
#define CTRL_BIT_AONPMC_E0_MISR_SEL         ((u32)0x00000001 << 4)           /*!< R/W 1'b0  */
#define CTRL_MASK_HSYS_E1_MISR_SEL          ((u32)0x0000000F << 0)           /*!< R/W 4'b0  */
#define CTRL_HSYS_E1_MISR_SEL(x)            (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_HSYS_E1_MISR_SEL(x)        ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_HSYS_E1_ROM_MISR0
  * @brief
  * @{
  */
#define CTRL_MASK_HSYS_E1_MISR_DOUT_31_0   ((u32)0xFFFFFFFF << 0)           /*!< R 32'b0  */
#define CTRL_HSYS_E1_MISR_DOUT_31_0(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define CTRL_GET_HSYS_E1_MISR_DOUT_31_0(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_CTRL_HSYS_E1_ROM_MISR1
  * @brief
  * @{
  */
#define CTRL_MASK_HSYS_E1_MISR_DOUT_63_32   ((u32)0xFFFFFFFF << 0)           /*!< R 32'b0  */
#define CTRL_HSYS_E1_MISR_DOUT_63_32(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define CTRL_GET_HSYS_E1_MISR_DOUT_63_32(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_CTRL_LSPMC_E0_ROM_MISR1
  * @brief
  * @{
  */
#define CTRL_MASK_LSPMC_E0_MISR_DOUT_31_0   ((u32)0xFFFFFFFF << 0)           /*!< R 32'b0  */
#define CTRL_LSPMC_E0_MISR_DOUT_31_0(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define CTRL_GET_LSPMC_E0_MISR_DOUT_31_0(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_CTRL_LSPMC_E0_ROM_MISR2
  * @brief
  * @{
  */
#define CTRL_MASK_LSPMC_E0_MISR_DOUT_35_32   ((u32)0x0000000F << 0)           /*!< R 4'b0  */
#define CTRL_LSPMC_E0_MISR_DOUT_35_32(x)     (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_LSPMC_E0_MISR_DOUT_35_32(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_AONPMC_E0_ROM_MISR2
  * @brief
  * @{
  */
#define CTRL_MASK_AONPMC_E0_MISR_DOUT_31_0   ((u32)0xFFFFFFFF << 0)           /*!< R 32'b0  */
#define CTRL_AONPMC_E0_MISR_DOUT_31_0(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define CTRL_GET_AONPMC_E0_MISR_DOUT_31_0(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_CTRL_AONPMC_E0_ROM_MISR3
  * @brief
  * @{
  */
#define CTRL_MASK_AONPMC_E0_MISR_DOUT_35_32   ((u32)0x0000000F << 0)           /*!< R 4'b0  */
#define CTRL_AONPMC_E0_MISR_DOUT_35_32(x)     (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_AONPMC_E0_MISR_DOUT_35_32(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_KM4TZ_ITCM0_E0_ROM_MISR3
  * @brief
  * @{
  */
#define CTRL_MASK_KM4TZ_ITCM0_E0_MISR_DOUT_31_0   ((u32)0xFFFFFFFF << 0)           /*!< R 32'b0  */
#define CTRL_KM4TZ_ITCM0_E0_MISR_DOUT_31_0(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define CTRL_GET_KM4TZ_ITCM0_E0_MISR_DOUT_31_0(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_CTRL_KM4TZ_ITCM0_E0_ROM_MISR4
  * @brief
  * @{
  */
#define CTRL_MASK_KM4TZ_ITCM0_E0_MISR_DOUT_63_32   ((u32)0xFFFFFFFF << 0)           /*!< R 32'b0  */
#define CTRL_KM4TZ_ITCM0_E0_MISR_DOUT_63_32(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define CTRL_GET_KM4TZ_ITCM0_E0_MISR_DOUT_63_32(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_CTRL_PKE_E1_ROM_MISR4
  * @brief
  * @{
  */
#define CTRL_MASK_PKE_E1_MISR_DOUT_31_0   ((u32)0xFFFFFFFF << 0)           /*!< R 32'b0  */
#define CTRL_PKE_E1_MISR_DOUT_31_0(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define CTRL_GET_PKE_E1_MISR_DOUT_31_0(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_CTRL_PKE_E1_ROM_MISR5
  * @brief
  * @{
  */
#define CTRL_MASK_PKE_E1_MISR_DOUT_63_32   ((u32)0xFFFFFFFF << 0)           /*!< R 32'b0  */
#define CTRL_PKE_E1_MISR_DOUT_63_32(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define CTRL_GET_PKE_E1_MISR_DOUT_63_32(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_CTRL_GRP_PWR_MSK0
  * @brief
  * @{
  */
#define CTRL_BIT_GRP_PMSK_WPOFF_E0     ((u32)0x00000001 << 27)           /*!< R/W 0x1  */
#define CTRL_MASK_GRP_PMSK_WLK4_E0     ((u32)0x00000003 << 25)           /*!< R/W 0x3  */
#define CTRL_GRP_PMSK_WLK4_E0(x)       (((u32)((x) & 0x00000003) << 25))
#define CTRL_GET_GRP_PMSK_WLK4_E0(x)   ((u32)(((x >> 25) & 0x00000003)))
#define CTRL_BIT_GRP_PMSK_USB_E0       ((u32)0x00000001 << 24)           /*!< R/W 0x1  */
#define CTRL_BIT_GRP_PMSK_SDIO_E1      ((u32)0x00000001 << 23)           /*!< R/W 0x1  */
#define CTRL_MASK_GRP_PMSK_SDIO_E0     ((u32)0x00000003 << 21)           /*!< R/W 0x3  */
#define CTRL_GRP_PMSK_SDIO_E0(x)       (((u32)((x) & 0x00000003) << 21))
#define CTRL_GET_GRP_PMSK_SDIO_E0(x)   ((u32)(((x >> 21) & 0x00000003)))
#define CTRL_MASK_GRP_PMSK_AONPMC_E0   ((u32)0x00000003 << 19)           /*!< R/W 0x3  */
#define CTRL_GRP_PMSK_AONPMC_E0(x)     (((u32)((x) & 0x00000003) << 19))
#define CTRL_GET_GRP_PMSK_AONPMC_E0(x) ((u32)(((x >> 19) & 0x00000003)))
#define CTRL_BIT_GRP_PMSK_LSPMC_E0     ((u32)0x00000001 << 18)           /*!< R/W 0x1  */
#define CTRL_BIT_GRP_PMSK_LRET_E0      ((u32)0x00000001 << 17)           /*!< R/W 0x1  */
#define CTRL_MASK_GRP_PMSK_HSYS_E1     ((u32)0x000001FF << 8)            /*!< R/W 0x1ff  */
#define CTRL_GRP_PMSK_HSYS_E1(x)       (((u32)((x) & 0x000001FF) << 8))
#define CTRL_GET_GRP_PMSK_HSYS_E1(x)   ((u32)(((x >> 8) & 0x000001FF)))
#define CTRL_MASK_GRP_PMSK_HSYS_E0     ((u32)0x000000FF << 0)            /*!< R/W 0xff  */
#define CTRL_GRP_PMSK_HSYS_E0(x)       (((u32)((x) & 0x000000FF) << 0))
#define CTRL_GET_GRP_PMSK_HSYS_E0(x)   ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup REG_CTRL_GRP_PWR_MSK1
  * @brief
  * @{
  */
#define CTRL_MASK_GRP_PMSK_KM4NS_CACHE_E0   ((u32)0x0000000F << 28)           /*!< R/W 0xf  */
#define CTRL_GRP_PMSK_KM4NS_CACHE_E0(x)     (((u32)((x) & 0x0000000F) << 28))
#define CTRL_GET_GRP_PMSK_KM4NS_CACHE_E0(x) ((u32)(((x >> 28) & 0x0000000F)))
#define CTRL_MASK_GRP_PMSK_KM4TZ_ITCM0_E0   ((u32)0x0000000F << 24)           /*!< R/W 0xf  */
#define CTRL_GRP_PMSK_KM4TZ_ITCM0_E0(x)     (((u32)((x) & 0x0000000F) << 24))
#define CTRL_GET_GRP_PMSK_KM4TZ_ITCM0_E0(x) ((u32)(((x >> 24) & 0x0000000F)))
#define CTRL_MASK_GRP_PMSK_KM4TZ_CACHE_E0   ((u32)0x0000000F << 20)           /*!< R/W 0xf  */
#define CTRL_GRP_PMSK_KM4TZ_CACHE_E0(x)     (((u32)((x) & 0x0000000F) << 20))
#define CTRL_GET_GRP_PMSK_KM4TZ_CACHE_E0(x) ((u32)(((x >> 20) & 0x0000000F)))
#define CTRL_BIT_GRP_PMSK_BT_E1             ((u32)0x00000001 << 19)           /*!< R/W 0x1  */
#define CTRL_MASK_GRP_PMSK_BT_E0            ((u32)0x00000003 << 17)           /*!< R/W 0x3  */
#define CTRL_GRP_PMSK_BT_E0(x)              (((u32)((x) & 0x00000003) << 17))
#define CTRL_GET_GRP_PMSK_BT_E0(x)          ((u32)(((x >> 17) & 0x00000003)))
#define CTRL_BIT_GRP_PMSK_WPON_E0           ((u32)0x00000001 << 16)           /*!< R/W 0x1  */
#define CTRL_MASK_GRP_PMSK_WPOFF_E5         ((u32)0x00000003 << 14)           /*!< R/W 0x3  */
#define CTRL_GRP_PMSK_WPOFF_E5(x)           (((u32)((x) & 0x00000003) << 14))
#define CTRL_GET_GRP_PMSK_WPOFF_E5(x)       ((u32)(((x >> 14) & 0x00000003)))
#define CTRL_BIT_GRP_PMSK_WPOFF_E4          ((u32)0x00000001 << 13)           /*!< R/W 0x1  */
#define CTRL_BIT_GRP_PMSK_WPOFF_E3          ((u32)0x00000001 << 12)           /*!< R/W 0x1  */
#define CTRL_MASK_GRP_PMSK_WPOFF_E2         ((u32)0x00000003 << 10)           /*!< R/W 0x3  */
#define CTRL_GRP_PMSK_WPOFF_E2(x)           (((u32)((x) & 0x00000003) << 10))
#define CTRL_GET_GRP_PMSK_WPOFF_E2(x)       ((u32)(((x >> 10) & 0x00000003)))
#define CTRL_MASK_GRP_PMSK_WPOFF_E1         ((u32)0x000003FF << 0)            /*!< R/W 0x3ff  */
#define CTRL_GRP_PMSK_WPOFF_E1(x)           (((u32)((x) & 0x000003FF) << 0))
#define CTRL_GET_GRP_PMSK_WPOFF_E1(x)       ((u32)(((x >> 0) & 0x000003FF)))
/** @} */

/** @defgroup REG_CTRL_GRP_PWR_MSK2
  * @brief
  * @{
  */
#define CTRL_MASK_GRP_PMSK_PPE_E0     ((u32)0x000000FF << 23)           /*!< R/W 0xff  */
#define CTRL_GRP_PMSK_PPE_E0(x)       (((u32)((x) & 0x000000FF) << 23))
#define CTRL_GET_GRP_PMSK_PPE_E0(x)   ((u32)(((x >> 23) & 0x000000FF)))
#define CTRL_BIT_GRP_PMSK_PKE_E1      ((u32)0x00000001 << 22)           /*!< R/W 0x1  */
#define CTRL_MASK_GRP_PMSK_PKE_E0     ((u32)0x00000003 << 20)           /*!< R/W 0x3  */
#define CTRL_GRP_PMSK_PKE_E0(x)       (((u32)((x) & 0x00000003) << 20))
#define CTRL_GET_GRP_PMSK_PKE_E0(x)   ((u32)(((x >> 20) & 0x00000003)))
#define CTRL_MASK_GRP_PMSK_CAN_E0     ((u32)0x00000003 << 18)           /*!< R/W 0x3  */
#define CTRL_GRP_PMSK_CAN_E0(x)       (((u32)((x) & 0x00000003) << 18))
#define CTRL_GET_GRP_PMSK_CAN_E0(x)   ((u32)(((x >> 18) & 0x00000003)))
#define CTRL_MASK_GRP_PMSK_GMAC_E0    ((u32)0x00000003 << 16)           /*!< R/W 0x3  */
#define CTRL_GRP_PMSK_GMAC_E0(x)      (((u32)((x) & 0x00000003) << 16))
#define CTRL_GET_GRP_PMSK_GMAC_E0(x)  ((u32)(((x >> 16) & 0x00000003)))
#define CTRL_MASK_GRP_PMSK_MJPEG_E2   ((u32)0x0000007F << 9)            /*!< R/W 0x7f  */
#define CTRL_GRP_PMSK_MJPEG_E2(x)     (((u32)((x) & 0x0000007F) << 9))
#define CTRL_GET_GRP_PMSK_MJPEG_E2(x) ((u32)(((x >> 9) & 0x0000007F)))
#define CTRL_BIT_GRP_PMSK_MJPEG_E1    ((u32)0x00000001 << 8)            /*!< R/W 0x1  */
#define CTRL_MASK_GRP_PMSK_MJPEG_E0   ((u32)0x0000007F << 1)            /*!< R/W 0x7f  */
#define CTRL_GRP_PMSK_MJPEG_E0(x)     (((u32)((x) & 0x0000007F) << 1))
#define CTRL_GET_GRP_PMSK_MJPEG_E0(x) ((u32)(((x >> 1) & 0x0000007F)))
#define CTRL_BIT_GRP_PMSK_LCDC_E0     ((u32)0x00000001 << 0)            /*!< R/W 0x1  */
/** @} */

/** @defgroup REG_CTRL_GRP_PWR_MSK3
  * @brief
  * @{
  */
#define CTRL_MASK_GRP_PMSK_PPE_E1   ((u32)0x000000FF << 0)           /*!< R/W 0xff  */
#define CTRL_GRP_PMSK_PPE_E1(x)     (((u32)((x) & 0x000000FF) << 0))
#define CTRL_GET_GRP_PMSK_PPE_E1(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup REG_CTRL_GRP_HSYS_E0_CTRL
  * @brief
  * @{
  */
#define CTRL_MASK_HSYS_E0_WAEM          ((u32)0x0000000F << 18)           /*!< R/W 4'b1010  WAEM*/
#define CTRL_HSYS_E0_WAEM(x)            (((u32)((x) & 0x0000000F) << 18))
#define CTRL_GET_HSYS_E0_WAEM(x)        ((u32)(((x >> 18) & 0x0000000F)))
#define CTRL_BIT_HSYS_E0_WAE            ((u32)0x00000001 << 17)           /*!< R/W 1'b1  WAE*/
#define CTRL_BIT_HSYS_E0_RAE            ((u32)0x00000001 << 16)           /*!< R/W 1'b1  RAE*/
#define CTRL_BIT_HSYS_E0_TMCK           ((u32)0x00000001 << 15)           /*!< R/W 1'b0  TMCK*/
#define CTRL_MASK_HSYS_E0_DVS           ((u32)0x0000000F << 11)           /*!< R/W 4'b0000  DVS*/
#define CTRL_HSYS_E0_DVS(x)             (((u32)((x) & 0x0000000F) << 11))
#define CTRL_GET_HSYS_E0_DVS(x)         ((u32)(((x >> 11) & 0x0000000F)))
#define CTRL_BIT_HSYS_E0_DVSE           ((u32)0x00000001 << 10)           /*!< R/W 1'b0  DVSE*/
#define CTRL_MASK_GRP_HSYS_E0_MSEL      ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_HSYS_E0_MSEL(x)        (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_HSYS_E0_MSEL(x)    ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_HSYS_E0_LOOP_MODE  ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
#define CTRL_MASK_GRP_HSYS_E0_PWR_LPS   ((u32)0x00000007 << 3)            /*!< R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_HSYS_E0_PWR_LPS(x)     (((u32)((x) & 0x00000007) << 3))
#define CTRL_GET_GRP_HSYS_E0_PWR_LPS(x) ((u32)(((x >> 3) & 0x00000007)))
#define CTRL_MASK_GRP_HSYS_E0_PWR_ACT   ((u32)0x00000007 << 0)            /*!< R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_HSYS_E0_PWR_ACT(x)     (((u32)((x) & 0x00000007) << 0))
#define CTRL_GET_GRP_HSYS_E0_PWR_ACT(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_CTRL_GRP_HSYS_E1_CTRL
  * @brief
  * @{
  */
#define CTRL_MASK_HSYS_E1_DVS           ((u32)0x0000000F << 11)           /*!< R/W 4'b0101  DVS*/
#define CTRL_HSYS_E1_DVS(x)             (((u32)((x) & 0x0000000F) << 11))
#define CTRL_GET_HSYS_E1_DVS(x)         ((u32)(((x >> 11) & 0x0000000F)))
#define CTRL_BIT_HSYS_E1_DVSE           ((u32)0x00000001 << 10)           /*!< R/W 1'b0  DVSE*/
#define CTRL_MASK_GRP_HSYS_E1_MSEL      ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_HSYS_E1_MSEL(x)        (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_HSYS_E1_MSEL(x)    ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_HSYS_E1_LOOP_MODE  ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
#define CTRL_MASK_GRP_HSYS_E1_PWR_LPS   ((u32)0x00000007 << 3)            /*!< R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_HSYS_E1_PWR_LPS(x)     (((u32)((x) & 0x00000007) << 3))
#define CTRL_GET_GRP_HSYS_E1_PWR_LPS(x) ((u32)(((x >> 3) & 0x00000007)))
#define CTRL_MASK_GRP_HSYS_E1_PWR_ACT   ((u32)0x00000007 << 0)            /*!< R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_HSYS_E1_PWR_ACT(x)     (((u32)((x) & 0x00000007) << 0))
#define CTRL_GET_GRP_HSYS_E1_PWR_ACT(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_CTRL_GRP_LRET_E0_CTRL
  * @brief
  * @{
  */
#define CTRL_MASK_LRET_E0_WAEM          ((u32)0x0000000F << 18)           /*!< R/W 4'b1011  WAEM*/
#define CTRL_LRET_E0_WAEM(x)            (((u32)((x) & 0x0000000F) << 18))
#define CTRL_GET_LRET_E0_WAEM(x)        ((u32)(((x >> 18) & 0x0000000F)))
#define CTRL_BIT_LRET_E0_WAE            ((u32)0x00000001 << 17)           /*!< R/W 1'b1  WAE*/
#define CTRL_BIT_LRET_E0_RAE            ((u32)0x00000001 << 16)           /*!< R/W 1'b1  RAE*/
#define CTRL_BIT_LRET_E0_TMCK           ((u32)0x00000001 << 15)           /*!< R/W 1'b0  TMCK*/
#define CTRL_MASK_LRET_E0_DVS           ((u32)0x0000000F << 11)           /*!< R/W 4'b0000  DVS*/
#define CTRL_LRET_E0_DVS(x)             (((u32)((x) & 0x0000000F) << 11))
#define CTRL_GET_LRET_E0_DVS(x)         ((u32)(((x >> 11) & 0x0000000F)))
#define CTRL_BIT_LRET_E0_DVSE           ((u32)0x00000001 << 10)           /*!< R/W 1'b0  DVSE*/
#define CTRL_MASK_GRP_LRET_E0_MSEL      ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_LRET_E0_MSEL(x)        (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_LRET_E0_MSEL(x)    ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_LRET_E0_LOOP_MODE  ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
#define CTRL_MASK_GRP_LRET_E0_PWR_LPS   ((u32)0x00000007 << 3)            /*!< R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_LRET_E0_PWR_LPS(x)     (((u32)((x) & 0x00000007) << 3))
#define CTRL_GET_GRP_LRET_E0_PWR_LPS(x) ((u32)(((x >> 3) & 0x00000007)))
#define CTRL_MASK_GRP_LRET_E0_PWR_ACT   ((u32)0x00000007 << 0)            /*!< R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_LRET_E0_PWR_ACT(x)     (((u32)((x) & 0x00000007) << 0))
#define CTRL_GET_GRP_LRET_E0_PWR_ACT(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_CTRL_GRP_LSPMC_E0_CTRL
  * @brief
  * @{
  */
#define CTRL_MASK_LSPMC_E0_DVS           ((u32)0x0000000F << 11)           /*!< R/W 4'b0011  DVS*/
#define CTRL_LSPMC_E0_DVS(x)             (((u32)((x) & 0x0000000F) << 11))
#define CTRL_GET_LSPMC_E0_DVS(x)         ((u32)(((x >> 11) & 0x0000000F)))
#define CTRL_BIT_LSPMC_E0_DVSE           ((u32)0x00000001 << 10)           /*!< R/W 1'b0  DVSE*/
#define CTRL_MASK_GRP_LSPMC_E0_MSEL      ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_LSPMC_E0_MSEL(x)        (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_LSPMC_E0_MSEL(x)    ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_LSPMC_E0_LOOP_MODE  ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
#define CTRL_MASK_GRP_LSPMC_E0_PWR_LPS   ((u32)0x00000007 << 3)            /*!< R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_LSPMC_E0_PWR_LPS(x)     (((u32)((x) & 0x00000007) << 3))
#define CTRL_GET_GRP_LSPMC_E0_PWR_LPS(x) ((u32)(((x >> 3) & 0x00000007)))
#define CTRL_MASK_GRP_LSPMC_E0_PWR_ACT   ((u32)0x00000007 << 0)            /*!< R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_LSPMC_E0_PWR_ACT(x)     (((u32)((x) & 0x00000007) << 0))
#define CTRL_GET_GRP_LSPMC_E0_PWR_ACT(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_CTRL_GRP_AONPMC_E0_CTRL
  * @brief
  * @{
  */
#define CTRL_MASK_AONPMC_E0_DVS           ((u32)0x0000000F << 11)           /*!< R/W 4'b0011  DVS*/
#define CTRL_AONPMC_E0_DVS(x)             (((u32)((x) & 0x0000000F) << 11))
#define CTRL_GET_AONPMC_E0_DVS(x)         ((u32)(((x >> 11) & 0x0000000F)))
#define CTRL_BIT_AONPMC_E0_DVSE           ((u32)0x00000001 << 10)           /*!< R/W 1'b0  DVSE*/
#define CTRL_MASK_GRP_AONPMC_E0_MSEL      ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_AONPMC_E0_MSEL(x)        (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_AONPMC_E0_MSEL(x)    ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_AONPMC_E0_LOOP_MODE  ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
#define CTRL_MASK_GRP_AONPMC_E0_PWR_LPS   ((u32)0x00000007 << 3)            /*!< R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_AONPMC_E0_PWR_LPS(x)     (((u32)((x) & 0x00000007) << 3))
#define CTRL_GET_GRP_AONPMC_E0_PWR_LPS(x) ((u32)(((x >> 3) & 0x00000007)))
#define CTRL_MASK_GRP_AONPMC_E0_PWR_ACT   ((u32)0x00000007 << 0)            /*!< R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_AONPMC_E0_PWR_ACT(x)     (((u32)((x) & 0x00000007) << 0))
#define CTRL_GET_GRP_AONPMC_E0_PWR_ACT(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_CTRL_GRP_SDIO_E0_CTRL
  * @brief
  * @{
  */
#define CTRL_BIT_SDIO_E0_TEST1          ((u32)0x00000001 << 15)           /*!< R/W 1'b0  TEST1*/
#define CTRL_MASK_SDIO_E0_RM            ((u32)0x0000000F << 11)           /*!< R/W 4'b0010  RM*/
#define CTRL_SDIO_E0_RM(x)              (((u32)((x) & 0x0000000F) << 11))
#define CTRL_GET_SDIO_E0_RM(x)          ((u32)(((x >> 11) & 0x0000000F)))
#define CTRL_BIT_SDIO_E0_RME            ((u32)0x00000001 << 10)           /*!< R/W 1'b0  RME*/
#define CTRL_MASK_GRP_SDIO_E0_MSEL      ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_SDIO_E0_MSEL(x)        (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_SDIO_E0_MSEL(x)    ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_SDIO_E0_LOOP_MODE  ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
#define CTRL_MASK_GRP_SDIO_E0_PWR_LPS   ((u32)0x00000007 << 3)            /*!< R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_SDIO_E0_PWR_LPS(x)     (((u32)((x) & 0x00000007) << 3))
#define CTRL_GET_GRP_SDIO_E0_PWR_LPS(x) ((u32)(((x >> 3) & 0x00000007)))
#define CTRL_MASK_GRP_SDIO_E0_PWR_ACT   ((u32)0x00000007 << 0)            /*!< R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_SDIO_E0_PWR_ACT(x)     (((u32)((x) & 0x00000007) << 0))
#define CTRL_GET_GRP_SDIO_E0_PWR_ACT(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_CTRL_GRP_SDIO_E1_CTRL
  * @brief
  * @{
  */
#define CTRL_BIT_SDIO_E1_TEST1B         ((u32)0x00000001 << 21)           /*!< R/W 1'b0  TEST1B*/
#define CTRL_BIT_SDIO_E1_TEST1A         ((u32)0x00000001 << 20)           /*!< R/W 1'b0  TEST1A*/
#define CTRL_MASK_SDIO_E1_RMB           ((u32)0x0000000F << 16)           /*!< R/W 4'b0010  RMB*/
#define CTRL_SDIO_E1_RMB(x)             (((u32)((x) & 0x0000000F) << 16))
#define CTRL_GET_SDIO_E1_RMB(x)         ((u32)(((x >> 16) & 0x0000000F)))
#define CTRL_MASK_SDIO_E1_RMA           ((u32)0x0000000F << 12)           /*!< R/W 4'b0010  RMA*/
#define CTRL_SDIO_E1_RMA(x)             (((u32)((x) & 0x0000000F) << 12))
#define CTRL_GET_SDIO_E1_RMA(x)         ((u32)(((x >> 12) & 0x0000000F)))
#define CTRL_BIT_SDIO_E1_RMEB           ((u32)0x00000001 << 11)           /*!< R/W 1'b0  RMEB*/
#define CTRL_BIT_SDIO_E1_RMEA           ((u32)0x00000001 << 10)           /*!< R/W 1'b0  RMEA*/
#define CTRL_MASK_GRP_SDIO_E1_MSEL      ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_SDIO_E1_MSEL(x)        (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_SDIO_E1_MSEL(x)    ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_SDIO_E1_LOOP_MODE  ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
#define CTRL_MASK_GRP_SDIO_E1_PWR_LPS   ((u32)0x00000007 << 3)            /*!< R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_SDIO_E1_PWR_LPS(x)     (((u32)((x) & 0x00000007) << 3))
#define CTRL_GET_GRP_SDIO_E1_PWR_LPS(x) ((u32)(((x >> 3) & 0x00000007)))
#define CTRL_MASK_GRP_SDIO_E1_PWR_ACT   ((u32)0x00000007 << 0)            /*!< R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_SDIO_E1_PWR_ACT(x)     (((u32)((x) & 0x00000007) << 0))
#define CTRL_GET_GRP_SDIO_E1_PWR_ACT(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_CTRL_GRP_USB_E0_CTRL
  * @brief
  * @{
  */
#define CTRL_BIT_USB_E0_TEST1          ((u32)0x00000001 << 15)           /*!< R/W 1'b0  TEST1*/
#define CTRL_MASK_USB_E0_RM            ((u32)0x0000000F << 11)           /*!< R/W 4'b0010  RM*/
#define CTRL_USB_E0_RM(x)              (((u32)((x) & 0x0000000F) << 11))
#define CTRL_GET_USB_E0_RM(x)          ((u32)(((x >> 11) & 0x0000000F)))
#define CTRL_BIT_USB_E0_RME            ((u32)0x00000001 << 10)           /*!< R/W 1'b0  RME*/
#define CTRL_MASK_GRP_USB_E0_MSEL      ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_USB_E0_MSEL(x)        (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_USB_E0_MSEL(x)    ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_USB_E0_LOOP_MODE  ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
#define CTRL_MASK_GRP_USB_E0_PWR_LPS   ((u32)0x00000007 << 3)            /*!< R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_USB_E0_PWR_LPS(x)     (((u32)((x) & 0x00000007) << 3))
#define CTRL_GET_GRP_USB_E0_PWR_LPS(x) ((u32)(((x >> 3) & 0x00000007)))
#define CTRL_MASK_GRP_USB_E0_PWR_ACT   ((u32)0x00000007 << 0)            /*!< R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_USB_E0_PWR_ACT(x)     (((u32)((x) & 0x00000007) << 0))
#define CTRL_GET_GRP_USB_E0_PWR_ACT(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_CTRL_GRP_WLK4_E0_CTRL
  * @brief
  * @{
  */
#define CTRL_BIT_WLK4_E0_TMCK          ((u32)0x00000001 << 15)           /*!< R/W 1'b0  TMCK*/
#define CTRL_MASK_WLK4_E0_DVS          ((u32)0x0000000F << 11)           /*!< R/W 4'b0101  DVS*/
#define CTRL_WLK4_E0_DVS(x)            (((u32)((x) & 0x0000000F) << 11))
#define CTRL_GET_WLK4_E0_DVS(x)        ((u32)(((x >> 11) & 0x0000000F)))
#define CTRL_BIT_WLK4_E0_DVSE          ((u32)0x00000001 << 10)           /*!< R/W 1'b0  DVSE*/
#define CTRL_MASK_GRP_WLK4_E0_MSEL     ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_WLK4_E0_MSEL(x)       (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_WLK4_E0_MSEL(x)   ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_WLK4_E0_LOOP_MODE ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
/** @} */

/** @defgroup REG_CTRL_GRP_WPOFF_E0_CTRL
  * @brief
  * @{
  */
#define CTRL_BIT_WPOFF_E0_TEST1         ((u32)0x00000001 << 15)           /*!< R/W 1'b0  TEST1*/
#define CTRL_MASK_WPOFF_E0_RM           ((u32)0x0000000F << 11)           /*!< R/W 4'b0010  RM*/
#define CTRL_WPOFF_E0_RM(x)             (((u32)((x) & 0x0000000F) << 11))
#define CTRL_GET_WPOFF_E0_RM(x)         ((u32)(((x >> 11) & 0x0000000F)))
#define CTRL_BIT_WPOFF_E0_RME           ((u32)0x00000001 << 10)           /*!< R/W 1'b0  RME*/
#define CTRL_MASK_GRP_WPOFF_E0_MSEL     ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_WPOFF_E0_MSEL(x)       (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_WPOFF_E0_MSEL(x)   ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_WPOFF_E0_LOOP_MODE ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
/** @} */

/** @defgroup REG_CTRL_GRP_WPOFF_E1_CTRL
  * @brief
  * @{
  */
#define CTRL_BIT_WPOFF_E1_TEST1         ((u32)0x00000001 << 15)           /*!< R/W 1'b0  TEST1*/
#define CTRL_MASK_WPOFF_E1_RM           ((u32)0x0000000F << 11)           /*!< R/W 4'b0010  RM*/
#define CTRL_WPOFF_E1_RM(x)             (((u32)((x) & 0x0000000F) << 11))
#define CTRL_GET_WPOFF_E1_RM(x)         ((u32)(((x >> 11) & 0x0000000F)))
#define CTRL_BIT_WPOFF_E1_RME           ((u32)0x00000001 << 10)           /*!< R/W 1'b0  RME*/
#define CTRL_MASK_GRP_WPOFF_E1_MSEL     ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_WPOFF_E1_MSEL(x)       (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_WPOFF_E1_MSEL(x)   ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_WPOFF_E1_LOOP_MODE ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
/** @} */

/** @defgroup REG_CTRL_GRP_WPOFF_E2_CTRL
  * @brief
  * @{
  */
#define CTRL_BIT_WPOFF_E2_TEST1B        ((u32)0x00000001 << 21)           /*!< R/W 1'b0  TEST1B*/
#define CTRL_BIT_WPOFF_E2_TEST1A        ((u32)0x00000001 << 20)           /*!< R/W 1'b0  TEST1A*/
#define CTRL_MASK_WPOFF_E2_RMB          ((u32)0x0000000F << 16)           /*!< R/W 4'b0010  RMB*/
#define CTRL_WPOFF_E2_RMB(x)            (((u32)((x) & 0x0000000F) << 16))
#define CTRL_GET_WPOFF_E2_RMB(x)        ((u32)(((x >> 16) & 0x0000000F)))
#define CTRL_MASK_WPOFF_E2_RMA          ((u32)0x0000000F << 12)           /*!< R/W 4'b0010  RMA*/
#define CTRL_WPOFF_E2_RMA(x)            (((u32)((x) & 0x0000000F) << 12))
#define CTRL_GET_WPOFF_E2_RMA(x)        ((u32)(((x >> 12) & 0x0000000F)))
#define CTRL_BIT_WPOFF_E2_RMEB          ((u32)0x00000001 << 11)           /*!< R/W 1'b0  RMEB*/
#define CTRL_BIT_WPOFF_E2_RMEA          ((u32)0x00000001 << 10)           /*!< R/W 1'b0  RMEA*/
#define CTRL_MASK_GRP_WPOFF_E2_MSEL     ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_WPOFF_E2_MSEL(x)       (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_WPOFF_E2_MSEL(x)   ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_WPOFF_E2_LOOP_MODE ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
/** @} */

/** @defgroup REG_CTRL_GRP_WPOFF_E3_CTRL
  * @brief
  * @{
  */
#define CTRL_BIT_WPOFF_E3_TEST1         ((u32)0x00000001 << 15)           /*!< R/W 1'b0  TEST1*/
#define CTRL_MASK_WPOFF_E3_RM           ((u32)0x0000000F << 11)           /*!< R/W 4'b0010  RM*/
#define CTRL_WPOFF_E3_RM(x)             (((u32)((x) & 0x0000000F) << 11))
#define CTRL_GET_WPOFF_E3_RM(x)         ((u32)(((x >> 11) & 0x0000000F)))
#define CTRL_BIT_WPOFF_E3_RME           ((u32)0x00000001 << 10)           /*!< R/W 1'b0  RME*/
#define CTRL_MASK_GRP_WPOFF_E3_MSEL     ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_WPOFF_E3_MSEL(x)       (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_WPOFF_E3_MSEL(x)   ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_WPOFF_E3_LOOP_MODE ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
/** @} */

/** @defgroup REG_CTRL_GRP_WPOFF_E4_CTRL
  * @brief
  * @{
  */
#define CTRL_BIT_WPOFF_E4_TEST1         ((u32)0x00000001 << 15)           /*!< R/W 1'b0  TEST1*/
#define CTRL_MASK_WPOFF_E4_RM           ((u32)0x0000000F << 11)           /*!< R/W 4'b0010  RM*/
#define CTRL_WPOFF_E4_RM(x)             (((u32)((x) & 0x0000000F) << 11))
#define CTRL_GET_WPOFF_E4_RM(x)         ((u32)(((x >> 11) & 0x0000000F)))
#define CTRL_BIT_WPOFF_E4_RME           ((u32)0x00000001 << 10)           /*!< R/W 1'b0  RME*/
#define CTRL_MASK_GRP_WPOFF_E4_MSEL     ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_WPOFF_E4_MSEL(x)       (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_WPOFF_E4_MSEL(x)   ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_WPOFF_E4_LOOP_MODE ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
/** @} */

/** @defgroup REG_CTRL_GRP_WPOFF_E5_CTRL
  * @brief
  * @{
  */
#define CTRL_BIT_WPOFF_E5_TEST1B        ((u32)0x00000001 << 21)           /*!< R/W 1'b0  TEST1B*/
#define CTRL_BIT_WPOFF_E5_TEST1A        ((u32)0x00000001 << 20)           /*!< R/W 1'b0  TEST1A*/
#define CTRL_MASK_WPOFF_E5_RMB          ((u32)0x0000000F << 16)           /*!< R/W 4'b0010  RMB*/
#define CTRL_WPOFF_E5_RMB(x)            (((u32)((x) & 0x0000000F) << 16))
#define CTRL_GET_WPOFF_E5_RMB(x)        ((u32)(((x >> 16) & 0x0000000F)))
#define CTRL_MASK_WPOFF_E5_RMA          ((u32)0x0000000F << 12)           /*!< R/W 4'b0010  RMA*/
#define CTRL_WPOFF_E5_RMA(x)            (((u32)((x) & 0x0000000F) << 12))
#define CTRL_GET_WPOFF_E5_RMA(x)        ((u32)(((x >> 12) & 0x0000000F)))
#define CTRL_BIT_WPOFF_E5_RMEB          ((u32)0x00000001 << 11)           /*!< R/W 1'b0  RMEB*/
#define CTRL_BIT_WPOFF_E5_RMEA          ((u32)0x00000001 << 10)           /*!< R/W 1'b0  RMEA*/
#define CTRL_MASK_GRP_WPOFF_E5_MSEL     ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_WPOFF_E5_MSEL(x)       (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_WPOFF_E5_MSEL(x)   ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_WPOFF_E5_LOOP_MODE ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
/** @} */

/** @defgroup REG_CTRL_GRP_WPON_E0_CTRL
  * @brief
  * @{
  */
#define CTRL_MASK_WPON_E0_WAEM         ((u32)0x0000000F << 18)           /*!< R/W 4'b1010  WAEM*/
#define CTRL_WPON_E0_WAEM(x)           (((u32)((x) & 0x0000000F) << 18))
#define CTRL_GET_WPON_E0_WAEM(x)       ((u32)(((x >> 18) & 0x0000000F)))
#define CTRL_BIT_WPON_E0_WAE           ((u32)0x00000001 << 17)           /*!< R/W 1'b1  WAE*/
#define CTRL_BIT_WPON_E0_RAE           ((u32)0x00000001 << 16)           /*!< R/W 1'b1  RAE*/
#define CTRL_BIT_WPON_E0_TMCK          ((u32)0x00000001 << 15)           /*!< R/W 1'b0  TMCK*/
#define CTRL_MASK_WPON_E0_DVS          ((u32)0x0000000F << 11)           /*!< R/W 4'b0000  DVS*/
#define CTRL_WPON_E0_DVS(x)            (((u32)((x) & 0x0000000F) << 11))
#define CTRL_GET_WPON_E0_DVS(x)        ((u32)(((x >> 11) & 0x0000000F)))
#define CTRL_BIT_WPON_E0_DVSE          ((u32)0x00000001 << 10)           /*!< R/W 1'b0  DVSE*/
#define CTRL_MASK_GRP_WPON_E0_MSEL     ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_WPON_E0_MSEL(x)       (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_WPON_E0_MSEL(x)   ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_WPON_E0_LOOP_MODE ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
/** @} */

/** @defgroup REG_CTRL_GRP_BT_E0_CTRL
  * @brief
  * @{
  */
#define CTRL_MASK_BT_E0_WAEM          ((u32)0x0000000F << 18)           /*!< R/W 4'b1010  WAEM*/
#define CTRL_BT_E0_WAEM(x)            (((u32)((x) & 0x0000000F) << 18))
#define CTRL_GET_BT_E0_WAEM(x)        ((u32)(((x >> 18) & 0x0000000F)))
#define CTRL_BIT_BT_E0_WAE            ((u32)0x00000001 << 17)           /*!< R/W 1'b1  WAE*/
#define CTRL_BIT_BT_E0_RAE            ((u32)0x00000001 << 16)           /*!< R/W 1'b1  RAE*/
#define CTRL_BIT_BT_E0_TMCK           ((u32)0x00000001 << 15)           /*!< R/W 1'b0  TMCK*/
#define CTRL_MASK_BT_E0_DVS           ((u32)0x0000000F << 11)           /*!< R/W 4'b0000  DVS*/
#define CTRL_BT_E0_DVS(x)             (((u32)((x) & 0x0000000F) << 11))
#define CTRL_GET_BT_E0_DVS(x)         ((u32)(((x >> 11) & 0x0000000F)))
#define CTRL_BIT_BT_E0_DVSE           ((u32)0x00000001 << 10)           /*!< R/W 1'b0  DVSE*/
#define CTRL_MASK_GRP_BT_E0_MSEL      ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_BT_E0_MSEL(x)        (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_BT_E0_MSEL(x)    ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_BT_E0_LOOP_MODE  ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
#define CTRL_MASK_GRP_BT_E0_PWR_LPS   ((u32)0x00000007 << 3)            /*!< R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_BT_E0_PWR_LPS(x)     (((u32)((x) & 0x00000007) << 3))
#define CTRL_GET_GRP_BT_E0_PWR_LPS(x) ((u32)(((x >> 3) & 0x00000007)))
#define CTRL_MASK_GRP_BT_E0_PWR_ACT   ((u32)0x00000007 << 0)            /*!< R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_BT_E0_PWR_ACT(x)     (((u32)((x) & 0x00000007) << 0))
#define CTRL_GET_GRP_BT_E0_PWR_ACT(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_CTRL_GRP_BT_E1_CTRL
  * @brief
  * @{
  */
#define CTRL_MASK_BT_E1_WAEM          ((u32)0x0000000F << 18)           /*!< R/W 4'b1010  WAEM*/
#define CTRL_BT_E1_WAEM(x)            (((u32)((x) & 0x0000000F) << 18))
#define CTRL_GET_BT_E1_WAEM(x)        ((u32)(((x >> 18) & 0x0000000F)))
#define CTRL_BIT_BT_E1_WAE            ((u32)0x00000001 << 17)           /*!< R/W 1'b1  WAE*/
#define CTRL_BIT_BT_E1_RAE            ((u32)0x00000001 << 16)           /*!< R/W 1'b1  RAE*/
#define CTRL_BIT_BT_E1_TMCK           ((u32)0x00000001 << 15)           /*!< R/W 1'b0  TMCK*/
#define CTRL_MASK_BT_E1_DVS           ((u32)0x0000000F << 11)           /*!< R/W 4'b0000  DVS*/
#define CTRL_BT_E1_DVS(x)             (((u32)((x) & 0x0000000F) << 11))
#define CTRL_GET_BT_E1_DVS(x)         ((u32)(((x >> 11) & 0x0000000F)))
#define CTRL_BIT_BT_E1_DVSE           ((u32)0x00000001 << 10)           /*!< R/W 1'b0  DVSE*/
#define CTRL_MASK_GRP_BT_E1_MSEL      ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_BT_E1_MSEL(x)        (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_BT_E1_MSEL(x)    ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_BT_E1_LOOP_MODE  ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
#define CTRL_MASK_GRP_BT_E1_PWR_LPS   ((u32)0x00000007 << 3)            /*!< R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_BT_E1_PWR_LPS(x)     (((u32)((x) & 0x00000007) << 3))
#define CTRL_GET_GRP_BT_E1_PWR_LPS(x) ((u32)(((x >> 3) & 0x00000007)))
#define CTRL_MASK_GRP_BT_E1_PWR_ACT   ((u32)0x00000007 << 0)            /*!< R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_BT_E1_PWR_ACT(x)     (((u32)((x) & 0x00000007) << 0))
#define CTRL_GET_GRP_BT_E1_PWR_ACT(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_CTRL_GRP_KM4TZ_CACHE_E0_CTRL
  * @brief
  * @{
  */
#define CTRL_BIT_KM4TZ_CACHE_E0_TEST1          ((u32)0x00000001 << 15)           /*!< R/W 1'b0  TEST1*/
#define CTRL_MASK_KM4TZ_CACHE_E0_RM            ((u32)0x0000000F << 11)           /*!< R/W 4'b0010  RM*/
#define CTRL_KM4TZ_CACHE_E0_RM(x)              (((u32)((x) & 0x0000000F) << 11))
#define CTRL_GET_KM4TZ_CACHE_E0_RM(x)          ((u32)(((x >> 11) & 0x0000000F)))
#define CTRL_BIT_KM4TZ_CACHE_E0_RME            ((u32)0x00000001 << 10)           /*!< R/W 1'b0  RME*/
#define CTRL_MASK_GRP_KM4TZ_CACHE_E0_MSEL      ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_KM4TZ_CACHE_E0_MSEL(x)        (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_KM4TZ_CACHE_E0_MSEL(x)    ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_KM4TZ_CACHE_E0_LOOP_MODE  ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
#define CTRL_MASK_GRP_KM4TZ_CACHE_E0_PWR_LPS   ((u32)0x00000007 << 3)            /*!< R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_KM4TZ_CACHE_E0_PWR_LPS(x)     (((u32)((x) & 0x00000007) << 3))
#define CTRL_GET_GRP_KM4TZ_CACHE_E0_PWR_LPS(x) ((u32)(((x >> 3) & 0x00000007)))
#define CTRL_MASK_GRP_KM4TZ_CACHE_E0_PWR_ACT   ((u32)0x00000007 << 0)            /*!< R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_KM4TZ_CACHE_E0_PWR_ACT(x)     (((u32)((x) & 0x00000007) << 0))
#define CTRL_GET_GRP_KM4TZ_CACHE_E0_PWR_ACT(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_CTRL_GRP_KM4TZ_ITCM0_E0_CTRL
  * @brief
  * @{
  */
#define CTRL_MASK_KM4TZ_ITCM0_E0_DVS           ((u32)0x0000000F << 11)           /*!< R/W 4'b0101  DVS*/
#define CTRL_KM4TZ_ITCM0_E0_DVS(x)             (((u32)((x) & 0x0000000F) << 11))
#define CTRL_GET_KM4TZ_ITCM0_E0_DVS(x)         ((u32)(((x >> 11) & 0x0000000F)))
#define CTRL_BIT_KM4TZ_ITCM0_E0_DVSE           ((u32)0x00000001 << 10)           /*!< R/W 1'b0  DVSE*/
#define CTRL_MASK_GRP_KM4TZ_ITCM0_E0_MSEL      ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_KM4TZ_ITCM0_E0_MSEL(x)        (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_KM4TZ_ITCM0_E0_MSEL(x)    ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_KM4TZ_ITCM0_E0_LOOP_MODE  ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
#define CTRL_MASK_GRP_KM4TZ_ITCM0_E0_PWR_LPS   ((u32)0x00000007 << 3)            /*!< R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_KM4TZ_ITCM0_E0_PWR_LPS(x)     (((u32)((x) & 0x00000007) << 3))
#define CTRL_GET_GRP_KM4TZ_ITCM0_E0_PWR_LPS(x) ((u32)(((x >> 3) & 0x00000007)))
#define CTRL_MASK_GRP_KM4TZ_ITCM0_E0_PWR_ACT   ((u32)0x00000007 << 0)            /*!< R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_KM4TZ_ITCM0_E0_PWR_ACT(x)     (((u32)((x) & 0x00000007) << 0))
#define CTRL_GET_GRP_KM4TZ_ITCM0_E0_PWR_ACT(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_CTRL_GRP_KM4NS_CACHE_E0_CTRL
  * @brief
  * @{
  */
#define CTRL_BIT_KM4NS_CACHE_E0_TEST1          ((u32)0x00000001 << 15)           /*!< R/W 1'b0  TEST1*/
#define CTRL_MASK_KM4NS_CACHE_E0_RM            ((u32)0x0000000F << 11)           /*!< R/W 4'b0010  RM*/
#define CTRL_KM4NS_CACHE_E0_RM(x)              (((u32)((x) & 0x0000000F) << 11))
#define CTRL_GET_KM4NS_CACHE_E0_RM(x)          ((u32)(((x >> 11) & 0x0000000F)))
#define CTRL_BIT_KM4NS_CACHE_E0_RME            ((u32)0x00000001 << 10)           /*!< R/W 1'b0  RME*/
#define CTRL_MASK_GRP_KM4NS_CACHE_E0_MSEL      ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_KM4NS_CACHE_E0_MSEL(x)        (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_KM4NS_CACHE_E0_MSEL(x)    ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_KM4NS_CACHE_E0_LOOP_MODE  ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
#define CTRL_MASK_GRP_KM4NS_CACHE_E0_PWR_LPS   ((u32)0x00000007 << 3)            /*!< R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_KM4NS_CACHE_E0_PWR_LPS(x)     (((u32)((x) & 0x00000007) << 3))
#define CTRL_GET_GRP_KM4NS_CACHE_E0_PWR_LPS(x) ((u32)(((x >> 3) & 0x00000007)))
#define CTRL_MASK_GRP_KM4NS_CACHE_E0_PWR_ACT   ((u32)0x00000007 << 0)            /*!< R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_KM4NS_CACHE_E0_PWR_ACT(x)     (((u32)((x) & 0x00000007) << 0))
#define CTRL_GET_GRP_KM4NS_CACHE_E0_PWR_ACT(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_CTRL_GRP_LCDC_E0_CTRL
  * @brief
  * @{
  */
#define CTRL_BIT_LCDC_E0_TEST1B         ((u32)0x00000001 << 21)           /*!< R/W 1'b0  TEST1B*/
#define CTRL_BIT_LCDC_E0_TEST1A         ((u32)0x00000001 << 20)           /*!< R/W 1'b0  TEST1A*/
#define CTRL_MASK_LCDC_E0_RMB           ((u32)0x0000000F << 16)           /*!< R/W 4'b0010  RMB*/
#define CTRL_LCDC_E0_RMB(x)             (((u32)((x) & 0x0000000F) << 16))
#define CTRL_GET_LCDC_E0_RMB(x)         ((u32)(((x >> 16) & 0x0000000F)))
#define CTRL_MASK_LCDC_E0_RMA           ((u32)0x0000000F << 12)           /*!< R/W 4'b0010  RMA*/
#define CTRL_LCDC_E0_RMA(x)             (((u32)((x) & 0x0000000F) << 12))
#define CTRL_GET_LCDC_E0_RMA(x)         ((u32)(((x >> 12) & 0x0000000F)))
#define CTRL_BIT_LCDC_E0_RMEB           ((u32)0x00000001 << 11)           /*!< R/W 1'b0  RMEB*/
#define CTRL_BIT_LCDC_E0_RMEA           ((u32)0x00000001 << 10)           /*!< R/W 1'b0  RMEA*/
#define CTRL_MASK_GRP_LCDC_E0_MSEL      ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_LCDC_E0_MSEL(x)        (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_LCDC_E0_MSEL(x)    ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_LCDC_E0_LOOP_MODE  ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
#define CTRL_MASK_GRP_LCDC_E0_PWR_LPS   ((u32)0x00000007 << 3)            /*!< R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_LCDC_E0_PWR_LPS(x)     (((u32)((x) & 0x00000007) << 3))
#define CTRL_GET_GRP_LCDC_E0_PWR_LPS(x) ((u32)(((x >> 3) & 0x00000007)))
#define CTRL_MASK_GRP_LCDC_E0_PWR_ACT   ((u32)0x00000007 << 0)            /*!< R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_LCDC_E0_PWR_ACT(x)     (((u32)((x) & 0x00000007) << 0))
#define CTRL_GET_GRP_LCDC_E0_PWR_ACT(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_CTRL_GRP_MJPEG_E0_CTRL
  * @brief
  * @{
  */
#define CTRL_BIT_MJPEG_E0_TEST1          ((u32)0x00000001 << 15)           /*!< R/W 1'b0  TEST1*/
#define CTRL_MASK_MJPEG_E0_RM            ((u32)0x0000000F << 11)           /*!< R/W 4'b0010  RM*/
#define CTRL_MJPEG_E0_RM(x)              (((u32)((x) & 0x0000000F) << 11))
#define CTRL_GET_MJPEG_E0_RM(x)          ((u32)(((x >> 11) & 0x0000000F)))
#define CTRL_BIT_MJPEG_E0_RME            ((u32)0x00000001 << 10)           /*!< R/W 1'b0  RME*/
#define CTRL_MASK_GRP_MJPEG_E0_MSEL      ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_MJPEG_E0_MSEL(x)        (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_MJPEG_E0_MSEL(x)    ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_MJPEG_E0_LOOP_MODE  ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
#define CTRL_MASK_GRP_MJPEG_E0_PWR_LPS   ((u32)0x00000007 << 3)            /*!< R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_MJPEG_E0_PWR_LPS(x)     (((u32)((x) & 0x00000007) << 3))
#define CTRL_GET_GRP_MJPEG_E0_PWR_LPS(x) ((u32)(((x >> 3) & 0x00000007)))
#define CTRL_MASK_GRP_MJPEG_E0_PWR_ACT   ((u32)0x00000007 << 0)            /*!< R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_MJPEG_E0_PWR_ACT(x)     (((u32)((x) & 0x00000007) << 0))
#define CTRL_GET_GRP_MJPEG_E0_PWR_ACT(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_CTRL_GRP_MJPEG_E1_CTRL
  * @brief
  * @{
  */
#define CTRL_BIT_MJPEG_E1_TEST1          ((u32)0x00000001 << 15)           /*!< R/W 1'b0  TEST1*/
#define CTRL_MASK_MJPEG_E1_RM            ((u32)0x0000000F << 11)           /*!< R/W 4'b0010  RM*/
#define CTRL_MJPEG_E1_RM(x)              (((u32)((x) & 0x0000000F) << 11))
#define CTRL_GET_MJPEG_E1_RM(x)          ((u32)(((x >> 11) & 0x0000000F)))
#define CTRL_BIT_MJPEG_E1_RME            ((u32)0x00000001 << 10)           /*!< R/W 1'b0  RME*/
#define CTRL_MASK_GRP_MJPEG_E1_MSEL      ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_MJPEG_E1_MSEL(x)        (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_MJPEG_E1_MSEL(x)    ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_MJPEG_E1_LOOP_MODE  ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
#define CTRL_MASK_GRP_MJPEG_E1_PWR_LPS   ((u32)0x00000007 << 3)            /*!< R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_MJPEG_E1_PWR_LPS(x)     (((u32)((x) & 0x00000007) << 3))
#define CTRL_GET_GRP_MJPEG_E1_PWR_LPS(x) ((u32)(((x >> 3) & 0x00000007)))
#define CTRL_MASK_GRP_MJPEG_E1_PWR_ACT   ((u32)0x00000007 << 0)            /*!< R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_MJPEG_E1_PWR_ACT(x)     (((u32)((x) & 0x00000007) << 0))
#define CTRL_GET_GRP_MJPEG_E1_PWR_ACT(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_CTRL_GRP_MJPEG_E2_CTRL
  * @brief
  * @{
  */
#define CTRL_BIT_MJPEG_E2_TEST1          ((u32)0x00000001 << 15)           /*!< R/W 1'b0  TEST1*/
#define CTRL_MASK_MJPEG_E2_RM            ((u32)0x0000000F << 11)           /*!< R/W 4'b0010  RM*/
#define CTRL_MJPEG_E2_RM(x)              (((u32)((x) & 0x0000000F) << 11))
#define CTRL_GET_MJPEG_E2_RM(x)          ((u32)(((x >> 11) & 0x0000000F)))
#define CTRL_BIT_MJPEG_E2_RME            ((u32)0x00000001 << 10)           /*!< R/W 1'b0  RME*/
#define CTRL_MASK_GRP_MJPEG_E2_MSEL      ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_MJPEG_E2_MSEL(x)        (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_MJPEG_E2_MSEL(x)    ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_MJPEG_E2_LOOP_MODE  ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
#define CTRL_MASK_GRP_MJPEG_E2_PWR_LPS   ((u32)0x00000007 << 3)            /*!< R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_MJPEG_E2_PWR_LPS(x)     (((u32)((x) & 0x00000007) << 3))
#define CTRL_GET_GRP_MJPEG_E2_PWR_LPS(x) ((u32)(((x >> 3) & 0x00000007)))
#define CTRL_MASK_GRP_MJPEG_E2_PWR_ACT   ((u32)0x00000007 << 0)            /*!< R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_MJPEG_E2_PWR_ACT(x)     (((u32)((x) & 0x00000007) << 0))
#define CTRL_GET_GRP_MJPEG_E2_PWR_ACT(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_CTRL_GRP_GMAC_E0_CTRL
  * @brief
  * @{
  */
#define CTRL_BIT_GMAC_E0_TEST1B         ((u32)0x00000001 << 21)           /*!< R/W 1'b0  TEST1B*/
#define CTRL_BIT_GMAC_E0_TEST1A         ((u32)0x00000001 << 20)           /*!< R/W 1'b0  TEST1A*/
#define CTRL_MASK_GMAC_E0_RMB           ((u32)0x0000000F << 16)           /*!< R/W 4'b0010  RMB*/
#define CTRL_GMAC_E0_RMB(x)             (((u32)((x) & 0x0000000F) << 16))
#define CTRL_GET_GMAC_E0_RMB(x)         ((u32)(((x >> 16) & 0x0000000F)))
#define CTRL_MASK_GMAC_E0_RMA           ((u32)0x0000000F << 12)           /*!< R/W 4'b0010  RMA*/
#define CTRL_GMAC_E0_RMA(x)             (((u32)((x) & 0x0000000F) << 12))
#define CTRL_GET_GMAC_E0_RMA(x)         ((u32)(((x >> 12) & 0x0000000F)))
#define CTRL_BIT_GMAC_E0_RMEB           ((u32)0x00000001 << 11)           /*!< R/W 1'b0  RMEB*/
#define CTRL_BIT_GMAC_E0_RMEA           ((u32)0x00000001 << 10)           /*!< R/W 1'b0  RMEA*/
#define CTRL_MASK_GRP_GMAC_E0_MSEL      ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_GMAC_E0_MSEL(x)        (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_GMAC_E0_MSEL(x)    ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_GMAC_E0_LOOP_MODE  ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
#define CTRL_MASK_GRP_GMAC_E0_PWR_LPS   ((u32)0x00000007 << 3)            /*!< R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_GMAC_E0_PWR_LPS(x)     (((u32)((x) & 0x00000007) << 3))
#define CTRL_GET_GRP_GMAC_E0_PWR_LPS(x) ((u32)(((x >> 3) & 0x00000007)))
#define CTRL_MASK_GRP_GMAC_E0_PWR_ACT   ((u32)0x00000007 << 0)            /*!< R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_GMAC_E0_PWR_ACT(x)     (((u32)((x) & 0x00000007) << 0))
#define CTRL_GET_GRP_GMAC_E0_PWR_ACT(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_CTRL_GRP_CAN_E0_CTRL
  * @brief
  * @{
  */
#define CTRL_BIT_CAN_E0_TEST1          ((u32)0x00000001 << 15)           /*!< R/W 1'b0  TEST1*/
#define CTRL_MASK_CAN_E0_RM            ((u32)0x0000000F << 11)           /*!< R/W 4'b0010  RM*/
#define CTRL_CAN_E0_RM(x)              (((u32)((x) & 0x0000000F) << 11))
#define CTRL_GET_CAN_E0_RM(x)          ((u32)(((x >> 11) & 0x0000000F)))
#define CTRL_BIT_CAN_E0_RME            ((u32)0x00000001 << 10)           /*!< R/W 1'b0  RME*/
#define CTRL_MASK_GRP_CAN_E0_MSEL      ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_CAN_E0_MSEL(x)        (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_CAN_E0_MSEL(x)    ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_CAN_E0_LOOP_MODE  ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
#define CTRL_MASK_GRP_CAN_E0_PWR_LPS   ((u32)0x00000007 << 3)            /*!< R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_CAN_E0_PWR_LPS(x)     (((u32)((x) & 0x00000007) << 3))
#define CTRL_GET_GRP_CAN_E0_PWR_LPS(x) ((u32)(((x >> 3) & 0x00000007)))
#define CTRL_MASK_GRP_CAN_E0_PWR_ACT   ((u32)0x00000007 << 0)            /*!< R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_CAN_E0_PWR_ACT(x)     (((u32)((x) & 0x00000007) << 0))
#define CTRL_GET_GRP_CAN_E0_PWR_ACT(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_CTRL_GRP_PKE_E0_CTRL
  * @brief
  * @{
  */
#define CTRL_BIT_PKE_E0_TEST1B         ((u32)0x00000001 << 21)           /*!< R/W 1'b0  TEST1B*/
#define CTRL_BIT_PKE_E0_TEST1A         ((u32)0x00000001 << 20)           /*!< R/W 1'b0  TEST1A*/
#define CTRL_MASK_PKE_E0_RMB           ((u32)0x0000000F << 16)           /*!< R/W 4'b0010  RMB*/
#define CTRL_PKE_E0_RMB(x)             (((u32)((x) & 0x0000000F) << 16))
#define CTRL_GET_PKE_E0_RMB(x)         ((u32)(((x >> 16) & 0x0000000F)))
#define CTRL_MASK_PKE_E0_RMA           ((u32)0x0000000F << 12)           /*!< R/W 4'b0010  RMA*/
#define CTRL_PKE_E0_RMA(x)             (((u32)((x) & 0x0000000F) << 12))
#define CTRL_GET_PKE_E0_RMA(x)         ((u32)(((x >> 12) & 0x0000000F)))
#define CTRL_BIT_PKE_E0_RMEB           ((u32)0x00000001 << 11)           /*!< R/W 1'b0  RMEB*/
#define CTRL_BIT_PKE_E0_RMEA           ((u32)0x00000001 << 10)           /*!< R/W 1'b0  RMEA*/
#define CTRL_MASK_GRP_PKE_E0_MSEL      ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_PKE_E0_MSEL(x)        (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_PKE_E0_MSEL(x)    ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_PKE_E0_LOOP_MODE  ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
#define CTRL_MASK_GRP_PKE_E0_PWR_LPS   ((u32)0x00000007 << 3)            /*!< R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_PKE_E0_PWR_LPS(x)     (((u32)((x) & 0x00000007) << 3))
#define CTRL_GET_GRP_PKE_E0_PWR_LPS(x) ((u32)(((x >> 3) & 0x00000007)))
#define CTRL_MASK_GRP_PKE_E0_PWR_ACT   ((u32)0x00000007 << 0)            /*!< R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_PKE_E0_PWR_ACT(x)     (((u32)((x) & 0x00000007) << 0))
#define CTRL_GET_GRP_PKE_E0_PWR_ACT(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_CTRL_GRP_PKE_E1_CTRL
  * @brief
  * @{
  */
#define CTRL_MASK_PKE_E1_DVS           ((u32)0x0000000F << 11)           /*!< R/W 4'b0101  DVS*/
#define CTRL_PKE_E1_DVS(x)             (((u32)((x) & 0x0000000F) << 11))
#define CTRL_GET_PKE_E1_DVS(x)         ((u32)(((x >> 11) & 0x0000000F)))
#define CTRL_BIT_PKE_E1_DVSE           ((u32)0x00000001 << 10)           /*!< R/W 1'b0  DVSE*/
#define CTRL_MASK_GRP_PKE_E1_MSEL      ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_PKE_E1_MSEL(x)        (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_PKE_E1_MSEL(x)    ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_PKE_E1_LOOP_MODE  ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
#define CTRL_MASK_GRP_PKE_E1_PWR_LPS   ((u32)0x00000007 << 3)            /*!< R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_PKE_E1_PWR_LPS(x)     (((u32)((x) & 0x00000007) << 3))
#define CTRL_GET_GRP_PKE_E1_PWR_LPS(x) ((u32)(((x >> 3) & 0x00000007)))
#define CTRL_MASK_GRP_PKE_E1_PWR_ACT   ((u32)0x00000007 << 0)            /*!< R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_PKE_E1_PWR_ACT(x)     (((u32)((x) & 0x00000007) << 0))
#define CTRL_GET_GRP_PKE_E1_PWR_ACT(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_CTRL_GRP_PPE_E0_CTRL
  * @brief
  * @{
  */
#define CTRL_BIT_PPE_E0_TEST1          ((u32)0x00000001 << 15)           /*!< R/W 1'b0  TEST1*/
#define CTRL_MASK_PPE_E0_RM            ((u32)0x0000000F << 11)           /*!< R/W 4'b0010  RM*/
#define CTRL_PPE_E0_RM(x)              (((u32)((x) & 0x0000000F) << 11))
#define CTRL_GET_PPE_E0_RM(x)          ((u32)(((x >> 11) & 0x0000000F)))
#define CTRL_BIT_PPE_E0_RME            ((u32)0x00000001 << 10)           /*!< R/W 1'b0  RME*/
#define CTRL_MASK_GRP_PPE_E0_MSEL      ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_PPE_E0_MSEL(x)        (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_PPE_E0_MSEL(x)    ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_PPE_E0_LOOP_MODE  ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
#define CTRL_MASK_GRP_PPE_E0_PWR_LPS   ((u32)0x00000007 << 3)            /*!< R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_PPE_E0_PWR_LPS(x)     (((u32)((x) & 0x00000007) << 3))
#define CTRL_GET_GRP_PPE_E0_PWR_LPS(x) ((u32)(((x >> 3) & 0x00000007)))
#define CTRL_MASK_GRP_PPE_E0_PWR_ACT   ((u32)0x00000007 << 0)            /*!< R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_PPE_E0_PWR_ACT(x)     (((u32)((x) & 0x00000007) << 0))
#define CTRL_GET_GRP_PPE_E0_PWR_ACT(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_CTRL_GRP_PPE_E1_CTRL
  * @brief
  * @{
  */
#define CTRL_BIT_PPE_E1_TEST1          ((u32)0x00000001 << 15)           /*!< R/W 1'b0  TEST1*/
#define CTRL_MASK_PPE_E1_RM            ((u32)0x0000000F << 11)           /*!< R/W 4'b0010  RM*/
#define CTRL_PPE_E1_RM(x)              (((u32)((x) & 0x0000000F) << 11))
#define CTRL_GET_PPE_E1_RM(x)          ((u32)(((x >> 11) & 0x0000000F)))
#define CTRL_BIT_PPE_E1_RME            ((u32)0x00000001 << 10)           /*!< R/W 1'b0  RME*/
#define CTRL_MASK_GRP_PPE_E1_MSEL      ((u32)0x00000003 << 8)            /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_PPE_E1_MSEL(x)        (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_PPE_E1_MSEL(x)    ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_PPE_E1_LOOP_MODE  ((u32)0x00000001 << 7)            /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
#define CTRL_MASK_GRP_PPE_E1_PWR_LPS   ((u32)0x00000007 << 3)            /*!< R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_PPE_E1_PWR_LPS(x)     (((u32)((x) & 0x00000007) << 3))
#define CTRL_GET_GRP_PPE_E1_PWR_LPS(x) ((u32)(((x >> 3) & 0x00000007)))
#define CTRL_MASK_GRP_PPE_E1_PWR_ACT   ((u32)0x00000007 << 0)            /*!< R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down*/
#define CTRL_GRP_PPE_E1_PWR_ACT(x)     (((u32)((x) & 0x00000007) << 0))
#define CTRL_GET_GRP_PPE_E1_PWR_ACT(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_CTRL_GRP_BT_VIR_E0_CTRL
  * @brief
  * @{
  */
#define CTRL_MASK_GRP_BT_VIR_E0_MSEL     ((u32)0x00000003 << 8)           /*!< R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist*/
#define CTRL_GRP_BT_VIR_E0_MSEL(x)       (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_BT_VIR_E0_MSEL(x)   ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_BIT_GRP_BT_VIR_E0_LOOP_MODE ((u32)0x00000001 << 7)           /*!< R/W 1'b0  *0 - Assist loop mode; *1 - Don't assist loop mode*/
#define CTRL_BIT_GRP_BT_VIR_E0_RSTN      ((u32)0x00000001 << 6)           /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
/** @} */

/** @defgroup REG_CTRL_GRP_RSTN
  * @brief
  * @{
  */
#define CTRL_BIT_GRP_PPE_E1_RSTN         ((u32)0x00000001 << 30) /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_PPE_E0_RSTN         ((u32)0x00000001 << 29) /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_PKE_E1_RSTN         ((u32)0x00000001 << 28) /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_PKE_E0_RSTN         ((u32)0x00000001 << 27) /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_CAN_E0_RSTN         ((u32)0x00000001 << 26) /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_GMAC_E0_RSTN        ((u32)0x00000001 << 25) /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_MJPEG_E2_RSTN       ((u32)0x00000001 << 24) /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_MJPEG_E1_RSTN       ((u32)0x00000001 << 23) /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_MJPEG_E0_RSTN       ((u32)0x00000001 << 22) /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_LCDC_E0_RSTN        ((u32)0x00000001 << 21) /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_KM4NS_CACHE_E0_RSTN ((u32)0x00000001 << 20) /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_KM4TZ_ITCM0_E0_RSTN ((u32)0x00000001 << 19) /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_KM4TZ_CACHE_E0_RSTN ((u32)0x00000001 << 18) /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_BT_E1_RSTN          ((u32)0x00000001 << 17) /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_BT_E0_RSTN          ((u32)0x00000001 << 16) /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_WPON_E0_RSTN        ((u32)0x00000001 << 15) /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_WPOFF_E5_RSTN       ((u32)0x00000001 << 14) /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_WPOFF_E4_RSTN       ((u32)0x00000001 << 13) /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_WPOFF_E3_RSTN       ((u32)0x00000001 << 12) /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_WPOFF_E2_RSTN       ((u32)0x00000001 << 11) /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_WPOFF_E1_RSTN       ((u32)0x00000001 << 10) /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_WPOFF_E0_RSTN       ((u32)0x00000001 << 9)  /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_WLK4_E0_RSTN        ((u32)0x00000001 << 8)  /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_USB_E0_RSTN         ((u32)0x00000001 << 7)  /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_SDIO_E1_RSTN        ((u32)0x00000001 << 6)  /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_SDIO_E0_RSTN        ((u32)0x00000001 << 5)  /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_AONPMC_E0_RSTN      ((u32)0x00000001 << 4)  /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_LSPMC_E0_RSTN       ((u32)0x00000001 << 3)  /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_LRET_E0_RSTN        ((u32)0x00000001 << 2)  /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_HSYS_E1_RSTN        ((u32)0x00000001 << 1)  /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
#define CTRL_BIT_GRP_HSYS_E0_RSTN        ((u32)0x00000001 << 0)  /*!< R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group.*/
/** @} */

/** @defgroup REG_CTRL_BT_FTC_MEM_CTRL
  * @brief
  * @{
  */
#define CTRL_BIT_BT_DS_ALL       ((u32)0x00000001 << 23)           /*!< R/W 1'b0  1 - All BT memory switch to Deep Sleep mode 0 - Power mode of BT memory are controlled by BTPMC*/
#define CTRL_BIT_BT_LVDRF_CLKDIS ((u32)0x00000001 << 22)           /*!< R/W 0x0  */
#define CTRL_BIT_BT_TMCK_ALL     ((u32)0x00000001 << 20)           /*!< R/W 1'b0  */
#define CTRL_BIT_BT_ROM_DVSE     ((u32)0x00000001 << 19)           /*!< R/W 1'b0  */
#define CTRL_MASK_BT_ROM_DVS     ((u32)0x0000000F << 15)           /*!< R/W 4'b0101  FSNOU_A_SPNN*/
#define CTRL_BT_ROM_DVS(x)       (((u32)((x) & 0x0000000F) << 15))
#define CTRL_GET_BT_ROM_DVS(x)   ((u32)(((x >> 15) & 0x0000000F)))
#define CTRL_BIT_BT_1PSR_DVSE    ((u32)0x00000001 << 14)           /*!< R/W 1'b0  Not used in Lite*/
#define CTRL_MASK_BT_1PSR_DVS    ((u32)0x0000000F << 10)           /*!< R/W 4'b1011  Not used in Lite*/
#define CTRL_BT_1PSR_DVS(x)      (((u32)((x) & 0x0000000F) << 10))
#define CTRL_GET_BT_1PSR_DVS(x)  ((u32)(((x >> 10) & 0x0000000F)))
#define CTRL_BIT_BT_1PRF1_DVSE   ((u32)0x00000001 << 9)            /*!< R/W 1'b0  */
#define CTRL_MASK_BT_1PRF1_DVS   ((u32)0x0000000F << 5)            /*!< R/W 4'b1100  FSNOU_A_SYNL*/
#define CTRL_BT_1PRF1_DVS(x)     (((u32)((x) & 0x0000000F) << 5))
#define CTRL_GET_BT_1PRF1_DVS(x) ((u32)(((x >> 5) & 0x0000000F)))
#define CTRL_BIT_BT_1PRF0_DVSE   ((u32)0x00000001 << 4)            /*!< R/W 1'b0  */
#define CTRL_MASK_BT_1PRF0_DVS   ((u32)0x0000000F << 0)            /*!< R/W 4'b1100  FSNOU_A_SYNL*/
#define CTRL_BT_1PRF0_DVS(x)     (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_BT_1PRF0_DVS(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_SCAN_OCC_CTRL0
  * @brief
  * @{
  */
#define CTRL_MASK_SCAN_OCC_DBG_EN_MAC     ((u32)0x00000003 << 14)           /*!< R/W 0x0  scan occ dbg en mac*/
#define CTRL_SCAN_OCC_DBG_EN_MAC(x)       (((u32)((x) & 0x00000003) << 14))
#define CTRL_GET_SCAN_OCC_DBG_EN_MAC(x)   ((u32)(((x >> 14) & 0x00000003)))
#define CTRL_BIT_SCAN_PLL_CTRL_MPHY       ((u32)0x00000001 << 13)           /*!< R/W 0x0  scan pll ctrl mipiphy*/
#define CTRL_BIT_SCAN_PLL_CTRL_UPHY       ((u32)0x00000001 << 12)           /*!< R/W 0x0  scan pll ctrl uphy*/
#define CTRL_MASK_SCAN_OCC_DBG_EN_WPHY    ((u32)0x00000003 << 10)           /*!< R/W 0x0  scan occ dbg en wphy*/
#define CTRL_SCAN_OCC_DBG_EN_WPHY(x)      (((u32)((x) & 0x00000003) << 10))
#define CTRL_GET_SCAN_OCC_DBG_EN_WPHY(x)  ((u32)(((x >> 10) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_SEC     ((u32)0x00000003 << 8)            /*!< R/W 0x0  scan occ dbg en sec*/
#define CTRL_SCAN_OCC_DBG_EN_SEC(x)       (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_SCAN_OCC_DBG_EN_SEC(x)   ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_PSRAM   ((u32)0x00000003 << 6)            /*!< R/W 0x0  scan occ dbg en psram*/
#define CTRL_SCAN_OCC_DBG_EN_PSRAM(x)     (((u32)((x) & 0x00000003) << 6))
#define CTRL_GET_SCAN_OCC_DBG_EN_PSRAM(x) ((u32)(((x >> 6) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_SPIC    ((u32)0x00000003 << 4)            /*!< R/W 0x0  scan occ dbg en spic*/
#define CTRL_SCAN_OCC_DBG_EN_SPIC(x)      (((u32)((x) & 0x00000003) << 4))
#define CTRL_GET_SCAN_OCC_DBG_EN_SPIC(x)  ((u32)(((x >> 4) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_UPHY    ((u32)0x00000003 << 2)            /*!< R/W 0x0  scan occ dbg en uphy*/
#define CTRL_SCAN_OCC_DBG_EN_UPHY(x)      (((u32)((x) & 0x00000003) << 2))
#define CTRL_GET_SCAN_OCC_DBG_EN_UPHY(x)  ((u32)(((x >> 2) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_HP      ((u32)0x00000003 << 0)            /*!< R/W 0x0  scan occ dbg en hp*/
#define CTRL_SCAN_OCC_DBG_EN_HP(x)        (((u32)((x) & 0x00000003) << 0))
#define CTRL_GET_SCAN_OCC_DBG_EN_HP(x)    ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup REG_CTRL_SCAN_CTRL0
  * @brief
  * @{
  */
#define CTRL_MASK_SCAN_PLL_OUT_SEL   ((u32)0x0000003F << 9)           /*!< R/W 0x0  scan pll out sel*/
#define CTRL_SCAN_PLL_OUT_SEL(x)     (((u32)((x) & 0x0000003F) << 9))
#define CTRL_GET_SCAN_PLL_OUT_SEL(x) ((u32)(((x >> 9) & 0x0000003F)))
#define CTRL_BIT_SCAN_RET_EN         ((u32)0x00000001 << 8)           /*!< R/W 0x0  scan retention test enable*/
#define CTRL_BIT_SCAN_FT1            ((u32)0x00000001 << 7)           /*!< R/W 0x0  scan ft mode1*/
#define CTRL_BIT_SCAN_FT0            ((u32)0x00000001 << 6)           /*!< R/W 0x0  scan ft mode0*/
#define CTRL_BIT_SCAN_CP             ((u32)0x00000001 << 5)           /*!< R/W 0x0  scan cp mode*/
#define CTRL_BIT_SCAN_SPC_DISABLE    ((u32)0x00000001 << 4)           /*!< R/W 0x0  scan spc disable*/
#define CTRL_BIT_SCAN_PLL_BYPASS     ((u32)0x00000001 << 3)           /*!< R/W 0x0  scan pll bypass*/
#define CTRL_BIT_SCAN_COMPRESS       ((u32)0x00000001 << 2)           /*!< R/W 0x0  scan compress*/
#define CTRL_BIT_SCAN_PAR_LATPS      ((u32)0x00000001 << 1)           /*!< R/W 0x0  Used as scan latch enable bit . If want to change scan par from func setting , set this bit to 1 first , then write func register will also effect scan shadow register .*/
#define CTRL_BIT_SCAN_MODE_SETPS     ((u32)0x00000001 << 0)           /*!< R/W 0x0  scan mode enable . It is a level signal , write 1 will trigger into scan mode*/
/** @} */

/** @defgroup REG_CTRL_SCAN_OCC_CTRL1
  * @brief
  * @{
  */
#define CTRL_MASK_SCAN_OCC_DBG_EN_SDIO     ((u32)0x00000003 << 28)           /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_SDIO(x)       (((u32)((x) & 0x00000003) << 28))
#define CTRL_GET_SCAN_OCC_DBG_EN_SDIO(x)   ((u32)(((x >> 28) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_AC       ((u32)0x00000003 << 26)           /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_AC(x)         (((u32)((x) & 0x00000003) << 26))
#define CTRL_GET_SCAN_OCC_DBG_EN_AC(x)     ((u32)(((x >> 26) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_CAN      ((u32)0x00000003 << 20)           /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_CAN(x)        (((u32)((x) & 0x00000003) << 20))
#define CTRL_GET_SCAN_OCC_DBG_EN_CAN(x)    ((u32)(((x >> 20) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_VO       ((u32)0x00000003 << 16)           /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_VO(x)         (((u32)((x) & 0x00000003) << 16))
#define CTRL_GET_SCAN_OCC_DBG_EN_VO(x)     ((u32)(((x >> 16) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_SHPERI   ((u32)0x00000003 << 14)           /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_SHPERI(x)     (((u32)((x) & 0x00000003) << 14))
#define CTRL_GET_SCAN_OCC_DBG_EN_SHPERI(x) ((u32)(((x >> 14) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_SPORT    ((u32)0x00000003 << 12)           /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_SPORT(x)      (((u32)((x) & 0x00000003) << 12))
#define CTRL_GET_SCAN_OCC_DBG_EN_SPORT(x)  ((u32)(((x >> 12) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_LPERI    ((u32)0x00000003 << 4)            /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_LPERI(x)      (((u32)((x) & 0x00000003) << 4))
#define CTRL_GET_SCAN_OCC_DBG_EN_LPERI(x)  ((u32)(((x >> 4) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_IRDA     ((u32)0x00000003 << 2)            /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_IRDA(x)       (((u32)((x) & 0x00000003) << 2))
#define CTRL_GET_SCAN_OCC_DBG_EN_IRDA(x)   ((u32)(((x >> 2) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_HPERI    ((u32)0x00000003 << 0)            /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_HPERI(x)      (((u32)((x) & 0x00000003) << 0))
#define CTRL_GET_SCAN_OCC_DBG_EN_HPERI(x)  ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup REG_CTRL_SCAN_OCC_CTRL2
  * @brief
  * @{
  */
#define CTRL_MASK_SCAN_OCC_DBG_EN_AON4M     ((u32)0x00000003 << 26)           /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_AON4M(x)       (((u32)((x) & 0x00000003) << 26))
#define CTRL_GET_SCAN_OCC_DBG_EN_AON4M(x)   ((u32)(((x >> 26) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_WL        ((u32)0x00000003 << 24)           /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_WL(x)          (((u32)((x) & 0x00000003) << 24))
#define CTRL_GET_SCAN_OCC_DBG_EN_WL(x)      ((u32)(((x >> 24) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_UART_T    ((u32)0x00000003 << 20)           /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_UART_T(x)      (((u32)((x) & 0x00000003) << 20))
#define CTRL_GET_SCAN_OCC_DBG_EN_UART_T(x)  ((u32)(((x >> 20) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_SIC       ((u32)0x00000003 << 18)           /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_SIC(x)         (((u32)((x) & 0x00000003) << 18))
#define CTRL_GET_SCAN_OCC_DBG_EN_SIC(x)     ((u32)(((x >> 18) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_SDMX      ((u32)0x00000003 << 16)           /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_SDMX(x)        (((u32)((x) & 0x00000003) << 16))
#define CTRL_GET_SCAN_OCC_DBG_EN_SDMX(x)    ((u32)(((x >> 16) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_OTPC      ((u32)0x00000003 << 14)           /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_OTPC(x)        (((u32)((x) & 0x00000003) << 14))
#define CTRL_GET_SCAN_OCC_DBG_EN_OTPC(x)    ((u32)(((x >> 14) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_LPON      ((u32)0x00000003 << 12)           /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_LPON(x)        (((u32)((x) & 0x00000003) << 12))
#define CTRL_GET_SCAN_OCC_DBG_EN_LPON(x)    ((u32)(((x >> 12) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_LOGUART   ((u32)0x00000003 << 10)           /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_LOGUART(x)     (((u32)((x) & 0x00000003) << 10))
#define CTRL_GET_SCAN_OCC_DBG_EN_LOGUART(x) ((u32)(((x >> 10) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_HTIM      ((u32)0x00000003 << 8)            /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_HTIM(x)        (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_SCAN_OCC_DBG_EN_HTIM(x)    ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_GPIO      ((u32)0x00000003 << 6)            /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_GPIO(x)        (((u32)((x) & 0x00000003) << 6))
#define CTRL_GET_SCAN_OCC_DBG_EN_GPIO(x)    ((u32)(((x >> 6) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_BT        ((u32)0x00000003 << 4)            /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_BT(x)          (((u32)((x) & 0x00000003) << 4))
#define CTRL_GET_SCAN_OCC_DBG_EN_BT(x)      ((u32)(((x >> 4) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_AUX40M    ((u32)0x00000003 << 2)            /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_AUX40M(x)      (((u32)((x) & 0x00000003) << 2))
#define CTRL_GET_SCAN_OCC_DBG_EN_AUX40M(x)  ((u32)(((x >> 2) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_ADCC      ((u32)0x00000003 << 0)            /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_ADCC(x)        (((u32)((x) & 0x00000003) << 0))
#define CTRL_GET_SCAN_OCC_DBG_EN_ADCC(x)    ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup REG_CTRL_SCAN_OCC_CTRL3
  * @brief
  * @{
  */
#define CTRL_MASK_SCAN_OCC_DBG_EN_LTIM_3     ((u32)0x00000003 << 14)           /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_LTIM_3(x)       (((u32)((x) & 0x00000003) << 14))
#define CTRL_GET_SCAN_OCC_DBG_EN_LTIM_3(x)   ((u32)(((x >> 14) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_LTIM_2     ((u32)0x00000003 << 12)           /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_LTIM_2(x)       (((u32)((x) & 0x00000003) << 12))
#define CTRL_GET_SCAN_OCC_DBG_EN_LTIM_2(x)   ((u32)(((x >> 12) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_LTIM_1     ((u32)0x00000003 << 10)           /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_LTIM_1(x)       (((u32)((x) & 0x00000003) << 10))
#define CTRL_GET_SCAN_OCC_DBG_EN_LTIM_1(x)   ((u32)(((x >> 10) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_LTIM_0     ((u32)0x00000003 << 8)            /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_LTIM_0(x)       (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_SCAN_OCC_DBG_EN_LTIM_0(x)   ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_UART_R_3   ((u32)0x00000003 << 6)            /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_UART_R_3(x)     (((u32)((x) & 0x00000003) << 6))
#define CTRL_GET_SCAN_OCC_DBG_EN_UART_R_3(x) ((u32)(((x >> 6) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_UART_R_2   ((u32)0x00000003 << 4)            /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_UART_R_2(x)     (((u32)((x) & 0x00000003) << 4))
#define CTRL_GET_SCAN_OCC_DBG_EN_UART_R_2(x) ((u32)(((x >> 4) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_UART_R_1   ((u32)0x00000003 << 2)            /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_UART_R_1(x)     (((u32)((x) & 0x00000003) << 2))
#define CTRL_GET_SCAN_OCC_DBG_EN_UART_R_1(x) ((u32)(((x >> 2) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_UART_R_0   ((u32)0x00000003 << 0)            /*!< R/W 0x0  */
#define CTRL_SCAN_OCC_DBG_EN_UART_R_0(x)     (((u32)((x) & 0x00000003) << 0))
#define CTRL_GET_SCAN_OCC_DBG_EN_UART_R_0(x) ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup REG_CTRL_DUMMY
  * @brief
  * @{
  */
#define CTRL_BIT_PG_RET_SEL  ((u32)0x00000001 << 31) /*!< R/W 0x0  change all pg ram group power mode to ret*/
#define CTRL_BIT_NOPG_LS_SEL ((u32)0x00000001 << 30) /*!< R/W 0x0  change all nopg ram group power mode to ls*/
#define CTRL_BIT_PG_ROM_SEL  ((u32)0x00000001 << 29) /*!< R/W 0x0  change all rom group power mode to slp*/
/** @} */

/*==========MEM_CTRL Register Address Definition==========*/
#define REG_CTRL_MBIST_CTRL_ALL                     0x000
#define REG_CTRL_MBIST_GRP_EN_CTRL0                 0x004
#define REG_CTRL_MBIST_GRP_EN_CTRL1                 0x008
#define REG_CTRL_MBIST_GRP_EN_CTRL2                 0x00C
#define REG_CTRL_MBIST_GRP_EN_BT_VIR_E00            0x010
#define REG_CTRL_MBIST_DONE_STA0                    0x014
#define REG_CTRL_MBIST_START_PULSE0                 0x018
#define REG_CTRL_MBIST_NORM_FAIL0                   0x01C
#define REG_CTRL_MBIST_NORM_FAIL1                   0x020
#define REG_CTRL_MBIST_NORM_FAIL2                   0x024
#define REG_CTRL_MBIST_NORM_FAIL3                   0x028
#define REG_CTRL_MBIST_DRF_FAIL0                    0x02C
#define REG_CTRL_MBIST_DRF_FAIL1                    0x030
#define REG_CTRL_MBIST_DRF_FAIL2                    0x034
#define REG_CTRL_MBIST_DRF_FAIL3                    0x038
#define REG_CTRL_BT_VIR_E0_BIST_DONE0               0x03C
#define REG_CTRL_BT_VIR_E0_BIST_FAIL0               0x040
#define REG_CTRL_BT_VIR_E0_BIST_DRF_PAUSE0          0x044
#define REG_CTRL_BT_VIR_E0_BIST_RPT                 0x048
#define REG_CTRL_BT_VIR_E0_RPT_SEL                  0x04C
#define REG_CTRL_MISR_DOUT_SEL                      0x050
#define REG_CTRL_HSYS_E1_ROM_MISR0                  0x054
#define REG_CTRL_HSYS_E1_ROM_MISR1                  0x058
#define REG_CTRL_LSPMC_E0_ROM_MISR1                 0x05C
#define REG_CTRL_LSPMC_E0_ROM_MISR2                 0x060
#define REG_CTRL_AONPMC_E0_ROM_MISR2                0x064
#define REG_CTRL_AONPMC_E0_ROM_MISR3                0x068
#define REG_CTRL_KM4TZ_ITCM0_E0_ROM_MISR3           0x06C
#define REG_CTRL_KM4TZ_ITCM0_E0_ROM_MISR4           0x070
#define REG_CTRL_PKE_E1_ROM_MISR4                   0x074
#define REG_CTRL_PKE_E1_ROM_MISR5                   0x078
#define REG_CTRL_GRP_PWR_MSK0                       0x07C
#define REG_CTRL_GRP_PWR_MSK1                       0x080
#define REG_CTRL_GRP_PWR_MSK2                       0x084
#define REG_CTRL_GRP_PWR_MSK3                       0x088
#define REG_CTRL_GRP_HSYS_E0_CTRL                   0x08C
#define REG_CTRL_GRP_HSYS_E1_CTRL                   0x090
#define REG_CTRL_GRP_LRET_E0_CTRL                   0x094
#define REG_CTRL_GRP_LSPMC_E0_CTRL                  0x098
#define REG_CTRL_GRP_AONPMC_E0_CTRL                 0x09C
#define REG_CTRL_GRP_SDIO_E0_CTRL                   0x0A0
#define REG_CTRL_GRP_SDIO_E1_CTRL                   0x0A4
#define REG_CTRL_GRP_USB_E0_CTRL                    0x0A8
#define REG_CTRL_GRP_WLK4_E0_CTRL                   0x0AC
#define REG_CTRL_GRP_WPOFF_E0_CTRL                  0x0B0
#define REG_CTRL_GRP_WPOFF_E1_CTRL                  0x0B4
#define REG_CTRL_GRP_WPOFF_E2_CTRL                  0x0B8
#define REG_CTRL_GRP_WPOFF_E3_CTRL                  0x0BC
#define REG_CTRL_GRP_WPOFF_E4_CTRL                  0x0C0
#define REG_CTRL_GRP_WPOFF_E5_CTRL                  0x0C4
#define REG_CTRL_GRP_WPON_E0_CTRL                   0x0C8
#define REG_CTRL_GRP_BT_E0_CTRL                     0x0CC
#define REG_CTRL_GRP_BT_E1_CTRL                     0x0D0
#define REG_CTRL_GRP_KM4TZ_CACHE_E0_CTRL            0x0D4
#define REG_CTRL_GRP_KM4TZ_ITCM0_E0_CTRL            0x0D8
#define REG_CTRL_GRP_KM4NS_CACHE_E0_CTRL            0x0DC
#define REG_CTRL_GRP_LCDC_E0_CTRL                   0x0E0
#define REG_CTRL_GRP_MJPEG_E0_CTRL                  0x0E4
#define REG_CTRL_GRP_MJPEG_E1_CTRL                  0x0E8
#define REG_CTRL_GRP_MJPEG_E2_CTRL                  0x0EC
#define REG_CTRL_GRP_GMAC_E0_CTRL                   0x0F0
#define REG_CTRL_GRP_CAN_E0_CTRL                    0x0F4
#define REG_CTRL_GRP_PKE_E0_CTRL                    0x0F8
#define REG_CTRL_GRP_PKE_E1_CTRL                    0x0FC
#define REG_CTRL_GRP_PPE_E0_CTRL                    0x100
#define REG_CTRL_GRP_PPE_E1_CTRL                    0x104
#define REG_CTRL_GRP_BT_VIR_E0_CTRL                 0x108
#define REG_CTRL_GRP_RSTN                           0x110
#define REG_CTRL_BT_FTC_MEM_CTRL                    0x184
#define REG_CTRL_SCAN_OCC_CTRL0                     0x1C0
#define REG_CTRL_SCAN_CTRL0                         0x1C4
#define REG_CTRL_SCAN_OCC_CTRL1                     0x1F0
#define REG_CTRL_SCAN_OCC_CTRL2                     0x1F4
#define REG_CTRL_SCAN_OCC_CTRL3                     0x1F8
#define REG_CTRL_DUMMY                              0x1FC

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */

/** @defgroup MEM_CTRL_LPS_define
  * @{
  */
#define BIT_MEM_CTRL_LPS_ACTIVE		0x0
#define BIT_MEM_CTRL_LPS_LS			0x1
#define BIT_MEM_CTRL_LPS_DS			0x2
#define BIT_MEM_CTRL_LPS_SD			0x4


/** @defgroup MEM_CTRL_ACT_define
  * @{
  */
#define BIT_MEM_CTRL_ACT_ACTIVE		0x000
#define BIT_MEM_CTRL_ACT_LS			0x001
#define BIT_MEM_CTRL_ACT_DS			0x010
#define BIT_MEM_CTRL_ACT_SD			0x100


//Please add your defination here
#define CTRL_MASK_GRP_x_PWR_LPS			((u32)0x00000007 << 3)          /*!<R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. 000 - active 001 - light sleep 010 - deep sleep 100 - shut down */
#define CTRL_GRP_x_PWR_LPS(x)			((u32)(((x) & 0x00000007) << 3))

/* MANUAL_GEN_END */

#endif

/** @} */

/** @} */