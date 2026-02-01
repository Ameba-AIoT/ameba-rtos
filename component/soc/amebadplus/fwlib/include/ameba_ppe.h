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

/** @defgroup PPE
* @brief PPE driver modules
* @{
*/

/* AUTO_GEN_START */


/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


/* Exported types ------------------------------------------------------------*/
/** @defgroup PPE_Exported_Types PPE Exported Types
  * @{
  */

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

/**
  * @brief  PPE Result Layer Address Type
  */
typedef enum {
	PPE_ADDR_MEM,
	PPE_ADDR_QSPI,
} PPE_ADDR_TYPE;

/**
  * @brief  PPE Pixel Format Definition
  */
typedef enum {
	PPE_ABGR8888 = 0x0,
	PPE_ARGB8888,
	PPE_XBGR8888,
	PPE_XRGB8888,
	PPE_BGRA8888,
	PPE_RGBA8888,
	PPE_BGRX8888,
	PPE_RGBX8888,
	PPE_ABGR4444,
	PPE_ARGB4444,
	PPE_XBGR4444,
	PPE_XRGB4444,
	PPE_BGRA4444,
	PPE_RGBA4444,
	PPE_BGRX4444,
	PPE_RGBX4444,
	PPE_ABGR2222,	//0x10
	PPE_ARGB2222,
	PPE_XBGR2222,
	PPE_XRGB2222,
	PPE_BGRA2222,
	PPE_RGBA2222,
	PPE_BGRX2222,
	PPE_RGBX2222,
	PPE_ABGR8565,
	PPE_ARGB8565,
	PPE_XBGR8565,
	PPE_XRGB8565,
	PPE_BGRA5658,
	PPE_RGBA5658,
	PPE_BGRX5658,
	PPE_RGBX5658,
	PPE_ABGR1555,	//0x20
	PPE_ARGB1555,
	PPE_XBGR1555,
	PPE_XRGB1555,
	PPE_BGRA5551,
	PPE_RGBA5551,
	PPE_BGRX5551,
	PPE_RGBX5551,
	PPE_BGR888,
	PPE_RGB888,
	PPE_BGR565,
	PPE_RGB565,
	PPE_A8,
	PPE_X8,
	PPE_ABGR8666 = 0x32,
	PPE_ARGB8666,
	PPE_XBGR8666,
	PPE_XRGB8666,
	PPE_BGRA6668,
	PPE_RGBA6668,
	PPE_BGRX6668,
	PPE_RGBX6668,
} PPE_PIXEL_FORMAT;

/**
  * @brief  PPE Pixel Source Definition
  */
typedef enum {
	PPE_LAYER_SRC_CONST,
	PPE_LAYER_SRC_FROM_DMA,
} PPE_PIXEL_SOURCE;


/**
  * @brief  PPE DMA Handshake Definition
  */
typedef enum {
	PPE_DMA_HW_HANDSHAKE,
	PPE_DMA_SW_HANDSHAKE,
} PPE_DMA_HANDSHAKE;


/**
  * @brief  PPE DMA Msize Log Definition
  */
typedef enum {
	PPE_MSIZE_1,
	PPE_MSIZE_2,
	PPE_MSIZE_4,
	PPE_MSIZE_8,
	PPE_MSIZE_16,
	PPE_MSIZE_32,
	PPE_MSIZE_64,
	PPE_MSIZE_128,
	PPE_MSIZE_256,
	PPE_MSIZE_512,
	PPE_MSIZE_1024,
} PPE_MSIZE_LOG;


/**
  * @brief  PPE Max AXI Length Definition
  */
typedef enum {
	PPE_MAX_AXLEN_0,
	PPE_MAX_AXLEN_1,
	PPE_MAX_AXLEN_3,
	PPE_MAX_AXLEN_7,
	PPE_MAX_AXLEN_15,
	PPE_MAX_AXLEN_31,
	PPE_MAX_AXLEN_63,
	PPE_MAX_AXLEN_127,
} PPE_MAX_AXLEN;


