/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *const TAG = "PPE";

static PPE_InputLayer_InitTypeDef PPE_Input_Layer1;
static PPE_InputLayer_InitTypeDef PPE_Input_Layer2;
static PPE_InputLayer_InitTypeDef PPE_Input_Layer3;
static PPE_ResultLayer_InitTypeDef PPE_Result_Layer;
static void (*user_func)(void);

static void PPE_XOR_INTHandler(void)
{
	u32 irq_status = PPE_GetAllIntStatus();

	if (irq_status & PPE_BIT_INTR_ST_ALL_OVER) {
		PPE_ClearINTPendingBit(PPE_BIT_INTR_ST_ALL_OVER);
		if (user_func) {
			user_func();
		}
	}
}

static void PPE_MatrixMultiply(float matrix[3][3], float mult[3][3])
{
	float m00, m01, m02, m10, m11, m12, m20, m21, m22;
	float t00, t01, t02, t10, t11, t12, t20, t21, t22;

	m00 = matrix[0][0];
	m01 = matrix[0][1];
	m02 = matrix[0][2];

	m10 = matrix[1][0];
	m11 = matrix[1][1];
	m12 = matrix[1][2];

	m20 = matrix[2][0];
	m21 = matrix[2][1];
	m22 = matrix[2][2];

	t00 = mult[0][0];
	t01 = mult[0][1];
	t02 = mult[0][2];

	t10 = mult[1][0];
	t11 = mult[1][1];
	t12 = mult[1][2];

	t20 = mult[2][0];
	t21 = mult[2][1];
	t22 = mult[2][2];

	matrix[0][0] = (m00 * t00) + (m01 * t10) + (m02 * t20);
	matrix[0][1] = (m00 * t01) + (m01 * t11) + (m02 * t21);
	matrix[0][2] = (m00 * t02) + (m01 * t12) + (m02 * t22);

	matrix[1][0] = (m10 * t00) + (m11 * t10) + (m12 * t20);
	matrix[1][1] = (m10 * t01) + (m11 * t11) + (m12 * t21);
	matrix[1][2] = (m10 * t02) + (m11 * t12) + (m12 * t22);

	matrix[2][0] = (m20 * t00) + (m21 * t10) + (m22 * t20);
	matrix[2][1] = (m20 * t01) + (m21 * t11) + (m22 * t21);
	matrix[2][2] = (m20 * t02) + (m21 * t12) + (m22 * t22);
}

static void PPE_InverseMatrix(float input[3][3], float inverse[3][3])
{
	float augmented[3][2 * 3];
	int i, j, k;

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			augmented[i][j] = input[i][j];
			augmented[i][j + 3] = (i == j) ? 1.0f : 0.0f;
		}
	}

	for (i = 0; i < 3; i++) {
		if (augmented[i][i] == 0) {
			int swapRow = -1;
			for (k = i + 1; k < 3; k++) {
				if (augmented[k][i] != 0) {
					swapRow = k;
					break;
				}
			}
			if (swapRow == -1) {
				RTK_LOGE(TAG, "No inverse matrix\n");
				return;
			} else {
				for (j = 0; j < 2 * 3; j++) {
					float temp = augmented[i][j];
					augmented[i][j] = augmented[swapRow][j];
					augmented[swapRow][j] = temp;
				}
			}
		}

		float diag = augmented[i][i];
		for (j = 0; j < 2 * 3; j++) {
			augmented[i][j] /= diag;
		}

		for (j = 0; j < 3; j++) {
			if (j != i) {
				float factor = augmented[j][i];
				for (k = 0; k < 2 * 3; k++) {
					augmented[j][k] -= factor * augmented[i][k];
				}
			}
		}
	}

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			inverse[i][j] = augmented[i][j + 3];
		}
	}
}

