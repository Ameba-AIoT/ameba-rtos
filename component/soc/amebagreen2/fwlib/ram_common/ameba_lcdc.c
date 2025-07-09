/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include "ameba_soc.h"
static const char *const TAG = "LCDC";

u32 LCDC_SYS_CLK = 200000000;//200MHz

u32 LCDC_RccEnable(void)
{
	u32 RegVal;
	u32 LCDC_DMA_CLK;

	if (SYSCFG_CHIPType_Get() == CHIP_TYPE_FPGA) {
		/* System dummy reg: [7:5] = 111, voclk = 80MHz */
		RegVal = HAL_READ32(0x4080A2FC, 0);

		if ((RegVal >> 5 & 0x7) == 0x5) {
			LCDC_SYS_CLK = 80000000;
		} else {
			LCDC_SYS_CLK = 40000000;
		}

		/* fpga: set vo_cksl/shperi_cksl */
		RCC_PeriphClockSourceSet(VO, USB_PLL);
		RCC_PeriphClockSourceSet(SHPERI, USB_PLL);

		/* fpga: enable ckd_fen of vo_clk and shperi_clk */
		RCC_PeriphClockDividerFENSet(USB_PLL_VO, ENABLE);
		RCC_PeriphClockDividerFENSet(USB_PLL_SHPERI, ENABLE);

	} else {
		u8 vo_ckd = RRAM_DEV->clk_info_bk.vo_ckd;
		u8 shperi_ckd = RRAM_DEV->clk_info_bk.shperi_ckd;

		/* enable SYS_PLL_VO/USB_PLL_VO's ckd_fen for vo_clk */
		if (vo_ckd & IS_SYS_PLL) {
			RCC_PeriphClockDividerFENSet(SYS_PLL_VO, ENABLE);
		} else {
			RCC_PeriphClockDividerFENSet(USB_PLL_VO, ENABLE);
		}

		/* enable SYS_PLL_SHPERI/USB_PLL_SHPERI's ckd_fen for shperi_clk */
		if (shperi_ckd & IS_SYS_PLL) {
			RCC_PeriphClockDividerFENSet(SYS_PLL_SHPERI, ENABLE);
		} else {
			RCC_PeriphClockDividerFENSet(USB_PLL_SHPERI, ENABLE);
		}

		/* acquire voclk for calculating dclk divider */
		LCDC_DMA_CLK = SHPERI_ClkGet();
		LCDC_SYS_CLK = VO_ClkGet();
	}

	/* enable cke for shperi, also cke and fen for LCDC */
	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_SHPERI_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_LCDC, APBPeriph_LCDC_CLOCK, ENABLE);

	RTK_LOGI(TAG, "VOCLK: %luHz LCDC_ACLK=%luHz \r\n", LCDC_SYS_CLK, LCDC_DMA_CLK);
	return LCDC_SYS_CLK;
}

/** @addtogroup Ameba_Periph_Driver
 * @{
 */

/** @defgroup LCDC
 * @brief LCDC driver modules
 * @{
 */

/* Exported functions --------------------------------------------------------*/
/** @defgroup LCDC_Exported_Functions LCDC Exported Functions
 * @{
 */

/**
 * @brief  Fill each LCDC_MCUInitStruct member with its default value.
 * @param  LCDC_MCUInitStruct: pointer to an LCDC_MCUInitTypeDef structure which will be initialized.
 * @retval   None
 */
void LCDC_MCUStructInit(LCDC_MCUInitTypeDef *LCDC_MCUInitStruct)
{
	LCDC_MCUInitStruct->Panel_Init.IfWidth = LCDC_MCU_IF_8_BIT;
	LCDC_MCUInitStruct->Panel_Init.ImgWidth = 240;
	LCDC_MCUInitStruct->Panel_Init.ImgHeight = 320;
	LCDC_MCUInitStruct->Panel_Init.InputFormat = LCDC_INPUT_FORMAT_RGB565;
	LCDC_MCUInitStruct->Panel_Init.OutputFormat = LCDC_OUTPUT_FORMAT_RGB565;

	LCDC_MCUInitStruct->Panel_McuTiming.McuRdPolar = LCDC_MCU_RD_PUL_RISING_EDGE_FETCH;
	LCDC_MCUInitStruct->Panel_McuTiming.McuWrPolar = LCDC_MCU_WR_PUL_RISING_EDGE_FETCH;
	LCDC_MCUInitStruct->Panel_McuTiming.McuRsPolar = LCDC_MCU_RS_PUL_LOW_LEV_CMD_ADDR;
	LCDC_MCUInitStruct->Panel_McuTiming.McuTePolar = LCDC_MCU_TE_PUL_LOW_LEV_ACTIVE;
	LCDC_MCUInitStruct->Panel_McuTiming.McuSyncPolar = LCDC_MCU_VSYNC_PUL_LOW_LEV_ACTIVE;
}

static void LCDC_MCUCheckPanelCfg(const LCDC_MCUInitTypeDef *LCDC_MCUInitStruct)
{
	u32 bit_width, out_format;

	bit_width = LCDC_MCUInitStruct->Panel_Init.IfWidth;
	out_format = LCDC_MCUInitStruct->Panel_Init.OutputFormat;

	assert_param(IS_LCDC_MCU_IF_MODE(bit_width));
	assert_param(IS_LCDC_OUTPUT_FORMAT(out_format));
	assert_param(IS_LCDC_INPUT_FORMAT(LCDC_MCUInitStruct->Panel_Init.InputFormat));

	switch (bit_width) {
	case LCDC_MCU_IF_8_BIT:
		assert_param((out_format == LCDC_OUTPUT_FORMAT_RGB565) || \
					 (out_format == LCDC_OUTPUT_FORMAT_BGR565) || \
					 (out_format == LCDC_OUTPUT_FORMAT_RGB888) || \
					 (out_format == LCDC_OUTPUT_FORMAT_BGR888));
		break;

	case LCDC_MCU_IF_16_BIT:
		assert_param((out_format == LCDC_OUTPUT_FORMAT_RGB565) || \
					 (out_format == LCDC_OUTPUT_FORMAT_BGR565));
		break;

	case LCDC_MCU_IF_9_BIT:
		assert_param((out_format == LCDC_OUTPUT_FORMAT_RGB666));
		break;

	case LCDC_MCU_IF_18_BIT:
		assert_param((out_format == LCDC_OUTPUT_FORMAT_RGB666));
		break;

	case LCDC_MCU_IF_24_BIT:
		assert_param((out_format == LCDC_OUTPUT_FORMAT_RGB888) || \
					 (out_format == LCDC_OUTPUT_FORMAT_BGR888));
		break;

	default:
		RTK_LOGE(TAG, "MCU IF: bit-width not match output color format !\r\n");
		assert_param(0);
	}
}

/**
 * @brief  Initialize the LCDC to work in MCU interface's IO mode.
 * @param  LCDCx: Where LCDCx can be LCDC.
 * @param  LCDC_MCUInitStruct: Pointer to a LCDC_MCUInitTypeDef structure that contains
 *                             the configuration information for the specified LCDC peripheral.
 * @retval None
 * @note None
 */
