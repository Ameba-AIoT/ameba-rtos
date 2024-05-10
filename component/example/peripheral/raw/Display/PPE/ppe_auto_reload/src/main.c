#include "ameba_soc.h"
#include "os_wrapper.h"
#include <stdio.h>


#define WIDTH		50
#define HEIGHT		50
#define MEM_SIZE	WIDTH * HEIGHT * 4		  //ARGB888


static u8 SOURCE_PIC_1[MEM_SIZE] __attribute__((aligned(64)));
static u8 SOURCE_PIC_2[MEM_SIZE] __attribute__((aligned(64)));
static u8 SOURCE_PIC_3[MEM_SIZE] __attribute__((aligned(64)));
static u8 PIC_OUTPUT_LAYER[MEM_SIZE] __attribute__((aligned(64)));

static void Initialize_Array_with_Random(uint8_t *source_address, u32 sizeof_array)
{
	for (u32 counter = 0; counter < sizeof_array; counter++) {
		uint8_t byte_temp = (uint8_t)(_rand() % 0xFF);
		source_address[counter] = byte_temp;
	}
}

static u32 PPE_Handler(void *Data)
{
	u32 status;
	UNUSED(Data);

	status = PPE_GetAllIntStatus();
	if (status & PPE_LOAD_OVER_INT) {
		printf("Load over interrupt occurs\n");
		PPE_ClearINTPendingBit(PPE_LOAD_OVER_INT);
	}

	if (status & PPE_FR_OVER_INT) {
		printf("Frame over interrupt occurs\n");
		PPE_ClearINTPendingBit(PPE_FR_OVER_INT);
	}

	if (status & PPE_ALL_OVER_INT) {
		printf("all frame over interrupt occurs\n");
		printf("auto reload over\n");
		DCache_Invalidate((u32)PIC_OUTPUT_LAYER, sizeof(PIC_OUTPUT_LAYER));
		PPE_ClearINTPendingBit(PPE_ALL_OVER_INT);
		PPE_MaskAllInt();
	}

	if (status & (~(PPE_LOAD_OVER_INT | PPE_ALL_OVER_INT | PPE_FR_OVER_INT))) {
		printf("wrong interrupt status: %x\n", (int) status);
	}

	return 0;
}


static void ppe_auto_reload(void *para)
{
	UNUSED(para);
	/*declaration of input/result layer initialization struct*/
	PPE_InputLayer_InitTypeDef 		PPE_Input_Layer1;
	PPE_InputLayer_InitTypeDef 		PPE_Input_Layer2;
	PPE_InputLayer_InitTypeDef 		PPE_Input_Layer3;
	PPE_ResultLayer_InitTypeDef		PPE_Result_Layer;
	PPE_InitTypeDef								PPE_Init_User;

	/* enable function and clk */
	RCC_PeriphClockCmd(APBPeriph_QSPI, APBPeriph_QSPI_CLOCK, ENABLE);

	/*prepare for source data with random value*/
	Initialize_Array_with_Random(SOURCE_PIC_1, sizeof(SOURCE_PIC_1));
	Initialize_Array_with_Random(SOURCE_PIC_2, sizeof(SOURCE_PIC_2));
	Initialize_Array_with_Random(SOURCE_PIC_3, sizeof(SOURCE_PIC_3));

	DCache_Clean((u32)SOURCE_PIC_1, sizeof(SOURCE_PIC_1));
	DCache_Clean((u32)SOURCE_PIC_2, sizeof(SOURCE_PIC_2));
	DCache_Clean((u32)SOURCE_PIC_3, sizeof(SOURCE_PIC_3));

	PPE_InputLayer_StructInit(&PPE_Input_Layer1);
	PPE_InputLayer_StructInit(&PPE_Input_Layer2);
	PPE_InputLayer_StructInit(&PPE_Input_Layer3);
	PPE_ResultLayer_StructInit(&PPE_Result_Layer);
	PPE_StructInit(&PPE_Init_User);

	/*initial input layer1*/
	PPE_Input_Layer1.src_addr 								= (u32)SOURCE_PIC_1;
	PPE_Input_Layer1.start_x								= 0;
	PPE_Input_Layer1.start_y								= 0;
	PPE_Input_Layer1.width									= WIDTH;
	PPE_Input_Layer1.height									= HEIGHT;
	PPE_Input_Layer1.const_ABGR8888_value					= 0xFFFFFFFF;
	PPE_Input_Layer1.format									= PPE_ARGB8888;
	PPE_Input_Layer1.src									= PPE_LAYER_SRC_FROM_DMA;
	PPE_Input_Layer1.color_key_en							= DISABLE;
	PPE_Input_Layer1.line_len								= PPE_Input_Layer1.width;
	PPE_Input_Layer1.key_color_value						= 0;
	PPE_InitInputLayer(1, &PPE_Input_Layer1);

	/*initial input layer2*/
	PPE_Input_Layer2.src_addr 								= (u32)SOURCE_PIC_2;
	PPE_Input_Layer2.start_x								= 0;
	PPE_Input_Layer2.start_y								= 0;
	PPE_Input_Layer2.width									= WIDTH;
	PPE_Input_Layer2.height									= HEIGHT;
	PPE_Input_Layer2.const_ABGR8888_value					= 0xFFFFFFFF;
	PPE_Input_Layer2.format									= PPE_ARGB8888;
	PPE_Input_Layer2.src									= PPE_LAYER_SRC_FROM_DMA;
	PPE_Input_Layer2.color_key_en							= DISABLE;
	PPE_Input_Layer2.line_len								= PPE_Input_Layer2.width;
	PPE_Input_Layer2.key_color_value						= 0;
	PPE_InitInputLayer(2, &PPE_Input_Layer2);

	/*initial input layer3*/
	PPE_Input_Layer3.src_addr 								= (u32)SOURCE_PIC_3;
	PPE_Input_Layer3.start_x								= 0;
	PPE_Input_Layer3.start_y								= 0;
	PPE_Input_Layer3.width									= WIDTH;
	PPE_Input_Layer3.height									= HEIGHT;
	PPE_Input_Layer3.const_ABGR8888_value					= 0xFFFFFFFF;
	PPE_Input_Layer3.format									= PPE_ARGB8888;
	PPE_Input_Layer3.src									= PPE_LAYER_SRC_FROM_DMA;
	PPE_Input_Layer3.color_key_en							= DISABLE;
	PPE_Input_Layer3.line_len								= PPE_Input_Layer3.width;
	PPE_Input_Layer3.key_color_value						= 0;
	PPE_InitInputLayer(3, &PPE_Input_Layer3);

	/*initial result layer*/
	PPE_Result_Layer.src_addr					= (u32)PIC_OUTPUT_LAYER;
	PPE_Result_Layer.width						= WIDTH;
	PPE_Result_Layer.height						= HEIGHT;
	PPE_Result_Layer.format 					= PPE_RGB888;
	PPE_Result_Layer.line_len					= PPE_Result_Layer.width;
	PPE_Result_Layer.type 						= PPE_ADDR_MEM;
	PPE_InitResultLayer(&PPE_Result_Layer);

	/*initial PPE global*/
	PPE_Init_User.function				= PPE_FUNCTION_ALPHA_BLEND;
	PPE_Init_User.blend_layer_num		= 3;
	PPE_Init(&PPE_Init_User);

	/* Result layer and all 3 input layers are involved in the auto reload for the all following frames */
	PPE_AutoReloadSetLayer(PPE_RESULT_LAYER_BIT | PPE_INPUT_LAYER1_BIT | PPE_INPUT_LAYER2_BIT | PPE_INPUT_LAYER3_BIT);
	PPE_AutoClearCmd(ENABLE);

	InterruptRegister(PPE_Handler, PPE_IRQ, NULL, 4);
	InterruptEn(PPE_IRQ, 4);
	PPE_MaskINTConfig(PPE_LOAD_OVER_INT | PPE_FR_OVER_INT | PPE_ALL_OVER_INT, ENABLE);

	/* enable PPE to start first frame */
	PPE_Cmd(ENABLE);

	DelayMs(100);

	/* change the configuration of input layer1 for the second frame, include start position and
	window size and const alpha value */
	PPE_Input_Layer1.start_x									= 20;
	PPE_Input_Layer1.start_y									= 20;
	PPE_Input_Layer1.width										= 30;
	PPE_Input_Layer1.height										= 30;
	PPE_Input_Layer1.const_ABGR8888_value						= 0XAAFFFFFF;
	PPE_Input_Layer1.line_len									= PPE_Input_Layer1.width;
	/* Polling the valid value to check if layer regs load over for the first frame */
	while (PPE_GetValid() == 1);
	PPE_InitInputLayer(1, &PPE_Input_Layer1);
	printf("Here we set set_valid manually for the second frame\n");
	PPE_SetValid();

	DelayMs(100);
	/* change the configuration of input layer2 for the third frame, include start position and
	window size and const alpha value */
	PPE_Input_Layer2.start_x									= 10;
	PPE_Input_Layer2.start_y									= 10;
	PPE_Input_Layer2.width										= 25;
	PPE_Input_Layer2.height										= 25;
	PPE_Input_Layer2.const_ABGR8888_value						= 0XEEFFFFFF;
	PPE_Input_Layer2.line_len									= PPE_Input_Layer2.width;
	printf("Here we set set_valid manually for the third frame\n");
	while (PPE_GetValid() == 1);
	PPE_InitInputLayer(2, &PPE_Input_Layer2);
	PPE_SetValid();

	DelayMs(100);
	/* change the configuration of input layer3 for the forth frame, include start position and
	window size and const alpha value */
	PPE_Input_Layer3.start_x									= 10;
	PPE_Input_Layer3.start_y									= 10;
	PPE_Input_Layer3.width										= 30;
	PPE_Input_Layer3.height										= 20;
	PPE_Input_Layer3.const_ABGR8888_value						= 0XC1FFFFFF;
	PPE_Input_Layer3.line_len									= PPE_Input_Layer3.width;
	/* Stop auto reload */
	PPE_AutoReloadClrLayerAll();
	printf("Here we set set_valid manually for the forth frame\n");
	while (PPE_GetValid() == 1);
	PPE_InitInputLayer(3, &PPE_Input_Layer3);
	PPE_SetValid();

	while (1);
}

int main(void)
{
	if (rtos_task_create(NULL, "ppe_auto_reload", (rtos_task_t)ppe_auto_reload, NULL, (2048), (1)) != SUCCESS) {
		printf("Cannot create ppe_auto_reload demo task\n\r");
	}

	rtos_sched_start();
}

