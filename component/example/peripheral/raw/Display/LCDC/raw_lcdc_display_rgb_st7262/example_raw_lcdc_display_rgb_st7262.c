#include "ameba_soc.h"
#include "os_wrapper.h"
#include <stdio.h>

#define LCD_BLEN_RGB			_PC_1
#define LCD_DISPLAY_RGB			_PB_13

#define LCD_VSW					1
#define LCD_VBP					4
#define LCD_VFP					6
#define LCD_HSW					4
#define LCD_HBP					40
#define LCD_HFP					40

/* Uncomment this macro for the display scenario. */
#define ALLOC_BUFFER_FOR_DISPLAY

#ifdef ALLOC_BUFFER_FOR_DISPLAY
#define WIDTH						800
#define HEIGHT						480
#define MEM_SIZE					(WIDTH * HEIGHT * 3)		//rgb888
#define LCDC_LINE_NUM_INTR_DEF		(HEIGHT / 2)

static u8 *PIX_DATA0 = (u8 *)0x60000000;
static u8 *PIX_DATA1 = (u8 *)0x60100000;
static u8 *PIX_DATA2 = (u8 *)0x60200000;

#else /* alloc small buffer for compile issue */

#define WIDTH						50
#define HEIGHT						50
#define MEM_SIZE					(WIDTH * HEIGHT * 3)		//rgb888
#define LCDC_LINE_NUM_INTR_DEF		(HEIGHT / 2)

static u8 PIX_DATA0[MEM_SIZE] __attribute__((aligned(64)));
static u8 PIX_DATA1[MEM_SIZE] __attribute__((aligned(64)));
static u8 PIX_DATA2[MEM_SIZE] __attribute__((aligned(64)));
#endif

static struct LCDC_IRQInfoDef {
	u32 IrqNum;
	u32 IrqData;
	u32 IrqPriority;
} gLcdcIrqInfo;

/* config pinmux and control blen pad */
static void lcdc_pinmux_config(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "%s \r\n", __func__);

	/* LCD Display Pin for ST7262.
	high: normal display; low£º standby */
	GPIO_InitTypeDef GPIO_InitStruct_Display;

	GPIO_InitStruct_Display.GPIO_Pin = LCD_DISPLAY_RGB;
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
	Pinmux_Config(_PB_28, PINMUX_FUNCTION_LCD_D0);	/*D0 - B0*/
	Pinmux_Config(_PB_30, PINMUX_FUNCTION_LCD_D1);	/*D1*/
	Pinmux_Config(_PC_0, PINMUX_FUNCTION_LCD_D2);	/*D2*/
	Pinmux_Config(_PA_21, PINMUX_FUNCTION_LCD_D3);	/*D3*/
	Pinmux_Config(_PA_22, PINMUX_FUNCTION_LCD_D4);	/*D4*/
	Pinmux_Config(_PA_23, PINMUX_FUNCTION_LCD_D5);	/*D5*/
	Pinmux_Config(_PA_19, PINMUX_FUNCTION_LCD_D6);	/*D6*/
	Pinmux_Config(_PA_28, PINMUX_FUNCTION_LCD_D7);	/*D7 - B7 */

	Pinmux_Config(_PA_29, PINMUX_FUNCTION_LCD_D8);	/*D8 - G0*/
	Pinmux_Config(_PB_15, PINMUX_FUNCTION_LCD_D9);	/*D9*/
	Pinmux_Config(_PB_16, PINMUX_FUNCTION_LCD_D10);	/*D10*/
	Pinmux_Config(_PB_17, PINMUX_FUNCTION_LCD_D11);	/*D11*/
	Pinmux_Config(_PB_21, PINMUX_FUNCTION_LCD_D12);	/*D12*/
	Pinmux_Config(_PB_22, PINMUX_FUNCTION_LCD_D13);	/*D13*/
	Pinmux_Config(_PA_27, PINMUX_FUNCTION_LCD_D14);	/*D14*/
	Pinmux_Config(_PB_26, PINMUX_FUNCTION_LCD_D15);	/*D15 - G7*/

	Pinmux_Config(_PA_16, PINMUX_FUNCTION_LCD_D16);	/*D16 - R0*/
	Pinmux_Config(_PA_17, PINMUX_FUNCTION_LCD_D17);	/*D17 */
	Pinmux_Config(_PA_20, PINMUX_FUNCTION_LCD_D18);	/*D18*/
	Pinmux_Config(_PA_24, PINMUX_FUNCTION_LCD_D19);	/*D19*/
	Pinmux_Config(_PA_26, PINMUX_FUNCTION_LCD_D20);	/*D20*/
	Pinmux_Config(_PA_18, PINMUX_FUNCTION_LCD_D21);	/*D21*/
	Pinmux_Config(_PA_30, PINMUX_FUNCTION_LCD_D22);	/*D22*/
	Pinmux_Config(_PA_31, PINMUX_FUNCTION_LCD_D23);	/*D23 - R7*/

	Pinmux_Config(_PB_14, PINMUX_FUNCTION_LCD_RGB_HSYNC);	/*RD, HSYNC*/
	Pinmux_Config(_PB_18, PINMUX_FUNCTION_LCD_RGB_VSYNC);	/*VSYNC-TE, VSYNC*/
	Pinmux_Config(_PB_0, PINMUX_FUNCTION_LCD_RGB_DCLK);	/*WR, DCLK*/
	Pinmux_Config(_PB_23, PINMUX_FUNCTION_LCD_RGB_DE);	/*SYNC-DE*/

}