void LCDC_MCUInit(LCDC_TypeDef *LCDCx, LCDC_MCUInitTypeDef *LCDC_MCUInitStruct)
{
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	/* read registers for configurartion */
	u32 McuCfgReg = LCDCx->LCDC_MCU_CFG;
	u32 PlaneSize = LCDCx->LCDC_PLANE_SIZE;
	u32 McuTimingCfg = LCDCx->LCDC_MCU_TIMING_CFG;
	u32 McuInputColorFmtCfg = LCDCx->LCDC_IMG_CFG;
	u32 McuOutputColorFmtCfg = LCDCx->LCDC_COLOR_CFG;
	u32 LcdcCtrl = LCDCx->LCDC_CTRL;

	Panel_InitDef Panel = LCDC_MCUInitStruct->Panel_Init;
	Panel_McuTimingDef Timing = LCDC_MCUInitStruct->Panel_McuTiming;

	/* check bit-width and color_format */
	LCDC_MCUCheckPanelCfg(LCDC_MCUInitStruct);

	/* disable lcdc before all operations */
	LCDCx->LCDC_CTRL |= LCDC_BIT_INST_DIS;

	/* configure mcu io/dma mode to io mode */
	McuCfgReg |= LCDC_BIT_MCU_IO_MODE_EN;

	/* configure mcu sync mode for io mode */
	McuCfgReg &= ~LCDC_MASK_MCU_SYNC_MODE;
	McuCfgReg |= LCDC_MCU_SYNC_MODE(LCDC_MCU_SYNC_WITH_INTERNAL_CLK);

	/* configure rs, write, read polarity, keep cs and data default polarty */
	McuCfgReg &= ~(LCDC_BIT_RSPL | LCDC_BIT_WRPL | LCDC_BIT_RDPL | LCDC_BIT_TEPL | LCDC_MCU_VSYNC_PUL_HIGH_LEV_ACTIVE);
	McuCfgReg |= ((Timing.McuRsPolar == LCDC_MCU_RS_PUL_HIGH_LEV_CMD_ADDR ? LCDC_BIT_RSPL : 0) | \
				  (Timing.McuWrPolar == LCDC_MCU_WR_PUL_FALLING_EDGE_FETCH ? LCDC_BIT_WRPL : 0) | \
				  (Timing.McuRdPolar == LCDC_MCU_RD_PUL_FALLING_EDGE_FETCH ? LCDC_BIT_RDPL : 0) | \
				  (Timing.McuTePolar == LCDC_MCU_TE_PUL_HIGH_LEV_ACTIVE ? LCDC_BIT_TEPL : 0) | \
				  (Timing.McuSyncPolar == LCDC_MCU_VSYNC_PUL_HIGH_LEV_ACTIVE ? LCDC_BIT_MCUSYPL : 0));

	/* configure MCU I/F bit mode */
	LcdcCtrl &= ~LCDC_MASK_IF_MODE;
	LcdcCtrl |= LCDC_IF_MODE(Panel.IfWidth);

	/* configure input color format */
	McuInputColorFmtCfg &= ~LCDC_MASK_IMG_FORMAT;
	McuInputColorFmtCfg |= LCDC_IMG_FORMAT(Panel.InputFormat);

	/* configure output color format */
	McuOutputColorFmtCfg &= ~LCDC_MASK_COLOR_FORMAT;
	McuOutputColorFmtCfg |= LCDC_COLOR_FORMAT(Panel.OutputFormat);

	/* configure write/read pulse width */
	McuTimingCfg &= ~(LCDC_MASK_WRPULW | LCDC_MASK_RDACTW | LCDC_MASK_RDINACTW);

	/* write_act：10, read_inact:8, read_act: 8 */
	u32 wrdiv = LCDC_SYS_CLK / 1000000 * LCDC_WRITE_CYCLE_PERIOD_NS / 1000 / 2 - 1;
	u32 rddiv = LCDC_SYS_CLK / 1000000 * LCDC_READ_CYCLE_PERIOD_NS / 1000 / 2;
	// McuTimingCfg |= LCDC_WRPULW(8) | (LCDC_RDACTW(7) | LCDC_RDINACTW(7));
	McuTimingCfg |= LCDC_WRPULW(wrdiv) | (LCDC_RDACTW(rddiv) | LCDC_RDINACTW(rddiv));
	// RTK_LOGI(NOTAG, "@@@@ wrdiv=%u, rddiv=%u\r\n", wrdiv, rddiv);

	/* configure the plane size */
	PlaneSize &= ~(LCDC_MASK_IMAGEHEIGHT | LCDC_MASK_IMAGEWIDTH);
	/* plane size is needed in dma mode */
	PlaneSize |= (LCDC_IMAGEWIDTH(Panel.ImgWidth) | LCDC_IMAGEHEIGHT(Panel.ImgHeight));

	/* configure IO write/read timeout clock number according to write/read clock */
	LCDCx->LCDC_MCU_IO_TO_CFG &= ~LCDC_MASK_IO_TIMEOUT_NUM;
	LCDCx->LCDC_MCU_IO_TO_CFG |= LCDC_IO_TIMEOUT_NUM(176);//(WrPulWidth * 16 + RdAcvWidth + RdInacvWidth) = 16*10+8+8;

	/* write the value configured back to registers */
	LCDCx->LCDC_MCU_CFG = McuCfgReg;
	LCDCx->LCDC_PLANE_SIZE = PlaneSize;
	LCDCx->LCDC_MCU_TIMING_CFG = McuTimingCfg;
	LCDCx->LCDC_IMG_CFG = McuInputColorFmtCfg;
	LCDCx->LCDC_COLOR_CFG = McuOutputColorFmtCfg;
	LCDCx->LCDC_CTRL = LcdcCtrl;
}

/**
 * @brief  Configure the LCDC to work in MCU interface's DMA mode.
 * @param  LCDCx: Where LCDCx can be LCDC.
 * @param  DmaCfg: Pointer to a Lcdc_McuDmaCfgDef structure that contains
 *                             the configuration information for dma mode.
 * @retval None
 * @note Before this opertion, ensure panel and timing info has been initialzed by LCDC_MCUInit().
 */
void LCDC_MCUDmaMode(LCDC_TypeDef *LCDCx, Lcdc_McuDmaCfgDef *DmaCfg)
{
	u32 McuSyncMode;

	/* read registers for configurartion */
	u32 McuCfgReg = LCDCx->LCDC_MCU_CFG;
	u32 DmaModeCfg = LCDCx->LCDC_DMA_MODE_CFG;

	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	/* configure io/dma mode to dma mode */
	McuCfgReg &= ~LCDC_BIT_MCU_IO_MODE_EN;

	/* clear te mode to avoid hang when config with none-te sync mode */
	DmaModeCfg &= ~LCDC_BIT_DMA_TE_MODE;

	if (DmaCfg->TeMode == ENABLE) {
		assert_param(DmaCfg->TeDelay >= 5);
		/* te mode */
		McuSyncMode = LCDC_MCU_SYNC_WITH_TE;

		/* TE delay interval configuration*/
		McuCfgReg &= ~LCDC_MASK_TEDELAY;
		McuCfgReg |= LCDC_TEDELAY(DmaCfg->TeDelay - 5);

		/* enable DMA TE Mode mode */
		DmaModeCfg |= LCDC_BIT_DMA_TE_MODE;

	} else {
		/* vsync mode for both trigger and auto dma mode */
		McuSyncMode = LCDC_MCU_SYNC_WITH_VSYNC;

		if (DmaCfg->TriggerDma == ENABLE) {
			/* trigger dma mode */
			DmaModeCfg |= LCDC_BIT_DMA_TRIGER_MODE;
		} else {
			/* auto dma mode */
			DmaModeCfg &= ~LCDC_BIT_DMA_TRIGER_MODE;
		}
	}

	/* config sync mode */
	McuCfgReg &= ~LCDC_MASK_MCU_SYNC_MODE;
	McuCfgReg |= LCDC_MCU_SYNC_MODE(McuSyncMode);

	/* write the value configured back to registers */
	LCDCx->LCDC_MCU_CFG = McuCfgReg;
	LCDCx->LCDC_DMA_MODE_CFG = DmaModeCfg;
}

