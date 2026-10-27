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
 * @brief  Fill the EEE_Config structure with default IEEE 802.3az values.
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
  * @brief  Initialize the EEE (Energy Efficient Ethernet) functionality according to the 6-step procedure.
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
  * @brief  Read the pending interrupt status without clearing it.
  * @return ETH_ISR contents, a combination of BIT_ISR_* bits.
  */
u16 Ethernet_GetPendingINT(void)
{
	ETHERNET_TypeDef *ETHx = ((ETHERNET_TypeDef *) RMII_REG_BASE);

	return ETHx->ETH_ISR;
}

/**
  * @brief  Clear interrupt status bits.
  * @param  status BIT_ISR_* bits to clear.
  * @note   ETH_ISR is write-1-to-clear, so this is a plain write: the bits not in status, and any that
  *         arrived after they were read, are left pending. An ISR must clear every bit it read from
  *         Ethernet_GetPendingINT(), not only the ones it acts on: TDU and RDU1 latch until cleared and
  *         stall the DMA meanwhile.
  */
void Ethernet_ClearINT(u16 status)
{
	ETHERNET_TypeDef *ETHx = ((ETHERNET_TypeDef *) RMII_REG_BASE);

	ETHx->ETH_ISR = status;
}

/**
 *  @brief  Set the start address of the TX/RX descriptor ring.
 *
 *  @param  ETH_InitStruct The pointer to @ref ETH_InitTypeDef.
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
 *  @brief  Set the Ethernet MAC address.
 *
 *  @param  ETH_MacAddr The MAC address pointer.
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
  * @brief  Set the Ethernet RMII reference clock direction.
  * @param  refclk_mode Reference clock direction. See @ref eth_refclk_dir.
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
  * @brief  Select the MAC clock source and enable the MAC clock.
  * @param  src     ETH_MAC_CLK_INT50M: divide SYS_PLL or USB_PLL down to 50MHz.
  *                 ETH_MAC_CLK_EXT50M: take 50MHz from ext_pin.
  * @param  ext_pin Pin that receives the external clock, only used for ETH_MAC_CLK_EXT50M.
  * @return RTK_SUCCESS, or RTK_FAIL if the PLLs cannot give 50MHz.
  */
int Ethernet_MacClkConfig(enum eth_mac_clk_src src, u32 ext_pin)
{
	const SocClk_Info_TypeDef *pSocClk_Info = &SocClk_Info[Boot_SocClk_Info_Idx];
	u8 gmac_ckd;
	u32 gmac_clk;

	if (src == ETH_MAC_CLK_EXT50M) {
		/* The clock has to reach the pin before the mux switches to it, or the MAC loses its clock. */
		Ethernet_UseExtClk(ext_pin);
		RCC_PeriphClockSourceSet(GMAC, EXT50M);
	} else {
		/* Select a clock source that can be divided up to 50MHz */
		gmac_ckd = PLL_ClkSrcGet(pSocClk_Info->SYSPLL_CLK, pSocClk_Info->USBPLL_CLK, CLK_LIMIT_GMAC);
		/* The divider is 0 when the chosen PLL runs at 0Hz, and dividing by it below would fault */
		if (GET_CLK_DIV(gmac_ckd) == 0) {
			RTK_LOGE(TAG, "No PLL to derive the GMAC clock from\n");
			return RTK_FAIL;
		}
		if (gmac_ckd & IS_SYS_PLL) {
			gmac_clk = pSocClk_Info->SYSPLL_CLK / GET_CLK_DIV(gmac_ckd);
		} else {
			gmac_clk = pSocClk_Info->USBPLL_CLK / GET_CLK_DIV(gmac_ckd);
		}

		if (gmac_clk != CLK_LIMIT_GMAC) {
			RTK_LOGE(TAG, "GMAC clock %u Hz, RMII needs 50MHz\n", gmac_clk);
			return RTK_FAIL;
		}

		if (gmac_ckd & IS_SYS_PLL) {
			RCC_PeriphClockSourceSet(GMAC, SYS_PLL);
			RCC_PeriphClockDividerFENSet(SYS_PLL_GMAC, ENABLE);
			RCC_PeriphClockDividerSet(SYS_PLL_GMAC, GET_CLK_DIV(gmac_ckd));
		} else {
			RCC_PeriphClockSourceSet(GMAC, USB_PLL);
			RCC_PeriphClockDividerFENSet(USB_PLL_GMAC, ENABLE);
			RCC_PeriphClockDividerSet(USB_PLL_GMAC, GET_CLK_DIV(gmac_ckd));
		}
	}
	RCC_PeriphClockCmd(APBPeriph_GMAC, APBPeriph_GMAC_CLOCK, ENABLE);

	return RTK_SUCCESS;
}