static void lcdc_fill_framebuf(void)
{
	u32 i;
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "%s \r\n", __func__);

	for (i = 0; i < MEM_SIZE; i = i + 3) {
		/* buffer0: red */
		PIX_DATA0[i] = 0x00;
		PIX_DATA0[i + 1] = 0x00;
		PIX_DATA0[i + 2] = 0xFF;

		/* buffer1: green */
		PIX_DATA1[i] = 0x00;
		PIX_DATA1[i + 1] = 0xFF;
		PIX_DATA1[i + 2] = 0x00;

		/* buffer2: blue */
		PIX_DATA2[i] = 0xFF;
		PIX_DATA2[i + 1] = 0x00;
		PIX_DATA2[i + 2] = 0x00;
	}

	DCache_Clean((u32)&PIX_DATA0[0], MEM_SIZE);
	DCache_Clean((u32)&PIX_DATA1[0], MEM_SIZE);
	DCache_Clean((u32)&PIX_DATA2[0], MEM_SIZE);
}

static void lcdc_irq_handler(void)
{
	volatile u32 IntId;
	static u32 index = 0;
	static u32 counter = 0;

	IntId = LCDC_GetINTStatus(LCDC);
	LCDC_ClearINT(LCDC, IntId);

	RTK_LOGS(NOTAG, RTK_LOG_DEBUG, "irq 0x%x \r\n", IntId);

	if (IntId & LCDC_BIT_LCD_FRD_INTS) {
		RTK_LOGS(NOTAG, RTK_LOG_DEBUG, "intr: frame done \r\n");
	}

	if (IntId & LCDC_BIT_LCD_LIN_INTS) {
		RTK_LOGS(NOTAG, RTK_LOG_DEBUG, "intr: line hit \r\n");

		if (counter++ == 50) {
			counter = 0;

			/* display red green blue cyclically */
			if (index % 3 == 0) {
				LCDC_DMAImgCfg(LCDC, (u32)&PIX_DATA0[0]);
			} else if (index % 3 == 1) {
				LCDC_DMAImgCfg(LCDC, (u32)&PIX_DATA1[0]);
			} else if (index % 3 == 2) {
				LCDC_DMAImgCfg(LCDC, (u32)&PIX_DATA2[0]);
			}

			LCDC_ShadowReloadConfig(LCDC);
			index++;
		}
	}

	if (IntId & LCDC_BIT_DMA_UN_INTS) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "intr: dma udf !!! \r\n");
	}
}

