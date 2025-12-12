/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SYSREG_PMC_H_
#define _SYSREG_PMC_H_

/* AUTO_GEN_START */

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup PMC
* @brief PMC driver modules
* @{
*/

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup PMC_Register_Definitions PMC Register Definitions
 * @{
 *****************************************************************************/

/** @defgroup WAK_STATUS0
 * @brief
 * @{
 **/
#define PMC_MASK_WAK_STA_31_0             ((u32)0xFFFFFFFF << 0)          /*!<R 0  wak evt status. It can't be cleare here, clear at IP side . Bit0: WIFI_FISR_FESR_IRQ Bit1: WIFI_FTSR_MAILBOX_IRQ Bit2: KM4_WAKE_IRQ Bit3: BT_WAKE_HOST Bit4: IPC_KM4 Bit5: IWDG0 Bit6: Timer4 Bit7: Timer5 Bit8: Timer6 Bit9: Timer7 Bit10: pmc_timer0 Bit11: pmc_timer1 Bit12: UART0 Bit13: UART1 Bit14: UART2_BT Bit15: UART_LOG Bit16: GPIOA Bit17: GPIOB Bit18: I2C0 Bit19: I2C1 Bit20: CTOUCH Bit21: RTC Bit22: ADC Bit23: ADC_COMP Bit24: BOR Bit25: PWR_DOWN Bit26: Keyscan Bit27: AON_TIM Bit28: AON_WAKEPIN Bit29: SDIO Bit30~31: Reserved */
#define PMC_WAK_STA_31_0(x)               (((u32)((x) & 0xFFFFFFFF) << 0))
#define PMC_GET_WAK_STA_31_0(x)           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup WAK_MASK0_NP
 * @brief
 * @{
 **/
#define PMC_MASK_WAK_KM0_IMR_31_0         ((u32)0xFFFFFFFF << 0)          /*!<R/W 0  1:mask enable for wake NP, this used to wak pmc */
#define PMC_WAK_KM0_IMR_31_0(x)           (((u32)((x) & 0xFFFFFFFF) << 0))
#define PMC_GET_WAK_KM0_IMR_31_0(x)       ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup WAK_MASK0_AP
 * @brief
 * @{
 **/
#define PMC_MASK_WAK_KM4_IMR_31_0         ((u32)0xFFFFFFFF << 0)          /*!<R/W 0  1:mask enable for wake AP */
#define PMC_WAK_KM4_IMR_31_0(x)           (((u32)((x) & 0xFFFFFFFF) << 0))
#define PMC_GET_WAK_KM4_IMR_31_0(x)       ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup WLPMC_ISR
 * @brief
 * @{
 **/
#define PMC_BIT_WL_OFF_IMR                ((u32)0x00000001 << 17)          /*!<R/W 0   */
#define PMC_BIT_WL_ON_IMR                 ((u32)0x00000001 << 16)          /*!<R/W 0   */
#define PMC_BIT_WL_OFF_ISR                ((u32)0x00000001 << 1)          /*!<RW1CB 0   */
#define PMC_BIT_WL_ON_ISR                 ((u32)0x00000001 << 0)          /*!<RW1CB 0   */
/** @} */

/** @defgroup WLAN_PWRCTL
 * @brief
 * @{
 **/
#define PMC_BIT_SW_CLR_RDY2ACT            ((u32)0x00000001 << 17)          /*!<R/WA0 0  System infrom WLAN ready to active */
#define PMC_BIT_SW_SET_RDY2ACT            ((u32)0x00000001 << 16)          /*!<R/WA0 0   */
#define PMC_BIT_WL_RDY2ACT                ((u32)0x00000001 << 9)          /*!<R 0  System infrom WLAN ready to active */
#define PMC_BIT_WL_REQ2ACT                ((u32)0x00000001 << 8)          /*!<R 0   */
#define PMC_BIT_WL_HW_RESP_FEN            ((u32)0x00000001 << 0)          /*!<R/W 1  1: enable HW auto response */
/** @} */

/** @defgroup SYSPMC_CTRL
 * @brief
 * @{
 **/
#define PMC_MASK_STL_PDSOC                ((u32)0x00000007 << 24)          /*!<R/W 0x6  timer wait for core power power-cut power stable . 000:8us * 2^1 001:8us * 2^2 010:8us * 2^3 // 64us 011:8us * 2^4 100:8us * 2^5 101:8us * 2^6 110:8us * 2^7 // pon default 111:8us * 2^8 */
#define PMC_STL_PDSOC(x)                  (((u32)((x) & 0x00000007) << 24))
#define PMC_GET_STL_PDSOC(x)              ((u32)(((x >> 24) & 0x00000007)))
#define PMC_MASK_TUTIME                   ((u32)0x0000003F << 16)          /*!<R/W 0x1F  PMC timeunit setting, in cycles of osc4M e.g, default 0x1F, timeunit = 8us */
#define PMC_TUTIME(x)                     (((u32)((x) & 0x0000003F) << 16))
#define PMC_GET_TUTIME(x)                 ((u32)(((x >> 16) & 0x0000003F)))
#define PMC_BIT_MEM_SLP_MODE              ((u32)0x00000001 << 5)          /*!<R/W 1  Select memory parameter option 1: sleep setting 0: normal setting Note:defautl set to sleep setting to save power when snooze */
#define PMC_BIT_POF_WAIT_SLEP             ((u32)0x00000001 << 4)          /*!<R/W 1  1.when SW set pmen_slep or pmen_dslp , hw pmc will wait km0 enter sleep mode , then begin to pof seq 0.Not wait km0 sleep */
#define PMC_BIT_KM0_IRQ_MASK              ((u32)0x00000001 << 3)          /*!<R/W 0  1.mask all KM0 interrupt 0:release all KM0 interrupt */
#define PMC_BIT_KM4_IRQ_MASK              ((u32)0x00000001 << 2)          /*!<R/W 0  1.mask all KM4 interrupt 0:release all KM4 interrupt */
#define PMC_BIT_PMEN_SLEP                 ((u32)0x00000001 << 1)          /*!<R/W 0  1: Request PMC to enter sleep state, auto clear to 0 after PMC finishes */
#define PMC_BIT_PMEN_DSLP                 ((u32)0x00000001 << 0)          /*!<R/W 0  1: Request PMC to enter deep-sleep state, auto clear to 0 after PMC finishes */
/** @} */

/** @defgroup SYSPMC_OPT
 * @brief
 * @{
 **/
#define PMC_BIT_PST_CHG_BDML              ((u32)0x00000001 << 26)          /*!<R/W 1  normal setting for BT request Core LDO dummy load selection 1: enable dummy load during BT PMC change to sleep mode / normal mode 0: disable dummy load during BT PMC change to sleep mode / normal mode */
#define PMC_BIT_PST_G1_USE_MLDO           ((u32)0x00000001 << 25)          /*!<R/W 1  1: G1 pad use MLDO 1.8v IO power 0: G1 pad use external power and can't shutdown by pmc */
#define PMC_BIT_PST_G0_USE_MLDO           ((u32)0x00000001 << 24)          /*!<R/W 1  1: G0 pad use MLDO 1.8v IO power 0: G0 pad use external power and can't shutdown by pmc */
#define PMC_BIT_PST_SLEP_WDML             ((u32)0x00000001 << 23)          /*!<R/W 0  normal setting for WMAC request Core LDO dummy load selection 1: enable dummy load when PMC enter into sleep mode 0: disable dummy load when PMC enter into sleep mode */
#define PMC_BIT_PST_SLEP_BDML             ((u32)0x00000001 << 22)          /*!<R/W 0  normal setting for BT request Core LDO dummy load selection 1: enable dummy load when PMC enter into sleep mode 0: disable dummy load when PMC enter into sleep mode */
#define PMC_BIT_PST_SLEP_SDML             ((u32)0x00000001 << 21)          /*!<R/W 0  normal setting for system request Core LDO dummy load selection 1: enable dummy load when PMC enter into sleep mode 0: disable dummy load when PMC enter into sleep mode */
#define PMC_BIT_PST_NORM_WDML             ((u32)0x00000001 << 20)          /*!<R/W 0  normal setting for WMAC request Core LDO dummy load selection 1: enable dummy load when PMC enter into normal mode 0: disable dummy load when PMC enter into normal mode */
#define PMC_BIT_PST_NORM_BDML             ((u32)0x00000001 << 19)          /*!<R/W 0  normal setting for BT request Core LDO dummy load selection 1: enable dummy load when PMC enter into normal mode 0: disable dummy load when PMC enter into normal mode */
#define PMC_BIT_PST_NORM_SDML             ((u32)0x00000001 << 18)          /*!<R/W 0  normal setting for system request Core LDO dummy load selection 1: enable dummy load when PMC enter into normal mode 0: disable dummy load when PMC enter into normal mode */
#define PMC_BIT_PST_SLEP_G1PD             ((u32)0x00000001 << 7)          /*!<R/W 1  1: G1 pad used for 1.8v IO power , and will shutdown when LDO3318 off in slep, when dslp , it will always shutoff 0: G1 pad not used as 1.8v IO power */
#define PMC_BIT_PST_SLEP_G0PD             ((u32)0x00000001 << 6)          /*!<R/W 1  1: G0 pad used for 1.8v IO power , and will shutdown when LDO3318 off in slep,, when dslp , it will always shutoff 0: G0 pad not used as 1.8v IO power */
#define PMC_BIT_PST_SLEP_ERCK             ((u32)0x00000001 << 3)          /*!<R/W 0  1: Enable lpon clock when PMC enter into sleep mode 0: Gate lpon clock when PMC enter into sleep mode */
#define PMC_BIT_PST_SLEP_ESOC             ((u32)0x00000001 << 0)          /*!<R/W 0  1: power enable SOC platform when PMC enter into sleep mode (clock-gating) 0: power off SoC domain when PMC enter into sleep mode (power-gating) */
/** @} */

/** @defgroup DUMMY_LOAD_CTRL
 * @brief
 * @{
 **/
#define PMC_BIT_SYS_DMYL_SLP              ((u32)0x00000001 << 2)          /*!<R/W 1  this bit means the dummy load state when KM4 is sleep 0: disable 1: enable */
#define PMC_BIT_SYS_DMYL_ACT              ((u32)0x00000001 << 1)          /*!<R/W 0  this bit means the dummy load state when KM4 is active 0: disable 1: enable */
#define PMC_BIT_SYS_DMYL_MNL              ((u32)0x00000001 << 0)          /*!<R/W 0  system use this bit to switch core ldo dummy load control path manually 0: dummy load control is decided by LS/BT/WL PMC arbitration 1: dummy load control is decided by KM4 sleep state */
/** @} */

/** @defgroup PMC_PATCH_GRP0_0
 * @brief
 * @{
 **/
#define PMC_MASK_PATCH_GRP0_INSTR1_5_0    ((u32)0x0000003F << 26)          /*!<R/W 0  instruction1 of LSYSON PMC patch group0,bit[5:0] */
#define PMC_PATCH_GRP0_INSTR1_5_0(x)      (((u32)((x) & 0x0000003F) << 26))
#define PMC_GET_PATCH_GRP0_INSTR1_5_0(x)  ((u32)(((x >> 26) & 0x0000003F)))
#define PMC_MASK_PATCH_GRP0_INSTR0        ((u32)0x0003FFFF << 8)          /*!<R/W 0  instruction0 of LSYSON PMC patch group0 */
#define PMC_PATCH_GRP0_INSTR0(x)          (((u32)((x) & 0x0003FFFF) << 8))
#define PMC_GET_PATCH_GRP0_INSTR0(x)      ((u32)(((x >> 8) & 0x0003FFFF)))
#define PMC_MASK_PATCH_GRP0_SADDR         ((u32)0x000000FF << 0)          /*!<R/W 0  Start address of the first instruction of LSYSON PMC patch group0 */
#define PMC_PATCH_GRP0_SADDR(x)           (((u32)((x) & 0x000000FF) << 0))
#define PMC_GET_PATCH_GRP0_SADDR(x)       ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup PMC_PATCH_GRP0_1
 * @brief
 * @{
 **/
#define PMC_MASK_PATCH_GRP0_INSTR2        ((u32)0x0003FFFF << 12)          /*!<R/W 0  instruction2 of LSYSON PMC patch group0 */
#define PMC_PATCH_GRP0_INSTR2(x)          (((u32)((x) & 0x0003FFFF) << 12))
#define PMC_GET_PATCH_GRP0_INSTR2(x)      ((u32)(((x >> 12) & 0x0003FFFF)))
#define PMC_MASK_PATCH_GRP0_INSTR1_17_6   ((u32)0x00000FFF << 0)          /*!<R/W 0  instruction1 of LSYSON PMC patch group0,bit[17:6] */
#define PMC_PATCH_GRP0_INSTR1_17_6(x)     (((u32)((x) & 0x00000FFF) << 0))
#define PMC_GET_PATCH_GRP0_INSTR1_17_6(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup PMC_PATCH_GRP1_0
 * @brief
 * @{
 **/
#define PMC_MASK_PATCH_GRP1_INSTR1_5_0    ((u32)0x0000003F << 26)          /*!<R/W 0  instruction1 of LSYSON PMC patch group1,bit[5:0] */
#define PMC_PATCH_GRP1_INSTR1_5_0(x)      (((u32)((x) & 0x0000003F) << 26))
#define PMC_GET_PATCH_GRP1_INSTR1_5_0(x)  ((u32)(((x >> 26) & 0x0000003F)))
#define PMC_MASK_PATCH_GRP1_INSTR0        ((u32)0x0003FFFF << 8)          /*!<R/W 0  instruction0 of LSYSON PMC patch group1 */
#define PMC_PATCH_GRP1_INSTR0(x)          (((u32)((x) & 0x0003FFFF) << 8))
#define PMC_GET_PATCH_GRP1_INSTR0(x)      ((u32)(((x >> 8) & 0x0003FFFF)))
#define PMC_MASK_PATCH_GRP1_SADDR         ((u32)0x000000FF << 0)          /*!<R/W 0  Start address of the first instruction of LSYSON PMC patch group1 */
#define PMC_PATCH_GRP1_SADDR(x)           (((u32)((x) & 0x000000FF) << 0))
#define PMC_GET_PATCH_GRP1_SADDR(x)       ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup PMC_PATCH_GRP1_1
 * @brief
 * @{
 **/
#define PMC_MASK_PATCH_GRP1_INSTR2        ((u32)0x0003FFFF << 12)          /*!<R/W 0  instruction2 of LSYSON PMC patch group1 */
#define PMC_PATCH_GRP1_INSTR2(x)          (((u32)((x) & 0x0003FFFF) << 12))
#define PMC_GET_PATCH_GRP1_INSTR2(x)      ((u32)(((x >> 12) & 0x0003FFFF)))
#define PMC_MASK_PATCH_GRP1_INSTR1_17_6   ((u32)0x00000FFF << 0)          /*!<R/W 0  instruction1 of LSYSON PMC patch group1,bit[17:6] */
#define PMC_PATCH_GRP1_INSTR1_17_6(x)     (((u32)((x) & 0x00000FFF) << 0))
#define PMC_GET_PATCH_GRP1_INSTR1_17_6(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup PMC_PATCH_GRP2_0
 * @brief
 * @{
 **/
#define PMC_MASK_PATCH_GRP2_INSTR1_5_0    ((u32)0x0000003F << 26)          /*!<R/W 0  instruction1 of LSYSON PMC patch group2,bit[5:0] */
#define PMC_PATCH_GRP2_INSTR1_5_0(x)      (((u32)((x) & 0x0000003F) << 26))
#define PMC_GET_PATCH_GRP2_INSTR1_5_0(x)  ((u32)(((x >> 26) & 0x0000003F)))
#define PMC_MASK_PATCH_GRP2_INSTR0        ((u32)0x0003FFFF << 8)          /*!<R/W 0  instruction0 of LSYSON PMC patch group2 */
#define PMC_PATCH_GRP2_INSTR0(x)          (((u32)((x) & 0x0003FFFF) << 8))
#define PMC_GET_PATCH_GRP2_INSTR0(x)      ((u32)(((x >> 8) & 0x0003FFFF)))
#define PMC_MASK_PATCH_GRP2_SADDR         ((u32)0x000000FF << 0)          /*!<R/W 0  Start address of the first instruction of LSYSON PMC patch group2 */
#define PMC_PATCH_GRP2_SADDR(x)           (((u32)((x) & 0x000000FF) << 0))
#define PMC_GET_PATCH_GRP2_SADDR(x)       ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup PMC_PATCH_GRP2_1
 * @brief
 * @{
 **/
#define PMC_MASK_PATCH_GRP2_INSTR2        ((u32)0x0003FFFF << 12)          /*!<R/W 0  instruction2 of LSYSON PMC patch group2 */
#define PMC_PATCH_GRP2_INSTR2(x)          (((u32)((x) & 0x0003FFFF) << 12))
#define PMC_GET_PATCH_GRP2_INSTR2(x)      ((u32)(((x >> 12) & 0x0003FFFF)))
#define PMC_MASK_PATCH_GRP2_INSTR1_17_6   ((u32)0x00000FFF << 0)          /*!<R/W 0  instruction1 of LSYSON PMC patch group2,bit[17:6] */
#define PMC_PATCH_GRP2_INSTR1_17_6(x)     (((u32)((x) & 0x00000FFF) << 0))
#define PMC_GET_PATCH_GRP2_INSTR1_17_6(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup PMC_PATCH_GRP3_0
 * @brief
 * @{
 **/
#define PMC_MASK_PATCH_GRP3_INSTR1_5_0    ((u32)0x0000003F << 26)          /*!<R/W 0  instruction1 of LSYSON PMC patch group3,bit[5:0] */
#define PMC_PATCH_GRP3_INSTR1_5_0(x)      (((u32)((x) & 0x0000003F) << 26))
#define PMC_GET_PATCH_GRP3_INSTR1_5_0(x)  ((u32)(((x >> 26) & 0x0000003F)))
#define PMC_MASK_PATCH_GRP3_INSTR0        ((u32)0x0003FFFF << 8)          /*!<R/W 0  instruction0 of LSYSON PMC patch group3 */
#define PMC_PATCH_GRP3_INSTR0(x)          (((u32)((x) & 0x0003FFFF) << 8))
#define PMC_GET_PATCH_GRP3_INSTR0(x)      ((u32)(((x >> 8) & 0x0003FFFF)))
#define PMC_MASK_PATCH_GRP3_SADDR         ((u32)0x000000FF << 0)          /*!<R/W 0  Start address of the first instruction of LSYSON PMC patch group3 */
#define PMC_PATCH_GRP3_SADDR(x)           (((u32)((x) & 0x000000FF) << 0))
#define PMC_GET_PATCH_GRP3_SADDR(x)       ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup PMC_PATCH_GRP3_1
 * @brief
 * @{
 **/
#define PMC_MASK_PATCH_GRP3_INSTR2        ((u32)0x0003FFFF << 12)          /*!<R/W 0  instruction2 of LSYSON PMC patch group3 */
#define PMC_PATCH_GRP3_INSTR2(x)          (((u32)((x) & 0x0003FFFF) << 12))
#define PMC_GET_PATCH_GRP3_INSTR2(x)      ((u32)(((x >> 12) & 0x0003FFFF)))
#define PMC_MASK_PATCH_GRP3_INSTR1_17_6   ((u32)0x00000FFF << 0)          /*!<R/W 0  instruction1 of LSYSON PMC patch group3,bit[17:6] */
#define PMC_PATCH_GRP3_INSTR1_17_6(x)     (((u32)((x) & 0x00000FFF) << 0))
#define PMC_GET_PATCH_GRP3_INSTR1_17_6(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup PMC_PATCH_GRP4_0
 * @brief
 * @{
 **/
#define PMC_MASK_PATCH_GRP4_INSTR1_5_0    ((u32)0x0000003F << 26)          /*!<R/W 0  instruction1 of LSYSON PMC patch group4,bit[5:0] */
#define PMC_PATCH_GRP4_INSTR1_5_0(x)      (((u32)((x) & 0x0000003F) << 26))
#define PMC_GET_PATCH_GRP4_INSTR1_5_0(x)  ((u32)(((x >> 26) & 0x0000003F)))
#define PMC_MASK_PATCH_GRP4_INSTR0        ((u32)0x0003FFFF << 8)          /*!<R/W 0  instruction0 of LSYSON PMC patch group4 */
#define PMC_PATCH_GRP4_INSTR0(x)          (((u32)((x) & 0x0003FFFF) << 8))
#define PMC_GET_PATCH_GRP4_INSTR0(x)      ((u32)(((x >> 8) & 0x0003FFFF)))
#define PMC_MASK_PATCH_GRP4_SADDR         ((u32)0x000000FF << 0)          /*!<R/W 0  Start address of the first instruction of LSYSON PMC patch group4 */
#define PMC_PATCH_GRP4_SADDR(x)           (((u32)((x) & 0x000000FF) << 0))
#define PMC_GET_PATCH_GRP4_SADDR(x)       ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup PMC_PATCH_GRP4_1
 * @brief
 * @{
 **/
#define PMC_MASK_PATCH_GRP4_INSTR2        ((u32)0x0003FFFF << 12)          /*!<R/W 0  instruction2 of LSYSON PMC patch group4 */
#define PMC_PATCH_GRP4_INSTR2(x)          (((u32)((x) & 0x0003FFFF) << 12))
#define PMC_GET_PATCH_GRP4_INSTR2(x)      ((u32)(((x >> 12) & 0x0003FFFF)))
#define PMC_MASK_PATCH_GRP4_INSTR1_17_6   ((u32)0x00000FFF << 0)          /*!<R/W 0  instruction1 of LSYSON PMC patch group4,bit[17:6] */
#define PMC_PATCH_GRP4_INSTR1_17_6(x)     (((u32)((x) & 0x00000FFF) << 0))
#define PMC_GET_PATCH_GRP4_INSTR1_17_6(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup PMC_PATCH_GRP5_0
 * @brief
 * @{
 **/
#define PMC_MASK_PATCH_GRP5_INSTR1_5_0    ((u32)0x0000003F << 26)          /*!<R/W 0  instruction1 of LSYSON PMC patch group5,bit[5:0] */
#define PMC_PATCH_GRP5_INSTR1_5_0(x)      (((u32)((x) & 0x0000003F) << 26))
#define PMC_GET_PATCH_GRP5_INSTR1_5_0(x)  ((u32)(((x >> 26) & 0x0000003F)))
#define PMC_MASK_PATCH_GRP5_INSTR0        ((u32)0x0003FFFF << 8)          /*!<R/W 0  instruction0 of LSYSON PMC patch group5 */
#define PMC_PATCH_GRP5_INSTR0(x)          (((u32)((x) & 0x0003FFFF) << 8))
#define PMC_GET_PATCH_GRP5_INSTR0(x)      ((u32)(((x >> 8) & 0x0003FFFF)))
#define PMC_MASK_PATCH_GRP5_SADDR         ((u32)0x000000FF << 0)          /*!<R/W 0  Start address of the first instruction of LSYSON PMC patch group5 */
#define PMC_PATCH_GRP5_SADDR(x)           (((u32)((x) & 0x000000FF) << 0))
#define PMC_GET_PATCH_GRP5_SADDR(x)       ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup PMC_PATCH_GRP5_1
 * @brief
 * @{
 **/
#define PMC_MASK_PATCH_GRP5_INSTR2        ((u32)0x0003FFFF << 12)          /*!<R/W 0  instruction2 of LSYSON PMC patch group5 */
#define PMC_PATCH_GRP5_INSTR2(x)          (((u32)((x) & 0x0003FFFF) << 12))
#define PMC_GET_PATCH_GRP5_INSTR2(x)      ((u32)(((x >> 12) & 0x0003FFFF)))
#define PMC_MASK_PATCH_GRP5_INSTR1_17_6   ((u32)0x00000FFF << 0)          /*!<R/W 0  instruction1 of LSYSON PMC patch group5,bit[17:6] */
#define PMC_PATCH_GRP5_INSTR1_17_6(x)     (((u32)((x) & 0x00000FFF) << 0))
#define PMC_GET_PATCH_GRP5_INSTR1_17_6(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup AIP_TRIGGER
 * @brief
 * @{
 **/
#define PMC_BIT_SYS_PWM_H                 ((u32)0x00000001 << 30)          /*!<R/W 1'b0  Set pwm in heavy loading by system (SWR current > 200mA) 1:Heavy 0:Light */
#define PMC_MASK_SYS_MLDO_DML_SLP         ((u32)0x00000001 << 29)          /*!<R/W 1'b0  mem ldo dummy load slep option 1:dummy load en 0:dummy load disable */
#define PMC_SYS_MLDO_DML_SLP(x)           (((u32)((x) & 0x00000001) << 29))
#define PMC_GET_SYS_MLDO_DML_SLP(x)       ((u32)(((x >> 29) & 0x00000001)))
#define PMC_MASK_SYS_MLDO_DML_NORM        ((u32)0x00000001 << 28)          /*!<R/W 1'b1  mem ldo dummy load normal option 1:dummy load en 0:dummy load disable */
#define PMC_SYS_MLDO_DML_NORM(x)          (((u32)((x) & 0x00000001) << 28))
#define PMC_GET_SYS_MLDO_DML_NORM(x)      ((u32)(((x >> 28) & 0x00000001)))
#define PMC_MASK_SYS_MLDO_MODE_SLP        ((u32)0x00000003 << 26)          /*!<R/W 2'b00  mem ldo mode slep option 00:off 01:sleep 10:normal-0 10:normal-1 */
#define PMC_SYS_MLDO_MODE_SLP(x)          (((u32)((x) & 0x00000003) << 26))
#define PMC_GET_SYS_MLDO_MODE_SLP(x)      ((u32)(((x >> 26) & 0x00000003)))
#define PMC_MASK_SYS_MLDO_MODE_NORM       ((u32)0x00000003 << 24)          /*!<R/W 2'b10  mem ldo mode normal option 00:off 01:sleep 10:normal-0 10:normal-1 */
#define PMC_SYS_MLDO_MODE_NORM(x)         (((u32)((x) & 0x00000003) << 24))
#define PMC_GET_SYS_MLDO_MODE_NORM(x)     ((u32)(((x >> 24) & 0x00000003)))
#define PMC_BIT_SYS_BG_SLP                ((u32)0x00000001 << 23)          /*!<R/W 1'b0  0: BG OFF 1: BG ON */
#define PMC_BIT_SYS_BG_NORM               ((u32)0x00000001 << 22)          /*!<R/W 1'b1  0: BG OFF 1: BG ON */
#define PMC_MASK_SYS_XTAL_SLP             ((u32)0x00000007 << 19)          /*!<R/W 3'b001  00: OFF 001: LPS wo 40M 010: LPS with 40M , 011: NORM 100: HP */
#define PMC_SYS_XTAL_SLP(x)               (((u32)((x) & 0x00000007) << 19))
#define PMC_GET_SYS_XTAL_SLP(x)           ((u32)(((x >> 19) & 0x00000007)))
#define PMC_MASK_SYS_XTAL_NORM            ((u32)0x00000007 << 16)          /*!<R/W 3'b011  00: OFF 001: LPS wo 40M 010: LPS with 40M , 011: NORM 100: HP */
#define PMC_SYS_XTAL_NORM(x)              (((u32)((x) & 0x00000007) << 16))
#define PMC_GET_SYS_XTAL_NORM(x)          ((u32)(((x >> 16) & 0x00000007)))
#define PMC_BIT_SYS_REGU_STS_RDY          ((u32)0x00000001 << 14)          /*!<R 0  Ready signal for system regu state. Software write trg register and sts different value will lead this bit set to 0 by hardware. When REGU PMC finish the flow, this bit will be set to 1 by hardware. */
#define PMC_MASK_SYS_REGU_STS_SLP         ((u32)0x00000007 << 11)          /*!<R/W 3'b001  000: RSVD 001: state1 010: state2 011: state3 100: state4 101: state5 110: state6 111: RSVD */
#define PMC_SYS_REGU_STS_SLP(x)           (((u32)((x) & 0x00000007) << 11))
#define PMC_GET_SYS_REGU_STS_SLP(x)       ((u32)(((x >> 11) & 0x00000007)))
#define PMC_MASK_SYS_REGU_STS_NORM        ((u32)0x00000007 << 8)          /*!<R/W 3'b110  000: RSVD 001: state1 0.7/PC/PFM 010: state2 0.8/PC/PFM 011: state3 0.9/PC/PFM 100: state4 0.9(1.0)/SLP/PFM 101: state5 0.9(1.0)/SLP/PWM 110: state6 0.9(1.0)/NORM/PWM 111: RSVD */
#define PMC_SYS_REGU_STS_NORM(x)          (((u32)((x) & 0x00000007) << 8))
#define PMC_GET_SYS_REGU_STS_NORM(x)      ((u32)(((x >> 8) & 0x00000007)))
#define PMC_BIT_TRG_SYS_MLDO_DML          ((u32)0x00000001 << 4)          /*!<WA0 0   */
#define PMC_BIT_TRG_SYS_MLDO_MODE         ((u32)0x00000001 << 3)          /*!<WA0 0   */
#define PMC_BIT_TRG_SYS_BG                ((u32)0x00000001 << 2)          /*!<WA0 0   */
#define PMC_BIT_TRG_SYS_XTAL              ((u32)0x00000001 << 1)          /*!<WA0 0   */
#define PMC_BIT_TRG_SYS_REGU_STS          ((u32)0x00000001 << 0)          /*!<WA0 0  write 1 to trigger sys_regu_sts_norm value to take effect */
/** @} */

/** @defgroup AIP_OPTION
 * @brief
 * @{
 **/
#define PMC_BIT_OSC4M_IGN_VOLT            ((u32)0x00000001 << 0)          /*!<R/W 0  Options if osc4m can auto require voltage , disable it may speedup reponse time when system nevel goto 0.7v 0. osc4m will send volt areq when osc4m enable or disable 1. osc4m never req volt change , make sure , system voltage will nevel goto 0.7v */
/** @} */

/** @defgroup ARB_DBC
 * @brief
 * @{
 **/
#define PMC_MASK_DBC_CNT_XMD              ((u32)0x000000FF << 24)          /*!<R/W 8'd10  xmd debounce counter , cycle based in OSC4M */
#define PMC_DBC_CNT_XMD(x)                (((u32)((x) & 0x000000FF) << 24))
#define PMC_GET_DBC_CNT_XMD(x)            ((u32)(((x >> 24) & 0x000000FF)))
#define PMC_MASK_DBC_CNT_BG               ((u32)0x000000FF << 16)          /*!<R/W 8'd10  bg debounce counter , cycle based in OSC4M */
#define PMC_DBC_CNT_BG(x)                 (((u32)((x) & 0x000000FF) << 16))
#define PMC_GET_DBC_CNT_BG(x)             ((u32)(((x >> 16) & 0x000000FF)))
#define PMC_MASK_DBC_CNT_REGU_STS         ((u32)0x000000FF << 8)          /*!<R/W 8'd10  regu sts debounce counter , cycle based in OSC4M */
#define PMC_DBC_CNT_REGU_STS(x)           (((u32)((x) & 0x000000FF) << 8))
#define PMC_GET_DBC_CNT_REGU_STS(x)       ((u32)(((x >> 8) & 0x000000FF)))
#define PMC_BIT_DBC_EN_XMD                ((u32)0x00000001 << 2)          /*!<R/W 1  xtal mode arbit debounce enable 0:disable 1:enable */
#define PMC_BIT_DBC_EN_BG                 ((u32)0x00000001 << 1)          /*!<R/W 1  bg arbit debounce enable 0:disable 1:enable */
#define PMC_BIT_DBC_EN_REGU_STS           ((u32)0x00000001 << 0)          /*!<R/W 1  regu sts arbit debounce enable 0:disable 1:enable */
/** @} */

/** @defgroup ARB_VTH
 * @brief
 * @{
 **/
#define PMC_MASK_IAGT_VTHD_XMD            ((u32)0x00007FFF << 13)          /*!<R/W 0  This field used for xmd arb control. */
#define PMC_IAGT_VTHD_XMD(x)              (((u32)((x) & 0x00007FFF) << 13))
#define PMC_GET_IAGT_VTHD_XMD(x)          ((u32)(((x >> 13) & 0x00007FFF)))
#define PMC_MASK_IAGT_VTHD_BG             ((u32)0x0000000F << 9)          /*!<R/W 0  This field used for bg arb control. */
#define PMC_IAGT_VTHD_BG(x)               (((u32)((x) & 0x0000000F) << 9))
#define PMC_GET_IAGT_VTHD_BG(x)           ((u32)(((x >> 9) & 0x0000000F)))
#define PMC_MASK_IAGT_VTHD_REGU_STS       ((u32)0x000001FF << 0)          /*!<R/W 0  This field used for regu sts arb control. */
#define PMC_IAGT_VTHD_REGU_STS(x)         (((u32)((x) & 0x000001FF) << 0))
#define PMC_GET_IAGT_VTHD_REGU_STS(x)     ((u32)(((x >> 0) & 0x000001FF)))
/** @} */


/*==========PMC Register Address Definition==========*/
#define WAK_STATUS0                                  0x0000
#define WAK_MASK0_NP                                 0x0008
#define WAK_MASK0_AP                                 0x0010
#define WLPMC_ISR                                    0x0020
#define WLAN_PWRCTL                                  0x0024
#define SYSPMC_CTRL                                  0x0028
#define SYSPMC_OPT                                   0x002C
#define DUMMY_LOAD_CTRL                              0x0030
#define PMC_PATCH_GRP0_0                             0x0034
#define PMC_PATCH_GRP0_1                             0x0038
#define PMC_PATCH_GRP1_0                             0x003C
#define PMC_PATCH_GRP1_1                             0x0040
#define PMC_PATCH_GRP2_0                             0x0044
#define PMC_PATCH_GRP2_1                             0x0048
#define PMC_PATCH_GRP3_0                             0x004C
#define PMC_PATCH_GRP3_1                             0x0050
#define PMC_PATCH_GRP4_0                             0x0054
#define PMC_PATCH_GRP4_1                             0x0058
#define PMC_PATCH_GRP5_0                             0x005C
#define PMC_PATCH_GRP5_1                             0x0060
#define AIP_TRIGGER                                  0x0070
#define AIP_OPTION                                   0x0074
#define ARB_DBC                                      0x0080
#define ARB_VTH                                      0x0084

/** @} */

/* AUTO_GEN_END */

/* AUTO_GEN_WAKE_SRC_START */

/**************************************************************************//**
* @defgroup WAK_STATUS0/WAK_MASK0_LP/WAK_MASK0_NP/WAK_MASK0_AP
* @brief
* @{
*****************************************************************************/
#define WAKE_SRC_WIFI_FISR_FESR_IRQ                  ((u32)0x00000001 << 0)
#define WAKE_SRC_WIFI_FTSR_MAILBOX                   ((u32)0x00000001 << 1)
#define WAKE_SRC_KM4_WAKE_IRQ                        ((u32)0x00000001 << 2)
#define WAKE_SRC_BT_WAKE_HOST                        ((u32)0x00000001 << 3)
#define WAKE_SRC_IPC_KM4                             ((u32)0x00000001 << 4)
#define WAKE_SRC_IWDG0                               ((u32)0x00000001 << 5)
#define WAKE_SRC_Timer4                              ((u32)0x00000001 << 6)
#define WAKE_SRC_Timer5                              ((u32)0x00000001 << 7)
#define WAKE_SRC_Timer6                              ((u32)0x00000001 << 8)
#define WAKE_SRC_Timer7                              ((u32)0x00000001 << 9)
#define WAKE_SRC_pmc_timer0                          ((u32)0x00000001 << 10)
#define WAKE_SRC_pmc_timer1                          ((u32)0x00000001 << 11)
#define WAKE_SRC_UART0                               ((u32)0x00000001 << 12)
#define WAKE_SRC_UART1                               ((u32)0x00000001 << 13)
#define WAKE_SRC_UART2_BT                            ((u32)0x00000001 << 14)
#define WAKE_SRC_UART_LOG                            ((u32)0x00000001 << 15)
#define WAKE_SRC_GPIOA                               ((u32)0x00000001 << 16)
#define WAKE_SRC_GPIOB                               ((u32)0x00000001 << 17)
#define WAKE_SRC_I2C0                                ((u32)0x00000001 << 18)
#define WAKE_SRC_I2C1                                ((u32)0x00000001 << 19)
#define WAKE_SRC_CTOUCH                              ((u32)0x00000001 << 20)
#define WAKE_SRC_RTC                                 ((u32)0x00000001 << 21)
#define WAKE_SRC_ADC                                 ((u32)0x00000001 << 22)
#define WAKE_SRC_ADC_COMP                            ((u32)0x00000001 << 23)
#define WAKE_SRC_BOR                                 ((u32)0x00000001 << 24)
#define WAKE_SRC_PWR_DOWN                            ((u32)0x00000001 << 25)
#define WAKE_SRC_Keyscan                             ((u32)0x00000001 << 26)
#define WAKE_SRC_AON_TIM                             ((u32)0x00000001 << 27)
#define WAKE_SRC_AON_WAKEPIN                         ((u32)0x00000001 << 28)
#define WAKE_SRC_SDIO                                ((u32)0x00000001 << 29)
/** @} */

/* AUTO_GEN_WAKE_SRC_END */

/* MANUAL_GEN_START */

//Please add your defination here
#define STATE_LDOPC_SWRPFM_07	0x1
#define STATE_LDOPC_SWRPFM_08	0x2
#define STATE_LDOPC_SWRPFM_09	0x3
#define STATE_LDOSLP_SWRPFM		0x4
#define STATE_LDOSLP_SWRPWM		0x5
#define STATE_LDONORM_SWRPWM	0x6

#define LDO_MEM_MODE_OFF	0x0
#define LDO_MEM_MODE_SLEEP	0x1
#define LDO_MEM_MODE_NORM0	0x2
#define LDO_MEM_MODE_NORM1	0x3

/* MANUAL_GEN_END */

#endif

/** @} */

/** @} */
