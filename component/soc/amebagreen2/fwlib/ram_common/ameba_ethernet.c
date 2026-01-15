/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *const TAG = "ETH";

#define CHECK_PHY_STATUS
/**
  * @brief  clear all interrupt
  * @param  Data: the data pointer to IPCx
  * @retval 0
  */
u32 Ethernet_ClearALLINT(void)
{
	ETHERNET_TypeDef *RMII = ((ETHERNET_TypeDef *) RMII_REG_BASE);
	RMII->ETH_ISR_AND_IMR = 0xFFFF;
	return 0;
}

u32 Ethernet_GetINT(void)
{
	ETHERNET_TypeDef *RMII = ((ETHERNET_TypeDef *) RMII_REG_BASE);
	return RMII->ETH_ISR_AND_IMR;
}


u32 Ethernet_ClearINT(u32 INTrBit)
{
	ETHERNET_TypeDef *RMII = ((ETHERNET_TypeDef *) RMII_REG_BASE);
	RMII->ETH_ISR_AND_IMR |= INTrBit;
	return 0;

}

/**
 * @addtogroup RMII_IRQHandler
 * @ingroup
 * @{
 * @brief
 */

__weak u32 RMII_IRQHandler(ETH_InitTypeDef *ETH_InitStruct)
{
	ETHERNET_TypeDef *RMII = ((ETHERNET_TypeDef *) RMII_REG_BASE);

	u32 intr_status;
	intr_status = Ethernet_GetINT();

	if ((intr_status & BIT_ISR_ROK) && (intr_status & BIT_IMR_ROK)) {
		if ((ETH_InitStruct->callback) != NULL) {
			ETH_InitStruct->callback(EthRxDone, 0);
		}
		Ethernet_ClearINT(BIT_ISR_ROK);
	}

	if ((intr_status & BIT_ISR_RER_OVF) && (intr_status & BIT_IMR_RER_OVF)) {
		Ethernet_ClearINT(BIT_ISR_RER_OVF);
	}

	if ((intr_status & BIT_ISR_TOK_TI) && (intr_status & BIT_IMR_TOK_TI)) {
		if ((ETH_InitStruct->callback) != NULL) {
			ETH_InitStruct->callback(EthTxDone, 0);
		}
		Ethernet_ClearINT(BIT_ISR_TOK_TI);
	}

	if ((intr_status & BIT_ISR_LINKCHG) && (intr_status & BIT_IMR_LINKCHG)) {
		if ((ETH_InitStruct->callback) != NULL) {
			if (!(GET_LINKB(RMII->ETH_MSR))) {
				ETH_InitStruct->callback(EthLinkUp, 0);
			} else {
				ETH_InitStruct->callback(EthLinkDown, 0);
			}
			Ethernet_ClearINT(BIT_ISR_LINKCHG);
		}

	}

	return 0;
}

/**
 *  @brief To set the start address of Tx/Rx descriptor ring.
 *
 *  @param[in]  ETH_InitStruct The pointer to ETH_InitTypeDef.
 *  @param[in]  tx_desc The start address of Tx descriptor ring.
 *  @param[in]  rx_desc The start address of Rx descriptor ring.
 *
 *  @returns    void.
 */
void Ethernet_SetDescAddr(ETH_InitTypeDef *ETH_InitStruct)
{

	ETHERNET_TypeDef *RMII = ((ETHERNET_TypeDef *) RMII_REG_BASE);

	if ((ETH_InitStruct == NULL) || (ETH_InitStruct->ETH_TxDesc == NULL) || (ETH_InitStruct->ETH_RxDesc == NULL)) {
		RTK_LOGE(TAG, "Invalid parameter !!\r\n");
		return;
	}
	if ((((u32)ETH_InitStruct->ETH_TxDesc) & 0x1F) || (((u32)ETH_InitStruct->ETH_RxDesc) & 0x1F)) {
		RTK_LOGE(TAG, "The descriptor address must be 32-Byte alignment !!\r\n");
		return;
	}

	RMII->ETH_TXFDP1 = (u32)(ETH_InitStruct->ETH_TxDesc);
	RMII->ETH_RX_FDP1 = (u32)(ETH_InitStruct->ETH_RxDesc);
}