/**
 * @brief  Configure the DMA image address where DMA will fetch pix data.
 * @param  LCDCx: where LCDCx can be LCDC.
 * @param  ImgBaseAddr: frame buffer address.
 * @retval None
 */
void LCDC_DMAImgCfg(LCDC_TypeDef *LCDCx, u32 ImgBaseAddrA)
{
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	/* config dma buffer addr LCDC_IMG_A_BASE_ADDR */
	LCDCx->LCDC_IMG_A_BASE_ADDR = LCDC_IMG_A_BASE_ADDR(ImgBaseAddrA);
}

/**
 * @brief  Configure DMA two image address and offset for shift function.
 * @param  LCDCx: where LCDCx can be LCDC.
 * @param  DmaImgInfo: pointer LCDC_DMAImgAdvanceDef struct, contain imageA/imageB address, ofst_dotX/ofst_lineY.
 * @note For imagaA address: where DMA skips pixels based on ofst_dotX/ofst_lineY and then start fetches pixel data
	 For imageB address: where DMA starts fetching from the initial position, with the fetch length of ofst_dotX/ofst_lineY.
		The ofst_dotX/ofst_lineY cannot be non-zero simultaneously.
		The ofst_dotX need to be a multiple of 4.
* @retval None
*/
void LCDC_DMAImgCfgAdvance(LCDC_TypeDef *LCDCx, const LCDC_DMAImgAdvanceDef *DmaImgInfo)
{
	u32 HsOfst = DmaImgInfo->ImgOfstDotX;
	u32 VsOfst = DmaImgInfo->ImgOfstLineY;

	/* Ensure HsOfst is a multiple of 4 */
	assert_param(!(HsOfst % 4));
	/* The ofst_dotX/ofst_lineY cannot be non-zero simultaneously. */
	assert_param(!(HsOfst & VsOfst));

	LCDCx->LCDC_IMG_A_BASE_ADDR = LCDC_IMG_A_BASE_ADDR(DmaImgInfo->ImgAddrA);
	LCDCx->LCDC_IMG_B_BASE_ADDR = LCDC_IMG_B_BASE_ADDR(DmaImgInfo->ImgAddrB);

	/* config image dot/line offset for horizontal/vertical slide */
	LCDCx->LCDC_IMG_SLIDE &= ~(LCDC_MASK_IMG_VS_OFST | LCDC_MASK_IMG_HS_OFST) ;
	LCDCx->LCDC_IMG_SLIDE |= (LCDC_IMG_VS_OFST(VsOfst) | LCDC_IMG_HS_OFST(HsOfst));
}

/**
 * @brief  Trigger DMA frame one-time for dma work in trigger mode.
 * @param  LCDCx: where LCDCx can be LCDC.
 * @note None.
 * @retval None
 */
void LCDC_MCUDMATrigger(LCDC_TypeDef *LCDCx)
{
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	/* write 1 to TRIGER_ONETIME field in register LCDC_DMA_MODE_CFG */
	LCDCx->LCDC_DMA_MODE_CFG |= LCDC_BIT_TRIGER_ONETIME;
}

/**
 * @brief     Write data to MCU I/F bus.
 * @param  LCDCx: where LCDCx can be LCDC.
 * @param  Data: the data to transmit.
 * @retval   None
 */
void LCDC_MCUIOWriteData(LCDC_TypeDef *LCDCx, u32 Data)
{
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	/*write data to tx fifo, Bit31 indicates whether the current data in TX FIFO is regarded as a command or data. 0-Data, 1-Command */
	LCDCx->LCDC_MCU_IO_DATA = LCDC_MCU_RW_DATA(Data);
}

/**
 * @brief     Read data from MCU I/F bus.
 * @param  LCDCx: where LCDCx can be LCDC.
 * @retval   the read value
 */
u32 LCDC_MCUIOReadData(LCDC_TypeDef *LCDCx)
{
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	/*read data from bus*/
	return (u32)(LCDCx->LCDC_MCU_IO_DATA & LCDC_MASK_MCU_RW_DATA);
}

/**
 * @brief     Write command to MCU I/F bus.
 * @param  LCDCx: where LCDCx can be LCDC.
 * @param  Cmd: the command to transmit.
 * @retval   None
 */
void LCDC_MCUIOWriteCmd(LCDC_TypeDef *LCDCx, u32 Cmd)
{
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	/* write command to tx fifo, Bit31 indicates whether the current data in TX FIFO is regarded as a command or data. 0-Data, 1-Command*/
	LCDCx->LCDC_MCU_IO_DATA = LCDC_MCU_RW_DATA(Cmd) | BIT31;
}

/**
 * @brief     Get the MCU I/F IO mode run status.
 * @param  LCDCx: where LCDCx can be LCDC.
 * @retval   status value:
 *              - LCDC_MCU_RUN_IO_MODE(1): IO mode run
 *              - LCDC_MCU_RUN_DMA_MODE(0): DMA mode run
 */
u32 LCDC_MCUGetRunStatus(LCDC_TypeDef *LCDCx)
{
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	/*aquire the IO mode run status*/
	if (LCDCx->LCDC_MCU_CFG & LCDC_BIT_MCU_IO_MODE_RUN) {
		return LCDC_MCU_RUN_IO_MODE;
	} else {
		return LCDC_MCU_RUN_DMA_MODE;
	}
}


/**
 * @brief  Set pre-defined command to be sent before DMA frame data.
 * @param  LCDCx: Where LCDCx can be LCDC.
 * @param  Cmd: The command byte to be sent.
 * @param  CmdNum: The total number of the command to be sent,
 *                    must be in the range [0, 15].
 * @note   Commands are sent in the following order:
 * @note   Only standalone commands without additional parameters are supported.
 * @retval None
 */
void LCDC_MCUSetPreCmd(LCDC_TypeDef *LCDCx, const u8 *const Cmd, u8 CmdNum)
{
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));
	assert_param(CmdNum <= 16);

	u32 Ofst;
	volatile u8 *pCmdAddr = NULL;

	/* commands of 8bit without parameter are supported */
	u8 *pCmdBaseAddr = (u8 *)(&(LCDCx->LCDC_MCU_CMD_DWORD0_0));

	for (Ofst = 0; Ofst < CmdNum; Ofst++) {
		/* find the address that specific command will be written into */
		pCmdAddr = pCmdBaseAddr + Ofst;

		/* set the command to the target register */
		*pCmdAddr = LCDC_MCU_CMD_BYTE(Cmd[Ofst]);
	}

	/* set cmd number to send before dma */
	LCDCx->LCDC_MCU_CMD_WRNUM &= ~LCDC_MASK_MCU_CMD_WRNUM;
	LCDCx->LCDC_MCU_CMD_WRNUM |= LCDC_MCU_CMD_WRNUM(CmdNum);
}


/**
 * @brief  Reset total preset commands and command number.
 * @param  LCDCx: where LCDCx can be LCDC.
 * @retval   None
 */
