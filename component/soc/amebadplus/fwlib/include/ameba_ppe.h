/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_PPE_H_
#define _AMEBA_PPE_H_

#include "ameba_spic.h"
/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup PPE PPE
* @brief PPE driver modules
* @{
*/

/// @cond
/* AUTO_GEN_START */


/* AUTO_GEN_END */
/// @endcond

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


/* Exported types ------------------------------------------------------------*/
/** @defgroup PPE_Exported_Types PPE Exported Types
  * @{
  */

/// @cond
/**
  * @brief  PPE Global Register Structure Definition
  */
typedef struct {
	__IO u32 GLB_CTL;
	__IO u32 FUNC_CFG;
	__IO u32 RELOAD_CFG;
	__IO u32 LL_CFG;
	__IO u32 LLP;
	__I  u32 RSVD2;
	__IO u32 SCA_RATIO_X;
	__IO u32 SCA_RATIO_Y;
	__IO u32 MISC;
	__IO u32 LINE_INDEX;
	__I  u32 RSVD[6];
	__I  u32 INT_STATUS;
	__I  u32 INT_RAW;
	__O  u32 INT_CLR;
	__IO u32 INT_MASK;
} PPE_GLOBAL_TypeDef;

/**
  * @brief  PPE Result Layer Register Structure Definition
  */
typedef struct {
	__IO u32 ADDR;
	__I  u32 RSVD1;
	__I  u32 POS;
	__IO u32 WIN_SIZE;
	__I  u32 RSVD2;
	__IO u32 PIC_CFG;
	__I  u32 RSVD3;
	__IO u32 BUS_CFG;
	__IO u32 HS_CFG;
	__I  u32 RSVD4[7];
} PPE_RESULT_LAYER_TypeDef;


/**
  * @brief  PPE Input Layer Register Structure Definition
  */
typedef struct {
	__IO u32 ADDR;
	__I  u32 RSVD1;
	__IO u32 POS;
	__IO u32 WIN_SIZE;
	__IO u32 CONST_PIX;
	__IO u32 PIC_CFG;
	__IO u32 KEY_COLOR;
	__IO u32 BUS_CFG;
	__IO u32 HS_CFG;
	__I  u32 RSVD2[7];
} PPE_INPUT_LAYER_TypeDef;

/**
  * @brief  PPE Global Register Structure Definition
  */
typedef struct {
	PPE_RESULT_LAYER_TypeDef RESULT_LAYER;
	PPE_INPUT_LAYER_TypeDef INPUT_LAYER[15];
} PPE_LAYER_TypeDef;
/// @endcond

/**
  * @brief  PPE Result Layer Address Type
  */
typedef enum {
	PPE_ADDR_MEM,   /*!< Address in internal memory. */
	PPE_ADDR_QSPI,  /*!< Address in QSPI flash. */
} PPE_ADDR_TYPE;

/**
  * @brief  PPE Pixel Format Definition
  */
typedef enum {
	PPE_ABGR8888 = 0x0, /*!< 32bpp ABGR 8-8-8-8 format. */
	PPE_ARGB8888,       /*!< 32bpp ARGB 8-8-8-8 format. */
	PPE_XBGR8888,       /*!< 32bpp XBGR 8-8-8-8 format. */
	PPE_XRGB8888,       /*!< 32bpp XRGB 8-8-8-8 format. */
	PPE_BGRA8888,       /*!< 32bpp BGRA 8-8-8-8 format. */
	PPE_RGBA8888,       /*!< 32bpp RGBA 8-8-8-8 format. */
	PPE_BGRX8888,       /*!< 32bpp BGRX 8-8-8-8 format. */
	PPE_RGBX8888,       /*!< 32bpp RGBX 8-8-8-8 format. */
	PPE_ABGR4444,       /*!< 16bpp ABGR 4-4-4-4 format. */
	PPE_ARGB4444,       /*!< 16bpp ARGB 4-4-4-4 format. */
	PPE_XBGR4444,       /*!< 16bpp XBGR 4-4-4-4 format. */
	PPE_XRGB4444,       /*!< 16bpp XRGB 4-4-4-4 format. */
	PPE_BGRA4444,       /*!< 16bpp BGRA 4-4-4-4 format. */
	PPE_RGBA4444,       /*!< 16bpp RGBA 4-4-4-4 format. */
	PPE_BGRX4444,       /*!< 16bpp BGRX 4-4-4-4 format. */
	PPE_RGBX4444,       /*!< 16bpp RGBX 4-4-4-4 format. */
	PPE_ABGR2222,       /*!< 8bpp ABGR 2-2-2-2 format. (= 0x10) */
	PPE_ARGB2222,       /*!< 8bpp ARGB 2-2-2-2 format. */
	PPE_XBGR2222,       /*!< 8bpp XBGR 2-2-2-2 format. */
	PPE_XRGB2222,       /*!< 8bpp XRGB 2-2-2-2 format. */
	PPE_BGRA2222,       /*!< 8bpp BGRA 2-2-2-2 format. */
	PPE_RGBA2222,       /*!< 8bpp RGBA 2-2-2-2 format. */
	PPE_BGRX2222,       /*!< 8bpp BGRX 2-2-2-2 format. */
	PPE_RGBX2222,       /*!< 8bpp RGBX 2-2-2-2 format. */
	PPE_ABGR8565,       /*!< 24bpp ABGR 8-5-6-5 format. */
	PPE_ARGB8565,       /*!< 24bpp ARGB 8-5-6-5 format. */
	PPE_XBGR8565,       /*!< 24bpp XBGR 8-5-6-5 format. */
	PPE_XRGB8565,       /*!< 24bpp XRGB 8-5-6-5 format. */
	PPE_BGRA5658,       /*!< 24bpp BGRA 5-6-5-8 format. */
	PPE_RGBA5658,       /*!< 24bpp RGBA 5-6-5-8 format. */
	PPE_BGRX5658,       /*!< 24bpp BGRX 5-6-5-8 format. */
	PPE_RGBX5658,       /*!< 24bpp RGBX 5-6-5-8 format. */
	PPE_ABGR1555,       /*!< 16bpp ABGR 1-5-5-5 format. (= 0x20) */
	PPE_ARGB1555,       /*!< 16bpp ARGB 1-5-5-5 format. */
	PPE_XBGR1555,       /*!< 16bpp XBGR 1-5-5-5 format. */
	PPE_XRGB1555,       /*!< 16bpp XRGB 1-5-5-5 format. */
	PPE_BGRA5551,       /*!< 16bpp BGRA 5-5-5-1 format. */
	PPE_RGBA5551,       /*!< 16bpp RGBA 5-5-5-1 format. */
	PPE_BGRX5551,       /*!< 16bpp BGRX 5-5-5-1 format. */
	PPE_RGBX5551,       /*!< 16bpp RGBX 5-5-5-1 format. */
	PPE_BGR888,         /*!< 24bpp BGR 8-8-8 format. */
	PPE_RGB888,         /*!< 24bpp RGB 8-8-8 format. */
	PPE_BGR565,         /*!< 16bpp BGR 5-6-5 format. */
	PPE_RGB565,         /*!< 16bpp RGB 5-6-5 format. */
	PPE_A8,             /*!< 8bpp alpha-only format. */
	PPE_X8,             /*!< 8bpp ignored-alpha format. */
	PPE_ABGR8666 = 0x32, /*!< 32bpp ABGR 8-6-6-6 format. */
	PPE_ARGB8666,       /*!< 32bpp ARGB 8-6-6-6 format. */
	PPE_XBGR8666,       /*!< 32bpp XBGR 8-6-6-6 format. */
	PPE_XRGB8666,       /*!< 32bpp XRGB 8-6-6-6 format. */
	PPE_BGRA6668,       /*!< 32bpp BGRA 6-6-6-8 format. */
	PPE_RGBA6668,       /*!< 32bpp RGBA 6-6-6-8 format. */
	PPE_BGRX6668,       /*!< 32bpp BGRX 6-6-6-8 format. */
	PPE_RGBX6668,       /*!< 32bpp RGBX 6-6-6-8 format. */
} PPE_PIXEL_FORMAT;

/**
  * @brief  PPE Pixel Source Definition
  */
typedef enum {
	PPE_LAYER_SRC_CONST,      /*!< Pixel data from constant color register. */
	PPE_LAYER_SRC_FROM_DMA,   /*!< Pixel data read from memory via DMA. */
} PPE_PIXEL_SOURCE;


/**
  * @brief  PPE DMA Handshake Definition
  */
typedef enum {
	PPE_DMA_HW_HANDSHAKE,   /*!< Hardware DMA handshake mode. */
	PPE_DMA_SW_HANDSHAKE,   /*!< Software DMA handshake mode. */
} PPE_DMA_HANDSHAKE;

/// @cond
/**
  * @brief  PPE DMA Msize Log Definition
  */
typedef enum {
	PPE_MSIZE_1,    /*!< DMA burst transaction size: 1 data item. */
	PPE_MSIZE_2,    /*!< DMA burst transaction size: 2 data items. */
	PPE_MSIZE_4,    /*!< DMA burst transaction size: 4 data items. */
	PPE_MSIZE_8,    /*!< DMA burst transaction size: 8 data items. */
	PPE_MSIZE_16,   /*!< DMA burst transaction size: 16 data items. */
	PPE_MSIZE_32,   /*!< DMA burst transaction size: 32 data items. */
	PPE_MSIZE_64,   /*!< DMA burst transaction size: 64 data items. */
	PPE_MSIZE_128,  /*!< DMA burst transaction size: 128 data items. */
	PPE_MSIZE_256,  /*!< DMA burst transaction size: 256 data items. */
	PPE_MSIZE_512,  /*!< DMA burst transaction size: 512 data items. */
	PPE_MSIZE_1024, /*!< DMA burst transaction size: 1024 data items. */
} PPE_MSIZE_LOG;


/**
  * @brief  PPE Max AXI Length Definition
  */
typedef enum {
	PPE_MAX_AXLEN_0,   /*!< Maximum AXI burst length: 1 beat. */
	PPE_MAX_AXLEN_1,   /*!< Maximum AXI burst length: 2 beats. */
	PPE_MAX_AXLEN_3,   /*!< Maximum AXI burst length: 4 beats. */
	PPE_MAX_AXLEN_7,   /*!< Maximum AXI burst length: 8 beats. */
	PPE_MAX_AXLEN_15,  /*!< Maximum AXI burst length: 16 beats. */
	PPE_MAX_AXLEN_31,  /*!< Maximum AXI burst length: 32 beats. */
	PPE_MAX_AXLEN_63,  /*!< Maximum AXI burst length: 64 beats. */
	PPE_MAX_AXLEN_127, /*!< Maximum AXI burst length: 128 beats. */
} PPE_MAX_AXLEN;


/**
  * @brief  PPE DMA Handshake Polarity Definition
  */
typedef enum {
	PPE_POLARITY_HIGH,  /*!< Active-high DMA handshake polarity. */
	PPE_POLARITY_LOW,   /*!< Active-low DMA handshake polarity. */
} PPE_ACTIVE_POLARITY;
/// @endcond

/**
  * @brief  PPE Arburst Definition
  */
typedef enum {
	PPE_ARBURST_FIX,   /*!< AXI FIXED burst type. */
	PPE_ARBURST_INCR,  /*!< AXI INCR burst type. */
} PPE_ARBURST;

/**
  * @brief  PPE Global State Definition
  */
typedef enum {
	PPE_STATE_DISABLE  = 0, /*!< PPE is disabled. */
	PPE_STATE_RUNNING  = 1, /*!< PPE is running. */
} PPE_GLOBAL_STATE;

/**
  * @brief  PPE LLI Layer Format Definition
  */
typedef struct {
	u32 LAYER_ADDR;      /*!< Address of the first pixel. */
	u32 LAYER_POS;       /*!< Position of the first pixel in the result window. */
	u32 LAYER_WINSIZE;   /*!< Window size: height[31:16] and width[15:0] in pixels. */
	u32 LAYER_CONST_PIX; /*!< Constant pixel value in ABGR8888 format. */
} PPE_LLI_LAYER;

/**
  * @brief  PPE Global register Format Definition
  */
typedef struct {
	u32 LL_CFG;      /*!< Linked list configuration register value. */
	u32 LLP;         /*!< Linked list pointer register value. */
	u32 SCA_RATIO_X; /*!< Horizontal scale ratio register value. */
	u32 SCA_RATIO_Y; /*!< Vertical scale ratio register value. */
} PPE_LLI_GLB;

/**
  * @brief  PPE Function Structure Definition
  */
typedef struct {
	u32 function;        /*!< PPE function mode selection; see PPE_FUNCTION_ALPHA_BLEND and PPE_FUNCTION_SCALE. */
	u32 blend_layer_num; /*!< Number of input layers used for alpha blending. */
} PPE_InitTypeDef;


/**
  * @brief  PPE Input layer Init Structure Definition
  */
typedef struct {
	u32 src_addr;             /*!< Source address of the layer pixel data. */
	u32 start_x;              /*!< X coordinate of the top-left pixel in the result window. */
	u32 start_y;              /*!< Y coordinate of the top-left pixel in the result window. */
	u32 width;                /*!< Width of the input layer in pixels. */
	u32 height;               /*!< Height of the input layer in pixels. */
	u32 const_ABGR8888_value; /*!< Constant pixel color in ABGR8888 format, used when src is PPE_LAYER_SRC_CONST. */
	u32 key_color_value;      /*!< Transparent (chroma-key) color value. */
	u32 line_len;             /*!< Number of bytes between two vertically adjacent pixels in memory (line stride). */
	PPE_PIXEL_FORMAT format;  /*!< Pixel format of the input layer; see @ref PPE_PIXEL_FORMAT. */
	PPE_PIXEL_SOURCE src;     /*!< Pixel data source; see @ref PPE_PIXEL_SOURCE. */
	int color_key_en;         /*!< Color keying enable: 1 to enable, 0 to disable. */
} PPE_InputLayer_InitTypeDef;


/**
  * @brief  PPE Result layer Init Structure Definition
  */
typedef struct {
	u32 src_addr;            /*!< Destination address for the composited output. */
	u32 width;               /*!< Width of the output image in pixels. */
	u32 height;              /*!< Height of the output image in pixels. */
	u32 line_len;            /*!< Number of bytes between two vertically adjacent pixels in memory (line stride). */
	PPE_PIXEL_FORMAT format; /*!< Pixel format of the output layer; see @ref PPE_PIXEL_FORMAT. */
	PPE_ADDR_TYPE type;      /*!< Address type of the output buffer; see @ref PPE_ADDR_TYPE. */
} PPE_ResultLayer_InitTypeDef;

/** @} */

/// @cond
/* Registers Definitions --------------------------------------------------------*/

/** @defgroup PPE_Register_Definitions PPE Register Definitions
 * @{
 *****************************************************************************/

/** @defgroup GLB_CTL
 * @brief Global Control
 * @{
 **/
#define PPE_CHN_INACT_POS                              (15)
#define PPE_CHN_INACT_MSK                              (0x1FFFF << PPE_CHN_INACT_POS)
#define PPE_CHN_INACT_CLR                              (~PPE_CHN_INACT_MSK)
#define PPE_ALL_INACT_POS                               (4)
#define PPE_ALL_INACT_MSK                               (0x1 << PPE_ALL_INACT_POS)
#define PPE_ALL_INACT_CLR                               (~PPE_ALL_INACT_MSK)
#define PPE_RESUME_POS                               (3)
#define PPE_RESUME_MSK                               (0x1 << PPE_RESUME_POS)
#define PPE_RESUME_CLR                               (~PPE_RESUME_MSK)
#define PPE_SUSPEND_POS                               (2)
#define PPE_SUSPEND_MSK                               (0x1 << PPE_SUSPEND_POS)
#define PPE_SUSPEND_CLR                               (~PPE_SUSPEND_MSK)
#define PPE_ABORT_POS                               (1)
#define PPE_ABORT_MSK                               (0x1 << PPE_ABORT_POS)
#define PPE_ABORT_CLR                               (~PPE_ABORT_MSK)
#define PPE_GLB_EN_POS                               (0)
#define PPE_GLB_EN_MSK                               (0x1 << PPE_GLB_EN_POS)
#define PPE_GLB_EN_CLR                               (~PPE_GLB_EN_MSK)
/** @} */

/** @defgroup FUNC_CFG
 * @brief Function Configuration
 * @{
 **/
#define PPE_BLEND_LAYER_NUM_POS                              (4)
#define PPE_BLEND_LAYER_NUM_MSK                              (0xF << PPE_BLEND_LAYER_NUM_POS)
#define PPE_BLEND_LAYER_NUM_CLR                              (~PPE_BLEND_LAYER_NUM_MSK)
#define PPE_FUNC_SEL_POS                              (0)
#define PPE_FUNC_SEL_MSK                              (0xF << PPE_FUNC_SEL_POS)
#define PPE_FUNC_SEL_CLR                              (~PPE_FUNC_SEL_MSK)
/** @} */

/** @defgroup RELOAD_CFG
 * @brief Reload Configuration
 * @{
 **/
#define PPE_AUTO_CLR_POS                              (17)
#define PPE_AUTO_CLR_MSK                              (0x1 << PPE_AUTO_CLR_POS)
#define PPE_AUTO_CLR_CLR                              (~PPE_AUTO_CLR_MSK)
#define PPE_SET_VALID_POS                              (16)
#define PPE_SET_VALID_MSK                              (0x1 << PPE_SET_VALID_POS)
#define PPE_SET_VALID_CLR                              (~PPE_SET_VALID_MSK)
#define PPE_LAYER_RELOAD_EN_POS                              (0)
#define PPE_LAYER_RELOAD_EN_MSK                              (0xFFFF << PPE_LAYER_RELOAD_EN_POS)
#define PPE_LAYER_RELOAD_EN_CLR                              (~PPE_LAYER_RELOAD_EN_MSK)
/** @} */

/** @defgroup LL_CFG
 * @brief Link List Configuration
 * @{
 **/
#define PPE_LL_MAX_AXLEN_POS                              (24)
#define PPE_LL_MAX_AXLEN_MSK                              (0xFF << PPE_LL_MAX_AXLEN_POS)
#define PPE_LL_MAX_AXLEN_CLR                              (~PPE_LL_MAX_AXLEN_MSK)
#define PPE_LL_INCR_POS                              (19)
#define PPE_LL_INCR_MSK                              (0x1 << PPE_LL_INCR_POS)
#define PPE_LL_INCR_CLR                              (~PPE_LL_INCR_MSK)
#define PPE_LL_AXSIZE_POS                              (16)
#define PPE_LL_AXSIZE_MSK                              (0x7 << PPE_LL_AXSIZE_POS)
#define PPE_LL_AXSIZE_CLR                              (~PPE_LL_AXSIZE_MSK)
#define PPE_LAYER_LL_EN_POS                              (0)
#define PPE_LAYER_LL_EN_MSK                              (0xFFFF << PPE_LAYER_LL_EN_POS)
#define PPE_LAYER_LL_EN_CLR                              (~PPE_LAYER_LL_EN_MSK)
/** @} */

/** @defgroup LLP
 * @brief Link List Pointer
 * @{
 **/
#define PPE_LLP_POS                              (0)
#define PPE_LLP_MSK                              (0xFFFFFFFF << PPE_LLP_POS)
#define PPE_LLP_CLR                              (~PPE_LLP_MSK)
/** @} */

/** @defgroup SCA_RATIO_X
 * @brief Scale Ratio X
 * @{
 **/
#define PPE_SCA_RATIO_X_POS                              (0)
#define PPE_SCA_RATIO_X_MSK                              (0xFFFFFFFF << PPE_SCA_RATIO_X_POS)
#define PPE_SCA_RATIO_X_CLR                              (~PPE_SCA_RATIO_X_MSK)
/** @} */

/** @defgroup SCA_RATIO_Y
 * @brief Scale Ratio Y
 * @{
 **/
#define PPE_SCA_RATIO_Y_POS                              (0)
#define PPE_SCA_RATIO_Y_MSK                              (0xFFFFFFFF << PPE_SCA_RATIO_Y_POS)
#define PPE_SCA_RATIO_Y_CLR                              (~PPE_SCA_RATIO_Y_MSK)
/** @} */

/** @defgroup MISC
 * @brief MISC Configuration
 * @{
 **/
#define PPE_MISC_POS                              (0)
#define PPE_MISC_MSK                              (0x1 << PPE_MISC_POS)
#define PPE_MISC_CLR                              (~PPE_MISC_MSK)
/** @} */

/** @defgroup INTR_STATUS
 * @brief Interrupt Status
 * @{
 **/
#define PPE_INTR_STATUS_CHN_BUS_ERR_POS                              (15)
#define PPE_INTR_STATUS_CHN_BUS_ERR_MSK                              (0x1FFFF << PPE_INTR_STATUS_CHN_BUS_ERR_POS)
#define PPE_INTR_STATUS_CHN_BUS_ERR_CLR                              (~PPE_INTR_STATUS_CHN_BUS_ERR_MSK)
#define PPE_INTR_STATUS_SET_ERR_POS                              (6)
#define PPE_INTR_STATUS_SET_ERR_MSK                              (0x1 << PPE_INTR_STATUS_SET_ERR_POS)
#define PPE_INTR_STATUS_SET_ERR_CLR                              (~PPE_INTR_STATUS_SET_ERR_MSK)
#define PPE_INTR_STATUS_SECURE_ERR_POS                              (5)
#define PPE_INTR_STATUS_SECURE_ERR_MSK                              (0x1 << PPE_INTR_STATUS_SECURE_ERR_POS)
#define PPE_INTR_STATUS_SECURE_ERR_CLR                              (~PPE_INTR_STATUS_SECURE_ERR_MSK)
#define PPE_INTR_STATUS_SUSP_INAC_POS                              (4)
#define PPE_INTR_STATUS_SUSP_INAC_MSK                              (0x1 << PPE_INTR_STATUS_SUSP_INAC_POS)
#define PPE_INTR_STATUS_SUSP_INAC_CLR                              (~PPE_INTR_STATUS_SUSP_INAC_MSK)
#define PPE_INTR_STATUS_LINE_WL_POS                              (3)
#define PPE_INTR_STATUS_LINE_WL_MSK                              (0x1 << PPE_INTR_STATUS_LINE_WL_POS)
#define PPE_INTR_STATUS_LINE_WL_CLR                              (~PPE_INTR_STATUS_LINE_WL_MSK)
#define PPE_INTR_STATUS_LOAD_OVER_POS                              (2)
#define PPE_INTR_STATUS_LOAD_OVER_MSK                              (0x1 << PPE_INTR_STATUS_LOAD_OVER_POS)
#define PPE_INTR_STATUS_LOAD_OVER_CLR                              (~PPE_INTR_STATUS_LOAD_OVER_MSK)
#define PPE_INTR_STATUS_FR_OVER_POS                              (1)
#define PPE_INTR_STATUS_FR_OVER_MSK                              (0x1 << PPE_INTR_STATUS_FR_OVER_POS)
#define PPE_INTR_STATUS_FR_OVER_CLR                              (~PPE_INTR_STATUS_FR_OVER_MSK)
#define PPE_INTR_STATUS_ALL_OVER_POS                              (0)
#define PPE_INTR_STATUS_ALL_OVER_MSK                              (0x1 << PPE_INTR_STATUS_ALL_OVER_POS)
#define PPE_INTR_STATUS_ALL_OVER_CLR                              (~PPE_INTR_STATUS_ALL_OVER_MSK)
/** @} */

/** @defgroup INTR_RAW
 * @brief Interrupt Raw Status
 * @{
 **/
#define PPE_INTR_RAW_CHN_BUS_ERR_POS                              (15)
#define PPE_INTR_RAW_CHN_BUS_ERR_MSK                              (0x1FFFF << PPE_INTR_RAW_CHN_BUS_ERR_POS)
#define PPE_INTR_RAW_CHN_BUS_ERR_CLR                              (~PPE_INTR_RAW_CHN_BUS_ERR_MSK)
#define PPE_INTR_RAW_SET_ERR_POS                              (6)
#define PPE_INTR_RAW_SET_ERR_MSK                              (0x1 << PPE_INTR_RAW_SET_ERR_POS)
#define PPE_INTR_RAW_SET_ERR_CLR                              (~PPE_INTR_RAW_SET_ERR_MSK)
#define PPE_INTR_RAW_SECURE_ERR_POS                              (5)
#define PPE_INTR_RAW_SECURE_ERR_MSK                              (0x1 << PPE_INTR_RAW_SECURE_ERR_POS)
#define PPE_INTR_RAW_SECURE_ERR_CLR                              (~PPE_INTR_RAW_SECURE_ERR_MSK)
#define PPE_INTR_RAW_SUSP_INAC_POS                              (4)
#define PPE_INTR_RAW_SUSP_INAC_MSK                              (0x1 << PPE_INTR_RAW_SUSP_INAC_POS)
#define PPE_INTR_RAW_SUSP_INAC_CLR                              (~PPE_INTR_RAW_SUSP_INAC_MSK)
#define PPE_INTR_RAW_LINE_WL_POS                              (3)
#define PPE_INTR_RAW_LINE_WL_MSK                              (0x1 << PPE_INTR_RAW_LINE_WL_POS)
#define PPE_INTR_RAW_LINE_WL_CLR                              (~PPE_INTR_RAW_LINE_WL_MSK)
#define PPE_INTR_RAW_LOAD_OVER_POS                              (2)
#define PPE_INTR_RAW_LOAD_OVER_MSK                              (0x1 << PPE_INTR_RAW_LOAD_OVER_POS)
#define PPE_INTR_RAW_LOAD_OVER_CLR                              (~PPE_INTR_RAW_LOAD_OVER_MSK)
#define PPE_INTR_RAW_FR_OVER_POS                              (1)
#define PPE_INTR_RAW_FR_OVER_MSK                              (0x1 << PPE_INTR_RAW_FR_OVER_POS)
#define PPE_INTR_RAW_FR_OVER_CLR                              (~PPE_INTR_RAW_FR_OVER_MSK)
#define PPE_INTR_RAW_ALL_OVER_POS                              (0)
#define PPE_INTR_RAW_ALL_OVER_MSK                              (0x1 << PPE_INTR_RAW_ALL_OVER_POS)
#define PPE_INTR_RAW_ALL_OVER_CLR                              (~PPE_INTR_RAW_ALL_OVER_MSK)
/** @} */

/** @defgroup INTR_CLR
 * @brief Interrupt Clear
 * @{
 **/
#define PPE_INTR_CLR_CHN_BUS_ERR_POS                              (15)
#define PPE_INTR_CLR_CHN_BUS_ERR_MSK                              (0x1FFFF << PPE_INTR_CLR_CHN_BUS_ERR_POS)
#define PPE_INTR_CLR_CHN_BUS_ERR_CLR                              (~PPE_INTR_CLR_CHN_BUS_ERR_MSK)
#define PPE_INTR_CLR_SET_ERR_POS                              (6)
#define PPE_INTR_CLR_SET_ERR_MSK                              (0x1 << PPE_INTR_CLR_SET_ERR_POS)
#define PPE_INTR_CLR_SET_ERR_CLR                              (~PPE_INTR_CLR_SET_ERR_MSK)
#define PPE_INTR_CLR_SECURE_ERR_POS                              (5)
#define PPE_INTR_CLR_SECURE_ERR_MSK                              (0x1 << PPE_INTR_CLR_SECURE_ERR_POS)
#define PPE_INTR_CLR_SECURE_ERR_CLR                              (~PPE_INTR_CLR_SECURE_ERR_MSK)
#define PPE_INTR_CLR_SUSP_INAC_POS                              (4)
#define PPE_INTR_CLR_SUSP_INAC_MSK                              (0x1 << PPE_INTR_CLR_SUSP_INAC_POS)
#define PPE_INTR_CLR_SUSP_INAC_CLR                              (~PPE_INTR_CLR_SUSP_INAC_MSK)
#define PPE_INTR_CLR_LINE_WL_POS                              (3)
#define PPE_INTR_CLR_LINE_WL_MSK                              (0x1 << PPE_INTR_CLR_LINE_WL_POS)
#define PPE_INTR_CLR_LINE_WL_CLR                              (~PPE_INTR_CLR_LINE_WL_MSK)
#define PPE_INTR_CLR_LOAD_OVER_POS                              (2)
#define PPE_INTR_CLR_LOAD_OVER_MSK                              (0x1 << PPE_INTR_CLR_LOAD_OVER_POS)
#define PPE_INTR_CLR_LOAD_OVER_CLR                              (~PPE_INTR_CLR_LOAD_OVER_MSK)
#define PPE_INTR_CLR_FR_OVER_POS                              (1)
#define PPE_INTR_CLR_FR_OVER_MSK                              (0x1 << PPE_INTR_CLR_FR_OVER_POS)
#define PPE_INTR_CLR_FR_OVER_CLR                              (~PPE_INTR_CLR_FR_OVER_MSK)
#define PPE_INTR_CLR_ALL_OVER_POS                              (0)
#define PPE_INTR_CLR_ALL_OVER_MSK                              (0x1 << PPE_INTR_CLR_ALL_OVER_POS)
#define PPE_INTR_CLR_ALL_OVER_CLR                              (~PPE_INTR_CLR_ALL_OVER_MSK)
/** @} */

/** @defgroup INTR_MASK
 * @brief Interrupt Mask
 * @{
 **/
#define PPE_INTR_MASK_CHN_BUS_ERR_POS                              (15)
#define PPE_INTR_MASK_CHN_BUS_ERR_MSK                              (0x1FFFF << PPE_INTR_MASK_CHN_BUS_ERR_POS)
#define PPE_INTR_MASK_CHN_BUS_ERR_CLR                              (~PPE_INTR_MASK_CHN_BUS_ERR_MSK)
/* INTR_MASK[6]: SET_ERR, settings error */
#define PPE_INTR_MASK_SET_ERR_POS                              (6)
#define PPE_INTR_MASK_SET_ERR_MSK                              (0x1 << PPE_INTR_MASK_SET_ERR_POS)
#define PPE_INTR_MASK_SET_ERR_CLR                              (~PPE_INTR_MASK_SET_ERR_MSK)
/* INTR_MASK[5]: SECURE_ERR, secure error */
#define PPE_INTR_MASK_SECURE_ERR_POS                              (5)
#define PPE_INTR_MASK_SECURE_ERR_MSK                              (0x1 << PPE_INTR_MASK_SECURE_ERR_POS)
#define PPE_INTR_MASK_SECURE_ERR_CLR                              (~PPE_INTR_MASK_SECURE_ERR_MSK)
/* INTR_MASK[4]: SUSP_INAC, suspend inactive */
#define PPE_INTR_MASK_SUSP_INAC_POS                              (4)
#define PPE_INTR_MASK_SUSP_INAC_MSK                              (0x1 << PPE_INTR_MASK_SUSP_INAC_POS)
#define PPE_INTR_MASK_SUSP_INAC_CLR                              (~PPE_INTR_MASK_SUSP_INAC_MSK)
/* INTR_MASK[3]: LINE_WL,  */
#define PPE_INTR_MASK_LINE_WL_POS                              (3)
#define PPE_INTR_MASK_LINE_WL_MSK                              (0x1 << PPE_INTR_MASK_LINE_WL_POS)
#define PPE_INTR_MASK_LINE_WL_CLR                              (~PPE_INTR_MASK_LINE_WL_MSK)
/* INTR_MASK[2]: LOAD_OVER,  */
#define PPE_INTR_MASK_LOAD_OVER_POS                              (2)
#define PPE_INTR_MASK_LOAD_OVER_MSK                              (0x1 << PPE_INTR_MASK_LOAD_OVER_POS)
#define PPE_INTR_MASK_LOAD_OVER_CLR                              (~PPE_INTR_MASK_LOAD_OVER_MSK)
/* INTR_MASK[1]: FR_OVER,  */
#define PPE_INTR_MASK_FR_OVER_POS                              (1)
#define PPE_INTR_MASK_FR_OVER_MSK                              (0x1 << PPE_INTR_MASK_FR_OVER_POS)
#define PPE_INTR_MASK_FR_OVER_CLR                              (~PPE_INTR_MASK_FR_OVER_MSK)
/* INTR_MASK[0]: ALL_OVER,  */
#define PPE_INTR_MASK_ALL_OVER_POS                              (0)
#define PPE_INTR_MASK_ALL_OVER_MSK                              (0x1 << PPE_INTR_MASK_ALL_OVER_POS)
#define PPE_INTR_MASK_ALL_OVER_CLR                              (~PPE_INTR_MASK_ALL_OVER_MSK)
/** @} */


/** @defgroup ADDR
 * @brief Address of First Pixel
 * @{
 **/
#define RESULT_LAYER_ADDR_POS                              (0)
#define RESULT_LAYER_ADDR_MSK                              (0xFFFFFFFFF << RESULT_LAYER_ADDR_POS)
#define RESULT_LAYER_ADDR_CLR                              (~RESULT_LAYER_ADDR_MSK)
/** @} */

/** @defgroup POS
 * @brief Position of First Pixel in Result Window
 * @{
 **/
#define RESULT_LAYER_POS_Y_POS                              (16)
#define RESULT_LAYER_POS_Y_MSK                              (0xFFFF << RESULT_LAYER_POS_Y_POS)
#define RESULT_LAYER_POS_Y_CLR                              (~RESULT_LAYER_POS_Y_MSK)
#define RESULT_LAYER_POS_X_POS                              (0)
#define RESULT_LAYER_POS_X_MSK                              (0xFFFF << RESULT_LAYER_POS_X_POS)
#define RESULT_LAYER_POS_X_CLR                              (~RESULT_LAYER_POS_X_MSK)
/** @} */

/** @defgroup WIN_SIZE
 * @brief Windows Size
 * @{
 **/
#define RESULT_LAYER_HEIGHT_POS                              (16)
#define RESULT_LAYER_HEIGHT_MSK                              (0xFFFF << RESULT_LAYER_HEIGHT_POS)
#define RESULT_LAYER_HEIGHT_CLR                              (~RESULT_LAYER_HEIGHT_MSK)
#define RESULT_LAYER_WIDTH_POS                              (0)
#define RESULT_LAYER_WIDTH_MSK                              (0xFFFF << RESULT_LAYER_WIDTH_POS)
#define RESULT_LAYER_WIDTH_CLR                              (~RESULT_LAYER_WIDTH_MSK)
/** @} */

/** @defgroup PIC_CFG
 * @brief Picture Configuration
 * @{
 **/
#define RESULT_LAYER_LINE_LEN_POS                              (16)
#define RESULT_LAYER_LINE_LEN_MSK                              (0xFFFF << RESULT_LAYER_LINE_LEN_POS)
#define RESULT_LAYER_LINE_LEN_CLR                              (~RESULT_LAYER_LINE_LEN_MSK)
#define RESULT_LAYER_PIX_FORMAT_POS                              (0)
#define RESULT_LAYER_PIX_FORMAT_MSK                              (0xFF << RESULT_LAYER_PIX_FORMAT_POS)
#define RESULT_LAYER_PIX_FORMAT_CLR                              (~RESULT_LAYER_PIX_FORMAT_MSK)
/** @} */

/** @defgroup BUS_CFG
 * @brief DMA BUS Configuration
 * @{
 **/
#define RESULT_LAYER_PRIOR_POS                              (12)
#define RESULT_LAYER_PRIOR_MSK                              (0xF << RESULT_LAYER_PRIOR_POS)
#define RESULT_LAYER_PRIOR_CLR                              (~RESULT_LAYER_PRIOR_MSK)
#define RESULT_LAYER_MAX_AXLEN_LOG_POS                              (8)
#define RESULT_LAYER_MAX_AXLEN_LOG_MSK                              (0x7 << RESULT_LAYER_MAX_AXLEN_LOG_POS)
#define RESULT_LAYER_MAX_AXLEN_LOG_CLR                              (~RESULT_LAYER_MAX_AXLEN_LOG_MSK)
#define RESULT_LAYER_AXCACHE_POS                              (4)
#define RESULT_LAYER_AXCACHE_MSK                              (0xF << RESULT_LAYER_AXCACHE_POS)
#define RESULT_LAYER_AXCACHE_CLR                              (~RESULT_LAYER_AXCACHE_MSK)
#define RESULT_LAYER_INCR_POS                              (3)
#define RESULT_LAYER_INCR_MSK                              (0x1 << RESULT_LAYER_INCR_POS)
#define RESULT_LAYER_INCR_CLR                              (~RESULT_LAYER_INCR_MSK)
#define RESULT_LAYER_AXSIZE_POS                              (0)
#define RESULT_LAYER_AXSIZE_MSK                              (0x3 << RESULT_LAYER_AXSIZE_POS)
#define RESULT_LAYER_AXSIZE_CLR                              (~RESULT_LAYER_AXSIZE_MSK)
/** @} */

/** @defgroup HS_CFG
 * @brief DMA Handshake Configuration
 * @{
 **/
#define RESULT_LAYER_HW_INDEX_POS                              (8)
#define RESULT_LAYER_HW_INDEX_MSK                              (0xFF << RESULT_LAYER_HW_INDEX_POS)
#define RESULT_LAYER_HW_INDEX_CLR                              (~RESULT_LAYER_HW_INDEX_MSK)
#define RESULT_LAYER_MSIZE_LOG_POS                              (4)
#define RESULT_LAYER_MSIZE_LOG_MSK                              (0xF << RESULT_LAYER_MSIZE_LOG_POS)
#define RESULT_LAYER_MSIZE_LOG_CLR                              (~RESULT_LAYER_MSIZE_LOG_MSK)
#define RESULT_LAYER_HS_EN_POS                              (2)
#define RESULT_LAYER_HS_EN_MSK                              (0x1 << RESULT_LAYER_HS_EN_POS)
#define RESULT_LAYER_HS_EN_CLR                              (~RESULT_LAYER_HS_EN_MSK)
#define RESULT_LAYER_POLAR_POS                              (1)
#define RESULT_LAYER_POLAR_MSK                              (0x1 << RESULT_LAYER_POLAR_POS)
#define RESULT_LAYER_POLAR_CLR                              (~RESULT_LAYER_POLAR_MSK)
#define RESULT_LAYER_HW_HS_POS                              (0)
#define RESULT_LAYER_HW_HS_MSK                              (0x1 << RESULT_LAYER_HW_HS_POS)
#define RESULT_LAYER_HW_HS_CLR                              (~RESULT_LAYER_HW_HS_MSK)
/** @} */



/** @defgroup ADDR
 * @brief Address of First Pixel
 * @{
 **/
#define INPUT_LAYER_ADDR_POS                              (0)
#define INPUT_LAYER_ADDR_MSK                              (0xFFFFFFFFF << INPUT_LAYER_ADDR_POS)
#define INPUT_LAYER_ADDR_CLR                              (~INPUT_LAYER_ADDR_MSK)
/** @} */


/** @defgroup POS
 * @brief Position of First Pixel in Result Window
 * @{
 **/
#define INPUT_LAYER_POS_Y_POS                              (16)
#define INPUT_LAYER_POS_Y_MSK                              (0xFFFF << INPUT_LAYER_POS_Y_POS)
#define INPUT_LAYER_POS_Y_CLR                              (~INPUT_LAYER_POS_Y_MSK)
#define INPUT_LAYER_POS_X_POS                              (0)
#define INPUT_LAYER_POS_X_MSK                              (0xFFFF << INPUT_LAYER_POS_X_POS)
#define INPUT_LAYER_POS_X_CLR                              (~INPUT_LAYER_POS_X_MSK)
/** @} */


/** @defgroup WIN_SIZE
 * @brief Windows Size
 * @{
 **/
#define INPUT_LAYER_HEIGHT_POS                              (16)
#define INPUT_LAYER_HEIGHT_MSK                              (0xFFFF << INPUT_LAYER_HEIGHT_POS)
#define INPUT_LAYER_HEIGHT_CLR                              (~INPUT_LAYER_HEIGHT_MSK)
#define INPUT_LAYER_WIDTH_POS                              (0)
#define INPUT_LAYER_WIDTH_MSK                              (0xFFFF << INPUT_LAYER_WIDTH_POS)
#define INPUT_LAYER_WIDTH_CLR                              (~INPUT_LAYER_WIDTH_MSK)
/** @} */

/** @defgroup CONST_PIX
 * @brief Constant Pixel
 * @{
 **/
#define INPUT_LAYER_CONST_PIX_POS                              (0)
#define INPUT_LAYER_CONST_PIX_MSK                              (0xFFFFFFFF << INPUT_LAYER_CONST_PIX_POS)
#define INPUT_LAYER_CONST_PIX_CLR                              (~INPUT_LAYER_CONST_PIX_MSK)
/** @} */

/** @defgroup PIC_CFG
 * @brief Picture Configuration
 * @{
 **/
#define INPUT_LAYER_LINE_LEN_POS                              (16)
#define INPUT_LAYER_LINE_LEN_MSK                              (0xFFFF << INPUT_LAYER_LINE_LEN_POS)
#define INPUT_LAYER_LINE_LEN_CLR                              (~INPUT_LAYER_LINE_LEN_MSK)
#define INPUT_LAYER_KEY_EN_POS                              (12)
#define INPUT_LAYER_KEY_EN_MSK                              (0x1 << INPUT_LAYER_KEY_EN_POS)
#define INPUT_LAYER_KEY_EN_CLR                              (~INPUT_LAYER_KEY_EN_MSK)
#define INPUT_LAYER_PIX_SRC_POS                              (8)
#define INPUT_LAYER_PIX_SRC_MSK                              (0x3 << INPUT_LAYER_PIX_SRC_POS)
#define INPUT_LAYER_PIX_SRC_CLR                              (~INPUT_LAYER_PIX_SRC_MSK)
#define INPUT_LAYER_PIX_FORMAT_POS                              (0)
#define INPUT_LAYER_PIX_FORMAT_MSK                              (0xFF << INPUT_LAYER_PIX_FORMAT_POS)
#define INPUT_LAYER_PIX_FORMAT_CLR                              (~INPUT_LAYER_PIX_FORMAT_MSK)
/** @} */

/** @defgroup KEY_COLOR
 * @brief Transparent Color
 * @{
 **/
#define INPUT_LAYER_KEY_COLOR_POS                              (0)
#define INPUT_LAYER_KEY_COLOR_MSK                              (0xFFFFFFFF << INPUT_LAYER_KEY_COLOR_POS)
#define INPUT_LAYER_KEY_COLOR_CLR                              (~INPUT_LAYER_KEY_COLOR_MSK)
/** @} */


/** @defgroup BUS_CFG
 * @brief DMA BUS Configuration
 * @{
 **/
#define INPUT_LAYER_PRIOR_POS                              (12)
#define INPUT_LAYER_PRIOR_MSK                              (0xF << INPUT_LAYER_PRIOR_POS)
#define INPUT_LAYER_PRIOR_CLR                              (~INPUT_LAYER_PRIOR_MSK)
#define INPUT_LAYER_MAX_AXLEN_LOG_POS                              (8)
#define INPUT_LAYER_MAX_AXLEN_LOG_MSK                              (0x7 << INPUT_LAYER_MAX_AXLEN_LOG_POS)
#define INPUT_LAYER_MAX_AXLEN_LOG_CLR                              (~INPUT_LAYER_MAX_AXLEN_LOG_MSK)
#define INPUT_LAYER_AXCACHE_POS                              (4)
#define INPUT_LAYER_AXCACHE_MSK                              (0xF << INPUT_LAYER_AXCACHE_POS)
#define INPUT_LAYER_AXCACHE_CLR                              (~INPUT_LAYER_AXCACHE_MSK)
#define INPUT_LAYER_INCR_POS                              (3)
#define INPUT_LAYER_INCR_MSK                              (0x1 << INPUT_LAYER_INCR_POS)
#define INPUT_LAYER_INCR_CLR                              (~INPUT_LAYER_INCR_MSK)
#define INPUT_LAYER_AXSIZE_POS                              (0)
#define INPUT_LAYER_AXSIZE_MSK                              (0x3 << INPUT_LAYER_AXSIZE_POS)
#define INPUT_LAYER_AXSIZE_CLR                              (~INPUT_LAYER_AXSIZE_MSK)
/** @} */

/** @defgroup HS_CFG
 * @brief DMA Handshake Configuration
 * @{
 **/
#define INPUT_LAYER_HW_INDEX_POS                              (8)
#define INPUT_LAYER_HW_INDEX_MSK                              (0xFF << INPUT_LAYER_HW_INDEX_POS)
#define INPUT_LAYER_HW_INDEX_CLR                              (~INPUT_LAYER_HW_INDEX_MSK)
#define INPUT_LAYER_MSIZE_LOG_POS                              (4)
#define INPUT_LAYER_MSIZE_LOG_MSK                              (0xF << INPUT_LAYER_MSIZE_LOG_POS)
#define INPUT_LAYER_MSIZE_LOG_CLR                              (~INPUT_LAYER_MSIZE_LOG_MSK)
#define INPUT_LAYER_HS_EN_POS                              (2)
#define INPUT_LAYER_HS_EN_MSK                              (0x1 << INPUT_LAYER_HS_EN_POS)
#define INPUT_LAYER_HS_EN_CLR                              (~INPUT_LAYER_HS_EN_MSK)
#define INPUT_LAYER_POLAR_POS                              (1)
#define INPUT_LAYER_POLAR_MSK                              (0x1 << INPUT_LAYER_POLAR_POS)
#define INPUT_LAYER_POLAR_CLR                              (~INPUT_LAYER_POLAR_MSK)
#define INPUT_LAYER_HW_HS_POS                              (0)
#define INPUT_LAYER_HW_HS_MSK                              (0x1 << INPUT_LAYER_HW_HS_POS)
#define INPUT_LAYER_HW_HS_CLR                              (~INPUT_LAYER_HW_HS_MSK)
/** @} */

/** @defgroup PPE_INTR_ST PPE Interrupt Status Bits
  * @brief Interrupt status bits for PPE_ClearINTPendingBit and PPE_MaskINTConfig.
  * @{
  */
#define PPE_ALL_OVER_INT                                ((u32)0x1 << PPE_INTR_STATUS_ALL_OVER_POS)
#define PPE_FR_OVER_INT                                 ((u32)0x1 << PPE_INTR_STATUS_FR_OVER_POS)
#define PPE_LOAD_OVER_INT                               ((u32)0x1 << PPE_INTR_STATUS_LOAD_OVER_POS)
#define PPE_LINE_WL_INT                                 ((u32)0x1 << PPE_INTR_STATUS_LINE_WL_POS)
#define PPE_SUSP_INAC_INT                               ((u32)0x1 << PPE_INTR_STATUS_SUSP_INAC_POS)
/** @} */

/** @defgroup PPE_INTR_ERR PPE Interrupt Error Bits
  * @brief Interrupt error bits for internal use.
  * @{
  */
#define PPE_SECURE_ERR_INT                              ((u32)0x1 << PPE_INTR_STATUS_SECURE_ERR_POS)
#define PPE_SET_ERR_INT                                 ((u32)0x1 << PPE_INTR_STATUS_SET_ERR_POS)
#define PPE_BUS1_ERR_INT                                ((u32)0x1 << PPE_INTR_STATUS_CHN_BUS_ERR_POS)
#define PPE_BUS2_ERR_INT                                ((u32)0x2 << PPE_INTR_STATUS_CHN_BUS_ERR_POS)
#define PPE_BUS3_ERR_INT                                ((u32)0x4 << PPE_INTR_STATUS_CHN_BUS_ERR_POS)
#define PPE_BUS4_ERR_INT                                ((u32)0x8 << PPE_INTR_STATUS_CHN_BUS_ERR_POS)
#define PPE_BUS5_ERR_INT                                ((u32)0x10 << PPE_INTR_STATUS_CHN_BUS_ERR_POS)
#define PPE_BUS6_ERR_INT                                ((u32)0x20 << PPE_INTR_STATUS_CHN_BUS_ERR_POS)
#define PPE_BUS7_ERR_INT                                ((u32)0x40 << PPE_INTR_STATUS_CHN_BUS_ERR_POS)
#define PPE_BUS8_ERR_INT                                ((u32)0x80 << PPE_INTR_STATUS_CHN_BUS_ERR_POS)
#define PPE_BUS9_ERR_INT                                ((u32)0x100 << PPE_INTR_STATUS_CHN_BUS_ERR_POS)
#define PPE_BUS10_ERR_INT                               ((u32)0x200 << PPE_INTR_STATUS_CHN_BUS_ERR_POS)
#define PPE_BUS11_ERR_INT                               ((u32)0x400 << PPE_INTR_STATUS_CHN_BUS_ERR_POS)
#define PPE_BUS12_ERR_INT                               ((u32)0x800 << PPE_INTR_STATUS_CHN_BUS_ERR_POS)
#define PPE_BUS13_ERR_INT                               ((u32)0x1000 << PPE_INTR_STATUS_CHN_BUS_ERR_POS)
#define PPE_BUS14_ERR_INT                               ((u32)0x2000 << PPE_INTR_STATUS_CHN_BUS_ERR_POS)
#define PPE_BUS15_ERR_INT                               ((u32)0x4000 << PPE_INTR_STATUS_CHN_BUS_ERR_POS)
#define PPE_BUS16_ERR_INT                               ((u32)0x8000 << PPE_INTR_STATUS_CHN_BUS_ERR_POS)
#define PPE_BUS17_ERR_INT                               ((u32)0x10000 << PPE_INTR_STATUS_CHN_BUS_ERR_POS)
/** @} */

/**
  * @}
  */
/// @endcond

/* Exported constants --------------------------------------------------------*/
/** @defgroup PPE_Exported_Constants PPE Exported Constants
  * @{
  */

/** @brief Check if PPE channel index is valid (0-16). */
#define IS_PPE_CHN_INDEX(chn)                               (((chn) < 17) && ((chn) >= 0))

#define PPE_FUNCTION_ALPHA_BLEND                        ((u32)0x0)   /*!< PPE function mode: alpha blending. */
#define PPE_FUNCTION_SCALE                              ((u32)0x1)   /*!< PPE function mode: scaling. */
/** @brief Check if PPE function selection is valid. */
#define IS_PPE_FUNCTION(func)                           (((func) == PPE_FUNCTION_ALPHA_BLEND) || ((func) == PPE_FUNCTION_SCALE))

/** @brief Check if PPE alpha blend layer count is valid (< 16). */
#define IS_PPE_ALPHA_BLEND_LAYER_NUM(layer_num)         ((layer_num) < 16)

#define PPE_RESULT_LAYER_INDEX                          0   /*!< Index of the PPE result (output) layer. */
#define PPE_INPUT_LAYER1_INDEX                          1   /*!< Index of PPE input layer 1. */
#define PPE_INPUT_LAYER2_INDEX                          2   /*!< Index of PPE input layer 2. */
#define PPE_INPUT_LAYER3_INDEX                          3   /*!< Index of PPE input layer 3. */

#define PPE_RESULT_LAYER_BIT                          ((u32)0x1 << PPE_RESULT_LAYER_INDEX)   /*!< Bit mask for PPE result layer selection. */
#define PPE_INPUT_LAYER1_BIT                          ((u32)0x1 << PPE_INPUT_LAYER1_INDEX)   /*!< Bit mask for PPE input layer 1 selection. */
#define PPE_INPUT_LAYER2_BIT                          ((u32)0x1 << PPE_INPUT_LAYER2_INDEX)   /*!< Bit mask for PPE input layer 2 selection. */
#define PPE_INPUT_LAYER3_BIT                          ((u32)0x1 << PPE_INPUT_LAYER3_INDEX)   /*!< Bit mask for PPE input layer 3 selection. */

/** @} */

/* Exported functions --------------------------------------------------------*/
/** @defgroup PPE_Exported_Functions PPE Exported Functions
  * @{
  */

void PPE_Cmd(int state);
u32 PPE_GetGlobalState(void);
void PPE_Suspend(void);
void PPE_Resume(void);
void PPE_Abort(void);
int PPE_ChannelInactiveCheck(uint8_t chn_id);
void PPE_SetScaleRation(float scale_x, float scale_y);
u32 PPE_GetAllIntStatus(void);
void PPE_ClearINTPendingBit(u32 PPE_INT);
void PPE_MaskINTConfig(u32 PPE_INT_MSK, int NewState);
void PPE_MaskAllInt(void);
void PPE_AutoReloadSetLayer(u32 layers);
void PPE_AutoReloadClrLayerAll(void);
void PPE_AutoClearCmd(int NewState);
void PPE_SetValid(void);
u32 PPE_GetValid(void);
void PPE_SetLLP(u32      lli);
void PPE_LLSetLayer(u32 layers);
void PPE_StructInit(PPE_InitTypeDef *PPE_init_struct);
void PPE_InputLayer_StructInit(PPE_InputLayer_InitTypeDef *layer_init_struct);
void PPE_ResultLayer_StructInit(PPE_ResultLayer_InitTypeDef *layer_init_struct);
void PPE_Init(PPE_InitTypeDef *PPE_init_struct);
void PPE_InitInputLayer(uint8_t id, PPE_InputLayer_InitTypeDef *layer_init_struct);
void PPE_InitResultLayer(PPE_ResultLayer_InitTypeDef *layer_init_struct);
void PPE_SetLine(u32 line);
/**
  * @}
  */


#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

/**
  * @}
  */

/**
  * @}
  */

#endif