/**
 *  @brief To set the ethernet MAC address.
 *
 *  @param[in]  ETH_InitStruct The pointer to ETH_InitTypeDef.
 *
 *  @returns    void.
 */
void Ethernet_SetMacAddr(u8 *ETH_MacAddr)
{
	ETHERNET_TypeDef *RMII = ((ETHERNET_TypeDef *) RMII_REG_BASE);

	if (ETH_MacAddr == NULL) {
		RTK_LOGE(TAG, "Invalid parameter !!\r\n");
		return;
	}

	RMII->ETH_IDR0 = ((ETH_MacAddr[0]) << 24) | ((ETH_MacAddr[1]) << 16) | ((ETH_MacAddr[2]) << 8) |
					 (ETH_MacAddr[3]);
	RMII->ETH_IDR4 = ((ETH_MacAddr[4]) << 24) | ((ETH_MacAddr[5]) << 16);
}

/**
  * @brief  To set the ethernet refclk mode.
  * @param  refclk_mode: 1 ETH_REFCLK_OFF   0 ETH_REFCLK_ON
  * @retval None
  */
void Ethernet_SetRefclkDirec(u32 refclk_mode)
{
	ETHERNET_TypeDef *RMII = ((ETHERNET_TypeDef *) RMII_REG_BASE);

	if (refclk_mode) {
		RMII->ETH_MSR |= BIT_REFCLK_ON; // REFCLK on

	} else {
		RMII->ETH_MSR &= ~BIT_REFCLK_ON; // REFCLK off
	}
}

/**
  * @brief  use externel 50M clk as source clk.
  * @param  pin
  * @retval None
  */
void Ethernet_UseExtClk(u32 pin)
{
	Pinmux_Config(pin, PINMUX_FUNCTION_EXT_CLK50M_IN);
}

/**
  * @brief  output 50M clk to phy
  * @param  pin
  * @retval None
  */
void Ethernet_OutputClk2Phy(u32 pin)
{
	Pinmux_Config(pin, PINMUX_FUNCTION_EXT_CLK_OUT);
}

void Ethernet_AutoPolling(u32 opt)
{
	ETHERNET_TypeDef *RMII = ((ETHERNET_TypeDef *) RMII_REG_BASE);
	if (opt == DISABLE) {
		RMII->ETH_MIIAR |= BIT_DISABLE_AUTO_POLLING;
	} else {
		RMII->ETH_MIIAR &= ~BIT_DISABLE_AUTO_POLLING;
	}
}

/**
 *  @brief Get current tx pkt buf address
 *
 *  @param[in]  ETH_InitStruct The pointer to ETH_InitTypeDef.
 *
 *  @returns    The tx pkt buf address.
 */
u8 *Ethernet_GetTXPktInfo(ETH_InitTypeDef *ETH_InitStruct)
{
	ETHERNET_TypeDef *RMII = ((ETHERNET_TypeDef *) RMII_REG_BASE);
	u8 tx_idx = ETH_InitStruct->ETH_TxDescCurrentNum;
	u8 *buf = NULL;

	if (ETH_InitStruct == NULL) {
		RTK_LOGE(TAG, "Invalid parameter !!\r\n");
		return NULL;
	}

	/* check if current Tx descriptor is available */
	if ((((u32)(ETH_InitStruct->ETH_TxDesc[tx_idx].dw1)) & FEMAC_TX_DSC_BIT_OWN) == 0) {
		buf = (u8 *)ETH_InitStruct->ETH_TxDesc[tx_idx].addr;
	} else {
		RMII->ETH_ISR_AND_IMR |= BIT_ISR_TOK_TI;
	}

	return buf;
}

/**
 *  @brief Update txdesc and send pkt
 *
 *  @param[in]  ETH_InitStruct The pointer to ETH_InitTypeDef.
 *  @param[in]  size The size of this tx pkt.
 *
 *  @returns    None.
 */