void LCDC_MCUResetPreCmd(LCDC_TypeDef *LCDCx)
{
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	u32 *pCmdGroup = (u32 *)(&(LCDCx->LCDC_MCU_CMD_DWORD0_0));

	for (u8 i = 0; i < LCDC_CMD_DWORD_MAX; i++) {
		pCmdGroup[i] = LCDC_CMD_GROUP_INITIAL_VAL;
	}

	LCDCx->LCDC_MCU_CMD_WRNUM &= ~LCDC_MASK_MCU_CMD_WRNUM;
	// LCDCx->LCDC_MCU_CMD_WRNUM |= LCDC_MCU_CMD_WRNUM(0x0);
}

static void LCDC_RGBCheckPanelCfg(const LCDC_RGBInitTypeDef *LCDC_RGBInitStruct)
{
	u32 bit_width, out_format;
	bit_width = LCDC_RGBInitStruct->Panel_Init.IfWidth;
	out_format = LCDC_RGBInitStruct->Panel_Init.OutputFormat;

	assert_param(IS_LCDC_RGB_IF_MODE(bit_width));
	assert_param(IS_LCDC_OUTPUT_FORMAT(out_format));
	assert_param(IS_LCDC_INPUT_FORMAT(LCDC_RGBInitStruct->Panel_Init.InputFormat));

	switch (bit_width) {
	case LCDC_RGB_IF_6_BIT:
		assert_param((out_format == LCDC_OUTPUT_FORMAT_RGB565) || \
					 (out_format == LCDC_OUTPUT_FORMAT_BGR565) || \
					 (out_format == LCDC_OUTPUT_FORMAT_RGB666));
		break;

	case LCDC_RGB_IF_8_BIT:
		assert_param((out_format == LCDC_OUTPUT_FORMAT_RGB888) || \
					 (out_format == LCDC_OUTPUT_FORMAT_BGR888));
		break;

	case LCDC_RGB_IF_16_BIT:
		assert_param((out_format == LCDC_OUTPUT_FORMAT_RGB565) || \
					 (out_format == LCDC_OUTPUT_FORMAT_BGR565));
		break;

	case LCDC_RGB_IF_18_BIT:
		assert_param((out_format == LCDC_OUTPUT_FORMAT_RGB666));
		break;

	case LCDC_RGB_IF_24_BIT:
		assert_param((out_format == LCDC_OUTPUT_FORMAT_RGB888) || \
					 (out_format == LCDC_OUTPUT_FORMAT_BGR888));
		break;

	default:
		RTK_LOGE(TAG, "RGB IF: bit-width not match output color format !!! \r\n");
		assert_param(0);
	}

}

/**
 * @brief  Fill each LCDC_RGBInitStruct member with its default value.
 * @param  LCDC_RGBInitStruct: Pointer to an LCDC_RGBInitTypeDef structure which will be initialized.
 * @retval None
 */
void LCDC_RGBStructInit(LCDC_RGBInitTypeDef *LCDC_RGBInitStruct)
{
	/* configure panel parameters */
	LCDC_RGBInitStruct->Panel_Init.IfWidth = LCDC_RGB_IF_24_BIT;
	LCDC_RGBInitStruct->Panel_Init.ImgHeight = 480;
	LCDC_RGBInitStruct->Panel_Init.ImgWidth = 800;
	LCDC_RGBInitStruct->Panel_Init.InputFormat = LCDC_INPUT_FORMAT_RGB888;
	LCDC_RGBInitStruct->Panel_Init.OutputFormat = LCDC_OUTPUT_FORMAT_RGB888;
	LCDC_RGBInitStruct->Panel_Init.RGBRefreshFreq = 35;

	/* configure hsync/vsync parameters */
	LCDC_RGBInitStruct->Panel_RgbTiming.RgbVsw = 1;
	LCDC_RGBInitStruct->Panel_RgbTiming.RgbVbp = 4;
	LCDC_RGBInitStruct->Panel_RgbTiming.RgbVfp = 6;

	LCDC_RGBInitStruct->Panel_RgbTiming.RgbHsw = 4;
	LCDC_RGBInitStruct->Panel_RgbTiming.RgbHbp = 40;
	LCDC_RGBInitStruct->Panel_RgbTiming.RgbHfp = 40;

	/* configure signal polarity */
	LCDC_RGBInitStruct->Panel_RgbTiming.Flags.RgbEnPolar = LCDC_RGB_EN_PUL_HIGH_LEV_ACTIVE;
	LCDC_RGBInitStruct->Panel_RgbTiming.Flags.RgbHsPolar = LCDC_RGB_HS_PUL_LOW_LEV_SYNC;
	LCDC_RGBInitStruct->Panel_RgbTiming.Flags.RgbVsPolar = LCDC_RGB_VS_PUL_LOW_LEV_SYNC;
	LCDC_RGBInitStruct->Panel_RgbTiming.Flags.RgbDclkActvEdge = LCDC_RGB_DCLK_FALLING_EDGE_FETCH;
}

/**
 * @brief  Initialize the LCDC peripheral according to the specified
 *         parameters in the LCDC_RGBInitStruct.
 * @param  LCDCx: Where LCDCx can be LCDC.
 * @param  LCDC_RGBInitStruct: Pointer to a LCDC_RGBInitTypeDef structure that contains
 *                             the configuration information for the specified LCDC peripheral.
 * @retval None
 */
