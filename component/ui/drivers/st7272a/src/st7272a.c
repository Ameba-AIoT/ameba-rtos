/*
 * Copyright (c) 2021 Realtek, LLC.
 * All rights reserved.
 *
 * Licensed under the Realtek License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License from Realtek
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LCD_BLEN_SRGB               _PC_1

#include "st7272a.h"
#include "os_wrapper.h"

static ST7272AVBlankCallback *g_callback = NULL;
static void *g_data = NULL;

#define WIDTH                       320
#define HEIGHT                      240
#define MEM_SIZE                    (WIDTH * HEIGHT * 3)        //rgb888
#define LCDC_LINE_NUM_INTR_DEF      (HEIGHT * 3/ 4)

static u8 *g_buffer = NULL;
static int g_image_format = 0;

static struct LCDC_IRQInfoDef {
	u32 IrqNum;
	u32 IrqData;
	u32 IrqPriority;
} gLcdcIrqInfo;

/* config pinmux and control blen pad */
static void lcdc_pinmux_config(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "%s \r\n", __func__);

	/* LCD BLEN */
	GPIO_InitTypeDef GPIO_InitStruct_BLEN;
	GPIO_InitStruct_BLEN.GPIO_Pin = LCD_BLEN_SRGB;//_PC_1
	GPIO_InitStruct_BLEN.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&GPIO_InitStruct_BLEN);

	GPIO_WriteBit(LCD_BLEN_SRGB, 1);

	/* QAB group */
	Pinmux_Config(_PB_17, PINMUX_FUNCTION_LCD_D0);          /* D0 - B0 */
	Pinmux_Config(_PB_16, PINMUX_FUNCTION_LCD_D1);          /* D1 */
	Pinmux_Config(_PB_15, PINMUX_FUNCTION_LCD_D2);          /* D2 */
	Pinmux_Config(_PA_29, PINMUX_FUNCTION_LCD_D3);          /* D3 */
	Pinmux_Config(_PA_31, PINMUX_FUNCTION_LCD_D4);          /* D4 */
	Pinmux_Config(_PA_30, PINMUX_FUNCTION_LCD_D5);          /* D5 */
	Pinmux_Config(_PA_18, PINMUX_FUNCTION_LCD_D6);          /* D6 */
	Pinmux_Config(_PA_26, PINMUX_FUNCTION_LCD_D7);          /* D7 - B7 */

	Pinmux_Config(_PB_22, PINMUX_FUNCTION_LCD_RGB_HSYNC);   /* HSYNC */
	Pinmux_Config(_PA_27, PINMUX_FUNCTION_LCD_RGB_VSYNC);   /* VSYNC */
	Pinmux_Config(_PB_30, PINMUX_FUNCTION_LCD_RGB_DCLK);    /* DCLK */

	Pinmux_Config(_PA_20, PINMUX_FUNCTION_LCD_RGB_DE);      /* DE */
}