void Ethernet_UpdateTXDESCAndSend(ETH_InitTypeDef *ETH_InitStruct, u32 size)
{
	ETHERNET_TypeDef *RMII = ((ETHERNET_TypeDef *) RMII_REG_BASE);
	u8 tx_idx = ETH_InitStruct->ETH_TxDescCurrentNum;

	if (ETH_InitStruct == NULL) {
		RTK_LOGE(TAG, "Invalid parameter !!\r\n");
		return;
	}

	if (size > ETH_InitStruct->ETH_TxBufSize) {
		RTK_LOGE(TAG, "Invalid size !!\r\n");
		return;
	}

	DCache_Clean((u32)ETH_InitStruct->ETH_TxDesc[tx_idx].addr, (u32)size); //TODO: need invalidate or not

	ETH_InitStruct->ETH_TxDesc[tx_idx].dw2 = 0;
	ETH_InitStruct->ETH_TxDesc[tx_idx].dw3 = 0;
	ETH_InitStruct->ETH_TxDesc[tx_idx].dw4 = 0;

	if (tx_idx == ((ETH_InitStruct->ETH_TxDescNum) - 1)) {
		ETH_InitStruct->ETH_TxDesc[tx_idx].dw1 = FEMAC_TX_DSC_BIT_EOR | FEMAC_TX_DSC_BIT_FS | FEMAC_TX_DSC_BIT_IPCS | \
				FEMAC_TX_DSC_BIT_L4CS | FEMAC_TX_DSC_BIT_LS | FEMAC_TX_DSC_BIT_CRC | \
				FEMAC_TX_DSC_BIT_SIZE(size);
	} else {
		ETH_InitStruct->ETH_TxDesc[tx_idx].dw1 = FEMAC_TX_DSC_BIT_FS | FEMAC_TX_DSC_BIT_LS | FEMAC_TX_DSC_BIT_IPCS | \
				FEMAC_TX_DSC_BIT_L4CS | FEMAC_TX_DSC_BIT_CRC | FEMAC_TX_DSC_BIT_SIZE(size);
	}
	ETH_InitStruct->ETH_TxDesc[tx_idx].dw1 |= FEMAC_TX_DSC_BIT_OWN;

	RMII->ETH_ISR_AND_IMR |= BIT_ISR_TOK_TI;
	RMII->ETH_ETHER_IO_CMD |= BIT_TXFN1ST; //TODO: not need set for each pkt

	if (tx_idx == ((ETH_InitStruct->ETH_TxDescNum) - 1)) {
		ETH_InitStruct->ETH_TxDescCurrentNum = 0;
	} else {
		ETH_InitStruct->ETH_TxDescCurrentNum++;
	}
}

/**
 *  @brief Get current rx pkt buf address
 *
 *  @param[in]  ETH_InitStruct The pointer to ETH_InitTypeDef.
 *  @param[in]  rx_len The pointer to the actual rx pkt length.
 *
 *  @returns    The rx pkt buf address.
 */
u8 *Ethernet_GetRXPktInfo(ETH_InitTypeDef *ETH_InitStruct, u32 *rx_len)
{
	ETHERNET_TypeDef *RMII = ((ETHERNET_TypeDef *) RMII_REG_BASE);
	u32 pkt_size = 0;
	u8 rx_idx = ETH_InitStruct->ETH_RxDescCurrentNum;
	u8 *buf = NULL;

	/* check if current Rx descriptor is available */
	if ((((volatile u32)(ETH_InitStruct->ETH_RxDesc[rx_idx].dw1)) & FEMAC_RX_DSC_BIT_OWN) == 0) {
		pkt_size = FEMAC_RX_DSC_LEN(ETH_InitStruct->ETH_RxDesc[rx_idx].dw1);
		buf = (u8 *)(ETH_InitStruct->ETH_RxDesc[rx_idx].addr + 2);
		DCache_Invalidate((u32)(ETH_InitStruct->ETH_RxDesc[rx_idx].addr), (u32)pkt_size);
	} else {
		RMII->ETH_ISR_AND_IMR |= BIT_ISR_ROK;
	}

	*rx_len = pkt_size;

	return buf;
}