void LCDC_RGBInit(LCDC_TypeDef *LCDCx, const LCDC_RGBInitTypeDef *LCDC_RGBInitStruct)
{
	/* check the parameters */
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	/* variables for reading register */
	u32 RgbCfg;
	u32 RgbVsyncCfg;
	u32 RgbHsyncCfg;
	u32 DmaModeCfg;
	u32 PlaneSize;
	u32 RgbInputColorFmtCfg;
	u32 RgbOutputColorFmtCfg;
	u32 LcdcCtrl;

	/* variables for calculating dclk divider */
	u32 LcdcCkd;
	u32 TargetDclk;
	u32 HBlank, HTotal;
	u32 VTotal;

	/* variables for acquire pannel parameters */
	Panel_InitDef Panel = LCDC_RGBInitStruct->Panel_Init;
	Panel_RgbTimingDef Timing = LCDC_RGBInitStruct->Panel_RgbTiming;

	/* get registers for configurartion */
	RgbCfg = LCDCx->LCDC_RGB_CFG;
	RgbVsyncCfg = LCDCx->LCDC_RGB_VSYNC_CFG;
	RgbHsyncCfg = LCDCx->LCDC_RGB_HSYNC_CFG;
	DmaModeCfg = LCDCx->LCDC_DMA_MODE_CFG;
	PlaneSize = LCDCx->LCDC_PLANE_SIZE;
	RgbInputColorFmtCfg = LCDCx->LCDC_IMG_CFG;
	RgbOutputColorFmtCfg = LCDCx->LCDC_COLOR_CFG;
	LcdcCtrl = LCDCx->LCDC_CTRL;

	/* check pannel bus width and color format */
	LCDC_RGBCheckPanelCfg(LCDC_RGBInitStruct);

	/* disable lcdc befrore all operations */
	LCDCx->LCDC_CTRL |= LCDC_BIT_INST_DIS;//before all:LCDC_RccEnable()

	/* configure rgb sync mode */
	RgbCfg &= ~LCDC_MASK_RGB_SYNC_MODE;
	RgbCfg |= LCDC_RGB_SYNC_MODE(LCDC_RGB_HV_MODE);

	/* configure RGB I/F bit-width */
	LcdcCtrl &= ~LCDC_MASK_IF_MODE;
	LcdcCtrl |= LCDC_IF_MODE(Panel.IfWidth);

	/* configure plane size */
	PlaneSize &= ~(LCDC_MASK_IMAGEHEIGHT | LCDC_MASK_IMAGEWIDTH);
	PlaneSize |= (LCDC_IMAGEWIDTH(Panel.ImgWidth) | LCDC_IMAGEHEIGHT(Panel.ImgHeight));

	/* configure vsync/hsync/de pulse polarity and dclk active edge */
	RgbCfg &= ~(LCDC_BIT_VSPL | LCDC_BIT_HSPL | LCDC_BIT_ENPL | LCDC_BIT_DCLKPL);
	RgbCfg |= ((Timing.Flags.RgbEnPolar == LCDC_RGB_EN_PUL_HIGH_LEV_ACTIVE ? LCDC_BIT_ENPL : 0) | \
			   (Timing.Flags.RgbHsPolar == LCDC_RGB_HS_PUL_HIGH_LEV_SYNC ? LCDC_BIT_HSPL : 0) | \
			   (Timing.Flags.RgbVsPolar == LCDC_RGB_VS_PUL_HIGH_LEV_SYNC ? LCDC_BIT_VSPL : 0) | \
			   (Timing.Flags.RgbDclkActvEdge == LCDC_RGB_DCLK_FALLING_EDGE_FETCH ? LCDC_BIT_DCLKPL : 0));

	/* configure vsync parameters: vsw/vbp/vfp:[0x1, 0x10] */
	RgbVsyncCfg &= ~(LCDC_MASK_VSW | LCDC_MASK_VBP | LCDC_MASK_VFP);
	RgbVsyncCfg |= (LCDC_VSW(Timing.RgbVsw - 1) | LCDC_VBP(Timing.RgbVbp - 1) | LCDC_VFP(Timing.RgbVfp - 1));

	/* configure hsync parameters: hsw/hbp/hfp:[0x1, 0x100] */
	RgbHsyncCfg &= ~(LCDC_MASK_HSW | LCDC_MASK_HBP | LCDC_MASK_HFP);
	RgbHsyncCfg |= (LCDC_HSW(Timing.RgbHsw - 1) | LCDC_HBP(Timing.RgbHbp - 1) | LCDC_HFP(Timing.RgbHfp - 1));

	/* configure DMA as auto mode*/
	DmaModeCfg &= ~LCDC_BIT_DMA_TRIGER_MODE;

	/* calculate the DCLK clock divider according to the plane size, refreh frequency and LCDC_SYS_CLK */
	HBlank = Timing.RgbHsw + Timing.RgbHbp + Timing.RgbHfp;
	HTotal = HBlank;
	VTotal = Panel.ImgHeight + Timing.RgbVsw + Timing.RgbVbp + Timing.RgbVfp;

	switch (Panel.IfWidth) {
	case LCDC_RGB_IF_6_BIT:		/* RGB565 */
	case LCDC_RGB_IF_8_BIT:		/* RGB888 */
		HTotal += (3 * Panel.ImgWidth);
		break;

	case LCDC_RGB_IF_16_BIT:	/* RGB565 */
	case LCDC_RGB_IF_18_BIT:	/* RGB666 */
	case LCDC_RGB_IF_24_BIT:	/* RGB888 */
		HTotal += Panel.ImgWidth;
		break;

	default:
		RTK_LOGE(TAG, "Not supported RGB IF-width !!!\r\n");
		assert_param(0);
	}

	/* calculate the required dclk */
	TargetDclk = Panel.RGBRefreshFreq * HTotal * VTotal;

	/* calculate initial clock division and ensure div is an even value */
	LcdcCkd = (LCDC_SYS_CLK / TargetDclk / 2) * 2;
	LcdcCkd = MAX(LcdcCkd, 4);/* for amebad issue, at least div4 */

	LCDCx->LCDC_RGB_CLK_DIV &= ~LCDC_MASK_CLKDIV;
	LCDCx->LCDC_RGB_CLK_DIV |= LCDC_CLKDIV((LcdcCkd / 2) - 1);/* regval -> actval: 0/1/2/3 -> 2/4/6/8 */
	RTK_LOGI(TAG, "SysClk:%luHz, Divider%lu CalcDclk%luHz(ActiveDclk%luHz) \r\n", LCDC_SYS_CLK, LcdcCkd, TargetDclk, (LCDC_SYS_CLK / LcdcCkd));

	/* configure input color format */
	RgbInputColorFmtCfg &= ~LCDC_MASK_IMG_FORMAT;
	RgbInputColorFmtCfg |= LCDC_IMG_FORMAT(Panel.InputFormat);

	/* configure output color format */
	RgbOutputColorFmtCfg &= ~LCDC_MASK_COLOR_FORMAT;
	RgbOutputColorFmtCfg |= LCDC_COLOR_FORMAT(Panel.OutputFormat);

	/* write the value configured back to registers */
	LCDCx->LCDC_RGB_CFG = RgbCfg;
	LCDCx->LCDC_RGB_VSYNC_CFG = RgbVsyncCfg;
	LCDCx->LCDC_RGB_HSYNC_CFG = RgbHsyncCfg;
	LCDCx->LCDC_DMA_MODE_CFG = DmaModeCfg;
	LCDCx->LCDC_PLANE_SIZE = PlaneSize;
	LCDCx->LCDC_IMG_CFG = RgbInputColorFmtCfg;
	LCDCx->LCDC_COLOR_CFG = RgbOutputColorFmtCfg;
	LCDCx->LCDC_CTRL = LcdcCtrl;
}

/**
 * @brief  Get the RGB synchronization status.
 * @param  LCDCx: where LCDCx can be LCDC.
 * @param  pHSStatus: horizontal synchronization status pointer.
 *		    HSStatus can be one of the following values:
*     	    @arg LCDC_RGB_SYNC_STATUS_HSYNC:  HSYNC
*             @arg LCDC_RGB_SYNC_STATUS_HFP  :  HFP
*     	    @arg LCDC_RGB_SYNC_STATUS_HACTIVE:  ACTIVE
*             @arg LCDC_RGB_SYNC_STATUS_HBP    :  HBP
* @param  pVSStatus: vertical synchronization status pointer.
*		    VSStatus can be one of the following values:
*     	    @arg LCDC_RGB_SYNC_STATUS_VSYNC:  VSYNC
*             @arg LCDC_RGB_SYNC_STATUS_VFP    :  VFP
*     	    @arg LCDC_RGB_SYNC_STATUS_VACTIVE:  ACTIVE
*             @arg LCDC_RGB_SYNC_STATUS_VBP    :  VBP
* @retval None
*/
void LCDC_RGBGetSyncStatus(LCDC_TypeDef *LCDCx, u32 *pHSStatus, u32 *pVSStatus)
{
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	/* Get horizontal synchronization status */
	*pHSStatus = LCDC_GET_HSSTATUS(LCDCx->LCDC_RGB_SYNC_STATUS);

	/* Get vertical synchronization status */
	*pVSStatus = LCDC_GET_VSSTATUS(LCDCx->LCDC_RGB_SYNC_STATUS);
}