/**
  * @brief  PPE DMA Handshake Polarity Definition
  */
typedef enum {
	PPE_POLARITY_HIGH,
	PPE_POLARITY_LOW,
} PPE_ACTIVE_POLARITY;


/**
  * @brief  PPE Arbusrt Definition
  */
typedef enum {
	PPE_ARBURST_FIX,
	PPE_ARBURST_INCR,
} PPE_ARBURST;


/**
  * @brief  PPE Byte Swap Definition
  */
typedef enum {
	PPE_NO_SWAP,
	PPE_8BIT_SWAP,
	PPE_16BIT_SWAP,
	PPE_8_16BIT_SWAP,
	PPE_32BIT_SWAP,
	PPE_32_8BIT_SWAP,
	PPE_32_16BIT_SWAP,
	PPE_32_16_8BIT_SWAP,
} PPE_BYTE_SWAP;

/**
  * @brief  PPE LLI Layer Format Definition
  */
typedef struct {
	u32 LAYER_ADDR;
	u32 LAYER_POS;
	u32 LAYER_WINSIZE;
	u32 LAYER_CONST_PIX;
} PPE_LLI_LAYER;

/**
  * @brief  PPE Global register Format Definition
  */
typedef struct {
	u32 LL_CFG;
	u32 LLP;
	u32 SCA_RATIO_X;
	u32 SCA_RATIO_Y;
} PPE_LLI_GLB;

/**
  * @brief  PPE Function Structure Definition
  */
typedef struct {
	u32 function;
	u32 blend_layer_num;
} PPE_InitTypeDef;


/**
  * @brief  PPE Input layer Init Structure Definition
  */
typedef struct {
	u32 src_addr;
	u32 start_x;
	u32 start_y;
	u32 width;
	u32 height;
	u32 const_ABGR8888_value;
	u32 key_color_value;
	u32 line_len;
	PPE_PIXEL_FORMAT format;
	PPE_PIXEL_SOURCE src;
	int color_key_en;
} PPE_InputLayer_InitTypeDef;


/**
  * @brief  PPE Result layer Init Structure Definition
  */
typedef struct {
	u32 src_addr;
	u32 width;
	u32 height;
	u32 line_len;
	PPE_PIXEL_FORMAT format;
	PPE_ADDR_TYPE type;
} PPE_ResultLayer_InitTypeDef;
/** @} */

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
#define PPE_INT_CHN_BUS_ERR_POS                              (15)
#define PPE_INT_CHN_BUS_ERR_MSK                              (0x1FFFF << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_INT_CHN_BUS_ERR_CLR                              (~PPE_INT_CHN_BUS_ERR_MSK)
#define PPE_INT_SET_ERR_POS                              (6)
#define PPE_INT_SET_ERR_MSK                              (0x1 << PPE_INT_SET_ERR_POS)
#define PPE_INT_SET_ERR_CLR                              (~PPE_INT_SET_ERR_MSK)
#define PPE_INT_SECURE_ERR_POS                              (5)
#define PPE_INT_SECURE_ERR_MSK                              (0x1 << PPE_INT_SECURE_ERR_POS)
#define PPE_INT_SECURE_ERR_CLR                              (~PPE_INT_SECURE_ERR_MSK)
#define PPE_INT_SUSP_INAC_POS                              (4)
#define PPE_INT_SUSP_INAC_MSK                              (0x1 << PPE_INT_SUSP_INAC_POS)
#define PPE_INT_SUSP_INAC_CLR                              (~PPE_INT_SUSP_INAC_MSK)
#define PPE_INT_LINE_WL_POS                              (3)
#define PPE_INT_LINE_WL_MSK                              (0x1 << PPE_INT_LINE_WL_POS)
#define PPE_INT_LINE_WL_CLR                              (~PPE_INT_LINE_WL_MSK)
#define PPE_INT_LOAD_OVER_POS                              (2)
#define PPE_INT_LOAD_OVER_MSK                              (0x1 << PPE_INT_LOAD_OVER_POS)
#define PPE_INT_LOAD_OVER_CLR                              (~PPE_INT_LOAD_OVER_MSK)
#define PPE_INT_FR_OVER_POS                              (1)
#define PPE_INT_FR_OVER_MSK                              (0x1 << PPE_INT_FR_OVER_POS)
#define PPE_INT_FR_OVER_CLR                              (~PPE_INT_FR_OVER_MSK)
#define PPE_INT_ALL_OVER_POS                              (0)
#define PPE_INT_ALL_OVER_MSK                              (0x1 << PPE_INT_ALL_OVER_POS)
#define PPE_INT_ALL_OVER_CLR                              (~PPE_INT_ALL_OVER_MSK)
/** @} */

