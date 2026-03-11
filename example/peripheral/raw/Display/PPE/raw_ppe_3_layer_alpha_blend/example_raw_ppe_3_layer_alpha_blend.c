#include "ameba_soc.h"
#include "os_wrapper.h"
#include <stdio.h>

#define WIDTH		50
#define HEIGHT		50
#define MEM_SIZE	WIDTH * HEIGHT * 4		  //ARGB888

#define POLLING_MODE	0
#define INTR_MODE		1

#define PPE_MODE		INTR_MODE

static u8 SOURCE_PIC_1[MEM_SIZE] __attribute__((aligned(64)));
static u8 SOURCE_PIC_2[MEM_SIZE] __attribute__((aligned(64)));
static u8 SOURCE_PIC_3[MEM_SIZE] __attribute__((aligned(64)));
static u8 PIC_OUTPUT_LAYER[MEM_SIZE] __attribute__((aligned(64)));
static u8 PIC_OUTPUT_LAYER_SW[MEM_SIZE] __attribute__((aligned(64)));

static u32 blend_2pixel_ARGB8888(u32 background_pixel, u32 front_ground_pixel)
{
	uint8_t back_A = (background_pixel >> 24) & 0Xff;
	uint8_t back_R = (background_pixel >> 16) & 0Xff;;
	uint8_t back_G = (background_pixel >> 8)  & 0Xff;;
	uint8_t back_B = (background_pixel >> 0)  & 0Xff;;

	uint8_t front_A = (front_ground_pixel >> 24) & 0Xff;
	uint8_t front_R = (front_ground_pixel >> 16) & 0Xff;
	uint8_t front_G = (front_ground_pixel >> 8)  & 0Xff;
	uint8_t front_B = (front_ground_pixel >> 0)  & 0Xff;

	uint8_t result_A = ((front_A * front_A + back_A * (255 - front_A)) / 255) & 0xFF;
	uint8_t result_R = ((front_R * front_A + back_R * (255 - front_A)) / 255) & 0xFF;
	uint8_t result_G = ((front_G * front_A + back_G * (255 - front_A)) / 255) & 0xFF;
	uint8_t result_B = ((front_B * front_A + back_B * (255 - front_A)) / 255) & 0xFF;

	u32 result_pixel = (result_A << 24) + (result_R << 16) + (result_G << 8) + (result_B);

	return result_pixel;

}

/* For 3 layers alpha blend, all 3 layers pixel format is ARGB8888, output layer is RGB888 */
static u32 Caculate_Alpha_blending_Pixel_Value(u32 layer1_pix, u32 layer2_pix, u32 layer3_pix,
		u8 layer1_alpha, u8 layer2_alpha, u8 layer3_alpha)
{
	u32 pixel, pixel1, pixel2, pixel3;
	u8 alpha;

	alpha = (layer1_pix >> 24) * layer1_alpha / 255;
	pixel1 = (layer1_pix & 0X00FFFFFF) + (alpha << 24);

	alpha = (layer2_pix >> 24) * layer2_alpha / 255;
	pixel2 = (layer2_pix & 0X00FFFFFF) + (alpha << 24);

	alpha = (layer3_pix >> 24) * layer3_alpha / 255;
	pixel3 = (layer3_pix & 0X00FFFFFF) + (alpha << 24);

	pixel = blend_2pixel_ARGB8888(pixel1, pixel2);
	pixel = blend_2pixel_ARGB8888(pixel, pixel3);

	return pixel;
}


static void Initialize_Array_with_Random(uint8_t *source_address, u32 sizeof_array)
{
	for (u32 counter = 0; counter < sizeof_array; counter++) {
		uint8_t byte_temp = (uint8_t)(_rand() % 0xFF);
		source_address[counter] = byte_temp;
	}
}

