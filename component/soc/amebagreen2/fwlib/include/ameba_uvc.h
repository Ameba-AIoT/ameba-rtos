/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _RL7005_UVC_H_
#define _RL7005_UVC_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup UVC
  * @brief UVC driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup UVC_Register_Definitions UVC Register Definitions
  * @{
  */

/** @defgroup UVC_CTRL
  * @brief Control Register
  * @{
  */
#define UVC_MASK_CH1_EP_NUM      ((u32)0x0000000F << 20)           /*!< R/W 0x0  */
#define UVC_CH1_EP_NUM(x)        (((u32)((x) & 0x0000000F) << 20))
#define UVC_GET_CH1_EP_NUM(x)    ((u32)(((x >> 20) & 0x0000000F)))
#define UVC_MASK_CH0_EP_NUM      ((u32)0x0000000F << 16)           /*!< R/W 0x0  */
#define UVC_CH0_EP_NUM(x)        (((u32)((x) & 0x0000000F) << 16))
#define UVC_GET_CH0_EP_NUM(x)    ((u32)(((x >> 16) & 0x0000000F)))
#define UVC_BIT_CH1_FRM_DONE_OPT ((u32)0x00000001 << 5)            /*!< R/W 0x0  1: frame_done when xfercomp at EOF flag 0: frame_done when frame id changing*/
#define UVC_CH1_FRM_DONE_OPT(x)  ((u32)(((x) & 0x00000001) << 5))
#define UVC_BIT_CH0_FRM_DONE_OPT ((u32)0x00000001 << 4)            /*!< R/W 0x0  1: frame_done when xfercomp at EOF flag 0: frame_done when frame id changing*/
#define UVC_CH0_FRM_DONE_OPT(x)  ((u32)(((x) & 0x00000001) << 4))
#define UVC_BIT_CH1_CONCAT_EN    ((u32)0x00000001 << 1)            /*!< R/W 0x0  */
#define UVC_CH1_CONCAT_EN(x)     ((u32)(((x) & 0x00000001) << 1))
#define UVC_BIT_CH0_CONCAT_EN    ((u32)0x00000001 << 0)            /*!< R/W 0x0  */
#define UVC_CH0_CONCAT_EN(x)     ((u32)(((x) & 0x00000001) << 0))
/** @} */

/** @defgroup UVC_CHNL0_CTRL
  * @brief Channel_0 Control
  * @{
  */
#define UVC_MASK_CH0_DMA_ADDR   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  For checking ahb xfer with header, this value should be the same as HCDMAi in otg_core*/
#define UVC_CH0_DMA_ADDR(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define UVC_GET_CH0_DMA_ADDR(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup UVC_CHNL1_CTRL
  * @brief Channel_1 Control
  * @{
  */
#define UVC_MASK_CH1_DMA_ADDR   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  For checking ahb xfer with header, this value should be the same as HCDMAi in otg_core*/
#define UVC_CH1_DMA_ADDR(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define UVC_GET_CH1_DMA_ADDR(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup UVC_INTR
  * @brief Interrupt Mask and Status
  * @{
  */
#define UVC_BIT_CH1_BUF1_HEADER_ERR_MASK   ((u32)0x00000001 << 29)           /*!< R/W 0x0  */
#define UVC_CH1_BUF1_HEADER_ERR_MASK(x)    ((u32)(((x) & 0x00000001) << 29))
#define UVC_BIT_CH1_BUF0_HEADER_ERR_MASK   ((u32)0x00000001 << 28)           /*!< R/W 0x0  */
#define UVC_CH1_BUF0_HEADER_ERR_MASK(x)    ((u32)(((x) & 0x00000001) << 28))
#define UVC_BIT_CH1_BUF1_FRM_OVERSIZE_MASK ((u32)0x00000001 << 27)           /*!< R/W 0x0  */
#define UVC_CH1_BUF1_FRM_OVERSIZE_MASK(x)  ((u32)(((x) & 0x00000001) << 27))
#define UVC_BIT_CH1_BUF0_FRM_OVERSIZE_MASK ((u32)0x00000001 << 26)           /*!< R/W 0x0  */
#define UVC_CH1_BUF0_FRM_OVERSIZE_MASK(x)  ((u32)(((x) & 0x00000001) << 26))
#define UVC_BIT_CH1_BUF1_FRM_DONE_MASK     ((u32)0x00000001 << 25)           /*!< R/W 0x0  */
#define UVC_CH1_BUF1_FRM_DONE_MASK(x)      ((u32)(((x) & 0x00000001) << 25))
#define UVC_BIT_CH1_BUF0_FRM_DONE_MASK     ((u32)0x00000001 << 24)           /*!< R/W 0x0  */
#define UVC_CH1_BUF0_FRM_DONE_MASK(x)      ((u32)(((x) & 0x00000001) << 24))
#define UVC_BIT_CH0_BUF1_HEADER_ERR_MASK   ((u32)0x00000001 << 21)           /*!< R/W 0x0  */
#define UVC_CH0_BUF1_HEADER_ERR_MASK(x)    ((u32)(((x) & 0x00000001) << 21))
#define UVC_BIT_CH0_BUF0_HEADER_ERR_MASK   ((u32)0x00000001 << 20)           /*!< R/W 0x0  */
#define UVC_CH0_BUF0_HEADER_ERR_MASK(x)    ((u32)(((x) & 0x00000001) << 20))
#define UVC_BIT_CH0_BUF1_FRM_OVERSIZE_MASK ((u32)0x00000001 << 19)           /*!< R/W 0x0  */
#define UVC_CH0_BUF1_FRM_OVERSIZE_MASK(x)  ((u32)(((x) & 0x00000001) << 19))
#define UVC_BIT_CH0_BUF0_FRM_OVERSIZE_MASK ((u32)0x00000001 << 18)           /*!< R/W 0x0  */
#define UVC_CH0_BUF0_FRM_OVERSIZE_MASK(x)  ((u32)(((x) & 0x00000001) << 18))
#define UVC_BIT_CH0_BUF1_FRM_DONE_MASK     ((u32)0x00000001 << 17)           /*!< R/W 0x0  */
#define UVC_CH0_BUF1_FRM_DONE_MASK(x)      ((u32)(((x) & 0x00000001) << 17))
#define UVC_BIT_CH0_BUF0_FRM_DONE_MASK     ((u32)0x00000001 << 16)           /*!< R/W 0x0  */
#define UVC_CH0_BUF0_FRM_DONE_MASK(x)      ((u32)(((x) & 0x00000001) << 16))
#define UVC_BIT_CH1_BUF1_HEADER_ERR        ((u32)0x00000001 << 13)           /*!< RW1C 0x0  */
#define UVC_CH1_BUF1_HEADER_ERR(x)         ((u32)(((x) & 0x00000001) << 13))
#define UVC_BIT_CH1_BUF0_HEADER_ERR        ((u32)0x00000001 << 12)           /*!< RW1C 0x0  */
#define UVC_CH1_BUF0_HEADER_ERR(x)         ((u32)(((x) & 0x00000001) << 12))
#define UVC_BIT_CH1_BUF1_FRM_OVERSIZE      ((u32)0x00000001 << 11)           /*!< RW1C 0x0  */
#define UVC_CH1_BUF1_FRM_OVERSIZE(x)       ((u32)(((x) & 0x00000001) << 11))
#define UVC_BIT_CH1_BUF0_FRM_OVERSIZE      ((u32)0x00000001 << 10)           /*!< RW1C 0x0  */
#define UVC_CH1_BUF0_FRM_OVERSIZE(x)       ((u32)(((x) & 0x00000001) << 10))
#define UVC_BIT_CH1_BUF1_FRM_DONE          ((u32)0x00000001 << 9)            /*!< RW1C 0x0  */
#define UVC_CH1_BUF1_FRM_DONE(x)           ((u32)(((x) & 0x00000001) << 9))
#define UVC_BIT_CH1_BUF0_FRM_DONE          ((u32)0x00000001 << 8)            /*!< RW1C 0x0  */
#define UVC_CH1_BUF0_FRM_DONE(x)           ((u32)(((x) & 0x00000001) << 8))
#define UVC_BIT_CH0_BUF1_HEADER_ERR        ((u32)0x00000001 << 5)            /*!< RW1C 0x0  */
#define UVC_CH0_BUF1_HEADER_ERR(x)         ((u32)(((x) & 0x00000001) << 5))
#define UVC_BIT_CH0_BUF0_HEADER_ERR        ((u32)0x00000001 << 4)            /*!< RW1C 0x0  */
#define UVC_CH0_BUF0_HEADER_ERR(x)         ((u32)(((x) & 0x00000001) << 4))
#define UVC_BIT_CH0_BUF1_FRM_OVERSIZE      ((u32)0x00000001 << 3)            /*!< RW1C 0x0  */
#define UVC_CH0_BUF1_FRM_OVERSIZE(x)       ((u32)(((x) & 0x00000001) << 3))
#define UVC_BIT_CH0_BUF0_FRM_OVERSIZE      ((u32)0x00000001 << 2)            /*!< RW1C 0x0  */
#define UVC_CH0_BUF0_FRM_OVERSIZE(x)       ((u32)(((x) & 0x00000001) << 2))
#define UVC_BIT_CH0_BUF1_FRM_DONE          ((u32)0x00000001 << 1)            /*!< RW1C 0x0  */
#define UVC_CH0_BUF1_FRM_DONE(x)           ((u32)(((x) & 0x00000001) << 1))
#define UVC_BIT_CH0_BUF0_FRM_DONE          ((u32)0x00000001 << 0)            /*!< RW1C 0x0  */
#define UVC_CH0_BUF0_FRM_DONE(x)           ((u32)(((x) & 0x00000001) << 0))
/** @} */

/** @defgroup UVC_CHx_BUF0_START_ADDR
  * @brief
  * @{
  */
#define UVC_MASK_CHx_BUF0_START_ADDR   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  */
#define UVC_CHx_BUF0_START_ADDR(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define UVC_GET_CHx_BUF0_START_ADDR(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup UVC_CHx_BUF0_END_ADDR
  * @brief
  * @{
  */
#define UVC_MASK_CHx_BUF0_END_ADDR   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  */
#define UVC_CHx_BUF0_END_ADDR(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define UVC_GET_CHx_BUF0_END_ADDR(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup UVC_CHx_BUF0_FRM_SIZE
  * @brief
  * @{
  */
#define UVC_MASK_CHx_BUF0_FRM_SIZE   ((u32)0x00FFFFFF << 0)           /*!< R 0x0  */
#define UVC_CHx_BUF0_FRM_SIZE(x)     (((u32)((x) & 0x00FFFFFF) << 0))
#define UVC_GET_CHx_BUF0_FRM_SIZE(x) ((u32)(((x >> 0) & 0x00FFFFFF)))
/** @} */

/** @defgroup UVC_CHx_BUF0_PTS
  * @brief
  * @{
  */
#define UVC_MASK_CHx_BUF0_PTS   ((u32)0xFFFFFFFF << 0)           /*!< R 0x0  */
#define UVC_CHx_BUF0_PTS(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define UVC_GET_CHx_BUF0_PTS(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup UVC_CHx_BUF0_SCR_FIRST
  * @brief
  * @{
  */
#define UVC_MASK_CHx_BUF0_SCR_FIRST   ((u32)0xFFFFFFFF << 0)           /*!< R 0x0  */
#define UVC_CHx_BUF0_SCR_FIRST(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define UVC_GET_CHx_BUF0_SCR_FIRST(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup UVC_CHx_BUF0_SCR_LAST
  * @brief
  * @{
  */
#define UVC_MASK_CHx_BUF0_SCR_LAST   ((u32)0xFFFFFFFF << 0)           /*!< R 0x0  */
#define UVC_CHx_BUF0_SCR_LAST(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define UVC_GET_CHx_BUF0_SCR_LAST(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup UVC_CHx_BUF0_SOF_CNT
  * @brief
  * @{
  */
#define UVC_MASK_CHx_BUF0_SOF_CNT   ((u32)0x0000FFFF << 0)           /*!< R 0x0  */
#define UVC_CHx_BUF0_SOF_CNT(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define UVC_GET_CHx_BUF0_SOF_CNT(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup UVC_CHx_BUF1_START_ADDR
  * @brief
  * @{
  */
#define UVC_MASK_CHx_BUF1_START_ADDR   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  */
#define UVC_CHx_BUF1_START_ADDR(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define UVC_GET_CHx_BUF1_START_ADDR(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup UVC_CHx_BUF1_END_ADDR
  * @brief
  * @{
  */
#define UVC_MASK_CHx_BUF1_END_ADDR   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  */
#define UVC_CHx_BUF1_END_ADDR(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define UVC_GET_CHx_BUF1_END_ADDR(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup UVC_CHx_BUF1_FRM_SIZE
  * @brief
  * @{
  */
#define UVC_MASK_CHx_BUF1_FRM_SIZE   ((u32)0x00FFFFFF << 0)           /*!< R 0x0  */
#define UVC_CHx_BUF1_FRM_SIZE(x)     (((u32)((x) & 0x00FFFFFF) << 0))
#define UVC_GET_CHx_BUF1_FRM_SIZE(x) ((u32)(((x >> 0) & 0x00FFFFFF)))
/** @} */

/** @defgroup UVC_CHx_BUF1_PTS
  * @brief
  * @{
  */
#define UVC_MASK_CHx_BUF1_PTS   ((u32)0xFFFFFFFF << 0)           /*!< R 0x0  */
#define UVC_CHx_BUF1_PTS(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define UVC_GET_CHx_BUF1_PTS(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup UVC_CHx_BUF1_SCR_FIRST
  * @brief
  * @{
  */
#define UVC_MASK_CHx_BUF1_SCR_FIRST   ((u32)0xFFFFFFFF << 0)           /*!< R 0x0  */
#define UVC_CHx_BUF1_SCR_FIRST(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define UVC_GET_CHx_BUF1_SCR_FIRST(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup UVC_CHx_BUF1_SCR_LAST
  * @brief
  * @{
  */
#define UVC_MASK_CHx_BUF1_SCR_LAST   ((u32)0xFFFFFFFF << 0)           /*!< R 0x0  */
#define UVC_CHx_BUF1_SCR_LAST(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define UVC_GET_CHx_BUF1_SCR_LAST(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup UVC_CHx_BUF1_SOF_CNT
  * @brief
  * @{
  */
#define UVC_MASK_CHx_BUF1_SOF_CNT   ((u32)0x0000FFFF << 0)           /*!< R 0x0  */
#define UVC_CHx_BUF1_SOF_CNT(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define UVC_GET_CHx_BUF1_SOF_CNT(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup UVC_CH0_EP_CTRL
  * @brief Channel_0 Endpoint Control
  * @{
  */
#define UVC_MASK_CH0_EP_BINTERVAL   ((u32)0x000000FF << 8)           /*!< R/W 0x0  */
#define UVC_CH0_EP_BINTERVAL(x)     (((u32)((x) & 0x000000FF) << 8))
#define UVC_GET_CH0_EP_BINTERVAL(x) ((u32)(((x >> 8) & 0x000000FF)))
#define UVC_BIT_CH0_EP_AUTO_EN      ((u32)0x00000001 << 0)           /*!< R/W 0x0  */
#define UVC_CH0_EP_AUTO_EN(x)       ((u32)(((x) & 0x00000001) << 0))
/** @} */

/** @defgroup UVC_CH1_EP_CTRL
  * @brief Channel_1 Endpoint Control
  * @{
  */
#define UVC_MASK_CH1_EP_BINTERVAL   ((u32)0x000000FF << 8)           /*!< R/W 0x0  */
#define UVC_CH1_EP_BINTERVAL(x)     (((u32)((x) & 0x000000FF) << 8))
#define UVC_GET_CH1_EP_BINTERVAL(x) ((u32)(((x >> 8) & 0x000000FF)))
#define UVC_BIT_CH1_EP_AUTO_EN      ((u32)0x00000001 << 0)           /*!< R/W 0x0  */
#define UVC_CH1_EP_AUTO_EN(x)       ((u32)(((x) & 0x00000001) << 0))
/** @} */

/** @defgroup UVC_HST_FRREM_CTRL
  * @brief Host Frame Remain Threshold
  * @{
  */
#define UVC_MASK_HST_FRREMAIN_THRESH   ((u32)0x0000FFFF << 0)           /*!< R/W 0x10  threshold for checking hst_frrem if hst_frrem(otg_core) > threshold, enable iso endpoint in this frame else, enable iso endpoint in next frame*/
#define UVC_HST_FRREMAIN_THRESH(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define UVC_GET_HST_FRREMAIN_THRESH(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup UVC_CH0_CMDx_CTRL
  * @brief
  * @{
  */
#define UVC_BIT_CH0_CMDx_EN      ((u32)0x00000001 << 31)           /*!< R/W 0x0  */
#define UVC_CH0_CMDx_EN(x)       ((u32)(((x) & 0x00000001) << 31))
#define UVC_MASK_CH0_CMDx_TYPE   ((u32)0x00000007 << 28)           /*!< R/W 0x0  */
#define UVC_CH0_CMDx_TYPE(x)     (((u32)((x) & 0x00000007) << 28))
#define UVC_GET_CH0_CMDx_TYPE(x) ((u32)(((x >> 28) & 0x00000007)))
#define UVC_MASK_CH0_CMDx_OFST   ((u32)0x00000FFF << 0)            /*!< R/W 0x0  */
#define UVC_CH0_CMDx_OFST(x)     (((u32)((x) & 0x00000FFF) << 0))
#define UVC_GET_CH0_CMDx_OFST(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup UVC_CH0_CMDx_DATA
  * @brief
  * @{
  */
#define UVC_MASK_CH0_CMDx_DATA   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  */
#define UVC_CH0_CMDx_DATA(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define UVC_GET_CH0_CMDx_DATA(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup UVC_CH1_CMDx_CTRL
  * @brief
  * @{
  */
#define UVC_BIT_CH1_CMDx_EN      ((u32)0x00000001 << 31)           /*!< R/W 0x0  */
#define UVC_CH1_CMDx_EN(x)       ((u32)(((x) & 0x00000001) << 31))
#define UVC_MASK_CH1_CMDx_TYPE   ((u32)0x00000007 << 28)           /*!< R/W 0x0  */
#define UVC_CH1_CMDx_TYPE(x)     (((u32)((x) & 0x00000007) << 28))
#define UVC_GET_CH1_CMDx_TYPE(x) ((u32)(((x >> 28) & 0x00000007)))
#define UVC_MASK_CH1_CMDx_OFST   ((u32)0x00000FFF << 0)            /*!< R/W 0x0  */
#define UVC_CH1_CMDx_OFST(x)     (((u32)((x) & 0x00000FFF) << 0))
#define UVC_GET_CH1_CMDx_OFST(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup UVC_CH1_CMDx_DATA
  * @brief
  * @{
  */
#define UVC_MASK_CH1_CMDx_DATA   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  */
#define UVC_CH1_CMDx_DATA(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define UVC_GET_CH1_CMDx_DATA(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup UVC_Exported_Types UVC Exported Types
  * @{
  */

/** @brief UVC Port
  */
typedef struct {
	__IO uint32_t UVC_CHx_BUF0_START_ADDR;  /*!< Register */
	__IO uint32_t UVC_CHx_BUF0_END_ADDR  ;  /*!< Register */
	__I  uint32_t UVC_CHx_BUF0_FRM_SIZE  ;  /*!< Register */
	__I  uint32_t UVC_CHx_BUF0_PTS       ;  /*!< Register */
	__I  uint32_t UVC_CHx_BUF0_SCR_FIRST ;  /*!< Register */
	__I  uint32_t UVC_CHx_BUF0_SCR_LAST  ;  /*!< Register */
	__I  uint32_t UVC_CHx_BUF0_SOF_CNT   ;  /*!< Register */
	__IO uint32_t UVC_CHx_RSVD0          ;  /*!< Register */
	__IO uint32_t UVC_CHx_BUF1_START_ADDR;  /*!< Register */
	__IO uint32_t UVC_CHx_BUF1_END_ADDR  ;  /*!< Register */
	__I  uint32_t UVC_CHx_BUF1_FRM_SIZE  ;  /*!< Register */
	__I  uint32_t UVC_CHx_BUF1_PTS       ;  /*!< Register */
	__I  uint32_t UVC_CHx_BUF1_SCR_FIRST ;  /*!< Register */
	__I  uint32_t UVC_CHx_BUF1_SCR_LAST  ;  /*!< Register */
	__I  uint32_t UVC_CHx_BUF1_SOF_CNT   ;  /*!< Register */
	__IO uint32_t UVC_CHx_RSVD1          ;  /*!< Register */
} CH_BUF_TypeDef;

/** @brief UVC Port
  */
typedef struct {
	__IO uint32_t UVC_CH0_CMDx_CTRL;  /*!< Register */
	__IO uint32_t UVC_CH0_CMDx_DATA;  /*!< Register */
} CH0_CMD_TypeDef;

/** @brief UVC Port
  */
typedef struct {
	__IO uint32_t UVC_CH1_CMDx_CTRL;  /*!< Register */
	__IO uint32_t UVC_CH1_CMDx_DATA;  /*!< Register */
} CH1_CMD_TypeDef;

/** @brief UVC Register Declaration
  */

typedef struct {
	__IO uint32_t UVC_CTRL                  ;  /*!< CONTROL REGISTER,  Address offset:0x000 */
	__IO uint32_t UVC_CHNL0_CTRL            ;  /*!< CHANNEL_0 CONTROL Register,  Address offset:0x004 */
	__IO uint32_t UVC_CHNL1_CTRL            ;  /*!< CHANNEL_1 CONTROL Register,  Address offset:0x008 */
	__IO uint32_t UVC_INTR                  ;  /*!< INTERRUPT MASK AND STATUS Register,  Address offset:0x00C */
	CH_BUF_TypeDef CH_BUF[2]                ;  /*!< UVC IP have 2 ports,  Address offset:0x010-0x08C */
	__IO uint32_t RSVD0[28]                 ;  /*!< Reserved,  Address offset:0x090-0x0FF */
	__IO uint32_t UVC_CH0_EP_CTRL           ;  /*!< CHANNEL_0 ENDPOINT CONTROL Register,  Address offset:0x100 */
	__IO uint32_t UVC_CH1_EP_CTRL           ;  /*!< CHANNEL_1 ENDPOINT CONTROL Register,  Address offset:0x104 */
	__IO uint32_t RSVD1                     ;  /*!< Reserved,  Address offset:0x108-0x10B */
	__IO uint32_t UVC_HST_FRREM_CTRL        ;  /*!< HOST FRAME REMAIN THRESHOLD Register,  Address offset:0x10C */
	CH0_CMD_TypeDef CH0_CMD[6]              ;  /*!< UVC IP have 6 ports,  Address offset:0x110-0x13C */
	__IO uint32_t RSVD2[4]                  ;  /*!< Reserved,  Address offset:0x140-0x14F */
	CH1_CMD_TypeDef CH1_CMD[6]              ;  /*!< UVC IP have 6 ports,  Address offset:0x150-0x17C */
} UVC_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */


/* Exported constants --------------------------------------------------------*/
/** @defgroup UVC_Exported_Constants UVC Exported Constants
  * @{
  */

#define UVC_TOTAL_CHANNEL_NUM 								2
#define UVC_MAX_CMD_NUM 									6
#define UVC_MAX_BUF_NUM										2

/**
  * @}
  */

/* Exported types --------------------------------------------------------*/
/** @addtogroup UVC_Exported_Types UVC Exported Types
  * @{
  */


/**
  * @brief  UVC Cmd Reg Union Definition
  */
typedef union {
	struct {
		u32 offset : 12;
		u32 rsvd : 16;
		u32 type : 3;
		u32 enable : 1;
	};
	u32 ctrl;
} cmd_reg0_type;

/**
  * @brief  UVC Cmd Reg Structure Definition
  */
typedef struct {
	/* cmd reg0 */
	cmd_reg0_type cmd_reg0;
	/* cmd reg1 */
	u32 data;
} UVC_CmdReg;


/**
  * @brief  UVC Init Structure Definition
  */

typedef struct {
	u32 chn_num;
	u32 ep_auto_en;
	u32 ep_num;
	u32 frame_buf0_start_addr;
	u32 frame_buf0_end_addr;
	u32 frame_buf1_start_addr;
	u32 frame_buf1_end_addr;
	u32 hcdmai;
	UVC_CmdReg uvc_cmd[UVC_MAX_CMD_NUM];
	u32 eof;
	u8 binterval;
} UVC_InitStruct;

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup UVC_Exported_Functions UVC Exported Functions
  * @{
  */

u32 UVC_AllocChannel(void);
void UVC_FreeChannel(u32 uvc_chn);
u32 UVC_GetPTS(u32 uvc_chn, u32 buf_num);
u32 UVC_GetSCRFirst(u32 uvc_chn, u32 buf_num);
u32 UVC_GetSCRLast(u32 uvc_chn, u32 buf_num);
void UVC_INTConfig(u32 uvc_intr, u32 new_state);
void UVC_INTClear(u32 uvc_intr);
u32 UVC_GetIntStatus(void);
void UVC_SetBufferAddr(u32 uvc_chn, u32 buf_num, u32 start_addr, u32 end_addr);
u32 UVC_GetBufferAddr(u32 uvc_chn, u32 buf_num);
u32 UVC_GetFrameSize(u32 uvc_chn, u32 buf_num);
void UVC_SetCMD(u32 uvc_chn, u32 uvc_cmd_num, UVC_CmdReg *cmd);
void UVC_SetInterval(u32 uvc_chn, u8 bInterval);
void UVC_StructInit(UVC_InitStruct *uvc_initstruct);
void UVC_ChnlInit(UVC_InitStruct *uvc_initstruct);
void UVC_ConcatEn(u32 uvc_chn, u32 state);
void UVC_EPAutoEn(u32 uvc_chn, u32 state);
void UVC_SetEPNum(u32 uvc_chn, u32 ep_num);

/**
  * @}
  */

/* Other Definitions --------------------------------------------------------*/




/* MANUAL_GEN_END */

/** @} */

/** @} */

#endif