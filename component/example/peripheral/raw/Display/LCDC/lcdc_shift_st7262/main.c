#include "ameba_soc.h"
#include "lcdc_display.h"
#include "lcdc_rgb_shift.h"

#define LCD_RGB_IMG_WIDTH					800
#define LCD_RGB_IMG_HEIGHT					480
#define	LCDC_LINE_NUM_INTR_DEF				(LCD_RGB_IMG_WIDTH / 2)

/* config shift direction, which can be set enum LCDC_ShiftDir */
#define LCDC_SHIFT_DIRECTION				 LCDC_SHIFT_DIR_R2L

/* allocate image buffer address */
#define IMG1_BUF_IN_PSRAM					0x60000100
#define IMG2_BUF_IN_PSRAM					0x60200100
static u8 *const pImgBuf1 = (u8 *)(IMG1_BUF_IN_PSRAM);
static u8 *const pImgBuf2 = (u8 *)(IMG2_BUF_IN_PSRAM);

/* initialize irq info */
struct LCDC_IRQInfoDef gLcdcIrqInfo = {
	.IrqNum = LCDC_IRQ,//49
	.IrqData = (u32)LCDC,
	.IrqPriority = INT_PRI_MIDDLE,
};

/* split an image into slices to display step by step */
volatile u32 gRgbShiftSlice = 40;

LCDC_ShiftInfoDef gRGBShiftInfo;

/* implemented by the application layer */
void lvgl_callback(enum LCDC_INTR_EVENT event)
{
	/* fix for warning */
	UNUSED(event);

	//add your code here
}

/* handle dma fetching image start address and length */
static void lcdc_rgb_handle_slide(LCDC_ShiftInfoDef *pInfoStruct)
{
	u32 *pslice;
	u32 vs_ofst = 0, hs_ofst = 0;
	u32 vs_ofst_tmp = 0, hs_ofst_tmp = 0;

	u32 Img_W = pInfoStruct->ImgW;
	u32 Img_H = pInfoStruct->ImgH;
	u32 direction = pInfoStruct->Direction;

	pslice = &pInfoStruct->SliceIdx;

	/* split an figure into slices to display step by step */
	(*pslice)++;

	if (*pslice > (pInfoStruct->SliceTotal)) {
		*pslice = 0;
	}

	RTK_LOGI(NOTAG, "dir=%u sliceId=%lu\tW%u\tH%u\tSlotX%u\tSlotY%u \n", direction, pInfoStruct->SliceIdx, Img_W, Img_H, pInfoStruct->SlotX, pInfoStruct->SlotY);

	/* calculate vs_ofst/hs_ofst depends on direction */
	if ((direction == LCDC_SHIFT_DIR_T2B) || (direction == LCDC_SHIFT_DIR_B2T)) {
		vs_ofst_tmp = (*pslice) * (pInfoStruct->SlotY);

		if (vs_ofst_tmp > Img_H) {
			vs_ofst_tmp = 0;
			*pslice = 0;
		}

	} else if ((direction == LCDC_SHIFT_DIR_R2L) || (direction == LCDC_SHIFT_DIR_L2R))  {
		hs_ofst_tmp = (*pslice) * (pInfoStruct->SlotX);

		if (hs_ofst_tmp > Img_W) {
			hs_ofst_tmp = 0;
			*pslice = 0;
		}
	}

	switch (direction) {
	case LCDC_SHIFT_DIR_L2R:
		hs_ofst = Img_W - hs_ofst_tmp;
		vs_ofst = LCDC_SHIFT_OFST_INVALID;

		break;

	case LCDC_SHIFT_DIR_R2L:
		hs_ofst = hs_ofst_tmp;
		vs_ofst = LCDC_SHIFT_OFST_INVALID;

		break;

	case LCDC_SHIFT_DIR_T2B:
		hs_ofst = LCDC_SHIFT_OFST_INVALID;
		vs_ofst = Img_H - vs_ofst_tmp;

		break;

	case LCDC_SHIFT_DIR_B2T:
		hs_ofst = LCDC_SHIFT_OFST_INVALID;
		vs_ofst = vs_ofst_tmp;

		break;

	default :
		RTK_LOGE(NOTAG, "Unknow shift direction");
		return;
	}

	RTK_LOGI(NOTAG, "[Handle] hs%lu vs%lu \r\n\n", hs_ofst, vs_ofst);

	/* change dma fetching image start address and length */
	LCDC_DMAImageOfstConfig(LCDC, hs_ofst, vs_ofst);
	LCDC_ShadowReloadConfig(LCDC);
}