static void lcdc_driver_init(void)
{
	LCDC_RGBInitTypeDef LCDC_RGBInitStruct;

	LCDC_Cmd(LCDC, DISABLE);
	LCDC_RGBStructInit(&LCDC_RGBInitStruct);

	/* set HV para according to lcd spec */
	LCDC_RGBInitStruct.Panel_RgbTiming.RgbVsw = LCD_VSW;//1
	LCDC_RGBInitStruct.Panel_RgbTiming.RgbVbp = LCD_VBP;//4;
	LCDC_RGBInitStruct.Panel_RgbTiming.RgbVfp = LCD_VFP;//6;

	LCDC_RGBInitStruct.Panel_RgbTiming.RgbHsw = LCD_HSW;//4
	LCDC_RGBInitStruct.Panel_RgbTiming.RgbHbp = LCD_HBP;//40
	LCDC_RGBInitStruct.Panel_RgbTiming.RgbHfp = LCD_HFP;//40

	LCDC_RGBInitStruct.Panel_Init.IfWidth = LCDC_RGB_IF_24_BIT;
	LCDC_RGBInitStruct.Panel_Init.ImgWidth = WIDTH;
	LCDC_RGBInitStruct.Panel_Init.ImgHeight = HEIGHT;

	// LCDC_RGBInitStruct.LCDC_RGBSyncMode = LCDC_RGB_HV_MODE;
	LCDC_RGBInitStruct.Panel_RgbTiming.Flags.RgbEnPolar = LCDC_RGB_EN_PUL_HIGH_LEV_ACTIVE;
	LCDC_RGBInitStruct.Panel_RgbTiming.Flags.RgbDclkActvEdge = LCDC_RGB_DCLK_FALLING_EDGE_FETCH;
	LCDC_RGBInitStruct.Panel_RgbTiming.Flags.RgbHsPolar = LCDC_RGB_HS_PUL_LOW_LEV_SYNC;
	LCDC_RGBInitStruct.Panel_RgbTiming.Flags.RgbVsPolar = LCDC_RGB_VS_PUL_LOW_LEV_SYNC;

	LCDC_RGBInitStruct.Panel_Init.InputFormat = LCDC_INPUT_FORMAT_RGB888;//LCDC_INPUT_FORMAT_BGR888;
	LCDC_RGBInitStruct.Panel_Init.OutputFormat = LCDC_OUTPUT_FORMAT_BGR888;
	LCDC_RGBInitStruct.Panel_Init.RGBRefreshFreq = 40;//30//50;

	LCDC_RGBInit(LCDC, &LCDC_RGBInitStruct);

	/* configure DMA burst size */
	LCDC_DMABurstSizeConfig(LCDC, 2);

	/* configure base address */
	LCDC_DMAImgCfg(LCDC, (u32)&PIX_DATA0[0]);

	InterruptRegister((IRQ_FUN)lcdc_irq_handler, gLcdcIrqInfo.IrqNum, (u32)gLcdcIrqInfo.IrqData, gLcdcIrqInfo.IrqPriority);
	InterruptEn(gLcdcIrqInfo.IrqNum, gLcdcIrqInfo.IrqPriority);

	LCDC_LineINTPosConfig(LCDC, LCDC_LINE_NUM_INTR_DEF);
	LCDC_INTConfig(LCDC, LCDC_BIT_LCD_FRD_INTEN | LCDC_BIT_FRM_START_INTEN | LCDC_BIT_DMA_UN_INTEN | LCDC_BIT_LCD_LIN_INTEN, ENABLE);

	/*enable the LCDC*/
	LCDC_Cmd(LCDC, ENABLE);
}

static void lcdc_display_rgb_st7262(void)
{
	/* init lcdc irq info */
	gLcdcIrqInfo.IrqNum = LCDC_IRQ;//49
	gLcdcIrqInfo.IrqPriority = INT_PRI_MIDDLE;
	gLcdcIrqInfo.IrqData = (u32)LCDC;

	/* config pin info */
	lcdc_pinmux_config();

	/* prepare pix data */
	lcdc_fill_framebuf();

	/* enable function and clock */
	LCDC_RccEnable();

	/* register irq handler */
	InterruptRegister((IRQ_FUN)lcdc_irq_handler, gLcdcIrqInfo.IrqNum, NULL, gLcdcIrqInfo.IrqPriority);
	InterruptEn(gLcdcIrqInfo.IrqNum, gLcdcIrqInfo.IrqPriority);

	/* init lcdc driver */
	lcdc_driver_init();

	/* config irq event */
	LCDC_LineINTPosConfig(LCDC, LCDC_LINE_NUM_INTR_DEF);
	LCDC_INTConfig(LCDC, LCDC_BIT_LCD_FRD_INTEN | LCDC_BIT_DMA_UN_INTEN | LCDC_BIT_LCD_LIN_INTEN, ENABLE);

	/* enable lcdc */
	LCDC_Cmd(LCDC, ENABLE);

	while (1);
}

int example_raw_lcdc_display_rgb_st7262(void)
{
	if (rtos_task_create(NULL, "lcdc_display_rgb_st7262", (rtos_task_t)lcdc_display_rgb_st7262, NULL, (3072), (2)) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Cannot create lcdc_display_rgb_st7262 demo task\n\r");
	}

	return 0;
}