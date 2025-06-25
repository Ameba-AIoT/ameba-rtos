#include "ameba_soc.h"
#include "ameba_qspi.h"
#include "os_wrapper.h"
#include "ili9341.h"

#define WIDTH		    240
#define HEIGHT		    320
#define PPE_MEM_SIZE	WIDTH * HEIGHT * 2		//rgb565


static QSPI_CmdAddrInfo info;
u8 ili9341_buf[100];
u32 ili9341_buf_len = 0;

static ILI9341VBlankCallback *g_callback = NULL;
static void *g_data = NULL;

static void LCD_SPI_WR_REG(u8 cmd)
{
	info.cmd[0] = cmd;
	ili9341_buf_len = 0;
}

static void LCD_SPI_WR_DATA(u8 para)
{
	ili9341_buf[ili9341_buf_len] = para;
	ili9341_buf_len++ ;
}

static void ili9341_config_init(void)
{
	LCD_SPI_WR_REG(0xCF);
	LCD_SPI_WR_DATA(0x00);
	LCD_SPI_WR_DATA(0xD9);
	LCD_SPI_WR_DATA(0X30);
	QSPI_Write(&info, ili9341_buf, ili9341_buf_len);

	LCD_SPI_WR_REG(0xED);
	LCD_SPI_WR_DATA(0x64);
	LCD_SPI_WR_DATA(0x03);
	LCD_SPI_WR_DATA(0X12);
	LCD_SPI_WR_DATA(0X81);
	QSPI_Write(&info, ili9341_buf, ili9341_buf_len);

	LCD_SPI_WR_REG(0xE8);
	LCD_SPI_WR_DATA(0x85);
	LCD_SPI_WR_DATA(0x10);
	LCD_SPI_WR_DATA(0x78);
	QSPI_Write(&info, ili9341_buf, ili9341_buf_len);

	LCD_SPI_WR_REG(0xCB);
	LCD_SPI_WR_DATA(0x39);
	LCD_SPI_WR_DATA(0x2C);
	LCD_SPI_WR_DATA(0x00);
	LCD_SPI_WR_DATA(0x34);
	LCD_SPI_WR_DATA(0x02);
	QSPI_Write(&info, ili9341_buf, ili9341_buf_len);

	LCD_SPI_WR_REG(0xF7);
	LCD_SPI_WR_DATA(0x20);
	QSPI_Write(&info, ili9341_buf, ili9341_buf_len);

	LCD_SPI_WR_REG(0xEA);
	LCD_SPI_WR_DATA(0x00);
	LCD_SPI_WR_DATA(0x00);
	QSPI_Write(&info, ili9341_buf, ili9341_buf_len);

	LCD_SPI_WR_REG(0xC0);
	LCD_SPI_WR_DATA(0x21);
	QSPI_Write(&info, ili9341_buf, ili9341_buf_len);

	LCD_SPI_WR_REG(0xC1);
	LCD_SPI_WR_DATA(0x12);
	QSPI_Write(&info, ili9341_buf, ili9341_buf_len);

	LCD_SPI_WR_REG(0xC5);
	LCD_SPI_WR_DATA(0x32);
	LCD_SPI_WR_DATA(0x3C);
	QSPI_Write(&info, ili9341_buf, ili9341_buf_len);

	LCD_SPI_WR_REG(0xC7);
	LCD_SPI_WR_DATA(0XC1);
	QSPI_Write(&info, ili9341_buf, ili9341_buf_len);

	LCD_SPI_WR_REG(0x36);
	LCD_SPI_WR_DATA(0x08);
	QSPI_Write(&info, ili9341_buf, ili9341_buf_len);

	LCD_SPI_WR_REG(0x3A);
	LCD_SPI_WR_DATA(0x55);
	QSPI_Write(&info, ili9341_buf, ili9341_buf_len);

	LCD_SPI_WR_REG(0xB1);
	LCD_SPI_WR_DATA(0x00);
	LCD_SPI_WR_DATA(0x18);
	QSPI_Write(&info, ili9341_buf, ili9341_buf_len);

	LCD_SPI_WR_REG(0xB6);
	LCD_SPI_WR_DATA(0x0A);
	LCD_SPI_WR_DATA(0xA2);
	QSPI_Write(&info, ili9341_buf, ili9341_buf_len);

	LCD_SPI_WR_REG(0xF2);
	LCD_SPI_WR_DATA(0x00);
	QSPI_Write(&info, ili9341_buf, ili9341_buf_len);

	LCD_SPI_WR_REG(0x26);
	LCD_SPI_WR_DATA(0x01);
	QSPI_Write(&info, ili9341_buf, ili9341_buf_len);

	LCD_SPI_WR_REG(0xE0);
	LCD_SPI_WR_DATA(0x0F);
	LCD_SPI_WR_DATA(0x20);
	LCD_SPI_WR_DATA(0x1E);
	LCD_SPI_WR_DATA(0x09);
	LCD_SPI_WR_DATA(0x12);
	LCD_SPI_WR_DATA(0x0B);
	LCD_SPI_WR_DATA(0x50);
	LCD_SPI_WR_DATA(0XBA);
	LCD_SPI_WR_DATA(0x44);
	LCD_SPI_WR_DATA(0x09);
	LCD_SPI_WR_DATA(0x14);
	LCD_SPI_WR_DATA(0x05);
	LCD_SPI_WR_DATA(0x23);
	LCD_SPI_WR_DATA(0x21);
	LCD_SPI_WR_DATA(0x00);
	QSPI_Write(&info, ili9341_buf, ili9341_buf_len);

	LCD_SPI_WR_REG(0XE1);
	LCD_SPI_WR_DATA(0x00);
	LCD_SPI_WR_DATA(0x19);
	LCD_SPI_WR_DATA(0x19);
	LCD_SPI_WR_DATA(0x00);
	LCD_SPI_WR_DATA(0x12);
	LCD_SPI_WR_DATA(0x07);
	LCD_SPI_WR_DATA(0x2D);
	LCD_SPI_WR_DATA(0x28);
	LCD_SPI_WR_DATA(0x3F);
	LCD_SPI_WR_DATA(0x02);
	LCD_SPI_WR_DATA(0x0A);
	LCD_SPI_WR_DATA(0x08);
	LCD_SPI_WR_DATA(0x25);
	LCD_SPI_WR_DATA(0x2D);
	LCD_SPI_WR_DATA(0x0F);
	QSPI_Write(&info, ili9341_buf, ili9341_buf_len);

	LCD_SPI_WR_REG(0x11);
	QSPI_Write(&info, ili9341_buf, ili9341_buf_len);
	DelayMs(120);
	LCD_SPI_WR_REG(0x29);
	QSPI_Write(&info, ili9341_buf, ili9341_buf_len);
}

