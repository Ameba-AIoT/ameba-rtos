/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SYSREG_MEM_CTRL_H_
#define _SYSREG_MEM_CTRL_H_

/* AUTO_GEN_START */

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup MEM_CTRL
* @brief MEM_CTRL driver modules
* @{
*/

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup MEM_CTRL_Register_Definitions MEM_CTRL Register Definitions
 * @{
 *****************************************************************************/

/** @defgroup REG_CTRL_MBIST_CTRL_ALL
 * @brief MEMx MBIST control and status register
 * @{
 **/
#define CTRL_MASK_MBIST_STA                        ((u32)0x0000001F << 16)          /*!<R 5'b0  Total mbist status [16] MBIST DRF_START_PAUSE [17] MBIST NRML_DONE_ALL [18] MBIST NRML_FAIL_ALL [19] MBIST DRF_DONE_ALL [20] MBIST DRF_FAIL_ALL */
#define CTRL_MBIST_STA(x)                          (((u32)((x) & 0x0000001F) << 16))
#define CTRL_GET_MBIST_STA(x)                      ((u32)(((x >> 16) & 0x0000001F)))
#define CTRL_MASK_MBIST_CTRL                       ((u32)0x000000FF << 0)          /*!<R/W 8'b0  Total mbist control [0] MBIST_CLK_EN_ALL BIS_CLK enable of all system memory groups. [1] MBIST_RSTB_ALL Reset controll mbist circuit of all system memory groups. [3:2]MBIST_MSEL_ALL mbist method selection of all memory groups: 2'b01: enable normal mode mbist 2'b10: enable DRF mode mbist 2'b11: enable normal&DRF mode mbist [4]MBIST_RESUME [5]MBIST_RESUME_PS [6]MBIST_DYN_READ_EN If MBIST_MSEL_ALL[1]=1 & MBIST_DYN_READ_EN=1, run VDDR mode mbist. 7]MBIST_PHYCLK_STOP */
#define CTRL_MBIST_CTRL(x)                         (((u32)((x) & 0x000000FF) << 0))
#define CTRL_GET_MBIST_CTRL(x)                     ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup REG_CTRL_MBIST_GRP_EN_CTRL0
 * @brief Used to on
 * @{
 **/
#define CTRL_MASK_GRP_EN_KM4_ITCM0_E0              ((u32)0x000001FF << 23)          /*!<R/W 0x1ff   */
#define CTRL_GRP_EN_KM4_ITCM0_E0(x)                (((u32)((x) & 0x000001FF) << 23))
#define CTRL_GET_GRP_EN_KM4_ITCM0_E0(x)            ((u32)(((x >> 23) & 0x000001FF)))
#define CTRL_MASK_GRP_EN_KM4_ICACHE_E0             ((u32)0x00000003 << 21)          /*!<R/W 0x3   */
#define CTRL_GRP_EN_KM4_ICACHE_E0(x)               (((u32)((x) & 0x00000003) << 21))
#define CTRL_GET_GRP_EN_KM4_ICACHE_E0(x)           ((u32)(((x >> 21) & 0x00000003)))
#define CTRL_MASK_GRP_EN_KM4_DCACHE_E0             ((u32)0x00000003 << 19)          /*!<R/W 0x3   */
#define CTRL_GRP_EN_KM4_DCACHE_E0(x)               (((u32)((x) & 0x00000003) << 19))
#define CTRL_GET_GRP_EN_KM4_DCACHE_E0(x)           ((u32)(((x >> 19) & 0x00000003)))
#define CTRL_MASK_GRP_EN_WPOFF_E1                  ((u32)0x0000001F << 14)          /*!<R/W 0x1f   */
#define CTRL_GRP_EN_WPOFF_E1(x)                    (((u32)((x) & 0x0000001F) << 14))
#define CTRL_GET_GRP_EN_WPOFF_E1(x)                ((u32)(((x >> 14) & 0x0000001F)))
#define CTRL_MASK_GRP_EN_WLK4_E0                   ((u32)0x00000003 << 12)          /*!<R/W 0x3   */
#define CTRL_GRP_EN_WLK4_E0(x)                     (((u32)((x) & 0x00000003) << 12))
#define CTRL_GET_GRP_EN_WLK4_E0(x)                 ((u32)(((x >> 12) & 0x00000003)))
#define CTRL_MASK_GRP_EN_HPER_E0                   ((u32)0x00000003 << 10)          /*!<R/W 0x3   */
#define CTRL_GRP_EN_HPER_E0(x)                     (((u32)((x) & 0x00000003) << 10))
#define CTRL_GET_GRP_EN_HPER_E0(x)                 ((u32)(((x >> 10) & 0x00000003)))
#define CTRL_MASK_GRP_EN_AONPMC_E0                 ((u32)0x00000003 << 8)          /*!<R/W 0x3   */
#define CTRL_GRP_EN_AONPMC_E0(x)                   (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_GRP_EN_AONPMC_E0(x)               ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_MASK_GRP_EN_HSYS_E0                   ((u32)0x000000FF << 0)          /*!<R/W 0xff   */
#define CTRL_GRP_EN_HSYS_E0(x)                     (((u32)((x) & 0x000000FF) << 0))
#define CTRL_GET_GRP_EN_HSYS_E0(x)                 ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup REG_CTRL_MBIST_GRP_EN_CTRL1
 * @brief Used to on
 * @{
 **/
#define CTRL_MASK_GRP_EN_KM0_ITCM0_E0              ((u32)0x0000001F << 4)          /*!<R/W 0x1f   */
#define CTRL_GRP_EN_KM0_ITCM0_E0(x)                (((u32)((x) & 0x0000001F) << 4))
#define CTRL_GET_GRP_EN_KM0_ITCM0_E0(x)            ((u32)(((x >> 4) & 0x0000001F)))
#define CTRL_MASK_GRP_EN_KM0_ICACHE_E0             ((u32)0x00000003 << 2)          /*!<R/W 0x3   */
#define CTRL_GRP_EN_KM0_ICACHE_E0(x)               (((u32)((x) & 0x00000003) << 2))
#define CTRL_GET_GRP_EN_KM0_ICACHE_E0(x)           ((u32)(((x >> 2) & 0x00000003)))
#define CTRL_MASK_GRP_EN_KM0_DCACHE_E0             ((u32)0x00000003 << 0)          /*!<R/W 0x3   */
#define CTRL_GRP_EN_KM0_DCACHE_E0(x)               (((u32)((x) & 0x00000003) << 0))
#define CTRL_GET_GRP_EN_KM0_DCACHE_E0(x)           ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup REG_CTRL_MBIST_GRP_EN_BT_VIR_E00
 * @brief
 * @{
 **/
#define CTRL_MASK_BT_VIR_E0_BIST_EN_19_0           ((u32)0x000FFFFF << 0)          /*!<R/W 0xfffff   */
#define CTRL_BT_VIR_E0_BIST_EN_19_0(x)             (((u32)((x) & 0x000FFFFF) << 0))
#define CTRL_GET_BT_VIR_E0_BIST_EN_19_0(x)         ((u32)(((x >> 0) & 0x000FFFFF)))
/** @} */

/** @defgroup REG_CTRL_MBIST_DONE_STA0
 * @brief
 * @{
 **/
#define CTRL_MASK_MBIST_DONE_21_0                  ((u32)0x003FFFFF << 0)          /*!<R 22'b0   */
#define CTRL_MBIST_DONE_21_0(x)                    (((u32)((x) & 0x003FFFFF) << 0))
#define CTRL_GET_MBIST_DONE_21_0(x)                ((u32)(((x >> 0) & 0x003FFFFF)))
/** @} */

/** @defgroup REG_CTRL_MBIST_START_PULSE0
 * @brief
 * @{
 **/
#define CTRL_MASK_MBIST_START_PAUSE_21_0           ((u32)0x003FFFFF << 0)          /*!<R 22'b0   */
#define CTRL_MBIST_START_PAUSE_21_0(x)             (((u32)((x) & 0x003FFFFF) << 0))
#define CTRL_GET_MBIST_START_PAUSE_21_0(x)         ((u32)(((x >> 0) & 0x003FFFFF)))
/** @} */

/** @defgroup REG_CTRL_MBIST_NORM_FAIL0
 * @brief
 * @{
 **/
#define CTRL_MASK_MBIST_NORM_FAIL_31_0             ((u32)0xFFFFFFFF << 0)          /*!<R 32'b0   */
#define CTRL_MBIST_NORM_FAIL_31_0(x)               (((u32)((x) & 0xFFFFFFFF) << 0))
#define CTRL_GET_MBIST_NORM_FAIL_31_0(x)           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_CTRL_MBIST_NORM_FAIL1
 * @brief
 * @{
 **/
#define CTRL_MASK_MBIST_NORM_FAIL_52_32            ((u32)0x001FFFFF << 0)          /*!<R 21'b0   */
#define CTRL_MBIST_NORM_FAIL_52_32(x)              (((u32)((x) & 0x001FFFFF) << 0))
#define CTRL_GET_MBIST_NORM_FAIL_52_32(x)          ((u32)(((x >> 0) & 0x001FFFFF)))
/** @} */

/** @defgroup REG_CTRL_MBIST_DRF_FAIL0
 * @brief
 * @{
 **/
#define CTRL_MASK_MBIST_DRF_FAIL_31_0              ((u32)0xFFFFFFFF << 0)          /*!<R 32'b0   */
#define CTRL_MBIST_DRF_FAIL_31_0(x)                (((u32)((x) & 0xFFFFFFFF) << 0))
#define CTRL_GET_MBIST_DRF_FAIL_31_0(x)            ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_CTRL_MBIST_DRF_FAIL1
 * @brief
 * @{
 **/
#define CTRL_MASK_MBIST_DRF_FAIL_52_32             ((u32)0x001FFFFF << 0)          /*!<R 21'b0   */
#define CTRL_MBIST_DRF_FAIL_52_32(x)               (((u32)((x) & 0x001FFFFF) << 0))
#define CTRL_GET_MBIST_DRF_FAIL_52_32(x)           ((u32)(((x >> 0) & 0x001FFFFF)))
/** @} */

/** @defgroup REG_CTRL_BT_VIR_E0_BIST_DONE0
 * @brief
 * @{
 **/
#define CTRL_MASK_BT_VIR_E0_MBIST_DONE_19_0        ((u32)0x000FFFFF << 0)          /*!<R 20'b0   */
#define CTRL_BT_VIR_E0_MBIST_DONE_19_0(x)          (((u32)((x) & 0x000FFFFF) << 0))
#define CTRL_GET_BT_VIR_E0_MBIST_DONE_19_0(x)      ((u32)(((x >> 0) & 0x000FFFFF)))
/** @} */

/** @defgroup REG_CTRL_BT_VIR_E0_BIST_FAIL0
 * @brief
 * @{
 **/
#define CTRL_MASK_BT_VIR_E0_MBIST_FAIL_19_0        ((u32)0x000FFFFF << 0)          /*!<R 20'b0   */
#define CTRL_BT_VIR_E0_MBIST_FAIL_19_0(x)          (((u32)((x) & 0x000FFFFF) << 0))
#define CTRL_GET_BT_VIR_E0_MBIST_FAIL_19_0(x)      ((u32)(((x >> 0) & 0x000FFFFF)))
/** @} */

/** @defgroup REG_CTRL_BT_VIR_E0_BIST_DRF_PAUSE0
 * @brief
 * @{
 **/
#define CTRL_MASK_BT_VIR_E0_MBIST_DRF_PAUSE_19_0   ((u32)0x000FFFFF << 0)          /*!<R 20'b0   */
#define CTRL_BT_VIR_E0_MBIST_DRF_PAUSE_19_0(x)     (((u32)((x) & 0x000FFFFF) << 0))
#define CTRL_GET_BT_VIR_E0_MBIST_DRF_PAUSE_19_0(x) ((u32)(((x >> 0) & 0x000FFFFF)))
/** @} */

/** @defgroup REG_CTRL_BT_VIR_E0_BIST_RPT
 * @brief
 * @{
 **/
#define CTRL_MASK_BT_VIR_E0_BIST_RPT               ((u32)0xFFFFFFFF << 0)          /*!<R 32'b0   */
#define CTRL_BT_VIR_E0_BIST_RPT(x)                 (((u32)((x) & 0xFFFFFFFF) << 0))
#define CTRL_GET_BT_VIR_E0_BIST_RPT(x)             ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_CTRL_BT_VIR_E0_RPT_SEL
 * @brief
 * @{
 **/
#define CTRL_MASK_BT_VIR_E0_RPT_SEL                ((u32)0x0000000F << 0)          /*!<R/W 4'b0  Select which rom's misr_dataout or bist_status of each memory can be read from bt_vir_e0_bist_rpt */
#define CTRL_BT_VIR_E0_RPT_SEL(x)                  (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_BT_VIR_E0_RPT_SEL(x)              ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_MISR_DOUT_SEL
 * @brief
 * @{
 **/
#define CTRL_MASK_KM0_ITCM0_E0_MISR_SEL            ((u32)0x00000007 << 5)          /*!<R/W 3'b0   */
#define CTRL_KM0_ITCM0_E0_MISR_SEL(x)              (((u32)((x) & 0x00000007) << 5))
#define CTRL_GET_KM0_ITCM0_E0_MISR_SEL(x)          ((u32)(((x >> 5) & 0x00000007)))
#define CTRL_MASK_KM4_ITCM0_E0_MISR_SEL            ((u32)0x0000000F << 1)          /*!<R/W 4'b0   */
#define CTRL_KM4_ITCM0_E0_MISR_SEL(x)              (((u32)((x) & 0x0000000F) << 1))
#define CTRL_GET_KM4_ITCM0_E0_MISR_SEL(x)          ((u32)(((x >> 1) & 0x0000000F)))
#define CTRL_BIT_AONPMC_E0_MISR_SEL                ((u32)0x00000001 << 0)          /*!<R/W 1'b0   */
/** @} */

/** @defgroup REG_CTRL_LSPMC_E0_ROM_MISR0
 * @brief
 * @{
 **/
#define CTRL_MASK_LSPMC_E0_MISR_DOUT_31_0          ((u32)0xFFFFFFFF << 0)          /*!<R 32'b0   */
#define CTRL_LSPMC_E0_MISR_DOUT_31_0(x)            (((u32)((x) & 0xFFFFFFFF) << 0))
#define CTRL_GET_LSPMC_E0_MISR_DOUT_31_0(x)        ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_CTRL_LSPMC_E0_ROM_MISR1
 * @brief
 * @{
 **/
#define CTRL_MASK_LSPMC_E0_MISR_DOUT_35_32         ((u32)0x0000000F << 0)          /*!<R 4'b0   */
#define CTRL_LSPMC_E0_MISR_DOUT_35_32(x)           (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_LSPMC_E0_MISR_DOUT_35_32(x)       ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_AONPMC_E0_ROM_MISR1
 * @brief
 * @{
 **/
#define CTRL_MASK_AONPMC_E0_MISR_DOUT_31_0         ((u32)0xFFFFFFFF << 0)          /*!<R 32'b0   */
#define CTRL_AONPMC_E0_MISR_DOUT_31_0(x)           (((u32)((x) & 0xFFFFFFFF) << 0))
#define CTRL_GET_AONPMC_E0_MISR_DOUT_31_0(x)       ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_CTRL_AONPMC_E0_ROM_MISR2
 * @brief
 * @{
 **/
#define CTRL_MASK_AONPMC_E0_MISR_DOUT_35_32        ((u32)0x0000000F << 0)          /*!<R 4'b0   */
#define CTRL_AONPMC_E0_MISR_DOUT_35_32(x)          (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_AONPMC_E0_MISR_DOUT_35_32(x)      ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_KM4_ITCM0_E0_ROM_MISR2
 * @brief
 * @{
 **/
#define CTRL_MASK_KM4_ITCM0_E0_MISR_DOUT_31_0      ((u32)0xFFFFFFFF << 0)          /*!<R 32'b0   */
#define CTRL_KM4_ITCM0_E0_MISR_DOUT_31_0(x)        (((u32)((x) & 0xFFFFFFFF) << 0))
#define CTRL_GET_KM4_ITCM0_E0_MISR_DOUT_31_0(x)    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_CTRL_KM4_ITCM0_E0_ROM_MISR3
 * @brief
 * @{
 **/
#define CTRL_MASK_KM4_ITCM0_E0_MISR_DOUT_63_32     ((u32)0xFFFFFFFF << 0)          /*!<R 32'b0   */
#define CTRL_KM4_ITCM0_E0_MISR_DOUT_63_32(x)       (((u32)((x) & 0xFFFFFFFF) << 0))
#define CTRL_GET_KM4_ITCM0_E0_MISR_DOUT_63_32(x)   ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_CTRL_KM0_ITCM0_E0_ROM_MISR3
 * @brief
 * @{
 **/
#define CTRL_MASK_KM0_ITCM0_E0_MISR_DOUT_31_0      ((u32)0xFFFFFFFF << 0)          /*!<R 32'b0   */
#define CTRL_KM0_ITCM0_E0_MISR_DOUT_31_0(x)        (((u32)((x) & 0xFFFFFFFF) << 0))
#define CTRL_GET_KM0_ITCM0_E0_MISR_DOUT_31_0(x)    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_CTRL_KM0_ITCM0_E0_ROM_MISR4
 * @brief
 * @{
 **/
#define CTRL_MASK_KM0_ITCM0_E0_MISR_DOUT_63_32     ((u32)0xFFFFFFFF << 0)          /*!<R 32'b0   */
#define CTRL_KM0_ITCM0_E0_MISR_DOUT_63_32(x)       (((u32)((x) & 0xFFFFFFFF) << 0))
#define CTRL_GET_KM0_ITCM0_E0_MISR_DOUT_63_32(x)   ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_CTRL_GRP_PWR_MSK0
 * @brief
 * @{
 **/
#define CTRL_MASK_GRP_PMSK_KM4_DCACHE_E0           ((u32)0x00000003 << 30)          /*!<R/W 0x3   */
#define CTRL_GRP_PMSK_KM4_DCACHE_E0(x)             (((u32)((x) & 0x00000003) << 30))
#define CTRL_GET_GRP_PMSK_KM4_DCACHE_E0(x)         ((u32)(((x >> 30) & 0x00000003)))
#define CTRL_BIT_GRP_PMSK_BT_E1                    ((u32)0x00000001 << 29)          /*!<R/W 0x1   */
#define CTRL_BIT_GRP_PMSK_BT_E0                    ((u32)0x00000001 << 28)          /*!<R/W 0x1   */
#define CTRL_BIT_GRP_PMSK_WPON_E0                  ((u32)0x00000001 << 27)          /*!<R/W 0x1   */
#define CTRL_BIT_GRP_PMSK_WPOFF_E5                 ((u32)0x00000001 << 26)          /*!<R/W 0x1   */
#define CTRL_BIT_GRP_PMSK_WPOFF_E4                 ((u32)0x00000001 << 25)          /*!<R/W 0x1   */
#define CTRL_BIT_GRP_PMSK_WPOFF_E3                 ((u32)0x00000001 << 24)          /*!<R/W 0x1   */
#define CTRL_MASK_GRP_PMSK_WPOFF_E2                ((u32)0x00000003 << 22)          /*!<R/W 0x3   */
#define CTRL_GRP_PMSK_WPOFF_E2(x)                  (((u32)((x) & 0x00000003) << 22))
#define CTRL_GET_GRP_PMSK_WPOFF_E2(x)              ((u32)(((x >> 22) & 0x00000003)))
#define CTRL_MASK_GRP_PMSK_WPOFF_E1                ((u32)0x0000001F << 17)          /*!<R/W 0x1f   */
#define CTRL_GRP_PMSK_WPOFF_E1(x)                  (((u32)((x) & 0x0000001F) << 17))
#define CTRL_GET_GRP_PMSK_WPOFF_E1(x)              ((u32)(((x >> 17) & 0x0000001F)))
#define CTRL_BIT_GRP_PMSK_WPOFF_E0                 ((u32)0x00000001 << 16)          /*!<R/W 0x1   */
#define CTRL_MASK_GRP_PMSK_WLK4_E0                 ((u32)0x00000003 << 14)          /*!<R/W 0x3   */
#define CTRL_GRP_PMSK_WLK4_E0(x)                   (((u32)((x) & 0x00000003) << 14))
#define CTRL_GET_GRP_PMSK_WLK4_E0(x)               ((u32)(((x >> 14) & 0x00000003)))
#define CTRL_MASK_GRP_PMSK_HPER_E0                 ((u32)0x00000003 << 12)          /*!<R/W 0x3   */
#define CTRL_GRP_PMSK_HPER_E0(x)                   (((u32)((x) & 0x00000003) << 12))
#define CTRL_GET_GRP_PMSK_HPER_E0(x)               ((u32)(((x >> 12) & 0x00000003)))
#define CTRL_MASK_GRP_PMSK_AONPMC_E0               ((u32)0x00000003 << 10)          /*!<R/W 0x3   */
#define CTRL_GRP_PMSK_AONPMC_E0(x)                 (((u32)((x) & 0x00000003) << 10))
#define CTRL_GET_GRP_PMSK_AONPMC_E0(x)             ((u32)(((x >> 10) & 0x00000003)))
#define CTRL_BIT_GRP_PMSK_LSPMC_E0                 ((u32)0x00000001 << 9)          /*!<R/W 0x1   */
#define CTRL_BIT_GRP_PMSK_LRET_E0                  ((u32)0x00000001 << 8)          /*!<R/W 0x1   */
#define CTRL_MASK_GRP_PMSK_HSYS_E0                 ((u32)0x000000FF << 0)          /*!<R/W 0xff   */
#define CTRL_GRP_PMSK_HSYS_E0(x)                   (((u32)((x) & 0x000000FF) << 0))
#define CTRL_GET_GRP_PMSK_HSYS_E0(x)               ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup REG_CTRL_GRP_PWR_MSK1
 * @brief
 * @{
 **/
#define CTRL_MASK_GRP_PMSK_KM0_ITCM0_E0            ((u32)0x0000001F << 15)          /*!<R/W 0x1f   */
#define CTRL_GRP_PMSK_KM0_ITCM0_E0(x)              (((u32)((x) & 0x0000001F) << 15))
#define CTRL_GET_GRP_PMSK_KM0_ITCM0_E0(x)          ((u32)(((x >> 15) & 0x0000001F)))
#define CTRL_MASK_GRP_PMSK_KM0_ICACHE_E0           ((u32)0x00000003 << 13)          /*!<R/W 0x3   */
#define CTRL_GRP_PMSK_KM0_ICACHE_E0(x)             (((u32)((x) & 0x00000003) << 13))
#define CTRL_GET_GRP_PMSK_KM0_ICACHE_E0(x)         ((u32)(((x >> 13) & 0x00000003)))
#define CTRL_MASK_GRP_PMSK_KM0_DCACHE_E0           ((u32)0x00000003 << 11)          /*!<R/W 0x3   */
#define CTRL_GRP_PMSK_KM0_DCACHE_E0(x)             (((u32)((x) & 0x00000003) << 11))
#define CTRL_GET_GRP_PMSK_KM0_DCACHE_E0(x)         ((u32)(((x >> 11) & 0x00000003)))
#define CTRL_MASK_GRP_PMSK_KM4_ITCM0_E0            ((u32)0x000001FF << 2)          /*!<R/W 0x1ff   */
#define CTRL_GRP_PMSK_KM4_ITCM0_E0(x)              (((u32)((x) & 0x000001FF) << 2))
#define CTRL_GET_GRP_PMSK_KM4_ITCM0_E0(x)          ((u32)(((x >> 2) & 0x000001FF)))
#define CTRL_MASK_GRP_PMSK_KM4_ICACHE_E0           ((u32)0x00000003 << 0)          /*!<R/W 0x3   */
#define CTRL_GRP_PMSK_KM4_ICACHE_E0(x)             (((u32)((x) & 0x00000003) << 0))
#define CTRL_GET_GRP_PMSK_KM4_ICACHE_E0(x)         ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup REG_CTRL_GRP_x_CTRL
 * @brief
 * @{
 **/
#define CTRL_MASK_GRP_x_MSEL                       ((u32)0x00000003 << 16)          /*!<R/W 2'b11  *01 - This memory group can do normal mbist; *10 - This memory group can do DRF mbist; *11 - This memory group can do normal & DRF mbist */
#define CTRL_GRP_x_MSEL(x)                         (((u32)((x) & 0x00000003) << 16))
#define CTRL_GET_GRP_x_MSEL(x)                     ((u32)(((x >> 16) & 0x00000003)))
#define CTRL_BIT_GRP_x_LOOP_MODE                   ((u32)0x00000001 << 15)          /*!<R/W 1'b0  *0 - Assist loop mode; *1 - Don’t assist loop mode */
#define CTRL_BIT_GRP_x_RSTN                        ((u32)0x00000001 << 14)          /*!<R/W 1'b1  *0 - Reset mbist circuit of this memory group; *1 - Release mbist circuit of this memory group. */
/** @} */

/** @defgroup REG_CTRL_GRP_HSYS_E0_CTRL
 * @brief
 * @{
 **/
#define CTRL_MASK_GRP_HSYS_E0_PWR_LPS              ((u32)0x00000007 << 11)          /*!<R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down */
#define CTRL_GRP_HSYS_E0_PWR_LPS(x)                (((u32)((x) & 0x00000007) << 11))
#define CTRL_GET_GRP_HSYS_E0_PWR_LPS(x)            ((u32)(((x >> 11) & 0x00000007)))
#define CTRL_MASK_GRP_HSYS_E0_PWR_ACT              ((u32)0x00000007 << 8)          /*!<R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down */
#define CTRL_GRP_HSYS_E0_PWR_ACT(x)                (((u32)((x) & 0x00000007) << 8))
#define CTRL_GET_GRP_HSYS_E0_PWR_ACT(x)            ((u32)(((x >> 8) & 0x00000007)))
#define CTRL_MASK_GRP_HSYS_E0_RSEL                 ((u32)0x0000000F << 0)          /*!<R/W 4'd7  1psr,FTC_ULL_RW */
#define CTRL_GRP_HSYS_E0_RSEL(x)                   (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_GRP_HSYS_E0_RSEL(x)               ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_GRP_LRET_E0_CTRL
 * @brief
 * @{
 **/
#define CTRL_MASK_GRP_LRET_E0_PWR_LPS              ((u32)0x00000007 << 11)          /*!<R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down */
#define CTRL_GRP_LRET_E0_PWR_LPS(x)                (((u32)((x) & 0x00000007) << 11))
#define CTRL_GET_GRP_LRET_E0_PWR_LPS(x)            ((u32)(((x >> 11) & 0x00000007)))
#define CTRL_MASK_GRP_LRET_E0_PWR_ACT              ((u32)0x00000007 << 8)          /*!<R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down */
#define CTRL_GRP_LRET_E0_PWR_ACT(x)                (((u32)((x) & 0x00000007) << 8))
#define CTRL_GET_GRP_LRET_E0_PWR_ACT(x)            ((u32)(((x >> 8) & 0x00000007)))
#define CTRL_MASK_GRP_LRET_E0_RSEL                 ((u32)0x0000000F << 0)          /*!<R/W 4'd8  1prf,FTC_ULL_RW */
#define CTRL_GRP_LRET_E0_RSEL(x)                   (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_GRP_LRET_E0_RSEL(x)               ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_GRP_LSPMC_E0_CTRL
 * @brief
 * @{
 **/
#define CTRL_MASK_GRP_LSPMC_E0_PWR_LPS             ((u32)0x00000007 << 11)          /*!<R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down */
#define CTRL_GRP_LSPMC_E0_PWR_LPS(x)               (((u32)((x) & 0x00000007) << 11))
#define CTRL_GET_GRP_LSPMC_E0_PWR_LPS(x)           ((u32)(((x >> 11) & 0x00000007)))
#define CTRL_MASK_GRP_LSPMC_E0_PWR_ACT             ((u32)0x00000007 << 8)          /*!<R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down */
#define CTRL_GRP_LSPMC_E0_PWR_ACT(x)               (((u32)((x) & 0x00000007) << 8))
#define CTRL_GET_GRP_LSPMC_E0_PWR_ACT(x)           ((u32)(((x >> 8) & 0x00000007)))
#define CTRL_MASK_GRP_LSPMC_E0_RSEL                ((u32)0x0000000F << 0)          /*!<R/W 4'd9  rom,FTC_ULL */
#define CTRL_GRP_LSPMC_E0_RSEL(x)                  (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_GRP_LSPMC_E0_RSEL(x)              ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_GRP_AONPMC_E0_CTRL
 * @brief
 * @{
 **/
#define CTRL_MASK_GRP_AONPMC_E0_PWR_LPS            ((u32)0x00000007 << 11)          /*!<R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down */
#define CTRL_GRP_AONPMC_E0_PWR_LPS(x)              (((u32)((x) & 0x00000007) << 11))
#define CTRL_GET_GRP_AONPMC_E0_PWR_LPS(x)          ((u32)(((x >> 11) & 0x00000007)))
#define CTRL_MASK_GRP_AONPMC_E0_PWR_ACT            ((u32)0x00000007 << 8)          /*!<R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down */
#define CTRL_GRP_AONPMC_E0_PWR_ACT(x)              (((u32)((x) & 0x00000007) << 8))
#define CTRL_GET_GRP_AONPMC_E0_PWR_ACT(x)          ((u32)(((x >> 8) & 0x00000007)))
#define CTRL_MASK_GRP_AONPMC_E0_RSEL               ((u32)0x0000000F << 0)          /*!<R/W 4'd9  rom,FTC_ULL */
#define CTRL_GRP_AONPMC_E0_RSEL(x)                 (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_GRP_AONPMC_E0_RSEL(x)             ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_GRP_HPER_E0_CTRL
 * @brief
 * @{
 **/
#define CTRL_MASK_GRP_HPER_E0_PWR_LPS              ((u32)0x00000007 << 11)          /*!<R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down */
#define CTRL_GRP_HPER_E0_PWR_LPS(x)                (((u32)((x) & 0x00000007) << 11))
#define CTRL_GET_GRP_HPER_E0_PWR_LPS(x)            ((u32)(((x >> 11) & 0x00000007)))
#define CTRL_MASK_GRP_HPER_E0_PWR_ACT              ((u32)0x00000007 << 8)          /*!<R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down */
#define CTRL_GRP_HPER_E0_PWR_ACT(x)                (((u32)((x) & 0x00000007) << 8))
#define CTRL_GET_GRP_HPER_E0_PWR_ACT(x)            ((u32)(((x >> 8) & 0x00000007)))
#define CTRL_MASK_GRP_HPER_E0_RSEL                 ((u32)0x0000000F << 0)          /*!<R/W 4'd0  1prf,SNPS_ULP */
#define CTRL_GRP_HPER_E0_RSEL(x)                   (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_GRP_HPER_E0_RSEL(x)               ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_GRP_WLK4_E0_CTRL
 * @brief
 * @{
 **/
#define CTRL_MASK_GRP_WLK4_E0_PWR_LPS              ((u32)0x00000007 << 11)          /*!<R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down */
#define CTRL_GRP_WLK4_E0_PWR_LPS(x)                (((u32)((x) & 0x00000007) << 11))
#define CTRL_GET_GRP_WLK4_E0_PWR_LPS(x)            ((u32)(((x >> 11) & 0x00000007)))
#define CTRL_MASK_GRP_WLK4_E0_PWR_ACT              ((u32)0x00000007 << 8)          /*!<R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down */
#define CTRL_GRP_WLK4_E0_PWR_ACT(x)                (((u32)((x) & 0x00000007) << 8))
#define CTRL_GET_GRP_WLK4_E0_PWR_ACT(x)            ((u32)(((x >> 8) & 0x00000007)))
#define CTRL_MASK_GRP_WLK4_E0_RSEL                 ((u32)0x0000000F << 0)          /*!<R/W 4'd3  1prf,FTC_ULP */
#define CTRL_GRP_WLK4_E0_RSEL(x)                   (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_GRP_WLK4_E0_RSEL(x)               ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_GRP_WPOFF_E0_CTRL
 * @brief
 * @{
 **/
#define CTRL_MASK_GRP_WPOFF_E0_RSEL                ((u32)0x0000000F << 0)          /*!<R/W 4'd3  1prf,FTC_ULP */
#define CTRL_GRP_WPOFF_E0_RSEL(x)                  (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_GRP_WPOFF_E0_RSEL(x)              ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_GRP_WPOFF_E1_CTRL
 * @brief
 * @{
 **/
#define CTRL_MASK_GRP_WPOFF_E1_RSEL                ((u32)0x0000000F << 0)          /*!<R/W 4'd0  1prf,SNPS_ULP */
#define CTRL_GRP_WPOFF_E1_RSEL(x)                  (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_GRP_WPOFF_E1_RSEL(x)              ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_GRP_WPOFF_E2_CTRL
 * @brief
 * @{
 **/
#define CTRL_MASK_GRP_WPOFF_E2_RSEL                ((u32)0x0000000F << 0)          /*!<R/W 4'd4  2prf,FTC_ULP */
#define CTRL_GRP_WPOFF_E2_RSEL(x)                  (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_GRP_WPOFF_E2_RSEL(x)              ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_GRP_WPOFF_E3_CTRL
 * @brief
 * @{
 **/
#define CTRL_MASK_GRP_WPOFF_E3_RSEL                ((u32)0x0000000F << 0)          /*!<R/W 4'd0  1prf,SNPS_ULP */
#define CTRL_GRP_WPOFF_E3_RSEL(x)                  (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_GRP_WPOFF_E3_RSEL(x)              ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_GRP_WPOFF_E4_CTRL
 * @brief
 * @{
 **/
#define CTRL_MASK_GRP_WPOFF_E4_RSEL                ((u32)0x0000000F << 0)          /*!<R/W 4'd0  1prf,SNPS_ULP */
#define CTRL_GRP_WPOFF_E4_RSEL(x)                  (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_GRP_WPOFF_E4_RSEL(x)              ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_GRP_WPOFF_E5_CTRL
 * @brief
 * @{
 **/
#define CTRL_MASK_GRP_WPOFF_E5_RSEL                ((u32)0x0000000F << 0)          /*!<R/W 4'd4  2prf,FTC_ULP */
#define CTRL_GRP_WPOFF_E5_RSEL(x)                  (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_GRP_WPOFF_E5_RSEL(x)              ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_GRP_WPON_E0_CTRL
 * @brief
 * @{
 **/
#define CTRL_MASK_GRP_WPON_E0_RSEL                 ((u32)0x0000000F << 0)          /*!<R/W 4'd8  1prf,FTC_ULL_RW */
#define CTRL_GRP_WPON_E0_RSEL(x)                   (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_GRP_WPON_E0_RSEL(x)               ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_GRP_BT_E0_CTRL
 * @brief
 * @{
 **/
#define CTRL_MASK_GRP_BT_E0_PWR_LPS                ((u32)0x00000007 << 11)          /*!<R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down */
#define CTRL_GRP_BT_E0_PWR_LPS(x)                  (((u32)((x) & 0x00000007) << 11))
#define CTRL_GET_GRP_BT_E0_PWR_LPS(x)              ((u32)(((x >> 11) & 0x00000007)))
#define CTRL_MASK_GRP_BT_E0_PWR_ACT                ((u32)0x00000007 << 8)          /*!<R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down */
#define CTRL_GRP_BT_E0_PWR_ACT(x)                  (((u32)((x) & 0x00000007) << 8))
#define CTRL_GET_GRP_BT_E0_PWR_ACT(x)              ((u32)(((x >> 8) & 0x00000007)))
#define CTRL_MASK_GRP_BT_E0_RSEL                   ((u32)0x0000000F << 0)          /*!<R/W 4'd2  1psr,FTC_ULP */
#define CTRL_GRP_BT_E0_RSEL(x)                     (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_GRP_BT_E0_RSEL(x)                 ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_GRP_BT_E1_CTRL
 * @brief
 * @{
 **/
#define CTRL_MASK_GRP_BT_E1_PWR_LPS                ((u32)0x00000007 << 11)          /*!<R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down */
#define CTRL_GRP_BT_E1_PWR_LPS(x)                  (((u32)((x) & 0x00000007) << 11))
#define CTRL_GET_GRP_BT_E1_PWR_LPS(x)              ((u32)(((x >> 11) & 0x00000007)))
#define CTRL_MASK_GRP_BT_E1_PWR_ACT                ((u32)0x00000007 << 8)          /*!<R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down */
#define CTRL_GRP_BT_E1_PWR_ACT(x)                  (((u32)((x) & 0x00000007) << 8))
#define CTRL_GET_GRP_BT_E1_PWR_ACT(x)              ((u32)(((x >> 8) & 0x00000007)))
#define CTRL_MASK_GRP_BT_E1_RSEL                   ((u32)0x0000000F << 0)          /*!<R/W 4'd2  1psr,FTC_ULP */
#define CTRL_GRP_BT_E1_RSEL(x)                     (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_GRP_BT_E1_RSEL(x)                 ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_GRP_KM4_DCACHE_E0_CTRL
 * @brief
 * @{
 **/
#define CTRL_MASK_GRP_KM4_DCACHE_E0_PWR_LPS        ((u32)0x00000007 << 11)          /*!<R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down */
#define CTRL_GRP_KM4_DCACHE_E0_PWR_LPS(x)          (((u32)((x) & 0x00000007) << 11))
#define CTRL_GET_GRP_KM4_DCACHE_E0_PWR_LPS(x)      ((u32)(((x >> 11) & 0x00000007)))
#define CTRL_MASK_GRP_KM4_DCACHE_E0_PWR_ACT        ((u32)0x00000007 << 8)          /*!<R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down */
#define CTRL_GRP_KM4_DCACHE_E0_PWR_ACT(x)          (((u32)((x) & 0x00000007) << 8))
#define CTRL_GET_GRP_KM4_DCACHE_E0_PWR_ACT(x)      ((u32)(((x >> 8) & 0x00000007)))
#define CTRL_MASK_GRP_KM4_DCACHE_E0_RSEL           ((u32)0x0000000F << 0)          /*!<R/W 4'd3  1prf,FTC_ULP */
#define CTRL_GRP_KM4_DCACHE_E0_RSEL(x)             (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_GRP_KM4_DCACHE_E0_RSEL(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_GRP_KM4_ICACHE_E0_CTRL
 * @brief
 * @{
 **/
#define CTRL_MASK_GRP_KM4_ICACHE_E0_PWR_LPS        ((u32)0x00000007 << 11)          /*!<R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down */
#define CTRL_GRP_KM4_ICACHE_E0_PWR_LPS(x)          (((u32)((x) & 0x00000007) << 11))
#define CTRL_GET_GRP_KM4_ICACHE_E0_PWR_LPS(x)      ((u32)(((x >> 11) & 0x00000007)))
#define CTRL_MASK_GRP_KM4_ICACHE_E0_PWR_ACT        ((u32)0x00000007 << 8)          /*!<R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down */
#define CTRL_GRP_KM4_ICACHE_E0_PWR_ACT(x)          (((u32)((x) & 0x00000007) << 8))
#define CTRL_GET_GRP_KM4_ICACHE_E0_PWR_ACT(x)      ((u32)(((x >> 8) & 0x00000007)))
#define CTRL_MASK_GRP_KM4_ICACHE_E0_RSEL           ((u32)0x0000000F << 0)          /*!<R/W 4'd3  1prf,FTC_ULP */
#define CTRL_GRP_KM4_ICACHE_E0_RSEL(x)             (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_GRP_KM4_ICACHE_E0_RSEL(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_GRP_KM4_ITCM0_E0_CTRL
 * @brief
 * @{
 **/
#define CTRL_MASK_GRP_KM4_ITCM0_E0_PWR_LPS         ((u32)0x00000007 << 11)          /*!<R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down */
#define CTRL_GRP_KM4_ITCM0_E0_PWR_LPS(x)           (((u32)((x) & 0x00000007) << 11))
#define CTRL_GET_GRP_KM4_ITCM0_E0_PWR_LPS(x)       ((u32)(((x >> 11) & 0x00000007)))
#define CTRL_MASK_GRP_KM4_ITCM0_E0_PWR_ACT         ((u32)0x00000007 << 8)          /*!<R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down */
#define CTRL_GRP_KM4_ITCM0_E0_PWR_ACT(x)           (((u32)((x) & 0x00000007) << 8))
#define CTRL_GET_GRP_KM4_ITCM0_E0_PWR_ACT(x)       ((u32)(((x >> 8) & 0x00000007)))
#define CTRL_MASK_GRP_KM4_ITCM0_E0_RSEL            ((u32)0x0000000F << 0)          /*!<R/W 4'd5  rom,FTC_ULP */
#define CTRL_GRP_KM4_ITCM0_E0_RSEL(x)              (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_GRP_KM4_ITCM0_E0_RSEL(x)          ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_GRP_KM0_DCACHE_E0_CTRL
 * @brief
 * @{
 **/
#define CTRL_MASK_GRP_KM0_DCACHE_E0_PWR_LPS        ((u32)0x00000007 << 11)          /*!<R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down */
#define CTRL_GRP_KM0_DCACHE_E0_PWR_LPS(x)          (((u32)((x) & 0x00000007) << 11))
#define CTRL_GET_GRP_KM0_DCACHE_E0_PWR_LPS(x)      ((u32)(((x >> 11) & 0x00000007)))
#define CTRL_MASK_GRP_KM0_DCACHE_E0_PWR_ACT        ((u32)0x00000007 << 8)          /*!<R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down */
#define CTRL_GRP_KM0_DCACHE_E0_PWR_ACT(x)          (((u32)((x) & 0x00000007) << 8))
#define CTRL_GET_GRP_KM0_DCACHE_E0_PWR_ACT(x)      ((u32)(((x >> 8) & 0x00000007)))
#define CTRL_MASK_GRP_KM0_DCACHE_E0_RSEL           ((u32)0x0000000F << 0)          /*!<R/W 4'd3  1prf,FTC_ULP */
#define CTRL_GRP_KM0_DCACHE_E0_RSEL(x)             (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_GRP_KM0_DCACHE_E0_RSEL(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_GRP_KM0_ICACHE_E0_CTRL
 * @brief
 * @{
 **/
#define CTRL_MASK_GRP_KM0_ICACHE_E0_PWR_LPS        ((u32)0x00000007 << 11)          /*!<R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down */
#define CTRL_GRP_KM0_ICACHE_E0_PWR_LPS(x)          (((u32)((x) & 0x00000007) << 11))
#define CTRL_GET_GRP_KM0_ICACHE_E0_PWR_LPS(x)      ((u32)(((x >> 11) & 0x00000007)))
#define CTRL_MASK_GRP_KM0_ICACHE_E0_PWR_ACT        ((u32)0x00000007 << 8)          /*!<R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down */
#define CTRL_GRP_KM0_ICACHE_E0_PWR_ACT(x)          (((u32)((x) & 0x00000007) << 8))
#define CTRL_GET_GRP_KM0_ICACHE_E0_PWR_ACT(x)      ((u32)(((x >> 8) & 0x00000007)))
#define CTRL_MASK_GRP_KM0_ICACHE_E0_RSEL           ((u32)0x0000000F << 0)          /*!<R/W 4'd3  1prf,FTC_ULP */
#define CTRL_GRP_KM0_ICACHE_E0_RSEL(x)             (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_GRP_KM0_ICACHE_E0_RSEL(x)         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_GRP_KM0_ITCM0_E0_CTRL
 * @brief
 * @{
 **/
#define CTRL_MASK_GRP_KM0_ITCM0_E0_PWR_LPS         ((u32)0x00000007 << 11)          /*!<R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down */
#define CTRL_GRP_KM0_ITCM0_E0_PWR_LPS(x)           (((u32)((x) & 0x00000007) << 11))
#define CTRL_GET_GRP_KM0_ITCM0_E0_PWR_LPS(x)       ((u32)(((x >> 11) & 0x00000007)))
#define CTRL_MASK_GRP_KM0_ITCM0_E0_PWR_ACT         ((u32)0x00000007 << 8)          /*!<R/W 3'b000  Used to controll power mode of this memory group when chip is ACTIVE. For FTC_ULP mem, pwr_act[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_act[2:0] map to {SD,DS,LS}. *000 - active *001 - light sleep *010 - deep sleep *100 - shut down */
#define CTRL_GRP_KM0_ITCM0_E0_PWR_ACT(x)           (((u32)((x) & 0x00000007) << 8))
#define CTRL_GET_GRP_KM0_ITCM0_E0_PWR_ACT(x)       ((u32)(((x >> 8) & 0x00000007)))
#define CTRL_MASK_GRP_KM0_ITCM0_E0_RSEL            ((u32)0x0000000F << 0)          /*!<R/W 4'd5  rom,FTC_ULP */
#define CTRL_GRP_KM0_ITCM0_E0_RSEL(x)              (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_GRP_KM0_ITCM0_E0_RSEL(x)          ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_RSEL0
 * @brief
 * @{
 **/
#define CTRL_BIT_RSEL0_TEST1                       ((u32)0x00000001 << 5)          /*!<R/W 1'b0   */
#define CTRL_MASK_RSEL0_RM                         ((u32)0x0000000F << 1)          /*!<R/W 4'b0010   */
#define CTRL_RSEL0_RM(x)                           (((u32)((x) & 0x0000000F) << 1))
#define CTRL_GET_RSEL0_RM(x)                       ((u32)(((x >> 1) & 0x0000000F)))
#define CTRL_BIT_RSEL0_RME                         ((u32)0x00000001 << 0)          /*!<R/W 1'b1   */
/** @} */

/** @defgroup REG_CTRL_RSEL1
 * @brief
 * @{
 **/
#define CTRL_BIT_RSEL1_TEST1                       ((u32)0x00000001 << 5)          /*!<R/W 1'b0   */
#define CTRL_MASK_RSEL1_RM                         ((u32)0x0000000F << 1)          /*!<R/W 4'b0010   */
#define CTRL_RSEL1_RM(x)                           (((u32)((x) & 0x0000000F) << 1))
#define CTRL_GET_RSEL1_RM(x)                       ((u32)(((x >> 1) & 0x0000000F)))
#define CTRL_BIT_RSEL1_RME                         ((u32)0x00000001 << 0)          /*!<R/W 1'b1   */
/** @} */

/** @defgroup REG_CTRL_RSEL2
 * @brief
 * @{
 **/
#define CTRL_BIT_RSEL2_TMCK                        ((u32)0x00000001 << 5)          /*!<R/W 1'b0   */
#define CTRL_MASK_RSEL2_DVS                        ((u32)0x0000000F << 1)          /*!<R/W 4'b1001   */
#define CTRL_RSEL2_DVS(x)                          (((u32)((x) & 0x0000000F) << 1))
#define CTRL_GET_RSEL2_DVS(x)                      ((u32)(((x >> 1) & 0x0000000F)))
#define CTRL_BIT_RSEL2_DVSE                        ((u32)0x00000001 << 0)          /*!<R/W 1'b0   */
/** @} */

/** @defgroup REG_CTRL_RSEL3
 * @brief
 * @{
 **/
#define CTRL_BIT_RSEL3_TMCK                        ((u32)0x00000001 << 5)          /*!<R/W 1'b0   */
#define CTRL_MASK_RSEL3_DVS                        ((u32)0x0000000F << 1)          /*!<R/W 4'b0101   */
#define CTRL_RSEL3_DVS(x)                          (((u32)((x) & 0x0000000F) << 1))
#define CTRL_GET_RSEL3_DVS(x)                      ((u32)(((x >> 1) & 0x0000000F)))
#define CTRL_BIT_RSEL3_DVSE                        ((u32)0x00000001 << 0)          /*!<R/W 1'b0   */
/** @} */

/** @defgroup REG_CTRL_RSEL4
 * @brief
 * @{
 **/
#define CTRL_BIT_RSEL4_TMCK                        ((u32)0x00000001 << 5)          /*!<R/W 1'b0   */
#define CTRL_MASK_RSEL4_DVS                        ((u32)0x0000000F << 1)          /*!<R/W 4'b1010   */
#define CTRL_RSEL4_DVS(x)                          (((u32)((x) & 0x0000000F) << 1))
#define CTRL_GET_RSEL4_DVS(x)                      ((u32)(((x >> 1) & 0x0000000F)))
#define CTRL_BIT_RSEL4_DVSE                        ((u32)0x00000001 << 0)          /*!<R/W 1'b0   */
/** @} */

/** @defgroup REG_CTRL_RSEL5
 * @brief
 * @{
 **/
#define CTRL_MASK_RSEL5_DVS                        ((u32)0x0000000F << 1)          /*!<R/W 4'b0101   */
#define CTRL_RSEL5_DVS(x)                          (((u32)((x) & 0x0000000F) << 1))
#define CTRL_GET_RSEL5_DVS(x)                      ((u32)(((x >> 1) & 0x0000000F)))
#define CTRL_BIT_RSEL5_DVSE                        ((u32)0x00000001 << 0)          /*!<R/W 1'b0   */
/** @} */

/** @defgroup REG_CTRL_RSEL6
 * @brief
 * @{
 **/
#define CTRL_BIT_RSEL6_TMCK                        ((u32)0x00000001 << 5)          /*!<R/W 1'b0   */
#define CTRL_MASK_RSEL6_DVS                        ((u32)0x0000000F << 1)          /*!<R/W 4'b0101   */
#define CTRL_RSEL6_DVS(x)                          (((u32)((x) & 0x0000000F) << 1))
#define CTRL_GET_RSEL6_DVS(x)                      ((u32)(((x >> 1) & 0x0000000F)))
#define CTRL_BIT_RSEL6_DVSE                        ((u32)0x00000001 << 0)          /*!<R/W 1'b0   */
/** @} */

/** @defgroup REG_CTRL_RSEL7
 * @brief
 * @{
 **/
#define CTRL_BIT_RSEL7_TMCK                        ((u32)0x00000001 << 5)          /*!<R/W 1'b0   */
#define CTRL_MASK_RSEL7_DVS                        ((u32)0x0000000F << 1)          /*!<R/W 4'b01   */
#define CTRL_RSEL7_DVS(x)                          (((u32)((x) & 0x0000000F) << 1))
#define CTRL_GET_RSEL7_DVS(x)                      ((u32)(((x >> 1) & 0x0000000F)))
#define CTRL_BIT_RSEL7_DVSE                        ((u32)0x00000001 << 0)          /*!<R/W 1'b0   */
/** @} */

/** @defgroup REG_CTRL_RSEL8
 * @brief
 * @{
 **/
#define CTRL_BIT_RSEL8_TMCK                        ((u32)0x00000001 << 5)          /*!<R/W 1'b0   */
#define CTRL_MASK_RSEL8_DVS                        ((u32)0x0000000F << 1)          /*!<R/W 4'b01   */
#define CTRL_RSEL8_DVS(x)                          (((u32)((x) & 0x0000000F) << 1))
#define CTRL_GET_RSEL8_DVS(x)                      ((u32)(((x >> 1) & 0x0000000F)))
#define CTRL_BIT_RSEL8_DVSE                        ((u32)0x00000001 << 0)          /*!<R/W 1'b0   */
/** @} */

/** @defgroup REG_CTRL_RSEL9
 * @brief
 * @{
 **/
#define CTRL_MASK_RSEL9_DVS                        ((u32)0x0000000F << 1)          /*!<R/W 4'b0011   */
#define CTRL_RSEL9_DVS(x)                          (((u32)((x) & 0x0000000F) << 1))
#define CTRL_GET_RSEL9_DVS(x)                      ((u32)(((x >> 1) & 0x0000000F)))
#define CTRL_BIT_RSEL9_DVSE                        ((u32)0x00000001 << 0)          /*!<R/W 1'b0   */
/** @} */

/** @defgroup REG_CTRL_RSEL10
 * @brief
 * @{
 **/
#define CTRL_BIT_RSEL10_TMCK                       ((u32)0x00000001 << 5)          /*!<R/W 1'b0   */
#define CTRL_MASK_RSEL10_DVS                       ((u32)0x0000000F << 1)          /*!<R/W 4'b0011   */
#define CTRL_RSEL10_DVS(x)                         (((u32)((x) & 0x0000000F) << 1))
#define CTRL_GET_RSEL10_DVS(x)                     ((u32)(((x >> 1) & 0x0000000F)))
#define CTRL_BIT_RSEL10_DVSE                       ((u32)0x00000001 << 0)          /*!<R/W 1'b0   */
/** @} */

/** @defgroup REG_CTRL_ASEL0
 * @brief
 * @{
 **/
#define CTRL_MASK_ASEL0_WAEM                       ((u32)0x0000000F << 2)          /*!<R/W 4'b1010   */
#define CTRL_ASEL0_WAEM(x)                         (((u32)((x) & 0x0000000F) << 2))
#define CTRL_GET_ASEL0_WAEM(x)                     ((u32)(((x >> 2) & 0x0000000F)))
#define CTRL_BIT_ASEL0_WAE                         ((u32)0x00000001 << 1)          /*!<R/W 1'b1   */
#define CTRL_BIT_ASEL0_RAE                         ((u32)0x00000001 << 0)          /*!<R/W 1'b1   */
/** @} */

/** @defgroup REG_CTRL_ASEL1
 * @brief
 * @{
 **/
#define CTRL_MASK_ASEL1_WAEM                       ((u32)0x0000000F << 2)          /*!<R/W 4'b1010   */
#define CTRL_ASEL1_WAEM(x)                         (((u32)((x) & 0x0000000F) << 2))
#define CTRL_GET_ASEL1_WAEM(x)                     ((u32)(((x >> 2) & 0x0000000F)))
#define CTRL_BIT_ASEL1_WAE                         ((u32)0x00000001 << 1)          /*!<R/W 1'b1   */
#define CTRL_BIT_ASEL1_RAE                         ((u32)0x00000001 << 0)          /*!<R/W 1'b1   */
/** @} */

/** @defgroup REG_CTRL_ASEL2
 * @brief
 * @{
 **/
#define CTRL_MASK_ASEL2_WAEM                       ((u32)0x0000000F << 2)          /*!<R/W 4'b1010   */
#define CTRL_ASEL2_WAEM(x)                         (((u32)((x) & 0x0000000F) << 2))
#define CTRL_GET_ASEL2_WAEM(x)                     ((u32)(((x >> 2) & 0x0000000F)))
#define CTRL_BIT_ASEL2_WAE                         ((u32)0x00000001 << 1)          /*!<R/W 1'b1   */
#define CTRL_BIT_ASEL2_RAE                         ((u32)0x00000001 << 0)          /*!<R/W 1'b1   */
/** @} */

/** @defgroup REG_CTRL_ASEL3
 * @brief
 * @{
 **/
#define CTRL_MASK_ASEL3_WAEM                       ((u32)0x0000000F << 2)          /*!<R/W 4'b1011   */
#define CTRL_ASEL3_WAEM(x)                         (((u32)((x) & 0x0000000F) << 2))
#define CTRL_GET_ASEL3_WAEM(x)                     ((u32)(((x >> 2) & 0x0000000F)))
#define CTRL_BIT_ASEL3_WAE                         ((u32)0x00000001 << 1)          /*!<R/W 1'b1   */
#define CTRL_BIT_ASEL3_RAE                         ((u32)0x00000001 << 0)          /*!<R/W 1'b1   */
/** @} */

/** @defgroup REG_CTRL_ASEL4
 * @brief
 * @{
 **/
#define CTRL_MASK_ASEL4_WAEM                       ((u32)0x0000000F << 2)          /*!<R/W 4'b1001   */
#define CTRL_ASEL4_WAEM(x)                         (((u32)((x) & 0x0000000F) << 2))
#define CTRL_GET_ASEL4_WAEM(x)                     ((u32)(((x >> 2) & 0x0000000F)))
#define CTRL_BIT_ASEL4_WAE                         ((u32)0x00000001 << 1)          /*!<R/W 1'b1   */
#define CTRL_BIT_ASEL4_RAE                         ((u32)0x00000001 << 0)          /*!<R/W 1'b1   */
/** @} */

/** @defgroup REG_CTRL_ASEL5
 * @brief
 * @{
 **/
#define CTRL_MASK_ASEL5_WAEM                       ((u32)0x0000000F << 2)          /*!<R/W 4'b1001   */
#define CTRL_ASEL5_WAEM(x)                         (((u32)((x) & 0x0000000F) << 2))
#define CTRL_GET_ASEL5_WAEM(x)                     ((u32)(((x >> 2) & 0x0000000F)))
#define CTRL_BIT_ASEL5_WAE                         ((u32)0x00000001 << 1)          /*!<R/W 1'b1   */
#define CTRL_BIT_ASEL5_RAE                         ((u32)0x00000001 << 0)          /*!<R/W 1'b1   */
/** @} */

/** @defgroup REG_CTRL_BT_FTC_MEM_CTRL
 * @brief
 * @{
 **/
#define CTRL_BIT_BT_DS_ALL                         ((u32)0x00000001 << 23)          /*!<R/W 1'b0  1 - All BT memory switch to Deep Sleep mode 0 - Power mode of BT memory are controlled by BTPMC */
#define CTRL_BIT_BT_LVDRF_CLKDIS                   ((u32)0x00000001 << 22)          /*!<R/W 0x0   */
#define CTRL_BIT_BT_TMCK_ALL                       ((u32)0x00000001 << 20)          /*!<R/W 1'b0   */
#define CTRL_BIT_BT_ROM_DVSE                       ((u32)0x00000001 << 19)          /*!<R/W 1'b0   */
#define CTRL_MASK_BT_ROM_DVS                       ((u32)0x0000000F << 15)          /*!<R/W 4'b0101  FSNOU_A_SPNN */
#define CTRL_BT_ROM_DVS(x)                         (((u32)((x) & 0x0000000F) << 15))
#define CTRL_GET_BT_ROM_DVS(x)                     ((u32)(((x >> 15) & 0x0000000F)))
#define CTRL_BIT_BT_1PSR_DVSE                      ((u32)0x00000001 << 14)          /*!<R/W 1'b0  Not used in Lite */
#define CTRL_MASK_BT_1PSR_DVS                      ((u32)0x0000000F << 10)          /*!<R/W 4'b1011  Not used in Lite */
#define CTRL_BT_1PSR_DVS(x)                        (((u32)((x) & 0x0000000F) << 10))
#define CTRL_GET_BT_1PSR_DVS(x)                    ((u32)(((x >> 10) & 0x0000000F)))
#define CTRL_BIT_BT_1PRF1_DVSE                     ((u32)0x00000001 << 9)          /*!<R/W 1'b0   */
#define CTRL_MASK_BT_1PRF1_DVS                     ((u32)0x0000000F << 5)          /*!<R/W 4'b1100  FSNOU_A_SYNL */
#define CTRL_BT_1PRF1_DVS(x)                       (((u32)((x) & 0x0000000F) << 5))
#define CTRL_GET_BT_1PRF1_DVS(x)                   ((u32)(((x >> 5) & 0x0000000F)))
#define CTRL_BIT_BT_1PRF0_DVSE                     ((u32)0x00000001 << 4)          /*!<R/W 1'b0   */
#define CTRL_MASK_BT_1PRF0_DVS                     ((u32)0x0000000F << 0)          /*!<R/W 4'b1100  FSNOU_A_SYNL */
#define CTRL_BT_1PRF0_DVS(x)                       (((u32)((x) & 0x0000000F) << 0))
#define CTRL_GET_BT_1PRF0_DVS(x)                   ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_CTRL_SCAN_CTRL1
 * @brief
 * @{
 **/
#define CTRL_MASK_SCAN_OCC_DBG_EN_WPHY             ((u32)0x00000003 << 12)          /*!<R/W 0x0  scan occ dbg en wphy */
#define CTRL_SCAN_OCC_DBG_EN_WPHY(x)               (((u32)((x) & 0x00000003) << 12))
#define CTRL_GET_SCAN_OCC_DBG_EN_WPHY(x)           ((u32)(((x >> 12) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_SEC              ((u32)0x00000003 << 10)          /*!<R/W 0x0  scan occ dbg en sec */
#define CTRL_SCAN_OCC_DBG_EN_SEC(x)                (((u32)((x) & 0x00000003) << 10))
#define CTRL_GET_SCAN_OCC_DBG_EN_SEC(x)            ((u32)(((x >> 10) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_QSPI             ((u32)0x00000003 << 8)          /*!<R/W 0x0  scan occ dbg en qspi */
#define CTRL_SCAN_OCC_DBG_EN_QSPI(x)               (((u32)((x) & 0x00000003) << 8))
#define CTRL_GET_SCAN_OCC_DBG_EN_QSPI(x)           ((u32)(((x >> 8) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_PSRAM            ((u32)0x00000003 << 6)          /*!<R/W 0x0  scan occ dbg en psram */
#define CTRL_SCAN_OCC_DBG_EN_PSRAM(x)              (((u32)((x) & 0x00000003) << 6))
#define CTRL_GET_SCAN_OCC_DBG_EN_PSRAM(x)          ((u32)(((x >> 6) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_SPIC             ((u32)0x00000003 << 4)          /*!<R/W 0x0  scan occ dbg en spic */
#define CTRL_SCAN_OCC_DBG_EN_SPIC(x)               (((u32)((x) & 0x00000003) << 4))
#define CTRL_GET_SCAN_OCC_DBG_EN_SPIC(x)           ((u32)(((x >> 4) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_HPERI            ((u32)0x00000003 << 2)          /*!<R/W 0x0  scan occ dbg en hperi */
#define CTRL_SCAN_OCC_DBG_EN_HPERI(x)              (((u32)((x) & 0x00000003) << 2))
#define CTRL_GET_SCAN_OCC_DBG_EN_HPERI(x)          ((u32)(((x >> 2) & 0x00000003)))
#define CTRL_MASK_SCAN_OCC_DBG_EN_HP               ((u32)0x00000003 << 0)          /*!<R/W 0x0  scan occ dbg en hp */
#define CTRL_SCAN_OCC_DBG_EN_HP(x)                 (((u32)((x) & 0x00000003) << 0))
#define CTRL_GET_SCAN_OCC_DBG_EN_HP(x)             ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup REG_CTRL_SCAN_CTRL0
 * @brief
 * @{
 **/
#define CTRL_BIT_SCAN_RET_EN                       ((u32)0x00000001 << 12)          /*!<R/W 0x0  scan retention test enable */
#define CTRL_MASK_SCAN_PLL_OUT_SEL                 ((u32)0x00000007 << 9)          /*!<R/W 0x0  scan pll out sel */
#define CTRL_SCAN_PLL_OUT_SEL(x)                   (((u32)((x) & 0x00000007) << 9))
#define CTRL_GET_SCAN_PLL_OUT_SEL(x)               ((u32)(((x >> 9) & 0x00000007)))
#define CTRL_BIT_SCAN_FT1                          ((u32)0x00000001 << 8)          /*!<R/W 0x0  scan ft mode1 */
#define CTRL_BIT_SCAN_FT0                          ((u32)0x00000001 << 7)          /*!<R/W 0x0  scan ft mode0 */
#define CTRL_BIT_SCAN_CP1                          ((u32)0x00000001 << 6)          /*!<R/W 0x0  scan cp mode1/cp_ret */
#define CTRL_BIT_SCAN_CP0                          ((u32)0x00000001 << 5)          /*!<R/W 0x0  scan cp mode0 */
#define CTRL_BIT_SCAN_SPC_DISABLE                  ((u32)0x00000001 << 4)          /*!<R/W 0x0  scan spc disable */
#define CTRL_BIT_SCAN_PLL_BYPASS                   ((u32)0x00000001 << 3)          /*!<R/W 0x0  scan pll bypass */
#define CTRL_BIT_SCAN_COMPRESS                     ((u32)0x00000001 << 2)          /*!<R/W 0x0  scan compress */
#define CTRL_BIT_SCAN_PAR_LATPS                    ((u32)0x00000001 << 1)          /*!<R/W 0x0  Used as scan latch enable bit . If want to change scan par from func setting , set this bit to 1 first , then write func register will also effect scan shadow register . */
#define CTRL_BIT_SCAN_MODE_SETPS                   ((u32)0x00000001 << 0)          /*!<WA0 0x0  scan mode pulse */
/** @} */


/*==========MEM_CTRL Register Address Definition==========*/
#define REG_CTRL_MBIST_CTRL_ALL                      0x0000
#define REG_CTRL_MBIST_GRP_EN_CTRL0                  0x0004
#define REG_CTRL_MBIST_GRP_EN_CTRL1                  0x0008
#define REG_CTRL_MBIST_GRP_EN_BT_VIR_E00             0x000C
#define REG_CTRL_MBIST_DONE_STA0                     0x0010
#define REG_CTRL_MBIST_START_PULSE0                  0x0014
#define REG_CTRL_MBIST_NORM_FAIL0                    0x0018
#define REG_CTRL_MBIST_NORM_FAIL1                    0x001C
#define REG_CTRL_MBIST_DRF_FAIL0                     0x0020
#define REG_CTRL_MBIST_DRF_FAIL1                     0x0024
#define REG_CTRL_BT_VIR_E0_BIST_DONE0                0x0028
#define REG_CTRL_BT_VIR_E0_BIST_FAIL0                0x002C
#define REG_CTRL_BT_VIR_E0_BIST_DRF_PAUSE0           0x0030
#define REG_CTRL_BT_VIR_E0_BIST_RPT                  0x0034
#define REG_CTRL_BT_VIR_E0_RPT_SEL                   0x0038
#define REG_CTRL_MISR_DOUT_SEL                       0x003C
#define REG_CTRL_LSPMC_E0_ROM_MISR0                  0x0040
#define REG_CTRL_LSPMC_E0_ROM_MISR1                  0x0044
#define REG_CTRL_AONPMC_E0_ROM_MISR1                 0x0048
#define REG_CTRL_AONPMC_E0_ROM_MISR2                 0x004C
#define REG_CTRL_KM4_ITCM0_E0_ROM_MISR2              0x0050
#define REG_CTRL_KM4_ITCM0_E0_ROM_MISR3              0x0054
#define REG_CTRL_KM0_ITCM0_E0_ROM_MISR3              0x0058
#define REG_CTRL_KM0_ITCM0_E0_ROM_MISR4              0x005C
#define REG_CTRL_GRP_PWR_MSK0                        0x0060
#define REG_CTRL_GRP_PWR_MSK1                        0x0064
#define REG_CTRL_GRP_HSYS_E0_CTRL                    0x0068
#define REG_CTRL_GRP_LRET_E0_CTRL                    0x006C
#define REG_CTRL_GRP_LSPMC_E0_CTRL                   0x0070
#define REG_CTRL_GRP_AONPMC_E0_CTRL                  0x0074
#define REG_CTRL_GRP_HPER_E0_CTRL                    0x0078
#define REG_CTRL_GRP_WLK4_E0_CTRL                    0x007C
#define REG_CTRL_GRP_WPOFF_E0_CTRL                   0x0080
#define REG_CTRL_GRP_WPOFF_E1_CTRL                   0x0084
#define REG_CTRL_GRP_WPOFF_E2_CTRL                   0x0088
#define REG_CTRL_GRP_WPOFF_E3_CTRL                   0x008C
#define REG_CTRL_GRP_WPOFF_E4_CTRL                   0x0090
#define REG_CTRL_GRP_WPOFF_E5_CTRL                   0x0094
#define REG_CTRL_GRP_WPON_E0_CTRL                    0x0098
#define REG_CTRL_GRP_BT_E0_CTRL                      0x009C
#define REG_CTRL_GRP_BT_E1_CTRL                      0x00A0
#define REG_CTRL_GRP_KM4_DCACHE_E0_CTRL              0x00A4
#define REG_CTRL_GRP_KM4_ICACHE_E0_CTRL              0x00A8
#define REG_CTRL_GRP_KM4_ITCM0_E0_CTRL               0x00AC
#define REG_CTRL_GRP_KM0_DCACHE_E0_CTRL              0x00B0
#define REG_CTRL_GRP_KM0_ICACHE_E0_CTRL              0x00B4
#define REG_CTRL_GRP_KM0_ITCM0_E0_CTRL               0x00B8
#define REG_CTRL_GRP_BT_VIR_E0_CTRL                  0x00BC
#define REG_CTRL_RSEL0                               0x00C0
#define REG_CTRL_RSEL1                               0x00C4
#define REG_CTRL_RSEL2                               0x00C8
#define REG_CTRL_RSEL3                               0x00CC
#define REG_CTRL_RSEL4                               0x00D0
#define REG_CTRL_RSEL5                               0x00D4
#define REG_CTRL_RSEL6                               0x00D8
#define REG_CTRL_RSEL7                               0x00DC
#define REG_CTRL_RSEL8                               0x00E0
#define REG_CTRL_RSEL9                               0x00E4
#define REG_CTRL_RSEL10                              0x00E8
#define REG_CTRL_ASEL0                               0x00EC
#define REG_CTRL_ASEL1                               0x00F0
#define REG_CTRL_ASEL2                               0x00F4
#define REG_CTRL_ASEL3                               0x00F8
#define REG_CTRL_ASEL4                               0x00FC
#define REG_CTRL_ASEL5                               0x0100
#define REG_CTRL_BT_FTC_MEM_CTRL                     0x0184
#define REG_CTRL_SCAN_CTRL1                          0x01C0
#define REG_CTRL_SCAN_CTRL0                          0x01C4
#define REG_CTRL_DUMMY                               0x01FC

/** @} */

/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup MEM_CTRL_LPS_define
  * @{
  */
#define BIT_MEM_CTRL_LPS_ACTIVE		0x0
#define BIT_MEM_CTRL_LPS_LS			0x1
#define BIT_MEM_CTRL_LPS_DS			0x2
#define BIT_MEM_CTRL_LPS_SD			0x4

/** @defgroup MEM_CTRL_LPS_define (BIT14 of LRET, BTOFF netlist if reverse, bit14 = 0 represents slp)
  * @{
  */
#define BIT_MEM_CTRL_LPS_ACTIVE_REV		0x4		//active
#define BIT_MEM_CTRL_LPS_LS_REV			0x5		//light sleep
#define BIT_MEM_CTRL_LPS_DS_REV			0x6		//deep sleep
#define BIT_MEM_CTRL_LPS_SD_REV			0x0		//shutdown


/** @defgroup MEM_CTRL_ACT_define
  * @{
  */
#define BIT_MEM_CTRL_ACT_ACTIVE		0x000
#define BIT_MEM_CTRL_ACT_LS			0x001
#define BIT_MEM_CTRL_ACT_DS			0x010
#define BIT_MEM_CTRL_ACT_SD			0x100


//Please add your defination here
#define SYSTEM_MEM_CTRL_BASE     0x41008600

#define CTRL_MASK_GRP_x_PWR_LPS			((u32)0x00000007 << 11)          /*!<R/W 3'b010  Reserved for grp_x_pwr_lps for memory groups power controlled by SYSPMC. Setted before chip goes to SLEEP.Used to controll power mode of this memory group when chip is in SLEEP. For FTC_ULP mem, pwr_lps[2:0] map to {SLP,RET,NAP}; For SNPS mem, pwr_lps[2:0] map to {SD,DS,LS}. 000 - active 001 - light sleep 010 - deep sleep 100 - shut down */
#define CTRL_GRP_x_PWR_LPS(x)			((u32)(((x) & 0x00000007) << 11))

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

#endif

/** @} */

/** @} */