#if (PPE_MODE == INTR_MODE)
static u32 PPE_Handler(void *Data)
{
	int status;
	UNUSED(Data);

	status = PPE_GetAllIntStatus();
	if (status & PPE_FR_OVER_INT) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Frame over interrupt occurs\n");
		PPE_ClearINTPendingBit(PPE_FR_OVER_INT);
		DCache_Invalidate((u32) PIC_OUTPUT_LAYER, sizeof(PIC_OUTPUT_LAYER));
		for (int i = 0; i < MEM_SIZE; i++) {
			if (PIC_OUTPUT_LAYER[i] != PIC_OUTPUT_LAYER_SW[i]) {
				RTK_LOGS(NOTAG, RTK_LOG_ERROR, "[error] has occured! %x, %x\r\n", PIC_OUTPUT_LAYER[i], PIC_OUTPUT_LAYER_SW[i]);
			}
		}
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "3 layer alpha blending over\n");
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "wrong interrupt status: %x\n", status);
	}

	return 0;
}
#endif

static void ppe_3_layer_alpha_blend(void *para)
{
	UNUSED(para);

	/*declaration of input/result layer initialization struct*/
	PPE_InputLayer_InitTypeDef 		PPE_Input_Layer1;
	PPE_InputLayer_InitTypeDef 		PPE_Input_Layer2;
	PPE_InputLayer_InitTypeDef 		PPE_Input_Layer3;
	PPE_ResultLayer_InitTypeDef		PPE_Result_Layer;
	PPE_InitTypeDef						PPE_Init_User;
	u32 *ptr = (u32 *) PIC_OUTPUT_LAYER_SW;
	u32 *ptr_layer1 = (u32 *) SOURCE_PIC_1;
	u32 *ptr_layer2 = (u32 *) SOURCE_PIC_2;
	u32 *ptr_layer3 = (u32 *) SOURCE_PIC_3;

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
	PPE_Input_Layer1.line_len								= WIDTH;
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
	PPE_Input_Layer2.line_len								= WIDTH;
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
	PPE_Input_Layer3.line_len								= WIDTH;
	PPE_Input_Layer3.key_color_value						= 0;
	PPE_InitInputLayer(3, &PPE_Input_Layer3);

	/*initial result layer*/
	PPE_Result_Layer.src_addr								= (u32)PIC_OUTPUT_LAYER;
	PPE_Result_Layer.width									= WIDTH;
	PPE_Result_Layer.height									= HEIGHT;
	PPE_Result_Layer.format 								= PPE_ARGB8888;
	PPE_Result_Layer.line_len								= WIDTH;
	PPE_Result_Layer.type									= PPE_ADDR_MEM;
	PPE_InitResultLayer(&PPE_Result_Layer);

	/*initial PPE global*/
	PPE_Init_User.function									= PPE_FUNCTION_ALPHA_BLEND;
	PPE_Init_User.blend_layer_num 							= 3;
	PPE_Init(&PPE_Init_User);

	/* Caculate result by software */
	for (int i = 0 ; i < MEM_SIZE / 4; i++) {
		*(ptr + i) = Caculate_Alpha_blending_Pixel_Value(*(ptr_layer1 + i), *(ptr_layer2 + i), *(ptr_layer3 + i),
					 PPE_Input_Layer1.const_ABGR8888_value >> 24,
					 PPE_Input_Layer2.const_ABGR8888_value >> 24,
					 PPE_Input_Layer3.const_ABGR8888_value >> 24);
	}

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
			DCache_Invalidate((u32)PIC_OUTPUT_LAYER, sizeof(PIC_OUTPUT_LAYER));
			for (int i = 0; i < MEM_SIZE; i++) {
				if (PIC_OUTPUT_LAYER[i] != PIC_OUTPUT_LAYER_SW[i]) {
					RTK_LOGS(NOTAG, RTK_LOG_ERROR, "[error] has occured! %x, %x\r\n", PIC_OUTPUT_LAYER[i], PIC_OUTPUT_LAYER_SW[i]);
				}
			}
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "3 layer blending over\n");
			break;
		}
	}
#endif

	while (1);
}

int example_raw_ppe_3_layer_alpha_blend(void)
{
	if (rtos_task_create(NULL, "ppe_3_layer_alpha_blend", (rtos_task_t)ppe_3_layer_alpha_blend, NULL, (2048), (1)) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Cannot create ppe_3_layer_alpha_blend demo task\n\r");
	}

	return 0;
}