static u32 lcdc_rgb_isr(void *pInfo)
{
	volatile u32 IntId;
	u32 PosX, PosY;
	u32 HsOfst, VsOfst;

	LCDC_ShiftInfoDef *pShiftInfo = (LCDC_ShiftInfoDef *)pInfo;

	IntId = LCDC_GetINTStatus(LCDC);

	LCDC_GetCurPosStatus(LCDC, &PosX, &PosY);
	LCDC_GetImgOffset(LCDC, &HsOfst, &VsOfst);

	RTK_LOGI(NOTAG, "[INTS 0x%02x]  X=%lu, Y=%lu, H%lu, V%lu \n", IntId, PosX, PosY, HsOfst, VsOfst);

	if (IntId & LCDC_BIT_LCD_FRD_INTS) {
		LCDC_ClearINT(LCDC, LCDC_BIT_LCD_FRD_INTS);

		lvgl_callback(EVENT_FRM_DONE);
	}

	if (IntId & LCDC_BIT_FRM_START_INTS) {
		LCDC_ClearINT(LCDC, LCDC_BIT_FRM_START_INTS);

		lvgl_callback(EVENT_FRM_START);
	}

	if (IntId & LCDC_BIT_LCD_LIN_INTS) {
		LCDC_ClearINT(LCDC, LCDC_BIT_LCD_LIN_INTS);

		lvgl_callback(EVENT_LINE_HIT);

		/* switch image display info */
		lcdc_rgb_handle_slide(pShiftInfo);
	}

	if (IntId & LCDC_BIT_PANEL_TE_INTS) {
		LCDC_ClearINT(LCDC, LCDC_BIT_PANEL_TE_INTS);

		lvgl_callback(EVENT_PANEL_TE);
	}

	return 0;
}


static void lcdc_fill_frame_buffer(void)
{
	u32 i, TempDot;

#if LCDC_RGB_SHIFT_TEST_PURE_COLOR
	u8 *pbuf1 = (u8 *)pImgBuf1;
	u8 *pbuf2 = (u8 *)pImgBuf2;

	RTK_LOGI(NOTAG, "[fb] shift pure color. \r\n");

	TempDot = LCD_RGB_IMG_WIDTH * LCD_RGB_IMG_HEIGHT;

	for (i = 0; i < (TempDot * 3); i += 3) {//BGR888
		pbuf1[i] = 0x00;
		pbuf1[i + 1] = 0x00;
		pbuf1[i + 2] = 0xFF;	//RED

		pbuf2[i] = 0x00;
		pbuf2[i + 1] = 0xFF;	//GREEN
		pbuf2[i + 2] = 0x00;
	}

#else
	RTK_LOGI(NOTAG, "[fb] shift figure. \r\n");

	_memcpy((u8 *)pImgBuf1, (u8 *)rgb800x480x24bpp_current, rgb_figlen_current);
	_memcpy((u8 *)pImgBuf2, (u8 *)rgb800x480x24bpp_target, rgb_figlen_target);

#endif

	// DCache_Clean((u32)(pImgBuf1), (TempDot * 3));
	// DCache_Clean((u32)(pImgBuf2), (TempDot * 3));
}