static void PPE_CalulateME(float scale_x, float scale_y, u32 angle, u32 width, u32 height, u32 move_x, u32 move_y, float matrix[3][3])
{
	float matrix_moving[3][3];
	float matrix_scale[3][3];
	float matrix_rotation[3][3];

	/* optimize for only rotation */
	if ((scale_x == 1) && (scale_y == 1)) {
		switch (angle) {
		case 0:
			matrix[0][0] = 1;
			matrix[0][1] = 0;
			matrix[0][2] = 0;
			matrix[1][0] = 0;
			matrix[1][1] = 1;
			matrix[1][2] = 0;
			matrix[2][0] = 0;
			matrix[2][1] = 0;
			matrix[2][2] = 1;
			break;
		case 90:
			matrix[0][0] = 0;
			matrix[0][1] = 1;
			matrix[0][2] = 0;
			matrix[1][0] = -1;
			matrix[1][1] = 0;
			matrix[1][2] = height - 1;
			matrix[2][0] = 0;
			matrix[2][1] = 0;
			matrix[2][2] = 1;
			break;
		case 180:
			matrix[0][0] = -1;
			matrix[0][1] = 0;
			matrix[0][2] = width - 1;
			matrix[1][0] = 0;
			matrix[1][1] = -1;
			matrix[1][2] = height - 1;
			matrix[2][0] = 0;
			matrix[2][1] = 0;
			matrix[2][2] = 1;
			break;
		case 270:
			matrix[0][0] = 0;
			matrix[0][1] = -1;
			matrix[0][2] = width - 1;
			matrix[1][0] = 1;
			matrix[1][1] = 0;
			matrix[1][2] = 0;
			matrix[2][0] = 0;
			matrix[2][1] = 0;
			matrix[2][2] = 1;
			break;
		}
		return;
	}

	_memset(matrix_moving, 0, 36);
	_memset(matrix_scale, 0, 36);
	_memset(matrix_rotation, 0, 36);

	matrix_scale[0][0] = scale_x;
	matrix_scale[1][1] = scale_y;
	matrix_scale[2][2] = 1;

	matrix_moving[0][0] = 1;
	matrix_moving[1][1] = 1;
	matrix_moving[2][2] = 1;

	matrix_rotation[2][2] = 1;

	if (angle == 0) {
		matrix_rotation[0][0] = 1;
		matrix_rotation[0][1] = 0;
		matrix_rotation[1][0] = 0;
		matrix_rotation[1][1] = 1;
		matrix_moving[0][2] = move_x;
		matrix_moving[1][2] = move_y;
	} else if (angle == 90) {
		matrix_rotation[0][0] = 0;
		matrix_rotation[0][1] = -1;
		matrix_rotation[1][0] = 1;
		matrix_rotation[1][1] = 0;
		matrix_moving[0][2] = height * scale_y - 1;
		matrix_moving[1][2] = 0;
	} else if (angle == 180) {
		matrix_rotation[0][0] = -1;
		matrix_rotation[0][1] = 0;
		matrix_rotation[1][0] = 0;
		matrix_rotation[1][1] = -1;
		matrix_moving[0][2] = width * scale_x - 1;
		matrix_moving[1][2] = height * scale_y - 1;
	} else if (angle == 270) {
		matrix_rotation[0][0] = 0;
		matrix_rotation[0][1] = 1;
		matrix_rotation[1][0] = -1;
		matrix_rotation[1][1] = 0;
		matrix_moving[0][2] = 0;
		matrix_moving[1][2] = width * scale_x - 1;
	}

	PPE_MatrixMultiply(matrix_moving, matrix_scale);
	PPE_MatrixMultiply(matrix_moving, matrix_rotation);
	PPE_InverseMatrix(matrix_moving, matrix);
}

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
  *              This parameter can be ENABLE/DISABLE.
  * @retval None.
  */
void PPE_Cmd(int state)
{
	u32 reg;
	reg = PPE->PPE_GLB_STATUS;
	if (state) {
		reg &= ~PPE_MASK_RUN_STATUS;
		reg |= PPE_RUN_STATUS(PPE_STATE_RUNNING);
		PPE->PPE_GLB_STATUS = reg;
	} else {
		reg &= ~PPE_MASK_RUN_STATUS;
		reg |= PPE_RUN_STATUS(PPE_STATE_DISABLE);
		PPE->PPE_GLB_STATUS = reg;
	}
}

/**
  * @brief  Get PPE global state.
  * @param  None.
  * @retval Global state. It can be
  *              PPE_STATE_DISABLE: PPE is disabled
  *              PPE_STATE_RUNNING: PPE is running
  *              PPE_STATE_INACTIVE: PPE is inactive
  *              PPE_STATE_SUSPEND: PPE is suspended
  */
u32 PPE_GetGlobalState(void)
{
	return PPE_GET_RUN_STATUS(PPE->PPE_GLB_STATUS);
}

/**
  * @brief  Suspend PPE.
  * @param  None.
  * @retval None.
  */