/**
  * @brief  Output a 50MHz or 25MHz clock on a pin, to feed the PHY.
  * @param  pin Pin that supports the EXT_CLK_OUT pinmux function.
  * @param  clk ETH_PHY_CLK_50M or ETH_PHY_CLK_25M.
  */
void Ethernet_PhyClkConfig(u32 pin, enum eth_phy_clk clk)
{
	u32 temp;

	Pinmux_Config(pin, PINMUX_FUNCTION_EXT_CLK_OUT);

	temp = HAL_READ32(PINMUX_REG_BASE, REG_PINMUX_SUB_CTRL);
	temp |= PAD_BIT_DBG_CLK_FORCE;
	temp &= ~PAD_MASK_DBG_CLK0_SEL;
	temp |= PAD_DBG_CLK0_SEL(clk);
	HAL_WRITE32(PINMUX_REG_BASE, REG_PINMUX_SUB_CTRL, temp);
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
 *  @brief  Get the TX packet buffer address of the current TX descriptor.
 *
 *  @param  ETH_InitStruct The pointer to @ref ETH_InitTypeDef.
 *
 *  @return The word-aligned start of the TX packet buffer, or NULL if no descriptor is available.
 *          TX DMA reads from any byte address, so the caller may start the frame 1~3 bytes into
 *          the buffer by writing that address back to the descriptor.
 *  @note   Per-packet path: ETH_InitStruct and the ring are validated once by Ethernet_Init().
 */
u8 *Ethernet_GetTXPktInfo(ETH_InitTypeDef *ETH_InitStruct)
{
	u16 tx_idx = ETH_InitStruct->ETH_TxDescCurrentNum;
	u8 *buf = NULL;

	/* check if current Tx descriptor is available (OWN bit = 0 means CPU owns it) */
	if ((((u32)(ETH_InitStruct->ETH_TxDesc[tx_idx].dw1)) & FEMAC_TX_DSC_BIT_OWN) == 0) {
		/* The caller may have moved addr off the buffer start for the previous frame. */
		buf = (u8 *)(ETH_InitStruct->ETH_TxDesc[tx_idx].addr & ~3U);
	}

	return buf;
}

/**
 *  @brief  Update the TX descriptor with metadata and trigger packet transmission.
 *
 *  @param  ETH_InitStruct Pointer to Ethernet initialization structure.
 *  @param  meta           Pointer to packet metadata (Length, VLAN, etc.).
 *  @note   Per-packet path: ETH_InitStruct and the ring are validated once by Ethernet_Init().
 */
void Ethernet_UpdateTXDESCAndSend(ETH_InitTypeDef *ETH_InitStruct, ETH_PktMetaDef *meta)
{
	ETHERNET_TypeDef *ETHx = ((ETHERNET_TypeDef *)RMII_REG_BASE);
	u16 tx_idx = ETH_InitStruct->ETH_TxDescCurrentNum;
	u32 cmd_sts = 0;
	u32 size;
	u16 swapped_tci;

	/* 1. Validate the caller-supplied metadata */
	if (meta == NULL) {
		return;
	}

	size = meta->pkt_len;
	if (size == 0 || size > ETH_InitStruct->ETH_TxBufSize) {
		RTK_LOGE(TAG, "Invalid oversize packet: %u\n", size);
		return;
	}

	/* 2. Clean D-Cache for TX buffer */
	DCache_Clean((u32)ETH_InitStruct->ETH_TxDesc[tx_idx].addr, size);

	/* 3. Prepare Command/Status (DW1) */
	/* Request CRC append. Set as First & Last Segment. */
	cmd_sts = FEMAC_TX_DSC_BIT_CRC |
			  FEMAC_TX_DSC_BIT_FS | FEMAC_TX_DSC_BIT_LS | FEMAC_TX_DSC_VAL_SIZE(size);
#if ETH_CSUM_OFFLOAD
	/* Let the MAC fill in the IP/L4 checksums; the stack skips them for this netif. */
	cmd_sts |= FEMAC_TX_DSC_BIT_IPCS | FEMAC_TX_DSC_BIT_L4CS;
#endif

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

	/* 7. Set OWN bit to transfer ownership to DMA */
	ETH_InitStruct->ETH_TxDesc[tx_idx].dw1 = (cmd_sts | FEMAC_TX_DSC_BIT_OWN);

	/* The descriptor is Normal memory and the doorbell Device memory, which the core does not order
	 * against each other: without this DMA could be woken before it can see OWN. */
	__asm volatile("dsb" ::: "memory");

	/* 8. Update TX index */
	tx_idx++;
	if (tx_idx >= ETH_InitStruct->ETH_TxDescNum) {
		tx_idx = 0;
	}
	ETH_InitStruct->ETH_TxDescCurrentNum = tx_idx;

	/* 9. Trigger DMA to poll TX descriptor */
	ETHx->ETH_ETHER_IO_CMD |= BIT_TXFN1ST;
}


/**
 * @brief  Hand the next n RX descriptors back to DMA and advance the ring index past them.
 * @param  ETH_InitStruct Pointer to Ethernet initialization structure.
 * @param  n              Number of descriptors, starting at ETH_RxDescCurrentNum. After a frame
 *                        from Ethernet_GetRXPktInfo(), pass ETH_RxSegmentCount.
 * @note   Per-packet path: ETH_InitStruct and the ring are validated once by Ethernet_Init().
 */
void Ethernet_ReleaseRXDesc(ETH_InitTypeDef *ETH_InitStruct, u32 n)
{
	ETHERNET_TypeDef *ETHx = ((ETHERNET_TypeDef *) RMII_REG_BASE);
	u32 curr_idx = ETH_InitStruct->ETH_RxDescCurrentNum;
	u32 last_idx = ETH_InitStruct->ETH_RxDescNum - 1;
	u32 dw1_val;

	/* EOR is a property of the slot, not of the frame, so it comes from the index instead of a read of dw1. */
	while (n--) {
		ETH_InitStruct->ETH_RxDesc[curr_idx].dw2 = 0;
		ETH_InitStruct->ETH_RxDesc[curr_idx].dw3 = 0;

		dw1_val = FEMAC_RX_DSC_BIT_OWN | ETH_InitStruct->ETH_RxBufSize;
		if (curr_idx == last_idx) {
			dw1_val |= FEMAC_RX_DSC_BIT_EOR;
		}

		/* Transfer ownership back to DMA */
		ETH_InitStruct->ETH_RxDesc[curr_idx].dw1 = dw1_val;

		curr_idx = (curr_idx == last_idx) ? 0 : curr_idx + 1;
	}

	/* Update global index to point to the next fresh descriptor */
	ETH_InitStruct->ETH_RxDescCurrentNum = curr_idx;

	/* The OWN writes must reach memory before RDU1 is released, or DMA could poll a descriptor
	 * it does not own yet and latch RDU1 again. */
	__asm volatile("dsb" ::: "memory");

	/* DES_NUM tells the MAC the last descriptor the CPU has handed back, so its PAUSE check follows the free count. */
	curr_idx = (curr_idx == 0) ? last_idx : curr_idx - 1;
	ETHx->ETH_ETHRNTRXCPU_DES_NUM1 = (ETHx->ETH_ETHRNTRXCPU_DES_NUM1 & ~(MASK_ETHRNTRXCPU_DES_NUM_1_7_0 | MASK_ETHRNTRXCPU_DES_NUM_1_11_8)) |
									 ETHRNTRXCPU_DES_NUM_1_11_8(curr_idx >> 8) | ETHRNTRXCPU_DES_NUM_1_7_0(curr_idx);

	/* RDU1 latches when the ring runs dry and holds off RX DMA until cleared, so release it now
	 * that descriptors are back. Write that bit alone: ETH_ISR is write-1-to-clear, and reading it
	 * back first would also clear pending ROK/LINKCHG before the ISR sees them. */
	ETHx->ETH_ISR = BIT_ISR_RDU1;
}

/**
 * @brief  Get the current RX packet buffer address and extract packet metadata.
 *
 * @param  ETH_InitStruct Pointer to Ethernet initialization structure
 *                            containing the RX descriptor ring and current index.
 * @param meta           Pointer to metadata structure to be populated by the driver.
 * @return Start of the first RX buffer, or NULL if no complete frame is available. The MAC puts
 *         2 bytes ahead of the frame, so the ethernet header starts at return + 2.
 * @note   Per-packet path: ETH_InitStruct and the ring are validated once by Ethernet_Init().
 */
u8 *Ethernet_GetRXPktInfo(ETH_InitTypeDef *ETH_InitStruct, ETH_PktMetaDef *meta)
{
	u32 total_pkt_size = 0;
	u32 desc_len = 0;
	u32 seg_cnt = 0;
	u16 start_idx, scan_idx;
	u8 *buf = NULL;
	/* Plain locals: ETH_RxDesc is a volatile pointer, so each descriptor word is read from
	 * memory exactly once here and then used from a register. */
	u32 dw1_status;
	u32 dw2_status;

	if (meta == NULL) {
		return NULL;
	}

	start_idx = ETH_InitStruct->ETH_RxDescCurrentNum;
	scan_idx = start_idx;

	/* Every drain of the ring ends on an empty slot, so skip clearing meta when there is no frame. */
	if (ETH_InitStruct->ETH_RxDesc[start_idx].dw1 & FEMAC_RX_DSC_BIT_OWN) {
		return NULL;
	}

	/* Initialize meta block */
	_memset(meta, 0, sizeof(ETH_PktMetaDef));

	/* Scan loop to find a complete frame (FS -> LS) */
	while (1) {
		dw1_status = ETH_InitStruct->ETH_RxDesc[scan_idx].dw1;

		/* Check Ownership */
		if ((dw1_status & FEMAC_RX_DSC_BIT_OWN) != 0) {
			return NULL;
		}

		/* A frame must start on an FS, and a second FS before its LS means the first frame was cut
		 * short. Either way the segments counted so far, or this lone non-FS one, can never become
		 * a frame: give them back and scan again from here, or the ring index would park on them. */
		if ((dw1_status & FEMAC_RX_DSC_BIT_FS) ? (seg_cnt != 0) : (seg_cnt == 0)) {
			Ethernet_ReleaseRXDesc(ETH_InitStruct, seg_cnt ? seg_cnt : 1);
			start_idx = ETH_InitStruct->ETH_RxDescCurrentNum;
			scan_idx = start_idx;
			total_pkt_size = 0;
			seg_cnt = 0;
			continue;
		}

		/* Check First Segment (FS) */
		if (dw1_status & FEMAC_RX_DSC_BIT_FS) {
			buf = (u8 *)(ETH_InitStruct->ETH_RxDesc[scan_idx].addr);
		}

		/* Accumulate Length */
		desc_len = FEMAC_RX_DSC_GET_LEN(dw1_status);
		total_pkt_size += desc_len;
		seg_cnt++;

		/* Invalidate Cache */
		DCache_Invalidate((u32)(ETH_InitStruct->ETH_RxDesc[scan_idx].addr), desc_len);

		/* Check Last Segment (LS) */
		if (dw1_status & FEMAC_RX_DSC_BIT_LS) {
			/* Fill Metadata from LS descriptor */
			meta->pkt_len = total_pkt_size;
			ETH_InitStruct->ETH_RxSegmentCount = seg_cnt;

			/* Extract VLAN info; only the LS descriptor carries it. */
			dw2_status = ETH_InitStruct->ETH_RxDesc[scan_idx].dw2;
			if (dw2_status & FEMAC_RX_W2_BIT_CTAVA) {
				u16 raw_tci = (u16)(dw2_status & FEMAC_RX_W2_MASK_CVLAN_TAG);
				meta->vlan_valid = 1;
				meta->vlan_tci = ((raw_tci & 0x00FF) << 8) | ((raw_tci & 0xFF00) >> 8);
			}

			/* Extract Errors (Future extensions can be added here) */
			if (dw1_status & FEMAC_RX_DSC_BIT_CRCERR) {
				meta->rx_crc_err = 1;
			}
#if ETH_CSUM_OFFLOAD
			/* Checksum verdict from the MAC (RXCHKSUM enabled in Ethernet_Init) */
			meta->rx_pkt_type = (u8)((dw1_status & FEMAC_RX_DSC_MASK_PKTTYPE) >> FEMAC_RX_DSC_SHIFT_PKTTYPE);
			if (dw1_status & FEMAC_RX_DSC_BIT_IPV4CSF) {
				meta->csum_ip = 1;
			}
			if (dw1_status & FEMAC_RX_DSC_BIT_L4CSF) {
				meta->csum_l4 = 1;
			}
#endif
			break;
		}

		/* Move to next descriptor */
		scan_idx++;
		if (scan_idx >= ETH_InitStruct->ETH_RxDescNum) {
			scan_idx = 0;
		}

		if (scan_idx == start_idx) {
			/* The whole ring is one frame without an LS: drop it rather than wait forever. */
			RTK_LOGE(TAG, "RX Ring Error: No LS bit found\n");
			Ethernet_ReleaseRXDesc(ETH_InitStruct, seg_cnt);
			return NULL;
		}
	}

	return buf;
}

/**
 * @brief  Replace the buffer of one segment of the current RX frame, for zero-copy RX.
 * @param  ETH_InitStruct Pointer to Ethernet initialization structure.
 * @param  seg            Segment index within the frame: 0 is FS, must be below ETH_RxSegmentCount.
 * @param  new_buf        Buffer handed to DMA in its place: cache-line aligned, ETH_RxBufSize bytes, invalidated.
 * @param  seg_len        Returns the byte count DMA wrote into the old buffer.
 * @return The old buffer address, now owned by the caller.
 * @note   Call after Ethernet_GetRXPktInfo() and before Ethernet_ReleaseRXDesc(), which hands the
 *         descriptors back to DMA. Calling it again with the old buffer undoes the swap.
 */
u32 Ethernet_SwapRXSegBuf(ETH_InitTypeDef *ETH_InitStruct, u32 seg, u32 new_buf, u32 *seg_len)
{
	u32 idx = ETH_InitStruct->ETH_RxDescCurrentNum + seg;
	u32 old_buf;

	if (idx >= ETH_InitStruct->ETH_RxDescNum) {
		idx -= ETH_InitStruct->ETH_RxDescNum;
	}

	old_buf = ETH_InitStruct->ETH_RxDesc[idx].addr;
	*seg_len = FEMAC_RX_DSC_GET_LEN(ETH_InitStruct->ETH_RxDesc[idx].dw1);
	ETH_InitStruct->ETH_RxDesc[idx].addr = new_buf;

	return old_buf;
}

/**
 *  @brief  Get the Ethernet link status.
 *
 *  @return    1 for link up, 0 for link down.
 */
u32 Ethernet_GetLinkStatus(void)
{
	ETHERNET_TypeDef *ETHx = ((ETHERNET_TypeDef *) RMII_REG_BASE);

	return !GET_LINKB(ETHx->ETH_MSR);
}

/**
 * @brief  Get the speed and duplex the MAC is running at.
 * @param  speed  Returns the link speed, see @ref eth_speed.
 * @param  duplex Returns the duplex mode, see @ref eth_duplex.
 */
void Ethernet_GetSpeedDuplex(u32 *speed, u32 *duplex)
{
	ETHERNET_TypeDef *ETHx = ((ETHERNET_TypeDef *) RMII_REG_BASE);
	u32 msr = ETHx->ETH_MSR;

	*speed = GET_SPEED(msr);
	*duplex = GET_FULLDUPREG(msr);
}

/**
 * @brief Wait for the PHY MDIO bus to become idle.
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

 * @param  phy_addr  PHY address (0-31).
 * @param  reg_addr  Register address (0-31).
 * @param data      Pointer to store the read value.
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

 * @param  phy_addr  PHY address (0-31).
 * @param  reg_addr  Register address (0-31).
 * @param  data      Data to write.
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

	return RTK_SUCCESS;
}

/**
  * @brief  Initialize @ref ETH_InitTypeDef.
  * @param  ETH_InitStruct The pointer to @ref ETH_InitTypeDef.
  * @param  PHY_Dev The pointer to @ref eth_phy_dev.
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
	ETH_InitStruct->MacConfig.Bits.PktFilterConfig = ETH_FILTER_DEFAULT_UNICAST; /* Accept unicast + broadcast */
	ETH_InitStruct->MacConfig.Bits.EEEEnable       = DISABLE;               /* Disable EEE */
	ETH_InitStruct->MacConfig.Bits.AutoNego        = ETH_NWAY_ENABLE;       /* Enable Auto-Negotiation */
	ETH_InitStruct->MacConfig.Bits.FlowCtrl        = ETH_FLOW_FULL;         /* Send and honor PAUSE */

	/* EEE parameters, filled only once the caller has pointed EEE_Config at storage */
	if (ETH_InitStruct->MacConfig.Bits.EEEEnable && ETH_InitStruct->EEE_Config) {
		Ethernet_EEE_StructInit(ETH_InitStruct->EEE_Config);
	}

	/* VLAN parameters */
	ETH_InitStruct->VlanConfig.Bits.TxTagType = ETH_VLAN_TYPE_CTAG;
	ETH_InitStruct->VlanConfig.Bits.RxStrip = ETH_VLAN_STRIP_ENABLE;
	ETH_InitStruct->VlanConfig.Bits.STagPID = 0x88A8;  /* standard S-Tag PID */

	/* Buffer / DMA Defaults */
	ETH_InitStruct->DMA_RxThreshold = ETH_RX_THRESHOLD_256B;
	ETH_InitStruct->DMA_TxThreshold = ETH_TX_THRESHOLD_256B;
	ETH_InitStruct->DMA_RxTriggerLevel = ETH_TRIGGER_LEVEL_1_PKT;
	ETH_InitStruct->DMA_TxTriggerLevel = ETH_TRIGGER_LEVEL_1_PKT;

	ETH_InitStruct->ETH_RxDescNum = 8;
	ETH_InitStruct->ETH_TxDescNum = 8;

	/* Default Buffer Sizes */
	/* Slot stride, must match how the caller carves up the pool. The unaligned
	 * frame size would misalign every slot and let a full frame overrun it. */
	ETH_InitStruct->ETH_TxBufSize = ETH_MAX_BUF_SIZE;
	ETH_InitStruct->ETH_RxBufSize = ETH_MAX_BUF_SIZE;

	/* Interrupts */
	/* Only what the ISR acts on. TX completion and RX overflow need no handling, and an RX overflow
	 * interrupt per dropped frame only adds load when the CPU already cannot keep up. Their status
	 * bits still latch and are cleared along with the rest. */
	ETH_InitStruct->ETH_IntMask = BIT_IMR_LINKCHG | BIT_IMR_ROK;
}

