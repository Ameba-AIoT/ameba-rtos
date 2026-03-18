#include "ameba_soc.h"
#include "os_wrapper.h"
#include <stdio.h>
#include "bsp_mcu_com.h"
#include "bsp_mcu_ili9806.h"

#define LCD_BLEN_MCU			_PB_3

/* Uncomment this macro for the display scenario. */
#define ALLOC_BUFFER_FOR_DISPLAY

#ifdef ALLOC_BUFFER_FOR_DISPLAY
#define WIDTH						480
#define HEIGHT						800
#define MEM_SIZE					(WIDTH * HEIGHT * 3)		//rgb888
#define LCDC_LINE_NUM_INTR_DEF		(HEIGHT / 2)
static u8 *PIX_DATA0 = (u8 *)0x60000000;

#else /* alloc small buffer for compile issue */

#define WIDTH						50
#define HEIGHT						50
#define MEM_SIZE					(WIDTH * HEIGHT * 3)		//rgb888
#define LCDC_LINE_NUM_INTR_DEF		(HEIGHT / 2)

static u8 PIX_DATA0[MEM_SIZE] __attribute__((aligned(64)));
#endif

static struct LCDC_IRQInfoDef {
	u32 IrqNum;
	u32 IrqData;
	u32 IrqPriority;
} gLcdcIrqInfo;

/* config pinmux and control blen pad */
static void lcdc_pinmux_config(void)
{
	RTK_LOGI(NOTAG, "[LCDC_MCU] lcdc_pinmux_config \r\n");
	/* LCD BLEN Pin for ILI9806.
	high: BL enable; low: BL disable */
	GPIO_InitTypeDef GPIO_InitStruct_BLEN;
	GPIO_InitStruct_BLEN.GPIO_Pin = LCD_BLEN_MCU;
	GPIO_InitStruct_BLEN.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&GPIO_InitStruct_BLEN);

	GPIO_WriteBit(LCD_BLEN_MCU, 1);

	/* LCD Signal for MCU interface */
	Pinmux_Config(_PA_26, PINMUX_FUNCTION_LCD_D0);  /*D0 - B0*/
	Pinmux_Config(_PA_24, PINMUX_FUNCTION_LCD_D1);  /*D1*/
	Pinmux_Config(_PA_23, PINMUX_FUNCTION_LCD_D2);  /*D2*/
	Pinmux_Config(_PA_22, PINMUX_FUNCTION_LCD_D3);  /*D3*/
	Pinmux_Config(_PA_25, PINMUX_FUNCTION_LCD_D4);  /*D4*/
	Pinmux_Config(_PA_29, PINMUX_FUNCTION_LCD_D5);  /*D5*/
	Pinmux_Config(_PB_4, PINMUX_FUNCTION_LCD_D6);   /*D6*/
	Pinmux_Config(_PB_5, PINMUX_FUNCTION_LCD_D7);   /*D7 - B7 */

	Pinmux_Config(_PB_6, PINMUX_FUNCTION_LCD_D8);   /*D8 - G0*/
	Pinmux_Config(_PB_7, PINMUX_FUNCTION_LCD_D9);   /*D9*/
	Pinmux_Config(_PB_8, PINMUX_FUNCTION_LCD_D10);  /*D10*/
	Pinmux_Config(_PB_9, PINMUX_FUNCTION_LCD_D11);  /*D11*/
	Pinmux_Config(_PB_11, PINMUX_FUNCTION_LCD_D12); /*D12*/
	Pinmux_Config(_PB_10, PINMUX_FUNCTION_LCD_D13); /*D13*/
	Pinmux_Config(_PB_16, PINMUX_FUNCTION_LCD_D14); /*D14*/
	Pinmux_Config(_PB_22, PINMUX_FUNCTION_LCD_D15); /*D15 - G7*/

	Pinmux_Config(_PB_23, PINMUX_FUNCTION_LCD_D16); /*D16 - R0*/
	Pinmux_Config(_PB_14, PINMUX_FUNCTION_LCD_D17); /*D17 */
	Pinmux_Config(_PB_12, PINMUX_FUNCTION_LCD_D18); /*D18*/
	Pinmux_Config(_PB_15, PINMUX_FUNCTION_LCD_D19); /*D19*/
	Pinmux_Config(_PB_17, PINMUX_FUNCTION_LCD_D20); /*D20*/
	Pinmux_Config(_PB_21, PINMUX_FUNCTION_LCD_D21); /*D21*/
	Pinmux_Config(_PB_18, PINMUX_FUNCTION_LCD_D22); /*D22*/
	Pinmux_Config(_PA_6, PINMUX_FUNCTION_LCD_D23);  /*D23 - R7*/

	Pinmux_Config(_PA_9, PINMUX_FUNCTION_LCD_MCU_DCX);
	Pinmux_Config(_PA_11, PINMUX_FUNCTION_LCD_MCU_WR);
	Pinmux_Config(_PA_10, PINMUX_FUNCTION_LCD_MCU_RD);
	Pinmux_Config(_PA_7, PINMUX_FUNCTION_LCD_MCU_CSX);

	// Pinmux_Config(_PA_29, PINMUX_FUNCTION_LCD_MCU_VSYNC);
}