void PPE_Suspend(void)
{
	u32 reg = PPE->PPE_GLB_STATUS;
	reg &= ~PPE_MASK_RUN_STATUS;
	reg |= PPE_RUN_STATUS(PPE_STATE_SUSPEND);
	PPE->PPE_GLB_STATUS = reg;
}

/**
  * @brief  Resume PPE from suspend state.
  * @param  None.
  * @retval None.
  */
void PPE_Resume(void)
{
	u32 reg = PPE->PPE_GLB_STATUS;
	reg &= ~PPE_MASK_RUN_STATUS;
	reg |= PPE_RUN_STATUS(PPE_STATE_RUNNING);
	PPE->PPE_GLB_STATUS = reg;
}



/**
  * @brief  Check if a layer is inactive.
  * @param  chn_id: the layer number to be checked.
  * @retval Status.
  */
int PPE_ChannelInactiveCheck(u8 chn_id)
{
	int ret = 0;

	if (PPE_GET_CHN_INACT(PPE->PPE_GLB_STATUS) & (BIT(0) << chn_id)) {
		ret = 1;
	}

	return ret;
}

/**
  * @brief  Enable input layers.
  * @param  layers: input layer to be enabled.
  *          This parameter can be one or combinations of the following values:
  *            @arg PPE_INPUT_LAYER1_BIT: input layer1.
  *            @arg PPE_INPUT_LAYER2_BIT: input layer2.
  *            @arg PPE_INPUT_LAYER3_BIT: input layer3.
  * @retval None.
  */
void PPE_LayerEn(u32 layers)
{
	PPE->PPE_LAY_EN = layers;
}

/**
  * @brief  Get all interrupt status.
  * @param  None.
  * @retval Status.
  */
u32 PPE_GetAllIntStatus(void)
{
	return PPE->PPE_INTR_ST;
}

/**
  * @brief  Clear specific interrupt status.
  * @param  PPE_INT: specifies the PPE interrupt type.
  *          This parameter can be one or combinations of the following values:
  *            @arg PPE_BIT_INTR_ST_ALL_OVER: All frames blend or scale over.
  *            @arg PPE_BIT_INTR_ST_FR_OVER: One frame blend or scale over.
  *            @arg PPE_BIT_INTR_ST_LOAD_OVER: layers register be loaded to internal register over.
  *            @arg PPE_BIT_INTR_ST_LINE_OVER: Specific line blend/scale over.
  *            @arg PPE_BIT_INTR_ST_SUSP_INAC: All channels come to inactive after suspend.
  *            @arg PPE_BIT_INTR_ST_SECURE_ERR: Secure error.
  *            @arg PPE_BIT_INTR_ST_SET_ERR: Set error.
  *            @arg PPE_BIT_INTR_ST_BUS_ERR: bus error.
  *            @arg PPE_BIT_INTR_ST_DIV0_ERR: div0 error.
  * @retval None.
  */
void PPE_ClearINTPendingBit(u32 PPE_INT)
{
	PPE->PPE_INTR_CLR |= PPE_INT;
}

/**
  * @brief  Enable/disable specific interrupt.
  * @param  PPE_INT_MSK: specifies the PPE interrupt type.
  *          This parameter can be one or combinations of the following values:
  *            @arg PPE_BIT_INTR_ST_ALL_OVER: All frames blend or scale over.
  *            @arg PPE_BIT_INTR_ST_FR_OVER: One frame blend or scale over.
  *            @arg PPE_BIT_INTR_ST_LOAD_OVER: layers register be loaded to internal register over.
  *            @arg PPE_BIT_INTR_ST_LINE_OVER: Specific line blend/scale over.
  *            @arg PPE_BIT_INTR_ST_SUSP_INAC: All channels come to inactive after suspend.
  *            @arg PPE_BIT_INTR_ST_SECURE_ERR: Secure error.
  *            @arg PPE_BIT_INTR_ST_SET_ERR: Set error.
  *            @arg PPE_BIT_INTR_ST_BUS_ERR: bus error.
  *            @arg PPE_BIT_INTR_ST_DIV0_ERR: div0 error.
  *         NewState: the new state.
  *              This parameter can be: ENABLE or DISABLE.
  * @retval None.
  */
void PPE_MaskINTConfig(u32 PPE_INT_MSK, int NewState)
{
	if (NewState == ENABLE) {
		PPE->PPE_INTR_MASK |= PPE_INT_MSK;
	} else {
		PPE->PPE_INTR_MASK &= ~PPE_INT_MSK;
	}
}