/**
 *  @brief Update rxdesc
 *
 *  @param[in]  ETH_InitStruct The pointer to ETH_InitTypeDef.
 *
 *  @returns    None.
 */
void Ethernet_UpdateRXDESC(ETH_InitTypeDef *ETH_InitStruct)
{
	ETHERNET_TypeDef *RMII = ((ETHERNET_TypeDef *) RMII_REG_BASE);
	u8 read_idx = ETH_InitStruct->ETH_RxDescCurrentNum;

	if ((ETH_InitStruct == NULL)) {
		RTK_LOGE(TAG, "Invalid parameter !!\r\n");
		return;
	}


	ETH_InitStruct->ETH_RxDesc[read_idx].dw1 &= FEMAC_RX_DSC_BIT_EOR;
	ETH_InitStruct->ETH_RxDesc[read_idx].dw1 |= (FEMAC_RX_DSC_BIT_OWN | ETH_InitStruct->ETH_RxBufSize);
	ETH_InitStruct->ETH_RxDesc[read_idx].dw2 = 0;
	ETH_InitStruct->ETH_RxDesc[read_idx].dw3 = 0;

	if (read_idx == ((ETH_InitStruct->ETH_RxDescNum) - 1)) {
		ETH_InitStruct->ETH_RxDescCurrentNum = 0;
	} else {
		ETH_InitStruct->ETH_RxDescCurrentNum++;
	}

	RMII->ETH_ISR_AND_IMR |= BIT_ISR_ROK;
	RMII->ETH_IO_CMD1 |= BIT_RXRING1; // TODO: no need for each pkt
}

/**
 *  @brief To get the link status.
 *
 *  @returns    1 for link up, 0 for link down.
 */
u32 Ethernet_GetLinkStatus(void)
{
	ETHERNET_TypeDef *RMII = ((ETHERNET_TypeDef *) RMII_REG_BASE);

	if (GET_LINKB(RMII->ETH_MSR) == eth_link_up) {
		return 1;
	} else {
		return 0;
	}
}


u8 Ethernet_WaitBusy(u32 WaitType)
{
	ETHERNET_TypeDef *RMII = ((ETHERNET_TypeDef *) RMII_REG_BASE);
	u32 BusyCheck = 0;
	u8 status = 0;
	u32 i = 0;

	do {
		if (WaitType == WAIT_SMI_WRITE_DONE) {
			i++;
			BusyCheck = (RMII->ETH_MIIAR & (BIT_FLAG | BIT_MDIO_BUSY));

		} else if (WaitType == WAIT_SMI_READ_DONE) {
			i++;
			BusyCheck = (((RMII->ETH_MIIAR & BIT_MDIO_BUSY) != 0) || ((RMII->ETH_MIIAR & BIT_FLAG) == 0));

		} else if (WaitType == WAIT_RMII_LINKUP) {
			i++;
			status = (GET_LINKB(RMII->ETH_MSR));
			BusyCheck = (status == eth_link_up ? 0 : 1);
		}

		if (!BusyCheck) {
			break;
		}

		if (i > 0x200000) {
			status = 0xFF;
			RTK_LOGS(TAG, RTK_LOG_ERROR, "TIME OUT\n");
			break;
		}
	} while (1);
	return status;
}

/**
 * @brief Waits for the PHY MDIO bus to become idle.
 *        This function polls the MDIO busy flag until it is cleared or a timeout occurs.
 * @param regs         Pointer to the Ethernet MAC register structure.
 * @param timeout_us   Timeout threshold in microseconds.
 * @return 0 on success (MDIO idle), -1 on timeout.
 */