/* PPE use as a DMA */
static void Prepare_PPE(uint8_t *buffer)
{
	DCache_CleanInvalidate((u32)buffer, PPE_MEM_SIZE);
	PPE_InputLayer_InitTypeDef PPE_Input_Layer1;
	PPE_InputLayer_StructInit(&PPE_Input_Layer1);
	PPE_Input_Layer1.src_addr = (u32)buffer;
	PPE_Input_Layer1.start_x = 0;
	PPE_Input_Layer1.start_y = 0;
	PPE_Input_Layer1.width = WIDTH;
	PPE_Input_Layer1.height	= HEIGHT;
	PPE_Input_Layer1.const_ABGR8888_value = 0xFFFFFFFF;
	PPE_Input_Layer1.format	= PPE_RGB565;
	PPE_Input_Layer1.src = PPE_LAYER_SRC_FROM_DMA;
	PPE_Input_Layer1.color_key_en = DISABLE;
	PPE_Input_Layer1.line_len = PPE_Input_Layer1.width;
	PPE_Input_Layer1.key_color_value = 0;
	PPE_InitInputLayer(1, &PPE_Input_Layer1);


	PPE_ResultLayer_InitTypeDef PPE_Result_Layer;
	PPE_ResultLayer_StructInit(&PPE_Result_Layer);
	PPE_Result_Layer.src_addr = (u32) & (QSPI->DR[0].BYTE);
	PPE_Result_Layer.width = WIDTH;
	PPE_Result_Layer.height	= HEIGHT;
	PPE_Result_Layer.format = PPE_RGB565;
	PPE_Result_Layer.line_len = PPE_Result_Layer.width;
	PPE_Result_Layer.type = PPE_ADDR_QSPI;
	PPE_InitResultLayer(&PPE_Result_Layer);

	PPE_InitTypeDef PPE_Init_User;
	PPE_StructInit(&PPE_Init_User);
	PPE_Init_User.function = PPE_FUNCTION_ALPHA_BLEND;
	PPE_Init_User.blend_layer_num = 1;
	PPE_Init(&PPE_Init_User);
}