/**
 * @brief  Configure LCDC DMA burst size.
 * @param  LCDCx: where LCDCx can be LCDC.
 * @param  BurstSize: DMA burst size; Unit 64 Bytes.
 *         	@arg LCDC_DMA_BURSTSIZE_1X64BYTES:  Burst Trasnstions = 1;
 *           @arg LCDC_DMA_BURSTSIZE_2X64BYTES:  Burst Trasnstions = 2;
 *           @arg LCDC_DMA_BURSTSIZE_4X64BYTES:  Burst Trasnstions = 4;
 * @note   If the BurstSize=1, the actual burstsize = 1x64 Bytes;
		 If the BurstSize=2, the actual burstsize = 2x64 = 128 Bytes; etc.
		The parameter "BurstSize" is not more than 3.
* @retval None
*/
void LCDC_DMABurstSizeConfig(LCDC_TypeDef *LCDCx, u32 BurstSize)
{
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));
	assert_param(IS_LCDC_DMA_BURSTSIZE(BurstSize));

	LCDCx->LCDC_SHW_RLD_CFG &= (~LCDC_MASK_DMA_BURST_OPT);
	LCDCx->LCDC_SHW_RLD_CFG |= LCDC_DMA_BURST_OPT(BurstSize);
}

/**
 * @brief  Configure LCDC DMA output last data or error data when dma underflow event occurs.
 * @param  LCDCx: Where LCDCx can be LCDC.
 * @param  DmaUnFlwOutMode: DMA underflow output mode, this parameter
 *                       can be one of the following values:
 *     	                @arg LCDC_DMA_UNDFLW_OUTPUT_LASTDATA: output last data
 *                       @arg LCDC_DMA_UNDFLW_OUTPUT_ERRORDATA: output error data
 * @param  ErrorData: The output data when  DMA FIFO underflow occurred. When underflow mode is configured as
 *                    LCDC_DMA_UNDFLW_OUTPUT_ERRORDATA, this parameter is needed, and otherwise it can be ignored.
 * @retval   None
 */
void LCDC_DMAUnderFlowOutdata(LCDC_TypeDef *LCDCx, u32 DmaUnFlwOutMode, u32 ErrorData)
{
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));
	assert_param(IS_LCDC_DMA_UNDFLOW_MODE(DmaUnFlwOutMode));
	assert_param(ErrorData <= 0xFFFFFF);

	/* clear the DMA UNDFLW field */
	LCDCx->LCDC_UNDFLW_CFG &= ~LCDC_BIT_DMA_UN_MODE;

	if (DmaUnFlwOutMode == LCDC_DMA_UNDFLW_OUTPUT_ERRORDATA) {
		/* set the DMA UNDFLW  field */
		LCDCx->LCDC_UNDFLW_CFG |= LCDC_BIT_DMA_UN_MODE;

		/* fill the ERROUTDATA field */
		LCDCx->LCDC_UNDFLW_CFG &= (~LCDC_MASK_ERROUT_DATA);
		LCDCx->LCDC_UNDFLW_CFG |= LCDC_ERROUT_DATA(ErrorData);
	}
}

/**
 * @brief  Configure LCDC DMA options when dma underflow event occurs.
 * @param  LCDCx: Where LCDCx can be LCDC.
 * @param  DmaUnFlwOpt: DMA underflow option, this parameter can be one of the following values:
 *     	                @arg LCDC_DMA_UNDFLOW_PAUSE_ERRDATA_CURFRAME: Pause clock and data; after timeout, output error data for the remaining frame..
 *     	                @arg LCDC_DMA_UNDFLOW_PAUSE_DROP_LINE: Pause clock and data; after timeout, drop the current line and continue transmitting the next lines.
 *     	                @arg LCDC_DMA_UNDFLOW_INSTANT_ERRDATA_CURFRAME: Do not pause clock and data; instantly output error data for the remaining frame.
 *     	                @arg LCDC_DMA_UNDFLOW_INSTANT_DROP_LINE:  Do not pause clock and data; instantly drop the current line and continue transmitting the next lines.
 *                            The output data can be last_data or err_data, which depends on  the LCDC_BIT_DMA_UN_MODE config.
 * @param  Threshold: The timeout threshold value when dma underflow occurred.
 *                    When DmaUnFlwOpt is configured as LCDC_DMA_UNDFLOW_PAUSE_ERRDATA_CURFRAME or LCDC_DMA_UNDFLOW_PAUSE_DROP_LINE, this parameter is needed, otherwise it can be ignored.
 * @retval   None
 */
void LCDC_DMAUnderFlowOpt(LCDC_TypeDef *LCDCx, u32 DmaUnFlwOpt, u32 Threshold)
{
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));
	assert_param(IS_LCDC_DMA_UNDFLOW_OPTION(DmaUnFlwOpt));
	assert_param(Threshold <= 0xFFF);

	/* clear dma underflow option field */
	LCDCx->LCDC_UNDFLW_CFG &= (~LCDC_MASK_DMA_UDF_OPT);

	/* when dma underflow event occurs, select output Err_data mode or Drop_line mode after timeout */
	if ((DmaUnFlwOpt == LCDC_DMA_UNDFLOW_PAUSE_ERRDATA_CURFRAME) || (DmaUnFlwOpt == LCDC_DMA_UNDFLOW_PAUSE_DROP_LINE)) {
		/* Set dma option when underflow event occurs */
		LCDCx->LCDC_UNDFLW_CFG |= LCDC_DMA_UDF_OPT(DmaUnFlwOpt);

		/* Config threshold value for dma underflow timeout counter */
		LCDCx->LCDC_DMA_MODE_CFG &= (~LCDC_MASK_DMA_UDF_TIMEOUT);
		LCDCx->LCDC_DMA_MODE_CFG |= LCDC_DMA_UDF_TIMEOUT(Threshold);

		/* when dma underflow event occurs, select output Err_data mode or Drop_line mode instantly */
	} else if ((DmaUnFlwOpt == LCDC_DMA_UNDFLOW_INSTANT_ERRDATA_CURFRAME) || (DmaUnFlwOpt == LCDC_DMA_UNDFLOW_INSTANT_DROP_LINE)) {
		/* Set dma option when underflow event occurs */
		LCDCx->LCDC_UNDFLW_CFG |= LCDC_DMA_UDF_OPT(DmaUnFlwOpt);
	}
}

/**
 * @brief  Configure DMA image buffer address and offset.
 * @param  LCDCx: where LCDCx can be LCDC.
 * @param  ImgbufCurrent: the current image base address-A. This parameter is always required.
 *                        When the shift function is disabled, set Direction to LCDC_SHIFT_DISABLE.
 * @param  ImgbufTarget: the target image base address.
 *                        If disable shift function, this parameter is ignored.
 *                        If enable shift function, both ImgbufCurrent and ImgbufTarget must be set and Direction must not be LCDC_SHIFT_DISABLE.
 * @param  Direction: the shift direction. this parameter can be one of the following values:
 *     	                @arg LCDC_SHIFT_DISABLE
 *     	                @arg LCDC_SHIFT_DIR_L2R
 *     	                @arg LCDC_SHIFT_DIR_R2L
 *     	                @arg LCDC_SHIFT_DIR_T2B
 *     	                @arg LCDC_SHIFT_DIR_B2L
 * @note   Ensure that both ImgbufCurrent and ImgbufTarget are properly set when using the shift function.
 * @retval None
 */