/**
  * @brief  Mask all interrupt.
  * @param  None.
  * @retval None.
  */
void PPE_MaskAllInt(void)
{
	PPE->PPE_INTR_MASK = 0x0;
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

	reload_en = PPE_MASK_LYR_RELOAD_EN & layers;
	PPE->PPE_RELOAD_CFG &= ~PPE_MASK_LYR_RELOAD_EN;
	PPE->PPE_RELOAD_CFG |= reload_en;
}

/**
  * @brief  Enable/disable Auto reload function.
  * @param  NewState: the new state.
  *              This parameter can be: ENABLE or DISABLE.
  * @retval None.
  */
void PPE_AutoReloadClrLayerAll(void)
{
	PPE->PPE_RELOAD_CFG &= ~PPE_MASK_LYR_RELOAD_EN;
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
		PPE->PPE_RELOAD_CFG |= PPE_BIT_AUTO_CLR;
	} else {
		PPE->PPE_RELOAD_CFG &= ~PPE_BIT_AUTO_CLR;
	}
}

/**
  * @brief  Tell PPE setting is vaild for auto reload function.
  * @param  None
  * @retval None.
  */
void PPE_SetValid(void)
{
	PPE->PPE_RELOAD_CFG |= PPE_BIT_SET_VALID;
}

/**
  * @brief  Get setting valid value to polling.
  * @param  None.
  * @retval Set valid bit value. 0 means user can use to PPE_SetValid tell PPE setting is valid.
  */
u32 PPE_GetValid(void)
{
	return ((PPE->PPE_RELOAD_CFG & PPE_BIT_SET_VALID) == 0) ? 0 : 1;
}

/**
  * @brief  Set LLP value to the pointer of first linklist item.
  * @param  lli: pointer to first listlist item. Must align to 4Byte.
  * @retval None.
  */
void PPE_SetLLP(u32 lli)
{
	PPE->PPE_LLP = lli;
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

	layer_ll_en = PPE_MASK_LAYER_LL_EN & layers;
	PPE->PPE_LL_CFG &= ~PPE_MASK_LAYER_LL_EN;
	PPE->PPE_LL_CFG |= layer_ll_en;
}

/**
  * @brief  Set line index for line over interrupt.
  * @param  line: the link index.
  * @retval None.
  */
void PPE_SetLine(u32 line)
{
	PPE->PPE_LINE_INDEX = line;
}


/**
  * @brief  Fills each layer_init_struct member with its default value.
  * @param  layer_init_struct: pointer to an PPE_InputLayer_InitTypeDef structure which will be initialized.
  * @retval None
  */
void PPE_InputLayer_StructInit(PPE_InputLayer_InitTypeDef *layer_init_struct)
{
	layer_init_struct->src_addr 				= 0;
	layer_init_struct->pic_width				= 0;
	layer_init_struct->pic_height				= 0;
	layer_init_struct->format					= PPE_RGB888;
	layer_init_struct->pic_src					= PPE_LAYER_SRC_FROM_DMA;
	layer_init_struct->interp					= PPE_INTERP_TYPE_Nearest_Neighbor;
	layer_init_struct->key_mode					= PPE_KEY_MODE_DISABLE;
	layer_init_struct->line_len					= 0;
	layer_init_struct->const_ABGR8888_value		= 0xFFFFFFFF;
	layer_init_struct->win_min_x				= 0;
	layer_init_struct->win_min_y				= 0;
	layer_init_struct->win_max_x				= 0;
	layer_init_struct->win_max_y				= 0;
	layer_init_struct->key_min_bgr				= 0;
	layer_init_struct->key_max_bgr				= 0;
	layer_init_struct->angle					= 0;
	layer_init_struct->scale_x					= 1;
	layer_init_struct->scale_y					= 1;
}

/**
  * @brief  Fills each layer_init_struct member with its default value.
  * @param  layer_init_struct: pointer to an PPE_ResultLayer_InitTypeDef structure which will be initialized.
  * @retval None
  */
void PPE_ResultLayer_StructInit(PPE_ResultLayer_InitTypeDef *layer_init_struct)
{
	layer_init_struct->src_addr					= 0;
	layer_init_struct->pic_width				= 0;
	layer_init_struct->pic_height				= 0;
	layer_init_struct->format 					= PPE_RGB888;
	layer_init_struct->bg_src					= PPE_BACKGROUND_SOURCE_LAYER1;
	layer_init_struct->line_len 				= 0;
	layer_init_struct->const_bg 				= 0;
	layer_init_struct->blk_width 				= 8;
	layer_init_struct->blk_height 				= 8;
	layer_init_struct->xor_en 					= 0;
}


