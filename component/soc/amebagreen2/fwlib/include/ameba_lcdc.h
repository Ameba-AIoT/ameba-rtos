/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _RL7005_LCDC_H_
#define _RL7005_LCDC_H_

/** @addtogroup Ameba_Periph_Driver
 * @{
 */

/** @defgroup LCDC
 * @brief LCDC driver modules
 * @{
 */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup LCDC_Register_Definitions LCDC Register Definitions
 * @{
 */

/** @defgroup LCDC_CTRL
 * @brief LCDC control register
 * @{
 */
#define LCDC_MASK_IF_MODE   ((u32)0x0000000F << 16)           /*!< R/W 0x9  *0000: 8-bit MCU (RGB565/RGB888 support) *0001: 16-bit MCU (RGB565 support) *0010: 9-bit MCU (RGB666 support) *0011: 18-bit MCU (RGB666 support) *0100: 24-bit MCU (RGB888 support) *1000: 6-bit RGB parallel interface (RGB565/RGB666 support) *1001: 16-bit RGB parallel interface (RGB565 support) *1010: 18-bit RGB parallel interface (RGB666 support) *1011: 8-bit RGB parallel interface (RGB888 support) *1100: 24-bit RGB parallel interface (RGB888 support) *Others: Reserved*/
#define LCDC_IF_MODE(x)     (((u32)((x) & 0x0000000F) << 16))
#define LCDC_GET_IF_MODE(x) ((u32)(((x >> 16) & 0x0000000F)))
#define LCDC_BIT_IF_MIPI    ((u32)0x00000001 << 15)           /*!< R/W 0x0  */
#define LCDC_BIT_INST_DIS   ((u32)0x00000001 << 2)            /*!< R/WE/EC 0x0  Disable LCDC instantly. Reset LCDC internal states and disable LCDC then clear both this bit and LCDCEN.*/
#define LCDC_BIT_EN         ((u32)0x00000001 << 0)            /*!< R/W/EC 0x0  Write 1 to enable LCDC.*/
/** @} */

/** @defgroup LCDC_PLANE_SIZE
 * @brief LCDC plane size register
 * @{
 */
#define LCDC_MASK_IMAGEHEIGHT   ((u32)0x00000FFF << 16)           /*!< R/W 0x140  The height of panel (Y-channel based), the min. value is 0x1.*/
#define LCDC_IMAGEHEIGHT(x)     (((u32)((x) & 0x00000FFF) << 16))
#define LCDC_GET_IMAGEHEIGHT(x) ((u32)(((x >> 16) & 0x00000FFF)))
#define LCDC_MASK_IMAGEWIDTH    ((u32)0x00000FFF << 0)            /*!< R/W 0xf0  The width of panel (X-channel based), the min. value is 0x1. *For LCD, it means pixel number per line.*/
#define LCDC_IMAGEWIDTH(x)      (((u32)((x) & 0x00000FFF) << 0))
#define LCDC_GET_IMAGEWIDTH(x)  ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup LCDC_UNDFLW_CFG
 * @brief LCDC underflow configuration register
 * @{
 */
#define LCDC_MASK_DMA_UDF_OPT   ((u32)0x00000003 << 25)           /*!< R/W 0x0  0: output error data at current frame when dma underflow occurs 1: pause panel interface(clk and data) until underflow timeout counter reaches threshold value, then entering error mode 2: entering drop mode immediately when dma underflow occurs 3: pause panel interface(clk and data) until underflow timeout counter reaches threshold value, then  entering drop mode*/
#define LCDC_DMA_UDF_OPT(x)     (((u32)((x) & 0x00000003) << 25))
#define LCDC_GET_DMA_UDF_OPT(x) ((u32)(((x >> 25) & 0x00000003)))
#define LCDC_BIT_DMA_UN_MODE    ((u32)0x00000001 << 24)           /*!< R/W 0x0  *0:Output last data *1:Output erroutdata*/
#define LCDC_MASK_ERROUT_DATA   ((u32)0x00FFFFFF << 0)            /*!< R/W 0x0  Output data when DMA FIFO underflow occurred. (directly mapping to output D[23:0])*/
#define LCDC_ERROUT_DATA(x)     (((u32)((x) & 0x00FFFFFF) << 0))
#define LCDC_GET_ERROUT_DATA(x) ((u32)(((x >> 0) & 0x00FFFFFF)))
/** @} */

/** @defgroup LCDC_DMA_MODE_CFG
 * @brief LCDC DMA mode configuration register
 * @{
 */
#define LCDC_MASK_PIF_HS_EOL         ((u32)0x0000000F << 28)           /*!< R/W 0xF  threshold value to check if panelif hs_cnt almost reaching end of line*/
#define LCDC_PIF_HS_EOL(x)           (((u32)((x) & 0x0000000F) << 28))
#define LCDC_GET_PIF_HS_EOL(x)       ((u32)(((x >> 28) & 0x0000000F)))
#define LCDC_MASK_DMA_UDF_TIMEOUT    ((u32)0x00000FFF << 16)           /*!< R/W 0x400  threshold value for dma underflow timeout counter*/
#define LCDC_DMA_UDF_TIMEOUT(x)      (((u32)((x) & 0x00000FFF) << 16))
#define LCDC_GET_DMA_UDF_TIMEOUT(x)  ((u32)(((x >> 16) & 0x00000FFF)))
#define LCDC_MASK_DMA_FETCHED_THLD   ((u32)0x000000FF << 8)            /*!< R/W 0x0  threshold value for dma data_fetched indicator(start panelif FSM)*/
#define LCDC_DMA_FETCHED_THLD(x)     (((u32)((x) & 0x000000FF) << 8))
#define LCDC_GET_DMA_FETCHED_THLD(x) ((u32)(((x >> 8) & 0x000000FF)))
#define LCDC_BIT_DMA_TE_MODE         ((u32)0x00000001 << 4)            /*!< R/W 0x0  1: TE trigger-mode open, DMA refreshes frame manually based on panel TE input 0: use dma_triger_mode(bit 0 setting, auto or onetime)*/
#define LCDC_MASK_RD_OTSD            ((u32)0x00000003 << 2)            /*!< R/W 0x3  The value of read outstanding -1, for DMA burst size configuration*/
#define LCDC_RD_OTSD(x)              (((u32)((x) & 0x00000003) << 2))
#define LCDC_GET_RD_OTSD(x)          ((u32)(((x >> 2) & 0x00000003)))
#define LCDC_BIT_TRIGER_ONETIME      ((u32)0x00000001 << 1)            /*!< R/W/EC 0x0  Write '1', hardware DMA one frame from DMA buffer based on synchronous signal. After DMA completed,this bit is cleared automatically.*/
#define LCDC_BIT_DMA_TRIGER_MODE     ((u32)0x00000001 << 0)            /*!< R/W 0x0  DMA Trigger-mode enable. *0: Auto-mode, open, DMA refreshes automatically based on synchronous signal. *1: Trigger-mode open, DMA refreshes frame manually based on TRIGER_ONETIME bit of this register. Trigger mode is useful for LCD with internal GRAM.*/
/** @} */

/** @defgroup LCDC_SHW_RLD_CFG
 * @brief LCDC shadow reload configuration register
 * @{
 */
#define LCDC_MASK_DMA_BURST_OPT    ((u32)0x00000003 << 30)           /*!< R/W 0x0  0: axi 64-byte burst 1: axi 128-byte burst 2: axi 256-byte burst*/
#define LCDC_DMA_BURST_OPT(x)      (((u32)((x) & 0x00000003) << 30))
#define LCDC_GET_DMA_BURST_OPT(x)  ((u32)(((x >> 30) & 0x00000003)))
#define LCDC_MASK_DMA_AUTO_INTVL   ((u32)0x000FFFFF << 8)            /*!< R/W 0x0  interval between two dma transfer in auto-mode*/
#define LCDC_DMA_AUTO_INTVL(x)     (((u32)((x) & 0x000FFFFF) << 8))
#define LCDC_GET_DMA_AUTO_INTVL(x) ((u32)(((x >> 8) & 0x000FFFFF)))
#define LCDC_BIT_VBR               ((u32)0x00000001 << 0)            /*!< R/W/EC 0x0  vertical blanking reload. This bit is set by software and cleared only by hardware after reload (it cannot be cleared through register write once it is set). 0: no effect 1: The shadow registers are reloaded during the vertical blanking period (at the beginning of the first line after the active display area).*/
/** @} */

/** @defgroup LCDC_COLOR_CFG
 * @brief color config
 * @{
 */
#define LCDC_MASK_COLOR_FORMAT   ((u32)0x0000000F << 2)           /*!< R/W 0x0  0000: RGB888 0001: RGB565 0010: RGB666 1000: BGR888 1001: BGR565 1010: BGR666*/
#define LCDC_COLOR_FORMAT(x)     (((u32)((x) & 0x0000000F) << 2))
#define LCDC_GET_COLOR_FORMAT(x) ((u32)(((x >> 2) & 0x0000000F)))
/** @} */

/** @defgroup LCDC_IRQ_EN
 * @brief LCDC interrupt enable register
 * @{
 */
#define LCDC_BIT_PANEL_TE_INTEN   ((u32)0x00000001 << 6) /*!< R/W 0x0  panel TE input interrupt Enable*/
#define LCDC_BIT_FRM_START_INTEN  ((u32)0x00000001 << 5) /*!< R/W 0x0  DMA Frame start interrupt Enable. This bit can be set and cleared by software. * 0: DMA Frame start interrupt disable * 1: DMA Frame start interrupt enable*/
#define LCDC_BIT_IO_TIMEOUT_INTEN ((u32)0x00000001 << 4) /*!< R/W 0x0  Write or read timeout interrupt Enable in MCU I/F I/O mode. This bit can be set and cleared by software. * 0: MCU I/O mode write/read timeout interrupt disable * 1: MCU I/O mode write/read  timeout interrupt enable*/
#define LCDC_BIT_LCD_LIN_INTEN    ((u32)0x00000001 << 3) /*!< R/W 0x0  Line Interrupt Enable. This bit can be set and cleared by software. * 0: Line interrupt disable * 1: Line Interrupt enable*/
#define LCDC_BIT_LCD_FRD_INTEN    ((u32)0x00000001 << 2) /*!< R/W 0x0  LCD refresh frame done interrupt enable*/
#define LCDC_BIT_DMA_UN_INTEN     ((u32)0x00000001 << 0) /*!< R/W 0x0  DMA FIFO underflow interrupt enable*/
/** @} */

/** @defgroup LCDC_IRQ_STATUS
 * @brief LCDC interrupt status register
 * @{
 */
#define LCDC_BIT_PANEL_TE_INTS   ((u32)0x00000001 << 6) /*!< RW1C 0x0  panel TE input interrupt status. Write 1 to clear it. * 0: No panel TE input interrupt generated * 1: A panel TE input interrupt is generated*/
#define LCDC_BIT_FRM_START_INTS  ((u32)0x00000001 << 5) /*!< RW1C 0x0  DMA frame start interrupt status. Write 1 to clear it. * 0: No DMA frame start interrupt generated * 1: A DMA frame start interrupt is generated*/
#define LCDC_BIT_IO_TIMEOUT_INTS ((u32)0x00000001 << 4) /*!< RW1C 0x0  Write or read timeout interrupt status in MCU I/F I/O mode. Write 1 to clear it. * 0: No write/read timeout interrupt generated. * 1: A write/read timeout interrupt is generated.*/
#define LCDC_BIT_LCD_LIN_INTS    ((u32)0x00000001 << 3) /*!< RW1C 0x0  Line Interrupt status. Write 1 to clear it. * 0: No Line interrupt generated * 1: A Line interrupt is generated, when a programmed line is reached Note: When this bit is set and DMA fetches the first pixel data of the specified line number from frame buffer, the line interrupt happens. The line number depends on LCDC_LINE_INT_POS register value.*/
#define LCDC_BIT_LCD_FRD_INTS    ((u32)0x00000001 << 2) /*!< RW1C 0x0  LCD refresh frame done interrupt status. Write 1 to clear it. * 0: No LCD refresh frame done interrupt generated. * 1: This interrupt generated, when sending LCD frame done. Note: When this bit is set and DMA fetch the last pixel data from frame buffer, the After LCD refresh frame done, interrupt happens.*/
#define LCDC_BIT_DMA_UN_INTS     ((u32)0x00000001 << 0) /*!< RW1C 0x0  DMA FIFO underflow interrupt status. Write 1 to clear it. * 0: No DMA FIFO underflow interrupt generated * 1: Interrupt generated when DMA FIFO underflow happens*/
/** @} */

/** @defgroup LCDC_IRQ_RAW
 * @brief LCDC raw interrupt register
 * @{
 */
#define LCDC_BIT_PANEL_TE_INTRS   ((u32)0x00000001 << 6) /*!< R 0x0  panel TE input raw interrupt status. No matter panel TE input interrupt is enabled or disabled, this bit is set when panel TE input interrupt happens. Writting 1 to the PANEL_TE_INTS field in register LCDC_IRQ_STATUS is to clear this bit.*/
#define LCDC_BIT_FRM_START_INTRS  ((u32)0x00000001 << 5) /*!< R 0x0  DMA frame start raw interrupt status. No matter DMA frame start interrupt is enabled or disabled, this bit is set when DMA frame start interrupt happens. Writting 1 to the FRM_START_INTS field in register LCDC_IRQ_STATUS is to clear this bit.*/
#define LCDC_BIT_IO_TIMEOUT_INTRS ((u32)0x00000001 << 4) /*!< R 0x0  Write or read timeout interrupt raw interrupt status in MCU I/F I/O mode. No matter the I/O write/read timeout interrupt is enabled or disabled, this bit is set when I/O write/read timeout interrupt happens. Write 1 to the IO_TIMEOUT_INTS field in register LCDC_IRQ_STATUS to clear this bit.*/
#define LCDC_BIT_LCD_LIN_INTRS    ((u32)0x00000001 << 3) /*!< R 0x0  Line Interrupt raw status. No matter the line interrupt is enabled or disabled, this bit is set when line interrupt happens. Writting 1 to the LCD_LIN_INTS field in register LCDC_IRQ_STATUS is to clear this bit.*/
#define LCDC_BIT_LCD_FRD_INTRS    ((u32)0x00000001 << 2) /*!< R 0x0  LCD refresh frame done raw interrupt status. No matter the refresh done interrupt is enabled or disabled, this bit is set when refresh done interrupt happens. Writting 1 to the LCD_FRD_INTS field in register LCDC_IRQ_STATUS is to clear this bit.*/
#define LCDC_BIT_DMA_UN_INTRS     ((u32)0x00000001 << 0) /*!< R 0x0  DMA FIFO underflow raw interrupt status. When LCDC DMA FIFO under flow event happens, this bit is set. Write 1 to the DMAUNINTS field in register LCDC_IRQ_STATUS to clear this bit.*/
/** @} */

/** @defgroup LCDC_LINE_INT_POS
 * @brief LCDC line interrupt position register
 * @{
 */
#define LCDC_MASK_LINE_INT_POS   ((u32)0x00000FFF << 0)           /*!< R/W 0x0  Line Interrupt Position These bits configure the line interrupt position.*/
#define LCDC_LINE_INT_POS(x)     (((u32)((x) & 0x00000FFF) << 0))
#define LCDC_GET_LINE_INT_POS(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup LCDC_CUR_POS_STATUS
 * @brief Display current position
 * @{
 */
#define LCDC_MASK_CUR_POS_Y   ((u32)0x00000FFF << 16)           /*!< R 0x1  Current Y Position These bits return the current Y position.*/
#define LCDC_CUR_POS_Y(x)     (((u32)((x) & 0x00000FFF) << 16))
#define LCDC_GET_CUR_POS_Y(x) ((u32)(((x >> 16) & 0x00000FFF)))
#define LCDC_MASK_CUR_POS_X   ((u32)0x00000FFF << 0)            /*!< R 0x1  Current X Position These bits return the current X position.*/
#define LCDC_CUR_POS_X(x)     (((u32)((x) & 0x00000FFF) << 0))
#define LCDC_GET_CUR_POS_X(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup LCDC_DEBUG_STATUS
 * @brief Debug signal
 * @{
 */
#define LCDC_MASK_DEBUG_STATES   ((u32)0xFFFFFFFF << 0)           /*!< R 0x0  LCDC critial signal for debug*/
#define LCDC_DEBUG_STATES(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define LCDC_GET_DEBUG_STATES(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup LCDC_RGB_CFG
 * @brief LCDC RGB mode config
 * @{
 */
#define LCDC_MASK_RGB_SYNC_MODE   ((u32)0x00000003 << 25)           /*!< R/W 0x0  * 00: DE mode, frame synchronized with ENABLE signal * 01: HV mode, frame synchronized with synchronous signal * 10: TE mode * Others: Reserved*/
#define LCDC_RGB_SYNC_MODE(x)     (((u32)((x) & 0x00000003) << 25))
#define LCDC_GET_RGB_SYNC_MODE(x) ((u32)(((x >> 25) & 0x00000003)))
#define LCDC_BIT_RGBIF_UPDATE     ((u32)0x00000001 << 24)           /*!< R/W/EC 0x0  Force Hardware updates RGB I/F parameters after current LCD refresh frame done.  CPU writes 1 to force Hardware updating parameters. After updating, this bit is cleared. When the LCDC is running, if the following values related with RGB I/F mode are modified dynamically, only writing 1 to this bit can the newer value be used by hardware after the current frame refresh done. * The CLKDIV field in the LCDC_CLK_DIV register * The VFP, VBP, VSW fields in the LCDC_RGB_VSYNC_CFG register * The HBP, HFP, HSW fields in the LCDC_RGB_HSYNC_CFG register*/
#define LCDC_BIT_RGBTEPL          ((u32)0x00000001 << 21)           /*!< R/W 0x1  The TE pulse polarity. * 0: Low level for active pulse * 1: High level for active pulse*/
#define LCDC_BIT_RGB_DATPL        ((u32)0x00000001 << 20)           /*!< R/W 0x0  The Data pulse polarity.  * 0: Normal * 1: Inverted*/
#define LCDC_BIT_ENPL             ((u32)0x00000001 << 19)           /*!< R/W 0x1  The ENABLE pulse polarity. * 0: Low level for active data * 1: High level for active data*/
#define LCDC_BIT_HSPL             ((u32)0x00000001 << 18)           /*!< R/W 0x0  The HSYNC pulse polarity. * 0: Low level synchronous clock * 1: High level synchronous clock*/
#define LCDC_BIT_VSPL             ((u32)0x00000001 << 17)           /*!< R/W 0x0  The VSYNC pulse polarity. * 0: Low level synchronous clock * 1: High level synchronous clock*/
#define LCDC_BIT_DCLKPL           ((u32)0x00000001 << 16)           /*!< R/W 0x0  The polarity of the DCLK active edge. * 0: Data fetched at DCLK rising edge * 1: Data fetched at DCLK falling edge*/
#define LCDC_MASK_RGB_TEDELAY     ((u32)0x0000FFFF << 0)            /*!< R/W 0x0  The delay interval -5. This interval is from detected TE signal to starting frame transfer. Unit: WR pulse width. The maximum value is 65535. The real delay interval is TEDELAY + 4 or TEDELAY + 5.*/
#define LCDC_RGB_TEDELAY(x)       (((u32)((x) & 0x0000FFFF) << 0))
#define LCDC_GET_RGB_TEDELAY(x)   ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup LCDC_RGB_VSYNC_CFG
 * @brief RGB Vsync config
 * @{
 */
#define LCDC_MASK_VFP   ((u32)0x0000000F << 16)           /*!< R/W 0x3  Front porch line number-1. The number of inactive lines at the end of a frame, before vertical synchronization period.*/
#define LCDC_VFP(x)     (((u32)((x) & 0x0000000F) << 16))
#define LCDC_GET_VFP(x) ((u32)(((x >> 16) & 0x0000000F)))
#define LCDC_MASK_VBP   ((u32)0x0000000F << 8)            /*!< R/W 0x3  Back porch line number-1. The number of inactive lines at the start of a frame, after vertical synchronization period.*/
#define LCDC_VBP(x)     (((u32)((x) & 0x0000000F) << 8))
#define LCDC_GET_VBP(x) ((u32)(((x >> 8) & 0x0000000F)))
#define LCDC_MASK_VSW   ((u32)0x0000000F << 0)            /*!< R/W 0x1  Vertical synchronization signal width -1. Unit: inactive lines*/
#define LCDC_VSW(x)     (((u32)((x) & 0x0000000F) << 0))
#define LCDC_GET_VSW(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup LCDC_RGB_HSYNC_CFG
 * @brief RGB Hsync config
 * @{
 */
#define LCDC_MASK_HFP   ((u32)0x000000FF << 16)           /*!< R/W 0x1f  Horizontal front porch -1. The number of DCLK periods between the end of active data and the rising edge of HSYNC. HFP's minimum value is 1*/
#define LCDC_HFP(x)     (((u32)((x) & 0x000000FF) << 16))
#define LCDC_GET_HFP(x) ((u32)(((x >> 16) & 0x000000FF)))
#define LCDC_MASK_HBP   ((u32)0x000000FF << 8)            /*!< R/W 0x1f  Horizontal back porch -1. The number of DCLK periods between the falling edge of HSYNC and the start of active data. HBP's minimum value is 1*/
#define LCDC_HBP(x)     (((u32)((x) & 0x000000FF) << 8))
#define LCDC_GET_HBP(x) ((u32)(((x >> 8) & 0x000000FF)))
#define LCDC_MASK_HSW   ((u32)0x000000FF << 0)            /*!< R/W 0x17  Horizontal synchronization signal width -1. Unit: DCLK HSW's minimum value is 1*/
#define LCDC_HSW(x)     (((u32)((x) & 0x000000FF) << 0))
#define LCDC_GET_HSW(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup LCDC_RGB_CLK_DIV
 * @brief RGB clock divide
 * @{
 */
#define LCDC_MASK_CLKDIV   ((u32)0x00000FFF << 0)           /*!< R/W 0x2  DCLK clock divider depends on this value. The relationship between them is as follows: *0 : The DCLK clock divider is 2 *1 : The DCLK clock divider is 4 *2 : The DCLK clock divider is 6 *3 : The DCLK clock divider is 8 *... The frequency of DCLK is derived from the following equation: DCLK = SYS_CLK/((CLKDIV+1)*2)  where CLKDIV is any even value between 0 and 65534.*/
#define LCDC_CLKDIV(x)     (((u32)((x) & 0x00000FFF) << 0))
#define LCDC_GET_CLKDIV(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup LCDC_RGB_SYNC_STATUS
 * @brief RGB HSYNC/VSYNC status
 * @{
 */
#define LCDC_MASK_VSSTATUS   ((u32)0x00000003 << 2)           /*!< R 0x0  VSYNC Status. 00: VSYNC 01: VBP 11: ACTIVE 10: VFP*/
#define LCDC_VSSTATUS(x)     (((u32)((x) & 0x00000003) << 2))
#define LCDC_GET_VSSTATUS(x) ((u32)(((x >> 2) & 0x00000003)))
#define LCDC_MASK_HSSTATUS   ((u32)0x00000003 << 0)           /*!< R 0x0  HSYNC Status. 00: HSYNC 01: HBP 11: ACTIVE 10: HFP*/
#define LCDC_HSSTATUS(x)     (((u32)((x) & 0x00000003) << 0))
#define LCDC_GET_HSSTATUS(x) ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup LCDC_MCU_CFG
 * @brief LCDC mcu mode config
 * @{
 */
#define LCDC_MASK_TEDELAY         ((u32)0x0000FFFF << 16)           /*!< R/W 0x0  The delay interval -5. This interval is from detected TE signal to starting frame transfer. Unit: WR pulse width. The maximum value is 65535. The real delay interval is TEDELAY + 4 or TEDELAY + 5.*/
#define LCDC_TEDELAY(x)           (((u32)((x) & 0x0000FFFF) << 16))
#define LCDC_GET_TEDELAY(x)       ((u32)(((x >> 16) & 0x0000FFFF)))
#define LCDC_BIT_DAT_SWAP         ((u32)0x00000001 << 15)           /*!< R/W 0x0  swap dat[15:8] with dat[7:0] in mcu 8-bit RGB565*/
#define LCDC_BIT_MCU_IO_MODE_RUN  ((u32)0x00000001 << 12)           /*!< R 0x0  MCU I/F I/O mode run * 0: DMA mode run * 1: I/O mode run*/
#define LCDC_BIT_MCU_IO_MODE_EN   ((u32)0x00000001 << 11)           /*!< R/W 0x0  MCU I/F I/O mode enable * 0: Disable I/O mode and open DMA mode after I/O mode FIFO empty. * 1: Enable I/O mode after current frame refresh. Poll MCU_IO_MODE_RUN after updating this bit.*/
#define LCDC_MASK_MCU_SYNC_MODE   ((u32)0x00000003 << 9)            /*!< R/W 0x1  * 00: Synchronized with the internal clock * 01: Synchronized with VSYNC input * 10: Tearing effect line on (Mode1, VSYNC) * Others: Reserved*/
#define LCDC_MCU_SYNC_MODE(x)     (((u32)((x) & 0x00000003) << 9))
#define LCDC_GET_MCU_SYNC_MODE(x) ((u32)(((x >> 9) & 0x00000003)))
#define LCDC_BIT_MCUIF_UPDATE     ((u32)0x00000001 << 8)            /*!< R/W/EC 0x0  Force Hardware to update MCU I/F Timing shadow register at specific timing. CPU writes 1 to force Hardware updating. After Hardware updating finished, this bit is cleared. Software can't write related shadow registers when MCUIFUPDATE is still active. When the LCDC is running, if the following values related with MCU I/F mode are modified dynamically, only writing 1 to this bit can the newer value be used by hardware after the current frame refresh done. * The TEDELAY field in the LCDC_MCU_CFG register * The MCUVSW, MCUVSPD fields in the LCDC_MCU_VSYNC_CFG register * The WRPULW, RDACTW, RDINACTW fields in the LCDC_MCU_TIMING_CFG register*/
#define LCDC_BIT_MCUSYPL          ((u32)0x00000001 << 6)            /*!< R/W 0x0  The MCU VSYNC pulse polarity * 0: Low level for active pulse * 1: High level for active pulse*/
#define LCDC_BIT_TEPL             ((u32)0x00000001 << 5)            /*!< R/W 0x1  The TE pulse polarity. * 0: Low level for active pulse * 1: High level for active pulse*/
#define LCDC_BIT_MCU_DATPL        ((u32)0x00000001 << 4)            /*!< R/W 0x0  The Data pulse polarity. * 0: Normal * 1: Inverted*/
#define LCDC_BIT_RDPL             ((u32)0x00000001 << 3)            /*!< R/W 0x0  The RD pulse polarity. * 0: Data fetched at rising edge * 1: Data fetched at falling edge*/
#define LCDC_BIT_WRPL             ((u32)0x00000001 << 2)            /*!< R/W 0x0  The WR pulse polarity. * 0: Data fetched at rising edge * 1: Data fetched at falling edge*/
#define LCDC_BIT_RSPL             ((u32)0x00000001 << 1)            /*!< R/W 0x0  The RS pulse polarity. * 0: Low level for command address * 1: High level for command parameter*/
#define LCDC_BIT_CSPL             ((u32)0x00000001 << 0)            /*!< R/W 0x0  The CS pulse polarity. * 0: Low level for active pulse * 1: High level for active pulse*/
/** @} */

/** @defgroup LCDC_MCU_VSYNC_CFG
 * @brief MCU Vsync config
 * @{
 */
#define LCDC_MASK_MCUVSPD   ((u32)0x000FFFFF << 12)           /*!< R/W 0x0  * MCUIFMODE = 1: VSYNC idle period. This value equals to VSYNC period - IMAGEWIDTH * IMAGEHEIGHT * WR_pulse_per_pixel-5. Unit: WR pulse width. * MCUIFMODE = 0: The buffer time between frames. This value equals to the time of a frame's end to the next start -5. Unit: WR pulse width. * MCUIFMODE = 2: Invalid usage. The maximum value is 1048571.*/
#define LCDC_MCUVSPD(x)     (((u32)((x) & 0x000FFFFF) << 12))
#define LCDC_GET_MCUVSPD(x) ((u32)(((x >> 12) & 0x000FFFFF)))
#define LCDC_MASK_MCUVSW    ((u32)0x000000FF << 0)            /*!< R/W 0x0  VSYNC signal width - 1. Unit: WR pulse width. (only for MCU VSYNC mode)*/
#define LCDC_MCUVSW(x)      (((u32)((x) & 0x000000FF) << 0))
#define LCDC_GET_MCUVSW(x)  ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup LCDC_MCU_TIMING_CFG
 * @brief MCU timing config
 * @{
 */
#define LCDC_MASK_RDINACTW   ((u32)0x000007FF << 21)           /*!< R/W 0x0  RD inactive pulse width -1. Unit: system clock*/
#define LCDC_RDINACTW(x)     (((u32)((x) & 0x000007FF) << 21))
#define LCDC_GET_RDINACTW(x) ((u32)(((x >> 21) & 0x000007FF)))
#define LCDC_MASK_RDACTW     ((u32)0x000007FF << 10)           /*!< R/W 0x0  RD active pulse width -1. Unit: system clock*/
#define LCDC_RDACTW(x)       (((u32)((x) & 0x000007FF) << 10))
#define LCDC_GET_RDACTW(x)   ((u32)(((x >> 10) & 0x000007FF)))
#define LCDC_MASK_WRPULW     ((u32)0x000003FF << 0)            /*!< R/W 0x1  Write clock divider depends on this value. The relationship between them is as follows: * 0:  The write clock divider is 2 * 1:  The write clock divider is 4 * 2:  The write clock divider is 6 * 3:  The write clock divider is 8 *... The frequency of write clock is derived from the following equation: Write clock = SYS_CLK/((WRPULW+1) * 2)  where WRPULW is any even value between 0 and 1022.*/
#define LCDC_WRPULW(x)       (((u32)((x) & 0x000003FF) << 0))
#define LCDC_GET_WRPULW(x)   ((u32)(((x >> 0) & 0x000003FF)))
/** @} */

/** @defgroup LCDC_MCU_IO_DATA
 * @brief MCU IO mode config
 * @{
 */
#define LCDC_MASK_MCU_RW_DATA   ((u32)0x00FFFFFF << 0)           /*!< R 0x0  When reading this register, this lower 24 bits represent the data read from LCM through MCU I/O mode I/F. Note: When TX FIFO not full and I/O timeout interrupt not happen ,this data can be read.*/
#define LCDC_MCU_RW_DATA(x)     (((u32)((x) & 0x00FFFFFF) << 0))
#define LCDC_GET_MCU_RW_DATA(x) ((u32)(((x >> 0) & 0x00FFFFFF)))
/** @} */

/** @defgroup LCDC_MCU_IO_TO_CFG
 * @brief MCU IO timeout config
 * @{
 */
#define LCDC_MASK_IO_TIMEOUT_NUM   ((u32)0x0000FFFF << 0)           /*!< R/W 0x42  The I/O write/read timeout interrupthappens, When writing or reading is timeout for IO_RDY_TO_DOTCLK_NUM system clocks in MCU I/F I/O mode, t. When MCU I/F I/O mode is used, the recommand value is:  16 * WRPULW + RDACTW + RDINACTW*/
#define LCDC_IO_TIMEOUT_NUM(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define LCDC_GET_IO_TIMEOUT_NUM(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup LCDC_IMG_CFG
 * @brief source image config
 * @{
 */
#define LCDC_MASK_IMG_FORMAT   ((u32)0x0000000F << 4)           /*!< R/W 0x0  Image layer data format, it supports the following values: *0000: ARGB8888 *0001: RGB888 *0010: RGB565 *0011: ARGB1555 *0100: ARGB4444 *0101: RGB666 *0110: ARGB8666 *1000: ABGR8888 *1001: BGR888 *1010: BGR565 *Others: Reserved*/
#define LCDC_IMG_FORMAT(x)     (((u32)((x) & 0x0000000F) << 4))
#define LCDC_GET_IMG_FORMAT(x) ((u32)(((x >> 4) & 0x0000000F)))
/** @} */

/** @defgroup LCDC_IMG_A_BASE_ADDR
 * @brief LCDC image A base address
 * @{
 */
#define LCDC_MASK_IMG_A_BASE_ADDR   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Image layer DMA source address. After a frame refresh done, hardware loads the newer base address from this register automatically.*/
#define LCDC_IMG_A_BASE_ADDR(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define LCDC_GET_IMG_A_BASE_ADDR(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup LCDC_IMG_B_BASE_ADDR
 * @brief LCDC image B base address
 * @{
 */
#define LCDC_MASK_IMG_B_BASE_ADDR   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Image layer DMA source address. After a frame refresh done, hardware loads the newer base address from this register automatically.*/
#define LCDC_IMG_B_BASE_ADDR(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define LCDC_GET_IMG_B_BASE_ADDR(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup LCDC_IMG_SLIDE
 * @brief LCDC image slide
 * @{
 */
#define LCDC_MASK_IMG_VS_OFST   ((u32)0x00000FFF << 16)           /*!< R/W 0x0  pixel offset for image vertical slide*/
#define LCDC_IMG_VS_OFST(x)     (((u32)((x) & 0x00000FFF) << 16))
#define LCDC_GET_IMG_VS_OFST(x) ((u32)(((x >> 16) & 0x00000FFF)))
#define LCDC_MASK_IMG_HS_OFST   ((u32)0x00000FFF << 0)            /*!< R/W 0x0  pixel offset for image horizontal slide*/
#define LCDC_IMG_HS_OFST(x)     (((u32)((x) & 0x00000FFF) << 0))
#define LCDC_GET_IMG_HS_OFST(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup LCDC_MCU_CMD_DWORD0_0
 * @brief mcu predefined cmd
 * @{
 */
#define LCDC_MASK_MCU_CMD_DWORD0_0   ((u8)0x000000FF << 0)           /*!< R/W 0x0  mcu predefined cmd before data*/
#define LCDC_MCU_CMD_DWORD0_0(x)     (((u8)((x) & 0x000000FF) << 0))
#define LCDC_GET_MCU_CMD_DWORD0_0(x) ((u8)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup LCDC_MCU_CMD_DWORD0_1
 * @brief
 * @{
 */
#define LCDC_MASK_MCU_CMD_DWORD0_1   ((u8)0x000000FF << 0)           /*!< R/W 0x0  mcu predefined cmd before data*/
#define LCDC_MCU_CMD_DWORD0_1(x)     (((u8)((x) & 0x000000FF) << 0))
#define LCDC_GET_MCU_CMD_DWORD0_1(x) ((u8)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup LCDC_MCU_CMD_DWORD0_2
 * @brief
 * @{
 */
#define LCDC_MASK_MCU_CMD_DWORD0_2   ((u8)0x000000FF << 0)           /*!< R/W 0x0  mcu predefined cmd before data*/
#define LCDC_MCU_CMD_DWORD0_2(x)     (((u8)((x) & 0x000000FF) << 0))
#define LCDC_GET_MCU_CMD_DWORD0_2(x) ((u8)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup LCDC_MCU_CMD_DWORD0_3
 * @brief
 * @{
 */
#define LCDC_MASK_MCU_CMD_DWORD0_3   ((u8)0x000000FF << 0)           /*!< R/W 0x0  mcu predefined cmd before data*/
#define LCDC_MCU_CMD_DWORD0_3(x)     (((u8)((x) & 0x000000FF) << 0))
#define LCDC_GET_MCU_CMD_DWORD0_3(x) ((u8)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup LCDC_MCU_CMD_DWORD1_0
 * @brief
 * @{
 */
#define LCDC_MASK_MCU_CMD_DWORD1_0   ((u8)0x000000FF << 0)           /*!< R/W 0x0  mcu predefined cmd before data*/
#define LCDC_MCU_CMD_DWORD1_0(x)     (((u8)((x) & 0x000000FF) << 0))
#define LCDC_GET_MCU_CMD_DWORD1_0(x) ((u8)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup LCDC_MCU_CMD_DWORD1_1
 * @brief
 * @{
 */
#define LCDC_MASK_MCU_CMD_DWORD1_1   ((u8)0x000000FF << 0)           /*!< R/W 0x0  mcu predefined cmd before data*/
#define LCDC_MCU_CMD_DWORD1_1(x)     (((u8)((x) & 0x000000FF) << 0))
#define LCDC_GET_MCU_CMD_DWORD1_1(x) ((u8)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup LCDC_MCU_CMD_DWORD1_2
 * @brief
 * @{
 */
#define LCDC_MASK_MCU_CMD_DWORD1_2   ((u8)0x000000FF << 0)           /*!< R/W 0x0  mcu predefined cmd before data*/
#define LCDC_MCU_CMD_DWORD1_2(x)     (((u8)((x) & 0x000000FF) << 0))
#define LCDC_GET_MCU_CMD_DWORD1_2(x) ((u8)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup LCDC_MCU_CMD_DWORD1_3
 * @brief
 * @{
 */
#define LCDC_MASK_MCU_CMD_DWORD1_3   ((u8)0x000000FF << 0)           /*!< R/W 0x0  mcu predefined cmd before data*/
#define LCDC_MCU_CMD_DWORD1_3(x)     (((u8)((x) & 0x000000FF) << 0))
#define LCDC_GET_MCU_CMD_DWORD1_3(x) ((u8)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup LCDC_MCU_CMD_DWORD2_0
 * @brief
 * @{
 */
#define LCDC_MASK_MCU_CMD_DWORD2_0   ((u8)0x000000FF << 0)           /*!< R/W 0x0  mcu predefined cmd before data*/
#define LCDC_MCU_CMD_DWORD2_0(x)     (((u8)((x) & 0x000000FF) << 0))
#define LCDC_GET_MCU_CMD_DWORD2_0(x) ((u8)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup LCDC_MCU_CMD_DWORD2_1
 * @brief
 * @{
 */
#define LCDC_MASK_MCU_CMD_DWORD2_1   ((u8)0x000000FF << 0)           /*!< R/W 0x0  mcu predefined cmd before data*/
#define LCDC_MCU_CMD_DWORD2_1(x)     (((u8)((x) & 0x000000FF) << 0))
#define LCDC_GET_MCU_CMD_DWORD2_1(x) ((u8)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup LCDC_MCU_CMD_DWORD2_2
 * @brief
 * @{
 */
#define LCDC_MASK_MCU_CMD_DWORD2_2   ((u8)0x000000FF << 0)           /*!< R/W 0x0  mcu predefined cmd before data*/
#define LCDC_MCU_CMD_DWORD2_2(x)     (((u8)((x) & 0x000000FF) << 0))
#define LCDC_GET_MCU_CMD_DWORD2_2(x) ((u8)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup LCDC_MCU_CMD_DWORD2_3
 * @brief
 * @{
 */
#define LCDC_MASK_MCU_CMD_DWORD2_3   ((u8)0x000000FF << 0)           /*!< R/W 0x0  mcu predefined cmd before data*/
#define LCDC_MCU_CMD_DWORD2_3(x)     (((u8)((x) & 0x000000FF) << 0))
#define LCDC_GET_MCU_CMD_DWORD2_3(x) ((u8)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup LCDC_MCU_CMD_DWORD3_0
 * @brief
 * @{
 */
#define LCDC_MASK_MCU_CMD_DWORD3_0   ((u8)0x000000FF << 0)           /*!< R/W 0x0  mcu predefined cmd before data*/
#define LCDC_MCU_CMD_DWORD3_0(x)     (((u8)((x) & 0x000000FF) << 0))
#define LCDC_GET_MCU_CMD_DWORD3_0(x) ((u8)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup LCDC_MCU_CMD_DWORD3_1
 * @brief
 * @{
 */
#define LCDC_MASK_MCU_CMD_DWORD3_1   ((u8)0x000000FF << 0)           /*!< R/W 0x0  mcu predefined cmd before data*/
#define LCDC_MCU_CMD_DWORD3_1(x)     (((u8)((x) & 0x000000FF) << 0))
#define LCDC_GET_MCU_CMD_DWORD3_1(x) ((u8)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup LCDC_MCU_CMD_DWORD3_2
 * @brief
 * @{
 */
#define LCDC_MASK_MCU_CMD_DWORD3_2   ((u8)0x000000FF << 0)           /*!< R/W 0x0  mcu predefined cmd before data*/
#define LCDC_MCU_CMD_DWORD3_2(x)     (((u8)((x) & 0x000000FF) << 0))
#define LCDC_GET_MCU_CMD_DWORD3_2(x) ((u8)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup LCDC_MCU_CMD_DWORD3_3
 * @brief
 * @{
 */
#define LCDC_MASK_MCU_CMD_DWORD3_3   ((u8)0x000000FF << 0)           /*!< R/W 0x0  mcu predefined cmd before data*/
#define LCDC_MCU_CMD_DWORD3_3(x)     (((u8)((x) & 0x000000FF) << 0))
#define LCDC_GET_MCU_CMD_DWORD3_3(x) ((u8)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup LCDC_MCU_CMD_WRNUM
 * @brief number of mcu predefined cmd
 * @{
 */
#define LCDC_MASK_MCU_CMD_WRNUM   ((u32)0x000000FF << 0)           /*!< R/W 0x0  number of mcu predefined cmd, unit: byte*/
#define LCDC_MCU_CMD_WRNUM(x)     (((u32)((x) & 0x000000FF) << 0))
#define LCDC_GET_MCU_CMD_WRNUM(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup LCDC_Exported_Types LCDC Exported Types
 * @{
 */

/** @brief LCDC Register Declaration
 */
typedef struct {
	__IO uint32_t LCDC_CTRL            ;  /*!< LCDC CONTROL REGISTER,  Address offset:0x000 */
	__IO uint32_t LCDC_PLANE_SIZE      ;  /*!< LCDC PLANE SIZE REGISTER,  Address offset:0x004 */
	__IO uint32_t LCDC_UNDFLW_CFG      ;  /*!< LCDC UNDERFLOW CONFIGURATION REGISTER,  Address offset:0x008 */
	__IO uint32_t LCDC_DMA_MODE_CFG    ;  /*!< LCDC DMA MODE CONFIGURATION REGISTER,  Address offset:0x00C */
	__IO uint32_t LCDC_SHW_RLD_CFG     ;  /*!< LCDC SHADOW RELOAD CONFIGURATION REGISTER,  Address offset:0x010 */
	__IO uint32_t LCDC_COLOR_CFG       ;  /*!< COLOR CONFIG Register,  Address offset:0x014 */
	__IO uint32_t RSVD0                ;  /*!< Reserved,  Address offset:0x018-0x01B */
	__IO uint32_t LCDC_SEC_DEST_ADDR   ;  /*!< LCDC SECOND IMAGE DESTINATION ADDRESS REGISTER,  Address offset:0x01C */
	__IO uint32_t LCDC_IRQ_EN          ;  /*!< LCDC INTERRUPT ENABLE REGISTER,  Address offset:0x020 */
	__IO uint32_t LCDC_IRQ_STATUS      ;  /*!< LCDC INTERRUPT STATUS REGISTER,  Address offset:0x024 */
	__I  uint32_t LCDC_IRQ_RAW         ;  /*!< LCDC RAW INTERRUPT REGISTER,  Address offset:0x028 */
	__IO uint32_t LCDC_LINE_INT_POS    ;  /*!< LCDC LINE INTERRUPT POSITION REGISTER,  Address offset:0x02C */
	__I  uint32_t LCDC_CUR_POS_STATUS  ;  /*!< DISPLAY CURRENT POSITION Register,  Address offset:0x030 */
	__IO uint32_t LCDC_STATUS          ;  /*!< UNDERFLOW TIMES Register,  Address offset:0x034 */
	__IO uint32_t RSVD1                ;  /*!< Reserved,  Address offset:0x038-0x03B */
	__I  uint32_t LCDC_DEBUG_STATUS    ;  /*!< DEBUG SIGNAL Register,  Address offset:0x03C */
	__IO uint32_t LCDC_RGB_CFG         ;  /*!< LCDC RGB MODE CONFIG Register,  Address offset:0x040 */
	__IO uint32_t LCDC_RGB_VSYNC_CFG   ;  /*!< RGB VSYNC CONFIG Register,  Address offset:0x044 */
	__IO uint32_t LCDC_RGB_HSYNC_CFG   ;  /*!< RGB HSYNC CONFIG Register,  Address offset:0x048 */
	__IO uint32_t LCDC_RGB_CLK_DIV     ;  /*!< RGB CLOCK DIVIDE Register,  Address offset:0x04C */
	__I  uint32_t LCDC_RGB_SYNC_STATUS ;  /*!< RGB HSYNC/VSYNC STATUS Register,  Address offset:0x050 */
	__IO uint32_t RSVD2[3]             ;  /*!< Reserved,  Address offset:0x054-0x05F */
	__IO uint32_t LCDC_MCU_CFG         ;  /*!< LCDC MCU MODE CONFIG Register,  Address offset:0x060 */
	__IO uint32_t LCDC_MCU_VSYNC_CFG   ;  /*!< MCU VSYNC CONFIG Register,  Address offset:0x064 */
	__IO uint32_t LCDC_MCU_TIMING_CFG  ;  /*!< MCU TIMING CONFIG Register,  Address offset:0x068 */
	__IO  uint32_t LCDC_MCU_IO_DATA     ;  /*!< MCU IO MODE CONFIG Register,  Address offset:0x06C */
	__IO uint32_t LCDC_MCU_IO_TO_CFG   ;  /*!< MCU IO TIMEOUT CONFIG Register,  Address offset:0x070 */
	__IO uint32_t RSVD3[3]             ;  /*!< Reserved,  Address offset:0x074-0x07F */
	__IO uint32_t LCDC_IMG_CFG         ;  /*!< SOURCE IMAGE CONFIG Register,  Address offset:0x080 */
	__IO uint32_t LCDC_IMG_A_BASE_ADDR ;  /*!< LCDC IMAGE A BASE ADDRESS Register,  Address offset:0x084 */
	__IO uint32_t LCDC_IMG_B_BASE_ADDR ;  /*!< LCDC IMAGE B BASE ADDRESS Register,  Address offset:0x088 */
	__IO uint32_t LCDC_IMG_SLIDE       ;  /*!< LCDC IMAGE SLIDE Register,  Address offset:0x08C */
	__IO uint32_t RSVD4[20]            ;  /*!< Reserved,  Address offset:0x090-0x0DF */
	__IO uint8_t LCDC_MCU_CMD_DWORD0_0;  /*!< MCU PREDEFINED CMD Register,  Address offset:0x0E0 */
	__IO uint8_t LCDC_MCU_CMD_DWORD0_1;  /*!< Register,  Address offset:0x0E1 */
	__IO uint8_t LCDC_MCU_CMD_DWORD0_2;  /*!< Register,  Address offset:0x0E2 */
	__IO uint8_t LCDC_MCU_CMD_DWORD0_3;  /*!< Register,  Address offset:0x0E3 */
	__IO uint8_t LCDC_MCU_CMD_DWORD1_0;  /*!< Register,  Address offset:0x0E4 */
	__IO uint8_t LCDC_MCU_CMD_DWORD1_1;  /*!< Register,  Address offset:0x0E5 */
	__IO uint8_t LCDC_MCU_CMD_DWORD1_2;  /*!< Register,  Address offset:0x0E6 */
	__IO uint8_t LCDC_MCU_CMD_DWORD1_3;  /*!< Register,  Address offset:0x0E7 */
	__IO uint8_t LCDC_MCU_CMD_DWORD2_0;  /*!< Register,  Address offset:0x0E8 */
	__IO uint8_t LCDC_MCU_CMD_DWORD2_1;  /*!< Register,  Address offset:0x0E9 */
	__IO uint8_t LCDC_MCU_CMD_DWORD2_2;  /*!< Register,  Address offset:0x0EA */
	__IO uint8_t LCDC_MCU_CMD_DWORD2_3;  /*!< Register,  Address offset:0x0EB */
	__IO uint8_t LCDC_MCU_CMD_DWORD3_0;  /*!< Register,  Address offset:0x0EC */
	__IO uint8_t LCDC_MCU_CMD_DWORD3_1;  /*!< Register,  Address offset:0x0ED */
	__IO uint8_t LCDC_MCU_CMD_DWORD3_2;  /*!< Register,  Address offset:0x0EE */
	__IO uint8_t LCDC_MCU_CMD_DWORD3_3;  /*!< Register,  Address offset:0x0EF */
	__IO uint32_t LCDC_MCU_CMD_WRNUM   ;  /*!< NUMBER OF MCU PREDEFINED CMD Register,  Address offset:0x0F0 */
	__IO uint32_t RSVD5[2]             ;  /*!< Reserved,  Address offset:0x0F4-0x0FB */
	__IO uint32_t LCDC_DUMMY           ;  /*!< Register,  Address offset:0x0FC */
} LCDC_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */

//Please add your definition here
/* Exported Types --------------------------------------------------------*/

typedef struct {
	u32	IfWidth;					/*!< Specifies the LCDC MCU Interface Mode.
											This parameter can be a value of @ref LCDC_MCU_IF_WIDTH_DEFINE in MCU I/F MODE,
											This parameter can be a value of @ref LCDC_RGB_IF_WIDTH_DEFINE in RGB I/F MODE */
	u32	ImgHeight;				/*!< Specifies the height of the LCDC PlANE SIZE.
											This parameter can be a number between 0x000 and 0xfff. */
	u32	ImgWidth;					/*!< Specifies the width of the LCDC PlANE SIZE.
											This parameter can be a number between 0x000 and 0xfff. */
	u32	InputFormat;				/*!< Specifies the input format.
											This parameter can be a value @ref LCDC_INPUT_FORMAT_DEFINE. */
	u32	OutputFormat;				/*!< Specifies the output format.
											This parameter can be a value @ref LCDC_OUTPUT_FORMAT_DEFINE. */
	u32 RGBRefreshFreq;			/*!< Specifies the RGB frame refresh frequency.
											This parameter can be a number between 0x000 and 0xfff.*/
} Panel_InitDef;

typedef struct {
	u32 RgbVfp;					/*!< Specifies the front porch line number.
											This parameter can be a number between 0x1 and 0x10. Unit: line*/
	u32 RgbVbp;					/*!< Specifies the back porch line number.
											This parameter can be a number between 0x1 and 0x10. Unit: line*/
	u32 RgbVsw;					/*!< Specifies the vertical signal width.
											This parameter can be a number between 0x1 and 0x10. Unit: line*/
	u32 RgbHfp;					/*!< Specifies the Horizontal front porch Unit: DCLK.
											This parameter can be a number between 0x01 and 0x100. Unit: DCLK*/
	u32 RgbHbp;					/*!< Specifies the Horizontal back porch Unit: DCLK.
											This parameter can be a number between 0x01 and 0x100. Unit: DCLK*/
	u32 RgbHsw;					/*!< Specifies the Horizontal sync signal width.
											This parameter can be a number between 0x01 and 0x100. Unit: DCLK*/
	struct {
		u8 RgbEnPolar;			/*!< Specifies the RGB ENABLE pulse polariy.
											This parameter can be  a value of @ref LCDC_RGB_ENABLE_PUL_POLARITY_DEFINE */
		u8 RgbHsPolar;			/*!< Specifies the RGB HSYNC pulse polariy.
											This parameter can be  a value of @ref LCDC_RGB_HSYNC_PUL_POLARITY_DEFINE */
		u8 RgbVsPolar;			/*!< Specifies the RGB VSYNC pulse polariy.
											This parameter can be  a value of @ref LCDC_RGB_VSYNC_PUL_POLARITY_DEFINE */
		u8 RgbDclkActvEdge;		/*!< Specifies the RGB DCLK active edge.
											This parameter can be  a value of @ref LCDC_RGB_DCLK_ACTIVE_EDGE_DEFINE */
	} Flags;
} Panel_RgbTimingDef;

typedef struct {
	u8	McuSyncPolar;			/*!< Specifies the LCDC MCU vsync pulse polarity.
										This parameter can be a value of @ref LCDC_MCU_VSYNC_PUL_POLARITY_DEFINE. */

	u8	McuTePolar;				/*!< Specifies the LCDC MCU TE pulse polarity.
										This parameter can be a value of @ref LCDC_MCU_TE_PUL_POLARITY_DEFINE.*/

	u8	McuRdPolar;				/*!< Specifies the LCDC MCU read pulse polarity.
										This parameter can be a value of @ref LCDC_MCU_READ_PUL_POLARITY_DEFINE. */

	u8	McuWrPolar;				/*!< Specifies the LCDC MCU write pulse polarity.
										This parameter can be a value of @ref LCDC_MCU_WRITE_PUL_POLARITY_DEFINE. */

	u8	McuRsPolar;				/*!< Specifies the LCDC MCU RS pulse polarity.
										This parameter can be a value of @ref LCDC_MCU_RS_PUL_POLARITY_DEFINE. */
} Panel_McuTimingDef;

typedef struct {
	u16 TriggerDma;//1: trigger, 0: auto

	u16 TeMode;//1: te mode, 0: not te_mode
	u32 TeDelay;
} Lcdc_McuDmaCfgDef;

/**
 * @brief  LCDC MCU I/F Mode Initialization structure definition
 */
typedef struct {
	/* config panel info */
	Panel_InitDef Panel_Init;

	/* config signal timing info */
	Panel_McuTimingDef Panel_McuTiming;
} LCDC_MCUInitTypeDef;


/**
 * @brief LCDC RGB I/F Mode Initialization structure definition
 */
typedef struct {
	/* config panel info */
	Panel_InitDef Panel_Init;

	/* config signal timing info */
	Panel_RgbTimingDef Panel_RgbTiming;
} LCDC_RGBInitTypeDef;

/** @} */


/* Exported constants --------------------------------------------------------*/
/** @defgroup LCDC_Exported_Constants LCDC Exported Constants
 * @{
 */

/** @defgroup LCDC_MCU_IF_WIDTH_DEFINE
 * @{
 */
#define LCDC_MCU_IF_8_BIT						(0x0)		/* support RGB565/RG888 */
#define LCDC_MCU_IF_16_BIT						(0x1)		/* support RGB565 */
#define LCDC_MCU_IF_9_BIT						(0x2)		/* support RGB666 */
#define LCDC_MCU_IF_18_BIT						(0x3)		/* support RGB666 */
#define LCDC_MCU_IF_24_BIT						(0x4)		/* support RGB888 */

#define IS_LCDC_MCU_IF_MODE(MODE)				(((MODE) == LCDC_MCU_IF_8_BIT) || \
												((MODE) == LCDC_MCU_IF_16_BIT) || \
												((MODE) == LCDC_MCU_IF_9_BIT) || \
												((MODE) == LCDC_MCU_IF_18_BIT) || \
												((MODE) == LCDC_MCU_IF_24_BIT))

/** @} */


/** @defgroup LCDC_RGB_IF_WIDTH_DEFINE
 * @{
 */
#define LCDC_RGB_IF_6_BIT						(0x8)		/* support RGB565/RGB666 */
#define LCDC_RGB_IF_16_BIT						(0x9)		/* support RGB565 */
#define LCDC_RGB_IF_18_BIT						(0xa)		/* support RGB666 */
#define LCDC_RGB_IF_8_BIT						(0xb)		/* support RGB888 */
#define LCDC_RGB_IF_24_BIT						(0xc)		/* support RGB888 */

#define IS_LCDC_RGB_IF_MODE(MODE)				(((MODE) == LCDC_RGB_IF_6_BIT) || \
												((MODE) == LCDC_RGB_IF_16_BIT) || \
												((MODE) == LCDC_RGB_IF_18_BIT) || \
												((MODE) == LCDC_RGB_IF_8_BIT) || \
												((MODE) == LCDC_RGB_IF_24_BIT))

/** @} */


/** @defgroup LCDC_DMA_UNDERFLOW_MODE_DEFINE
 * @{
 */
#define LCDC_DMA_UNDFLW_OUTPUT_LASTDATA			(0x0)
#define LCDC_DMA_UNDFLW_OUTPUT_ERRORDATA		(0x1)
#define IS_LCDC_DMA_UNDFLOW_MODE(MODE)		(((MODE) == LCDC_DMA_UNDFLW_OUTPUT_LASTDATA) || \
												((MODE) == LCDC_DMA_UNDFLW_OUTPUT_ERRORDATA))

/** @} */

/** @defgroup LCDC_DMA_UNDERFLOW_OPTION_DEFINE
 * @{
 */
#define LCDC_DMA_UNDFLOW_INSTANT_ERRDATA_CURFRAME		(0x0)
#define LCDC_DMA_UNDFLOW_PAUSE_ERRDATA_CURFRAME			(0x1)
#define LCDC_DMA_UNDFLOW_INSTANT_DROP_LINE				(0x2)
#define LCDC_DMA_UNDFLOW_PAUSE_DROP_LINE				(0x3)

#define IS_LCDC_DMA_UNDFLOW_OPTION(OPT)					(((OPT) == LCDC_DMA_UNDFLOW_INSTANT_ERRDATA_CURFRAME) || \
														((OPT) == LCDC_DMA_UNDFLOW_PAUSE_ERRDATA_CURFRAME) || \
														((OPT) == LCDC_DMA_UNDFLOW_INSTANT_DROP_LINE) || \
														((OPT) == LCDC_DMA_UNDFLOW_PAUSE_DROP_LINE))

/** @} */

/** @defgroup LCDC_DMA_MODE_DEFINE
 * @{
 */
#define LCDC_AUTO_DMA_MODE						(0x0)
#define LCDC_TRIGGER_DMA_MODE					(0x1)

#define IS_LCDC_DMA_MODE(MODE)					(((MODE) == LCDC_AUTO_DMA_MODE) || \
												((MODE) == LCDC_TRIGGER_DMA_MODE))

/** @} */

/** @defgroup LCDC_DMA_BURST_SIZE
 * @{
 */
#define LCDC_DMA_BURSTSIZE_1X64BYTES			(0x0)
#define LCDC_DMA_BURSTSIZE_2X64BYTES			(0x1)
#define LCDC_DMA_BURSTSIZE_4X64BYTES			(0x2)

#define IS_LCDC_DMA_BURSTSIZE(x)				(((x) == LCDC_DMA_BURSTSIZE_1X64BYTES) || \
												((x) == LCDC_DMA_BURSTSIZE_2X64BYTES) || \
												((x) == LCDC_DMA_BURSTSIZE_4X64BYTES) )
/** @} */




/** @defgroup LCDC_INTR_TYPE_DEFINE
 * @{
 */
#define LCDC_INTR_TYPE_MASK						(LCDC_BIT_FRM_START_INTEN | \
												LCDC_BIT_IO_TIMEOUT_INTEN | \
												LCDC_BIT_LCD_LIN_INTEN | \
												LCDC_BIT_LCD_FRD_INTEN | \
												LCDC_BIT_PANEL_TE_INTEN | \
												LCDC_BIT_DMA_UN_INTEN)

#define IS_LCDC_INTR_TYPE(TYPE)					(((TYPE) & (~LCDC_INTR_TYPE_MASK)) == 0)

/** @} */


/** @defgroup LCDC_INTR_STATUS_DEFINE
 * @{
 */
#define LCDC_INTR_STATUS_ALL_BITS			(LCDC_BIT_FRM_START_INTS | \
											LCDC_BIT_IO_TIMEOUT_INTS | \
											LCDC_BIT_LCD_LIN_INTS | \
											LCDC_BIT_LCD_FRD_INTS | \
											LCDC_BIT_PANEL_TE_INTS | \
											LCDC_BIT_DMA_UN_INTS)

#define IS_LCDC_INTR_STATUS(TYPE)			(((TYPE) & ~LCDC_INTR_STATUS_ALL_BITS) == 0)

/** @} */


/** @defgroup LCDC_RGB_SYNC_MODE_DEFINE
 * @{
 */
#define LCDC_RGB_DE_MODE						(0)
#define LCDC_RGB_HV_MODE						(1)
#define LCDC_RGB_TE_MODE						(2)

#define IS_LCDC_RGB_SYNC_MODE(MODE)				(((MODE) == LCDC_RGB_DE_MODE) || \
												((MODE) == LCDC_RGB_HV_MODE) || \
												((MODE) == LCDC_RGB_TE_MODE))

/** @} */

/** @defgroup LCDC_RGB_DMA_MODE_DEFINE
 * @{
 */
#define LCDC_RGB_DMA_AUTO_MODE					(0)
#define LCDC_RGB_DMA_TRIGGER_MODE				(1)

#define IS_LCDC_RGB_DMA_MODE(MODE)				(((MODE) == LCDC_RGB_DMA_AUTO_MODE) || \
												((MODE) == LCDC_RGB_DMA_TRIGGER_MODE) )

/** @} */

/** @defgroup LCDC_RGB_DATA_PUL_POLARITY_DEFINE
 * @{
 */
#define LCDC_RGB_DAT_PUL_NORMAL					(0)
#define LCDC_RGB_DAT_PUL_INVERT					(1)
#define IS_LCDC_RGB_DAT_PUL_POLARITY(POLARITY)	(((POLARITY) == LCDC_RGB_DAT_PUL_NORMAL) || \
												((POLARITY) == LCDC_RGB_DAT_PUL_INVERT))

/** @} */


/** @defgroup LCDC_RGB_ENABLE_PUL_POLARITY_DEFINE
 * @{
 */
#define LCDC_RGB_EN_PUL_LOW_LEV_ACTIVE			(0)
#define LCDC_RGB_EN_PUL_HIGH_LEV_ACTIVE			(1)

#define IS_LCDC_RGB_EN_PUL_POLARITY(POLARITY)	(((POLARITY) == LCDC_RGB_EN_PUL_LOW_LEV_ACTIVE) || \
												((POLARITY) == LCDC_RGB_EN_PUL_HIGH_LEV_ACTIVE))

/** @} */


/** @defgroup LCDC_RGB_HSYNC_PUL_POLARITY_DEFINE
 * @{
 */
#define LCDC_RGB_HS_PUL_LOW_LEV_SYNC			(0)
#define LCDC_RGB_HS_PUL_HIGH_LEV_SYNC			(1)
#define IS_LCDC_RGB_HS_PUL_POLARITY(POLARITY)	(((POLARITY) == LCDC_RGB_HS_PUL_LOW_LEV_SYNC) || \
												((POLARITY) == LCDC_RGB_HS_PUL_HIGH_LEV_SYNC))

/** @} */

/** @defgroup LCDC_RGB_VSYNC_PUL_POLARITY_DEFINE
 * @{
 */
#define LCDC_RGB_VS_PUL_LOW_LEV_SYNC			(0)
#define LCDC_RGB_VS_PUL_HIGH_LEV_SYNC			(1)
#define IS_LCDC_RGB_VS_PUL_POLARITY(POLARITY)	(((POLARITY) == LCDC_RGB_VS_PUL_LOW_LEV_SYNC) || \
												((POLARITY) == LCDC_RGB_VS_PUL_HIGH_LEV_SYNC))

/** @} */

/** @defgroup LCDC_RGB_DCLK_ACTIVE_EDGE_DEFINE
 * @{
 */
#define LCDC_RGB_DCLK_RISING_EDGE_FETCH			(0)
#define LCDC_RGB_DCLK_FALLING_EDGE_FETCH		(1)
#define IS_LCDC_RGB_DCLK_EDGE(EDGE)				(((EDGE) == LCDC_RGB_DCLK_RISING_EDGE_FETCH) || \
												((EDGE) == LCDC_RGB_DCLK_FALLING_EDGE_FETCH))

/** @} */

/** @defgroup LCDC_RGB_TE_PUL_POLARITY_DEFINE
 * @{
 */
#define LCDC_RGB_TE_PUL_LOW_LEV_SYNC			(0)
#define LCDC_RGB_TE_PUL_HIGH_LEV_SYNC			(1)
#define IS_LCDC_RGB_TE_PUL_POLARITY(POLARITY)	(((POLARITY) == LCDC_RGB_TE_PUL_LOW_LEV_SYNC) || \
												((POLARITY) == LCDC_RGB_TE_PUL_HIGH_LEV_SYNC))

/** @} */

/** @defgroup LCDC_MCU_SWAP_DEFINE
 * @{
 */
#define LCDC_MCU_SWAP_DISABLE					(0)
#define LCDC_MCU_SWAP_ENABLE					(1)

#define IS_LCDC_MCU_SWAP_MODE(MODE)				(((MODE) == LCDC_MCU_SWAP_DISABLE) || \
												((MODE) == LCDC_MCU_SWAP_ENABLE))

/** @} */

/** @defgroup LCDC_MCU_MODE_DEFINE
 * @{
 */
#define LCDC_MCU_DMA_MODE						(0)
#define LCDC_MCU_IO_MODE						(1)

#define IS_LCDC_MCU_MODE(MODE)					(((MODE) == LCDC_MCU_IO_MODE) || \
												((MODE) == LCDC_MCU_DMA_MODE))

/** @} */


/** @defgroup LCDC_MCU_SYNC_MODE_DEFINE
 * @{
 */
#define LCDC_MCU_SYNC_WITH_INTERNAL_CLK			(0)
#define LCDC_MCU_SYNC_WITH_VSYNC				(1)
#define LCDC_MCU_SYNC_WITH_TE					(2)

#define IS_LCDC_MCU_SYNC_MODE(MODE)				(((MODE) == LCDC_MCU_SYNC_WITH_INTERNAL_CLK) || \
												((MODE) == LCDC_MCU_SYNC_WITH_VSYNC) || \
												((MODE) == LCDC_MCU_SYNC_WITH_TE))

/** @} */

/** @defgroup LCDC_MCU_VSYNC_PUL_POLARITY_DEFINE
 * @{
 */
#define LCDC_MCU_VSYNC_PUL_LOW_LEV_ACTIVE		(0)
#define LCDC_MCU_VSYNC_PUL_HIGH_LEV_ACTIVE		(1)
#define IS_LCDC_MCU_VSYNC_PUL_POLARITY(POL)		(((POL) == LCDC_MCU_VSYNC_PUL_LOW_LEV_ACTIVE) || \
												((POL) == LCDC_MCU_VSYNC_PUL_HIGH_LEV_ACTIVE))

/** @} */

/** @defgroup LCDC_MCU_TE_PUL_POLARITY_DEFINE
 * @{
 */
#define LCDC_MCU_TE_PUL_LOW_LEV_ACTIVE			(0)
#define LCDC_MCU_TE_PUL_HIGH_LEV_ACTIVE			(1)
#define IS_LCDC_MCU_TE_PUL_POLARITY(POL) 		(((POL) == LCDC_MCU_TE_PUL_LOW_LEV_ACTIVE) || \
												((POL) == LCDC_MCU_TE_PUL_HIGH_LEV_ACTIVE))

/** @} */

/** @defgroup LCDC_MCU_DATA_PUL_POLARITY_DEFINE
 * @{
 */
#define LCDC_MCU_DAT_PUL_NORMAL					(0)
#define LCDC_MCU_DAT_PUL_INVERT					(1)
#define IS_LCDC_MCU_DAT_PUL_POLARITY(POLARITY)	(((POLARITY) == LCDC_MCU_DAT_PUL_NORMAL) || \
												((POLARITY) == LCDC_MCU_DAT_PUL_INVERT))

/** @} */

/** @defgroup LCDC_MCU_READ_PUL_POLARITY_DEFINE
 * @{
 */
#define LCDC_MCU_RD_PUL_RISING_EDGE_FETCH		(0)
#define LCDC_MCU_RD_PUL_FALLING_EDGE_FETCH		(1)
#define IS_LCDC_MCU_RD_PUL_POLARITY(POLARITY)	(((POLARITY) == LCDC_MCU_RD_PUL_RISING_EDGE_FETCH) || \
												((POLARITY) == LCDC_MCU_RD_PUL_FALLING_EDGE_FETCH))

/** @} */

/** @defgroup LCDC_MCU_WRITE_PUL_POLARITY_DEFINE
 * @{
 */
#define LCDC_MCU_WR_PUL_RISING_EDGE_FETCH		(0)
#define LCDC_MCU_WR_PUL_FALLING_EDGE_FETCH		(1)
#define IS_LCDC_MCU_WR_PUL_POLARITY(POLARITY)	(((POLARITY) == LCDC_MCU_WR_PUL_RISING_EDGE_FETCH) || \
												((POLARITY) == LCDC_MCU_WR_PUL_FALLING_EDGE_FETCH))

/** @} */

/** @defgroup LCDC_MCU_RS_PUL_POLARITY_DEFINE
 * @{
 */
#define LCDC_MCU_RS_PUL_LOW_LEV_CMD_ADDR		(0)
#define LCDC_MCU_RS_PUL_HIGH_LEV_CMD_ADDR		(1)
#define IS_LCDC_MCU_RS_PUL_POLARITY(POLARITY)	(((POLARITY) == LCDC_MCU_RS_PUL_LOW_LEV_CMD_ADDR) || \
												((POLARITY) == LCDC_MCU_RS_PUL_HIGH_LEV_CMD_ADDR))

/** @} */

/** @defgroup LCDC_MCU_CS_PUL_POLARITY_DEFINE
 * @{
 */
#define LCDC_MCU_CS_PUL_LOW_LEV_ACTIVE			(0)
#define LCDC_MCU_CS_PUL_HIGH_LEV_ACTIVE			(1)
#define IS_LCDC_MCU_CS_PUL_POLARITY(POLARITY)	(((POLARITY) == LCDC_MCU_CS_PUL_LOW_LEV_ACTIVE) || \
												((POLARITY) == LCDC_MCU_CS_PUL_HIGH_LEV_ACTIVE))

/** @} */


/** @defgroup LCDC_INPUT_FORMAT_DEFINE
 * @{
 */
#define LCDC_INPUT_FORMAT_ARGB8888				(0x0)
#define LCDC_INPUT_FORMAT_RGB888				(0x1)
#define LCDC_INPUT_FORMAT_RGB565				(0x2)
#define LCDC_INPUT_FORMAT_ARGB1555				(0x3)
#define LCDC_INPUT_FORMAT_ARGB4444				(0x4)
#define LCDC_INPUT_FORMAT_RGB666				(0x5)
#define LCDC_INPUT_FORMAT_ARGB8666				(0x6)

#define LCDC_INPUT_FORMAT_ABGR8888				(0x8)
#define LCDC_INPUT_FORMAT_BGR888				(0x9)
#define LCDC_INPUT_FORMAT_BGR565				(0xa)
#define IS_LCDC_INPUT_FORMAT(MODE)				(((MODE) <= LCDC_INPUT_FORMAT_BGR565) && ((MODE) != 0x7))

/** @} */

/** @defgroup LCDC_OUTPUT_FORMAT_DEFINE
 * @{
 */
#define LCDC_OUTPUT_FORMAT_RGB888				(0x0)
#define LCDC_OUTPUT_FORMAT_RGB565				(0x1)
#define LCDC_OUTPUT_FORMAT_RGB666				(0x2)

#define LCDC_OUTPUT_FORMAT_BGR888				(0x8)//(0x4)
#define LCDC_OUTPUT_FORMAT_BGR565				(0x9)
#define LCDC_OUTPUT_FORMAT_BGR666				(0xa)

#define IS_LCDC_OUTPUT_FORMAT(MODE)				(((MODE) == LCDC_OUTPUT_FORMAT_RGB888) || \
												((MODE) == LCDC_OUTPUT_FORMAT_RGB565) || \
												((MODE) == LCDC_OUTPUT_FORMAT_RGB666) || \
												((MODE) == LCDC_OUTPUT_FORMAT_BGR888) || \
												((MODE) == LCDC_OUTPUT_FORMAT_BGR565) || \
												((MODE) == LCDC_OUTPUT_FORMAT_BGR666) )

/** @} */

/** @defgroup LCDC_PRECMD_DEFINE
 * @{
 */
#define LCDC_CMD_DWORD_MAX				4
#define LCDC_MCU_PRECMD_MAX_NUM			16
#define LCDC_CMD_GROUP_INITIAL_VAL		0x00000000		//4Byte in one group

#define LCDC_MASK_MCU_CMD_BYTE			((u8)0x000000FF << 0)
#define LCDC_MCU_CMD_BYTE(x)			((u8)((x) & 0x000000FF) << 0)
/** @} */

/** @defgroup LCDC_SHIFT_OFST_DEFINE
 * @{
 */
#define LCDC_SHIFT_OFST_INVALID			0x0
/** @} */

/** @defgroup LCDC_RW_PARAMETER
 * @{
 */
#define LCDC_WRITE_CYCLE_PERIOD_NS		(60)
#define LCDC_READ_CYCLE_PERIOD_NS		(800)
/** @} */

/** @} */


/* Exported types ------------------------------------------------------------*/
/** @addtogroup LCDC_Exported_Types LCDC Exported Types
 * @{
 */

/**
 * @brief  LCDC Run Mode Definition
 */
enum LCDC_MCU_RUN_MODE {
	LCDC_MCU_RUN_DMA_MODE = 0,
	LCDC_MCU_RUN_IO_MODE,
};

/**
 * @brief  LCDC Shift Direction Definition
 */
enum LCDC_ShiftDir {
	/* Disable shift function */
	LCDC_SHIFT_DISABLE = 0,

	/* Left to right */
	LCDC_SHIFT_DIR_L2R,
	LCDC_SHIFT_DIR_R2L,

	/* Top to bottom */
	LCDC_SHIFT_DIR_T2B,
	LCDC_SHIFT_DIR_B2T,
};

/**
 * @brief  LCDC Shift CfgInfo Definition
 */
typedef struct {
	u32 ImgAddrA;
	u32 ImgAddrB;
	u32 ImgOfstDotX;
	u32 ImgOfstLineY;
	// u32 ShiftFlag;//1-enable, 0-disable
} LCDC_DMAImgAdvanceDef;

/** @} */

/**
 * @brief  LCDC Shift BkupInfo Definition
 */
typedef struct {
	u32	Direction;

	u32 SliceTotal;
	u32 SliceIdx;

	u32 SlotX;
	u32 SlotY;

	u32 ImgW;
	u32 ImgH;

} LCDC_ShiftInfoDef;


/** @} */
/** @} */

/* Exported functions --------------------------------------------------------*/
u32 LCDC_RccEnable(void);
void LCDC_MCUStructInit(LCDC_MCUInitTypeDef *LCDC_MCUInitStruct);
void LCDC_MCUInit(LCDC_TypeDef *LCDCx, LCDC_MCUInitTypeDef *LCDC_MCUInitStruct);
void LCDC_MCUDMATrigger(LCDC_TypeDef *LCDCx);
u32 LCDC_MCUGetRunStatus(LCDC_TypeDef *LCDCx);
void LCDC_MCUIOWriteData(LCDC_TypeDef *LCDCx, u32 Data);
u32 LCDC_MCUIOReadData(LCDC_TypeDef *LCDCx);
void LCDC_MCUIOWriteCmd(LCDC_TypeDef *LCDCx, u32 Cmd);
void LCDC_MCUSetPreCmd(LCDC_TypeDef *LCDCx, const u8 *const Cmd, u8 CmdNum);
void LCDC_MCUResetPreCmd(LCDC_TypeDef *LCDCx);
void LCDC_MCUDmaMode(LCDC_TypeDef *LCDCx, Lcdc_McuDmaCfgDef *DmaCfg);
void LCDC_RGBStructInit(LCDC_RGBInitTypeDef   *LCDC_RGBInitStruct);
void LCDC_RGBInit(LCDC_TypeDef *LCDCx, const LCDC_RGBInitTypeDef *LCDC_RGBInitStruct);
void LCDC_RGBGetSyncStatus(LCDC_TypeDef *LCDCx, u32 *pHSStatus, u32 *pVSStatus);
void LCDC_DMABurstSizeConfig(LCDC_TypeDef *LCDCx, u32 BurstSize);
void LCDC_DMAUnderFlowOutdata(LCDC_TypeDef *LCDCx, u32 DmaUnFlwOutMode, u32 ErrorData);
void LCDC_DMAUnderFlowOpt(LCDC_TypeDef *LCDCx, u32 DmaUnFlwOpt, u32 Threshold);
void LCDC_DMAImageShiftConfig(LCDC_TypeDef *LCDCx, u32 ImgbufCurrent, u32 ImgbufTarget, enum LCDC_ShiftDir Direction);
void LCDC_DMAImgCfg(LCDC_TypeDef *LCDCx, u32 ImgBaseAddrA);
void LCDC_DMAImageOfstConfig(LCDC_TypeDef *LCDCx, u32 HsOfst, u32 VsOfst);
void LCDC_INTConfig(LCDC_TypeDef *LCDCx, u32 LCDC_IT, u32 NewState);
void LCDC_LineINTPosConfig(LCDC_TypeDef *LCDCx, u32 LineNum);
u32 LCDC_GetINTStatus(LCDC_TypeDef *LCDCx);
u32 LCDC_GetRawINTStatus(LCDC_TypeDef *LCDCx);
void LCDC_ClearAllINT(LCDC_TypeDef *LCDCx);
void LCDC_ClearINT(LCDC_TypeDef *LCDCx, u32 LCDC_IT);
void LCDC_GetImgOffset(LCDC_TypeDef *LCDCx, u32 *pImgHs, u32 *pImgVs);
void LCDC_GetImgAddr(LCDC_TypeDef *LCDCx, u32 *pImgA, u32 *pImgB);
void LCDC_GetCurPosStatus(LCDC_TypeDef *LCDCx, u32 *pCurPosX, u32 *pCurPosY);
void LCDC_Cmd(LCDC_TypeDef *LCDCx, u32 NewState);
void LCDC_DeInit(LCDC_TypeDef *LCDCx);
void LCDC_ShadowReloadConfig(LCDC_TypeDef *LCDCx);

extern u32 LCDC_SYS_CLK;// = 400000000;

#define IS_LCDC_ALL_PERIPH(PERIPH)				(PERIPH == LCDC)

// #define LCDC_FPGA_RTLSIM 				0//1 sim
#define LCDC_FRAME_BUF_PSRAM 			1//0 sim

// #define LCDC_VO_SHPERI_USE_USB_PLL		1
// // #define LCDC_ACLK_USE_SYS_PLL			1

/* MANUAL_GEN_END */


#endif