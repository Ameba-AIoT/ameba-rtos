/**
  ******************************************************************************
  * @file    ameba_trng.c
  * @author
  * @version V1.0.0
  * @date    2020-10-30
  * @brief   This file provides firmware functions to manage the following
  *          functionalities of the TRNG peripheral:
  *           - Get Rand data
  *           - Interrupt operate
  *           - Security configure
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */
#include "ameba_soc.h"

/**
  * @brief  Change the TRNG to a Security control Only IP.If configure TRNG to a Security only IP,
  * The No-security world can only Read Data from FIFO.
  * @param  NewState: specifies the state of the interrupt.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
HAL_ROM_TEXT_SECTION _LONG_CALL_
u32 TRNG_Security_Only(bool NewState)
{
	TRNG_TypeDef *TRNG  = TRNG_SDEV;
	u32 Temp;

	if (!TrustZone_IsSecure()) {
		DBG_8195A("Securiy CTRL can only changes from security word!!\n");
		return 0;
	}

	Temp = TRNG->TRNG_CTRL;
	Temp &= ~TRNG_MASK_SECURITY_CONTROL;

	if (NewState == DISABLE) {
		Temp |= TRNG_SECURITY_CONTROL(TRNG_NO_SECURITY_EN);
	}

	TRNG->TRNG_CTRL = Temp;
	// _SUCCESS(1)/ _FAIL(0) removed from SDK, change return value to RTK_SUCCESS(0) / RTK_FAIL(-1) in the future
	return 1;
}

/**
  * @brief  Enable or Disable TRNG INTConfig.
  * @param  TRNG_IT: specifies the TRNG interrupt sources to be enabled or disabled.
  *          This parameter can be one or combinations of the following values:
  *            @arg TRNG: TRNG_BIT_BUFFER_FULL_INTERRUPT_ENABLE Buffer FULL Interrupt
  *            @arg TRNG_BIT_APT0_FAIL_INTERRUPT_ENABLE: APT0_FAIL Interrupt
  *            @arg TRNG_BIT_RCT1_FAIL_INTERRUPT_ENABLE: RTC1 FAIL Interrupt
  *            @arg TRNG_BIT_RCT0_FAIL_INTERRUPT_ENABLE: RCT0 FAIL Interrupt
  *            @arg TRNG_BIT_READ_ERROR_INTERRUPT_ENABLE: Read ERROR Interrupt
  * @param  NewState: specifies the state of the interrupt.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
HAL_ROM_TEXT_SECTION _LONG_CALL_
void TRNG_INTConfig(u32 TRNG_IT, u32 NewState)
{
	TRNG_TypeDef *TRNG  = TRNG_DEV;

	if (TrustZone_IsSecure()) {
		TRNG = TRNG_SDEV;
	}

	if (NewState == ENABLE) {
		TRNG->TRNG_IER |= TRNG_IT;
	} else {
		TRNG->TRNG_IER &= ~TRNG_IT;
	}
}

/**
  * @brief  Clears the specified TRNG interrupt pending bit.
  * @param  INTrBit: specifies the interrupt to be cleared.
  *          This parameter can be one of the following values:
  *            @arg TRNG: TRNG_BIT_BUFFER_FULL_INTERRUPT_CLEAR Buffer FULL Interrupt
  *            @arg TRNG_BIT_APT0_FAIL_INTERRUPT_CLEAR: APT0_FAIL Interrupt
  *            @arg TRNG_BIT_RCT1_FAIL_INTERRUPT_CLEAR: RTC1 FAIL Interrupt
  *            @arg TRNG_BIT_RCT0_FAIL_INTERRUPT_CLEAR: RCT0 FAIL Interrupt
  *            @arg TRNG_BIT_READ_ERROR_INTERRUPT_CLEAR: Read ERROR Interrupt
  * @retval None
  */
HAL_ROM_TEXT_SECTION _LONG_CALL_
void TRNG_ClearINT(u32 INTrBit)
{
	TRNG_TypeDef *TRNG  = TRNG_DEV;

	if (TrustZone_IsSecure()) {
		TRNG = TRNG_SDEV;
	}

	TRNG->TRNG_ICR |= INTrBit;
}

/**
  * @brief  Get TRNG interrupt status.
  * @retval interrupt status
  */
HAL_ROM_TEXT_SECTION _LONG_CALL_
u32 TRNG_GetINT(void)
{
	TRNG_TypeDef *TRNG  = TRNG_DEV;

	if (TrustZone_IsSecure()) {
		TRNG = TRNG_SDEV;
	}

	return TRNG->TRNG_ISR;
}

HAL_ROM_TEXT_SECTION _LONG_CALL_
u32 TRNG_irq_rom(UNUSED_WARN_DIS void *Data)
{
	u32 INTrBit = TRNG_GetINT();

	if (INTrBit) {
		//DBG_8195A("TRNG FAIL %x \n", INTrBit);
		TRNG_ClearINT(INTrBit);
	}

	return 1;
}

/**
  * @brief  TRNG pseudo model init, the model only used in FPGA and paladium
  * @retval none
  */
HAL_ROM_TEXT_SECTION _LONG_CALL_
static inline void TRNG_Pseudo_model_init(void)
{
	TRNG_TypeDef *TRNG  = TRNG_SDEV;

	if ((SYSCFG_CHIPType_Get() == CHIP_TYPE_FPGA) || \
		(SYSCFG_CHIPType_Get() == CHIP_TYPE_PALADIUM)) {

		//force ROSC sensor lock
		TRNG->TRNG_ROSC_CAL_CTRL  = 0x010D0109;

		//set RCT0 cutoff to FF
		TRNG->TRNG_RCT0 = 0x0000FF01;
		//set RCT1 cutoff to FF
		TRNG->TRNG_RCT1 = 0x0000FF01;
		//set APT cutoff to 3FF
		TRNG->TRNG_APT = 0x13FF0003;
		// sw reset
		TRNG->TRNG_SW_RST = TRNG_BIT_SW_RESET;
	}
}

/**
  * @brief  TRNG init
  * @param  InitConsol
  *            @arg TRUE: shell_init_rom & InterruptRegister will be excute
  *            @arg FALSE: without shell_init_rom & InterruptRegister, used for just reinit HW like resume from PG
  * @retval none
  */
HAL_ROM_TEXT_SECTION _LONG_CALL_
void TRNG_Init(void)
{
	/* move to BOOT_ROM_ResetVsr.Release TRNG reset and clock */
	//RCC_PeriphClockCmd(APBPeriph_TRNG, APBPeriph_TRNG_CLOCK, ENABLE);

	/*only needed for FPGA and paldium*/
	TRNG_Pseudo_model_init();

	/* Set TRNG software no-security configurable */
	//TRNG_Security_Only(DISABLE);
}

/**
  * @brief  Do a random delay to avoid attract
  * @param  Non
  * @retval none
  */
HAL_ROM_TEXT_SECTION _LONG_CALL_
_OPTIMIZE_NONE_
void TRNG_Random_Delay(void)
{
	u32 DelayTime;

	/*Should Disable random delay by default,
	Also if the random delay should designed as
	a functionality that can't disabled once enabled*/
	if (SYSCFG_OTP_RandDelayEn() == FALSE) {
		return;
	}

	DelayTime = _rand() & 0x3FF;
	while (DelayTime--);
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
