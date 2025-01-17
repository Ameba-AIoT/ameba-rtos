#include "ameba_soc.h"
#include "ameba_qspi.h"
#include "os_wrapper.h"
#include <stdio.h>

#define WIDTH		50
#define HEIGHT		50
#define MEM_SIZE	WIDTH * HEIGHT * 2		//rgb565

#define RED			0x00F8
#define GREEN		0xE007
#define BLUE		0x1F00


static u8 LCD_DATA[MEM_SIZE] __attribute__((aligned(64)));
static QSPI_CmdAddrInfo info;
static u8 ili9341_buf[100];
static u32 ili9341_buf_len = 0;

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

static void ili9341_init(void)
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


static void Prepare_PPE(void)
{
	/*declaration of input/result layer initialization struct*/
	PPE_InputLayer_InitTypeDef 		PPE_Input_Layer1;
	PPE_ResultLayer_InitTypeDef		PPE_Result_Layer;
	PPE_InitTypeDef								PPE_Init_User;

	PPE_InputLayer_StructInit(&PPE_Input_Layer1);
	PPE_ResultLayer_StructInit(&PPE_Result_Layer);
	PPE_StructInit(&PPE_Init_User);

	/*initial input layer1*/
	PPE_Input_Layer1.src_addr 					= (u32)LCD_DATA;
	PPE_Input_Layer1.start_x					= 0;
	PPE_Input_Layer1.start_y					= 0;
	PPE_Input_Layer1.width						= WIDTH;
	PPE_Input_Layer1.height						= HEIGHT;
	PPE_Input_Layer1.const_ABGR8888_value		= 0xFFFFFFFF;
	PPE_Input_Layer1.format						= PPE_RGB565;
	PPE_Input_Layer1.src						= PPE_LAYER_SRC_FROM_DMA;
	PPE_Input_Layer1.color_key_en				= DISABLE;
	PPE_Input_Layer1.line_len					= PPE_Input_Layer1.width;
	PPE_Input_Layer1.key_color_value			= 0;
	PPE_InitInputLayer(1, &PPE_Input_Layer1);

	/*initial result layer*/
	PPE_Result_Layer.src_addr					= (u32) & (QSPI->DR[0].BYTE);
	PPE_Result_Layer.width						= WIDTH;
	PPE_Result_Layer.height						= HEIGHT;
	PPE_Result_Layer.format 					= PPE_RGB565;
	PPE_Result_Layer.line_len					= PPE_Result_Layer.width;
	PPE_Result_Layer.type						= PPE_ADDR_QSPI;
	PPE_InitResultLayer(&PPE_Result_Layer);

	/*initial PPE global*/
	PPE_Init_User.function						= PPE_FUNCTION_ALPHA_BLEND;
	PPE_Init_User.blend_layer_num 				= 1;
	PPE_Init(&PPE_Init_User);
}


static void qspi_display_ili9341_ppe(void *para)
{
	UNUSED(para);
	int index = 0, i;
	GPIO_InitTypeDef LCD_RST;
	u8 data[4];

	Pinmux_Config(_PA_19, PINMUX_FUNCTION_QSPI);	//OSPI_CLK
	Pinmux_Config(_PA_20, PINMUX_FUNCTION_QSPI);	//OSPI_CSN
	Pinmux_Config(_PA_21, PINMUX_FUNCTION_QSPI);	//OSPI_DC_SEL
	Pinmux_Config(_PB_6, PINMUX_FUNCTION_QSPI);	//OSPI_D0

	RCC_PeriphClockCmd(APBPeriph_QSPI, APBPeriph_QSPI_CLOCK, ENABLE);

	HAL_WRITE32(SYSTEM_CTRL_BASE, 0x2CC, HAL_READ32(SYSTEM_CTRL_BASE, 0x2CC) & (~BIT1));

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
	ili9341_init();

	u16 *LCD_DATA1 = (u16 *) LCD_DATA;
	while (1) {
		if (index % 3 == 0) {
			for (i = 0; i < MEM_SIZE / 2; i++) {
				LCD_DATA1[i] = RED;
			}
		} else if (index % 3 == 1) {
			for (i = 0; i < MEM_SIZE / 2; i++) {
				LCD_DATA1[i] = GREEN;
			}
		} else {
			for (i = 0; i < MEM_SIZE / 2; i++) {
				LCD_DATA1[i] = BLUE;
			}
		}
		index++;

		DCache_Clean((u32) LCD_DATA, MEM_SIZE);

		/* PPE use as a DMA */
		Prepare_PPE();

		/* Set window to (0, 0, 240, 320)*/
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

		/* Write pixel */
		info.cmd[0] = 0x2C;
		QSPI_WriteStart(&info, MEM_SIZE);
		PPE_Cmd(ENABLE);

		DelayMs(500);

	}
}

int example_raw_spi_display_with_ppe_ili9341(void)
{
	if (rtos_task_create(NULL, "qspi_display_ili9341_ppe", (rtos_task_t)qspi_display_ili9341_ppe, NULL, (2048), (1)) != SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Cannot create qspi_display_ili9341_ppe demo task\n\r");
	}

	return 0;
}