/** @defgroup INTR_RAW
 * @brief Interrupt Raw Status
 * @{
 **/
#define PPE_INT_CHN_BUS_ERR_POS                              (15)
#define PPE_INT_CHN_BUS_ERR_MSK                              (0x1FFFF << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_INT_CHN_BUS_ERR_CLR                              (~PPE_INT_CHN_BUS_ERR_MSK)
#define PPE_INT_SET_ERR_POS                              (6)
#define PPE_INT_SET_ERR_MSK                              (0x1 << PPE_INT_SET_ERR_POS)
#define PPE_INT_SET_ERR_CLR                              (~PPE_INT_SET_ERR_MSK)
#define PPE_INT_SECURE_ERR_POS                              (5)
#define PPE_INT_SECURE_ERR_MSK                              (0x1 << PPE_INT_SECURE_ERR_POS)
#define PPE_INT_SECURE_ERR_CLR                              (~PPE_INT_SECURE_ERR_MSK)
#define PPE_INT_SUSP_INAC_POS                              (4)
#define PPE_INT_SUSP_INAC_MSK                              (0x1 << PPE_INT_SUSP_INAC_POS)
#define PPE_INT_SUSP_INAC_CLR                              (~PPE_INT_SUSP_INAC_MSK)
#define PPE_INT_LINE_WL_POS                              (3)
#define PPE_INT_LINE_WL_MSK                              (0x1 << PPE_INT_LINE_WL_POS)
#define PPE_INT_LINE_WL_CLR                              (~PPE_INT_LINE_WL_MSK)
#define PPE_INT_LOAD_OVER_POS                              (2)
#define PPE_INT_LOAD_OVER_MSK                              (0x1 << PPE_INT_LOAD_OVER_POS)
#define PPE_INT_LOAD_OVER_CLR                              (~PPE_INT_LOAD_OVER_MSK)
#define PPE_INT_FR_OVER_POS                              (1)
#define PPE_INT_FR_OVER_MSK                              (0x1 << PPE_INT_FR_OVER_POS)
#define PPE_INT_FR_OVER_CLR                              (~PPE_INT_FR_OVER_MSK)
#define PPE_INT_ALL_OVER_POS                              (0)
#define PPE_INT_ALL_OVER_MSK                              (0x1 << PPE_INT_ALL_OVER_POS)
#define PPE_INT_ALL_OVER_CLR                              (~PPE_INT_ALL_OVER_MSK)
/** @} */

/** @defgroup INTR_CLR
 * @brief Interrupt Clear
 * @{
 **/