void *PPE_Handler(void)
{
	if (PPE->INT_STATUS & BIT1) {
		PPE->INT_CLR |= BIT1;
		if (g_callback) {
			g_callback->VBlank(g_data);
		}
	} else {
		printf("this is CHN_BUS_ERR interrupt !\r\n");
	}
	return NULL;
}

void ili9341_init(void)
{
	GPIO_InitTypeDef LCD_RST;

	Pinmux_Config(_PA_19, PINMUX_FUNCTION_QSPI);	//OSPI_CLK
	Pinmux_Config(_PA_20, PINMUX_FUNCTION_QSPI);	//OSPI_CSN
	Pinmux_Config(_PA_21, PINMUX_FUNCTION_QSPI);	//OSPI_DC_SEL
	Pinmux_Config(_PB_6, PINMUX_FUNCTION_QSPI);	//OSPI_D0

	RCC_PeriphClockCmd(APBPeriph_QSPI, APBPeriph_QSPI_CLOCK, ENABLE);

	/* reset LCD */
	LCD_RST.GPIO_Mode = GPIO_Mode_OUT;
	LCD_RST.GPIO_Pin = _PA_22;
	GPIO_Init(&LCD_RST);

	GPIO_WriteBit(_PA_22, 1);
	DelayMs(100);
	GPIO_WriteBit(_PA_22, 0);
	DelayMs(100);
	GPIO_WriteBit(_PA_22, 1);
	DelayMs(200);

	/* Init QSPI */
	QSPI_Init();
	QSPI_SetBaud(2);

	/* Set QSPI parameter */
	QSPI_StructInit(&info);
	info.cmd_length = 1;
	info.addr_length = 0;
	info.cmd_ch = 0;
	info.addr_ch = 0;
	info.data_ch = 0;
	info.rx_dummy_cycle = 0;
	info.ddr_en = 0;

	/* Init LCD */
	ili9341_config_init();
	PPE_MaskAllInt();
	PPE_ClearINTPendingBit(PPE_ALL_OVER_INT | PPE_FR_OVER_INT | PPE_LOAD_OVER_INT | PPE_LINE_WL_INT | PPE_SUSP_INAC_INT);

	InterruptRegister(PPE_Handler, PPE_IRQ, NULL, 4);
	InterruptEn(PPE_IRQ, 4);

	PPE_MaskINTConfig(PPE_FR_OVER_INT, ENABLE);
}

void ili9341_get_info(int *width, int *height)
{
	*width = WIDTH;
	*height = HEIGHT;
}

void ili9341_register_callback(ILI9341VBlankCallback *callback, void *data)
{
	g_callback = callback;
	g_data = data;
}

void ili9341_clean_invalidate_buffer(uint8_t *buffer)
{
	Prepare_PPE(buffer);

	/* Set window to (0, 0, 240, 320)*/
	u8 data[4];
	data[0] = 0 >> 8;
	data[1] = 0 & 0xff;
	data[2] = (WIDTH - 1) >> 8;
	data[3] = (WIDTH - 1) & 0xff;
	info.cmd[0] = 0x2A;

	QSPI_Write(&info, data, 4);

	data[0] = 0 >> 8;
	data[1] = 0 & 0xff;
	data[2] = (HEIGHT - 1) >> 8;
	data[3] = (HEIGHT - 1) & 0xff;
	info.cmd[0] = 0x2B;
	QSPI_Write(&info, data, 4);

	info.cmd[0] = 0x2C;
	QSPI_WriteStart(&info, PPE_MEM_SIZE);
	PPE_Cmd(ENABLE);
}

