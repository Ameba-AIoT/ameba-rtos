/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup ETHERNET ETHERNET
  *
  * @brief  ETHERNET driver modules
  * @{
  */

/**
 * @brief  Fills the EEE_Config structure with default IEEE 802.3az values.
 *         Use this to ensure safe defaults before tweaking specific parameters.
 * @param  EEE_Config Pointer to the EEE configuration structure.
 */
void Ethernet_EEE_StructInit(ETH_EEE_InitTypeDef *EEE_Config)
{
	/* 1. Global Direction: Enable both TX (Sleep) and RX (Wake) logic */
	EEE_Config->EnableTx = ENABLE;
	EEE_Config->EnableRx = ENABLE;

	/* 2. Standard Timings (Units are specific to hardware register definitions) */
	/* Tw (Wake Time): Min 30us for 100Base-TX.
	   Assuming register unit is 1us -> Set 30 or slightly higher for safety */
	EEE_Config->Tw_WakeTime = 30;

	/* Tr (LPI Interval): Min duration of LPI signal. Default ~20-22us */
	EEE_Config->Tr_LpiInterval = 22;

	/* Td (Decision Delay): Wait time before entering sleep. Unit: 8us.
	   Value 5 = 40us delay. Prevents sleeping during very short gaps. */
	EEE_Config->Td_TxDelay = 5;

	/* Tp (Pause Time): Default 10us */
	EEE_Config->Tp_PauseTime = 10;

	/* 3. Thresholds */
	/* TxTrafficThresh: 125 bytes is a standard balance point for 100M */
	EEE_Config->TxTrafficThresh = 0x7D;

	/* 4. Policies (Simplicity for Customer) */
	/* Sleep Policy:
	   SLEEP_QUEUE_EMPTY is the safest. It sleeps as soon as there is no data. */
	EEE_Config->SleepPolicy = ETH_EEE_SLEEP_QUEUE_EMPTY;

	/* Wake Policy:
	   WAKE_ANY_DATA ensures no packet loss. Wake up on any packet. */
	EEE_Config->WakePolicy = ETH_EEE_WAKE_ANY_DATA;
}
/**
  * @brief  Initializes the EEE (Energy Efficient Ethernet) functionality according to the 6-step procedure.
  * @param  ETHx Pointer to ETH register base address.
  * @param  EEE_Config Pointer to the configuration structure.
  */
void Ethernet_EEE_Init(ETHERNET_TypeDef *ETHx, ETH_EEE_InitTypeDef *EEE_Config)
{
	u32 temp_cr1 = 0;
	u32 temp_cr3 = 0;
	u32 temp_cr4 = 0;

	/* If the feature is disabled globally, clear all enable bits and return immediately */
	if ((EEE_Config->EnableTx) == 0 && (EEE_Config->EnableRx == 0)) {
		ETHx->ETH_EEE_CR1 &= ~(BIT_EN_EEE | BIT_EN_EEE_TX | BIT_EN_EEE_RX);
		return;
	}

	/* Read current CR1 value to preserve other settings */
	temp_cr1 = ETHx->ETH_EEE_CR1;

	/* 1. Configure Timer Unit - ETH_EEE_CR1. */
	temp_cr1 &= ~(MASK_EEE_TIMER_UNIT_100_1 | MASK_EEE_TIMER_UNIT_100_2);

	/* Set units as per requirement for 100M:
	   EEE_TIMER_UNIT_100_1 = 00 (1us) -> Base for Tw, Tr, Tp
	   EEE_TIMER_UNIT_100_2 = 00 (8us) -> Base for Td
	*/
	temp_cr1 |= EEE_TIMER_UNIT_100_1(0) | EEE_TIMER_UNIT_100_2(0);

	/* 2. Set Protocol Timers - ETH_EEE_CR3.
	* Configure CR3 for 100M mode operation based on struct params
	*/
	temp_cr3 = EEE_TIMER_TW_100(EEE_Config->Tw_WakeTime)    | \
			   EEE_TIMER_TR_100(EEE_Config->Tr_LpiInterval) | \
			   EEE_TIMER_TD_100(EEE_Config->Td_TxDelay)     | \
			   EEE_TIMER_TP_100(EEE_Config->Tp_PauseTime);

	ETHx->ETH_EEE_CR3 = temp_cr3;

	/* 3. Configure Decision Thresholds - ETH_EEE_CR4.*/
	temp_cr4 = ETHx->ETH_EEE_CR4 & (~MASK_EEE_TX_THR_100);

	/* Set the 100M TX Byte Threshold */
	temp_cr4 |= EEE_TX_THR_100(EEE_Config->TxTrafficThresh);

	ETHx->ETH_EEE_CR4 = temp_cr4;

	/* 4. Enable Enter & Exit Policies - ETH_EEE_CR1. */
	temp_cr1 &= ~ETH_EEE_SLEEP_ALL_MASK; /* Clears REQ_SET 0, 1, 2 */
	temp_cr1 &= ~ETH_EEE_WAKE_ALL_MASK;  /* Clears WAKE_SET 0, 1 */

	/* Apply new policies from structure (Masking ensures safety) */
	temp_cr1 |= (EEE_Config->SleepPolicy & ETH_EEE_SLEEP_ALL_MASK);
	temp_cr1 |= (EEE_Config->WakePolicy  & ETH_EEE_WAKE_ALL_MASK);

	/* 5. Global Enable - ETH_EEE_CR1.*/
	temp_cr1 &= ~(BIT_EN_EEE | BIT_EN_EEE_TX | BIT_EN_EEE_RX);

	/* Write configuration to register first (Policies & Timers) */
	ETHx->ETH_EEE_CR1 = temp_cr1;

	if (EEE_Config->EnableTx) {
		temp_cr1 |= BIT_EN_EEE_TX;
	}
	if (EEE_Config->EnableRx) {
		temp_cr1 |= BIT_EN_EEE_RX;
	}

	/* Enable General EEE bit and internal statistic timers (TXTimer & RXTimer) */
	temp_cr1 |= BIT_EN_EEE | BIT_EEE_TXTIMER_EN | BIT_EEE_RXTIMER_EN;

	ETHx->ETH_EEE_CR1 = temp_cr1;
}

/**
  * @brief  Get EEE Real-time Status (Step 6 - Real-time Status)
  * @param  ETHx Pointer to ETH register base address.
  * @retval Status Bitmap. The return value is a combination of the following flags:
  *         - BIT_EEE_TX_STS:    TX is in LPI mode (Bit 18)
  *         - BIT_EEE_RX_STS:    RX is in LPI mode (Bit 17)
  *         - BIT_EEE_STS:       Both TX and RX are in LPI mode simultaneously (Bit 19)
  *         - BIT_EEE_PAUSEFLAG: LPI request triggered by RX Pause frame (Bit 16)
  */
u32 Ethernet_EEE_GetStatus(ETHERNET_TypeDef *ETHx)
{
	/* Read the EEE Control/Status Register (assuming CR1 contains these bits) */
	u32 tmpreg = ETHx->ETH_EEE_CR1;
	u32 status = 0;

	/* Check if TX is in LPI mode using the provided GET macro */
	if (GET_EEE_TX_STS(tmpreg)) {
		status |= BIT_EEE_TX_STS;
	}

	/* Check if RX is in LPI mode using the provided GET macro */
	if (GET_EEE_RX_STS(tmpreg)) {
		status |= BIT_EEE_RX_STS;
	}

	/* Check if BOTH are in LPI mode using the provided GET macro */
	if (GET_EEE_STS(tmpreg)) {
		status |= BIT_EEE_STS;
	}

	/* Check for Pause Frame LPI trigger using the provided GET macro */
	if (GET_EEE_PAUSEFLAG(tmpreg)) {
		status |= BIT_EEE_PAUSEFLAG;
	}

	return status;
}

/**
  * @brief  Get Cumulative LPI Power Saving Time (Step 6 - Statistics)
  * @param  ETHx Pointer to ETH register base address.
  * @param  TxLpiTime Pointer to store TX LPI accumulation time (us). Can be NULL.
  * @param  RxLpiTime Pointer to store RX LPI accumulation time (us). Can be NULL.
  */
void Ethernet_EEE_GetSaveTime(ETHERNET_TypeDef *ETHx, u32 *TxLpiTime, u32 *RxLpiTime)
{
	/* Read LPI Time registers if pointers are valid */
	if (TxLpiTime) {
		*TxLpiTime = GET_EEE_TXLPI_TIME(ETHx->ETH_EEE_LPI_TM1_REG);
	}
	if (RxLpiTime) {
		*RxLpiTime = GET_EEE_RXLPI_TIME(ETHx->ETH_EEE_LPI_TMO_REG);
	}
}

static const char *const TAG = "ETH";

/* Exported functions --------------------------------------------------------*/
/** @defgroup ETHERNET_Exported_Functions ETHERNET Exported Functions
  * @{
  */

/**
 * @brief  Clear all pending interrupt status bits.
 *         Usually called during initialization or error recovery.
 */
void Ethernet_ClearAllINT(void)
{
	ETHERNET_TypeDef *ETHx = ((ETHERNET_TypeDef *) RMII_REG_BASE);

	/* Construct mask of all possible ISR status bits */
	u32 all_isr_mask = BIT_ISR_ROK | BIT_ISR_RER_OVF | BIT_ISR_RER_RUNT |
					   BIT_ISR_TOK_TI | BIT_ISR_TER | BIT_ISR_LINKCHG |
					   BIT_ISR_CNT_WRAP | BIT_ISR_SWINT | BIT_ISR_TDU |
					   BIT_ISR_RDU1 | BIT_ISR_RDU2 | BIT_ISR_RDU3 |
					   BIT_ISR_RDU4 | BIT_ISR_RDU5 | BIT_ISR_RDU6;

	/* Write 1 to clear status bits */
	ETHx->ETH_ISR_AND_IMR |= all_isr_mask;
}

/**
 * @brief  Get currently pending and enabled interrupt events.
 *         Converts hardware register bits to logical software events.
 * @return Bitmask of pending events (enum eth_link_event).
 */
u32 Ethernet_GetPendingINT(void)
{
	ETHERNET_TypeDef *ETHx = ((ETHERNET_TypeDef *) RMII_REG_BASE);

	/* Read the combined ISR (Status) and IMR (Mask) register */
	volatile u32 raw_reg = ETHx->ETH_ISR_AND_IMR;
	u32 int_status = ETH_EVT_NO_EVENT;

	/*
	 * Logic: An interrupt is valid only if both the Status bit (ISR)
	 * and the Mask bit (IMR) are set.
	 */

	/* 1. Check RX OK */
	if ((raw_reg & BIT_ISR_ROK) && (raw_reg & BIT_IMR_ROK)) {
		int_status |= ETH_EVT_RX_DONE;
	}

	/* 2. Check TX OK */
	if ((raw_reg & BIT_ISR_TOK_TI) && (raw_reg & BIT_IMR_TOK_TI)) {
		int_status |= ETH_EVT_TX_DONE;
	}

	/* 3. Check Link Change */
	if ((raw_reg & BIT_ISR_LINKCHG) && (raw_reg & BIT_IMR_LINKCHG)) {
		int_status |= ETH_EVT_LINK_CHG;
	}

	/* 4. Check RX Errors (Overflow or Runt) */
	if (((raw_reg & BIT_ISR_RER_OVF) && (raw_reg & BIT_IMR_RER_OVF)) ||
		((raw_reg & BIT_ISR_RER_RUNT) && (raw_reg & BIT_IMR_RER_RUNT))) {
		int_status |= ETH_EVT_RX_ERROR;
	}

	/* 5. Check TX Errors */
	if ((raw_reg & BIT_ISR_TER) && (raw_reg & BIT_IMR_TER)) {
		int_status |= ETH_EVT_TX_ERROR;
	}

	return int_status;
}

/**
 * @brief  Clear specified interrupt events.
 *         Converts software events back to hardware register write-1-to-clear.
 * @param  int_events Bitmask of events to clear.
 */
void Ethernet_ClearINT(uint32_t int_events)
{
	ETHERNET_TypeDef *ETHx = ((ETHERNET_TypeDef *) RMII_REG_BASE);
	u32 clear_mask = 0;

	if (int_events & ETH_EVT_RX_DONE) {
		clear_mask |= BIT_ISR_ROK;
	}

	if (int_events & ETH_EVT_TX_DONE) {
		clear_mask |= BIT_ISR_TOK_TI;
	}

	if (int_events & ETH_EVT_LINK_CHG) {
		clear_mask |= BIT_ISR_LINKCHG;
	}

	if (int_events & ETH_EVT_RX_ERROR) {
		clear_mask |= (BIT_ISR_RER_OVF | BIT_ISR_RER_RUNT);
	}

	if (int_events & ETH_EVT_TX_ERROR) {
		clear_mask |= BIT_ISR_TER;
	}

	/* Write 1 to clear the interrupt status bits */
	if (clear_mask != 0) {
		ETHx->ETH_ISR_AND_IMR |= clear_mask;
	}
}

/**
 *  @brief To set the start address of Tx/Rx descriptor ring.
 *
 *  @param[in]  ETH_InitStruct The pointer to ETH_InitTypeDef.
 */
void Ethernet_SetDescAddr(ETH_InitTypeDef *ETH_InitStruct)
{

	ETHERNET_TypeDef *ETHx = ((ETHERNET_TypeDef *) RMII_REG_BASE);

	if ((ETH_InitStruct == NULL) || (ETH_InitStruct->ETH_TxDesc == NULL) || (ETH_InitStruct->ETH_RxDesc == NULL)) {
		RTK_LOGE(TAG, "Invalid para\n");
		return;
	}

	/* Both TX and RX descriptors must be cache-line aligned for proper DMA operation */
	if (IS_CACHE_LINE_ALIGNED((u32)ETH_InitStruct->ETH_TxDesc) &&
		IS_CACHE_LINE_ALIGNED((u32)ETH_InitStruct->ETH_RxDesc)) {
		ETHx->ETH_TXFDP1 = (u32)(ETH_InitStruct->ETH_TxDesc);
		ETHx->ETH_RX_FDP1 = (u32)(ETH_InitStruct->ETH_RxDesc);
	} else {
		RTK_LOGE(TAG, "Descriptor misalignment\n");
	}
}

/**
 *  @brief To set the ethernet MAC address.
 *
 *  @param[in]  ETH_MacAddr The MAC address pointer.
 */
void Ethernet_SetMacAddr(u8 *ETH_MacAddr)
{
	ETHERNET_TypeDef *ETHx = ((ETHERNET_TypeDef *) RMII_REG_BASE);

	if (ETH_MacAddr == NULL) {
		RTK_LOGE(TAG, "Invalid para\n");
		return;
	}

	ETHx->ETH_IDR0 = ((ETH_MacAddr[0]) << 24) | ((ETH_MacAddr[1]) << 16) | ((ETH_MacAddr[2]) << 8) |
					 (ETH_MacAddr[3]);
	ETHx->ETH_IDR4 = ((ETH_MacAddr[4]) << 24) | ((ETH_MacAddr[5]) << 16);
}

/**
  * @brief  To set the ethernet refclk mode.
  * @param  refclk_mode 0 ETH_REFCLK_PHY2MAC   1 ETH_REFCLK_MAC2PHY
  */
void Ethernet_SetRefclkDirec(u32 refclk_mode)
{
	ETHERNET_TypeDef *ETHx = ((ETHERNET_TypeDef *) RMII_REG_BASE);

	if (refclk_mode) {
		ETHx->ETH_MSR |= BIT_REFCLK_ON;

	} else {
		ETHx->ETH_MSR &= ~BIT_REFCLK_ON;
	}
}

/**
  * @brief  Use external 50M clock as the RMII reference source clock.
  * @param  pin The pin used to input the external 50MHz clock signal.
  *             It must be a pin that supports the EXT_CLK50M_IN pinmux function.
  */
void Ethernet_UseExtClk(u32 pin)
{
	Pinmux_Config(pin, PINMUX_FUNCTION_EXT_CLK50M_IN);
}

/**
  * @brief  Enable or disable MDIO auto-polling of the PHY status registers.
  * @param  opt This parameter can be one of the following values:
  *         - ENABLE:  Enable auto-polling.
  *         - DISABLE: Disable auto-polling (required before manual MDIO read/write).
  */
void Ethernet_AutoPolling(u32 opt)
{
	ETHERNET_TypeDef *ETHx = ((ETHERNET_TypeDef *) RMII_REG_BASE);
	if (opt == DISABLE) {
		ETHx->ETH_MIIAR |= BIT_DISABLE_AUTO_POLLING;
	} else {
		ETHx->ETH_MIIAR &= ~BIT_DISABLE_AUTO_POLLING;
	}
}

/**
 *  @brief Get current tx pkt buf address
 *
 *  @param[in]  ETH_InitStruct The pointer to ETH_InitTypeDef.
 *
 *  @returns    The tx pkt buf address, or NULL if descriptor not available.
 */
u8 *Ethernet_GetTXPktInfo(ETH_InitTypeDef *ETH_InitStruct)
{
	ETHERNET_TypeDef *ETHx = ((ETHERNET_TypeDef *) RMII_REG_BASE);
	u8 tx_idx;
	u8 *buf = NULL;

	if (ETH_InitStruct == NULL) {
		RTK_LOGE(TAG, "Invalid para: ETH_InitStruct is NULL\n");
		return NULL;
	}

	if (ETH_InitStruct->ETH_TxDesc == NULL) {
		RTK_LOGE(TAG, "Invalid para: ETH_TxDesc is NULL\n");
		return NULL;
	}

	/* Validate descriptor count */
	if (ETH_InitStruct->ETH_TxDescNum == 0) {
		RTK_LOGE(TAG, "Invalid ETH_TxDescNum: 0\n");
		return NULL;
	}

	tx_idx = ETH_InitStruct->ETH_TxDescCurrentNum;

	/* Bounds check for tx_idx */
	if (tx_idx >= ETH_InitStruct->ETH_TxDescNum) {
		RTK_LOGE(TAG, "Tx index %d exceeds count %d\n", tx_idx, ETH_InitStruct->ETH_TxDescNum);
		return NULL;
	}

	/* check if current Tx descriptor is available (OWN bit = 0 means CPU owns it) */
	if ((((u32)(ETH_InitStruct->ETH_TxDesc[tx_idx].dw1)) & FEMAC_TX_DSC_BIT_OWN) == 0) {
		buf = (u8 *)ETH_InitStruct->ETH_TxDesc[tx_idx].addr;
	} else {
		/* Trigger TX interrupt to wake DMA */
		ETHx->ETH_ISR_AND_IMR |= BIT_ISR_TOK_TI;
	}

	return buf;
}

/**
 *  @brief  Update TX descriptor and send packet using Meta Data.
 *
 *  @param[in]  ETH_InitStruct Pointer to Ethernet initialization structure.
 *  @param[in]  meta           Pointer to packet metadata (Length, VLAN, etc.).
 *
 *  @returns    None.
 */
void Ethernet_UpdateTXDESCAndSend(ETH_InitTypeDef *ETH_InitStruct, ETH_PktMetaDef *meta)
{
	ETHERNET_TypeDef *ETHx = ((ETHERNET_TypeDef *)RMII_REG_BASE);
	u8 tx_idx = ETH_InitStruct->ETH_TxDescCurrentNum;
	u32 cmd_sts = 0;
	u32 size;
	u16 swapped_tci;

	/* 1. Validate parameters */
	if (ETH_InitStruct == NULL || meta == NULL) {
		RTK_LOGE(TAG, "Invalid para\n");
		return;
	}

	size = meta->pkt_len;
	if (size == 0 || size > ETH_InitStruct->ETH_TxBufSize) {
		RTK_LOGE(TAG, "Invalid oversize packet: %lu\n", size);
		return;
	}

	/* 2. Clean D-Cache for TX buffer */
	DCache_Clean((u32)ETH_InitStruct->ETH_TxDesc[tx_idx].addr, size);

	/* 3. Prepare Command/Status (DW1) */
	/* Enable HW Checksum (IP/L4) and CRC append. Set as First & Last Segment. */
	cmd_sts = FEMAC_TX_DSC_BIT_IPCS | FEMAC_TX_DSC_BIT_L4CS | FEMAC_TX_DSC_BIT_CRC |
			  FEMAC_TX_DSC_BIT_FS | FEMAC_TX_DSC_BIT_LS | FEMAC_TX_DSC_VAL_SIZE(size);

	/* 4. Prepare Ext Config (DW2) based on Meta Data */
	if (meta->vlan_valid) {
		swapped_tci = ((meta->vlan_tci & 0x00FF) << 8) | ((meta->vlan_tci & 0xFF00) >> 8);
		ETH_InitStruct->ETH_TxDesc[tx_idx].dw2 = FEMAC_TX_W2_VAL_VLAN_ACT(meta->vlan_act) | FEMAC_TX_W2_VAL_VLAN_TAG(swapped_tci);
	} else {
		ETH_InitStruct->ETH_TxDesc[tx_idx].dw2 = 0;
	}

	/* 5. Clear reserved fields (DW3, DW4) */
	ETH_InitStruct->ETH_TxDesc[tx_idx].dw3 = 0;
	ETH_InitStruct->ETH_TxDesc[tx_idx].dw4 = 0;

	/* 6. Handle End of Ring (EOR) bit */
	if (tx_idx == (ETH_InitStruct->ETH_TxDescNum - 1)) {
		cmd_sts |= FEMAC_TX_DSC_BIT_EOR;
	}

	/* Core Protection: Memory Barrier, ensuring that dw2/dw3 has been actually written to physical memory  */
	__asm volatile("dsb" ::: "memory");

	/* 7. Set OWN bit to transfer ownership to DMA */
	ETH_InitStruct->ETH_TxDesc[tx_idx].dw1 = (cmd_sts | FEMAC_TX_DSC_BIT_OWN);

	/* 8. Update TX index */
	tx_idx++;
	if (tx_idx >= ETH_InitStruct->ETH_TxDescNum) {
		tx_idx = 0;
	}
	ETH_InitStruct->ETH_TxDescCurrentNum = tx_idx;

	/* 9. Trigger DMA to poll TX descriptor */
	ETHx->ETH_ISR_AND_IMR |= BIT_ISR_TOK_TI;
	ETHx->ETH_ETHER_IO_CMD |= BIT_TXFN1ST;
}


/**
 * @brief  Get current RX packet buffer address and extract Meta Data.
 *
 * @param[in]  ETH_InitStruct Pointer to Ethernet initialization structure
 *                            containing the RX descriptor ring and current index.
 * @param[out] meta           Pointer to Meta Data to be filled by driver.
 * @return Packet buffer address, or NULL if no complete frame is available.
 */
u8 *Ethernet_GetRXPktInfo(ETH_InitTypeDef *ETH_InitStruct, ETH_PktMetaDef *meta)
{
	ETHERNET_TypeDef *ETHx = ((ETHERNET_TypeDef *) RMII_REG_BASE);
	u32 total_pkt_size = 0;
	u32 desc_len = 0;
	u8 start_idx, scan_idx;
	u8 *buf = NULL;
	volatile u32 dw1_status;
	volatile u32 dw2_status;
	u8 is_fs = 0;

	if (ETH_InitStruct == NULL || ETH_InitStruct->ETH_RxDesc == NULL || meta == NULL) {
		RTK_LOGE(TAG, "Invalid para\n");
		return NULL;
	}

	if (ETH_InitStruct->ETH_RxDescNum == 0) {
		return NULL;
	}

	/* Initialize meta block */
	_memset(meta, 0, sizeof(ETH_PktMetaDef));

	start_idx = ETH_InitStruct->ETH_RxDescCurrentNum;
	scan_idx = start_idx;

	/* Scan loop to find a complete frame (FS -> LS) */
	while (1) {
		dw1_status = ETH_InitStruct->ETH_RxDesc[scan_idx].dw1;
		dw2_status = ETH_InitStruct->ETH_RxDesc[scan_idx].dw2;

		/* Check Ownership */
		if ((dw1_status & FEMAC_RX_DSC_BIT_OWN) != 0) {
			/* Trigger RX interrupt to ensure DMA keeps working */
			ETHx->ETH_ISR_AND_IMR |= BIT_ISR_ROK;
			return NULL;
		}

		/* Check First Segment (FS) */
		if (dw1_status & FEMAC_RX_DSC_BIT_FS) {
			is_fs = 1;
			buf = (u8 *)(ETH_InitStruct->ETH_RxDesc[scan_idx].addr + 2);
		}

		/* Accumulate Length */
		desc_len = FEMAC_RX_DSC_GET_LEN(dw1_status);
		total_pkt_size += desc_len;

		/* Invalidate Cache */
		DCache_Invalidate((u32)(ETH_InitStruct->ETH_RxDesc[scan_idx].addr), desc_len);

		/* Check Last Segment (LS) */
		if (dw1_status & FEMAC_RX_DSC_BIT_LS) {
			/* Fill Metadata from LS descriptor */
			meta->pkt_len = total_pkt_size;

			/* Extract VLAN info */
			if (dw2_status & FEMAC_RX_W2_BIT_CTAVA) {
				u16 raw_tci = (u16)(dw2_status & FEMAC_RX_W2_MASK_CVLAN_TAG);
				meta->vlan_valid = 1;
				meta->vlan_tci = ((raw_tci & 0x00FF) << 8) | ((raw_tci & 0xFF00) >> 8);
			}

			/* Extract Errors (Future extensions can be added here) */
			if (dw1_status & FEMAC_RX_DSC_BIT_CRCERR) {
				meta->rx_crc_err = 1;
			}
			break;
		}

		/* Move to next descriptor */
		scan_idx++;
		if (scan_idx >= ETH_InitStruct->ETH_RxDescNum) {
			scan_idx = 0;
		}

		if (scan_idx == start_idx) {
			RTK_LOGE(TAG, "RX Ring Error: No LS bit found\n");
			return NULL;
		}
	}

	if (is_fs && buf) {
		return buf;
	}

	return NULL;
}

/**
 *  @brief Update rxdesc and return to DMA (Supports Jumbo Frames)
 *
 *  @param[in]  ETH_InitStruct The pointer to ETH_InitTypeDef.
 *
 *  @returns    None.
 */
void Ethernet_UpdateRXDESC(ETH_InitTypeDef *ETH_InitStruct)
{
	ETHERNET_TypeDef *ETHx = ((ETHERNET_TypeDef *) RMII_REG_BASE);
	u8 curr_idx;
	u32 dw1_val;

	if (ETH_InitStruct == NULL || ETH_InitStruct->ETH_RxDesc == NULL) {
		return;
	}

	curr_idx = ETH_InitStruct->ETH_RxDescCurrentNum;

	/*
	 * Release loop:
	 * Release all descriptors used by the current frame (FS...LS) back to DMA.
	 */
	while (1) {
		/* Read current status to check for LS bit later */
		volatile u32 current_status = ETH_InitStruct->ETH_RxDesc[curr_idx].dw1;
		u8 is_last_segment = (current_status & FEMAC_RX_DSC_BIT_LS) ? 1 : 0;
		ETH_InitStruct->ETH_RxDesc[curr_idx].dw2 = 0;
		ETH_InitStruct->ETH_RxDesc[curr_idx].dw3 = 0;

		/* Prepare new Status Word */
		dw1_val = (current_status & FEMAC_RX_DSC_BIT_EOR);
		dw1_val |= (FEMAC_RX_DSC_BIT_OWN | ETH_InitStruct->ETH_RxBufSize);

		/* Core protection: Memory barrier, ensuring that the OWN bit is assigned only after the clear operation takes effect. */
		__asm volatile("dsb" ::: "memory");

		/* Transfer ownership back to DMA */
		ETH_InitStruct->ETH_RxDesc[curr_idx].dw1 = dw1_val;

		curr_idx++;
		if (curr_idx >= ETH_InitStruct->ETH_RxDescNum) {
			curr_idx = 0;
		}

		if (is_last_segment) {
			break;
		}

		if (curr_idx == ETH_InitStruct->ETH_RxDescCurrentNum) {
			break;
		}
	}

	/* Update global index to point to the next fresh descriptor */
	ETH_InitStruct->ETH_RxDescCurrentNum = curr_idx;

	/* Trigger RX interrupt to wake DMA */
	ETHx->ETH_ISR_AND_IMR |= BIT_ISR_ROK;
}

/**
 *  @brief To get the link status.
 *
 *  @returns    1 for link up, 0 for link down.
 */
u32 Ethernet_GetLinkStatus(void)
{
	ETHERNET_TypeDef *ETHx = ((ETHERNET_TypeDef *) RMII_REG_BASE);

	return !GET_LINKB(ETHx->ETH_MSR);
}

/**
 * @brief Get speed and duplex of ethernet
 *
 */
void Ethernet_GetSpeedDuplex(void)
{
	ETHERNET_TypeDef *ETHx = ((ETHERNET_TypeDef *) RMII_REG_BASE);
	/* Get and Log MAC Link Info (Only if link is up) */
	u32 speed = GET_SPEED(ETHx->ETH_MSR);
	u32 duplex = GET_FULLDUPREG(ETHx->ETH_MSR);

	RTK_LOGI(TAG, "MAC Link Info: %s, %s\n",
			 (speed == ETH_SPEED_100M) ? "100 Mb/s" :
			 (speed == ETH_SPEED_10M)  ? "10 Mb/s"  : "Unknown",
			 (duplex == ETH_FULL_DUPLEX) ? "Full Duplex" : "Half Duplex");
}

/**
 * @brief Waits for the PHY MDIO bus to become idle.
 *        This function polls the MDIO busy flag until it is cleared or a timeout occurs.
 * @param ETHx         Pointer to the Ethernet MAC register structure.
 * @param timeout_us   Timeout threshold in microseconds.
 * @return 0 on success (MDIO idle), -1 on timeout.
 */
static inline int Ethernet_WaitPhyIdle(ETHERNET_TypeDef *ETHx, uint32_t timeout_us)
{
	uint32_t start = DTimestamp_Get();

	/* Poll until BIT_MDIO_BUSY is cleared */
	while ((DTimestamp_Get() - start) <= timeout_us) {
		if (!(ETHx->ETH_MIIAR & BIT_MDIO_BUSY)) {
			return RTK_SUCCESS;
		}
	}
	return -RTK_ERR_TIMEOUT;
}

/**
 * @brief      Read data from MDIO bus (Clause 22).
 *
 * Replaces the legacy Ethernet_ReadPhyReg function.
 *
 * @param[in]  phy_addr  PHY address (0-31).
 * @param[in]  reg_addr  Register address (0-31).
 * @param[out] data      Pointer to store the read value.
 *
 * @return     RTK_SUCCESS on success, negative error code on failure.
 */
int Ethernet_ReadPhyReg(uint8_t phy_addr, uint8_t reg_addr, uint16_t *data)
{
	/* 1. Get the register base from the private data */
	ETHERNET_TypeDef *ETHx = ((ETHERNET_TypeDef *) RMII_REG_BASE);

	/* Parameter check */
	if (reg_addr > 31) {
		return RTK_FAIL;
	}

	/* 2. Wait for bus idle (Legacy logic: check busy before operation) */
	if (Ethernet_WaitPhyIdle(ETHx, MDIO_WAIT_TIME) != RTK_SUCCESS) {
		return -RTK_ERR_BUSY;
	}

	/* 3. Initiate Read: Write PHY address and Reg address to MIIAR */
	/* Note: For Read, BIT_FLAG is 0. */
	ETHx->ETH_MIIAR = (PHYADDRESS(phy_addr) | REGADDR4_0(reg_addr));

	/* 4. Wait for data to be ready (Busy bit clears when read is done) */
	if (Ethernet_WaitPhyIdle(ETHx, MDIO_WAIT_TIME) != RTK_SUCCESS) {
		return -RTK_ERR_BUSY;
	}

	/* 5. Extract the 16-bit data */
	*data = (uint16_t)GET_DATA15_0(ETHx->ETH_MIIAR);

	return RTK_SUCCESS;
}

/**
 * @brief      Write data to MDIO bus (Clause 22).
 *
 * Replaces the legacy Ethernet_WritePhyReg function.
 *
 * @param[in]  phy_addr  PHY address (0-31).
 * @param[in]  reg_addr  Register address (0-31).
 * @param[in]  data      Data to write.
 *
 * @return     RTK_SUCCESS on success, negative error code on failure.
 */
int Ethernet_WritePhyReg(uint8_t phy_addr, uint8_t reg_addr, uint16_t data)
{
	ETHERNET_TypeDef *ETHx = ((ETHERNET_TypeDef *) RMII_REG_BASE);

	if (reg_addr > 31) {
		return RTK_FAIL;
	}

	/* 1. Wait for bus idle */
	if (Ethernet_WaitPhyIdle(ETHx, MDIO_WAIT_TIME) != RTK_SUCCESS) {
		return -RTK_ERR_BUSY;
	}

	/* 2. Initiate Write: Set Flag, Disable Auto Poll, Addr, and Data */
	ETHx->ETH_MIIAR = (BIT_FLAG | BIT_DISABLE_AUTO_POLLING |
					   PHYADDRESS(phy_addr) | REGADDR4_0(reg_addr) | data);

	/* Note: The write is triggered immediately. Some drivers wait for idle here too. */
	return RTK_SUCCESS;
}

/**
  * @brief  Initialize ETH_InitTypeDef.
  * @param  ETH_InitStruct The pointer to ETH_InitTypeDef.
  * @param  PHY_Dev The pointer to eth_phy_dev.
  */
void Ethernet_StructInit(ETH_InitTypeDef *ETH_InitStruct, struct eth_phy_dev *PHY_Dev)
{
	if (!ETH_InitStruct || !PHY_Dev) {
		return;
	}

	_memset(ETH_InitStruct, 0, sizeof(ETH_InitTypeDef));

	/* 1. Basic Links */
	ETH_InitStruct->phy_dev = PHY_Dev;

	/* 2. MAC Parameters */
	ETH_InitStruct->MacConfig.Bits.MacMode         = ETH_MAC_MODE;
	ETH_InitStruct->MacConfig.Bits.Loopback        = ETH_LPB_NONE;          /* Normal mode */
	ETH_InitStruct->MacConfig.Bits.IFGTime         = ETH_IFG_3;             /* 96-bit time for 100Mbps */
	ETH_InitStruct->MacConfig.Bits.RefClkDir       = ETH_REFCLK_MAC2PHY;    /* Output Ref Clk to PHY */
	ETH_InitStruct->MacConfig.Bits.RefClkPhase     = ETH_SAMPLED_ON_RISING_EDGE;
	ETH_InitStruct->MacConfig.Bits.RxJumbo         = ETH_RX_JUMBO_DISABLE;
	/* Default to Promiscuous for debug. Change to UNICAST for production. */
	ETH_InitStruct->MacConfig.Bits.PktFilterConfig = ETH_FILTER_PROMISCUOUS;
	ETH_InitStruct->MacConfig.Bits.EEEEnable       = DISABLE;               /* Disable EEE */
	ETH_InitStruct->MacConfig.Bits.AutoNego        = ETH_NWAY_ENABLE;       /* Enable Auto-Negotiation */

	/* EEE parameters */
	if (ETH_InitStruct->MacConfig.Bits.EEEEnable) {
		Ethernet_EEE_StructInit(ETH_InitStruct->EEE_Config);
	}

	/* VLAN parameters */
	ETH_InitStruct->VlanConfig.Bits.TxTagType = ETH_VLAN_TYPE_CTAG;
	ETH_InitStruct->VlanConfig.Bits.RxStrip = ETH_VLAN_STRIP_ENABLE;
	ETH_InitStruct->VlanConfig.Bits.STagPID = 0x88A8;  /* standard S-Tag PID */

	/* Buffer / DMA Defaults */
	ETH_InitStruct->DMA_TxThreshold = ETH_RX_THRESHOLD_256B;
	ETH_InitStruct->DMA_TxThreshold = ETH_TX_THRESHOLD_256B;
	ETH_InitStruct->DMA_RxTriggerLevel = ETH_TRIGGER_LEVEL_1_PKT;
	ETH_InitStruct->DMA_TxTriggerLevel = ETH_TRIGGER_LEVEL_1_PKT;

	ETH_InitStruct->ETH_RxDescNum = 10;
	ETH_InitStruct->ETH_TxDescNum = 8;

	/* Default Buffer Sizes */
	ETH_InitStruct->ETH_TxBufSize = ETH_PKT_MAX_SIZE;
	ETH_InitStruct->ETH_RxBufSize = ETH_PKT_MAX_SIZE;

	/* Interrupts */
	ETH_InitStruct->ETH_IntMaskAndStatus = BIT_IMR_LINKCHG | BIT_IMR_TOK_TI |
										   BIT_IMR_RER_OVF | BIT_IMR_ROK | 0xFFFF;
}

/**
  * @brief  Initialize Ethernet MAC, PHY, and DMA descriptors.
  * @param  ETH_InitStruct Pointer to ETH_InitTypeDef containing the full Ethernet configuration.
  * @return RTK_SUCCESS on success, or a negative error code:
  *         - -RTK_ERR_BADARG if ETH_InitStruct, phy_dev, or phy_dev->ops is NULL.
  *         - -RTK_ERR_TIMEOUT if MAC reset or auto-negotiation timed out.
  */
int Ethernet_Init(ETH_InitTypeDef *ETH_InitStruct)
{
	ETHERNET_TypeDef *ETHx = ((ETHERNET_TypeDef *) RMII_REG_BASE);
	u32 temp = 0;
	u8 i, filter_mode;
	u32 timeout = 1000;
	int32_t ret = RTK_SUCCESS;
	struct eth_phy_dev *phy = NULL;

	if (!ETH_InitStruct || !ETH_InitStruct->phy_dev || !ETH_InitStruct->phy_dev->ops) {
		RTK_LOGE(TAG, "Init: Invalid Param or PHY Dev\r\n");
		ret = -RTK_ERR_BADARG;
		goto exit;
	}

	phy = ETH_InitStruct->phy_dev;

	Ethernet_AutoPolling(DISABLE);
	DelayMs(50);

	/* reset MAC */
	ETHx->ETH_CR |= BIT_RST;

	while ((ETHx->ETH_CR & BIT_RST) && timeout--) {
		DelayUs(10);
	}
	if (timeout == 0) {
		ret = -RTK_ERR_TIMEOUT;
		RTK_LOGE(TAG, "MAC Reset Timeout\n");
		goto exit;
	}

	/* reset phy */
	if (phy->ops->init) {
		ret = phy->ops->init(phy);
		if (ret != RTK_SUCCESS) {
			RTK_LOGE(TAG, "PHY Init failed: %d\n", ret);
			goto exit;
		}
	}
	if (phy->ops->reset) {
		ret = phy->ops->reset(phy);
		if (ret != RTK_SUCCESS) {
			RTK_LOGE(TAG, "PHY Reset failed: %d\n", ret);
			goto exit;
		}
	}
	/* Configure ref clock direction*/
	if (phy->ops->cfg_refclock) {
		ret = phy->ops->cfg_refclock(phy, ETH_InitStruct->MacConfig.Bits.RefClkDir);
		if (ret != RTK_SUCCESS) {
			RTK_LOGE(TAG, "PHY RefClock config failed: %d\n", ret);
			goto exit;
		}
	}

	if (ETH_InitStruct->MacConfig.Bits.EEEEnable) {
		if (phy->ops->cfg_eee) {
			ret = phy->ops->cfg_eee(phy, ENABLE);
			if (ret != RTK_SUCCESS) {
				RTK_LOGE(TAG, "PHY EEE config failed: %d\n", ret);
				goto exit;
			}
		}
	}

	/* Auto-Negotiation/Nway Settings */
	if (ETH_InitStruct->MacConfig.Bits.AutoNego) {
		phy->link_cfg.autoneg_en = TRUE;
		phy->link_cfg.advertise_100m = TRUE;
	} else {
		phy->link_cfg.autoneg_en = FALSE;
		/* speed */
		if (ETH_InitStruct->MacConfig.Bits.Speed == ETH_SPEED_100M) {
			phy->link_cfg.speed = PHY_SPEED_100M;
			phy->link_cfg.advertise_100m = TRUE;
		} else {
			phy->link_cfg.speed = PHY_SPEED_10M;
			phy->link_cfg.advertise_10m = TRUE;
		}
		ETHx->ETH_MSR &= ~MASK_FORCE_SPD;
		ETHx->ETH_MSR |= BIT_FORCE_SPD_MODE | FORCE_SPD(ETH_InitStruct->MacConfig.Bits.Speed);
		/* Duplex */
		if (ETH_InitStruct->MacConfig.Bits.Duplex == ETH_FULL_DUPLEX) {
			phy->link_cfg.duplex = PHY_DUPLEX_FULL;
		} else {
			phy->link_cfg.duplex = PHY_DUPLEX_HALF;
		}
		ETHx->ETH_MSR &= ~BIT_FORCEDFULLDUP;
		ETHx->ETH_MSR |= FORCEDFULLDUP(ETH_InitStruct->MacConfig.Bits.Duplex);

	}
	if (phy->ops->cfg_link) {
		ret = phy->ops->cfg_link(phy, &phy->link_cfg);
		if (ret != RTK_SUCCESS) {
			RTK_LOGE(TAG, "PHY Link config failed: %d\n", ret);
			goto exit;
		}
	}

	if (ETH_InitStruct->MacConfig.Bits.AutoNego) {
		if (phy->ops->autoneg_restart) {
			ret = phy->ops->autoneg_restart(phy);
			if (ret != RTK_SUCCESS) {
				RTK_LOGE(TAG, "PHY AutoNeg restart failed: %d\n", ret);
				/* Don't goto exit - continue with partial configuration */
			}
		}
	}
	/* Tx settings */
	ETHx->ETH_MSR |= BIT_REG_RMII2MII_EN;
	Ethernet_SetRefclkDirec(ETH_InitStruct->MacConfig.Bits.RefClkDir);

	ETHx->ETH_MSR |= BIT_TXFCE;
	ETHx->ETH_MSR |= BIT_RXFCE; /* Rx flow control bit can be set only in full duplex mode */
	ETHx->ETH_MSR |= REFCLK_PHASE(ETH_InitStruct->MacConfig.Bits.RefClkPhase);

	ETHx->ETH_TCR &= ~MASK_IFG2_0;
	ETHx->ETH_TCR |= IFG2_0(ETH_InitStruct->MacConfig.Bits.IFGTime);

	ETHx->ETH_TCR &= ~MASK_LBK;
	ETHx->ETH_TCR |= LBK(ETH_InitStruct->MacConfig.Bits.Loopback);

	/* Rx settings */
	Ethernet_SetMacAddr(ETH_InitStruct->ETH_MacAddr);

	/* VLAN Configuration */
	/* Configure Rx VLAN Stripping in ETH_CR */
	if (ETH_InitStruct->VlanConfig.Bits.RxStrip) {
		ETHx->ETH_CR |= BIT_RXVLAN;  /* Hardware strips the tag from RX packets */
	} else {
		ETHx->ETH_CR &= ~BIT_RXVLAN;
	}

	/* Configure Global VLAN Register (S-Tag PID & Tag Type) */
	temp = ETHx->ETH_VLAN_REG;

	/* Clear PID and Type bits first */
	temp &= ~(MASK_STAG_PID | BIT_TDSC_VLAN_TYPE);

	/* Set S-Tag Protocol Identifier (default usually 0x88A8 or 0x9100, user defined) */
	temp |= STAG_PID(ETH_InitStruct->VlanConfig.Bits.STagPID);

	/* Set TX Descriptor VLAN Type */
	if (ETH_InitStruct->VlanConfig.Bits.TxTagType) {
		/* 1: TX command acts on S-Tag
		 * 0: TX command acts on C-Tag (default) */
		temp |= BIT_TDSC_VLAN_TYPE;
	}

	ETHx->ETH_VLAN_REG = temp;
	/* Enable MAC Rx VLAN de-tagging */
	if (ETH_InitStruct->VlanConfig.Bits.RxStrip) {
		ETHx->ETH_CR |= BIT_RXVLAN;
	}
	/* EEE configuration */
	if (ETH_InitStruct->MacConfig.Bits.EEEEnable) {
		Ethernet_EEE_Init(ETHx, ETH_InitStruct->EEE_Config);
		phy->ops->cfg_eee(phy, ENABLE);
	}
	/* Packet filer*/
	filter_mode = ETH_InitStruct->MacConfig.Bits.PktFilterConfig;
	/* 3. Set bits according to the selected mode */
	switch (filter_mode) {
	case ETH_FILTER_WITH_MULTICAST:
		/* Accept multicast, broadcast and physical match packets */
		ETHx->ETH_RCR = BIT_APM | BIT_AB | BIT_AM;
		break;

	case ETH_FILTER_STRICT_UNICAST:
		/* Accept only physical match packets. Ignore Broadcast. */
		ETHx->ETH_RCR = BIT_APM;
		break;

	case ETH_FILTER_PROMISCUOUS:
		/*
		 * Promiscuous Mode:
		 * BIT_AAP: Accept all packets with destination MAC addr.
		 * BIT_AB:  Explicitly enable Broadcast (Hardware might not imply AB when AAP is set).
		 * BIT_AM:  Explicitly enable Multicast.
		 * Use case: Network sniffing (Wireshark) or software bridging.
		 */
		ETHx->ETH_RCR = BIT_AAP | BIT_AB | BIT_AM;
		break;

	case ETH_FILTER_DIAGNOSTIC_ALL:
		/*
		 * Diagnostic Mode:
		 * Accept absolutely everything including Runt (< 64 bytes) and Error frames.
		 * Combine AAP/AB/AM to ensure no valid frames are filtered out,
		 * while AR/AER captures the corrupted ones.
		 */
		ETHx->ETH_RCR = BIT_AAP | BIT_AB | BIT_AM | BIT_AR | BIT_AER;
		break;

	default:
		/* Default Standard Mode (Unicast + Broadcast + Multicast).
		 * Excluding Multicast (BIT_AM) here might break IPv6 or Service Discovery.
		 */
		ETHx->ETH_RCR = BIT_APM | BIT_AB | BIT_AM;
		break;
	}

	if (ETH_InitStruct->MacConfig.Bits.RxJumbo == ETH_RX_JUMBO_ENABLE) {
		ETHx->ETH_CR |= BIT_RXJUMBO;
	}
	/* DMA descriptor control*/
	ETHx->ETH_ETHRNTRXCPU_DES_NUM1 = RX_PSE_DES_THRES_OFF_1_7_0(0x01) | \
									 RX_PSE_DES_THRES_ON_1_11_8((ETH_InitStruct->ETH_RxDescNum - 2) >> 8) | RX_PSE_DES_THRES_ON_1_7_0(ETH_InitStruct->ETH_RxDescNum - 2) | \
									 ETHRNTRXCPU_DES_NUM_1_11_8((ETH_InitStruct->ETH_RxDescNum - 1) >> 8) | ETHRNTRXCPU_DES_NUM_1_7_0(ETH_InitStruct->ETH_RxDescNum - 1);
	ETHx->ETH_RX_RINGSIZE1 |= RXRINGSIZE_1_LOW(ETH_InitStruct->ETH_RxDescNum - 1);
	/* I/O command: short desc. format = 1, Tx & Rx FIFO threshold = 256 bytes */
	ETHx->ETH_IO_CMD1 = DSC_FORMAT_EXTRA(0x3) | BIT_EN_4GB;
	ETHx->ETH_ETHER_IO_CMD = RXFTH(ETH_InitStruct->DMA_TxThreshold) | TSH(ETH_InitStruct->DMA_TxThreshold) |
							 BIT_SHORTDESFORMAT;

	ETHx->ETH_TXFDP1 = (u32)ETH_InitStruct->ETH_TxDesc;
	ETHx->ETH_RX_FDP1 = (u32)ETH_InitStruct->ETH_RxDesc;

	/* Initialize Tx descriptors */
	for (i = 0; i < (ETH_InitStruct->ETH_TxDescNum); i++) {
		ETH_InitStruct->ETH_TxDesc[i].dw1 = 0;
		ETH_InitStruct->ETH_TxDesc[i].addr = (u32)(ETH_InitStruct->ETH_TxPktBuf + (i * ETH_InitStruct->ETH_TxBufSize));
		ETH_InitStruct->ETH_TxDesc[i].dw2 = 0;//(eth_vlan_hdr_remove << 25) | (ETH_C_VLAN_HDR & 0xFFFF);
		ETH_InitStruct->ETH_TxDesc[i].dw3 = 0;
		ETH_InitStruct->ETH_TxDesc[i].dw4 = 0;
	}

	/* Initialize Rx descriptors */
	for (i = 0; i < (ETH_InitStruct->ETH_RxDescNum); i++) {
		if (i == ((ETH_InitStruct->ETH_RxDescNum) - 1)) {
			ETH_InitStruct->ETH_RxDesc[i].dw1 = FEMAC_RX_DSC_BIT_OWN | FEMAC_RX_DSC_BIT_EOR | ETH_InitStruct->ETH_RxBufSize;
		} else {
			ETH_InitStruct->ETH_RxDesc[i].dw1 = FEMAC_RX_DSC_BIT_OWN | ETH_InitStruct->ETH_RxBufSize;
		}
		ETH_InitStruct->ETH_RxDesc[i].addr = (u32)(ETH_InitStruct->ETH_RxPktBuf + (i * ETH_InitStruct->ETH_RxBufSize));
		ETH_InitStruct->ETH_RxDesc[i].dw2 = 0;
		ETH_InitStruct->ETH_RxDesc[i].dw3 = 0;
	}

	/* enable Tx & Rx */
	ETHx->ETH_ETHER_IO_CMD |= BIT_TE | BIT_RE;

	/* isr & imr */
	ETHx->ETH_ISR_AND_IMR =  ETH_InitStruct->ETH_IntMaskAndStatus;

	/* enable auto-polling */
	Ethernet_AutoPolling(ENABLE);

	/* enable Rx ring1 */
	ETHx->ETH_IO_CMD1 |= BIT_RXRING1;

	if (ETH_InitStruct->MacConfig.Bits.AutoNego) {
		timeout = 50000;
		while (timeout--) {
			if (GET_NWCOMPLETE((ETHx->ETH_MSR) & BIT_NWCOMPLETE)) {
				break;
			}
		}
		if (timeout == 0) {
			ret = -RTK_ERR_TIMEOUT;
		}
	}
exit:
	return ret;
}
/** @} */

/** @} */

/** @} */
