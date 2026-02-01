/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_PPE_H_
#define _AMEBA_PPE_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup PPE
  * @brief PPE driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup PPE_Register_Definitions PPE Register Definitions
  * @{
  */

/** @defgroup PPE_GLB_STATUS
  * @brief Global status Register
  * @{
  */
#define PPE_MASK_CHN_INACT    ((u32)0x0001FFFF << 15)           /*!< R 0x0  Indicates whether the internal DMA channels are inactive (protocol over). Bit 15: 0x0: There are currently outstanding transactions for AWID=0 0x1: There are currently no outstanding transactions for AWID=0 Bit i (16 <= i <= 31): 0x0: There are currently outstanding transactions for ARID=i-16 0x1: There are currently no outstanding transactions for ARID=i-16*/
#define PPE_CHN_INACT(x)      (((u32)((x) & 0x0001FFFF) << 15))
#define PPE_GET_CHN_INACT(x)  ((u32)(((x >> 15) & 0x0001FFFF)))
#define PPE_MASK_RUN_STATUS   ((u32)0x00000007 << 0)            /*!< R/W 0x1FFFF  Run status 0x0: Disabled 0x1: Runing 0x2: Inactive (w/o outstanding) 0x3: Suspend (w/ outstanding) Others: reserved*/
#define PPE_RUN_STATUS(x)     (((u32)((x) & 0x00000007) << 0))
#define PPE_GET_RUN_STATUS(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup PPE_LAY_EN
  * @brief Layer enable register
  * @{
  */
#define PPE_MASK_INPUT_LAY_EN   ((u32)0x00000007 << 1)           /*!< R/W 0x0  Input layers enable field. Bit i (1 <= i <=3 ) 0x0: Input layer i is disabled. 0x1: Input layer i is enabled.*/
#define PPE_INPUT_LAY_EN(x)     (((u32)((x) & 0x00000007) << 1))
#define PPE_GET_INPUT_LAY_EN(x) ((u32)(((x >> 1) & 0x00000007)))
/** @} */

/** @defgroup PPE_RELOAD_CFG
  * @brief Reload configuration Register
  * @{
  */
#define PPE_BIT_AUTO_CLR         ((u32)0x00000001 << 17)          /*!< R/W 0x1  Auto clear "SET_VALID" after loading layer settings*/
#define PPE_BIT_SET_VALID        ((u32)0x00000001 << 16)          /*!< R/W1S 0x0  Setting is valid Will be auto set to 1 when writing 1 to GLB_EN*/
#define PPE_MASK_LYR_RELOAD_EN   ((u32)0x0000000F << 0)           /*!< R/W 0x0  Layers setting reload enable Bit0~15: Layer0~15*/
#define PPE_LYR_RELOAD_EN(x)     (((u32)((x) & 0x0000000F) << 0))
#define PPE_GET_LYR_RELOAD_EN(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup PPE_LL_CFG
  * @brief Linked List configuration Register
  * @{
  */
#define PPE_MASK_LL_PRIOR           ((u32)0x0000000F << 28)           /*!< R 0x0  Priority (ARQos) (Reserved)*/
#define PPE_LL_PRIOR(x)             (((u32)((x) & 0x0000000F) << 28))
#define PPE_GET_LL_PRIOR(x)         ((u32)(((x >> 28) & 0x0000000F)))
#define PPE_MASK_LL_MAX_ARLEN_LOG   ((u32)0x00000007 << 24)           /*!< R 0x7  Limit of ARLen (Reserved)*/
#define PPE_LL_MAX_ARLEN_LOG(x)     (((u32)((x) & 0x00000007) << 24))
#define PPE_GET_LL_MAX_ARLEN_LOG(x) ((u32)(((x >> 24) & 0x00000007)))
#define PPE_MASK_LL_ARCACHE         ((u32)0x0000000F << 20)           /*!< R 0x2  ARCache (Reserved). Refer to AXI4 protocol.*/
#define PPE_LL_ARCACHE(x)           (((u32)((x) & 0x0000000F) << 20))
#define PPE_GET_LL_ARCACHE(x)       ((u32)(((x >> 20) & 0x0000000F)))
#define PPE_BIT_LL_INCR             ((u32)0x00000001 << 19)           /*!< R 0x1  INCR or FIXED burst for reading linked list item (Reserved)*/
#define PPE_MASK_LL_ARSIZE          ((u32)0x00000007 << 16)           /*!< R 0x2  ARSize for reading linked list item (Reserved)*/
#define PPE_LL_ARSIZE(x)            (((u32)((x) & 0x00000007) << 16))
#define PPE_GET_LL_ARSIZE(x)        ((u32)(((x >> 16) & 0x00000007)))
#define PPE_MASK_LAYER_LL_EN        ((u32)0x0000000F << 0)            /*!< R/W 0x0  Update layers registers by linked list enable Bit0~15: Layer0~15*/
#define PPE_LAYER_LL_EN(x)          (((u32)((x) & 0x0000000F) << 0))
#define PPE_GET_LAYER_LL_EN(x)      ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup PPE_LLP
  * @brief Linked List Pointer Register
  * @{
  */
#define PPE_MASK_LLP   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Linked List Pointer Note: Must align to 4 Bytes. Bits[1:0] is read only and return 0 on a read.*/
#define PPE_LLP(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define PPE_GET_LLP(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup PPE_MISC
  * @brief Secure enable register
  * @{
  */
#define PPE_BIT_SECURE ((u32)0x00000001 << 0) /*!< R 0x0  Secure enable This field can only be modified by secure APB transaction. 0x0: PPE in non-secure mode 0x1: PPE in secure mode*/
/** @} */

/** @defgroup PPE_LINE_INDEX
  * @brief Line index register
  * @{
  */
#define PPE_MASK_LINE_INDEX   ((u32)0x000007FF << 0)           /*!< R/W 0x7FF  Line index for line over interrupt LINE_OVER interrupt is raised when the transaction of last pixel of this index marked line has been completed.*/
#define PPE_LINE_INDEX(x)     (((u32)((x) & 0x000007FF) << 0))
#define PPE_GET_LINE_INDEX(x) ((u32)(((x >> 0) & 0x000007FF)))
/** @} */

/** @defgroup PPE_RELEASE_DATE
  * @brief Release Date Register
  * @{
  */
#define PPE_MASK_RELEASE_DATE   ((u32)0xFFFFFFFF << 0)           /*!< R Release Date  Release Date*/
#define PPE_RELEASE_DATE(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define PPE_GET_RELEASE_DATE(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup PPE_RTL_VERSION
  * @brief RTL version Register
  * @{
  */
#define PPE_MASK_RTL_VERSION   ((u32)0xFFFFFFFF << 0)           /*!< R RTL version  RTL version*/
#define PPE_RTL_VERSION(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define PPE_GET_RTL_VERSION(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup PPE_INTR_ST
  * @brief Interrupt status (raw & mask) register
  * @{
  */
#define PPE_BIT_INTR_ST_DIV0_ERR   ((u32)0x00000001 << 8) /*!< R 0x0  */
#define PPE_BIT_INTR_ST_BUS_ERR    ((u32)0x00000001 << 7) /*!< R 0x0  */
#define PPE_BIT_INTR_ST_SET_ERR    ((u32)0x00000001 << 6) /*!< R 0x0  */
#define PPE_BIT_INTR_ST_SECURE_ERR ((u32)0x00000001 << 5) /*!< R 0x0  */
#define PPE_BIT_INTR_ST_SUSP_INAC  ((u32)0x00000001 << 4) /*!< R 0x0  */
#define PPE_BIT_INTR_ST_LINE_OVER  ((u32)0x00000001 << 3) /*!< R 0x0  */
#define PPE_BIT_INTR_ST_LOAD_OVER  ((u32)0x00000001 << 2) /*!< R 0x0  */
#define PPE_BIT_INTR_ST_FR_OVER    ((u32)0x00000001 << 1) /*!< R 0x0  */
#define PPE_BIT_INTR_ST_ALL_OVER   ((u32)0x00000001 << 0) /*!< R 0x0  */
/** @} */

/** @defgroup PPE_INTR_RAW
  * @brief Interrupt raw status register
  * @{
  */
#define PPE_BIT_INTR_RAW_DIV0_ERR   ((u32)0x00000001 << 8) /*!< R 0x0  */
#define PPE_BIT_INTR_RAW_BUS_ERR    ((u32)0x00000001 << 7) /*!< R 0x0  */
#define PPE_BIT_INTR_RAW_SET_ERR    ((u32)0x00000001 << 6) /*!< R 0x0  Setting error (Reserved)*/
#define PPE_BIT_INTR_RAW_SECURE_ERR ((u32)0x00000001 << 5) /*!< R 0x0  Secure error Raised after non-secure APB request access secure PPE.*/
#define PPE_BIT_INTR_RAW_SUSP_INAC  ((u32)0x00000001 << 4) /*!< R 0x0  Master port inactive after being suspended Raised after the master port is inactive and PPE is suspended.*/
#define PPE_BIT_INTR_RAW_LINE_OVER  ((u32)0x00000001 << 3) /*!< R 0x0  Line index over Raised after the transaction of last pixel of LINE_INDEX marked line has been completed.*/
#define PPE_BIT_INTR_RAW_LOAD_OVER  ((u32)0x00000001 << 2) /*!< R 0x0  Register load over Raised after layers registers load to internal DMA.*/
#define PPE_BIT_INTR_RAW_FR_OVER    ((u32)0x00000001 << 1) /*!< R 0x0  Frame over Raised after every frame completion.*/
#define PPE_BIT_INTR_RAW_ALL_OVER   ((u32)0x00000001 << 0) /*!< R 0x0  All frames over Raised after all frames completion.*/
/** @} */

/** @defgroup PPE_INTR_CLR
  * @brief Interrupt clear register
  * @{
  */
#define PPE_BIT_INTR_CLR_DIV0_ERR   ((u32)0x00000001 << 8) /*!< W1C 0x0  Wirte 1 to clear the corresponding interrupt*/
#define PPE_BIT_INTR_CLR_BUS_ERR    ((u32)0x00000001 << 7) /*!< W1C 0x0  Wirte 1 to clear the corresponding interrupt*/
#define PPE_BIT_INTR_CLR_SET_ERR    ((u32)0x00000001 << 6) /*!< W1C 0x0  Wirte 1 to clear the corresponding interrupt*/
#define PPE_BIT_INTR_CLR_SECURE_ERR ((u32)0x00000001 << 5) /*!< W1C 0x0  Wirte 1 to clear the corresponding interrupt*/
#define PPE_BIT_INTR_CLR_SUSP_INAC  ((u32)0x00000001 << 4) /*!< W1C 0x0  Wirte 1 to clear the corresponding interrupt*/
#define PPE_BIT_INTR_CLR_LINE_OVER  ((u32)0x00000001 << 3) /*!< W1C 0x0  Wirte 1 to clear the corresponding interrupt*/
#define PPE_BIT_INTR_CLR_LOAD_OVER  ((u32)0x00000001 << 2) /*!< W1C 0x0  Wirte 1 to clear the corresponding interrupt*/
#define PPE_BIT_INTR_CLR_FR_OVER    ((u32)0x00000001 << 1) /*!< W1C 0x0  Wirte 1 to clear the corresponding interrupt*/
#define PPE_BIT_INTR_CLR_ALL_OVER   ((u32)0x00000001 << 0) /*!< W1C 0x0  Wirte 1 to clear the corresponding interrupt*/
/** @} */

/** @defgroup PPE_INTR_MASK
  * @brief Interrupt mask register
  * @{
  */
#define PPE_BIT_INTR_MASK_DIV0_ERR   ((u32)0x00000001 << 8) /*!< R/W 0x0  */
#define PPE_BIT_INTR_MASK_BUS_ERR    ((u32)0x00000001 << 7) /*!< R/W 0x0  */
#define PPE_BIT_INTR_MASK_SET_ERR    ((u32)0x00000001 << 6) /*!< R/W 0x0  */
#define PPE_BIT_INTR_MASK_SECURE_ERR ((u32)0x00000001 << 5) /*!< R/W 0x0  */
#define PPE_BIT_INTR_MASK_SUSP_INAC  ((u32)0x00000001 << 4) /*!< R/W 0x0  */
#define PPE_BIT_INTR_MASK_LINE_OVER  ((u32)0x00000001 << 3) /*!< R/W 0x0  */
#define PPE_BIT_INTR_MASK_LOAD_OVER  ((u32)0x00000001 << 2) /*!< R/W 0x0  */
#define PPE_BIT_INTR_MASK_FR_OVER    ((u32)0x00000001 << 1) /*!< R/W 0x0  */
#define PPE_BIT_INTR_MASK_ALL_OVER   ((u32)0x00000001 << 0) /*!< R/W 0x0  */
/** @} */

/** @defgroup PPE_BUS_ERR_DTL
  * @brief BUS error detail register
  * @{
  */
#define PPE_BIT_WR_ERR    ((u32)0x00000001 << 16)          /*!< R 0x0  The mark for write error response Assert this bit when receiving error response from axi B cahnnel*/
#define PPE_MASK_RD_ERR   ((u32)0x0000000F << 0)           /*!< R 0x0  The mark for read error response Assert bit N when receiving error response from axi_rid=N*/
#define PPE_RD_ERR(x)     (((u32)((x) & 0x0000000F) << 0))
#define PPE_GET_RD_ERR(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup PPE_XOR_CTRL
  * @brief XOR control register
  * @{
  */
#define PPE_BIT_XOR ((u32)0x00000001 << 0) /*!< R/W 0x1  0x1: Enable XOR function 0x0: Disable XOR function*/
/** @} */

/** @defgroup PPE_RESULT_LAYER_ADDR_L
  * @brief Address of the first pixel register
  * @{
  */
#define PPE_MASK_RESULT_LAYER_ADDR_L   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Address of the first pixel (Physical address)*/
#define PPE_RESULT_LAYER_ADDR_L(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define PPE_GET_RESULT_LAYER_ADDR_L(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup PPE_RESULT_LAYER_PIC_SIZE
  * @brief Canvas size register
  * @{
  */
#define PPE_MASK_RESULT_LAYER_PIC_HEIGHT   ((u32)0x000007FF << 16)           /*!< R/W 0x1  Canvas height (in pixels) Must be greater than 0.*/
#define PPE_RESULT_LAYER_PIC_HEIGHT(x)     (((u32)((x) & 0x000007FF) << 16))
#define PPE_GET_RESULT_LAYER_PIC_HEIGHT(x) ((u32)(((x >> 16) & 0x000007FF)))
#define PPE_MASK_RESULT_LAYER_PIC_WIDTH    ((u32)0x000007FF << 0)            /*!< R/W 0x1  Canvas width (in pixels) Must be greater than 0.*/
#define PPE_RESULT_LAYER_PIC_WIDTH(x)      (((u32)((x) & 0x000007FF) << 0))
#define PPE_GET_RESULT_LAYER_PIC_WIDTH(x)  ((u32)(((x >> 0) & 0x000007FF)))
/** @} */

/** @defgroup PPE_RESULT_LAYER_PIC_CFG
  * @brief Picture configuration register
  * @{
  */
#define PPE_MASK_RESULT_LAYER_LINE_LEN   ((u32)0x0000FFFF << 16)           /*!< R/W 0x0  Indicates the bytes in memory between two vertically adjacent pixels.*/
#define PPE_RESULT_LAYER_LINE_LEN(x)     (((u32)((x) & 0x0000FFFF) << 16))
#define PPE_GET_RESULT_LAYER_LINE_LEN(x) ((u32)(((x >> 16) & 0x0000FFFF)))
#define PPE_BIT_RESULT_LAYER_BG_SRC      ((u32)0x00000001 << 8)            /*!< R/W 0x0  Background source 0x0: Use CONST_BG as background 0x1: Use lyr1 as background*/
#define PPE_MASK_RESULT_LAYER_FORMAT     ((u32)0x000000FF << 0)            /*!< R/W 0x0  Pixel format*/
#define PPE_RESULT_LAYER_FORMAT(x)       (((u32)((x) & 0x000000FF) << 0))
#define PPE_GET_RESULT_LAYER_FORMAT(x)   ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup PPE_RESULT_LAYER_CONST_BG
  * @brief Constant background color register
  * @{
  */
#define PPE_MASK_RESULT_LAYER_ABGR   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Background ABGR (in ABGR8888 format)*/
#define PPE_RESULT_LAYER_ABGR(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define PPE_GET_RESULT_LAYER_ABGR(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup PPE_RESULT_LAYER_BUS_CFG
  * @brief Master port BUS configuration register
  * @{
  */
#define PPE_MASK_RESULT_LAYER_PRIOR           ((u32)0x0000000F << 12)           /*!< R/W 0x0  Priority (AWQos)*/
#define PPE_RESULT_LAYER_PRIOR(x)             (((u32)((x) & 0x0000000F) << 12))
#define PPE_GET_RESULT_LAYER_PRIOR(x)         ((u32)(((x >> 12) & 0x0000000F)))
#define PPE_MASK_RESULT_LAYER_MAX_AXLEN_LOG   ((u32)0x00000007 << 8)            /*!< R/W 0x7  Limit of AWLen*/
#define PPE_RESULT_LAYER_MAX_AXLEN_LOG(x)     (((u32)((x) & 0x00000007) << 8))
#define PPE_GET_RESULT_LAYER_MAX_AXLEN_LOG(x) ((u32)(((x >> 8) & 0x00000007)))
#define PPE_MASK_RESULT_LAYER_AXCACHE         ((u32)0x0000000F << 4)            /*!< R/W 0x0  AWCache Refer to AXI4 protocol.*/
#define PPE_RESULT_LAYER_AXCACHE(x)           (((u32)((x) & 0x0000000F) << 4))
#define PPE_GET_RESULT_LAYER_AXCACHE(x)       ((u32)(((x >> 4) & 0x0000000F)))
#define PPE_BIT_RESULT_LAYER_INCR             ((u32)0x00000001 << 3)            /*!< R 0x1  AWBurst 0x1: AWBurst = INCR 0x0: AWBurst = FIXED*/
#define PPE_MASK_RESULT_LAYER_AXSIZE          ((u32)0x00000007 << 0)            /*!< R/W 0x2  AWSize Refer to AXI4 protocol. (AXI cmd size must be less than or equal to BUS width)*/
#define PPE_RESULT_LAYER_AXSIZE(x)            (((u32)((x) & 0x00000007) << 0))
#define PPE_GET_RESULT_LAYER_AXSIZE(x)        ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup PPE_RESULT_LAYER_BLK_SIZE
  * @brief Canvas size register
  * @{
  */
#define PPE_MASK_RESULT_LAYER_BLK_HEIGHT   ((u32)0x000007FF << 16)           /*!< R/W 0x1  Block height (in pixels) Must be greater than 0 and less than or equal to canvas width.*/
#define PPE_RESULT_LAYER_BLK_HEIGHT(x)     (((u32)((x) & 0x000007FF) << 16))
#define PPE_GET_RESULT_LAYER_BLK_HEIGHT(x) ((u32)(((x >> 16) & 0x000007FF)))
#define PPE_MASK_RESULT_LAYER_BLK_WIDTH    ((u32)0x000007FF << 0)            /*!< R/W 0x1  Block width (in pixels) Must be greater than 0 and less than or equal to canvas width.*/
#define PPE_RESULT_LAYER_BLK_WIDTH(x)      (((u32)((x) & 0x000007FF) << 0))
#define PPE_GET_RESULT_LAYER_BLK_WIDTH(x)  ((u32)(((x >> 0) & 0x000007FF)))
/** @} */

/** @defgroup PPE_INPUT_LAYERx_ADDR_L
  * @brief Address of the first pixel register
  * @{
  */
#define PPE_MASK_INPUT_LAYERx_ADDR_L   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  */
#define PPE_INPUT_LAYERx_ADDR_L(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define PPE_GET_INPUT_LAYERx_ADDR_L(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup PPE_INPUT_LAYERx_PIC_SIZE
  * @brief Canvas size register
  * @{
  */
#define PPE_MASK_INPUT_LAYERx_PIC_HEIGHT   ((u32)0x000007FF << 16)           /*!< R/W 0x1  Picture height (in pixels) Must be greater than 0.*/
#define PPE_INPUT_LAYERx_PIC_HEIGHT(x)     (((u32)((x) & 0x000007FF) << 16))
#define PPE_GET_INPUT_LAYERx_PIC_HEIGHT(x) ((u32)(((x >> 16) & 0x000007FF)))
#define PPE_MASK_INPUT_LAYERx_PIC_WIDTH    ((u32)0x000007FF << 0)            /*!< R/W 0x1  Picture width (in pixels) Must be greater than 0.*/
#define PPE_INPUT_LAYERx_PIC_WIDTH(x)      (((u32)((x) & 0x000007FF) << 0))
#define PPE_GET_INPUT_LAYERx_PIC_WIDTH(x)  ((u32)(((x >> 0) & 0x000007FF)))
/** @} */

/** @defgroup PPE_INPUT_LAYERx_PIC_CFG
  * @brief Picture configuration register
  * @{
  */
#define PPE_MASK_INPUT_LAYERx_LINE_LEN   ((u32)0x000007FF << 16)           /*!< R/W 0x0  Indicates the bytes in memory between two vertically adjacent pixels.*/
#define PPE_INPUT_LAYERx_LINE_LEN(x)     (((u32)((x) & 0x0000FFFF) << 16))
#define PPE_GET_INPUT_LAYERx_LINE_LEN(x) ((u32)(((x >> 16) & 0x000007FF)))
#define PPE_MASK_INPUT_LAYERx_KEY_MODE   ((u32)0x00000003 << 12)           /*!< R/W 0x0  Color keying match mode 0x0, 0x1: Disable 0x2: Inside ((Rmin<=R<=Rmax) & (Gmin<=G<=Gmax) & (Bmin<=B<=Bmax)) 0x3: Outside ((R<Rmin) | (R>Rmax) | (G<Gmin) | (G>Gmax) | (B<Bmin) | (B>Bmax))*/
#define PPE_INPUT_LAYERx_KEY_MODE(x)     (((u32)((x) & 0x00000003) << 12))
#define PPE_GET_INPUT_LAYERx_KEY_MODE(x) ((u32)(((x >> 12) & 0x00000003)))
#define PPE_BIT_INPUT_LAYERx_INTERP      ((u32)0x00000001 << 10)           /*!< R/W 0x0  Interpolation 0x0: Nearest-neighbor 0x1: Bilinear*/
#define PPE_BIT_INPUT_LAYERx_PIX_SRC     ((u32)0x00000001 << 8)            /*!< R/W 0x0  Pixel source: 0x0: CONST_PIX 0x1: DMA (read from memory)*/
#define PPE_MASK_INPUT_LAYERx_FORMAT     ((u32)0x000000FF << 0)            /*!< R/W 0x0  Format of pixels from memory*/
#define PPE_INPUT_LAYERx_FORMAT(x)       (((u32)((x) & 0x000000FF) << 0))
#define PPE_GET_INPUT_LAYERx_FORMAT(x)   ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup PPE_INPUT_LAYERx_CONST_PIX
  * @brief Constant pixel register
  * @{
  */
#define PPE_MASK_INPUT_LAYERx_ABGR   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0xFFFFFFFF  In ABGR8888 format*/
#define PPE_INPUT_LAYERx_ABGR(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define PPE_GET_INPUT_LAYERx_ABGR(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup PPE_INPUT_LAYERx_BUS_CFG
  * @brief Master port BUS configuration register
  * @{
  */
#define PPE_MASK_INPUT_LAYERx_MAX_AXLEN_LOG   ((u32)0x00000007 << 8)           /*!< R/W 0x7  Limit of ARLen*/
#define PPE_INPUT_LAYERx_MAX_AXLEN_LOG(x)     (((u32)((x) & 0x00000007) << 8))
#define PPE_GET_INPUT_LAYERx_MAX_AXLEN_LOG(x) ((u32)(((x >> 8) & 0x00000007)))
#define PPE_MASK_INPUT_LAYERx_AXCACHE         ((u32)0x0000000F << 4)           /*!< R/W 0x0  ARCache Refer to AXI4 protocol. Note: Set to 0x6 to enable read buffer for input layer*/
#define PPE_INPUT_LAYERx_AXCACHE(x)           (((u32)((x) & 0x0000000F) << 4))
#define PPE_GET_INPUT_LAYERx_AXCACHE(x)       ((u32)(((x >> 4) & 0x0000000F)))
#define PPE_BIT_INPUT_LAYERx_INCR             ((u32)0x00000001 << 3)           /*!< R 0x1  ARBurst 0x1: ARBurst = INCR 0x0: ARBurst = FIXED*/
#define PPE_MASK_INPUT_LAYERx_AXSIZE          ((u32)0x00000007 << 0)           /*!< R/W 0x2  ARSize Refer to AXI4 protocol.  · AXI cmd size must be less than or equal to BUS width. · Must be 0x2 when AXCACHE=6.*/
#define PPE_INPUT_LAYERx_AXSIZE(x)            (((u32)((x) & 0x00000007) << 0))
#define PPE_GET_INPUT_LAYERx_AXSIZE(x)        ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup PPE_INPUT_LAYERx_WIN_MIN
  * @brief Minimum coordinate of the input window register
  * @{
  */
#define PPE_MASK_INPUT_LAYERx_Y_MIN   ((u32)0x000007FF << 16)           /*!< R/W 0x0  Y coordinate*/
#define PPE_INPUT_LAYERx_Y_MIN(x)     (((u32)((x) & 0x000007FF) << 16))
#define PPE_GET_INPUT_LAYERx_Y_MIN(x) ((u32)(((x >> 16) & 0x000007FF)))
#define PPE_MASK_INPUT_LAYERx_X_MIN   ((u32)0x000007FF << 0)            /*!< R/W 0x0  X coordinate*/
#define PPE_INPUT_LAYERx_X_MIN(x)     (((u32)((x) & 0x000007FF) << 0))
#define PPE_GET_INPUT_LAYERx_X_MIN(x) ((u32)(((x >> 0) & 0x000007FF)))
/** @} */

/** @defgroup PPE_INPUT_LAYERx_WIN_MAX
  * @brief Maximum coordinate of the input window register
  * @{
  */
#define PPE_MASK_INPUT_LAYERx_Y_MAX   ((u32)0x000007FF << 16)           /*!< R/W 0x0  Y coordinate*/
#define PPE_INPUT_LAYERx_Y_MAX(x)     (((u32)((x) & 0x000007FF) << 16))
#define PPE_GET_INPUT_LAYERx_Y_MAX(x) ((u32)(((x >> 16) & 0x000007FF)))
#define PPE_MASK_INPUT_LAYERx_X_MAX   ((u32)0x000007FF << 0)            /*!< R/W 0x0  X coordinate*/
#define PPE_INPUT_LAYERx_X_MAX(x)     (((u32)((x) & 0x000007FF) << 0))
#define PPE_GET_INPUT_LAYERx_X_MAX(x) ((u32)(((x >> 0) & 0x000007FF)))
/** @} */

/** @defgroup PPE_INPUT_LAYERx_KEY_MIN
  * @brief Minimum of the color key match condition register
  * @{
  */
#define PPE_MASK_INPUT_LAYERx_BGR_MIN   ((u32)0x00FFFFFF << 0)           /*!< R/W 0x0  Minimum BGR of the color key match condition In BGR888 format.*/
#define PPE_INPUT_LAYERx_BGR_MIN(x)     (((u32)((x) & 0x00FFFFFF) << 0))
#define PPE_GET_INPUT_LAYERx_BGR_MIN(x) ((u32)(((x >> 0) & 0x00FFFFFF)))
/** @} */

/** @defgroup PPE_INPUT_LAYERx_KEY_MAX
  * @brief Maximum of the color key match condition register
  * @{
  */
#define PPE_MASK_INPUT_LAYERx_BGR_MAX   ((u32)0x00FFFFFF << 0)           /*!< R/W 0x0  Maximum BGR of the color key match condition In BGR888 format.*/
#define PPE_INPUT_LAYERx_BGR_MAX(x)     (((u32)((x) & 0x00FFFFFF) << 0))
#define PPE_GET_INPUT_LAYERx_BGR_MAX(x) ((u32)(((x >> 0) & 0x00FFFFFF)))
/** @} */

/** @defgroup PPE_INPUT_LAYERx_MATRIX_E11
  * @brief Matrix E11 register
  * @{
  */
#define PPE_MASK_INPUT_LAYERx_MATRIX_E11   ((u32)0x0000FFFF << 0)           /*!< R/W 0x10  Element E11 ~ E33 of coordinates transformation matrix*/
#define PPE_INPUT_LAYERx_MATRIX_E11(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define PPE_GET_INPUT_LAYERx_MATRIX_E11(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup PPE_INPUT_LAYERx_MATRIX_E12
  * @brief Matrix E12 register
  * @{
  */
#define PPE_MASK_INPUT_LAYERx_MATRIX_E12   ((u32)0x0000FFFF << 0)           /*!< R/W 0x0  Element E11 ~ E33 of coordinates transformation matrix · Elements E12, E21, E31~E33 are RO if SUPPORT_ROTATION_i = 0.*/
#define PPE_INPUT_LAYERx_MATRIX_E12(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define PPE_GET_INPUT_LAYERx_MATRIX_E12(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup PPE_INPUT_LAYERx_MATRIX_E13
  * @brief Matrix E13 register
  * @{
  */
#define PPE_MASK_INPUT_LAYERx_MATRIX_E13   ((u32)0x0000FFFF << 0)           /*!< R/W 0x0  Element E11 ~ E33 of coordinates transformation matrix*/
#define PPE_INPUT_LAYERx_MATRIX_E13(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define PPE_GET_INPUT_LAYERx_MATRIX_E13(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup PPE_INPUT_LAYERx_MATRIX_E21
  * @brief Matrix E21 register
  * @{
  */
#define PPE_MASK_INPUT_LAYERx_MATRIX_E21   ((u32)0x0000FFFF << 0)           /*!< R/W 0x0  Element E11 ~ E33 of coordinates transformation matrix · Elements E12, E21, E31~E33 are RO if SUPPORT_ROTATION_i = 0.*/
#define PPE_INPUT_LAYERx_MATRIX_E21(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define PPE_GET_INPUT_LAYERx_MATRIX_E21(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup PPE_INPUT_LAYERx_MATRIX_E22
  * @brief Matrix E22 register
  * @{
  */
#define PPE_MASK_INPUT_LAYERx_MATRIX_E22   ((u32)0x0000FFFF << 0)           /*!< R/W 0x10  Element E11 ~ E33 of coordinates transformation matrix*/
#define PPE_INPUT_LAYERx_MATRIX_E22(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define PPE_GET_INPUT_LAYERx_MATRIX_E22(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup PPE_INPUT_LAYERx_MATRIX_E23
  * @brief Matrix E23 register
  * @{
  */
#define PPE_MASK_INPUT_LAYERx_MATRIX_E23   ((u32)0x0000FFFF << 0)           /*!< R/W 0x0  Element E11 ~ E33 of coordinates transformation matrix*/
#define PPE_INPUT_LAYERx_MATRIX_E23(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define PPE_GET_INPUT_LAYERx_MATRIX_E23(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup PPE_INPUT_LAYERx_MATRIX_E31
  * @brief Matrix E31 register
  * @{
  */
#define PPE_MASK_INPUT_LAYERx_MATRIX_E31   ((u32)0xFFFFFFFF << 0)           /*!< R 0x0  Element E11 ~ E33 of coordinates transformation matrix · Elements E12, E21, E31~E33 are RO if SUPPORT_ROTATION_i = 0. · Elements E31~E33 are RO if SUPPORT_PERSPECTIVE_i = 0.*/
#define PPE_INPUT_LAYERx_MATRIX_E31(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define PPE_GET_INPUT_LAYERx_MATRIX_E31(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup PPE_INPUT_LAYERx_MATRIX_E32
  * @brief Matrix E32 register
  * @{
  */
#define PPE_MASK_INPUT_LAYERx_MATRIX_E32   ((u32)0xFFFFFFFF << 0)           /*!< R 0x0  Element E11 ~ E33 of coordinates transformation matrix · Elements E12, E21, E31~E33 are RO if SUPPORT_ROTATION_i = 0. · Elements E31~E33 are RO if SUPPORT_PERSPECTIVE_i = 0.*/
#define PPE_INPUT_LAYERx_MATRIX_E32(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define PPE_GET_INPUT_LAYERx_MATRIX_E32(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup PPE_INPUT_LAYERx_MATRIX_E33
  * @brief Matrix E33 register
  * @{
  */
#define PPE_MASK_INPUT_LAYERx_MATRIX_E33   ((u32)0xFFFFFFFF << 0)           /*!< R 0x10  Element E11 ~ E33 of coordinates transformation matrix · Elements E12, E21, E31~E33 are RO if SUPPORT_ROTATION_i = 0. · Elements E31~E33 are RO if SUPPORT_PERSPECTIVE_i = 0.*/
#define PPE_INPUT_LAYERx_MATRIX_E33(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define PPE_GET_INPUT_LAYERx_MATRIX_E33(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup PPE_Exported_Types PPE Exported Types
  * @{
  */

/** @brief PPE Port
  */
typedef struct {
	__IO uint32_t PPE_INPUT_LAYERx_ADDR_L    ;  /*!< ADDRESS OF THE FIRST PIXEL REGISTER */
	__IO uint32_t RSVD0                      ;  /*!< Reserved */
	__IO uint32_t PPE_INPUT_LAYERx_PIC_SIZE  ;  /*!< CANVAS SIZE REGISTER */
	__IO uint32_t PPE_INPUT_LAYERx_PIC_CFG   ;  /*!< PICTURE CONFIGURATION REGISTER */
	__IO uint32_t PPE_INPUT_LAYERx_CONST_PIX ;  /*!< CONSTANT PIXEL REGISTER */
	__IO uint32_t PPE_INPUT_LAYERx_BUS_CFG   ;  /*!< MASTER PORT BUS CONFIGURATION REGISTER */
	__IO uint32_t RSVD1                      ;  /*!< Reserved */
	__IO uint32_t PPE_INPUT_LAYERx_WIN_MIN   ;  /*!< MINIMUM COORDINATE OF THE INPUT WINDOW REGISTER */
	__IO uint32_t PPE_INPUT_LAYERx_WIN_MAX   ;  /*!< MAXIMUM COORDINATE OF THE INPUT WINDOW REGISTER */
	__IO uint32_t PPE_INPUT_LAYERx_KEY_MIN   ;  /*!< MINIMUM OF THE COLOR KEY MATCH CONDITION REGISTER */
	__IO uint32_t PPE_INPUT_LAYERx_KEY_MAX   ;  /*!< MAXIMUM OF THE COLOR KEY MATCH CONDITION REGISTER */
	__IO uint32_t PPE_INPUT_LAYERx_MATRIX_E11;  /*!< MATRIX E11 REGISTER */
	__IO uint32_t PPE_INPUT_LAYERx_MATRIX_E12;  /*!< MATRIX E12 REGISTER */
	__IO uint32_t PPE_INPUT_LAYERx_MATRIX_E13;  /*!< MATRIX E13 REGISTER */
	__IO uint32_t PPE_INPUT_LAYERx_MATRIX_E21;  /*!< MATRIX E21 REGISTER */
	__IO uint32_t PPE_INPUT_LAYERx_MATRIX_E22;  /*!< MATRIX E22 REGISTER */
	__IO uint32_t PPE_INPUT_LAYERx_MATRIX_E23;  /*!< MATRIX E23 REGISTER */
	__I  uint32_t PPE_INPUT_LAYERx_MATRIX_E31;  /*!< MATRIX E31 REGISTER */
	__I  uint32_t PPE_INPUT_LAYERx_MATRIX_E32;  /*!< MATRIX E32 REGISTER */
	__I  uint32_t PPE_INPUT_LAYERx_MATRIX_E33;  /*!< MATRIX E33 REGISTER */
	__IO uint8_t RSVD2[45]                  ;  /*!< Reserved */
} Input_Layer_TypeDef;

/** @brief PPE Register Declaration
  */

typedef struct {
	__IO uint32_t PPE_GLB_STATUS                    ;  /*!< GLOBAL STATUS REGISTER,  Address offset:0x000 */
	__IO uint32_t PPE_LAY_EN                        ;  /*!< LAYER ENABLE REGISTER,  Address offset:0x004 */
	__IO uint32_t PPE_RELOAD_CFG                    ;  /*!< RELOAD CONFIGURATION REGISTER,  Address offset:0x008 */
	__IO uint32_t PPE_LL_CFG                        ;  /*!< LINKED LIST CONFIGURATION REGISTER,  Address offset:0x00C */
	__IO uint32_t PPE_LLP                           ;  /*!< LINKED LIST POINTER REGISTER,  Address offset:0x010 */
	__IO uint32_t RSVD0                             ;  /*!< Reserved,  Address offset:0x014-0x017 */
	__I  uint32_t PPE_MISC                          ;  /*!< SECURE ENABLE REGISTER,  Address offset:0x018 */
	__IO uint32_t PPE_LINE_INDEX                    ;  /*!< LINE INDEX REGISTER,  Address offset:0x01C */
	__I  uint32_t PPE_RELEASE_DATE                  ;  /*!< RELEASE DATE REGISTER,  Address offset:0x020 */
	__I  uint32_t PPE_RTL_VERSION                   ;  /*!< RTL VERSION REGISTER,  Address offset:0x024 */
	__IO uint32_t RSVD1[6]                          ;  /*!< Reserved,  Address offset:0x028-0x03F */
	__I  uint32_t PPE_INTR_ST                       ;  /*!< INTERRUPT STATUS (RAW & MASK) REGISTER,  Address offset:0x040 */
	__I  uint32_t PPE_INTR_RAW                      ;  /*!< INTERRUPT RAW STATUS REGISTER,  Address offset:0x044 */
	__O  uint32_t PPE_INTR_CLR                      ;  /*!< INTERRUPT CLEAR REGISTER,  Address offset:0x048 */
	__IO uint32_t PPE_INTR_MASK                     ;  /*!< INTERRUPT MASK REGISTER,  Address offset:0x04C */
	__I  uint32_t PPE_BUS_ERR_DTL                   ;  /*!< BUS ERROR DETAIL REGISTER,  Address offset:0x050 */
	__IO uint32_t RSVD2                             ;  /*!< Reserved,  Address offset:0x054-0x057 */
	__IO uint32_t PPE_XOR_CTRL                      ;  /*!< XOR CONTROL REGISTER,  Address offset:0x058 */
	__IO uint32_t RSVD3                             ;  /*!< Reserved,  Address offset:0x05C-0x05F */
	__IO uint32_t PPE_RESULT_LAYER_ADDR_L           ;  /*!< ADDRESS OF THE FIRST PIXEL REGISTER,  Address offset:0x060 */
	__IO uint32_t RSVD4                             ;  /*!< Reserved,  Address offset:0x064-0x067 */
	__IO uint32_t PPE_RESULT_LAYER_PIC_SIZE         ;  /*!< CANVAS SIZE REGISTER,  Address offset:0x068 */
	__IO uint32_t PPE_RESULT_LAYER_PIC_CFG          ;  /*!< PICTURE CONFIGURATION REGISTER,  Address offset:0x06C */
	__IO uint32_t PPE_RESULT_LAYER_CONST_BG         ;  /*!< CONSTANT BACKGROUND COLOR REGISTER,  Address offset:0x070 */
	__IO uint32_t PPE_RESULT_LAYER_BUS_CFG          ;  /*!< MASTER PORT BUS CONFIGURATION REGISTER,  Address offset:0x074 */
	__IO uint32_t RSVD5                             ;  /*!< Reserved,  Address offset:0x078-0x07B */
	__IO uint32_t PPE_RESULT_LAYER_BLK_SIZE         ;  /*!< CANVAS SIZE REGISTER,  Address offset:0x07C */
	Input_Layer_TypeDef Input_Layer[3]              ;  /*!< PPE IP have 3 ports,  Address offset:0x080-0x1FC */
} PPE_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your definition here
/* Exported types --------------------------------------------------------*/

/** @defgroup PPE_Exported_Types PPE Exported Types
* @{
*/

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
	PPE_ABGR8666 = 0x34,
	PPE_ARGB8666,
	PPE_XBGR8666,
	PPE_XRGB8666,
	PPE_BGRA6668,
	PPE_RGBA6668,
	PPE_BGRX6668,
	PPE_RGBX6668,
	PPE_BGR565_S,//0x3C
	PPE_RGB565_S,
} PPE_PIXEL_FORMAT;

/**
  * @brief  PPE Pixel Source Definition
  */
typedef enum {
	PPE_LAYER_SRC_CONST,
	PPE_LAYER_SRC_FROM_DMA,
} PPE_PIXEL_SOURCE;

/**
  * @brief  PPE Interpolation Type Definition
  */
typedef enum {
	PPE_INTERP_TYPE_Nearest_Neighbor,
	PPE_INTERP_TYPE_Bilinear,
} PPE_INTERP_TYPE;

/**
  * @brief  PPE Color Key Mode Definition
  */
typedef enum {
	PPE_KEY_MODE_DISABLE,
	PPE_KEY_MODE_INSIDE_MODE = 0x2,
	PPE_KEY_MODE_OUTSIDE_MODE,
} PPE_KEY_MODE;

/**
  * @brief  PPE Color Key Mode Definition
  */
typedef enum {
	PPE_BACKGROUND_SOURCE_CONST_BG,
	PPE_BACKGROUND_SOURCE_LAYER1,
} PPE_BACKGROUND_SOURCE;

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
  * @brief  PPE LLI Input Layer Format Definition
  */
typedef struct {
	u32 ADDR_L;
	u32 PIC_SIZE;
	u32 PIC_CFG;
	u32 CONST_PIX;
	u32 WIN_MIN;
	u32 WIN_MAX;
	u32 KEY_MIN;
	u32 KEY_MAX;
	u32 MATRIX_E11;
	u32 MATRIX_E12;
	u32 MATRIX_E13;
	u32 MATRIX_E21;
	u32 MATRIX_E22;
	u32 MATRIX_E23;
	u32 MATRIX_E31;
	u32 MATRIX_E32;
	u32 MATRIX_E33;
} PPE_LLI_INPUT_LAYER;

/**
  * @brief  PPE LLI Result Layer Format Definition
  */
typedef struct {
	u32 ADDR_L;
	u32 PIC_SIZE;
	u32 PIC_CFG;
	u32 CONST_BG;
} PPE_LLI_RESULT_LAYER;

/**
  * @brief  PPE Global register Format Definition
  */
typedef struct {
	u32 LAY_EN;
	u32 LL_CFG;
	u32 LLP;
} PPE_LLI_GLB;

/**
  * @brief  PPE Input layer Init Structure Definition
  */
typedef struct {
	u32 src_addr;
	u32 pic_width;
	u32 pic_height;
	PPE_PIXEL_FORMAT format;
	PPE_PIXEL_SOURCE pic_src;
	PPE_INTERP_TYPE interp;
	PPE_KEY_MODE key_mode;
	u32 line_len;
	u32 const_ABGR8888_value;
	u32 win_min_x;
	u32 win_min_y;
	u32 win_max_x;
	u32 win_max_y;
	u32 key_min_bgr;
	u32 key_max_bgr;
	float scale_x;		/* can be 16/1, 16/2, 16/3, ..., 16/65535*/
	float scale_y;		/* can be 16/1, 16/2, 16/3, ..., 16/65535*/
	u32 angle;
} PPE_InputLayer_InitTypeDef;

/**
  * @brief  PPE Result layer Init Structure Definition
  */
typedef struct {
	u32 src_addr;
	u32 pic_width;
	u32 pic_height;
	PPE_PIXEL_FORMAT format;
	PPE_BACKGROUND_SOURCE bg_src;
	u32 line_len;
	u32 const_bg;
	u32 blk_width;
	u32 blk_height;
	u32 xor_en;
} PPE_ResultLayer_InitTypeDef;
/** @} */

/* Exported constants --------------------------------------------------------*/
/** @defgroup PPE_Exported_Constants PPE Exported Constants
  * @{
  */
#define IS_PPE_CHN_INDEX(chn)                               (((chn) < 17) && ((chn) >= 0))

#define IS_PPE_ALPHA_BLEND_LAYER_NUM(layer_num)         ((layer_num) < 16)

#define PPE_RESULT_LAYER_INDEX                          0
#define PPE_INPUT_LAYER1_INDEX                          1
#define PPE_INPUT_LAYER2_INDEX                          2
#define PPE_INPUT_LAYER3_INDEX                          3

#define PPE_RESULT_LAYER_BIT                          ((u32)0x1 << PPE_RESULT_LAYER_INDEX)
#define PPE_INPUT_LAYER1_BIT                          ((u32)0x1 << PPE_INPUT_LAYER1_INDEX)
#define PPE_INPUT_LAYER2_BIT                          ((u32)0x1 << PPE_INPUT_LAYER2_INDEX)
#define PPE_INPUT_LAYER3_BIT                          ((u32)0x1 << PPE_INPUT_LAYER3_INDEX)

#define PPE_STATE_DISABLE                          0
#define PPE_STATE_RUNNING                          1
#define PPE_STATE_INACTIVE                         2
#define PPE_STATE_SUSPEND                          3
/** @} */


/* Exported functions --------------------------------------------------------*/
/** @defgroup PPE_Exported_Functions PPE Exported Functions
  * @{
  */
void PPE_Cmd(int state);
u32 PPE_GetGlobalState(void);
void PPE_Suspend(void);
void PPE_Resume(void);
void PPE_LayerEn(u32 layers);
int PPE_ChannelInactiveCheck(u8 chn_id);
u32 PPE_GetAllIntStatus(void);
void PPE_ClearINTPendingBit(u32 PPE_INT);
void PPE_MaskINTConfig(u32 PPE_INT_MSK, int NewState);
void PPE_MaskAllInt(void);
void PPE_AutoReloadSetLayer(u32 layers);
void PPE_AutoReloadClrLayerAll(void);
void PPE_AutoClearCmd(int NewState);
void PPE_SetValid(void);
u32 PPE_GetValid(void);
void PPE_SetLLP(u32 lli);
void PPE_LLSetLayer(u32 layers);
void PPE_SetLine(u32 line);
void PPE_InputLayer_StructInit(PPE_InputLayer_InitTypeDef *layer_init_struct);
void PPE_ResultLayer_StructInit(PPE_ResultLayer_InitTypeDef *layer_init_struct);
void PPE_InitInputLayer(u8 id, PPE_InputLayer_InitTypeDef *layer_init_struct);
void PPE_InitResultLayer(PPE_ResultLayer_InitTypeDef *layer_init_struct);
void PPE_XOR_init(void);
void PPE_XOR_process(u32 src1, u32 src2, u32 src3, u32 dst);
void PPE_XOR_Int_handler(void (*pfunc)(void));
/** @} */
#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

/** @} */

/** @} */


#endif