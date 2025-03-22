#include "ameba_soc.h"
#include "os_wrapper.h"
#include <stdio.h>

#define WIDTH		20
#define HEIGHT		20
#define MEM_SIZE	(WIDTH * HEIGHT * 3)		  //RGB888
#define X_RATIO		2
#define Y_RATIO		2
#define MEM_SIZE_OUT	(u32)(MEM_SIZE * X_RATIO * Y_RATIO)		  //RGB888

#define POLLING_MODE	0
#define INTR_MODE		1

#define PPE_MODE		INTR_MODE

static u8 SOURCE_PIC_1[MEM_SIZE] __attribute__((aligned(64)));
static u8 PIC_OUTPUT_LAYER[MEM_SIZE_OUT] __attribute__((aligned(64)));


#if (PPE_MODE == INTR_MODE)
static u32 PPE_Handler(void *Data)
{
	int status;
	UNUSED(Data);

	status = PPE_GetAllIntStatus();
	if (status & PPE_FR_OVER_INT) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Frame over interrupt occurs\n");
		PPE_ClearINTPendingBit(PPE_FR_OVER_INT);
		DCache_Invalidate((u32)PIC_OUTPUT_LAYER, MEM_SIZE_OUT);
		for (u32 i = 0; i < MEM_SIZE_OUT; i++) {
			if (PIC_OUTPUT_LAYER[i] != 0x5C) {
				RTK_LOGS(NOTAG, RTK_LOG_ERROR, "[error] has occured!\r\n");
			}
		}
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "scale over\n");
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "wrong interrupt status: %x\n", status);
	}

	return 0;
}
#endif

static void ppe_scale(void *para)
{
	UNUSED(para);

	/* enable function and clk */
	RCC_PeriphClockCmd(APBPeriph_QSPI, APBPeriph_QSPI_CLOCK, ENABLE);

	/*prepare data*/
	memset(SOURCE_PIC_1, 0x5C, MEM_SIZE);
	memset(PIC_OUTPUT_LAYER, 0, MEM_SIZE_OUT);
	DCache_Clean((u32)SOURCE_PIC_1, MEM_SIZE);
	DCache_Clean((u32)PIC_OUTPUT_LAYER, MEM_SIZE_OUT);

	/*declaration of input/result layer initialization struct*/
	PPE_InputLayer_InitTypeDef 		PPE_Input_Layer1;
	PPE_ResultLayer_InitTypeDef		PPE_Result_Layer;
	PPE_InitTypeDef					PPE_Init_User;

	PPE_InputLayer_StructInit(&PPE_Input_Layer1);
	PPE_ResultLayer_StructInit(&PPE_Result_Layer);
	PPE_StructInit(&PPE_Init_User);

	/* scale up */
	PPE_SetScaleRation((float)X_RATIO, (float)Y_RATIO);

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


	/*initial result layer*/
	PPE_Result_Layer.src_addr					= (u32)PIC_OUTPUT_LAYER;
	PPE_Result_Layer.width						= PPE_Input_Layer1.width * X_RATIO;
	PPE_Result_Layer.height						= PPE_Input_Layer1.height * Y_RATIO;
	PPE_Result_Layer.format 					= PPE_RGB888;
	PPE_Result_Layer.line_len					= PPE_Result_Layer.width;
	PPE_Result_Layer.type 						= PPE_ADDR_MEM;
	PPE_InitResultLayer(&PPE_Result_Layer);

	/*initial PPE global*/
	PPE_Init_User.function					=	PPE_FUNCTION_SCALE;
	PPE_Init(&PPE_Init_User);

#if (PPE_MODE == INTR_MODE)
	InterruptRegister(PPE_Handler, PPE_IRQ, NULL, 4);
	InterruptEn(PPE_IRQ, 4);
	PPE_MaskINTConfig(PPE_FR_OVER_INT, ENABLE);
#endif

	/*enable PPE*/
	PPE_Cmd(ENABLE);

#if (PPE_MODE == POLLING_MODE)
	while (1) {
		if (PPE_GetGlobalState() == 0) {
			DCache_Invalidate((u32)PIC_OUTPUT_LAYER, MEM_SIZE_OUT);
			for (u32 i = 0; i < MEM_SIZE_OUT; i++) {
				if (PIC_OUTPUT_LAYER[i] != 0x5C) {
					RTK_LOGS(NOTAG, RTK_LOG_ERROR, "[error] has occured!\r\n");
				}
			}
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "scale over\n");
			break;
		}
	}
#endif

	while (1);
}

int example_raw_ppe_scale(void)
{
	if (rtos_task_create(NULL, "ppe_scale", (rtos_task_t)ppe_scale, NULL, (2048), (1)) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Cannot create ppe_scale demo task\n\r");
	}

	return 0;
}