static void lcdc_fill_framebuf(void)
{
	u32 i;
	RTK_LOGI(NOTAG, "%s \r\n", __func__);

	for (i = 0; i < MEM_SIZE; i = i + 3) {
		/* buffer0: red */
		PIX_DATA0[i] = 0x00;
		PIX_DATA0[i + 1] = 0x00;
		PIX_DATA0[i + 2] = 0xFF;
	}

	DCache_Clean((u32)&PIX_DATA0[0], MEM_SIZE);
}

static void lcdc_irq_handler(void)
{
	volatile u32 IntId, IntRawStatus;

	IntId = LCDC_GetINTStatus(LCDC);
	IntRawStatus = LCDC_GetRawINTStatus(LCDC);
	LCDC_ClearINT(LCDC, IntId);

	/* for debug usage */
	RTK_LOGS(NOTAG, RTK_LOG_DEBUG, "irq 0x%x raw:0x%x \r\n", IntId, IntRawStatus);

	if (IntId & LCDC_BIT_DMA_UN_INTS) {
		RTK_LOGI(NOTAG, "intr: dma udf !!! \r\n");
	}
}

static void lcd_init_ili9806(void)
{
	/* config ili9806 cmd: setx, sety, write_ram, width, height */
	LCD_Display_Dir(0);

	/* config ili9806 to 24-bit mode */
	ILI9806_IFbitmode_Config(ILI9806_MCU_Parallel_24b_0010);

	/* delay is required before send init commands */
	DelayMs(50);

	/* send command-list to initialize lcd */
	ILI9806_Init();

	/* clear lcd to white.
	step1: set window first; step2: send write_ram cmd; step3: send pixel_data */
	LCD_Clear(RGB888_WHITE_MASK);

	/* read bpp in mcu io mode */
	u32 pix = LCD_ReadPixelFormat(0x0C);
	RTK_LOGI(NOTAG, "[%s] 0x%x bpp (0x7/0x77: 24bpp)\r\n", __func__, pix);
}

static void lcdc_driver_init(void)
{
	RTK_LOGI(NOTAG, "lcdc_driver_init for ili9806 \r\n");

	LCDC_MCUInitTypeDef LCDC_MCUInitStruct;
	LCDC_MCUStructInit(&LCDC_MCUInitStruct);

	LCDC_MCUInitStruct.Panel_Init.IfWidth = LCDC_MCU_IF_24_BIT;//LCDC_MCU_IF_8_BIT
	LCDC_MCUInitStruct.Panel_Init.ImgWidth = WIDTH;
	LCDC_MCUInitStruct.Panel_Init.ImgHeight = HEIGHT;
	LCDC_MCUInitStruct.Panel_Init.InputFormat = LCDC_INPUT_FORMAT_RGB888;
	LCDC_MCUInitStruct.Panel_Init.OutputFormat = LCDC_OUTPUT_FORMAT_RGB888;

	/* KD043WVFBA085 */
	LCDC_MCUInitStruct.Panel_McuTiming.McuRdPolar = LCDC_MCU_RD_PUL_RISING_EDGE_FETCH;
	LCDC_MCUInitStruct.Panel_McuTiming.McuWrPolar = LCDC_MCU_WR_PUL_RISING_EDGE_FETCH;
	LCDC_MCUInitStruct.Panel_McuTiming.McuRsPolar = LCDC_MCU_RS_PUL_LOW_LEV_CMD_ADDR;
	LCDC_MCUInitStruct.Panel_McuTiming.McuTePolar = LCDC_MCU_TE_PUL_HIGH_LEV_ACTIVE;	/* workaround for G2 a-cut MCU-TE irq issue */
	LCDC_MCUInitStruct.Panel_McuTiming.McuSyncPolar = LCDC_MCU_VSYNC_PUL_LOW_LEV_ACTIVE;
	LCDC_MCUInit(LCDC, &LCDC_MCUInitStruct);

	/* enable the LCDC */
	LCDC_Cmd(LCDC, ENABLE);
}