static void lcdc_irq_handler(void)
{
	volatile u32 IntId;

	IntId = LCDC_GetINTStatus(LCDC);
	LCDC_ClearINT(LCDC, IntId);

	RTK_LOGS(NOTAG, RTK_LOG_DEBUG, "irq 0x%x \r\n", IntId);

	if (IntId & LCDC_BIT_LCD_FRD_INTS) {
		RTK_LOGS(NOTAG, RTK_LOG_DEBUG, "intr: frame done \r\n");
	}

	if (IntId & LCDC_BIT_LCD_LIN_INTS) {
		RTK_LOGS(NOTAG, RTK_LOG_DEBUG, "intr: line hit \r\n");
		LCDC_DMAImgCfg(LCDC, (u32)g_buffer);
		LCDC_ShadowReloadConfig(LCDC);
	}

	if (IntId & LCDC_BIT_LCD_LIN_INTEN) {
		if (g_callback) {
			g_callback->VBlank(g_data);
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
	LCDC_RGBInitStruct.Panel_RgbTiming.RgbVsw = 4;
	LCDC_RGBInitStruct.Panel_RgbTiming.RgbVbp = 8;
	LCDC_RGBInitStruct.Panel_RgbTiming.RgbVfp = 8;

	LCDC_RGBInitStruct.Panel_RgbTiming.RgbHsw = 4;
	LCDC_RGBInitStruct.Panel_RgbTiming.RgbHbp = 39;
	LCDC_RGBInitStruct.Panel_RgbTiming.RgbHfp = 8;

	LCDC_RGBInitStruct.Panel_Init.IfWidth = LCDC_RGB_IF_8_BIT;
	LCDC_RGBInitStruct.Panel_Init.ImgWidth = WIDTH;
	LCDC_RGBInitStruct.Panel_Init.ImgHeight = HEIGHT;

	LCDC_RGBInitStruct.Panel_RgbTiming.Flags.RgbEnPolar = LCDC_RGB_EN_PUL_HIGH_LEV_ACTIVE;
	LCDC_RGBInitStruct.Panel_RgbTiming.Flags.RgbDclkActvEdge = LCDC_RGB_DCLK_FALLING_EDGE_FETCH;
	LCDC_RGBInitStruct.Panel_RgbTiming.Flags.RgbHsPolar = LCDC_RGB_HS_PUL_LOW_LEV_SYNC;
	LCDC_RGBInitStruct.Panel_RgbTiming.Flags.RgbVsPolar = LCDC_RGB_VS_PUL_LOW_LEV_SYNC;

	LCDC_RGBInitStruct.Panel_Init.InputFormat = LCDC_INPUT_FORMAT_RGB888;
	LCDC_RGBInitStruct.Panel_Init.OutputFormat = LCDC_OUTPUT_FORMAT_RGB888;
	LCDC_RGBInitStruct.Panel_Init.RGBRefreshFreq = 35;

	LCDC_RGBInit(LCDC, &LCDC_RGBInitStruct);

	/* configure DMA burst size */
	LCDC_DMABurstSizeConfig(LCDC, 2);
}

void st7272a_init(int image_format)
{
	g_image_format = image_format;
	if (g_image_format == ARGB8888) {
		g_buffer = (uint8_t *)malloc(MEM_SIZE);
	}
	/* init lcdc irq info */
	gLcdcIrqInfo.IrqNum = LCDC_IRQ;//49
	gLcdcIrqInfo.IrqPriority = INT_PRI_MIDDLE;
	gLcdcIrqInfo.IrqData = (u32)LCDC;

	/* config pin info */
	lcdc_pinmux_config();

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
}

void convert_argb8888_to_rgb888(uint8_t *argb_buffer, uint8_t *rgb_buffer)
{
	for (int i = 0; i < WIDTH * HEIGHT; i++) {
		int argb_index = i * 4;
		int rgb_index = i * 3;

		uint8_t r = argb_buffer[argb_index];     // Alpha
		uint8_t g = argb_buffer[argb_index + 1]; // Red
		uint8_t b = argb_buffer[argb_index + 2]; // Green
		//uint8_t a = argb_buffer[argb_index + 3]; // Blue

		rgb_buffer[rgb_index] = r;
		rgb_buffer[rgb_index + 1] = g;
		rgb_buffer[rgb_index + 2] = b;
	}
}


void st7272a_clean_invalidate_buffer(u8 *buffer)
{
	if (g_image_format == ARGB8888) {
		convert_argb8888_to_rgb888(buffer, g_buffer);
	} else {
		g_buffer = buffer;
	}
	DCache_Clean((u32)g_buffer, MEM_SIZE);
}

void st7272a_get_info(int *width, int *height)
{
	*width = WIDTH;
	*height = HEIGHT;
}

void st7272a_register_callback(ST7272AVBlankCallback *callback, void *data)
{
	g_callback = callback;
	g_data = data;
}
