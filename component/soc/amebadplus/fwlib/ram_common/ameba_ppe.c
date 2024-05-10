/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup PPE
* @brief PPE driver modules
* @{
*/

/* Exported functions --------------------------------------------------------*/
/** @defgroup PPE_Exported_Functions PPE Exported Functions
  * @{
  */


/**
  * @brief  Enable PPE.
  * @param  state: the state of PPE.
  *              This parameter can only be ENABLE.
  * @retval None.
  */
void PPE_Cmd(int state)
{
	if (state) {
		PPE->GLB_CTL |= PPE_GLB_EN_MSK;
	}
}

/**
  * @brief  Get PPE global state.
  * @param  None.
  * @retval Global state. 0 means PPE is not working, 1 means PPE is working.
  */
u32 PPE_GetGlobalState(void)
{
	return (PPE->GLB_CTL & PPE_GLB_EN_MSK);
}

/**
  * @brief  Suspend PPE.
  * @param  None.
  * @retval None.
  */
void PPE_Suspend(void)
{
	PPE->GLB_CTL |= PPE_SUSPEND_MSK;
}

/**
  * @brief  Resume PPE from suspend state.
  * @param  None.
  * @retval None.
  */
void PPE_Resume(void)
{
	PPE->GLB_CTL = PPE_RESUME_MSK;
}

/**
  * @brief  PPE abort.
  * @param  None.
  * @retval None.
  */
void PPE_Abort(void)
{
	/* Abort the transaction when the values of SUSP and ALL_INACT are both 1*/
	PPE->GLB_CTL |= PPE_SUSPEND_MSK;
	while (!(PPE->GLB_CTL & PPE_ALL_INACT_MSK));
	PPE->GLB_CTL = PPE_ABORT_MSK;
}


/**
  * @brief  Check if a layer is inactive.
  * @param  chn_id: the layer number to be checked.
  * @retval Status.
  */
int PPE_ChannelInactiveCheck(u8 chn_id)
{
	int rv = 0;

	if (PPE->GLB_CTL & ((BIT0 << PPE_CHN_INACT_POS) << chn_id)) {
		rv = 1;
	}
	return rv;
}

/**
  * @brief  Set ration of scale function.
  * @param  scale_x: ration of scale in X diretion. No smaller than 1/16.
  *         scale_y: ration of scale in Y diretion. No smaller than 1/16.
  * @retval None.
  */
void PPE_SetScaleRation(float scale_x, float scale_y)
{
	u32 fix_x, fix_y;

	assert_param(scale_x > 0);
	assert_param(scale_y > 0);

	fix_x = (u32)(65536 / scale_x);
	fix_y = (u32)(65536 / scale_y);

	/* workaround: In multi-frame function, if fix_y of first frame is multiple of 65536 and fix_y of
	second frame, and input layer1 is not reload for the second frame, error may occur. So here set fix_y
	minus 1 */
	if ((fix_y >= 65536) && (fix_y % 65536 == 0)) {
		fix_y = fix_y - 1;
	}

	PPE->SCA_RATIO_X = fix_x;
	PPE->SCA_RATIO_Y = fix_y;
}

/**
  * @brief  Get all interrupt status.
  * @param  None.
  * @retval Status.
  */
u32 PPE_GetAllIntStatus(void)
{
	return PPE->INT_STATUS;
}

/**
  * @brief  Clear specific interrupt status.
  * @param  PPE_INT: specifies the PPE interrupt type.
  *          This parameter can be one or combinations of the following values:
  *            @arg PPE_ALL_OVER_INT: All frames blend or scale over.
  *            @arg PPE_FR_OVER_INT: One frame blend or scale over.
  *            @arg PPE_LOAD_OVER_INT: layers register be loaded to internal register over.
  *            @arg PPE_LINE_WL_INT: Specific line blend/scale over.
  *            @arg PPE_SUSP_INAC_INT: All channels come to inactive after suspend.
  * @retval None.
  */
void PPE_ClearINTPendingBit(u32 PPE_INT)
{
	PPE->INT_CLR |= PPE_INT;
}

/**
  * @brief  Enable/disable specific interrupt.
  * @param  PPE_INT_MSK: specifies the PPE interrupt type.
  *          This parameter can be one or combinations of the following values:
  *            @arg PPE_ALL_OVER_INT: All frames blend or scale over.
  *            @arg PPE_FR_OVER_INT: One frame blend or scale over.
  *            @arg PPE_LOAD_OVER_INT: layers register be loaded to internal register over.
  *            @arg PPE_LINE_WL_INT: Specific line blend/scale over.
  *            @arg PPE_SUSP_INAC_INT: All channels come to inactive after suspend.
  *         NewState: the new state.
  *              This parameter can be: ENABLE or DISABLE.
  * @retval None.
  */
void PPE_MaskINTConfig(u32 PPE_INT_MSK, int NewState)
{
	if (NewState == ENABLE) {
		PPE->INT_MASK |= PPE_INT_MSK;
	} else {
		PPE->INT_MASK &= ~PPE_INT_MSK;
	}
}

/**
  * @brief  Mask all interrupt.
  * @param  None.
  * @retval None.
  */
void PPE_MaskAllInt(void)
{
	PPE->INT_MASK = 0x0;
}

/**
  * @brief  Configure auto reload layers.
  * @param  layers: auto reload layers.
  *          This parameter can be one or combinations of the following values:
  *            @arg PPE_RESULT_LAYER_BIT: result layer.
  *            @arg PPE_INPUT_LAYER1_BIT: input layer1.
  *            @arg PPE_INPUT_LAYER2_BIT: input layer2.
  *            @arg PPE_INPUT_LAYER3_BIT: input layer3.
  * @retval None.
  */
void PPE_AutoReloadSetLayer(u32 layers)
{
	u32 reload_en;

	reload_en = PPE_LAYER_RELOAD_EN_MSK & layers;
	PPE->RELOAD_CFG &= PPE_LAYER_RELOAD_EN_CLR;
	PPE->RELOAD_CFG |= reload_en;
}

/**
  * @brief  Enable/disable Auto reload function.
  * @param  NewState: the new state.
  *              This parameter can be: ENABLE or DISABLE.
  * @retval None.
  */
void PPE_AutoReloadClrLayerAll(void)
{
	PPE->RELOAD_CFG &= PPE_LAYER_RELOAD_EN_CLR;
}

/**
  * @brief  Enable/disable auto cleared after loading.
  * @param  NewState:the new state.
  *              This parameter can be: ENABLE or DISABLE.
  * @retval None.
  */
void PPE_AutoClearCmd(int NewState)
{
	if (NewState) {
		PPE->RELOAD_CFG |= PPE_AUTO_CLR_MSK;
	} else {
		PPE->RELOAD_CFG &= PPE_AUTO_CLR_CLR;
	}
}

/**
  * @brief  Tell PPE setting is vaild for auto reload function.
  * @param  None
  * @retval None.
  */
void PPE_SetValid(void)
{
	PPE->RELOAD_CFG |= PPE_SET_VALID_MSK;
}

/**
  * @brief  Get setting valid value to polling.
  * @param  None.
  * @retval Set valid bit value. 0 means user can use to PPE_SetValid tell PPE setting is valid.
  */
u32 PPE_GetValid(void)
{
	return ((PPE->RELOAD_CFG & PPE_SET_VALID_MSK) == 0) ? 0 : 1;
}

/**
  * @brief  Set LLP value to the pointer of first linklist item.
  * @param  lli: pointer to first listlist item. Must align to 4Byte.
  * @retval None.
  */
void PPE_SetLLP(u32      lli)
{
	PPE->LLP = lli;
}

/**
  * @brief  Set layer used by first linklist item.
  * @param  layers: linklist layers.
  *          This parameter can be one or combinations of the following values:
  *            @arg PPE_RESULT_LAYER_BIT: result layer.
  *            @arg PPE_INPUT_LAYER1_BIT: input layer1.
  *            @arg PPE_INPUT_LAYER2_BIT: input layer2.
  *            @arg PPE_INPUT_LAYER3_BIT: input layer3.
  * @retval None.
  */
void PPE_LLSetLayer(u32 layers)
{
	u32 layer_ll_en;

	layer_ll_en = PPE_LAYER_LL_EN_MSK & layers;
	PPE->LL_CFG &= PPE_LAYER_LL_EN_CLR;
	PPE->LL_CFG |= layer_ll_en;
}


/**
  * @brief  Set line index for line over interrupt.
  * @param  line: the link index.
  * @retval None.
  */
void PPE_SetLine(u32 line)
{
	PPE->LINE_INDEX = line;
}


/**
  * @brief  Fills each PPE_init_struct member with its default value.
  * @param  PPE_init_struct: pointer to an PPE_InitTypeDef structure which will be initialized.
  * @retval None
  */
void PPE_StructInit(PPE_InitTypeDef *PPE_init_struct)
{
	assert_param(PPE_init_struct != NULL);

	PPE_init_struct->function = PPE_FUNCTION_ALPHA_BLEND;
	PPE_init_struct->blend_layer_num = 0;
}

/**
  * @brief  Fills each layer_init_struct member with its default value.
  * @param  layer_init_struct: pointer to an PPE_InputLayer_InitTypeDef structure which will be initialized.
  * @retval None
  */
void PPE_InputLayer_StructInit(PPE_InputLayer_InitTypeDef *layer_init_struct)
{
	layer_init_struct->src_addr 								= 0;
	layer_init_struct->start_x									= 0;
	layer_init_struct->start_y									= 0;
	layer_init_struct->width									= 1;
	layer_init_struct->height									= 1;
	layer_init_struct->const_ABGR8888_value						= 0xFFFFFFFF;
	layer_init_struct->format									= PPE_RGB888;
	layer_init_struct->src										= PPE_LAYER_SRC_FROM_DMA;
	layer_init_struct->color_key_en								= DISABLE;
	layer_init_struct->line_len									= 0;
	layer_init_struct->key_color_value							= 0xFFFFFFFF;
}

/**
  * @brief  Fills each layer_init_struct member with its default value.
  * @param  layer_init_struct: pointer to an PPE_ResultLayer_InitTypeDef structure which will be initialized.
  * @retval None
  */
void PPE_ResultLayer_StructInit(PPE_ResultLayer_InitTypeDef *layer_init_struct)
{
	layer_init_struct->src_addr						= 0;
	layer_init_struct->width						= 1;
	layer_init_struct->height						= 1;
	layer_init_struct->format 						= PPE_RGB888;
	layer_init_struct->line_len						= 1;
	layer_init_struct->type 						= PPE_ADDR_MEM;
}

/**
  * @brief  Initializes the PPE peripheral according to the specified
  *              parameters in the PPE_init_struct.
  * @param  PPE_init_struct: pointer to a PPE_InitTypeDef structure that contains
  *              the configuration information for the specified PPE peripheral.
  * @retval None
  */
void PPE_Init(PPE_InitTypeDef *PPE_init_struct)
{
	assert_param(PPE_init_struct != NULL);
	assert_param(IS_PPE_FUNCTION(PPE_init_struct->function));

	switch (PPE_init_struct->function) {
	case PPE_FUNCTION_ALPHA_BLEND: {
		assert_param(IS_PPE_ALPHA_BLEND_LAYER_NUM(PPE_init_struct->blend_layer_num));
		PPE->FUNC_CFG &= PPE_FUNC_SEL_CLR;
		PPE->FUNC_CFG |= ((PPE_FUNCTION_ALPHA_BLEND << PPE_FUNC_SEL_POS) & PPE_FUNC_SEL_MSK);
		PPE->FUNC_CFG &= PPE_BLEND_LAYER_NUM_CLR;
		PPE->FUNC_CFG |= ((PPE_init_struct->blend_layer_num << PPE_BLEND_LAYER_NUM_POS) & PPE_BLEND_LAYER_NUM_MSK);
		break;
	}
	case PPE_FUNCTION_SCALE: {
		PPE->FUNC_CFG &= PPE_FUNC_SEL_CLR;
		PPE->FUNC_CFG |= ((PPE_FUNCTION_SCALE << PPE_FUNC_SEL_POS) & PPE_FUNC_SEL_MSK);
		break;
	}
	}
}

/**
  * @brief  Initialize specific input layer.
  * @param  id: Input layer number. Can be 1/2/3.
  *         layer_init_struct: pointer to a PPE_InputLayer_InitTypeDef structure that contains
  *              the configuration information for the specified input layer.
  * @retval None
  */
void PPE_InitInputLayer(u8 id, PPE_InputLayer_InitTypeDef *layer_init_struct)
{
	u8 cov_layer_id = id - 1;

	PPE_LAYER->INPUT_LAYER[cov_layer_id].ADDR = layer_init_struct->src_addr;
	PPE_LAYER->INPUT_LAYER[cov_layer_id].CONST_PIX = layer_init_struct->const_ABGR8888_value;
	PPE_LAYER->INPUT_LAYER[cov_layer_id].POS = (
				((layer_init_struct->start_x << INPUT_LAYER_POS_X_POS) & INPUT_LAYER_POS_X_MSK) |
				((layer_init_struct->start_y << INPUT_LAYER_POS_Y_POS) & INPUT_LAYER_POS_Y_MSK)
			);
	PPE_LAYER->INPUT_LAYER[cov_layer_id].WIN_SIZE = (
				((layer_init_struct->width << INPUT_LAYER_WIDTH_POS) & INPUT_LAYER_WIDTH_MSK) |
				((layer_init_struct->height << INPUT_LAYER_HEIGHT_POS) & INPUT_LAYER_HEIGHT_MSK)
			);
	PPE_LAYER->INPUT_LAYER[cov_layer_id].PIC_CFG = (
				((layer_init_struct->format << INPUT_LAYER_PIX_FORMAT_POS) & INPUT_LAYER_PIX_FORMAT_MSK) |
				((layer_init_struct->src << INPUT_LAYER_PIX_SRC_POS) & INPUT_LAYER_PIX_SRC_MSK) |
				((layer_init_struct->color_key_en << INPUT_LAYER_KEY_EN_POS) & INPUT_LAYER_KEY_EN_MSK) |
				((layer_init_struct->line_len << INPUT_LAYER_LINE_LEN_POS) & INPUT_LAYER_LINE_LEN_MSK)
			);
	PPE_LAYER->INPUT_LAYER[cov_layer_id].KEY_COLOR = layer_init_struct->key_color_value;
	PPE_LAYER->INPUT_LAYER[cov_layer_id].HS_CFG = (
				((0 << INPUT_LAYER_HW_INDEX_POS) & INPUT_LAYER_HW_INDEX_MSK) |
				((PPE_MSIZE_8 << INPUT_LAYER_MSIZE_LOG_POS) & INPUT_LAYER_MSIZE_LOG_MSK) |
				((DISABLE << INPUT_LAYER_HS_EN_POS) & INPUT_LAYER_HS_EN_MSK) |
				((PPE_POLARITY_HIGH << INPUT_LAYER_POLAR_POS) & INPUT_LAYER_POLAR_MSK) |
				((PPE_DMA_HW_HANDSHAKE << INPUT_LAYER_HW_HS_POS) & INPUT_LAYER_HW_HS_MSK)
			);
	PPE_LAYER->INPUT_LAYER[cov_layer_id].BUS_CFG = (
				((0 << INPUT_LAYER_PRIOR_POS) & INPUT_LAYER_PRIOR_MSK) |
				((PPE_MAX_AXLEN_127 << INPUT_LAYER_MAX_AXLEN_LOG_POS) & INPUT_LAYER_MAX_AXLEN_LOG_MSK) |
				((1 << INPUT_LAYER_AXCACHE_POS) & INPUT_LAYER_AXCACHE_MSK) |
				((PPE_ARBURST_INCR << INPUT_LAYER_INCR_POS) & INPUT_LAYER_INCR_MSK) |
				((2 << INPUT_LAYER_AXSIZE_POS) & INPUT_LAYER_AXSIZE_MSK)
			);
}

/**
  * @brief  Initialize result layer.
  * @param  layer_init_struct: pointer to a PPE_ResultLayer_InitTypeDef structure that contains
  *              the configuration information for the result layer.
  * @retval None
  */
void PPE_InitResultLayer(PPE_ResultLayer_InitTypeDef *layer_init_struct)
{
	u32 handshake_en, INCR;

	if (layer_init_struct->type == PPE_ADDR_QSPI) {
		handshake_en = ENABLE;
		INCR = PPE_ARBURST_FIX;
	} else {
		handshake_en = DISABLE;
		INCR = PPE_ARBURST_INCR;
	}

	PPE_LAYER->RESULT_LAYER.ADDR = layer_init_struct->src_addr;
	PPE_LAYER->RESULT_LAYER.WIN_SIZE = (
										   ((layer_init_struct->width << RESULT_LAYER_WIDTH_POS) & RESULT_LAYER_WIDTH_MSK) |
										   ((layer_init_struct->height << RESULT_LAYER_HEIGHT_POS) & RESULT_LAYER_HEIGHT_MSK)
									   );
	PPE_LAYER->RESULT_LAYER.PIC_CFG = (
										  ((layer_init_struct->format << RESULT_LAYER_PIX_FORMAT_POS) & RESULT_LAYER_PIX_FORMAT_MSK) |
										  ((layer_init_struct->line_len << RESULT_LAYER_LINE_LEN_POS) & RESULT_LAYER_LINE_LEN_MSK)
									  );
	PPE_LAYER->RESULT_LAYER.HS_CFG = (
										 ((0 << RESULT_LAYER_HW_INDEX_POS) & RESULT_LAYER_HW_INDEX_MSK) |
										 ((PPE_MSIZE_8 << RESULT_LAYER_MSIZE_LOG_POS) & RESULT_LAYER_MSIZE_LOG_MSK) |
										 ((handshake_en << RESULT_LAYER_HS_EN_POS) & RESULT_LAYER_HS_EN_MSK) |
										 ((PPE_POLARITY_HIGH << RESULT_LAYER_POLAR_POS) & RESULT_LAYER_POLAR_MSK) |
										 ((PPE_DMA_HW_HANDSHAKE << RESULT_LAYER_HW_HS_POS) & RESULT_LAYER_HW_HS_MSK)
									 );
	PPE_LAYER->RESULT_LAYER.BUS_CFG = (
										  ((0 << RESULT_LAYER_PRIOR_POS) & RESULT_LAYER_PRIOR_MSK) |
										  ((PPE_MAX_AXLEN_127 << RESULT_LAYER_MAX_AXLEN_LOG_POS) & RESULT_LAYER_MAX_AXLEN_LOG_MSK) |
										  ((1 << RESULT_LAYER_AXCACHE_POS) & RESULT_LAYER_AXCACHE_MSK) |
										  ((INCR << RESULT_LAYER_INCR_POS) & RESULT_LAYER_INCR_MSK) |
										  ((2 << RESULT_LAYER_AXSIZE_POS) & RESULT_LAYER_AXSIZE_MSK)
									  );
}

/** @} */

/** @} */

/**
  * @}
  */

