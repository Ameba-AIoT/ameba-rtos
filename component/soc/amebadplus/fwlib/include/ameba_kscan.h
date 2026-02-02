/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_KSCAN_H_
#define _AMEBA_KSCAN_H_

/* AUTO_GEN_START */

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup KSCAN
* @brief KSCAN driver modules
* @{
*/

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup KSCAN_Register_Definitions KSCAN Register Definitions
 * @{
 *****************************************************************************/

/** @defgroup KS_CLK_DIV
 * @brief clock division register
 * @{
 **/
#define KS_MASK_CLK_DIV                           ((u32)0x00000FFF << 0)          /*!<R/W 0x0  scan clock = bus clock/(keyscan_clk_div+1) */
#define KS_CLK_DIV(x)                             (((u32)((x) & 0x00000FFF) << 0))
#define KS_GET_CLK_DIV(x)                         ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup KS_TIM_CFG0
 * @brief Key-Scan configure register 0
 * @{
 **/
#define KS_MASK_POST_GUARD_TIMER                  ((u32)0x0000000F << 24)          /*!<R/W 0x3  Post guard time for key column, post guard time = scan_clk * reg_col_gt_post_sel */
#define KS_POST_GUARD_TIMER(x)                    (((u32)((x) & 0x0000000F) << 24))
#define KS_GET_POST_GUARD_TIMER(x)                ((u32)(((x >> 24) & 0x0000000F)))
#define KS_MASK_PRE_GUARD_TIMER                   ((u32)0x0000000F << 16)          /*!<R/W 0x3  Pre guard time for key column, pre guard time = scan_clk * reg_col_gt_pre_sel */
#define KS_PRE_GUARD_TIMER(x)                     (((u32)((x) & 0x0000000F) << 16))
#define KS_GET_PRE_GUARD_TIMER(x)                 ((u32)(((x >> 16) & 0x0000000F)))
#define KS_MASK_DEB_TIMER                         ((u32)0x00000FFF << 0)          /*!<R/W 0x0  Debounce counter n*scan_clk */
#define KS_DEB_TIMER(x)                           (((u32)((x) & 0x00000FFF) << 0))
#define KS_GET_DEB_TIMER(x)                       ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup KS_TIM_CFG1
 * @brief Key-Scan configure register 1
 * @{
 **/
#define KS_MASK_INTERVAL_TIMER                    ((u32)0x00000FFF << 16)          /*!<R/W 0x0  Scan interval counter n*scan_clk */
#define KS_INTERVAL_TIMER(x)                      (((u32)((x) & 0x00000FFF) << 16))
#define KS_GET_INTERVAL_TIMER(x)                  ((u32)(((x >> 16) & 0x00000FFF)))
#define KS_MASK_RELEASE_TIMER                     ((u32)0x00000FFF << 0)          /*!<R/W 0x0  Release detect counter n*scan_clk */
#define KS_RELEASE_TIMER(x)                       (((u32)((x) & 0x00000FFF) << 0))
#define KS_GET_RELEASE_TIMER(x)                   ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup KS_CTRL
 * @brief Key-Scan control register
 * @{
 **/
#define KS_BIT_WORK_MODE                          ((u32)0x00000001 << 3)          /*!<R/W 0x0  Work mode *0x1: Event Trigger Mode *0x0: Regular Scan Mode */
#define KS_BIT_BUSY_STATUS                        ((u32)0x00000001 << 1)          /*!<R 0x0  FSM busy status */
#define KS_BIT_RUN_ENABLE                         ((u32)0x00000001 << 0)          /*!<R/W 0x0  Enable internal Key-Scan scan clock. The Key-Scan clock enable must be after Key-Scan configuration is done. *0x1: Key-Scan clock enabled (HW FSM starts to run). *0x0: Key-Scan clock disabled (HW FSM stops). */
/** @} */

/** @defgroup KS_FIFO_CFG
 * @brief FIFO configuration register
 * @{
 **/
#define KS_MASK_FIFO_LIMIT_LEVEL                  ((u32)0x0000000F << 24)          /*!<R/W 0x0  Limit the max allowable key number be pressed at a time 0x0: no limit 0x1: only one scan data is allowable in each key scan … 0x6: Max 6 scan data is allowable in each key scan 0x7 ~ 0xf: DO NOT USE */
#define KS_FIFO_LIMIT_LEVEL(x)                    (((u32)((x) & 0x0000000F) << 24))
#define KS_GET_FIFO_LIMIT_LEVEL(x)                ((u32)(((x >> 24) & 0x0000000F)))
#define KS_MASK_FIFO_TH_LEVEL                     ((u32)0x0000000F << 16)          /*!<R/W 0x0  FIFO threshold setup */
#define KS_FIFO_TH_LEVEL(x)                       (((u32)((x) & 0x0000000F) << 16))
#define KS_GET_FIFO_TH_LEVEL(x)                   ((u32)(((x >> 16) & 0x0000000F)))
#define KS_BIT_FIFO_OV_CTRL                       ((u32)0x00000001 << 1)          /*!<R/W 0x0  FIFO overflow control 0x0: rejects the new scan data when FIFO is full 0x1: discard the oldest scan data when FIFO is full */
#define KS_BIT_FIFO_CLR                           ((u32)0x00000001 << 0)          /*!<WA0 0x0  Write 1to clear FIFO data */
/** @} */

/** @defgroup KS_COL_CFG
 * @brief column selection register
 * @{
 **/
#define KS_MASK_COL_SEL                           ((u32)0x000000FF << 0)          /*!<R/W 0x0  The control register to define which column is used. *0x0: No key column selected *0x1: Key column 0 selected *0x2: Key column 1 selected *0x3: Key column 0 and column 1 selected *0x4: Key column 2 selected *... */
#define KS_COL_SEL(x)                             (((u32)((x) & 0x000000FF) << 0))
#define KS_GET_COL_SEL(x)                         ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup KS_ROW_CFG
 * @brief row selection register
 * @{
 **/
#define KS_MASK_ROW_SEL                           ((u32)0x000000FF << 0)          /*!<R/W 0x0  The control register to define which row is used. *0x0: No key row selected *0x1: Key row 0 selected *0x2: Key row 1 selected *0x3: Key row 0 and row 1 selected *0x4: Key row 2 selected *... */
#define KS_ROW_SEL(x)                             (((u32)((x) & 0x000000FF) << 0))
#define KS_GET_ROW_SEL(x)                         ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup KS_DATA_NUM
 * @brief FIFO status
 * @{
 **/
#define KS_BIT_FIFO_FULL                          ((u32)0x00000001 << 17)          /*!<R 0x0  FIFO full status */
#define KS_BIT_FIFO_EMPTY                         ((u32)0x00000001 << 16)          /*!<R 0x1  FIFO empty status */
#define KS_MASK_FIFO_DATA_LEVEL                   ((u32)0x0000001F << 0)          /*!<R 0x0  Number of entry in FIFO */
#define KS_FIFO_DATA_LEVEL(x)                     (((u32)((x) & 0x0000001F) << 0))
#define KS_GET_FIFO_DATA_LEVEL(x)                 ((u32)(((x >> 0) & 0x0000001F)))
/** @} */

/** @defgroup KS_DATA
 * @brief Key-Scan data register
 * @{
 **/
#define KS_MASK_DATA                              ((u32)0x00000FFF << 0)          /*!<RP 0x0  When reading this register, these bits return the value in the Event FIFO. *Bit[11:8]: press or release event **0x0: release event **0x1: press event *Bit[7:4]: row index *Bit[3:0]: column index */
#define KS_DATA(x)                                (((u32)((x) & 0x00000FFF) << 0))
#define KS_GET_DATA(x)                            ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup KS_IMR
 * @brief interrupt mask register
 * @{
 **/
#define KS_BIT_ALL_DEFAULT_INT_MASK               ((u32)0x00000001 << 8)          /*!<R/W 0x0  0x0: Mask all default interrupt */
#define KS_BIT_STUCK_EVENT_INT_MASK               ((u32)0x00000001 << 7)          /*!<R/W 0x0  0x0: Mask stuck event interrupt */
#define KS_BIT_SCAN_EVENT_INT_MASK                ((u32)0x00000001 << 6)          /*!<R/W 0x0  0x0: Mask scan event interrupt */
#define KS_BIT_FIFO_LIMIT_INT_MASK                ((u32)0x00000001 << 5)          /*!<R/W 0x0  0x0: Mask FIFO limit interrupt */
#define KS_BIT_FIFO_OV_INT_MASK                   ((u32)0x00000001 << 4)          /*!<R/W 0x0  0x0: Mask FIFO overflow interrupt */
#define KS_BIT_FIFO_FULL_INT_MASK                 ((u32)0x00000001 << 3)          /*!<R/W 0x0  0x0: Mask FIFO full interrupt */
#define KS_BIT_SCAN_FINISH_INT_MASK               ((u32)0x00000001 << 2)          /*!<R/W 0x0  0x0: Mask scan finish interrupt */
#define KS_BIT_FIFO_NOTEMPTY_INT_MASK             ((u32)0x00000001 << 1)          /*!<R/W 0x0  0x0: Mask FIFO nonempty interrupt */
#define KS_BIT_ALL_RELEASE_INT_MASK               ((u32)0x00000001 << 0)          /*!<R/W 0x0  0x0: Mask All Release interrupt */
/** @} */

/** @defgroup KS_ICR
 * @brief interrupt clear register
 * @{
 **/
#define KS_BIT_ALL_DEFAULT_INT_CLR                ((u32)0x00000001 << 8)          /*!<WA0 0x0  Clear All Default interrupt flag Write '1' to clear */
#define KS_BIT_STUCK_INT_CLR                      ((u32)0x00000001 << 7)          /*!<WA0 0x0  Clear Stuck interrupt flag Write '1' to clear */
#define KS_BIT_FIFO_LIMIT_INT_CLR                 ((u32)0x00000001 << 5)          /*!<WA0 0x0  Clear FIFO limit interrupt flag Write '1' to clear */
#define KS_BIT_FIFO_OV_INT_CLR                    ((u32)0x00000001 << 4)          /*!<WA0 0x0  Clear FIFO overflow interrupt flag Write '1' to clear */
#define KS_BIT_SCAN_FINISH_INT_CLR                ((u32)0x00000001 << 2)          /*!<WA0 0x0  Clear scan finish interrupt flag Write '1' to clear */
#define KS_BIT_ALL_RELEASE_INT_CLR                ((u32)0x00000001 << 0)          /*!<WA0 0x0  Clear all release interrupt flag Write '1' to clear */
/** @} */

/** @defgroup KS_ISR
 * @brief interrupt status register
 * @{
 **/
#define KS_BIT_ALL_DEFAULT_INT_STATUS             ((u32)0x00000001 << 8)          /*!<R 0x0  Masked All Default interrupt status, means row status equal to ks_row_default_status */
#define KS_BIT_STUCK_EVENT_INT_STATUS             ((u32)0x00000001 << 7)          /*!<R 0x0  Masked Stuck event interrupt status */
#define KS_BIT_SCAN_EVENT_INT_STATUS              ((u32)0x00000001 << 6)          /*!<R 0x0  Masked Scan event interrupt status */
#define KS_BIT_FIFO_LIMIT_INT_STATUS              ((u32)0x00000001 << 5)          /*!<R 0x0  Masked FIFO limit interrupt status */
#define KS_BIT_FIFO_OV_INT_STATUS                 ((u32)0x00000001 << 4)          /*!<R 0x0  Masked FIFO overflow interrupt status */
#define KS_BIT_FIFO_FULL_INT_STATUS               ((u32)0x00000001 << 3)          /*!<R 0x0  Masked FIFO full interrupt status */
#define KS_BIT_SCAN_FINISH_INT_STATUS             ((u32)0x00000001 << 2)          /*!<R 0x0  Masked Scan finish interrupt status */
#define KS_BIT_FIFO_NOTEMPTY_INT_STATUS           ((u32)0x00000001 << 1)          /*!<R 0x0  Masked FIFO nonempty interrupt status */
#define KS_BIT_ALL_RELEASE_INT_STATUS             ((u32)0x00000001 << 0)          /*!<R 0x0  Masked All release interrupt status */
/** @} */

/** @defgroup KS_ISR_RAW
 * @brief raw interrupt flag register
 * @{
 **/
#define KS_BIT_ALL_DEFAULT_RAW_INT_STATUS         ((u32)0x00000001 << 8)          /*!<R 0x0  All default interrupt, means row status equal to ks_row_default_status */
#define KS_BIT_STUCK_EVENT_RAW_INT_STATUS         ((u32)0x00000001 << 7)          /*!<R 0x0  Key stuck event raw interrupt */
#define KS_BIT_SCAN_EVENT_RAW_INT_STATUS          ((u32)0x00000001 << 6)          /*!<R 0x0  Scan event raw interrupt, auto clear when the item is read */
#define KS_BIT_FIFO_LIMIT_RAW_INT_STATUS          ((u32)0x00000001 << 5)          /*!<R 0x0  FIFO limit raw interrupt */
#define KS_BIT_FIFO_OV_RAW_INT_STATUS             ((u32)0x00000001 << 4)          /*!<R 0x0  FIFO overflow raw interrupt */
#define KS_BIT_FIFO_FULL_RAW_INT_STATUS           ((u32)0x00000001 << 3)          /*!<R 0x0  FIFO full raw interrupt */
#define KS_BIT_SCAN_FINISH_RAW_INT_STATUS         ((u32)0x00000001 << 2)          /*!<R 0x0  Scan finish raw interrupt */
#define KS_BIT_FIFO_NOTEMPTY_RAW_INT_STATUS       ((u32)0x00000001 << 1)          /*!<R 0x0  FIFO nonempty raw interrupt Auto clear when FIFO is empty */
#define KS_BIT_ALL_RELEASE_RAW_INT_STATUS         ((u32)0x00000001 << 0)          /*!<R 0x0  All Release raw interrupt */
/** @} */

/** @defgroup KS_DISCHARGED_CTRL
 * @brief keyboard discharged register
 * @{
 **/
#define KS_BIT_DISCHARGE                          ((u32)0x00000001 << 1)          /*!<R/W 0x0  1: The keyboard is discharged before scan. */
#define KS_BIT_INTERVAL_POL                       ((u32)0x00000001 << 0)          /*!<R/W 0x0  1: The keyboard is discharged at interval (FSM!=scan idle allrelease). */
/** @} */

/** @defgroup KS_STUCK_CFG0
 * @brief Key-Scan Stuck Configuration Register
 * @{
 **/
#define KS_BIT_AUTO_CHECK_STUCK_ENABLE            ((u32)0x00000001 << 31)          /*!<R/W 0x0  Enable auto check stuck or not *0x1: Key-Scan auto chek stuck enabled. *0x0: Key-Scan auto check stuck disabled. */
#define KS_MASK_STUCK_TIME_THRESHOLD              ((u32)0x7FFFFFFF << 0)          /*!<R/W 0x7FFFFFFF  When the threshold is exceeded, it is considered to be stuck event or release stuck key, this counter is in scan clk. */
#define KS_STUCK_TIME_THRESHOLD(x)                (((u32)((x) & 0x7FFFFFFF) << 0))
#define KS_GET_STUCK_TIME_THRESHOLD(x)            ((u32)(((x >> 0) & 0x7FFFFFFF)))
/** @} */

/** @defgroup KS_STUCK_CFG1
 * @brief Key-Scan Stuck Configuration Register 1
 * @{
 **/
#define KS_MASK_ROW_DEFAULT_STATUS                ((u32)0x000000FF << 0)          /*!<R/W 0xFF  Keyboard row default status: *0b11111111: No key row is stucked *0b11111110: Key row 0 is stucked *0b11111101: Key row 1 is stucked *0b11111100: Key row 0 and column 1 are stucked *0b11111011: Key row 2 is stucked … */
#define KS_ROW_DEFAULT_STATUS(x)                  (((u32)((x) & 0x000000FF) << 0))
#define KS_GET_ROW_DEFAULT_STATUS(x)              ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup KS_STUCK_CFG2
 * @brief Key-Scan Stuck Configuration Register 2
 * @{
 **/
#define KS_BIT_STUCKROW_DETECT_ENABLE             ((u32)0x00000001 << 31)          /*!<R/W 0x0  Enable stuckrow detect function, and row gpio will be "no pull" state in ks_stuckrow_detect_time_interval */
#define KS_MASK_STUCKROW_DETECT_TIME              ((u32)0x000007FF << 13)          /*!<R/W 0x1  Key-Scan stuckrow detect timer, this counter is in clk_131k */
#define KS_STUCKROW_DETECT_TIME(x)                (((u32)((x) & 0x000007FF) << 13))
#define KS_GET_STUCKROW_DETECT_TIME(x)            ((u32)(((x >> 13) & 0x000007FF)))
#define KS_MASK_STUCKROW_DETECT_TIME_INTERVAL     ((u32)0x00000FFF << 0)          /*!<R/W 0x0  Key-Scan stuckrow detect interval timer, this counter is in clk_131k */
#define KS_STUCKROW_DETECT_TIME_INTERVAL(x)       (((u32)((x) & 0x00000FFF) << 0))
#define KS_GET_STUCKROW_DETECT_TIME_INTERVAL(x)   ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup KS_STUCK_KEY_STATUS1
 * @brief Key-Scan Status Register 1
 * @{
 **/
#define KS_MASK_KEY_STATUS1                       ((u32)0xFFFFFFFF << 0)          /*!<R 0x0  Each bit means the corresponding key's stuck state *0x1: key stucked *0x0: key unstucked Corresponding: key_status1[7:0] - row0[7:0] key_status1[15:8] - row1[7:0] key_status1[13:16] - row2[7:0] key_status1[31:24] - row3[7:0] */
#define KS_KEY_STATUS1(x)                         (((u32)((x) & 0xFFFFFFFF) << 0))
#define KS_GET_KEY_STATUS1(x)                     ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup KS_STUCK_KEY_STATUS2
 * @brief Key-Scan Status Register 2
 * @{
 **/
#define KS_MASK_KEY_STATUS2                       ((u32)0xFFFFFFFF << 0)          /*!<R 0x0  Each bit means the corresponding key's stuck state *0x1: key stucked *0x0: key unstucked Corresponding: key_status2[7:0] - row4[7:0] key_status2[15:8] - row5[7:0] key_status2[13:16] - row6[7:0] key_status2[31:24] - row7[7:0] */
#define KS_KEY_STATUS2(x)                         (((u32)((x) & 0xFFFFFFFF) << 0))
#define KS_GET_KEY_STATUS2(x)                     ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup KS_STUCK_KEY_ROW_STATUS1
 * @brief Key-Scan Row Status Register 1
 * @{
 **/
#define KS_MASK_ROW_STATUS                        ((u32)0x000000FF << 0)          /*!<R 0x0  Each bit means the corresponding key row's stuck state *0x1: row stucked *0x0: row unstucked Corresponding: row_status[7:0] - row[7:0] */
#define KS_ROW_STATUS(x)                          (((u32)((x) & 0x000000FF) << 0))
#define KS_GET_ROW_STATUS(x)                      ((u32)(((x >> 0) & 0x000000FF)))
/** @} */
/** @} */



/* Exported types --------------------------------------------------------*/

/** @defgroup KSCAN_Exported_Types KSCAN Exported Types
  * @{
*****************************************************************************/


/**
 * @brief KSCAN Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t KS_CLK_DIV;                             /*!< CLOCK DIVISION REGISTER,  Address offset: 0x000 */
	__IO uint32_t KS_TIM_CFG0;                            /*!< KEY-SCAN CONFIGURE REGISTER 0 Register,  Address offset: 0x004 */
	__IO uint32_t KS_TIM_CFG1;                            /*!< KEY-SCAN CONFIGURE REGISTER 1 Register,  Address offset: 0x008 */
	__IO uint32_t KS_CTRL;                                /*!< KEY-SCAN CONTROL REGISTER,  Address offset: 0x00C */
	__IO uint32_t KS_FIFO_CFG;                            /*!< FIFO CONFIGURATION REGISTER,  Address offset: 0x010 */
	__IO uint32_t KS_COL_CFG;                             /*!< COLUMN SELECTION REGISTER,  Address offset: 0x014 */
	__IO uint32_t KS_ROW_CFG;                             /*!< ROW SELECTION REGISTER,  Address offset: 0x018 */
	__I  uint32_t KS_DATA_NUM;                            /*!< FIFO STATUS Register,  Address offset: 0x01C */
	__IO uint32_t KS_DATA;                                /*!< KEY-SCAN DATA REGISTER,  Address offset: 0x020 */
	__IO uint32_t KS_IMR;                                 /*!< INTERRUPT MASK REGISTER,  Address offset: 0x024 */
	__O  uint32_t KS_ICR;                                 /*!< INTERRUPT CLEAR REGISTER,  Address offset: 0x028 */
	__I  uint32_t KS_ISR;                                 /*!< INTERRUPT STATUS REGISTER,  Address offset: 0x02C */
	__I  uint32_t KS_ISR_RAW;                             /*!< RAW INTERRUPT FLAG REGISTER,  Address offset: 0x030 */
	__IO uint32_t KS_DISCHARGED_CTRL;                     /*!< KEYBOARD DISCHARGED REGISTER,  Address offset: 0x034 */
	__IO uint32_t KS_IDLE_DEBOUNCE;                       /*!< KEY IDLE DEBOUNCE CONFIGURATION REGISTER,  Address offset: 0x038 */
	__IO uint32_t KS_STUCK_CFG0;                          /*!< KEY-SCAN STUCK CONFIGURATION REGISTER,  Address offset: 0x03C */
	__IO uint32_t KS_STUCK_CFG1;                          /*!< KEY-SCAN STUCK CONFIGURATION REGISTER 1 Register,  Address offset: 0x040 */
	__IO uint32_t KS_STUCK_CFG2;                          /*!< KEY-SCAN STUCK CONFIGURATION REGISTER 2 Register,  Address offset: 0x044 */
	__I  uint32_t KS_STUCK_KEY_STATUS1;                   /*!< KEY-SCAN STATUS REGISTER 1 Register,  Address offset: 0x048 */
	__I  uint32_t KS_STUCK_KEY_STATUS2;                   /*!< KEY-SCAN STATUS REGISTER 2 Register,  Address offset: 0x04C */
	__I  uint32_t KS_STUCK_KEY_ROW_STATUS1;               /*!< KEY-SCAN ROW STATUS REGISTER 1 Register,  Address offset: 0x050 */
	__IO uint32_t KS_TIM_CFG2;                            /*!< KEY-SCAN CONFIGURE REGISTER 2 Register,  Address offset: 0x054 */
	__IO uint32_t KS_DUMMY;                               /*!< KEY-SCAN DUMMY REGISTER,  Address offset: 0x058 */
} KSCAN_TypeDef;
/** @} */
/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your defination here

/* Exported types --------------------------------------------------------*/
/** @addtogroup KSCAN_Exported_Types KSCAN Exported Types
  * @{
  */

/**
  * @brief  KSCAN Init structure definition
  */
typedef struct {
	u32 KS_ClkDiv;			/*!< Specifies Keyscan clock divider. Scan_clk = Bus clock/(KS_ClkDiv+1).
					This parameter must be set to a value in the 0x0-0xfff range. */

	u32 KS_WorkMode;		/*!< Specifies Keyscan work mode.
					This parameter can be a value of @ref KSCAN_Work_Mode_definitions */

	u32 KS_RowSel;			/*!< Specifies which row(s) is used.
					This parameter must be set to a value in the 0x1-0xff range. */

	u32 KS_ColSel;			/*!< Specifies which column(s) is used.
					This parameter must be set to a value in the 0x1-0xff range. */

	u32 KS_DebounceCnt;		/*!< Specifies Keyscan Debounce Timer. Debounce Timer = (KS_DebounceCnt +1)* Scan_clk.
					This parameter must be set to a value in the 0x0-0xfff range. */

	u32 KS_IntervalCnt;		/*!< Specifies Keyscan Scan Interval Timer. Interval Timer = (KS_IntervalCnt +1)* Scan_clk.
					This parameter must be set to a value in the 0x0-0xfff range. */

	u32 KS_ReleaseCnt;		/*!< Specifies Keyscan All Release Timer. Release Timer = (KS_ReleaseCnt+1) * Scan_clk.
					This parameter must be set to a value in the 0x0-0xfff range. */

	u32 KS_LimitLevel;		/*!< Specifies the max allowable key number be pressed at a time
					This parameter can be a value of @ref KSCAN_FIFO_LimitLevel_Control */

	u32 KS_ThreholdLevel;	/*!< Specifies Keyscan FIFO threshold to trigger KS_FIFO_FULL
					This parameter can be a value of @ref KSCAN_FIFO_ThreholdLevel_Control */

	u32 KS_OverCtrl;		/*!< Specifies Keyscan FIFO over control.
					This parameter can be a value of @ref KSCAN_FIFO_Overflow_Control */
} KeyScan_InitTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup KSCAN_Exported_Constants KSCAN Exported Constants
  * @{
  */

/** @defgroup KSCAN_Peripheral_definitions
  * @{
  */
#define IS_KEYSCAN_ALL_PERIPH(PERIPH) ((PERIPH) == KEYSCAN_DEV || (PERIPH) == KEYSCAN_DEV_S)
/**
  * @}
  */

/** @defgroup KSCAN_Work_Mode_definitions
  * @{
  */
#define KS_REGULAR_SCAN_MODE			((u32)0x00000000)
#define KS_EVENT_TRIGGER_MODE			((u32)0x00000001)
#define IS_KS_WORK_MODE(MODE)			(((MODE) == KS_REGULAR_SCAN_MODE) || \
                               ((MODE) == KS_EVENT_TRIGGER_MODE))
/**
  * @}
  */

/** @defgroup KSCAN_FIFO_Overflow_Control
  * @{
  */
#define KS_FIFO_OVER_CTRL_DIS_NEW		((u32)0x00000000)
#define KS_FIFO_OVER_CTRL_DIS_LAST		((u32)0x00000001)
#define IS_KS_FIFO_OVER_CTRL(CTRL)		(((CTRL) == KS_FIFO_OVER_CTRL_DIS_NEW) || \
				((CTRL) == KS_FIFO_OVER_CTRL_DIS_LAST))
/**
  * @}
  */

/** @defgroup KSCAN_All_Interrupt_Mask
  * @{
  */
#define KS_BIT_ALL_INT_MSK				(KS_BIT_ALL_DEFAULT_INT_MASK | \
										KS_BIT_STUCK_EVENT_INT_MASK | \
										KS_BIT_SCAN_EVENT_INT_MASK | \
										KS_BIT_FIFO_LIMIT_INT_MASK | \
										KS_BIT_FIFO_OV_INT_MASK | \
										KS_BIT_FIFO_FULL_INT_MASK | \
										KS_BIT_SCAN_FINISH_INT_MASK | \
										KS_BIT_FIFO_NOTEMPTY_INT_MASK | \
										KS_BIT_ALL_RELEASE_INT_MASK)
/**
  * @}
  */

/** @defgroup KSCAN_All_Interrupt_Clear
  * @{
  */
#define KS_BIT_ALL_INT_CLR				(KS_BIT_ALL_DEFAULT_INT_CLR | \
										KS_BIT_STUCK_INT_CLR | \
										KS_BIT_FIFO_LIMIT_INT_CLR | \
										KS_BIT_FIFO_OV_INT_CLR | \
										KS_BIT_SCAN_FINISH_INT_CLR | \
										KS_BIT_ALL_RELEASE_INT_CLR)
/**
  * @}
  */

/** @defgroup KSCAN_FIFO_LimitLevel_Control
  * @{
  */
#define IS_KS_FIFO_LIMIT_LEVEL(DATA_NUM)	((DATA_NUM) <= 6)
/**
  * @}
  */

/** @defgroup KSCAN_FIFO_ThreholdLevel_Control
  * @{
  */
#define IS_KS_FIFO_TH_LEVEL(DATA_NUM)		(((DATA_NUM) > 0) && ((DATA_NUM) < 16))
/**
  * @}
  */

/**
  * @}
  */

/** @} */

/** @} */

_LONG_CALL_ void KeyScan_StructInit(KeyScan_InitTypeDef *KeyScan_InitStruct);
_LONG_CALL_ void KeyScan_Init(KSCAN_TypeDef *KeyScan, KeyScan_InitTypeDef *KeyScan_InitStruct);
_LONG_CALL_ void KeyScan_INTConfig(KSCAN_TypeDef *KeyScan, u32 KeyScan_IT, u8 newState);
_LONG_CALL_ void KeyScan_ClearINT(KSCAN_TypeDef *KeyScan, u32 KeyScan_IT);
_LONG_CALL_ u32 KeyScan_GetRawINT(KSCAN_TypeDef *KeyScan);
_LONG_CALL_ u32 KeyScan_GetINT(KSCAN_TypeDef *KeyScan);
_LONG_CALL_ u8 KeyScan_GetDataNum(KSCAN_TypeDef *KeyScan);
_LONG_CALL_ void KeyScan_ClearFIFOData(KSCAN_TypeDef *KeyScan);
_LONG_CALL_ bool KeyScan_GetFIFOState(KSCAN_TypeDef *KeyScan, u32 KeyScan_Flag);
_LONG_CALL_ void KeyScan_Read(KSCAN_TypeDef *KeyScan, u32 *outBuf, u8 count);
_LONG_CALL_ void KeyScan_Cmd(KSCAN_TypeDef *KeyScan, u8 NewState);
_LONG_CALL_ void KeyScan_SetColRow(KSCAN_TypeDef *KeyScan, u32 column_sel, u32 row_sel);
_LONG_CALL_ void KeyScan_StuckAutoCmd(KSCAN_TypeDef *KeyScan, u8 NewState);
_LONG_CALL_ void KeyScan_SetStuckThreshold(KSCAN_TypeDef *KeyScan, u32 ThresholdMs);
_LONG_CALL_ void KeyScan_StuckPeriodicalPull(KSCAN_TypeDef *KeyScan, u32 PullUpUs, u32 NoPullUs);
_LONG_CALL_ u32 KeyScan_GetStuckRow(KSCAN_TypeDef *KeyScan);
_LONG_CALL_ void KeyScan_SetStuckRow(KSCAN_TypeDef *KeyScan, u32 Status);

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

#endif
