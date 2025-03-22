#include "ameba_soc.h"
#include "os_wrapper.h"
#include <stdio.h>

#define WIDTH		50
#define HEIGHT		50
#define MEM_SIZE	WIDTH * HEIGHT * 3		  //rgb888

#define RED			0xFF0000
#define GREEN		0x00FF00
#define BLUE		0x0000FF

/* SH8601A QSPI Instruction Code */
#define SH8601A_QSPI_INST_CMD_WRITE                     (0x02)
#define SH8601A_QSPI_INST_CMD_READ                      (0x03)
#define SH8601A_QSPI_INST_4WIRE_PIXEL_WRITE             (0x32)
#define SH8601A_QSPI_SEQ_FINISH_CODE                    (0x00)

typedef struct {
	u8 cmd;
	u8 addr;
	u16 delay;
	u16 wordcount;
	u8 payload[100];
} SH8601A_CMD_DESC;



static const SH8601A_CMD_DESC SH8601A_INIT_SEQ_CMD[] = {
	{SH8601A_QSPI_INST_CMD_WRITE, 0xC0, 1,  2,  {0x5A, 0x5A}},
	{SH8601A_QSPI_INST_CMD_WRITE, 0xC1, 1,  2,  {0x5A, 0x5A}},
	{SH8601A_QSPI_INST_CMD_WRITE, 0x11, 10, 0,  {0}},
	{SH8601A_QSPI_INST_CMD_WRITE, 0x2A, 1,  4,  {0x00, 0x00, 0x01, 0xC5}},
	{SH8601A_QSPI_INST_CMD_WRITE, 0x2B, 1,  4,  {0x00, 0x00, 0x01, 0xC5}},
	{SH8601A_QSPI_INST_CMD_WRITE, 0x44, 1,  2,  {0x01, 0xC2}},
	{SH8601A_QSPI_INST_CMD_WRITE, 0x35, 1,  1,  {0x00}},
	{SH8601A_QSPI_INST_CMD_WRITE, 0xB0, 1,  1,  {0x16}},
	{SH8601A_QSPI_INST_CMD_WRITE, 0xB1, 1,  9,  {0x01, 0x05, 0x00, 0xA2, 0x00, 0xA7, 0x00, 0xA7, 0x00}},   // 0x01=45Hz, 0x00=60Hz

	{SH8601A_QSPI_INST_CMD_WRITE, 0x53, 1,  1,  {0x28}},
	{SH8601A_QSPI_INST_CMD_WRITE, 0xC4, 25, 1,  {0x84}},
	{SH8601A_QSPI_INST_CMD_WRITE, 0x29, 1,  0,  {0}},
	//{SH8601A_QSPI_INST_CMD_WRITE, 0xBA, 1,  1,  {0x81}},         // bist: 0x81,      exit bist: 0x80
	{SH8601A_QSPI_INST_CMD_WRITE, 0xB1, 1,  1,  {0xC0}},

	{SH8601A_QSPI_INST_CMD_WRITE, 0xC0, 1,  2,  {0xA5, 0xA5}},
	{SH8601A_QSPI_INST_CMD_WRITE, 0xC1, 1,  2,  {0xA5, 0xA5}},
//	{SH8601A_QSPI_INST_CMD_WRITE, 0x23, 1,  0,  {0}},

	{SH8601A_QSPI_SEQ_FINISH_CODE,   0, 0,  0,  {0}},
};


static u8 PIX_DATA[MEM_SIZE] __attribute__((aligned(64)));


/* PPE use as a DMA */
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
	PPE_Input_Layer1.src_addr 					= (u32)PIX_DATA;
	PPE_Input_Layer1.start_x					= 0;
	PPE_Input_Layer1.start_y					= 0;
	PPE_Input_Layer1.width						= WIDTH;
	PPE_Input_Layer1.height						= HEIGHT;
	PPE_Input_Layer1.const_ABGR8888_value		= 0xFFFFFFFF;
	PPE_Input_Layer1.format						= PPE_RGB888;
	PPE_Input_Layer1.src						= PPE_LAYER_SRC_FROM_DMA;
	PPE_Input_Layer1.color_key_en				= DISABLE;
	PPE_Input_Layer1.line_len					= PPE_Input_Layer1.width;
	PPE_Input_Layer1.key_color_value			= 0;
	PPE_InitInputLayer(1, &PPE_Input_Layer1);

	/*initial result layer*/
	PPE_Result_Layer.src_addr					= (u32) & (QSPI->DR[0].BYTE);
	PPE_Result_Layer.width						= WIDTH;
	PPE_Result_Layer.height						= HEIGHT;
	PPE_Result_Layer.format 					= PPE_RGB888;
	PPE_Result_Layer.line_len					= PPE_Result_Layer.width;
	PPE_Result_Layer.type						= PPE_ADDR_QSPI;
	PPE_InitResultLayer(&PPE_Result_Layer);

	/*initial PPE global*/
	PPE_Init_User.function						= PPE_FUNCTION_ALPHA_BLEND;
	PPE_Init_User.blend_layer_num 				= 1;
	PPE_Init(&PPE_Init_User);
}