/**
  * @brief  Initialize Ethernet MAC, PHY, and DMA descriptors.
  * @param  ETH_InitStruct Pointer to @ref ETH_InitTypeDef containing the full Ethernet configuration.
  * @return RTK_SUCCESS on success, or a negative error code:
  *         - -RTK_ERR_BADARG if ETH_InitStruct, phy_dev, or phy_dev->ops is NULL.
  *         - -RTK_ERR_TIMEOUT if MAC reset timed out.
  * @note   Does not wait for auto-negotiation: it can take seconds, or never finish with no cable,
  *         and the link change interrupt reports the result.
  */
int Ethernet_Init(ETH_InitTypeDef *ETH_InitStruct)
{
	ETHERNET_TypeDef *ETHx = ((ETHERNET_TypeDef *) RMII_REG_BASE);
	u32 temp = 0;
	u16 i;
	u8 filter_mode;
	u32 timeout = 1000, pse_on, pse_off;
	int32_t ret = RTK_SUCCESS;
	struct eth_phy_dev *phy = NULL;

	if (!ETH_InitStruct || !ETH_InitStruct->phy_dev || !ETH_InitStruct->phy_dev->ops) {
		RTK_LOGE(TAG, "Init: Invalid Param or PHY Dev\r\n");
		ret = -RTK_ERR_BADARG;
		goto exit;
	}

	/* The per-packet TRX functions rely on this check and do not repeat it. RINGSIZE takes the RX count
	 * minus one as a mask, and the PAUSE thresholds need at least 4. */
	if (!ETH_InitStruct->ETH_TxDesc || !ETH_InitStruct->ETH_RxDesc || !ETH_InitStruct->ETH_TxDescNum || ETH_InitStruct->ETH_RxDescNum < 4 ||
		ETH_InitStruct->ETH_RxDescNum > 4096 || (ETH_InitStruct->ETH_RxDescNum & (ETH_InitStruct->ETH_RxDescNum - 1))) {
		RTK_LOGE(TAG, "Init: Invalid desc ring\n");
		ret = -RTK_ERR_BADARG;
		goto exit;
	}

	phy = ETH_InitStruct->phy_dev;

	Ethernet_AutoPolling(DISABLE);
	DelayMs(50);

	/* reset MAC */
	ETHx->ETH_CR |= BIT_RST;

	/* timeout-- in the loop condition wraps to 0xFFFFFFFF on expiry, so test it inside instead. */
	while (ETHx->ETH_CR & BIT_RST) {
		if (timeout-- == 0) {
			ret = -RTK_ERR_TIMEOUT;
			RTK_LOGE(TAG, "MAC Reset Timeout\n");
			goto exit;
		}
		DelayUs(10);
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
		phy->link_cfg.advertise_pause = TRUE;
		phy->link_cfg.advertise_asym_pause = TRUE;
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

	/* TXFCE sends PAUSE, RXFCE honors it; both act in full duplex only. */
	ETHx->ETH_MSR &= ~(BIT_TXFCE | BIT_RXFCE);
	if (ETH_InitStruct->MacConfig.Bits.FlowCtrl & ETH_FLOW_TX_ONLY) {
		ETHx->ETH_MSR |= BIT_TXFCE;
	}
	if (ETH_InitStruct->MacConfig.Bits.FlowCtrl & ETH_FLOW_RX_ONLY) {
		ETHx->ETH_MSR |= BIT_RXFCE;
	}
	ETHx->ETH_MSR |= REFCLK_PHASE(ETH_InitStruct->MacConfig.Bits.RefClkPhase);

	ETHx->ETH_TCR &= ~MASK_IFG2_0;
	ETHx->ETH_TCR |= IFG2_0(ETH_InitStruct->MacConfig.Bits.IFGTime);

	ETHx->ETH_TCR &= ~MASK_LBK;
	ETHx->ETH_TCR |= LBK(ETH_InitStruct->MacConfig.Bits.Loopback);

	/* Rx settings */
	Ethernet_SetMacAddr(ETH_InitStruct->ETH_MacAddr);

#if ETH_CSUM_OFFLOAD
	/* MAC verify IPv4/TCP/UDP checksums. Results land in the RX
	 * descriptor as IPV4CSF/L4CSF, read back by Ethernet_GetRXPktInfo(). */
	ETHx->ETH_CR |= BIT_RXCHKSUM;
#endif

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
	/* EEE configuration */
	if (ETH_InitStruct->MacConfig.Bits.EEEEnable && ETH_InitStruct->EEE_Config) {
		Ethernet_EEE_Init(ETHx, ETH_InitStruct->EEE_Config);
		if (phy->ops->cfg_eee) {
			phy->ops->cfg_eee(phy, ENABLE);
		}
	}
	/* Packet filter */
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
	/* PAUSE on when the free RX descriptors minus one fall to pse_on, off when back to pse_off.
	 * A higher ON or OFF keeps the sender stopped too long: 0/2 measured best with 4 descriptors. */
	pse_on = ETH_InitStruct->ETH_RxDescNum / 3 - 1;
	pse_off = ETH_InitStruct->ETH_RxDescNum / 2;
	ETHx->ETH_ETHRNTRXCPU_DES_NUM1 = RX_PSE_DES_THRES_OFF_1_7_0(pse_off) | \
									 RX_PSE_DES_THRES_ON_1_11_8(pse_on >> 8) | RX_PSE_DES_THRES_ON_1_7_0(pse_on) | \
									 ETHRNTRXCPU_DES_NUM_1_11_8((ETH_InitStruct->ETH_RxDescNum - 1) >> 8) | ETHRNTRXCPU_DES_NUM_1_7_0(ETH_InitStruct->ETH_RxDescNum - 1);
	/* OFF bits 11:8 live in another register, next to clock settings that must be kept. */
	ETHx->ETH_CONFIG_CLK_AND_RX_PSE_DES_THRES_H = (ETHx->ETH_CONFIG_CLK_AND_RX_PSE_DES_THRES_H & ~MASK_RX_PSE_DES_THRES_OFF_1) |
			RX_PSE_DES_THRES_OFF_1(pse_off >> 8);
	/* Overwrite both halves of the size field: |= kept whatever a previous init left there. */
	ETHx->ETH_RX_RINGSIZE1 = (ETHx->ETH_RX_RINGSIZE1 & ~(MASK_RXRINGSIZE_1_LOW | MASK_RXRINGSIZE_1_HIGH)) |
							 RXRINGSIZE_1_LOW(ETH_InitStruct->ETH_RxDescNum - 1) | RXRINGSIZE_1_HIGH((ETH_InitStruct->ETH_RxDescNum - 1) >> 8);

	ETHx->ETH_IO_CMD1 = DSC_FORMAT_EXTRA(0x3) | BIT_EN_4GB | BIT_RXOKINT_MSK_128B;
	/* RX interrupt mitigation: raise RxOK once per 4 frames instead of once per
	 * frame. The timer is the escape hatch for when fewer than 4 arrive, and it
	 * has to cover the time 4 full frames take, or it expires first and the
	 * batching never happens. At 100M a 1518B frame plus preamble and IFG
	 * occupies 123.04us, so 4 frames need 492us. The timeout is
	 * RXPKTTIMER x 4 x TU, and TU is 40.96us at 100M for TIMER_SEL=2, giving
	 * 3 x 4 x 40.96 = 491.5us.
	 * Two hardware details this relies on: the timer restarts when RxOK asserts
	 * rather than on every frame, and it does not apply below 128 bytes, so bare
	 * TCP ACKs still raise RxOK immediately and TX is unaffected. */
	ETHx->ETH_ETHER_IO_CMD = RXFTH(ETH_InitStruct->DMA_RxThreshold) | TSH(ETH_InitStruct->DMA_TxThreshold) |
							 BIT_SHORTDESFORMAT | RXINTMITIGATION_2_0(1) |
							 REG_INT_TIMER_SEL(2) | RXPKTTIMER_2_0(3);

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
		/* A NULL ETH_RxPktBuf means the caller already put a buffer in each descriptor (zero-copy RX). */
		if (ETH_InitStruct->ETH_RxPktBuf) {
			ETH_InitStruct->ETH_RxDesc[i].addr = (u32)(ETH_InitStruct->ETH_RxPktBuf + (i * ETH_InitStruct->ETH_RxBufSize));
		}
		ETH_InitStruct->ETH_RxDesc[i].dw2 = 0;
		ETH_InitStruct->ETH_RxDesc[i].dw3 = 0;
	}

	/* enable Tx & Rx */
	ETHx->ETH_ETHER_IO_CMD |= BIT_TE | BIT_RE;

	/* isr & imr */
	/* Drop status left over from before init, then unmask. */
	ETHx->ETH_ISR = 0xFFFF;
	ETHx->ETH_IMR = ETH_InitStruct->ETH_IntMask;

	/* enable auto-polling */
	Ethernet_AutoPolling(ENABLE);

	/* enable Rx ring1 */
	ETHx->ETH_IO_CMD1 |= BIT_RXRING1;

exit:
	return ret;
}
/** @} */

/** @} */

/** @} */