/**
  * @brief  Initialize specific input layer.
  * @param  id: Input layer number. Can be PPE_INPUT_LAYER1_INDEX or PPE_INPUT_LAYER2_INDEX or PPE_INPUT_LAYER3_INDEX.
  * @param  layer_init_struct: pointer to a PPE_InputLayer_InitTypeDef structure that contains
  *              the configuration information for the specified input layer.
  * @retval None
  */
void PPE_InitInputLayer(u8 id, PPE_InputLayer_InitTypeDef *layer_init_struct)
{
	u8 cov_layer_id = id - 1;
	float matrix_element[3][3];
	u32 val = 0;

	assert_param((id == PPE_INPUT_LAYER1_INDEX) || (id == PPE_INPUT_LAYER2_INDEX) || (id == PPE_INPUT_LAYER3_INDEX));
	assert_param((layer_init_struct->angle == 0) || (layer_init_struct->angle == 90) || (layer_init_struct->angle == 180) ||
				 (layer_init_struct->angle == 270));

	if ((id != PPE_INPUT_LAYER1_INDEX) && (layer_init_struct->angle != 0)) {
		RTK_LOGE(TAG, " Layer2 and layer3 can't support rotation\n");
		assert_param(0);
	}

	/* Calculate matrix element */
	PPE_CalulateME(layer_init_struct->scale_x, layer_init_struct->scale_y, layer_init_struct->angle,
				   layer_init_struct->pic_width, layer_init_struct->pic_height, layer_init_struct->win_min_x,
				   layer_init_struct->win_min_y, matrix_element);

	PPE->Input_Layer[cov_layer_id].PPE_INPUT_LAYERx_ADDR_L = layer_init_struct->src_addr;
	PPE->Input_Layer[cov_layer_id].PPE_INPUT_LAYERx_PIC_SIZE = (
				(PPE_INPUT_LAYERx_PIC_HEIGHT(layer_init_struct->pic_height)) |
				(PPE_INPUT_LAYERx_PIC_WIDTH(layer_init_struct->pic_width))
			);
	PPE->Input_Layer[cov_layer_id].PPE_INPUT_LAYERx_PIC_CFG = (
				(PPE_INPUT_LAYERx_FORMAT(layer_init_struct->format)) |
				((layer_init_struct->pic_src == PPE_LAYER_SRC_CONST) ? 0 : PPE_BIT_INPUT_LAYERx_PIX_SRC) |
				((layer_init_struct->interp == PPE_INTERP_TYPE_Nearest_Neighbor) ? 0 : PPE_BIT_INPUT_LAYERx_INTERP) |
				(PPE_INPUT_LAYERx_KEY_MODE(layer_init_struct->key_mode)) |
				(PPE_INPUT_LAYERx_LINE_LEN(layer_init_struct->line_len))
			);
	PPE->Input_Layer[cov_layer_id].PPE_INPUT_LAYERx_CONST_PIX = layer_init_struct->const_ABGR8888_value;
	/* if pixel source is PSRAM, set AXICACHE=0x6 to use read buffer */
	if ((layer_init_struct->src_addr >= 0x60000000) && (layer_init_struct->src_addr < 0x80000000)) {
		val &= (~PPE_MASK_INPUT_LAYERx_AXCACHE);
		val |= (PPE_INPUT_LAYERx_AXCACHE(0x6));
		val &= (~PPE_MASK_INPUT_LAYERx_AXSIZE);
		val |= (PPE_INPUT_LAYERx_AXSIZE(0x2));
		val &= (~PPE_MASK_INPUT_LAYERx_MAX_AXLEN_LOG);
		PPE->Input_Layer[cov_layer_id].PPE_INPUT_LAYERx_BUS_CFG = val;
	}
	PPE->Input_Layer[cov_layer_id].PPE_INPUT_LAYERx_WIN_MIN = (
				(PPE_INPUT_LAYERx_X_MIN(layer_init_struct->win_min_x)) |
				(PPE_INPUT_LAYERx_Y_MIN(layer_init_struct->win_min_y))
			);
	PPE->Input_Layer[cov_layer_id].PPE_INPUT_LAYERx_WIN_MAX = (
				(PPE_INPUT_LAYERx_X_MAX(layer_init_struct->win_max_x)) |
				(PPE_INPUT_LAYERx_Y_MAX(layer_init_struct->win_max_y))
			);
	PPE->Input_Layer[cov_layer_id].PPE_INPUT_LAYERx_KEY_MIN = PPE_INPUT_LAYERx_BGR_MIN(layer_init_struct->key_min_bgr);
	PPE->Input_Layer[cov_layer_id].PPE_INPUT_LAYERx_KEY_MAX = PPE_INPUT_LAYERx_BGR_MAX(layer_init_struct->key_max_bgr);
	PPE->Input_Layer[cov_layer_id].PPE_INPUT_LAYERx_MATRIX_E11 = (int)(matrix_element[0][0] * 16);
	PPE->Input_Layer[cov_layer_id].PPE_INPUT_LAYERx_MATRIX_E12 = (int)(matrix_element[0][1] * 16);
	PPE->Input_Layer[cov_layer_id].PPE_INPUT_LAYERx_MATRIX_E13 = (int)(matrix_element[0][2] * 16);
	PPE->Input_Layer[cov_layer_id].PPE_INPUT_LAYERx_MATRIX_E21 = (int)(matrix_element[1][0] * 16);
	PPE->Input_Layer[cov_layer_id].PPE_INPUT_LAYERx_MATRIX_E22 = (int)(matrix_element[1][1] * 16);
	PPE->Input_Layer[cov_layer_id].PPE_INPUT_LAYERx_MATRIX_E23 = (int)(matrix_element[1][2] * 16);
}