void LCDC_DMAImageShiftConfig(LCDC_TypeDef *LCDCx, u32 ImgbufCurrent, u32 ImgbufTarget, enum LCDC_ShiftDir Direction)
{
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	u32 ImgW, ImgH;
	LCDC_DMAImgAdvanceDef CfgInfo;

	/* Get image width and height */
	ImgW = LCDC_GET_IMAGEWIDTH(LCDCx->LCDC_PLANE_SIZE);
	ImgH = LCDC_GET_IMAGEHEIGHT(LCDCx->LCDC_PLANE_SIZE);

	/* Fill image address and offset based on different directions */
	switch (Direction) {
	case LCDC_SHIFT_DISABLE:
		RTK_LOGI(TAG, "DIR%lu \n", LCDC_SHIFT_DISABLE);
		CfgInfo.ImgAddrA = ImgbufCurrent;
		CfgInfo.ImgAddrB = (u32)NULL;
		CfgInfo.ImgOfstDotX = LCDC_SHIFT_OFST_INVALID;
		CfgInfo.ImgOfstLineY = LCDC_SHIFT_OFST_INVALID;
		break;

	case LCDC_SHIFT_DIR_L2R:
		RTK_LOGI(TAG, "DIR%lu L2R \n", LCDC_SHIFT_DIR_L2R);
		CfgInfo.ImgAddrA = ImgbufTarget;
		CfgInfo.ImgAddrB = ImgbufCurrent;
		CfgInfo.ImgOfstDotX = ImgW;//ImgW > 0
		CfgInfo.ImgOfstLineY = LCDC_SHIFT_OFST_INVALID;

		break;

	case LCDC_SHIFT_DIR_R2L:
		RTK_LOGI(TAG, "DIR%lu R2L \n", LCDC_SHIFT_DIR_R2L);
		CfgInfo.ImgAddrA = ImgbufCurrent;
		CfgInfo.ImgAddrB = ImgbufTarget;
		CfgInfo.ImgOfstDotX = 0x0;//0 > ImgW
		CfgInfo.ImgOfstLineY = LCDC_SHIFT_OFST_INVALID;

		break;

	case LCDC_SHIFT_DIR_T2B:
		RTK_LOGI(TAG, "DIR%lu T2B \n", LCDC_SHIFT_DIR_T2B);
		CfgInfo.ImgAddrA = ImgbufTarget;
		CfgInfo.ImgAddrB = ImgbufCurrent;
		CfgInfo.ImgOfstDotX = LCDC_SHIFT_OFST_INVALID;
		CfgInfo.ImgOfstLineY = ImgH; //ImgH > 0

		break;

	case LCDC_SHIFT_DIR_B2T:
		RTK_LOGI(TAG, "DIR%lu B2L \n", LCDC_SHIFT_DIR_B2T);
		CfgInfo.ImgAddrA = ImgbufCurrent;
		CfgInfo.ImgAddrB = ImgbufTarget;
		CfgInfo.ImgOfstDotX = LCDC_SHIFT_OFST_INVALID;
		CfgInfo.ImgOfstLineY = 0x0; // 0 > ImgH

		break;

	default:
		RTK_LOGE(TAG, "Unknown shift direction. \n");
		return;
	}

	/* Ensure HsOfst and VsOfst cannot be non-zero simultaneously */
	// assert_param(!((HsOfst) && (VsOfst)));!!!
	//ofst_w < panel_width && ofst_h < panel_height!!!

	LCDC_DMAImgCfgAdvance(LCDCx, &CfgInfo);
}

/**
 * @brief  Trigger shadow register reload to active register, to apply new configuration
 * @param  LCDCx: where LCDCx can be LCDC.
 * @note   The shadow registers read back the active values. Until the reload has been done, the 'old' value is read.
 * @retval None
 */
void LCDC_ShadowReloadConfig(LCDC_TypeDef *LCDCx)
{
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	/* set shadow reload bit in vertical blank period, cleared by hardware after reload done */
	LCDCx->LCDC_SHW_RLD_CFG |= LCDC_BIT_VBR;
}


/**
 * @brief  Config LCDC output color format.
 * @param  LCDCx: Where LCDCx can be LCDC.
 * @param  CorlorFmtOut: Specifies the output color format.
 *                    This parameter can be any following value:
 *                    @arg LCDC_OUTPUT_FORMAT_RGB888
 *                    @arg LCDC_OUTPUT_FORMAT_RGB565
 *                    @arg LCDC_OUTPUT_FORMAT_BGR888
 *                    @arg LCDC_OUTPUT_FORMAT_BGR565
 * @retval None
 */
void LCDC_ColorFomatOutputConfig(LCDC_TypeDef *LCDCx, u32 CorlorFmtOut)
{
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));
	assert_param(IS_LCDC_OUTPUT_FORMAT(CorlorFmtOut));

	LCDCx->LCDC_COLOR_CFG &= (~LCDC_MASK_COLOR_FORMAT);
	LCDCx->LCDC_COLOR_CFG |= (LCDC_COLOR_FORMAT(CorlorFmtOut));
}

/**
 * @brief  Config LCDC output color format.
 * @param  LCDCx: Where LCDCx can be LCDC.
 * @param  CorlorFmtIn: Specifies the intput color format.
 *                    This parameter can be any following value:
 *                    @arg
 *                    @arg
 *                    @arg LCDC_OUTPUT_FORMAT_RGB666
 * @retval None
 */
void LCDC_ColorFomatInputConfig(LCDC_TypeDef *LCDCx, u32 CorlorFmtIn)
{
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));
	assert_param(IS_LCDC_INPUT_FORMAT(CorlorFmtIn));

	LCDCx->LCDC_IMG_CFG &= ~(LCDC_MASK_IMG_FORMAT);
	LCDCx->LCDC_IMG_CFG |= (LCDC_IMG_FORMAT(CorlorFmtIn));
}

/**
 * @brief  Enable or disable the specified LCDC interrupts.
 * @param  LCDCx: Where LCDCx can be LCDC.
 * @param  LCDC_IT: Specifies the LCDC interrupts sources to be enabled or disabled.
 *     This parameter can be any combination of the following values:
 *     @arg LCDC_IT_DMAUNDFW: DMA FIFO underflow interrupt
 *     @arg LCDC_IT_FRDN: LCD refresh done interrupt
 *     @arg LCDC_IT_LINE: line interrupt
 *     @arg LCDC_IT_IO_TIMEOUT: IO write/read timeout interrupt
 *     @arg LCDC_IT_FRM_START: Frame Start interrupt
 * @param  NewState: New state of the specified LCDC interrupts.
 *         This parameter can be: ENABLE or DISABLE.
 * @retval None
 */
void LCDC_INTConfig(LCDC_TypeDef *LCDCx, u32 LCDC_IT, u32 NewState)
{
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));
	assert_param(IS_LCDC_INTR_TYPE(LCDC_IT));

	if (NewState != DISABLE) {
		/* enable the selected LCDC interrupts */
		LCDCx->LCDC_IRQ_EN |= LCDC_IT;
	} else {
		/* disable the selected LCDC interrupts */
		LCDCx->LCDC_IRQ_EN &= (~LCDC_IT);
	}
}

/**
 * @brief  Configure line interrupt position.
 * @param  LCDCx: Where LCDCx can be LCDC.
 * @param  LineNum: The specific line number to assert LineInterrupt.
 * @retval None
 */
void LCDC_LineINTPosConfig(LCDC_TypeDef *LCDCx, u32 LineNum)
{
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));
	// assert_param(LineNum <= 0xFFF);
	assert_param(LineNum < LCDC_GET_IMAGEHEIGHT(LCDCx->LCDC_PLANE_SIZE));

	/* fill the LINE_INT_POS field */
	LCDCx->LCDC_LINE_INT_POS &= (~LCDC_MASK_LINE_INT_POS);
	LCDCx->LCDC_LINE_INT_POS |= LCDC_LINE_INT_POS(LineNum);
}

