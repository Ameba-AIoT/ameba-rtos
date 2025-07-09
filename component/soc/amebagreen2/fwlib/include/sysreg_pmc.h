/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SYSREG_PMC_H_
#define _SYSREG_PMC_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup PMC
  * @brief PMC driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup PMC_Register_Definitions PMC Register Definitions
  * @{
  */

/** @defgroup WAK_STATUS0
  * @brief
  * @{
  */
#define PMC_MASK_WAK_EVT0_STA_29_0   ((u32)0x3FFFFFFF << 0)           /*!< R 0x0  Acoording chip_arch spec*/
#define PMC_WAK_EVT0_STA_29_0(x)     (((u32)((x) & 0x3FFFFFFF) << 0))
#define PMC_GET_WAK_EVT0_STA_29_0(x) ((u32)(((x >> 0) & 0x3FFFFFFF)))
/** @} */

/** @defgroup WAK_STATUS1
  * @brief
  * @{
  */
#define PMC_MASK_WAK_EVT0_STA_59_30   ((u32)0x3FFFFFFF << 0)           /*!< R 0x0  Acoording chip_arch spec*/
#define PMC_WAK_EVT0_STA_59_30(x)     (((u32)((x) & 0x3FFFFFFF) << 0))
#define PMC_GET_WAK_EVT0_STA_59_30(x) ((u32)(((x >> 0) & 0x3FFFFFFF)))
/** @} */

/** @defgroup WAK_MASK0_NP
  * @brief
  * @{
  */
#define PMC_MASK_WAK_NP_IMR_29_0   ((u32)0x3FFFFFFF << 0)           /*!< R/W 0x0  1:mask enable for wake NP, this used to wak pmc*/
#define PMC_WAK_NP_IMR_29_0(x)     (((u32)((x) & 0x3FFFFFFF) << 0))
#define PMC_GET_WAK_NP_IMR_29_0(x) ((u32)(((x >> 0) & 0x3FFFFFFF)))
/** @} */

/** @defgroup WAK_MASK1_NP
  * @brief
  * @{
  */
#define PMC_MASK_WAK_NP_IMR_59_30   ((u32)0x3FFFFFFF << 0)           /*!< R/W 0x0  1:mask enable for wake NP, this used to wak pmc*/
#define PMC_WAK_NP_IMR_59_30(x)     (((u32)((x) & 0x3FFFFFFF) << 0))
#define PMC_GET_WAK_NP_IMR_59_30(x) ((u32)(((x >> 0) & 0x3FFFFFFF)))
/** @} */

/** @defgroup WAK_MASK0_AP
  * @brief
  * @{
  */
#define PMC_MASK_WAK_AP_IMR_29_0   ((u32)0x3FFFFFFF << 0)           /*!< R/W 0x0  1:mask enable for wake AP*/
#define PMC_WAK_AP_IMR_29_0(x)     (((u32)((x) & 0x3FFFFFFF) << 0))
#define PMC_GET_WAK_AP_IMR_29_0(x) ((u32)(((x >> 0) & 0x3FFFFFFF)))
/** @} */

/** @defgroup WAK_MASK1_AP
  * @brief
  * @{
  */
#define PMC_MASK_WAK_AP_IMR_59_30   ((u32)0x3FFFFFFF << 0)           /*!< R/W 0x0  1:mask enable for wake AP*/
#define PMC_WAK_AP_IMR_59_30(x)     (((u32)((x) & 0x3FFFFFFF) << 0))
#define PMC_GET_WAK_AP_IMR_59_30(x) ((u32)(((x >> 0) & 0x3FFFFFFF)))
/** @} */

/** @defgroup WLPMC_ISR
  * @brief
  * @{
  */
#define PMC_BIT_WL_OFF_IMR ((u32)0x00000001 << 17) /*!< R/W 0x0   DD_SEC: S*/
#define PMC_BIT_WL_ON_IMR  ((u32)0x00000001 << 16) /*!< R/W 0x0   DD_SEC: S*/
#define PMC_BIT_WL_OFF_ISR ((u32)0x00000001 << 1)  /*!< RW1CB 0x0   DD_SEC: S*/
#define PMC_BIT_WL_ON_ISR  ((u32)0x00000001 << 0)  /*!< RW1CB 0x0   DD_SEC: S*/
/** @} */

/** @defgroup WLAN_PWRCTL
  * @brief
  * @{
  */
#define PMC_BIT_SW_CLR_RDY2ACT ((u32)0x00000001 << 17) /*!< R/WA0 0x0  System infrom WLAN ready to active DD_SEC: S*/
#define PMC_BIT_SW_SET_RDY2ACT ((u32)0x00000001 << 16) /*!< R/WA0 0x0   DD_SEC: S*/
#define PMC_BIT_WL_RDY2ACT     ((u32)0x00000001 << 9)  /*!< R 0x0  System infrom WLAN ready to active DD_SEC: S*/
#define PMC_BIT_WL_REQ2ACT     ((u32)0x00000001 << 8)  /*!< R 0x0   DD_SEC: S*/
#define PMC_BIT_WL_HW_RESP_FEN ((u32)0x00000001 << 0)  /*!< R/W 0x1  1: enable HW auto response DD_SEC: S*/
/** @} */

/** @defgroup SYSPMC_CTRL
  * @brief
  * @{
  */
#define PMC_BIT_CPU_IS_AP     ((u32)0x00000001 << 28)           /*!< R 0x0  This bit indicate if current cpu who read this bit is used as AP 1: is AP 0: is NP*/
#define PMC_BIT_NP_CPU_ID     ((u32)0x00000001 << 27)           /*!< R/W 0x1  0:cpu0(km4tz) used as NP, pmc will enable it after system wake 1:cpu1(km4ns) used as NP. DD_SEC: S*/
#define PMC_MASK_STL_PDSOC    ((u32)0x00000007 << 24)           /*!< R/W 0x6  timer wait for core power power-cut power stable . 000:8us * 2^1 001:8us * 2^2 010:8us * 2^3  // 64us 011:8us * 2^4 100:8us * 2^5 101:8us * 2^6   110:8us * 2^7  // pon default 111:8us * 2^8 DD_SEC: bpc_pmc*/
#define PMC_STL_PDSOC(x)      (((u32)((x) & 0x00000007) << 24))
#define PMC_GET_STL_PDSOC(x)  ((u32)(((x >> 24) & 0x00000007)))
#define PMC_MASK_TUTIME       ((u32)0x0000003F << 16)           /*!< R/W 0x1F  PMC timeunit setting,  in cycles of osc4M  e.g, default 0x1F,  timeunit = 8us DD_SEC: bpc_pmc*/
#define PMC_TUTIME(x)         (((u32)((x) & 0x0000003F) << 16))
#define PMC_GET_TUTIME(x)     ((u32)(((x >> 16) & 0x0000003F)))
#define PMC_BIT_PLL_POF       ((u32)0x00000001 << 7)            /*!< R/W 0x0  1: Request PLL pof by software,plfm and system cpu will auto select to pll path . Which one pll will be active is determined by pst_cksl_sys_cpu and pst_cksl_sys_hperi.  This bit auto clear to 0 after by HW PMC Note: HW will pof one pll or two if cpu pll and hperi cpu are different after arbitration with bt . DD_SEC: bpc_pmc*/
#define PMC_BIT_PLL_PON       ((u32)0x00000001 << 6)            /*!< R/W 0x0  1: Request PLL pon by software,plfm and system cpu will auto select to pll path . Which one pll will be active is determined by pst_cksl_sys_cpu and pst_cksl_sys_hperi.  This bit auto clear to 0 after by HW PMC Note: HW will pon one pll or two if cpu pll and hperi cpu are different DD_SEC: bpc_pmc*/
#define PMC_BIT_MEM_SLP_MODE  ((u32)0x00000001 << 5)            /*!< R/W 0x0  Select memory parameter option 1: sleep setting 0: normal setting Note:defautl set to sleep setting to save power when snooze DD_SEC: bpc_pmc*/
#define PMC_BIT_POF_WAIT_SLEP ((u32)0x00000001 << 4)            /*!< R/W 0x1  1.when SW set pmen_slep or pmen_dslp , hw pmc will wait cpu0 enter sleep mode , then begin to pof seq 0.Not wait cpu sleep DD_SEC: bpc_pmc*/
#define PMC_BIT_PST_SLEP_WDML ((u32)0x00000001 << 3)            /*!< R/W 0x0  normal setting for WMAC request Core LDO dummy load selection 1: enable dummy load when PMC enter into sleep mode 0: disable dummy load when PMC enter into sleep mode DD_SEC: bpc_pmc*/
#define PMC_BIT_CPU0_IRQ_MASK ((u32)0x00000001 << 2)            /*!< R/W 0x0  1.mask all KM4 interrupt 0:release all KM4 interrupt DD_SEC: bpc_pmc*/
#define PMC_BIT_PMEN_SLEP     ((u32)0x00000001 << 1)            /*!< R/W 0x0  1: Request PMC to enter sleep state, auto clear to 0 after PMC finishes DD_SEC: bpc_pmc*/
#define PMC_BIT_PMEN_DSLP     ((u32)0x00000001 << 0)            /*!< R/W 0x0  1: Request PMC to enter deep-sleep state, auto clear to 0 after PMC finishes DD_SEC: bpc_pmc*/
/** @} */

/** @defgroup SYSPMC_OPT
  * @brief
  * @{
  */
#define PMC_BIT_CKE_XTAL40M_SLEP ((u32)0x00000001 << 28) /*!< R/W 1'b0  when pmc is sleep status, this bit control u_ckg_xtal40m_slep 1:Enable xtal40m_slep 0:Disable Note:when pmc is not sleep, u_ckg_xtal40m_slep will be enabled whether value this bit is Remove to lp_pmc option register DD_SEC: bpc_pmc*/
#define PMC_BIT_PST_CHG_BDML     ((u32)0x00000001 << 26) /*!< R/W 0x1  normal setting for BT request Core LDO dummy load selection 1: enable dummy load during BT PMC change to sleep mode / normal mode 0: disable dummy load during BT PMC change to sleep mode / normal mode DD_SEC: bpc_pmc*/
#define PMC_BIT_PST_G4_USE_MLDO  ((u32)0x00000001 << 25) /*!< R/W 0x1  1: G4 pad use MLDO 1.8v IO power 0: G4 pad use external power and can't shutdown by pmc DD_SEC: bpc_pmc*/
#define PMC_BIT_PST_G5_USE_MLDO  ((u32)0x00000001 << 24) /*!< R/W 0x1  1: G5 pad use MLDO 1.8v IO power(psram only) 0: G5 pad use external power and can't shutdown by pmc DD_SEC: bpc_pmc*/
#define PMC_BIT_CPU1_IRQ_MASK    ((u32)0x00000001 << 23) /*!< R/W 0x0  1.mask all CPU1 interrupt 0:release all CPU1 interrupt DD_SEC: bpc_pmc*/
#define PMC_BIT_PST_SLEP_BDML    ((u32)0x00000001 << 22) /*!< R/W 0x0  normal setting for BT request Core LDO dummy load selection 1: enable dummy load when PMC enter into sleep mode 0: disable dummy load when PMC enter into sleep mode DD_SEC: bpc_pmc*/
#define PMC_BIT_PST_SLEP_SDML    ((u32)0x00000001 << 21) /*!< R/W 0x0  normal setting for system request Core LDO dummy load selection 1: enable dummy load when PMC enter into sleep mode 0: disable dummy load when PMC enter into sleep mode DD_SEC: bpc_pmc*/
#define PMC_BIT_PST_NORM_WDML    ((u32)0x00000001 << 20) /*!< R/W 0x0  normal setting for WMAC request Core LDO dummy load selection 1: enable dummy load when PMC enter into normal mode 0: disable dummy load when PMC enter into normal mode DD_SEC: bpc_pmc*/
#define PMC_BIT_PST_NORM_BDML    ((u32)0x00000001 << 19) /*!< R/W 0x0  normal setting for BT request Core LDO dummy load selection 1: enable dummy load when PMC enter into normal mode 0: disable dummy load when PMC enter into normal mode DD_SEC: bpc_pmc*/
#define PMC_BIT_PST_NORM_SDML    ((u32)0x00000001 << 18) /*!< R/W 0x0  normal setting for system request Core LDO dummy load selection 1: enable dummy load when PMC enter into normal mode 0: disable dummy load when PMC enter into normal mode DD_SEC: bpc_pmc*/
#define PMC_BIT_PST_SLEP_G4PD    ((u32)0x00000001 << 7)  /*!< R/W 0x1  1: G4 pad used for 1.8v IO power , and will shutdown when LDO3318 off in slep, when dslp , it will always shutoff 0: G1 pad not used as 1.8v IO power DD_SEC: bpc_pmc*/
#define PMC_BIT_PST_SLEP_G5PD    ((u32)0x00000001 << 6)  /*!< R/W 0x1  1: G5 pad used for 1.8v IO power , and will shutdown when LDO3318 off in slep,, when dslp , it will always shutoff 0: G2 pad not used as 1.8v IO power DD_SEC: bpc_pmc*/
#define PMC_BIT_PST_SLEP_ERCK    ((u32)0x00000001 << 3)  /*!< R/W 0x0  1: Enable lpon clock when PMC enter into sleep mode 0: Gate lpon clock when PMC enter into sleep mode DD_SEC: bpc_pmc*/
#define PMC_BIT_PST_SLEP_ESOC    ((u32)0x00000001 << 0)  /*!< R/W 0x0  1: power enable SOC platform when PMC enter into sleep mode (clock-gating) 0: power off SoC domain when PMC enter into sleep mode (power-gating) DD_SEC: bpc_pmc*/
/** @} */

/** @defgroup DUMMY_LOAD_CTRL
  * @brief
  * @{
  */
#define PMC_BIT_SYS_DMYL_SLP ((u32)0x00000001 << 2) /*!< R/W 0x1  this bit means the dummy load state when KM4 is sleep 0: disable 1: enable*/
#define PMC_BIT_SYS_DMYL_ACT ((u32)0x00000001 << 1) /*!< R/W 0x0  this bit means the dummy load state when KM4 is active 0: disable 1: enable*/
#define PMC_BIT_SYS_DMYL_MNL ((u32)0x00000001 << 0) /*!< R/W 0x0  system use this bit to switch core ldo dummy load control path manually 0: dummy load control is decided by LS/BT/WL PMC arbitration 1: dummy load control is decided by KM4 sleep state*/
/** @} */

/** @defgroup PMC_PATCH_GRP0_0
  * @brief
  * @{
  */
#define PMC_MASK_PATCH_GRP0_INSTR1_5_0   ((u32)0x0000003F << 26)           /*!< R/W 0x0  instruction1 of  LSYSON PMC patch group0,bit[5:0] DD_SEC: S*/
#define PMC_PATCH_GRP0_INSTR1_5_0(x)     (((u32)((x) & 0x0000003F) << 26))
#define PMC_GET_PATCH_GRP0_INSTR1_5_0(x) ((u32)(((x >> 26) & 0x0000003F)))
#define PMC_MASK_PATCH_GRP0_INSTR0       ((u32)0x0003FFFF << 8)            /*!< R/W 0x0  instruction0 of  LSYSON PMC patch group0 DD_SEC: S*/
#define PMC_PATCH_GRP0_INSTR0(x)         (((u32)((x) & 0x0003FFFF) << 8))
#define PMC_GET_PATCH_GRP0_INSTR0(x)     ((u32)(((x >> 8) & 0x0003FFFF)))
#define PMC_MASK_PATCH_GRP0_SADDR        ((u32)0x000000FF << 0)            /*!< R/W 0x0  Start address of the first instruction of  LSYSON PMC patch group0 DD_SEC: S*/
#define PMC_PATCH_GRP0_SADDR(x)          (((u32)((x) & 0x000000FF) << 0))
#define PMC_GET_PATCH_GRP0_SADDR(x)      ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup PMC_PATCH_GRP0_1
  * @brief
  * @{
  */
#define PMC_MASK_PATCH_GRP0_INSTR2        ((u32)0x0003FFFF << 12)           /*!< R/W 0x0  instruction2 of  LSYSON PMC patch group0 DD_SEC: S*/
#define PMC_PATCH_GRP0_INSTR2(x)          (((u32)((x) & 0x0003FFFF) << 12))
#define PMC_GET_PATCH_GRP0_INSTR2(x)      ((u32)(((x >> 12) & 0x0003FFFF)))
#define PMC_MASK_PATCH_GRP0_INSTR1_17_6   ((u32)0x00000FFF << 0)            /*!< R/W 0x0  instruction1 of  LSYSON PMC patch group0,bit[17:6] DD_SEC: S*/
#define PMC_PATCH_GRP0_INSTR1_17_6(x)     (((u32)((x) & 0x00000FFF) << 0))
#define PMC_GET_PATCH_GRP0_INSTR1_17_6(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup PMC_PATCH_GRP1_0
  * @brief
  * @{
  */
#define PMC_MASK_PATCH_GRP1_INSTR1_5_0   ((u32)0x0000003F << 26)           /*!< R/W 0x0  instruction1 of  LSYSON PMC patch group1,bit[5:0] DD_SEC: S*/
#define PMC_PATCH_GRP1_INSTR1_5_0(x)     (((u32)((x) & 0x0000003F) << 26))
#define PMC_GET_PATCH_GRP1_INSTR1_5_0(x) ((u32)(((x >> 26) & 0x0000003F)))
#define PMC_MASK_PATCH_GRP1_INSTR0       ((u32)0x0003FFFF << 8)            /*!< R/W 0x0  instruction0 of  LSYSON PMC patch group1 DD_SEC: S*/
#define PMC_PATCH_GRP1_INSTR0(x)         (((u32)((x) & 0x0003FFFF) << 8))
#define PMC_GET_PATCH_GRP1_INSTR0(x)     ((u32)(((x >> 8) & 0x0003FFFF)))
#define PMC_MASK_PATCH_GRP1_SADDR        ((u32)0x000000FF << 0)            /*!< R/W 0x0  Start address of the first instruction of  LSYSON PMC patch group1 DD_SEC: S*/
#define PMC_PATCH_GRP1_SADDR(x)          (((u32)((x) & 0x000000FF) << 0))
#define PMC_GET_PATCH_GRP1_SADDR(x)      ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup PMC_PATCH_GRP1_1
  * @brief
  * @{
  */
#define PMC_MASK_PATCH_GRP1_INSTR2        ((u32)0x0003FFFF << 12)           /*!< R/W 0x0  instruction2 of  LSYSON PMC patch group1 DD_SEC: S*/
#define PMC_PATCH_GRP1_INSTR2(x)          (((u32)((x) & 0x0003FFFF) << 12))
#define PMC_GET_PATCH_GRP1_INSTR2(x)      ((u32)(((x >> 12) & 0x0003FFFF)))
#define PMC_MASK_PATCH_GRP1_INSTR1_17_6   ((u32)0x00000FFF << 0)            /*!< R/W 0x0  instruction1 of  LSYSON PMC patch group1,bit[17:6] DD_SEC: S*/
#define PMC_PATCH_GRP1_INSTR1_17_6(x)     (((u32)((x) & 0x00000FFF) << 0))
#define PMC_GET_PATCH_GRP1_INSTR1_17_6(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup PMC_PATCH_GRP2_0
  * @brief
  * @{
  */
#define PMC_MASK_PATCH_GRP2_INSTR1_5_0   ((u32)0x0000003F << 26)           /*!< R/W 0x0  instruction1 of  LSYSON PMC patch group2,bit[5:0] DD_SEC: S*/
#define PMC_PATCH_GRP2_INSTR1_5_0(x)     (((u32)((x) & 0x0000003F) << 26))
#define PMC_GET_PATCH_GRP2_INSTR1_5_0(x) ((u32)(((x >> 26) & 0x0000003F)))
#define PMC_MASK_PATCH_GRP2_INSTR0       ((u32)0x0003FFFF << 8)            /*!< R/W 0x0  instruction0 of  LSYSON PMC patch group2 DD_SEC: S*/
#define PMC_PATCH_GRP2_INSTR0(x)         (((u32)((x) & 0x0003FFFF) << 8))
#define PMC_GET_PATCH_GRP2_INSTR0(x)     ((u32)(((x >> 8) & 0x0003FFFF)))
#define PMC_MASK_PATCH_GRP2_SADDR        ((u32)0x000000FF << 0)            /*!< R/W 0x0  Start address of the first instruction of  LSYSON PMC patch group2 DD_SEC: S*/
#define PMC_PATCH_GRP2_SADDR(x)          (((u32)((x) & 0x000000FF) << 0))
#define PMC_GET_PATCH_GRP2_SADDR(x)      ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup PMC_PATCH_GRP2_1
  * @brief
  * @{
  */
#define PMC_MASK_PATCH_GRP2_INSTR2        ((u32)0x0003FFFF << 12)           /*!< R/W 0x0  instruction2 of  LSYSON PMC patch group2 DD_SEC: S*/
#define PMC_PATCH_GRP2_INSTR2(x)          (((u32)((x) & 0x0003FFFF) << 12))
#define PMC_GET_PATCH_GRP2_INSTR2(x)      ((u32)(((x >> 12) & 0x0003FFFF)))
#define PMC_MASK_PATCH_GRP2_INSTR1_17_6   ((u32)0x00000FFF << 0)            /*!< R/W 0x0  instruction1 of  LSYSON PMC patch group2,bit[17:6] DD_SEC: S*/
#define PMC_PATCH_GRP2_INSTR1_17_6(x)     (((u32)((x) & 0x00000FFF) << 0))
#define PMC_GET_PATCH_GRP2_INSTR1_17_6(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup PMC_PATCH_GRP3_0
  * @brief
  * @{
  */
#define PMC_MASK_PATCH_GRP3_INSTR1_5_0   ((u32)0x0000003F << 26)           /*!< R/W 0x0  instruction1 of  LSYSON PMC patch group3,bit[5:0] DD_SEC: S*/
#define PMC_PATCH_GRP3_INSTR1_5_0(x)     (((u32)((x) & 0x0000003F) << 26))
#define PMC_GET_PATCH_GRP3_INSTR1_5_0(x) ((u32)(((x >> 26) & 0x0000003F)))
#define PMC_MASK_PATCH_GRP3_INSTR0       ((u32)0x0003FFFF << 8)            /*!< R/W 0x0  instruction0 of  LSYSON PMC patch group3 DD_SEC: S*/
#define PMC_PATCH_GRP3_INSTR0(x)         (((u32)((x) & 0x0003FFFF) << 8))
#define PMC_GET_PATCH_GRP3_INSTR0(x)     ((u32)(((x >> 8) & 0x0003FFFF)))
#define PMC_MASK_PATCH_GRP3_SADDR        ((u32)0x000000FF << 0)            /*!< R/W 0x0  Start address of the first instruction of  LSYSON PMC patch group3 DD_SEC: S*/
#define PMC_PATCH_GRP3_SADDR(x)          (((u32)((x) & 0x000000FF) << 0))
#define PMC_GET_PATCH_GRP3_SADDR(x)      ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup PMC_PATCH_GRP3_1
  * @brief
  * @{
  */
#define PMC_MASK_PATCH_GRP3_INSTR2        ((u32)0x0003FFFF << 12)           /*!< R/W 0x0  instruction2 of  LSYSON PMC patch group3 DD_SEC: S*/
#define PMC_PATCH_GRP3_INSTR2(x)          (((u32)((x) & 0x0003FFFF) << 12))
#define PMC_GET_PATCH_GRP3_INSTR2(x)      ((u32)(((x >> 12) & 0x0003FFFF)))
#define PMC_MASK_PATCH_GRP3_INSTR1_17_6   ((u32)0x00000FFF << 0)            /*!< R/W 0x0  instruction1 of  LSYSON PMC patch group3,bit[17:6] DD_SEC: S*/
#define PMC_PATCH_GRP3_INSTR1_17_6(x)     (((u32)((x) & 0x00000FFF) << 0))
#define PMC_GET_PATCH_GRP3_INSTR1_17_6(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup PMC_PATCH_GRP4_0
  * @brief
  * @{
  */
#define PMC_MASK_PATCH_GRP4_INSTR1_5_0   ((u32)0x0000003F << 26)           /*!< R/W 0x0  instruction1 of  LSYSON PMC patch group4,bit[5:0] DD_SEC: S*/
#define PMC_PATCH_GRP4_INSTR1_5_0(x)     (((u32)((x) & 0x0000003F) << 26))
#define PMC_GET_PATCH_GRP4_INSTR1_5_0(x) ((u32)(((x >> 26) & 0x0000003F)))
#define PMC_MASK_PATCH_GRP4_INSTR0       ((u32)0x0003FFFF << 8)            /*!< R/W 0x0  instruction0 of  LSYSON PMC patch group4 DD_SEC: S*/
#define PMC_PATCH_GRP4_INSTR0(x)         (((u32)((x) & 0x0003FFFF) << 8))
#define PMC_GET_PATCH_GRP4_INSTR0(x)     ((u32)(((x >> 8) & 0x0003FFFF)))
#define PMC_MASK_PATCH_GRP4_SADDR        ((u32)0x000000FF << 0)            /*!< R/W 0x0  Start address of the first instruction of  LSYSON PMC patch group4 DD_SEC: S*/
#define PMC_PATCH_GRP4_SADDR(x)          (((u32)((x) & 0x000000FF) << 0))
#define PMC_GET_PATCH_GRP4_SADDR(x)      ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup PMC_PATCH_GRP4_1
  * @brief
  * @{
  */
#define PMC_MASK_PATCH_GRP4_INSTR2        ((u32)0x0003FFFF << 12)           /*!< R/W 0x0  instruction2 of  LSYSON PMC patch group4 DD_SEC: S*/
#define PMC_PATCH_GRP4_INSTR2(x)          (((u32)((x) & 0x0003FFFF) << 12))
#define PMC_GET_PATCH_GRP4_INSTR2(x)      ((u32)(((x >> 12) & 0x0003FFFF)))
#define PMC_MASK_PATCH_GRP4_INSTR1_17_6   ((u32)0x00000FFF << 0)            /*!< R/W 0x0  instruction1 of  LSYSON PMC patch group4,bit[17:6] DD_SEC: S*/
#define PMC_PATCH_GRP4_INSTR1_17_6(x)     (((u32)((x) & 0x00000FFF) << 0))
#define PMC_GET_PATCH_GRP4_INSTR1_17_6(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup PMC_PATCH_GRP5_0
  * @brief
  * @{
  */
#define PMC_MASK_PATCH_GRP5_INSTR1_5_0   ((u32)0x0000003F << 26)           /*!< R/W 0x0  instruction1 of  LSYSON PMC patch group5,bit[5:0] DD_SEC: S*/
#define PMC_PATCH_GRP5_INSTR1_5_0(x)     (((u32)((x) & 0x0000003F) << 26))
#define PMC_GET_PATCH_GRP5_INSTR1_5_0(x) ((u32)(((x >> 26) & 0x0000003F)))
#define PMC_MASK_PATCH_GRP5_INSTR0       ((u32)0x0003FFFF << 8)            /*!< R/W 0x0  instruction0 of  LSYSON PMC patch group5 DD_SEC: S*/
#define PMC_PATCH_GRP5_INSTR0(x)         (((u32)((x) & 0x0003FFFF) << 8))
#define PMC_GET_PATCH_GRP5_INSTR0(x)     ((u32)(((x >> 8) & 0x0003FFFF)))
#define PMC_MASK_PATCH_GRP5_SADDR        ((u32)0x000000FF << 0)            /*!< R/W 0x0  Start address of the first instruction of  LSYSON PMC patch group5 DD_SEC: S*/
#define PMC_PATCH_GRP5_SADDR(x)          (((u32)((x) & 0x000000FF) << 0))
#define PMC_GET_PATCH_GRP5_SADDR(x)      ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup PMC_PATCH_GRP5_1
  * @brief
  * @{
  */
#define PMC_MASK_PATCH_GRP5_INSTR2        ((u32)0x0003FFFF << 12)           /*!< R/W 0x0  instruction2 of  LSYSON PMC patch group5 DD_SEC: S*/
#define PMC_PATCH_GRP5_INSTR2(x)          (((u32)((x) & 0x0003FFFF) << 12))
#define PMC_GET_PATCH_GRP5_INSTR2(x)      ((u32)(((x >> 12) & 0x0003FFFF)))
#define PMC_MASK_PATCH_GRP5_INSTR1_17_6   ((u32)0x00000FFF << 0)            /*!< R/W 0x0  instruction1 of  LSYSON PMC patch group5,bit[17:6] DD_SEC: S*/
#define PMC_PATCH_GRP5_INSTR1_17_6(x)     (((u32)((x) & 0x00000FFF) << 0))
#define PMC_GET_PATCH_GRP5_INSTR1_17_6(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup PMC_DBG_FLAG
  * @brief
  * @{
  */
#define PMC_MASK_LPON_PMC_FLAG_PC   ((u32)0x000000FF << 0)           /*!< R/W 0x0  PMC will raise a debug flag if hw pmc pc value hit this value DD_SEC: S*/
#define PMC_LPON_PMC_FLAG_PC(x)     (((u32)((x) & 0x000000FF) << 0))
#define PMC_GET_LPON_PMC_FLAG_PC(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup PMC_ADDR45
  * @brief
  * @{
  */
#define PMC_MASK_PATCH_ADDR0   ((u32)0x00000FFF << 0)            /*!< R/W 12'hF0  HW used only , these are for pmc patch DD_SEC: S*/
#define PMC_PATCH_ADDR0(x)     (((u32)((x) & 0x00000FFF) << 0))
#define PMC_GET_PATCH_ADDR0(x) ((u32)(((x >> 0) & 0x00000FFF)))
#define PMC_MASK_PATCH_ADDR1   ((u32)0x00000FFF << 16)           /*!< R/W 12'hF0  HW used only , these are for pmc patch DD_SEC: S*/
#define PMC_PATCH_ADDR1(x)     (((u32)((x) & 0x00000FFF) << 16))
#define PMC_GET_PATCH_ADDR1(x) ((u32)(((x >> 16) & 0x00000FFF)))
/** @} */

/** @defgroup PMC_ADDR67
  * @brief
  * @{
  */
#define PMC_MASK_PATCH_ADDR2   ((u32)0x00000FFF << 0)            /*!< R/W 12'hF0  HW used only , these are for pmc patch DD_SEC: S*/
#define PMC_PATCH_ADDR2(x)     (((u32)((x) & 0x00000FFF) << 0))
#define PMC_GET_PATCH_ADDR2(x) ((u32)(((x >> 0) & 0x00000FFF)))
#define PMC_MASK_PATCH_ADDR3   ((u32)0x00000FFF << 16)           /*!< R/W 12'hF0  HW used only , these are for pmc patch DD_SEC: S*/
#define PMC_PATCH_ADDR3(x)     (((u32)((x) & 0x00000FFF) << 16))
#define PMC_GET_PATCH_ADDR3(x) ((u32)(((x >> 16) & 0x00000FFF)))
/** @} */

/** @defgroup AIP_TRIGGER
  * @brief
  * @{
  */
#define PMC_BIT_SYS_PWM_H            ((u32)0x00000001 << 30)           /*!< R/W 1'b0  Set pwm in heavy loading by system (SWR current > 200mA) 1:Heavy 0:Light DD_SEC: bpc_pmc*/
#define PMC_MASK_SYS_MLDO_SLP        ((u32)0x00000003 << 26)           /*!< R/W 2'b00  mem ldo mode slep option  00:off 01:sleep (wo dummy) 10:normal-0 (wi dummy) 10:normal-1 (wo dummy) DD_SEC: bpc_pmc*/
#define PMC_SYS_MLDO_SLP(x)          (((u32)((x) & 0x00000003) << 26))
#define PMC_GET_SYS_MLDO_SLP(x)      ((u32)(((x >> 26) & 0x00000003)))
#define PMC_MASK_SYS_MLDO_NORM       ((u32)0x00000003 << 24)           /*!< R/W 2'b10  mem ldo mode normal option  00:off 01:sleep (wo dummy) 10:normal-0 (wi dummy) 10:normal-1 (wo dummy) DD_SEC: bpc_pmc*/
#define PMC_SYS_MLDO_NORM(x)         (((u32)((x) & 0x00000003) << 24))
#define PMC_GET_SYS_MLDO_NORM(x)     ((u32)(((x >> 24) & 0x00000003)))
#define PMC_MASK_SYS_XTAL_SLP        ((u32)0x00000007 << 20)           /*!< R/W 3'b001  00: OFF    001: LPS wo 40M   010: LPS with 40M , 011: NORM     100: HP DD_SEC: bpc_pmc*/
#define PMC_SYS_XTAL_SLP(x)          (((u32)((x) & 0x00000007) << 20))
#define PMC_GET_SYS_XTAL_SLP(x)      ((u32)(((x >> 20) & 0x00000007)))
#define PMC_MASK_SYS_XTAL_NORM       ((u32)0x00000007 << 17)           /*!< R/W 3'b011  00: OFF    001: LPS wo 40M   010: LPS with 40M , 011: NORM     100: HP DD_SEC: bpc_pmc*/
#define PMC_SYS_XTAL_NORM(x)         (((u32)((x) & 0x00000007) << 17))
#define PMC_GET_SYS_XTAL_NORM(x)     ((u32)(((x >> 17) & 0x00000007)))
#define PMC_BIT_SYS_MLDO_RDY         ((u32)0x00000001 << 16)           /*!< R 0x0  Ready signal for system mldo req. Software write trg register and sts different value will lead this bit set to 0 by hardware. When HW finish the flow, this bit will be set to 1 by hardware. DD_SEC: bpc_pmc*/
#define PMC_BIT_SYS_XMD_RDY          ((u32)0x00000001 << 15)           /*!< R 0x0  Ready signal for system xmd state. Software write trg register and sts different value will lead this bit set to 0 by hardware. When HW finish the flow, this bit will be set to 1 by hardware. DD_SEC: bpc_pmc*/
#define PMC_BIT_SYS_REGU_STS_RDY     ((u32)0x00000001 << 14)           /*!< R 0x0  Ready signal for system regu state. Software write trg register and sts different value will lead this bit set to 0 by hardware. When REGU PMC finish the flow, this bit will be set to 1 by hardware. DD_SEC: bpc_pmc*/
#define PMC_MASK_SYS_REGU_STS_SLP    ((u32)0x00000007 << 11)           /*!< R/W 3'b010  000: RSVD     001: state1   010: state2   011: state3 100: state4    101: state5   110: state6   111: RSVD DD_SEC: bpc_pmc*/
#define PMC_SYS_REGU_STS_SLP(x)      (((u32)((x) & 0x00000007) << 11))
#define PMC_GET_SYS_REGU_STS_SLP(x)  ((u32)(((x >> 11) & 0x00000007)))
#define PMC_MASK_SYS_REGU_STS_NORM   ((u32)0x00000007 << 8)            /*!< R/W 3'b110  000: RSVD      001: state1  0.7/PC/PFM 010: state2  0.8/PC/PFM  011: state3  0.9/PC/PFM 100: state4  0.9(1.0)/SLP/PFM   101: state5  0.9(1.0)/SLP/PWM  110: state6  0.9(1.0)/NORM/PWM  111: RSVD DD_SEC: bpc_pmc*/
#define PMC_SYS_REGU_STS_NORM(x)     (((u32)((x) & 0x00000007) << 8))
#define PMC_GET_SYS_REGU_STS_NORM(x) ((u32)(((x >> 8) & 0x00000007)))
#define PMC_BIT_TRG_SYS_MLDO         ((u32)0x00000001 << 3)            /*!< WA0 0x0   DD_SEC: bpc_pmc*/
#define PMC_BIT_TRG_SYS_XTAL         ((u32)0x00000001 << 1)            /*!< WA0 0x0   DD_SEC: bpc_pmc*/
#define PMC_BIT_TRG_SYS_REGU_STS     ((u32)0x00000001 << 0)            /*!< WA0 0x0  write 1 to trigger sys_regu_sts_norm value to take effect DD_SEC: bpc_pmc*/
/** @} */

/** @defgroup ARB_DBC0
  * @brief
  * @{
  */
#define PMC_MASK_DBC_CNT_XMD        ((u32)0x00000FFF << 17)           /*!< R/W 12'd10  xmd debounce counter , cycle based in OSC4M DD_SEC: S*/
#define PMC_DBC_CNT_XMD(x)          (((u32)((x) & 0x00000FFF) << 17))
#define PMC_GET_DBC_CNT_XMD(x)      ((u32)(((x >> 17) & 0x00000FFF)))
#define PMC_BIT_DBC_EN_XMD          ((u32)0x00000001 << 16)           /*!< R/W 0x1  xtal mode arbit debounce enable 0:disable 1:enable DD_SEC: S*/
#define PMC_MASK_DBC_CNT_REGU_STS   ((u32)0x00000FFF << 1)            /*!< R/W 12'd199  regu sts debounce counter , cycle based in OSC4M for 4M sync to 100K, it must wait 40us for async handshake DD_SEC: S*/
#define PMC_DBC_CNT_REGU_STS(x)     (((u32)((x) & 0x00000FFF) << 1))
#define PMC_GET_DBC_CNT_REGU_STS(x) ((u32)(((x >> 1) & 0x00000FFF)))
#define PMC_BIT_DBC_EN_REGU_STS     ((u32)0x00000001 << 0)            /*!< R/W 0x1  regu sts arbit debounce enable 0:disable 1:enable DD_SEC: S*/
/** @} */

/** @defgroup ARB_DBC1
  * @brief
  * @{
  */
#define PMC_MASK_DBC_CNT_MLDO   ((u32)0x00000FFF << 1)           /*!< R/W 12'd199  mldo debounce counter , cycle based in OSC4M for 4M sync to 100K, it must wait 40us for async handshake DD_SEC: S*/
#define PMC_DBC_CNT_MLDO(x)     (((u32)((x) & 0x00000FFF) << 1))
#define PMC_GET_DBC_CNT_MLDO(x) ((u32)(((x >> 1) & 0x00000FFF)))
#define PMC_BIT_DBC_EN_MLDO     ((u32)0x00000001 << 0)           /*!< R/W 0x1  mldo arbit debounce enable 0:disable 1:enable DD_SEC: S*/
/** @} */

/** @defgroup ARB_VTH
  * @brief
  * @{
  */
#define PMC_MASK_IAGT_VTHD_XMD        ((u32)0x00007FFF << 16)           /*!< R/W 0x0  This field used for xmd arb control. DD_SEC: S*/
#define PMC_IAGT_VTHD_XMD(x)          (((u32)((x) & 0x00007FFF) << 16))
#define PMC_GET_IAGT_VTHD_XMD(x)      ((u32)(((x >> 16) & 0x00007FFF)))
#define PMC_MASK_IAGT_VTHD_MLDO       ((u32)0x0000003F << 9)            /*!< R/W 0x0  This field used for mldo arb control. DD_SEC: S*/
#define PMC_IAGT_VTHD_MLDO(x)         (((u32)((x) & 0x0000003F) << 9))
#define PMC_GET_IAGT_VTHD_MLDO(x)     ((u32)(((x >> 9) & 0x0000003F)))
#define PMC_MASK_IAGT_VTHD_REGU_STS   ((u32)0x000001FF << 0)            /*!< R/W 0x0  This field used for regu sts arb control. DD_SEC: S*/
#define PMC_IAGT_VTHD_REGU_STS(x)     (((u32)((x) & 0x000001FF) << 0))
#define PMC_GET_IAGT_VTHD_REGU_STS(x) ((u32)(((x >> 0) & 0x000001FF)))
/** @} */

/** @defgroup AON_BUS_ST
  * @brief
  * @{
  */
#define PMC_BIT_RTC_RESP_TIMEOUT ((u32)0x00000001 << 4) /*!< RW1C 0x0  This flag indicate system access rtc domain timeout , access maybe failed. Active 1. Write 1 will clear the status. DD_SEC: S*/
#define PMC_BIT_AON_RESP_TIMEOUT ((u32)0x00000001 << 0) /*!< RW1C 0x0  This flag indicate system access aon domain timeout , access maybe failed. Active 1. Write 1 will clear the status. DD_SEC: S*/
/** @} */

/** @defgroup CLK_FORCE
  * @brief
  * @{
  */
#define PMC_BIT_SYS_PLL_DIV3_FORCE ((u32)0x00000001 << 2) /*!< R/W 0x0  force to enable sys pll div3 1:sys pll div3 clock always enable 0:sys pll div3 clock always auto gate when no IP use it DD_SEC: S*/
#define PMC_BIT_USB_PLL_DIV3_FORCE ((u32)0x00000001 << 1) /*!< R/W 0x0  force to enable usb pll div3 1:usb pll div3 clock always enable 0:usb pll div3 clock always auto gate when no IP use it DD_SEC: S*/
#define PMC_BIT_OSC4M_REQ_FORCE    ((u32)0x00000001 << 0) /*!< R/W 0x0  force to request osc4m 1:osc4m will never shutdown 0:osc4m will auto shutdown when no IP use it DD_SEC: S*/
/** @} */

/*==========PMC Register Address Definition==========*/
#define WAK_STATUS0               0x000
#define WAK_STATUS1               0x004
#define WAK_MASK0_NP              0x008
#define WAK_MASK1_NP              0x00C
#define WAK_MASK0_AP              0x010
#define WAK_MASK1_AP              0x014
#define WAK_BT_STATUS             0x018
#define WAK_MASK0_BT              0x01C
#define WLPMC_ISR                 0x020
#define WLAN_PWRCTL               0x024
#define SYSPMC_CTRL               0x028
#define SYSPMC_OPT                0x02C
#define DUMMY_LOAD_CTRL           0x030
#define PMC_PATCH_GRP0_0          0x034
#define PMC_PATCH_GRP0_1          0x038
#define PMC_PATCH_GRP1_0          0x03C
#define PMC_PATCH_GRP1_1          0x040
#define PMC_PATCH_GRP2_0          0x044
#define PMC_PATCH_GRP2_1          0x048
#define PMC_PATCH_GRP3_0          0x04C
#define PMC_PATCH_GRP3_1          0x050
#define PMC_PATCH_GRP4_0          0x054
#define PMC_PATCH_GRP4_1          0x058
#define PMC_PATCH_GRP5_0          0x05C
#define PMC_PATCH_GRP5_1          0x060
#define PMC_DBG_FLAG              0x064
#define PMC_ADDR45                0x068
#define PMC_ADDR67                0x06C
#define AIP_TRIGGER               0x070
#define PLL_ARB                   0x074
#define ARB_DBC0                  0x080
#define ARB_DBC1                  0x084
#define ARB_VTH                   0x088
#define AON_BUS_ST                0x08C
#define CLK_FORCE                 0x090

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* AUTO_GEN_WAKE_SRC_START */

/**************************************************************************//**
* @defgroup WAK_STATUS0/WAK_MASK0_LP/WAK_MASK0_NP/WAK_MASK0_AP
* @brief
* @{
*****************************************************************************/
#define WAKE_SRC_WIFI_FISR_FESR_IRQ                  ((u32)0x00000001 << 0)
#define WAKE_SRC_WIFI_FTSR_MAILBOX_IRQ               ((u32)0x00000001 << 1)
#define WAKE_SRC_AP_WAKE_IRQ                         ((u32)0x00000001 << 2)
#define WAKE_SRC_IPC_CPU1                            ((u32)0x00000001 << 3)
#define WAKE_SRC_IPC_CPU0                            ((u32)0x00000001 << 4)
#define WAKE_SRC_IWDG                                ((u32)0x00000001 << 5)
#define WAKE_SRC_Timer0                              ((u32)0x00000001 << 6)
#define WAKE_SRC_Timer1                              ((u32)0x00000001 << 7)
#define WAKE_SRC_Timer2                              ((u32)0x00000001 << 8)
#define WAKE_SRC_Timer3                              ((u32)0x00000001 << 9)
#define WAKE_SRC_pmc_timer0                          ((u32)0x00000001 << 10)
#define WAKE_SRC_pmc_timer1                          ((u32)0x00000001 << 11)
#define WAKE_SRC_UART0                               ((u32)0x00000001 << 12)
#define WAKE_SRC_UART1                               ((u32)0x00000001 << 13)
#define WAKE_SRC_UART2                               ((u32)0x00000001 << 14)
#define WAKE_SRC_UART3                               ((u32)0x00000001 << 15)
#define WAKE_SRC_UART_LOG                            ((u32)0x00000001 << 16)
#define WAKE_SRC_GPIOA                               ((u32)0x00000001 << 17)
#define WAKE_SRC_GPIOB                               ((u32)0x00000001 << 18)
#define WAKE_SRC_GPIOC                               ((u32)0x00000001 << 19)
#define WAKE_SRC_RTC                                 ((u32)0x00000001 << 22)
#define WAKE_SRC_ADC                                 ((u32)0x00000001 << 23)
#define WAKE_SRC_CAPTOUCH                            ((u32)0x00000001 << 25)
#define WAKE_SRC_BOR                                 ((u32)0x00000001 << 26)
#define WAKE_SRC_PWR_DOWN                            ((u32)0x00000001 << 27)
#define WAKE_SRC_RMII                                ((u32)0x00000001 << 28)
#define WAKE_SRC_AON_TIM                             ((u32)0x00000001 << 29)
/** @} */

/**************************************************************************//**
* @defgroup WAK_STATUS1/WAK_MASK1_LP/WAK_MASK1_NP/WAK_MASK1_AP
* @brief
* @{
*****************************************************************************/
#define WAKE_SRC_AON_WAKEPIN                         ((u32)(3 << 30) | (0x00000001 << 0))
#define WAKE_SRC_SDIO_WIFI                           ((u32)(3 << 30) | (0x00000001 << 1))
#define WAKE_SRC_SDIO_BT                             ((u32)(3 << 30) | (0x00000001 << 2))
#define WAKE_SRC_SDIO_HOST                           ((u32)(3 << 30) | (0x00000001 << 3))
#define WAKE_SRC_USB                                 ((u32)(3 << 30) | (0x00000001 << 4))
#define WAKE_SRC_CAN0                                ((u32)(3 << 30) | (0x00000001 << 5))
#define WAKE_SRC_CAN1                                ((u32)(3 << 30) | (0x00000001 << 6))
#define WAKE_SRC_BT_SCB                              ((u32)(3 << 30) | (0x00000001 << 7))
#define WAKE_SRC_BT_WAKE_HOST                        ((u32)(3 << 30) | (0x00000001 << 8))
/** @} */

/* AUTO_GEN_WAKE_SRC_END */

/* MANUAL_GEN_START */

//Please add your defination here
/** @defgroup PowerCut_Stable_Time_define
  * @{
  */
enum Power_State {
	STATE1_LDOPC_SWRPFM_07 = 0x1,
	STATE2_LDOPC_SWRPFM_08 = 0x2,
	STATE3_LDOPC_SWRPFM_09 = 0x3,
	STATE4_LDOSLP_SWRPFM   = 0x4,
	STATE5_LDONORM_SWRPFM  = 0x5,
	STATE6_LDONORM_SWRPWM  = 0x6,
};
/** @} */

/** @defgroup PowerCut_Stable_Time_define
  * @{
  */
enum PowerCut_Stable_Time {
	PC_Stable_16 = 0x00,
	PC_Stable_32 = 0x01,
	PC_Stable_64 = 0x02,
	PC_Stable_128 = 0x03,
	PC_Stable_256 = 0x04,
	PC_Stable_512 = 0x05,
	PC_Stable_1024 = 0x06,
	PC_Stable_2048 = 0x07,
};
/** @} */

/* MANUAL_GEN_END */

#endif

/** @} */

/** @} */