/**
  * @brief  Initialize result layer.
  * @param  layer_init_struct: pointer to a PPE_ResultLayer_InitTypeDef structure that contains
  *              the configuration information for the result layer.
  * @retval None
  */
void PPE_InitResultLayer(PPE_ResultLayer_InitTypeDef *layer_init_struct)
{
	if (layer_init_struct->xor_en == 0) {
		PPE->PPE_XOR_CTRL &= (~PPE_BIT_XOR);
	} else {
		PPE->PPE_XOR_CTRL |= (PPE_BIT_XOR);
	}

	PPE->PPE_RESULT_LAYER_ADDR_L = layer_init_struct->src_addr;
	PPE->PPE_RESULT_LAYER_PIC_SIZE = (
										 (PPE_RESULT_LAYER_PIC_HEIGHT(layer_init_struct->pic_height)) |
										 (PPE_RESULT_LAYER_PIC_WIDTH(layer_init_struct->pic_width))
									 );
	PPE->PPE_RESULT_LAYER_PIC_CFG = (
										(PPE_RESULT_LAYER_FORMAT(layer_init_struct->format)) |
										((layer_init_struct->bg_src == PPE_BACKGROUND_SOURCE_CONST_BG) ? 0 : PPE_BIT_RESULT_LAYER_BG_SRC) |
										(PPE_RESULT_LAYER_LINE_LEN(layer_init_struct->line_len))
									);
	PPE->PPE_RESULT_LAYER_CONST_BG = layer_init_struct->const_bg;
	PPE->PPE_RESULT_LAYER_BLK_SIZE = (
										 (PPE_RESULT_LAYER_BLK_HEIGHT(layer_init_struct->blk_height)) |
										 (PPE_RESULT_LAYER_BLK_WIDTH(layer_init_struct->blk_width))
									 );
}

/**
  * @brief  Init XOR function.
  * @param  None
  * @retval None
  */