#define PPE_INT_CHN_BUS_ERR_POS                              (15)
#define PPE_INT_CHN_BUS_ERR_MSK                              (0x1FFFF << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_INT_CHN_BUS_ERR_CLR                              (~PPE_INT_CHN_BUS_ERR_MSK)
#define PPE_INT_SET_ERR_POS                              (6)
#define PPE_INT_SET_ERR_MSK                              (0x1 << PPE_INT_SET_ERR_POS)
#define PPE_INT_SET_ERR_CLR                              (~PPE_INT_SET_ERR_MSK)
#define PPE_INT_SECURE_ERR_POS                              (5)
#define PPE_INT_SECURE_ERR_MSK                              (0x1 << PPE_INT_SECURE_ERR_POS)
#define PPE_INT_SECURE_ERR_CLR                              (~PPE_INT_SECURE_ERR_MSK)
#define PPE_INT_SUSP_INAC_POS                              (4)
#define PPE_INT_SUSP_INAC_MSK                              (0x1 << PPE_INT_SUSP_INAC_POS)
#define PPE_INT_SUSP_INAC_CLR                              (~PPE_INT_SUSP_INAC_MSK)
#define PPE_INT_LINE_WL_POS                              (3)
#define PPE_INT_LINE_WL_MSK                              (0x1 << PPE_INT_LINE_WL_POS)
#define PPE_INT_LINE_WL_CLR                              (~PPE_INT_LINE_WL_MSK)
#define PPE_INT_LOAD_OVER_POS                              (2)
#define PPE_INT_LOAD_OVER_MSK                              (0x1 << PPE_INT_LOAD_OVER_POS)
#define PPE_INT_LOAD_OVER_CLR                              (~PPE_INT_LOAD_OVER_MSK)
#define PPE_INT_FR_OVER_POS                              (1)
#define PPE_INT_FR_OVER_MSK                              (0x1 << PPE_INT_FR_OVER_POS)
#define PPE_INT_FR_OVER_CLR                              (~PPE_INT_FR_OVER_MSK)
#define PPE_INT_ALL_OVER_POS                              (0)
#define PPE_INT_ALL_OVER_MSK                              (0x1 << PPE_INT_ALL_OVER_POS)
#define PPE_INT_ALL_OVER_CLR                              (~PPE_INT_ALL_OVER_MSK)
/** @} */

/** @defgroup INTR_MASK
 * @brief Interrupt Mask
 * @{
 **/
#define PPE_INT_CHN_BUS_ERR_POS                              (15)
#define PPE_INT_CHN_BUS_ERR_MSK                              (0x1FFFF << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_INT_CHN_BUS_ERR_CLR                              (~PPE_INT_CHN_BUS_ERR_MSK)
/* INTR_MASK[6]: SET_ERR, settings error */
#define PPE_INT_SET_ERR_POS                              (6)
#define PPE_INT_SET_ERR_MSK                              (0x1 << PPE_INT_SET_ERR_POS)
#define PPE_INT_SET_ERR_CLR                              (~PPE_INT_SET_ERR_MSK)
/* INTR_MASK[5]: SECURE_ERR, secure error */
#define PPE_INT_SECURE_ERR_POS                              (5)
#define PPE_INT_SECURE_ERR_MSK                              (0x1 << PPE_INT_SECURE_ERR_POS)
#define PPE_INT_SECURE_ERR_CLR                              (~PPE_INT_SECURE_ERR_MSK)
/* INTR_MASK[4]: SUSP_INAC, suspend inactive */
#define PPE_INT_SUSP_INAC_POS                              (4)
#define PPE_INT_SUSP_INAC_MSK                              (0x1 << PPE_INT_SUSP_INAC_POS)
#define PPE_INT_SUSP_INAC_CLR                              (~PPE_INT_SUSP_INAC_MSK)
/* INTR_MASK[3]: LINE_WL,  */
#define PPE_INT_LINE_WL_POS                              (3)
#define PPE_INT_LINE_WL_MSK                              (0x1 << PPE_INT_LINE_WL_POS)
#define PPE_INT_LINE_WL_CLR                              (~PPE_INT_LINE_WL_MSK)
/* INTR_MASK[2]: LOAD_OVER,  */
#define PPE_INT_LOAD_OVER_POS                              (2)
#define PPE_INT_LOAD_OVER_MSK                              (0x1 << PPE_INT_LOAD_OVER_POS)
#define PPE_INT_LOAD_OVER_CLR                              (~PPE_INT_LOAD_OVER_MSK)
/* INTR_MASKINTR_MASK[1]: FR_OVER,  */
#define PPE_INT_FR_OVER_POS                              (1)
#define PPE_INT_FR_OVER_MSK                              (0x1 << PPE_INT_FR_OVER_POS)
#define PPE_INT_FR_OVER_CLR                              (~PPE_INT_FR_OVER_MSK)
/* INTR_MASK[0]: ALL_OVER,  */
#define PPE_INT_ALL_OVER_POS                              (0)
#define PPE_INT_ALL_OVER_MSK                              (0x1 << PPE_INT_ALL_OVER_POS)
#define PPE_INT_ALL_OVER_CLR                              (~PPE_INT_ALL_OVER_MSK)
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