static void pinmux_config_rgb(u32 group)
{
	RTK_LOGI(NOTAG, "PINMUX_LCDCRgb group%u \r\n", group);

	if (group == PINMUX_S0) {

		/* LCD Display Pin for ST7262.
		high: normal display; low： standby */
		GPIO_InitTypeDef GPIO_InitStruct_Display;

		GPIO_InitStruct_Display.GPIO_Pin = LCD_DISPLAY_RGB;//PB30
		GPIO_InitStruct_Display.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_Init(&GPIO_InitStruct_Display);
		GPIO_WriteBit(LCD_DISPLAY_RGB, 1);

		/* LCD BLEN Pin for ST7262.
		high: BL enable; low: BL disable */
		GPIO_InitTypeDef GPIO_InitStruct_BLEN;
		GPIO_InitStruct_BLEN.GPIO_Pin = LCD_BLEN_RGB;
		GPIO_InitStruct_BLEN.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_Init(&GPIO_InitStruct_BLEN);

		GPIO_WriteBit(LCD_BLEN_RGB, 1);

		/* LCD Signal for RGB interface in HV mode
		DE signal is required for LCD SYNC-DE mode */
		Pinmux_Config(_PA_23, PINMUX_FUNCTION_LCD_D0);	/*D0 - B0*/
		Pinmux_Config(_PB_31, PINMUX_FUNCTION_LCD_D1);	/*D1*/
		Pinmux_Config(_PB_27, PINMUX_FUNCTION_LCD_D2);	/*D2*/
		Pinmux_Config(_PB_22, PINMUX_FUNCTION_LCD_D3);	/*D3*/
		Pinmux_Config(_PC_1, PINMUX_FUNCTION_LCD_D4);	/*D4*/
		Pinmux_Config(_PA_21, PINMUX_FUNCTION_LCD_D5);	/*D5*/
		Pinmux_Config(_PA_22, PINMUX_FUNCTION_LCD_D6);	/*D6*/
		Pinmux_Config(_PB_23, PINMUX_FUNCTION_LCD_D7);	/*D7 - B7 */

		Pinmux_Config(_PA_12, PINMUX_FUNCTION_LCD_D8);	/*D8 - G0*/
		Pinmux_Config(_PA_20, PINMUX_FUNCTION_LCD_D9);	/*D9*/
		Pinmux_Config(_PB_26, PINMUX_FUNCTION_LCD_D10);	/*D10*/
		Pinmux_Config(_PA_17, PINMUX_FUNCTION_LCD_D11);	/*D11*/
		Pinmux_Config(_PA_25, PINMUX_FUNCTION_LCD_D12);	/*D12*/
		Pinmux_Config(_PB_25, PINMUX_FUNCTION_LCD_D13);	/*D13*/
		Pinmux_Config(_PB_0, PINMUX_FUNCTION_LCD_D14);	/*D14*/
		Pinmux_Config(_PA_24, PINMUX_FUNCTION_LCD_D15);	/*D15 - G7*/

		Pinmux_Config(_PB_21, PINMUX_FUNCTION_LCD_D16);	/*D16 - R0*/
		Pinmux_Config(_PB_2, PINMUX_FUNCTION_LCD_D17);	/*D17 */
		Pinmux_Config(_PB_28, PINMUX_FUNCTION_LCD_D18);	/*D18*/
		Pinmux_Config(_PC_0, PINMUX_FUNCTION_LCD_D19);	/*D19*/
		Pinmux_Config(_PA_13, PINMUX_FUNCTION_LCD_D20);	/*D20*/
		Pinmux_Config(_PA_28, PINMUX_FUNCTION_LCD_D21);	/*D21*/
		Pinmux_Config(_PA_29, PINMUX_FUNCTION_LCD_D22);	/*D22*/
		Pinmux_Config(_PB_29, PINMUX_FUNCTION_LCD_D23);	/*D23 - R7*/

		Pinmux_Config(_PA_26, PINMUX_FUNCTION_LCD_RGB_HSYNC);	/*RD, HSYNC*/
		Pinmux_Config(_PA_16, PINMUX_FUNCTION_LCD_RGB_VSYNC);	/*VSYNC-TE, VSYNC*/
		Pinmux_Config(_PA_27, PINMUX_FUNCTION_LCD_RGB_DCLK);	/*WR, DCLK*/

		Pinmux_Config(_PB_1, PINMUX_FUNCTION_LCD_RGB_DE);	/*SYNC-DE*/

	} else {
		RTK_LOGW(NOTAG, "Unknow Pinmux Group%u \r\n", group);
	}
}