static inline int Ethernet_WaitPhyIdle(ETHERNET_TypeDef *regs, uint32_t timeout_us)
{
	uint32_t start = DTimestamp_Get();

	/* Poll until BIT_MDIO_BUSY is cleared */
	while ((DTimestamp_Get() - start) <= timeout_us) {
		if (!(regs->ETH_MIIAR & BIT_MDIO_BUSY)) {
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
	ETHERNET_TypeDef *RMII = ((ETHERNET_TypeDef *) RMII_REG_BASE);

	/* Parameter check */
	if (reg_addr > 31) {
		return -RTK_FAIL;
	}

	/* 2. Wait for bus idle (Legacy logic: check busy before operation) */
	if (Ethernet_WaitPhyIdle(RMII, MDIO_WAIT_TIME) != RTK_SUCCESS) {
		return -RTK_ERR_BUSY;
	}

	/* 3. Initiate Read: Write PHY address and Reg address to MIIAR */
	/* Note: For Read, BIT_FLAG is 0. */
	RMII->ETH_MIIAR = (PHYADDRESS(phy_addr) | REGADDR4_0(reg_addr));

	/* 4. Wait for data to be ready (Busy bit clears when read is done) */
	if (Ethernet_WaitPhyIdle(RMII, MDIO_WAIT_TIME) != RTK_SUCCESS) {
		return -RTK_ERR_BUSY;
	}

	/* 5. Extract the 16-bit data */
	*data = (uint16_t)GET_DATA15_0(RMII->ETH_MIIAR);

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
	ETHERNET_TypeDef *RMII = ((ETHERNET_TypeDef *) RMII_REG_BASE);

	if (reg_addr > 31) {
		return -RTK_FAIL;
	}

	/* 1. Wait for bus idle */
	if (Ethernet_WaitPhyIdle(RMII, MDIO_WAIT_TIME) != RTK_SUCCESS) {
		return -RTK_ERR_BUSY;
	}

	/* 2. Initiate Write: Set Flag, Disable Auto Poll, Addr, and Data */
	RMII->ETH_MIIAR = (BIT_FLAG | BIT_DISABLE_AUTO_POLLING |
					   PHYADDRESS(phy_addr) | REGADDR4_0(reg_addr) | data);

	/* Note: The write is triggered immediately. Some drivers wait for idle here too. */
	return RTK_SUCCESS;
}

/**
  * \brief  Enable ethernet RX.
  * \param  None.
  * \return None.
  */
void ETH_EnableRx(void)
{
	ETHERNET_TypeDef *RMII = ((ETHERNET_TypeDef *) RMII_REG_BASE);
	/* enable Rx ring1 */
	RMII->ETH_IO_CMD1 |= BIT_RXRING1;
}

/**
  * \brief  Initialize ETH_InitTypeDef.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \return None.
  */
void Ethernet_StructInit(ETH_InitTypeDef *ETH_InitStruct, struct eth_phy_dev *PHY_Dev)
{
	if (!ETH_InitStruct || !PHY_Dev) {
		return;
	}

	_memset(ETH_InitStruct, 0, sizeof(ETH_InitTypeDef));

	/* Set Defaults - decouple specific PHY ID */
	ETH_InitStruct->ETH_Phy_Type = 0; // Should be set by upper layer or detected
	ETH_InitStruct->ETH_Phy_mode = ETH_MAC_MODE;
	ETH_InitStruct->ETH_IFGTime = ETH_IFG_3;
	ETH_InitStruct->ETH_Mac_LoopBackMode = ETH_NORMAL_MODE;
	ETH_InitStruct->ETH_Phy_LooPBackMode = ETH_PHY_NORMAL_MODE;

	/* Timing defaults */
	ETH_InitStruct->ETH_PhyRxSetupTime = ETH_PHY_RX_SETUP_TIME_8NS;
	ETH_InitStruct->ETH_PhyTxSetupTime = ETH_PHY_TX_SETUP_TIME_6NS;

	/* MAC Filter config */
	ETH_InitStruct->ETH_RCR = BIT_AAP | BIT_APM | BIT_AM | BIT_AB | BIT_AR | BIT_AER | BIT_AFLOW;

	/* Buffer / DMA Defaults */
	ETH_InitStruct->ETH_RxJumbo = ETH_RX_JUMBO_ENABLE;
	ETH_InitStruct->ETH_RefClkPhase = ETH_SAMPLED_ON_RISING_EDGE;
	ETH_InitStruct->ETH_RefClkDirec = ETH_REFCLK_ON;
	ETH_InitStruct->ETH_RxThreshold = ETH_RX_THRESHOLD_256B;
	ETH_InitStruct->ETH_TxThreshold = ETH_TX_THRESHOLD_256B;
	ETH_InitStruct->ETH_RxTriggerLevel = ETH_TRIGGER_LEVEL_1_PKT;
	ETH_InitStruct->ETH_TxTriggerLevel = ETH_TRIGGER_LEVEL_1_PKT;

	ETH_InitStruct->ETH_RxDescNum = 10;
	ETH_InitStruct->ETH_TxDescNum = 8;

	/* Default Buffer Sizes */
	ETH_InitStruct->ETH_TxAllocBufSize = 1600;
	ETH_InitStruct->ETH_RxAllocBufSize = 1600;
	ETH_InitStruct->ETH_TxBufSize = ETH_PKT_MAX_SIZE;
	ETH_InitStruct->ETH_RxBufSize = ETH_PKT_MAX_SIZE;

	/* Interrupts */
	ETH_InitStruct->ETH_IntMaskAndStatus = BIT_IMR_LINKCHG | BIT_IMR_TOK_TI |
										   BIT_IMR_RER_OVF | BIT_IMR_ROK | 0xFFFF;
	/* Link Driver */
	ETH_InitStruct->phy_dev = PHY_Dev;
}

int Ethernet_Init(ETH_InitTypeDef *ETH_InitStruct)
{
	ETHERNET_TypeDef *RMII = ((ETHERNET_TypeDef *) RMII_REG_BASE);
	u8 i;
	u32 timeout = 1000;
	struct eth_phy_dev *phy = NULL;

	if (!ETH_InitStruct || !ETH_InitStruct->phy_dev) {
		RTK_LOGE(TAG, "Init: Invalid Param or PHY Dev\r\n");
		return RTK_ERR_BADARG;
	}

	phy = ETH_InitStruct->phy_dev;
	Ethernet_AutoPolling(DISABLE);
	DelayMs(50);

	/* reset MAC */
	RMII->ETH_CR |= BIT_RST;

	while ((RMII->ETH_CR & BIT_RST) && timeout--) {
		DelayUs(10);
	}
	if (timeout == 0) {
		RTK_LOGE(TAG, "MAC Reset Timeout\r\n");
		return -RTK_ERR_TIMEOUT;
	}

	/* reset phy */
	if (phy->ops && phy->ops->init) {
		phy->ops->init(phy);
	}
	if (phy->ops && phy->ops->reset) {
		phy->ops->reset(phy);
	}

	if (phy->ops && phy->ops->cfg_clock) {
		phy->ops->cfg_clock(phy, PHY_CLK_OSC_50M);
	}

#ifdef ENABLE_EEE_FUNCTION
	if (phy->ops && phy->ops->cfg_eee) {
		phy->ops->cfg_eee(phy, PHY_EEE_DISABLE);
	}
#endif
	if (phy->ops && phy->ops->autoneg_restart) {
		phy->ops->autoneg_restart(phy);
	}


	/* Tx settings */

	RMII->ETH_MSR |= BIT_REG_RMII2MII_EN;
	Ethernet_SetRefclkDirec(ETH_InitStruct->ETH_RefClkDirec);

	RMII->ETH_MSR |= BIT_TXFCE;
	RMII->ETH_MSR |= BIT_RXFCE;
	RMII->ETH_MSR |= REFCLK_PHASE(ETH_InitStruct->ETH_RefClkPhase);

	RMII->ETH_TCR &= ~MASK_IFG2_0;
	RMII->ETH_TCR |= IFG2_0(eth_ifg_time_3);

	RMII->ETH_TCR &= ~MASK_LBK;
	RMII->ETH_TCR |= LBK(eth_normal_op);

	/* Rx settings */
	Ethernet_SetMacAddr(ETH_InitStruct->ETH_MacAddr);
	RMII->ETH_RCR = BIT_AAP | BIT_APM | BIT_AM | BIT_AB | BIT_AR | BIT_AER; // Accept error/runt/broadcast/multicast packets, etc.

	// RMII->ETH_CR |= BIT_RXJUMBO; // Support jumbo packet
	RMII->ETH_ETHRNTRXCPU_DES_NUM1 = RX_PSE_DES_THRES_OFF_1_7_0(ETH_InitStruct->ETH_RxDescNum - 1) | RX_PSE_DES_THRES_ON_1_7_0(0x0) | ETHRNTRXCPU_DES_NUM_1_7_0(
										 ETH_InitStruct->ETH_RxDescNum);

	/* I/O command */
	RMII->ETH_IO_CMD1 = DSC_FORMAT_EXTRA(0x3) | BIT_EN_4GB; //? Extra desc. format = 011, support 1GB addressing
	RMII->ETH_ETHER_IO_CMD = RXFTH(ETH_InitStruct->ETH_RxThreshold) | TSH(ETH_InitStruct->ETH_TxThreshold) |
							 BIT_SHORTDESFORMAT; // short desc. format = 1, Tx & Rx FIFO threshold = 256 bytes

	RMII->ETH_TXFDP1 = (u32)ETH_InitStruct->ETH_TxDesc;
	RMII->ETH_RX_FDP1 = (u32)ETH_InitStruct->ETH_RxDesc;

	/* initialize Tx descriptors */
	for (i = 0; i < (ETH_InitStruct->ETH_TxDescNum); i++) {
		ETH_InitStruct->ETH_TxDesc[i].dw1 = 0;
		ETH_InitStruct->ETH_TxDesc[i].addr = (u32)(ETH_InitStruct->ETH_TxPktBuf + (i * ETH_InitStruct->ETH_TxBufSize));
		ETH_InitStruct->ETH_TxDesc[i].dw2 = 0;//(eth_vlan_hdr_remove << 25) | (ETH_C_VLAN_HDR & 0xFFFF);
		ETH_InitStruct->ETH_TxDesc[i].dw3 = 0;
		ETH_InitStruct->ETH_TxDesc[i].dw4 = 0;
	}

	/* initialize Rx descriptors */
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

	RMII->ETH_RX_RINGSIZE1 |= RXRINGSIZE_1_LOW(ETH_InitStruct->ETH_RxDescNum - 1);
	/* enable Tx & Rx */
	RMII->ETH_ETHER_IO_CMD |= BIT_TE | BIT_RE;

	/* isr & imr */
	RMII->ETH_ISR_AND_IMR =  ETH_InitStruct->ETH_IntMaskAndStatus;

	/* enable auto-polling */
	Ethernet_AutoPolling(ENABLE);

	while ((((RMII->ETH_MSR) & BIT_NWCOMPLETE) >> 21) == 0x0);

	/* Wait MAC's link is up (Hardware polling result) */
	if (Ethernet_WaitBusy(WAIT_RMII_LINKUP) == eth_link_up) {
		link_is_up = 1;
	}

	/* Get MAC's link info. */
	switch (GET_SPEED(RMII->ETH_MSR)) {
	case eth_speed_100:
		if (GET_FULLDUPREG(RMII->ETH_MSR) == eth_full_duplex) {
			RTK_LOGI(TAG, "MAC's link info.: 100 Mb/s, Full duplex\r\n");
		} else {
			RTK_LOGI(TAG, "MAC's link info.: 100 Mb/s, Half duplex\r\n");
		}
		break;
	case eth_speed_10:
		if (GET_FULLDUPREG(RMII->ETH_MSR) == eth_full_duplex) {
			RTK_LOGI(TAG, "MAC's link info.: 10 Mb/s, Full duplex\r\n");
		} else {
			RTK_LOGI(TAG, "MAC's link info.: 10 Mb/s, Half duplex\r\n");
		}
		break;
	default:
		RTK_LOGI(TAG, "MAC's link speed: %d\r\n", GET_FULLDUPREG(RMII->ETH_MSR));
		break;
	}
	/* enable Rx ring1 */
	ETH_EnableRx();

#ifdef CHECK_PHY_STATUS
	phy_link_state_t state = {0};

	if (phy->ops && phy->ops->get_link) {
		phy->ops->get_link(phy, &state);
	}

	RTK_LOGI(TAG, "link up=%d, speed=%d, duplex=%d\n", state.link_up, state.speed, state.duplex);

#endif
	return RTK_SUCCESS;
}