/**
 * @brief  Get LCDC interrupt status.
 * @param  LCDCx: Where LCDCx can be LCDC.
 * @retval Interrupt status
 */
u32 LCDC_GetINTStatus(LCDC_TypeDef *LCDCx)
{
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	return LCDCx->LCDC_IRQ_STATUS;
}


/**
 * @brief  Get LCDC Raw Interrupt Status.
 * @param  LCDCx: Where LCDCx can be LCDC.
 * @retval Raw interrupt status
 */
u32 LCDC_GetRawINTStatus(LCDC_TypeDef *LCDCx)
{
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	return LCDCx->LCDC_IRQ_RAW;
}

/**
 * @brief  Clear all of the LCDC interrupt pending bit.
 * @param  LCDCx: Where LCDCx can be LCDC.
 * @retval None
 */
void LCDC_ClearAllINT(LCDC_TypeDef *LCDCx)
{
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	/* write 1 to clear interrupt status */
	LCDCx->LCDC_IRQ_STATUS = 0xFFFFFFFF;
}

/**
 * @brief  Clear the LCDC's interrupt pending bits.
 * @param  LCDCx: Where LCDCx can be LCDC.
 * @param  LCDC_IT: Specifies the interrupt to be cleared.
 *     This parameter can be any combination of the following values:
 *     @arg LCDC_BIT_DMA_UN_INTS:line interrupt
 *     @arg LCDC_BIT_LCD_FRD_INTS: refresh frame done interrupt
 *     @arg LCDC_BIT_LCD_LIN_INTS: DMA FIFO under flow interrupt
 *     @arg LCDC_BIT_IO_TIMEOUT_INTS: IO write/read timeout interrupt
 *     @arg LCDC_BIT_FRM_START_INTS: Frame Start interrupt
 * @retval None
 */
void LCDC_ClearINT(LCDC_TypeDef *LCDCx, u32 LCDC_IT)
{
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));
	assert_param(IS_LCDC_INTR_STATUS(LCDC_IT));

	/* clear the specified interrupt */
	LCDCx->LCDC_IRQ_STATUS = LCDC_IT;
}

/**
 * @brief  Get the current Image offset .
 * @param  LCDCx: where LCDCx can be LCDC.
 * @param  pImgHs: the image horizen offset pointer.
 * @param  pImgVs: the image vertical offset pointer.
 * @retval None
 */
void LCDC_GetImgOffset(LCDC_TypeDef *LCDCx, u32 *pImgHs, u32 *pImgVs)
{
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	/* Get the Image hs_ofst */
	*pImgHs = LCDC_GET_IMG_HS_OFST(LCDCx->LCDC_IMG_SLIDE);

	/* Get the Image vs_ofst */
	*pImgVs = LCDC_GET_IMG_VS_OFST(LCDCx->LCDC_IMG_SLIDE);
}

/**
 * @brief  Get the Image base address .
 * @param  LCDCx: where LCDCx can be LCDC.
 * @param  pCurImgA: the current ImgA address pointer.
 * @param  pCurImgB: the current ImgB address pointer.
 * @retval None
 */
void LCDC_GetImgAddr(LCDC_TypeDef *LCDCx, u32 *pImgA, u32 *pImgB)
{
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	/* Get the ImgA address */
	*pImgA = LCDC_GET_IMG_A_BASE_ADDR(LCDCx->LCDC_IMG_A_BASE_ADDR);

	/* Get the ImgB address */
	*pImgB = LCDC_GET_IMG_B_BASE_ADDR(LCDCx->LCDC_IMG_B_BASE_ADDR);
}

/**
 * @brief  Get the current position.
 * @param  LCDCx: where LCDCx can be LCDC.
 * @param  pCurPosX: the current X position pointer.
 * @param  pCurPosY: the current Y position pointer.
 * @retval None
 */
void LCDC_GetCurPosStatus(LCDC_TypeDef *LCDCx, u32 *pCurPosX, u32 *pCurPosY)
{
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	/* Get the X position */
	*pCurPosX = LCDC_GET_CUR_POS_X(LCDCx->LCDC_CUR_POS_STATUS);

	/* Get the Y position */
	*pCurPosY = LCDC_GET_CUR_POS_Y(LCDCx->LCDC_CUR_POS_STATUS);
}

/**
 * @brief  Enable or disable the LCDC.
 * @param  LCDCx: Where LCDCx can be LCDC.
 * @param  NewState: New state of the LCDC.
 *                   This parameter can be: ENABLE or DISABLE.
 * @note  when NewState is DISABLE, the disable action will be performed instantly.
 * @retval None
 */
void LCDC_Cmd(LCDC_TypeDef *LCDCx, u32 NewState)
{
	u32 TempCtrl;

	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	TempCtrl = LCDCx->LCDC_CTRL;

	if (NewState != DISABLE) {
		/* clear instant disable bit, or it leads to enable LCDC unsuccessfully */
		TempCtrl &= ~LCDC_BIT_INST_DIS;

		/* set enable bit to enable LCDC */
		TempCtrl |= LCDC_BIT_EN;
	} else {
		/* set instant disable bit, clear both this bit and LCDCEN bit */
		TempCtrl |= LCDC_BIT_INST_DIS;
	}

	LCDCx->LCDC_CTRL = TempCtrl;
}

/**
 * @brief  Deinitialize the LCDC.
 * @param  LCDCx: where LCDCx can be LCDC.
 * @note   Disable LCDC instantly, clear and disable all interrupts.
 * @retval None
 */
void LCDC_DeInit(LCDC_TypeDef *LCDCx)
{
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	/* disable LCDC instantly */
	LCDCx->LCDC_CTRL |= LCDC_BIT_INST_DIS;

	/* clear all interrupts */
	LCDCx->LCDC_IRQ_STATUS = 0xFFFFFFFF;

	/* disable all interrupts */
	LCDCx->LCDC_IRQ_EN = 0;
}



/**
 * @brief Ctrl MCU swap function, default disable swap function.
 *        If enable swap, swap data[15:8] with date[7:0].
 * @param Status: Enable/Disable.
 * @retval Supported in 8-bit RGB565 of MCU-DMA mode.
 * @note   Called after struct init for assert issue
 */
void LCDC_MCUCtrlSwap(LCDC_TypeDef *LCDCx, u8 Status)
{
	u32 regtmp;
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	assert_param(LCDC_GET_IF_MODE(LCDCx->LCDC_CTRL) == LCDC_MCU_IF_8_BIT);
	assert_param(LCDC_GET_COLOR_FORMAT(LCDCx->LCDC_COLOR_CFG) == LCDC_OUTPUT_FORMAT_RGB565);//rgb565
	assert_param((LCDCx->LCDC_MCU_CFG & LCDC_BIT_MCU_IO_MODE_RUN) == LCDC_MCU_RUN_DMA_MODE);//dma mode

	regtmp = LCDCx->LCDC_MCU_CFG;

	if (Status == ENABLE) {
		regtmp |= LCDC_BIT_DAT_SWAP;
	} else {
		regtmp &= ~LCDC_BIT_DAT_SWAP;
	}

	LCDCx->LCDC_MCU_CFG = regtmp;
}
/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */