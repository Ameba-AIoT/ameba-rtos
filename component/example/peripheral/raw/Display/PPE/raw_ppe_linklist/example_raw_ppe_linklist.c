#include "ameba_soc.h"
#include "os_wrapper.h"
#include <stdio.h>

#define WIDTH		50
#define HEIGHT		50
#define MEM_SIZE	WIDTH * HEIGHT * 4		  //ARGB888

/**
  * User should struture linklist item (lli) according to involved layer.
  * This is a example lli with result layer + 3 input layer involved.
  */
typedef struct {
	PPE_LLI_LAYER layer[4];			//use result layer + 3 input layer
	PPE_LLI_GLB glb;				//global configuration
} lli_user;

static lli_user lli[5] __attribute__((aligned(64)));
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
	if (status & PPE_FR_OVER_INT) {
		PPE_ClearINTPendingBit(PPE_FR_OVER_INT);
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Frame over interrupt occurs\n");
	}

	if (status & PPE_ALL_OVER_INT) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "all frame over interrupt occurs\n");
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "linklist over\n");
		DCache_Invalidate((u32)PIC_OUTPUT_LAYER, sizeof(PIC_OUTPUT_LAYER));
		PPE_ClearINTPendingBit(PPE_ALL_OVER_INT);
		PPE_MaskAllInt();
	}

	if (status & (~(PPE_ALL_OVER_INT | PPE_FR_OVER_INT))) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "wrong interrupt status: %x\n", (int) status);
	}

	return 0;
}



static void ppe_linklist(void *para)
{
	UNUSED(para);
	/*declaration of input/result layer initialization struct*/
	PPE_InputLayer_InitTypeDef 		PPE_Input_Layer1;
	PPE_InputLayer_InitTypeDef 		PPE_Input_Layer2;
	PPE_InputLayer_InitTypeDef 		PPE_Input_Layer3;
	PPE_ResultLayer_InitTypeDef		PPE_Result_Layer;
	PPE_InitTypeDef					PPE_Init_User;

	/* enable function and clk */
	RCC_PeriphClockCmd(APBPeriph_QSPI, APBPeriph_QSPI_CLOCK, ENABLE);

	/*prepare for source data with random value*/
	Initialize_Array_with_Random(SOURCE_PIC_1, sizeof(SOURCE_PIC_1));
	Initialize_Array_with_Random(SOURCE_PIC_2, sizeof(SOURCE_PIC_2));
	Initialize_Array_with_Random(SOURCE_PIC_3, sizeof(SOURCE_PIC_3));
	Initialize_Array_with_Random(PIC_OUTPUT_LAYER, sizeof(PIC_OUTPUT_LAYER));
	DCache_Clean((u32)SOURCE_PIC_1, sizeof(SOURCE_PIC_1));
	DCache_Clean((u32)SOURCE_PIC_2, sizeof(SOURCE_PIC_2));
	DCache_Clean((u32)SOURCE_PIC_3, sizeof(SOURCE_PIC_3));
	DCache_Clean((u32)PIC_OUTPUT_LAYER, sizeof(PIC_OUTPUT_LAYER));

	/**
	  * Actually the following configuration will not work in linklist function.
	  * We do all the following initiliation just to get register value to fill
	  * linklist item (lli).
	  */

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
	PPE_Input_Layer1.format									= PPE_RGB888;
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
	PPE_Input_Layer2.format									= PPE_RGB888;
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
	PPE_Input_Layer3.format									= PPE_RGB888;
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
	PPE_Init_User.blend_layer_num 		= 3;
	PPE_Init(&PPE_Init_User);

	/* set first involved layer and first lli addr */
	PPE_LLSetLayer(PPE_RESULT_LAYER_BIT | PPE_INPUT_LAYER1_BIT | PPE_INPUT_LAYER2_BIT | PPE_INPUT_LAYER3_BIT);
	PPE_SetLLP((u32) & (lli[0]));

	/* fill all lli through PPE register. just for init lli */
	lli[0].layer[0].LAYER_ADDR = PPE_LAYER->RESULT_LAYER.ADDR;
	lli[0].layer[0].LAYER_POS = PPE_LAYER->RESULT_LAYER.POS;
	lli[0].layer[0].LAYER_CONST_PIX = PPE_LAYER->RESULT_LAYER.RSVD2;
	lli[0].layer[0].LAYER_WINSIZE = PPE_LAYER->RESULT_LAYER.WIN_SIZE;
	lli[0].layer[1].LAYER_ADDR = PPE_LAYER->INPUT_LAYER[0].ADDR;
	lli[0].layer[1].LAYER_POS = PPE_LAYER->INPUT_LAYER[0].POS;
	lli[0].layer[1].LAYER_CONST_PIX = PPE_LAYER->INPUT_LAYER[0].CONST_PIX;
	lli[0].layer[1].LAYER_WINSIZE = PPE_LAYER->INPUT_LAYER[0].WIN_SIZE;
	lli[0].layer[2].LAYER_ADDR = PPE_LAYER->INPUT_LAYER[1].ADDR;
	lli[0].layer[2].LAYER_POS = PPE_LAYER->INPUT_LAYER[1].POS;
	lli[0].layer[2].LAYER_CONST_PIX = PPE_LAYER->INPUT_LAYER[1].CONST_PIX;
	lli[0].layer[2].LAYER_WINSIZE = PPE_LAYER->INPUT_LAYER[1].WIN_SIZE;
	lli[0].layer[3].LAYER_ADDR = PPE_LAYER->INPUT_LAYER[2].ADDR;
	lli[0].layer[3].LAYER_POS = PPE_LAYER->INPUT_LAYER[2].POS;
	lli[0].layer[3].LAYER_CONST_PIX = PPE_LAYER->INPUT_LAYER[2].CONST_PIX;
	lli[0].layer[3].LAYER_WINSIZE = PPE_LAYER->INPUT_LAYER[2].WIN_SIZE;
	lli[0].glb.LLP = PPE->LLP;
	lli[0].glb.LL_CFG = PPE->LL_CFG;
	lli[0].glb.SCA_RATIO_X = PPE->SCA_RATIO_X;
	lli[0].glb.SCA_RATIO_Y = PPE->SCA_RATIO_Y;

	_memcpy(&lli[1], &lli[0], sizeof(lli_user));
	_memcpy(&lli[2], &lli[0], sizeof(lli_user));
	_memcpy(&lli[3], &lli[0], sizeof(lli_user));
	_memcpy(&lli[4], &lli[0], sizeof(lli_user));

	/* set each lli to require configuration */

	/* first frame will blendig SOURCE_PIC_1 SOURCE_PIC_2 SOURCE_PIC_3 to PIC_OUTPUT_LAYER*/
	lli[0].layer[0].LAYER_ADDR = (u32)PIC_OUTPUT_LAYER;
	lli[0].layer[1].LAYER_ADDR = (u32)SOURCE_PIC_1;
	lli[0].layer[2].LAYER_ADDR = (u32)SOURCE_PIC_2;
	lli[0].layer[3].LAYER_ADDR = (u32)SOURCE_PIC_3;
	lli[0].glb.LLP = (u32) &lli[1];

	/* second frame will blendig SOURCE_PIC_2 SOURCE_PIC_3 PIC_OUTPUT_LAYER to SOURCE_PIC_1*/
	lli[1].layer[0].LAYER_ADDR = (u32)SOURCE_PIC_1;
	lli[1].layer[1].LAYER_ADDR = (u32)SOURCE_PIC_2;
	lli[1].layer[2].LAYER_ADDR = (u32)SOURCE_PIC_3;
	lli[1].layer[3].LAYER_ADDR = (u32)PIC_OUTPUT_LAYER;
	lli[1].glb.LLP = (u32) &lli[2];

	/* third frame will blendigSOURCE_PIC_3 PIC_OUTPUT_LAYER SOURCE_PIC_1  to SOURCE_PIC_2*/
	lli[2].layer[0].LAYER_ADDR = (u32)SOURCE_PIC_2;
	lli[2].layer[1].LAYER_ADDR = (u32)SOURCE_PIC_3;
	lli[2].layer[2].LAYER_ADDR = (u32)PIC_OUTPUT_LAYER;
	lli[2].layer[3].LAYER_ADDR = (u32)SOURCE_PIC_1;
	lli[2].glb.LLP = (u32) &lli[3];

	/* forth frame will blendig PIC_OUTPUT_LAYERSOURCE_PIC_1 SOURCE_PIC_2 to SOURCE_PIC_3*/
	lli[3].layer[0].LAYER_ADDR = (u32)SOURCE_PIC_3;
	lli[3].layer[1].LAYER_ADDR = (u32)PIC_OUTPUT_LAYER;
	lli[3].layer[2].LAYER_ADDR = (u32)SOURCE_PIC_1;
	lli[3].layer[3].LAYER_ADDR = (u32)SOURCE_PIC_2;
	lli[3].glb.LLP = (u32) &lli[4];

	/* final frame will blendig SOURCE_PIC_1 SOURCE_PIC_2 SOURCE_PIC_3 to PIC_OUTPUT_LAYER*/
	lli[4].layer[0].LAYER_ADDR = (u32)PIC_OUTPUT_LAYER;
	lli[4].layer[1].LAYER_ADDR = (u32)SOURCE_PIC_1;
	lli[4].layer[2].LAYER_ADDR = (u32)SOURCE_PIC_2;
	lli[4].layer[3].LAYER_ADDR = (u32)SOURCE_PIC_3;
	lli[4].glb.LLP = 0;
	lli[4].glb.LL_CFG &= 0xFFFF0000;						//here set all layers inactive and linklist function will stop

	/* dcache clean here because PPE will read lli after ppe enable */
	DCache_Clean((u32)lli, sizeof(lli));

	InterruptRegister(PPE_Handler, PPE_IRQ, NULL, 4);
	InterruptEn(PPE_IRQ, 4);
	PPE_MaskINTConfig(PPE_ALL_OVER_INT | PPE_FR_OVER_INT, ENABLE);

	/*enable PPE*/
	PPE_Cmd(ENABLE);

	while (1);
}

int example_raw_ppe_linklist(void)
{
	if (rtos_task_create(NULL, "ppe_linklist", (rtos_task_t)ppe_linklist, NULL, (2048), (1)) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Cannot create ppe_linklist demo task\n\r");
	}

	return 0;
}