/**
  * @}
  */


/* Exported constants --------------------------------------------------------*/
/** @defgroup PPE_Exported_Definitions PPE Exported Constants
  * @{
  */

#define PPE_ALL_OVER_INT                                ((u32)0x1 << PPE_INT_ALL_OVER_POS)
#define PPE_FR_OVER_INT                                 ((u32)0x1 << PPE_INT_FR_OVER_POS)
#define PPE_LOAD_OVER_INT                               ((u32)0x1 << PPE_INT_LOAD_OVER_POS)
#define PPE_LINE_WL_INT                                 ((u32)0x1 << PPE_INT_LINE_WL_POS)
#define PPE_SUSP_INAC_INT                               ((u32)0x1 << PPE_INT_SUSP_INAC_POS)
#define PPE_SECURE_ERR_INT                              ((u32)0x1 << PPE_INT_SECURE_ERR_POS)
#define PPE_SET_ERR_INT                                 ((u32)0x1 << PPE_INT_SET_ERR_POS)
#define PPE_BUS1_ERR_INT                                ((u32)0x1 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS2_ERR_INT                                ((u32)0x2 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS3_ERR_INT                                ((u32)0x4 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS4_ERR_INT                                ((u32)0x8 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS5_ERR_INT                                ((u32)0x10 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS6_ERR_INT                                ((u32)0x20 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS7_ERR_INT                                ((u32)0x40 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS8_ERR_INT                                ((u32)0x80 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS9_ERR_INT                                ((u32)0x100 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS10_ERR_INT                               ((u32)0x200 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS11_ERR_INT                               ((u32)0x400 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS12_ERR_INT                               ((u32)0x800 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS13_ERR_INT                               ((u32)0x1000 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS14_ERR_INT                               ((u32)0x2000 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS15_ERR_INT                               ((u32)0x4000 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS16_ERR_INT                               ((u32)0x8000 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS17_ERR_INT                               ((u32)0x10000 << PPE_INT_CHN_BUS_ERR_POS)


#define IS_PPE_CHN_INDEX(chn)                               (((chn) < 17) && ((chn) >= 0))

#define PPE_FUNCTION_ALPHA_BLEND                        ((u32)0x0)
#define PPE_FUNCTION_SCALE                              ((u32)0x1)
#define IS_PPE_FUNCTION(func)                           (((func) == PPE_FUNCTION_ALPHA_BLEND) || ((func) == PPE_FUNCTION_SCALE))

#define IS_PPE_ALPHA_BLEND_LAYER_NUM(layer_num)         ((layer_num) < 16)

#define PPE_RESULT_LAYER_INDEX                          0
#define PPE_INPUT_LAYER1_INDEX                          1
#define PPE_INPUT_LAYER2_INDEX                          2
#define PPE_INPUT_LAYER3_INDEX                          3

#define PPE_RESULT_LAYER_BIT                          ((u32)0x1 << PPE_RESULT_LAYER_INDEX)
#define PPE_INPUT_LAYER1_BIT                          ((u32)0x1 << PPE_INPUT_LAYER1_INDEX)
#define PPE_INPUT_LAYER2_BIT                          ((u32)0x1 << PPE_INPUT_LAYER2_INDEX)
#define PPE_INPUT_LAYER3_BIT                          ((u32)0x1 << PPE_INPUT_LAYER3_INDEX)

/** @} */

/** @} */

/** @} */

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


#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

#endif