void lcdc_rgb_shift_st7262(void)
{
	LCDC_RGBInitTypeDef LCDC_RGBInitStructTmp;

	/* disable Dcache */
	DCache_Disable();

	/* pinmux config */
	pinmux_config_rgb(PINMUX_S0);

	/* fill frame buffer */
	lcdc_fill_frame_buffer();

	if (SYSCFG_CHIPType_Get() == CHIP_TYPE_FPGA) {
		u32 regval;

		/* debug port select LCDC */
		regval = HAL_READ32(0x4080A2FC, 0);
		regval &= (~(0xF << 0));
		regval |= ((0x1 & 0xF) << 0);
		HAL_WRITE32(0x4080A2FC, 0x0, regval);

		RTK_LOGI(NOTAG, "Adjust voclk to 80MHz \r\n");
		regval = HAL_READ32(0x4080A2FC, 0);
		regval &= (~(0x7 << 5));
		regval |= ((0x5 & 0x7) << 5);

		HAL_WRITE32(0x4080A2FC, 0x0, regval);
	}

	/* enable ckd_fen for vo_clk and shperi_clk */
	RCC_PeriphClockDividerFENSet(USB_PLL_VO, ENABLE);
	RCC_PeriphClockDividerFENSet(USB_PLL_SHPERI, ENABLE);

	/* enable cke for SHPERI, also cke and fen for LCDC */
	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_SHPERI_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_LCDC, APBPeriph_LCDC_CLOCK, ENABLE);

	/* config LCDC structure follow st7262 spec */
	LCDC_RGBStructInit(&LCDC_RGBInitStructTmp);
	LCDC_RGBInitStructTmp.LCDC_RGBIfMode = LCDC_RGB_IF_24_BIT;
	LCDC_RGBInitStructTmp.LCDC_RGBDMAMode = LCDC_RGB_DMA_AUTO_MODE;
	LCDC_RGBInitStructTmp.LCDC_RGBSyncMode = LCDC_RGB_HV_MODE;

	LCDC_RGBInitStructTmp.LCDC_RGBImgWidth = LCD_RGB_IMG_WIDTH;
	LCDC_RGBInitStructTmp.LCDC_RGBImgHeight = LCD_RGB_IMG_HEIGHT;
	LCDC_RGBInitStructTmp.LCDC_RGBRefreshFreq = 35;//30 not enough
	LCDC_RGBInitStructTmp.LCDC_RGBInputFormat = LCDC_INPUT_FORMAT_BGR888;//LCDC_INPUT_FORMAT_RGB888;
	LCDC_RGBInitStructTmp.LCDC_RGBOutputFormat = LCDC_OUTPUT_FORMAT_RGB888;

	LCDC_RGBInitStructTmp.LCDC_RGBVSW = 1;//4;
	LCDC_RGBInitStructTmp.LCDC_RGBVBP = 4;//8;
	LCDC_RGBInitStructTmp.LCDC_RGBVFP = 6;//8;
	LCDC_RGBInitStructTmp.LCDC_RGBHSW = 4;
	LCDC_RGBInitStructTmp.LCDC_RGBHBP = 40;//39;
	LCDC_RGBInitStructTmp.LCDC_RGBHFP = 40;

	LCDC_RGBInitStructTmp.LCDC_RGBEnPulPlrty = LCDC_RGB_EN_PUL_HIGH_LEV_ACTIVE;
	LCDC_RGBInitStructTmp.LCDC_RGBHsPulPlrty = LCDC_RGB_HS_PUL_LOW_LEV_SYNC;
	LCDC_RGBInitStructTmp.LCDC_RGBVsPulPlrty = LCDC_RGB_VS_PUL_LOW_LEV_SYNC;
	LCDC_RGBInitStructTmp.LCDC_RGBDclkActvEdge = LCDC_RGB_DCLK_FALLING_EDGE_FETCH;

	/* init LCDC controller */
	LCDC_RGBInit(LCDC, &LCDC_RGBInitStructTmp);

	/* config dma burst size */
	LCDC_DMABurstSizeConfig(LCDC, 2);

	/* config dma image buffer address and shift direction */
	LCDC_DMAImageShiftConfig(LCDC, (u32)pImgBuf1, (u32)pImgBuf2, LCDC_SHIFT_DIRECTION);

	/* storage info used for handle shift operation */
	gRGBShiftInfo.ImgW = LCD_RGB_IMG_WIDTH;
	gRGBShiftInfo.ImgH = LCD_RGB_IMG_HEIGHT;
	gRGBShiftInfo.Direction = LCDC_SHIFT_DIRECTION;
	gRGBShiftInfo.SliceTotal = gRgbShiftSlice;

	if ((LCDC_SHIFT_DIRECTION == LCDC_SHIFT_DIR_L2R) || (LCDC_SHIFT_DIRECTION == LCDC_SHIFT_DIR_R2L)) {
		gRGBShiftInfo.SlotX = LCD_RGB_IMG_WIDTH / gRgbShiftSlice;
		gRGBShiftInfo.SlotY = 0;

	} else if ((LCDC_SHIFT_DIRECTION == LCDC_SHIFT_DIR_T2B) || (LCDC_SHIFT_DIRECTION == LCDC_SHIFT_DIR_B2T)) {
		gRGBShiftInfo.SlotX = 0;
		gRGBShiftInfo.SlotY = LCD_RGB_IMG_HEIGHT / gRgbShiftSlice;
	}

	RTK_LOGI(NOTAG, "> LCDC_SHIFT_DIRECTION=%lu, W%lu H%lu SlotX=%lu SlotY=%lu\n", LCDC_SHIFT_DIRECTION, gRGBShiftInfo.ImgW, gRGBShiftInfo.ImgH,
			 gRGBShiftInfo.SlotX,
			 gRGBShiftInfo.SlotY);

	/* register interrupt handler */
	InterruptRegister((IRQ_FUN)lcdc_rgb_isr, gLcdcIrqInfo.IrqNum, (u32)(&gRGBShiftInfo), gLcdcIrqInfo.IrqPriority);
	InterruptEn(gLcdcIrqInfo.IrqNum, gLcdcIrqInfo.IrqPriority);

	/* config intr line number */
	LCDC_LineINTPosConfig(LCDC, LCDC_LINE_NUM_INTR_DEF);

	/* enable intr type */
	LCDC_INTConfig(LCDC, (LCDC_BIT_LCD_FRD_INTEN | LCDC_BIT_FRM_START_INTEN | LCDC_BIT_LCD_LIN_INTEN | LCDC_BIT_PANEL_TE_INTEN), ENABLE);

	/* enable lcdc controller */
	LCDC_Cmd(LCDC, ENABLE);

	// while(1);
	rtos_task_delete(NULL);
}

int main(void)
{
	if (rtos_task_create(NULL, "rgb_shift_st7262", (rtos_task_t)lcdc_rgb_shift_st7262, NULL, (4096), 1) != SUCCESS) {
		printf("Cannot create rgb_shift_st7262 demo task\n\r");
	}

	rtos_sched_start();
}