static void qspi_display_with_ppe_sh8601a(void *para)
{
	UNUSED(para);
	int i = 0;
	int index = 0;
	u8 data[4];
	QSPI_CmdAddrInfo info;

	/* pinmux config */
	Pinmux_Config(_PA_12, PINMUX_FUNCTION_QSPI);	//OSPI_CLK
	Pinmux_Config(_PA_24, PINMUX_FUNCTION_QSPI);	//OSPI_CSN
	Pinmux_Config(_PA_25, PINMUX_FUNCTION_QSPI);	//OSPI_DC_SEL
	Pinmux_Config(_PA_26, PINMUX_FUNCTION_QSPI);	//OSPI_D0
	Pinmux_Config(_PA_27, PINMUX_FUNCTION_QSPI);	//OSPI_D1
	Pinmux_Config(_PB_6, PINMUX_FUNCTION_QSPI);		//OSPI_D2
	Pinmux_Config(_PB_7, PINMUX_FUNCTION_QSPI);		//OSPI_D3

	/* enable function and clk */
	RCC_PeriphClockCmd(APBPeriph_QSPI, APBPeriph_QSPI_CLOCK, ENABLE);

	/* Reset LCD */
	GPIO_InitTypeDef LCD_RST;
	LCD_RST.GPIO_Mode = GPIO_Mode_OUT;
	LCD_RST.GPIO_Pin = _PA_22;
	GPIO_Init(&LCD_RST);

	GPIO_WriteBit(_PA_22, 1);
	DelayMs(1);
	GPIO_WriteBit(_PA_22, 0);
	DelayMs(10);
	GPIO_WriteBit(_PA_22, 1);
	DelayMs(10);

	/* Init QSPI */
	QSPI_Init();

	/* Set QSPI parameter */
	QSPI_StructInit(&info);
	info.cmd_length = 1;
	info.addr_length = 3;
	info.cmd_ch = 0;
	info.addr_ch = 0;
	info.data_ch = 0;
	info.cmd[0] = 	SH8601A_QSPI_INST_CMD_WRITE;
	info.addr[0] = 0;
	info.addr[2] = 0;
	info.rx_dummy_cycle = 1;
	info.ddr_en = 0;

	/* LCD init flow */
	while (SH8601A_INIT_SEQ_CMD[i].cmd != SH8601A_QSPI_SEQ_FINISH_CODE) {
		info.addr[1] = SH8601A_INIT_SEQ_CMD[i].addr;
		QSPI_Write(&info, (u8 *) SH8601A_INIT_SEQ_CMD[i].payload, SH8601A_INIT_SEQ_CMD[i].wordcount);
		DelayMs(SH8601A_INIT_SEQ_CMD[i].delay);
		i++;
	}

	/* LCD display */
	while (1) {
		/* Prepare rgb data */
		if (index % 3 == 0) {
			for (i = 0; i < MEM_SIZE; i = i + 3) { //RED
				PIX_DATA[i] = 0xFF;
				PIX_DATA[i + 1] = 0x00;
				PIX_DATA[i + 2] = 0x00;
			}
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "Red\n");
		} else if (index % 3 == 1) {
			for (i = 0; i < MEM_SIZE; i = i + 3) { //GREEN
				PIX_DATA[i] = 0x00;
				PIX_DATA[i + 1] = 0xFF;
				PIX_DATA[i + 2] = 0x00;
			}
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "Green\n");
		} else {
			for (i = 0; i < MEM_SIZE; i = i + 3) { //BLUE
				PIX_DATA[i] = 0x00;
				PIX_DATA[i + 1] = 0x00;
				PIX_DATA[i + 2] = 0xFF;
			}
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "Blue\n");
		}
		index++;

		DCache_Clean((u32) PIX_DATA, MEM_SIZE);

		/* PPE use as a DMA */
		Prepare_PPE();

		/* Set window to (0, 0, 453, 453)*/
		data[0] = 0 >> 8;
		data[1] = 0 & 0xff;
		data[2] = (WIDTH - 1) >> 8;
		data[3] = (WIDTH - 1) & 0xff;
		info.cmd[0] = 	SH8601A_QSPI_INST_CMD_WRITE;
		info.addr[1] = 0x2A;
		info.data_ch = 0;
		QSPI_Write(&info, data, 4);
		data[0] = 0 >> 8;
		data[1] = 0 & 0xff;
		data[2] = (HEIGHT - 1) >> 8;
		data[3] = (HEIGHT - 1) & 0xff;
		info.cmd[0] = 	SH8601A_QSPI_INST_CMD_WRITE;
		info.addr[1] = 0x2B;
		info.data_ch = 0;
		QSPI_Write(&info, data, 4);

		/* Write pixel */
		info.data_ch = 2;
		info.cmd[0] = SH8601A_QSPI_INST_4WIRE_PIXEL_WRITE;
		info.addr[1] = 0x2C;
		QSPI_WriteStart(&info, MEM_SIZE);
		PPE_Cmd(ENABLE);

		/* Wait for DMA done */
		DelayMs(500);
	}
}

int example_raw_qspi_display_with_ppe_sh8601a(void)
{
	if (rtos_task_create(NULL, "qspi_display_with_ppe_sh8601a", (rtos_task_t)qspi_display_with_ppe_sh8601a, NULL, (3072), (1)) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Cannot create qspi_display_with_ppe_sh8601a demo task\n\r");
	}

	return 0;
}