void PPE_XOR_init(void)
{
	u32 width_xor = 90;
	u32 height_xor = 4;

	/*declaration of input/result layer initialization struct*/
	PPE_InputLayer_StructInit(&PPE_Input_Layer1);
	PPE_InputLayer_StructInit(&PPE_Input_Layer2);
	PPE_InputLayer_StructInit(&PPE_Input_Layer3);
	PPE_ResultLayer_StructInit(&PPE_Result_Layer);

	/*initial input layer1*/
	PPE_Input_Layer1.pic_width	 			= width_xor;
	PPE_Input_Layer1.pic_height				= height_xor;
	PPE_Input_Layer1.format					= 0;
	PPE_Input_Layer1.pic_src				= PPE_LAYER_SRC_FROM_DMA;
	PPE_Input_Layer1.interp					= PPE_INTERP_TYPE_Nearest_Neighbor;
	PPE_Input_Layer1.key_mode				= PPE_KEY_MODE_DISABLE;
	PPE_Input_Layer1.line_len				= PPE_Input_Layer1.pic_width * 4;
	PPE_Input_Layer1.const_ABGR8888_value	= 0xFFFFFFFF;
	PPE_Input_Layer1.win_min_x				= 0;
	PPE_Input_Layer1.win_min_y				= 0;
	PPE_Input_Layer1.win_max_x				= width_xor - 1;
	PPE_Input_Layer1.win_max_y				= height_xor - 1;
	PPE_Input_Layer1.key_min_bgr			= 0;
	PPE_Input_Layer1.key_max_bgr			= 0;
	PPE_Input_Layer1.scale_x				= 1;
	PPE_Input_Layer1.scale_y				= 1;
	PPE_Input_Layer1.angle					= 0;

	/*initial input layer1*/
	PPE_Input_Layer2.pic_width	 			= width_xor;
	PPE_Input_Layer2.pic_height				= height_xor;
	PPE_Input_Layer2.format					= 0;
	PPE_Input_Layer2.pic_src				= PPE_LAYER_SRC_FROM_DMA;
	PPE_Input_Layer2.interp					= PPE_INTERP_TYPE_Nearest_Neighbor;
	PPE_Input_Layer2.key_mode				= PPE_KEY_MODE_DISABLE;
	PPE_Input_Layer2.line_len				= PPE_Input_Layer2.pic_width * 4;
	PPE_Input_Layer2.const_ABGR8888_value	= 0xFFFFFFFF;
	PPE_Input_Layer2.win_min_x				= 0;
	PPE_Input_Layer2.win_min_y				= 0;
	PPE_Input_Layer2.win_max_x				= width_xor - 1;
	PPE_Input_Layer2.win_max_y				= height_xor - 1;
	PPE_Input_Layer2.key_min_bgr			= 0;
	PPE_Input_Layer2.key_max_bgr			= 0;
	PPE_Input_Layer2.scale_x				= 1;
	PPE_Input_Layer2.scale_y				= 1;
	PPE_Input_Layer2.angle					= 0;

	/*initial input layer1*/
	PPE_Input_Layer3.pic_width	 			= width_xor;
	PPE_Input_Layer3.pic_height				= height_xor;
	PPE_Input_Layer3.format					= 0;
	PPE_Input_Layer3.pic_src				= PPE_LAYER_SRC_FROM_DMA;
	PPE_Input_Layer3.interp					= PPE_INTERP_TYPE_Nearest_Neighbor;
	PPE_Input_Layer3.key_mode				= PPE_KEY_MODE_DISABLE;
	PPE_Input_Layer3.line_len				= PPE_Input_Layer3.pic_width * 4;
	PPE_Input_Layer3.const_ABGR8888_value	= 0xFFFFFFFF;
	PPE_Input_Layer3.win_min_x				= 0;
	PPE_Input_Layer3.win_min_y				= 0;
	PPE_Input_Layer3.win_max_x				= width_xor - 1;
	PPE_Input_Layer3.win_max_y				= height_xor - 1;
	PPE_Input_Layer3.key_min_bgr			= 0;
	PPE_Input_Layer3.key_max_bgr			= 0;
	PPE_Input_Layer3.scale_x				= 1;
	PPE_Input_Layer3.scale_y				= 1;
	PPE_Input_Layer3.angle					= 0;

	/*initial result layer*/
	PPE_Result_Layer.pic_width				= width_xor;
	PPE_Result_Layer.pic_height 			= height_xor;
	PPE_Result_Layer.format					= 0x0;
	PPE_Result_Layer.bg_src					= PPE_BACKGROUND_SOURCE_CONST_BG;
	PPE_Result_Layer.line_len				= PPE_Result_Layer.pic_width * 4;
	PPE_Result_Layer.const_bg				= 0;
	PPE_Result_Layer.blk_width				= PPE_Result_Layer.pic_width;
	PPE_Result_Layer.blk_height				= PPE_Result_Layer.pic_height;
	PPE_Result_Layer.xor_en					= 1;

	PPE_InitInputLayer(PPE_INPUT_LAYER1_INDEX, &PPE_Input_Layer1);
	PPE_InitInputLayer(PPE_INPUT_LAYER2_INDEX, &PPE_Input_Layer2);
	PPE_InitInputLayer(PPE_INPUT_LAYER3_INDEX, &PPE_Input_Layer3);
	PPE_InitResultLayer(&PPE_Result_Layer);

	InterruptRegister((IRQ_FUN) PPE_XOR_INTHandler, PPE_IRQ, (u32)NULL, INT_PRI_LOWEST);
	InterruptEn(PPE_IRQ, INT_PRI_LOWEST);
}

