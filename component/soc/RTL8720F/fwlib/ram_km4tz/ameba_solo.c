/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *const TAG = "SOLO";

static void aes_sha_dma_abort(u8 GDMA_ChNum)
{
	u32 timeout = 500;
	u32 reg_val = 0;
	GDMA_TypeDef *GDMA = ((GDMA_TypeDef *) AES_SHA_DMA_REG_BASE);
	if (TrustZone_IsSecure()) {
		GDMA = ((GDMA_TypeDef *) AES_SHA_DMA_REG_BASE_S);
	}

	assert_param(IS_GDMA_ChannelNum(GDMA_ChNum));

	GDMA->CH[GDMA_ChNum].GDMA_CFGx_L |= GDMA_BIT_CFGx_L_CH_SUSP;

	/*If ChEnReg[GDMA_ChNum] is not equal to 0, it means that
	  the channel is working and the Suspend status must be checked.*/
	while (timeout--) {
		if ((GDMA_GET_CHENREG_L_1_CH_EN(GDMA->GDMA_CHENREG_L_1) & BIT(GDMA_ChNum)) == 0 || \
			(GDMA->CH[GDMA_ChNum].GDMA_CFGx_L & GDMA_BIT_CFGx_L_INACTIVE)) {
			break;
		}
	}
	/*If the channel is still active after the timeout period, resume is required*/
	if (timeout == 0) {
		RTK_LOGI(TAG, "fail to abort dma channel %d !\n", GDMA_ChNum);
	}

	reg_val |= GDMA_CHENREG_L_1_CH_EN_WE(BIT(GDMA_ChNum));
	reg_val &= ~GDMA_CHENREG_L_1_CH_EN(BIT(GDMA_ChNum));
	GDMA->GDMA_CHENREG_L_1 = reg_val;
}


static void process_timeout_flow(void)
{
	u32 intr_bit = RXI300_BIT_TMO_CLR_SW_RST_N_PLFM1_MA | RXI300_BIT_TMO_CLR_SW_RST_N_CPU1_MA;
	u32 cnt = 0;

	/* set corresponding reset of timeout function register (TIMEOUT_CLEAR_N) to 0 */
	RXI_300->TIMEOUT_CLEAR_N &= (~intr_bit);

	/* wait corresponding reset of timeout function register (TIMEOUT_CLEAR_N) to 1*/
	while (1) {
		if (((RXI_300->TIMEOUT_CLEAR_N & intr_bit) == intr_bit) || (cnt++ > 10)) {
			break;
		}
		DelayUs(1);
	}

	/* clear interrupt */
	RXI_300->INT_CLR = RXI300_BIT_ERR_INTR_CLR;
}

void SOLO_Plfm1ResetHandler(void)
{
	u32 cnt = 0;
	RTK_LOGI(TAG, "reset reason: %x\n", HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_BOOT_REASON_HW));

	/* step1: write timeout_trigger_n to 0 and check tmo_chk to 1, to clear residual transaction from plfm1*/
	RXI_300->MST_EN &= (~RXI300_BIT_TIMEOUT_TRIGGER_N);
	while (1) {
		if (((RXI_300->MST_TIMEOUT_STATUS & (RXI300_BIT_TMO_TRIG_PLFM1 | RXI300_BIT_TMO_TRIG_CPU1)) ==
			 (RXI300_BIT_TMO_TRIG_PLFM1 | RXI300_BIT_TMO_TRIG_CPU1)) || (cnt++ > 10)) {
			break;
		}
		DelayUs(1);
	}

	/* step2: write timeout_trigger_n to 1 and check tmo_chk to 0, to back to origin value */
	RXI_300->MST_EN |= RXI300_BIT_TIMEOUT_TRIGGER_N;
	cnt = 0;
	while (1) {
		if (((RXI_300->MST_TIMEOUT_STATUS & (RXI300_BIT_TMO_TRIG_PLFM1 | RXI300_BIT_TMO_TRIG_CPU1)) == 0) || (cnt++ > 10)) {
			break;
		}
		DelayUs(1);
	}

	/* step3: disable all plfm1 master agent in plfm0 and  ensure MST/SLV_STAT and PLFM1_SLV are in idle state */
	RXI_300->MST_EN |= RXI300_BIT_PLFM1_MASTER_DISABLE;
	cnt = 0;
	while (1) {
		if (((RXI_300->MST_TIMEOUT_STATUS & (RXI300_BIT_MA_STATUS_PLFM1 | RXI300_BIT_MA_STATUS_CPU1)) == 0) || (cnt++ > 10)) {
			break;
		}
		DelayUs(1);
	}

	/* step4: enable isoluation between plfm0 and plfm1 */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PLAT_CTRL, HAL_READ32(SYSTEM_CTRL_BASE,
				REG_LSYS_PLAT_CTRL) | LSYS_BIT_PLFM0_ISOLATION_ENABLE);

	/* step5: plfm1 is waiting to release reset signal, not release it to let cpu1 boot */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_DUMMY_10, HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_DUMMY_10) | LSYS_BIT_PLFM1_RESET_CONFIRM);

	/* step6: clear aon boot reason bit to clear interrupt, and set time delay to make sure plfm1 is boot */
	DelayUs(40);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_BOOT_REASON_HW, AON_BIT_RSTF_PLFM1 | AON_BIT_RSTF_WDG0_GLB);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_DUMMY_10, HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_DUMMY_10) & (~LSYS_BIT_PLFM1_RESET_CONFIRM));

	/* step7: enable all plfm1 master agent in plfm0. */
	RXI_300->MST_EN &= (~RXI300_BIT_PLFM1_MASTER_DISABLE);

	/* step8: execute timeout processing flow. reset timeout component, clear timeout interrupt */
	process_timeout_flow();

	/* step9: disable isoluation between plfm0 and plfm1 */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PLAT_CTRL, HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PLAT_CTRL) & (~LSYS_BIT_PLFM0_ISOLATION_ENABLE));

	/* step10: abort dma and reset crypto */
	for (u8 i = 0; i < 2; i++) {
		aes_sha_dma_abort(i);
	}

	RCC_PeriphClockCmd(APBPeriph_CRYPTO, APBPeriph_CLOCK_NULL, DISABLE);
	RCC_PeriphClockCmd(APBPeriph_CRYPTO, APBPeriph_CLOCK_NULL, ENABLE);
	CRYPTO_Init();

	/* step11: let cpu1 run */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_CFG, HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_CFG) | LSYS_BIT_BOOT_CPU1_RUN);

	/* step12: clear interrupt trigger by sw write timeout_trigger_n */
	NVIC_ClearPendingIRQ(RXI300_IRQ);
}


u32 SOLO_Rxi300TimoutHandler(void *param)
{
	UNUSED(param);
	RTK_LOGI(TAG, "rxi300_handler: %x \n", RXI_300->CODE);

	InterruptDis(RXI300_IRQ);

	/* timeout occurs */
	if (RXI_300->CODE == 0x2C || RXI_300->CODE == 0x2D || RXI_300->CODE == 0x2E) {
		/* clear interrupt */
		process_timeout_flow();

		/* let platform1 reset */
		Platform1_Reset();
	}

	InterruptEn(RXI300_IRQ, INT_PRI_HIGHEST);

	return 0;
}
