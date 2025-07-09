/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"


/**
  * @brief  reset pseudo i2c
  * @param  None
  * @retval None
  */
void pseudo_i2c_reset(void)
{
	//disable pseudo i2c fen clk
	RCC_PeriphClockCmd(APBPeriph_LEDC, APBPeriph_LEDC_CLOCK, DISABLE);

	//enable pseudo i2c fen clk
	RCC_PeriphClockCmd(APBPeriph_LEDC, APBPeriph_LEDC_CLOCK, ENABLE);
}


/**
  * @brief  set register protect status
  * @param  sequence: 0xa5a5a5a5 disable protect ;other enable protect
  * @retval None
  */
void pseudo_i2c_protect(u32 sequence)
{
	LEDC_PRO_TypeDef *pseudo_i2c_ctrl = PSEUDO_I2C_DEV;

	pseudo_i2c_ctrl->LEDC_PRO_SEQUENCE_CHECK1 = sequence;
}


/**
  * @brief  clear and reset ram data
  * @param  None
  * @retval None
  */
void pseudo_i2c_clear_ram(void)
{
	LEDC_PRO_TypeDef *pseudo_i2c_ctrl = PSEUDO_I2C_DEV;

	pseudo_i2c_ctrl->LEDC_PRO_LED_PALY_CTL0 |= LEDC_PRO_BIT_R_LED_BUF_CLEAR;
}



/**
  * @brief  set pseudo i2c clk
  * @param  div clock_di_factor = div*2 , minimum value is 2
  * @retval None
  */
void pseudo_i2c_set_clkdiv(u32 div)
{
	LEDC_PRO_TypeDef *pseudo_i2c_ctrl = PSEUDO_I2C_DEV;

	assert_param(IS_VALID_DIV(div));

	// dsiable control register portect
	pseudo_i2c_protect(REG_PROTECT_DISABLE);

	pseudo_i2c_ctrl->LEDC_PRO_PSDO_I2C_CTL &= ~LEDC_PRO_MASK_R_I2C_CLK_DIV;
	pseudo_i2c_ctrl->LEDC_PRO_PSDO_I2C_CTL |= LEDC_PRO_R_I2C_CLK_DIV(div);

	//enable control register portect
	pseudo_i2c_protect(REG_PROTECT_ENABLE);
}

/**
  * @brief  set pseudo i2c data number
  * @param  len need to be less than 65
  * @retval None
  */
void pseudo_i2c_set_len(u32 len)
{
	LEDC_PRO_TypeDef *pseudo_i2c_ctrl = PSEUDO_I2C_DEV;

	assert_param(IS_VALID_NUM(len));

	// dsiable control register portect
	pseudo_i2c_protect(REG_PROTECT_DISABLE);

	pseudo_i2c_ctrl->LEDC_PRO_PSDO_I2C_CTL &= ~LEDC_PRO_MASK_R_I2C_DATA_NUM;
	pseudo_i2c_ctrl->LEDC_PRO_PSDO_I2C_CTL |= LEDC_PRO_R_I2C_DATA_NUM(len);

	//enable control register portect
	pseudo_i2c_protect(REG_PROTECT_ENABLE);
}



/**
  * @brief  init pseudo i2c
  * @param div clock_di_factor = div*2 , minimum value is 2
  * @retval None
  */
void pseudo_i2c_init(u32 div)
{
	LEDC_PRO_TypeDef *pseudo_i2c_ctrl = PSEUDO_I2C_DEV;

	//reset i2c
	pseudo_i2c_reset();

	//set bit7, switch to i2c mode
	pseudo_i2c_ctrl->LEDC_PRO_PSDO_I2C_CTL |= LEDC_PRO_BIT_R_PSEDO_I2C_EN;

	// dsiable control register portect
	pseudo_i2c_protect(REG_PROTECT_DISABLE);

	//set clkdiv
	pseudo_i2c_ctrl->LEDC_PRO_PSDO_I2C_CTL |= LEDC_PRO_R_I2C_CLK_DIV(div);

	//enable control register portect
	pseudo_i2c_protect(REG_PROTECT_ENABLE);

}

/**
  * @brief  write data to ram
  * @param cmd: AIP provides four instructions for setting display related functions
  * @param data: Pointer to RAM data buffer
  * @param len: buffer len
  * @retval None
  */
void pseudo_i2c_write_ram(u8 cmd, u8 *data, int len)
{
	int i;
	LEDC_PRO_TypeDef *pseudo_i2c_ctrl = PSEUDO_I2C_DEV;

	assert_param(IS_VALID_NUM(len));

	//polling i2c status, until idle
	while ((pseudo_i2c_ctrl->LEDC_PRO_PSDO_I2C_CTL  & LEDC_PRO_BIT_R_I2C_TX_STATUS) == 0);

	//clear ram
	pseudo_i2c_ctrl->LEDC_PRO_LED_PALY_CTL0 |= LEDC_PRO_BIT_R_LED_BUF_CLEAR;

	HAL_WRITE8(LEDC_REG_BASE, RAM_CMD_OFFSET, cmd);

	if (data != NULL) {

		for (i = 0; i < len; i++) {
			HAL_WRITE8(LEDC_REG_BASE + RAM_DATA_OFFSET, i, data[i]);

		}
		pseudo_i2c_set_len(len + 1);
	} else {
		len = 0;
		pseudo_i2c_set_len(1);
	}

	//trigger i2c tx data
	pseudo_i2c_ctrl->LEDC_PRO_SEQUENCE_CHECK0 = TRIGGER_SEQUENCE1;
	pseudo_i2c_ctrl->LEDC_PRO_SEQUENCE_CHECK0 = TRIGGER_SEQUENCE2;

	//polling sequence pass
	while ((pseudo_i2c_ctrl->LEDC_PRO_LED_PALY_CTL0  & LEDC_PRO_BIT_R_SEQ_PASS_FLAG) == 0);
}