static void lcdc_display_mcu_ili9806(void)
{
	u8 write_cmd;
	u32 index, test_cnt = 0;
	u8 *pbuf8 = NULL;
	u32 RGBBuf[3] = {RGB888_RED_MASK, RGB888_GREEN_MASK, RGB888_BLUE_MASK};

	/* wait other appilcations bring-up */
	rtos_time_delay_ms(5000);

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

	/* init lcdc to io mode */
	lcdc_driver_init();

	/* init lcd for 24-bit mode and send init-commands */
	lcd_init_ili9806();

	/* set pre-cmd: write ram */
	write_cmd = lcd_dev.wramcmd;		//0x2C
	RTK_LOGI(NOTAG, "write_cmd:0x%x \r\n", write_cmd);
	LCDC_MCUSetPreCmd(LCDC, &write_cmd, 1);

	/* switch to auto/trigger DMA mode */
	LCDC_Cmd(LCDC, DISABLE);

	Lcdc_McuDmaCfgDef LCDC_MCUDmaCfgStruct;
	LCDC_MCUDmaCfgStruct.TeMode = 0;	/* DISABLE */
	LCDC_MCUDmaCfgStruct.TriggerDma = LCDC_TRIGGER_DMA_MODE;
	LCDC_MCUDmaMode(LCDC, &LCDC_MCUDmaCfgStruct);

	LCDC_DMABurstSizeConfig(LCDC, LCDC_DMA_BURSTSIZE_4X64BYTES);
	LCDC_DMAImgCfg(LCDC, (u32)PIX_DATA0);

	/* register irq handler */
	InterruptRegister((IRQ_FUN)lcdc_irq_handler, gLcdcIrqInfo.IrqNum, NULL, gLcdcIrqInfo.IrqPriority);
	InterruptEn(gLcdcIrqInfo.IrqNum, gLcdcIrqInfo.IrqPriority);

	LCDC_LineINTPosConfig(LCDC, LCDC_LINE_NUM_INTR_DEF);
	LCDC_INTConfig(LCDC, LCDC_BIT_LCD_FRD_INTEN | LCDC_BIT_DMA_UN_INTEN | LCDC_BIT_LCD_LIN_INTEN, ENABLE);

	LCDC_Cmd(LCDC, ENABLE);

	pbuf8 = (u8 *)PIX_DATA0;
	while (1) {
		rtos_time_delay_ms(500);

		index = test_cnt % 3;

		RTK_LOGI(NOTAG, "pbuf8:0x%x index:%lu \r\n", pbuf8, index);

		for (u32 i = 0; i < (MEM_SIZE); i += 3) { //RGB888
			pbuf8[i] = (u8)(RGBBuf[index] & 0xFF);
			pbuf8[i + 1] = (u8)(((RGBBuf[index]) >> 8) & 0xFF);
			pbuf8[i + 2] = (u8)(((RGBBuf[index]) >> 16) & 0xFF);
		}
		DCache_Clean((u32)pbuf8, MEM_SIZE);

		LCDC_MCUDMATrigger(LCDC);
		test_cnt++;
	}
}

int example_raw_lcdc_mcu_ili9806(void)
{
	if (rtos_task_create(NULL, "lcdc_display_mcu_ili9806", (rtos_task_t)lcdc_display_mcu_ili9806, NULL, (3072), (2)) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Cannot create lcdc_display_mcu_ili9806 demo task\n\r");
	}

	return 0;
}