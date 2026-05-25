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
#ifdef RTL8720F_TODO
	//disable pseudo i2c fen clk
	RCC_PeriphClockCmd(APBPeriph_LEDC, APBPeriph_LEDC_CLOCK, DISABLE);

	//enable pseudo i2c fen clk
	RCC_PeriphClockCmd(APBPeriph_LEDC, APBPeriph_LEDC_CLOCK, ENABLE);
#else
	RTK_LOGW(NOTAG, "function %s needs to be updated!!!\n", __FUNCTION__);
#endif
}


/**
  * @brief  set register protect status
  * @param  sequence: 0xa5a5a5a5 disable protect ;other enable protect
  * @retval None
  */
void pseudo_i2c_protect(LEDC_PRO_TypeDef *PI2Cx, u32 sequence)
{
	PI2Cx->LEDC_PRO_SEQUENCE_CHECK1 = sequence;
}


/**
  * @brief  clear and reset ram data
  * @param  None
  * @retval None
  */
void pseudo_i2c_clear_ram(LEDC_PRO_TypeDef *PI2Cx)
{
	PI2Cx->LEDC_PRO_LED_PALY_CTL0 |= LEDC_PRO_BIT_R_LED_BUF_CLEAR;
}


/**
  * @brief  set pseudo i2c clk
  * @param  div clock_di_factor = div*2 , minimum value is 2
  * @retval None
  */
void pseudo_i2c_set_clkdiv(LEDC_PRO_TypeDef *PI2Cx, u32 div)
{
	// LEDC_PRO_TypeDef *pseudo_i2c_ctrl = PSEUDO_I2C_DEV;

	assert_param(IS_VALID_DIV(div));

	// dsiable control register portect
	pseudo_i2c_protect(PI2Cx, REG_PROTECT_DISABLE);

	PI2Cx->LEDC_PRO_PSDO_I2C_CTL &= ~LEDC_PRO_MASK_R_I2C_CLK_DIV;
	PI2Cx->LEDC_PRO_PSDO_I2C_CTL |= LEDC_PRO_R_I2C_CLK_DIV(div);

	//enable control register portect
	pseudo_i2c_protect(PI2Cx, REG_PROTECT_ENABLE);
}

/**
  * @brief  set pseudo i2c data number
  * @param  len need to be less than 65
  * @retval None
  */
void pseudo_i2c_set_len(LEDC_PRO_TypeDef *PI2Cx, u32 len)
{
	assert_param(IS_VALID_NUM(len));

	// dsiable control register portect
	pseudo_i2c_protect(PI2Cx, REG_PROTECT_DISABLE);

	PI2Cx->LEDC_PRO_PSDO_I2C_CTL &= ~LEDC_PRO_MASK_R_I2C_DATA_NUM;
	PI2Cx->LEDC_PRO_PSDO_I2C_CTL |= LEDC_PRO_R_I2C_DATA_NUM(len);

	//enable control register portect
	pseudo_i2c_protect(PI2Cx, REG_PROTECT_ENABLE);
}



/**
  * @brief  init pseudo i2c
  * @param div clock_di_factor = div*2 , minimum value is 2
  * @retval None
  */
void pseudo_i2c_init(LEDC_PRO_TypeDef *PI2Cx, u32 div)
{
	//reset i2c
	pseudo_i2c_reset();

	//set bit7, switch to i2c mode
	PI2Cx->LEDC_PRO_PSDO_I2C_CTL |= LEDC_PRO_BIT_R_PSEDO_I2C_EN;

	// dsiable control register portect
	pseudo_i2c_protect(PI2Cx, REG_PROTECT_DISABLE);

	//set clkdiv
	PI2Cx->LEDC_PRO_PSDO_I2C_CTL |= LEDC_PRO_R_I2C_CLK_DIV(div);

	//enable control register portect
	pseudo_i2c_protect(PI2Cx, REG_PROTECT_ENABLE);

}

/**
  * @brief  write data to ram
  * @param cmd: AIP provides four instructions for setting display related functions
  * @param data: Pointer to RAM data buffer
  * @param len: buffer len
  * @retval None
  */
void pseudo_i2c_write_ram(LEDC_PRO_TypeDef *PI2Cx, u8 cmd, u8 *data, int len)
{
	int i;

	assert_param(IS_VALID_NUM(len));

	u32 ledc_ram_base = (u32) & (PI2Cx->LEDC_PRO_LED_RAM);

	//polling i2c status, until idle
	while ((PI2Cx->LEDC_PRO_PSDO_I2C_CTL  & LEDC_PRO_BIT_R_I2C_TX_STATUS) == 0);

	//clear ram
	PI2Cx->LEDC_PRO_LED_PALY_CTL0 |= LEDC_PRO_BIT_R_LED_BUF_CLEAR;

	HAL_WRITE8(ledc_ram_base, 0, cmd);

	if (data != NULL) {

		for (i = 0; i < len; i++) {
			HAL_WRITE8(ledc_ram_base + 0x1, i, data[i]);
		}
		pseudo_i2c_set_len(PI2Cx, len + 1);
	} else {
		len = 0;
		pseudo_i2c_set_len(PI2Cx, 1);
	}

	//trigger i2c tx data
	PI2Cx->LEDC_PRO_SEQUENCE_CHECK0 = TRIGGER_SEQUENCE1;
	PI2Cx->LEDC_PRO_SEQUENCE_CHECK0 = TRIGGER_SEQUENCE2;

	//polling sequence pass
	while ((PI2Cx->LEDC_PRO_LED_PALY_CTL0  & LEDC_PRO_BIT_R_SEQ_PASS_FLAG) == 0);
}