/**
  * @brief  start XOR.
  * @param  src1: XOR input data source address 1.
  * @param  src2: XOR input data source address 2.
  * @param  src3: XOR input data source address 3.
  * @param  dst: XOR output data destination address.
  * @retval None
  */
void PPE_XOR_process(u32 src1, u32 src2, u32 src3, u32 dst)
{
	u32 bus_cfg;

	PPE->Input_Layer[0].PPE_INPUT_LAYERx_ADDR_L = src1;
	PPE->Input_Layer[1].PPE_INPUT_LAYERx_ADDR_L = src2;
	PPE->Input_Layer[2].PPE_INPUT_LAYERx_ADDR_L = src3;
	PPE->PPE_RESULT_LAYER_ADDR_L = dst;

	if ((src1 >= 0x60000000) && (src1 < 0x80000000)) {
		bus_cfg = PPE->Input_Layer[0].PPE_INPUT_LAYERx_BUS_CFG;
		bus_cfg &= (~PPE_MASK_INPUT_LAYERx_AXCACHE);
		bus_cfg |= (PPE_INPUT_LAYERx_AXCACHE(0x6));
		bus_cfg &= (~PPE_MASK_INPUT_LAYERx_AXSIZE);
		bus_cfg |= (PPE_INPUT_LAYERx_AXSIZE(0x2));
		bus_cfg &= (~PPE_MASK_INPUT_LAYERx_MAX_AXLEN_LOG);
		PPE->Input_Layer[0].PPE_INPUT_LAYERx_BUS_CFG = bus_cfg;
	}

	if ((src2 >= 0x60000000) && (src2 < 0x80000000)) {
		bus_cfg = PPE->Input_Layer[1].PPE_INPUT_LAYERx_BUS_CFG;
		bus_cfg &= (~PPE_MASK_INPUT_LAYERx_AXCACHE);
		bus_cfg |= (PPE_INPUT_LAYERx_AXCACHE(0x6));
		bus_cfg &= (~PPE_MASK_INPUT_LAYERx_AXSIZE);
		bus_cfg |= (PPE_INPUT_LAYERx_AXSIZE(0x2));
		bus_cfg &= (~PPE_MASK_INPUT_LAYERx_MAX_AXLEN_LOG);
		PPE->Input_Layer[1].PPE_INPUT_LAYERx_BUS_CFG = bus_cfg;
	}

	if ((src3 >= 0x60000000) && (src3 < 0x80000000)) {
		bus_cfg = PPE->Input_Layer[2].PPE_INPUT_LAYERx_BUS_CFG;
		bus_cfg &= (~PPE_MASK_INPUT_LAYERx_AXCACHE);
		bus_cfg |= (PPE_INPUT_LAYERx_AXCACHE(0x6));
		bus_cfg &= (~PPE_MASK_INPUT_LAYERx_AXSIZE);
		bus_cfg |= (PPE_INPUT_LAYERx_AXSIZE(0x2));
		bus_cfg &= (~PPE_MASK_INPUT_LAYERx_MAX_AXLEN_LOG);
		PPE->Input_Layer[2].PPE_INPUT_LAYERx_BUS_CFG = bus_cfg;
	}

	if (src3 == 0) {
		PPE_LayerEn(PPE_INPUT_LAYER1_BIT | PPE_INPUT_LAYER2_BIT);
	} else {
		PPE_LayerEn(PPE_INPUT_LAYER1_BIT | PPE_INPUT_LAYER2_BIT | PPE_INPUT_LAYER3_BIT);
	}

	PPE_Cmd(ENABLE);
}

/**
  * @brief  Register xor interrupt handler callback.
  * @param  xor_cb: XOR function callback
  * @retval None
  */
void PPE_XOR_Int_handler(void (*xor_cb)(void))
{
	if (xor_cb) {
		user_func = xor_cb;
		PPE_MaskINTConfig(PPE_BIT_INTR_ST_ALL_OVER, ENABLE);
	} else {
		user_func = NULL;
		PPE_MaskINTConfig(PPE_BIT_INTR_ST_ALL_OVER, DISABLE);
	}
}


/** @} */

/** @} */

/**
  * @}
  */

