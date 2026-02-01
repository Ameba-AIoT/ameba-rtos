/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
static const char *const TAG = "PSRAM";
u8 APM_WR_INIT_LATENCY_SPEC[6] = {
	APM_WR_INIT_LATENCY_3CLK,
	APM_WR_INIT_LATENCY_4CLK,
	APM_WR_INIT_LATENCY_5CLK,
	APM_WR_INIT_LATENCY_6CLK,
	APM_WR_INIT_LATENCY_7CLK,
	APM_WR_INIT_LATENCY_8CLK,
};


static const u32 PSRAM_CALIB_PATTERN[6] = {
	0x11223344,
	0xA55AA55A,
	0x5AA55AA5,
	0x44332211,
	0x96696996,
	0x69969669,
};
PSRAMINFO_TypeDef PsramInfo;


/**
 * @brief  Initialize PSRAMINFO_TypeDef according to the memory configuration from MCM_MemTypeDef.
 * @param  meminfo [in]  Pointer to MCM_MemTypeDef structure (typically from ChipInfo_MCMInfo()).
 * @param  info    [out] Pointer to PSRAMINFO_TypeDef structure to fill in.
 * @return None
 * @note
 */
void ChipInfo_InitPsramInfoFromMemInfo(const MCM_MemTypeDef *meminfo, PSRAMINFO_TypeDef *info)
{
	_memset(info, 0, sizeof(PSRAMINFO_TypeDef));
	info->Psram_Page_size = 0;
	info->Psram_Clk_Limit = 0;
	info->Psram_Type      = 0;
	RRAM_TypeDef *rram = RRAM_DEV;

	if (meminfo == NULL || info == NULL) {
		return;
	}

	if (meminfo->mem_type == MCM_TYPE_PSRAM) {
		switch (meminfo->dram_info.model) {
		case MCM_PSRAM_VENDOR_WB:
			info->Psram_Vendor = MCM_PSRAM_VENDOR_WB;
			switch (meminfo->dram_info.density) {
			case MCM_PSRAM_SIZE_32Mb:
				info->Psram_Size	   = 4 * 1024 * 1024;
				info->Psram_Page_size  = PSRAM_PAGE128;
				info->Psram_Clk_Limit  = PSRAM_DEVICE_CLK_200;
				info->Psram_Type       = PSRAM_TYPE_WB955;
				break;
			case MCM_PSRAM_SIZE_128Mb:
				info->Psram_Size	   = 16 * 1024 * 1024;
				info->Psram_Page_size  = PSRAM_PAGE2048;
				info->Psram_Clk_Limit  = PSRAM_DEVICE_CLK_250;
				info->Psram_Type       = PSRAM_TYPE_WB957;
				break;
			case MCM_PSRAM_SIZE_256Mb:
				info->Psram_Size	   = 32 * 1024 * 1024;
				info->Psram_Page_size  = PSRAM_PAGE1024;
				info->Psram_Clk_Limit  = PSRAM_DEVICE_CLK_250;
				info->Psram_Type       = PSRAM_TYPE_WB958;
				break;
			default:
				break;
			}
			break;
		case MCM_PSRAM_VENDOR_APM:
			info->Psram_Vendor = MCM_PSRAM_VENDOR_APM;
			switch (meminfo->dram_info.density) {
			case MCM_PSRAM_SIZE_64Mb:
				info->Psram_Size	   = 8 * 1024 * 1024;
				info->Psram_Page_size  = PSRAM_PAGE1024;
				info->Psram_Clk_Limit  = PSRAM_DEVICE_CLK_200;
				info->Psram_Type       = PSRAM_TYPE_APM64;
				break;
			default:
				break;
			}
			break;

		default:
			info->Psram_Vendor = MCM_PSRAM_VENDOR_NOTCARE;
			break;
		}
	} else {
		info->Psram_Vendor = MCM_PSRAM_VENDOR_NOTCARE;
	}
	rram->PSRAM_TYPE = info->Psram_Vendor;
}


/**
  * @brief get psram clk info
  * @param  none
  * @retval none
  */
void PSRAM_CLK_Update(void)
{
	MCM_MemTypeDef meminfo = ChipInfo_MCMInfo();
	ChipInfo_InitPsramInfoFromMemInfo(&meminfo, &PsramInfo);

	u32 PsramClk = 0;
	u32 PLLCLk, Div;
	u32 Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_GRP0);
	u32 ClkSel = LSYS_GET_CKSL_PSRAM(Temp);

	if (ClkSel == BIT_LSYS_CKSL_SPIC_XTAL) {

		PsramClk = XTAL_ClkGet();

	} else if (ClkSel == BIT_LSYS_CKSL_SPIC_PLL) {
		Div = LSYS_GET_PSRAMC_FLASH_DIV_INT(HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL));
		Div += 1;

		PLLCLk = PLL_ClkGet();
		PsramClk = PLLCLk / Div;

	}

	if (!PsramClk) {
		RTK_LOGE(TAG, "Invalid PsramClk\n");
		return ;
	}

	PsramInfo.Psram_Clk_Set = PsramClk / 2;
	PsramInfo.PSRAMC_Clk_Unit = 1000000000000 / PsramClk;// unit ps

	if (PsramInfo.Psram_Vendor == MCM_PSRAM_VENDOR_WB) {
		if (PsramInfo.Psram_Clk_Set <= 83000000) {
			PsramInfo.Psram_Latency_Set = 3;
			PsramInfo.Psram_TRWR = Psram_WB_TRWR133;
			PsramInfo.Psram_Latency_Code = WB_INIT_LATENCY_3CLK;
		} else if (PsramInfo.Psram_Clk_Set <= 104000000) {
			PsramInfo.Psram_Latency_Set = 4;
			PsramInfo.Psram_TRWR = Psram_WB_TRWR133;
			PsramInfo.Psram_Latency_Code = WB_INIT_LATENCY_4CLK;
		} else if (PsramInfo.Psram_Clk_Set <= 133000000) {
			PsramInfo.Psram_Latency_Set = 5;
			PsramInfo.Psram_TRWR = Psram_WB_TRWR133;
			PsramInfo.Psram_Latency_Code = WB_INIT_LATENCY_5CLK;
		} else if (PsramInfo.Psram_Clk_Set <= 200000000) {
			PsramInfo.Psram_Latency_Set = 6;
			PsramInfo.Psram_TRWR = Psram_WB_TRWR166;
			PsramInfo.Psram_Latency_Code = WB_INIT_LATENCY_6CLK;
		}

	} else if (PsramInfo.Psram_Vendor == MCM_PSRAM_VENDOR_APM) {

		if (PsramInfo.Psram_Clk_Set <= 104000000) {
			PsramInfo.Psram_Latency_Set = 4;
			PsramInfo.Psram_CSHI = Psram_APM_CSHI133;
		} else if (PsramInfo.Psram_Clk_Set <= 133000000) {
			PsramInfo.Psram_Latency_Set = 5;
			PsramInfo.Psram_CSHI = Psram_APM_CSHI133;
		} else if (PsramInfo.Psram_Clk_Set <= 166000000) {
			PsramInfo.Psram_Latency_Set = 6;
			PsramInfo.Psram_CSHI = Psram_APM_CSHI166;
		} else if (PsramInfo.Psram_Clk_Set <= 200000000) {
			PsramInfo.Psram_Latency_Set = 7;
			PsramInfo.Psram_CSHI = Psram_APM_CSHI200;
		} else if (PsramInfo.Psram_Clk_Set <= 250000000) {
			PsramInfo.Psram_Latency_Set = 8;
			PsramInfo.Psram_CSHI = Psram_APM_CSHI250;
		}
	}

	// RTK_LOGI(TAG, "PSRAM Ctrl CLK: %lu Hz \n", PsramClk);
}

/**
  * @brief  Fills each PSPHY_InitStruct member with its default value.
  * @param  PSPHY_InitStruct: pointer to an PSPHY_InitStruct structure which will be initialized.
  * @retval None
  */
void PSRAM_PHY_StructInit(PSPHY_InitTypeDef *PSPHY_InitStruct)
{
	// 0x04
	PSPHY_InitStruct->PSRAMP_PRE_CAL_PHASE = 0x0; /* only for rtl now, default choose 1 on asic */
	PSPHY_InitStruct->PSRAMP_CFG_CAL_JMIN = 0x2;
	PSPHY_InitStruct->PSRAMP_CFG_CAL_JMAX = 0x3;
	PSPHY_InitStruct->PSRAMP_CFG_CAL_J = 0x3;
	PSPHY_InitStruct->PSRAMP_CFG_CAL_N = 0xA;

	//0x18
	PSPHY_InitStruct->PSRAMP_SW_RST_N = 0x1;
	PSPHY_InitStruct->PSRAMP_LATENCY_TYPE = PSRSAM_FIX_LATENCY;	//0:variable latency 1:fix leatency
	PSPHY_InitStruct->PSRAMP_RWDS_CHK_OPT = 0x1; //1:for fpga
	PSPHY_InitStruct->PSRAMP_RWDS_PD_EN = 0x0; //0: for fpga
	PSPHY_InitStruct->PSRAMP_CAL_RWDS_PHASE = 0x1;
	if (PSPHY_InitStruct->PSRAMP_LATENCY_TYPE) {
		PSPHY_InitStruct->PSRAMP_RFIFO_RDY_DLY = PSPHY_RFIFO_READY_DELAY_FIX;
	} else {
		PSPHY_InitStruct->PSRAMP_RFIFO_RDY_DLY = PSPHY_RFIFO_READY_DELAY_VL;

	}
}

/**
  * @brief Initializes the PSRAM phy according to the specified
  *   parameters in the PSPHY_InitStruct
  * @param  PSPHY_InitStruct: pointer to a PSPHY_InitStruct structure that
  *   contains the configuration information for the specified PSRAM phy
  * @retval None
  */
void PSRAM_PHY_Init(PSPHY_InitTypeDef *PSPHY_InitStruct)
{

	PSPHY_TypeDef *psram_phy = PSRAMPHY_DEV;

	//0x04
	psram_phy->PSPHY_CAL_PAR = PSPHY_PRE_CAL_PHASE(PSPHY_InitStruct->PSRAMP_PRE_CAL_PHASE) | \
							   PSPHY_CFG_CAL_JMIN(PSPHY_InitStruct->PSRAMP_CFG_CAL_JMIN) | \
							   PSPHY_CFG_CAL_JMAX(PSPHY_InitStruct->PSRAMP_CFG_CAL_JMAX) | \
							   PSPHY_CFG_CAL_J(PSPHY_InitStruct->PSRAMP_CFG_CAL_J) | \
							   PSPHY_CFG_CAL_N(PSPHY_InitStruct->PSRAMP_CFG_CAL_N);

	//0x18
	psram_phy->PSPHY_PHY_CTRL = PSPHY_PSRAM_SW_RST_N(PSPHY_InitStruct->PSRAMP_SW_RST_N) | \
								PSPHY_RWDS_CHK_OPT(PSPHY_InitStruct->PSRAMP_RWDS_CHK_OPT) | \
								PSPHY_RWDS_PD_EN(PSPHY_InitStruct->PSRAMP_RWDS_PD_EN) | \
								PSPHY_LATENCY_TYPE(PSPHY_InitStruct->PSRAMP_LATENCY_TYPE) | \
								PSPHY_CAL_RWDS_PHASE(PSPHY_InitStruct->PSRAMP_CAL_RWDS_PHASE) | \
								PSPHY_RFIFO_RDY_DLY(PSPHY_InitStruct->PSRAMP_RFIFO_RDY_DLY);

}

void PSRAM_InfoDump(void)
{
	SPIC_TypeDef *psram_ctrl = PSRAMC_DEV;
	PSPHY_TypeDef *psram_phy = PSRAMPHY_DEV;
	RRAM_TypeDef *rram = RRAM_DEV;
	u8 psramir0[2] = {0, 0};
	u8 psramcr0[2] = {0, 0};
	u8 psramcr1[2] = {0, 0};
	u8 psram_type = rram->PSRAM_TYPE;

	RTK_LOGI(TAG, "BDnum is 0x%x\n", ChipInfo_BDNum());

	RTK_LOGI(TAG, "SSIENR\t0x%x=0x%x\n", &psram_ctrl->SSIENR, psram_ctrl->SSIENR);
	RTK_LOGI(TAG, "CTRLR0\t0x%x=0x%x\n", &psram_ctrl->CTRLR0, psram_ctrl->CTRLR0);
	RTK_LOGI(TAG, "BAUDR\t0x%x=0x%x\n", &psram_ctrl->BAUDR, psram_ctrl->BAUDR);
	RTK_LOGI(TAG, "VALID_CMD\t0x%x=0x%x\n", &psram_ctrl->VALID_CMD, psram_ctrl->VALID_CMD);
	RTK_LOGI(TAG, "CTRLR2\t0x%x=0x%x\n", &psram_ctrl->CTRLR2, psram_ctrl->CTRLR2);
	RTK_LOGI(TAG, "DEVICE_INFO\t0x%x=0x%x\n", &psram_ctrl->DEVICE_INFO, psram_ctrl->DEVICE_INFO);
	RTK_LOGI(TAG, "TPR0\t0x%x=0x%x\n", &psram_ctrl->TPR0, psram_ctrl->TPR0);
	RTK_LOGI(TAG, "READ_FAST_SINGLE\t0x%x=0x%x\n", &psram_ctrl->READ_FAST_SINGLE, psram_ctrl->READ_FAST_SINGLE);
	RTK_LOGI(TAG, "WRITE_SIGNLE\t0x%x=0x%x\n", &psram_ctrl->WRITE_SIGNLE, psram_ctrl->WRITE_SIGNLE);
	RTK_LOGI(TAG, "AUTO_LENGTH\t0x%x=0x%x\n", &psram_ctrl->AUTO_LENGTH, psram_ctrl->AUTO_LENGTH);
	RTK_LOGI(TAG, "USER_LENGTH\t0x%x=0x%x\n", &psram_ctrl->USER_LENGTH, psram_ctrl->USER_LENGTH);
	RTK_LOGI(TAG, "AUTO_LENGTH2\t0x%x=0x%x\n", &psram_ctrl->AUTO_LENGTH2, psram_ctrl->AUTO_LENGTH2);
	RTK_LOGI(TAG, "SR\t0x%x=0x%x\n", &psram_ctrl->SR, psram_ctrl->SR);

	RTK_LOGI(TAG, "PSPHY_CAL_PAR\t0x%x=0x%x\n", &psram_phy->PSPHY_CAL_PAR, psram_phy->PSPHY_CAL_PAR);
	RTK_LOGI(TAG, "PSPHY_PHY_CTRL\t0x%x=0x%x\n", &psram_phy->PSPHY_PHY_CTRL, psram_phy->PSPHY_PHY_CTRL);
	RTK_LOGI(TAG, "PSPHY_DBG_SEL\t0x%x=0x%x\n", &psram_phy->PSPHY_DBG_SEL, psram_phy->PSPHY_DBG_SEL);

	PSRAM_REG_Read(psram_type, 0x0, 2, psramir0, 0);
	RTK_LOGI(TAG, "IR0[1]=0x%x\n", psramir0[1]);
	RTK_LOGI(TAG, "IR0[0]=0x%x\n", psramir0[0]);
	PSRAM_REG_Read(psram_type, 0x0, 2, psramcr0, 1);
	RTK_LOGI(TAG, "CR0[1]=0x%x\n", psramcr0[1]);
	RTK_LOGI(TAG, "CR0[0]=0x%x\n", psramcr0[0]);
	PSRAM_REG_Read(psram_type, 0x1, 2, psramcr1, 1);
	RTK_LOGI(TAG, "CR1[1]=0x%x\n", psramcr1[1]);
	RTK_LOGI(TAG, "CR1[0]=0x%x\n", psramcr1[0]);

}


/**
  * @brief Initializes the PSRAM controller according to the specified
  *   parameters in the PSRAM_InitStruct
  * @param  PSRAM_InitStruct: pointer to a PSRAM_InitTypeDef structure that
  *   contains the configuration information for the specified PSRAM controller
  * @retval None
  */
void PSRAM_CTRL_Init(void)
{
	SPIC_TypeDef *psram_ctrl = PSRAMC_DEV;

	/* Disable psramc */
	psram_ctrl->SSIENR = 0;

	//0x00
	psram_ctrl->CTRLR0 = MASK_DDR_EN | MASK_ADDR_CH | CK_MTIMES(0x1) | MASK_DATA_CH | MASK_CMD_CH;

	//0x14
	psram_ctrl->BAUDR = SCKDV(Psram_ClkDiv2);

	//0x120
	psram_ctrl->VALID_CMD = BIT_SEQ_WR_EN | BIT_SEQ_RD_EN | BIT_FRD_SINGLE | BIT_CTRLR0_CH;

	//psram_ctrl->CTRLR2 &= ~BIT_FULL_WR;  //bit 13 = 0 need to disable,default = 0
	//0x110
	if (PsramInfo.Psram_Vendor == MCM_PSRAM_VENDOR_WB) {
		psram_ctrl->CTRLR2 = BIT_SO_DNUM | TX_FIFO_ENTRY(0x5) | RX_FIFO_ENTRY(0x5) | BIT_DM_ACT | BIT_DIS_DM_CA | \
							 WR_VL_EN(!PSRSAM_FIX_LATENCY) | RD_VL_EN(!PSRSAM_FIX_LATENCY) | RD_WEIGHT(0x2);
		//0x134 waiting update typedefine
		psram_ctrl->DEVICE_INFO = BIT_DATA_UNIT_2B | BIT_JEDEC_P2CMF | BIT_PSRAM | ATOM_SIZE(0x1) | BIT_RD_PAGE_EN | \
								  BIT_WR_PAGE_EN | PAGE_SIZE(PsramInfo.Psram_Page_size);
		psram_ctrl->TPR1 &= ~(MASK_CR_IDLE_WINDOW | MASK_CR_TPWR);
		psram_ctrl->TPR0 = (CS_TCEM(Psram_Tcem_T85 * 1000 / PsramInfo.PSRAMC_Clk_Unit / 32)) | \
						   (CS_SEQ_TIMEOUT(Psram_Seq_timeout)) | \
						   (CS_ACTIVE_HOLD(Psram_WB_CSH)) | \
						   (CS_H_WR_DUM_LEN(((PsramInfo.Psram_TRWR * 1000 + PsramInfo.PSRAMC_Clk_Unit - 1) / PsramInfo.PSRAMC_Clk_Unit - 2 - 3 - 1))) | \
						   (CS_H_RD_DUM_LEN(((PsramInfo.Psram_TRWR * 1000 + PsramInfo.PSRAMC_Clk_Unit - 1) / PsramInfo.PSRAMC_Clk_Unit - 2 - 3 - 1)));
		//0xe0
		psram_ctrl->READ_FAST_SINGLE = FRD_CMD(WB_RD_CMD);
		//0xf4
		psram_ctrl->WRITE_SIGNLE = WR_CMD(WB_WR_CMD);
		//0x11c
		psram_ctrl->AUTO_LENGTH = AUTO_ADDR_LENGTH(0x5);
		//0x118
		psram_ctrl->USER_LENGTH = USER_ADDR_LENGTH(0x5) | USER_CMD_LENGHT(0x1);
	} else {

		//0x110
		psram_ctrl->CTRLR2 = BIT_SO_DNUM | TX_FIFO_ENTRY(0x5) | RX_FIFO_ENTRY(0x5) | BIT_DM_ACT | \
							 WR_VL_EN(!PSRSAM_FIX_LATENCY) | RD_VL_EN(!PSRSAM_FIX_LATENCY) | RD_WEIGHT(0x2);
		//0x134 waiting update typedefine
		psram_ctrl->DEVICE_INFO = BIT_PSRAM | ATOM_SIZE(0x1) | BIT_RD_PAGE_EN | \
								  BIT_WR_PAGE_EN | PAGE_SIZE(PsramInfo.Psram_Page_size);
		//0x138 waiting update typedefine
		//CS_TCEM = 0 for seq performance
		//todo
		psram_ctrl->TPR0 = (CS_TCEM(Psram_Tcem_T85 * 1000 / PsramInfo.PSRAMC_Clk_Unit / 32)) | \
						   (CS_SEQ_TIMEOUT(Psram_Seq_timeout)) | \
						   (CS_ACTIVE_HOLD(Psram_APM_CSH)) | \
						   (CS_H_WR_DUM_LEN(PsramInfo.Psram_CSHI * 1000 / PsramInfo.PSRAMC_Clk_Unit)) | \
						   (CS_H_RD_DUM_LEN(PsramInfo.Psram_CSHI * 1000 / PsramInfo.PSRAMC_Clk_Unit));
		psram_ctrl->READ_FAST_SINGLE = FRD_CMD(APM_RD_CMD);
		psram_ctrl->WRITE_SIGNLE = WR_CMD(APM_WR_CMD);
		//0x11c
		psram_ctrl->AUTO_LENGTH = AUTO_ADDR_LENGTH(0x4);
		//0x118
		psram_ctrl->USER_LENGTH = USER_ADDR_LENGTH(0x4) | USER_CMD_LENGHT(0x2);
	}


	if (PSRSAM_FIX_LATENCY) {
		//auto rd latency
		psram_ctrl->AUTO_LENGTH |= RD_DUMMY_LENGTH(2 * 2 * PsramInfo.Psram_Latency_Set - 1) | \
								   IN_PHYSICAL_CYC(PSPHY_RFIFO_READY_DELAY_FIX + 2);
		//auto wr latency
		psram_ctrl->AUTO_LENGTH2 = WR_DUMMY_LENGTH(2 * 2 * PsramInfo.Psram_Latency_Set - 2);

		psram_ctrl->USER_LENGTH |= USER_RD_DUMMY_LENGTH(2 * 2 * PsramInfo.Psram_Latency_Set - 1);
	} else {
		//auto rd latency
		psram_ctrl->AUTO_LENGTH |= RD_DUMMY_LENGTH(2 * PsramInfo.Psram_Latency_Set - 1) ;
		//auto wr latency
		psram_ctrl->AUTO_LENGTH2 = WR_DUMMY_LENGTH(2 * PsramInfo.Psram_Latency_Set - 2);
		psram_ctrl->USER_LENGTH |= USER_RD_DUMMY_LENGTH(2 * PsramInfo.Psram_Latency_Set - 1);
	}
}



void PSRAM_APM_DEVIC_Init(void)
{
	u8 mr0[2];
	u8 mr4[2];
	mr0[0] = PSRAM_READ_LATENCY_CODE(PsramInfo.Psram_Latency_Set - 3) | \
			 PSRAM_LT_SELECT(PSRSAM_FIX_LATENCY) | \
			 PSRAM_Drive_Strength(APM_Drive_Strength_100ohms); //0:variable latency 1:fix latency

	mr4[0] = PSRAM_WRITE_LATENCY_CODE(APM_WR_INIT_LATENCY_SPEC[PsramInfo.Psram_Latency_Set - 3]);

	mr0[1] = mr0[0];
	mr4[1] = mr4[0];

	PSRAM_REG_Write(MCM_PSRAM_VENDOR_APM, 0x0, 2, mr0);
	PSRAM_REG_Write(MCM_PSRAM_VENDOR_APM, 0x4, 2, mr4);
}

void PSRAM_WB_DEVIC_Init(void)
{
	u8 data[2];
	data[0] = PSRAM_WB_RESERVER | \
			  PSRAM_WB_DRIVE_STRENGTH(WB_Drive_Strength_100ohms) | \
			  PSRAM_WB_DPD_EN(WB_NORMAL_OPE);

	data[1] = PSRAM_WB_BURST_LENGTH(WB_BURST_LEN_128B) | \
			  PSRAM_WB_HyBURSE_EN | \
			  PSRAM_WB_FIX_LATENCY_EN(PSRSAM_FIX_LATENCY) | \
			  PSRAM_WB_INIT_LATENCY(PsramInfo.Psram_Latency_Code);

	PSRAM_REG_Write(MCM_PSRAM_VENDOR_WB, PSRAM_WB_CR0, 2, data);
}


/**
  * @brief Get APM/Winbond PSRAM register value
  * @param  type: APM :0 Winbond:1
  * @param  addr: APM/Winbond PSRAM register addr
  * @param  read_len: read data length
  * @param  read_data: read data output buf
  * @retval None
  */
NON_DRAM_TEXT_SECTION
void PSRAM_REG_Read(u8 type, u32 addr, u32 read_len, u8 *read_data, u32 CR)
{
	SPIC_TypeDef *psram_ctrl = PSRAMC_DEV;
	u32 rx_num = 0;

	/* Wait spic busy done before switch to user mode */
	while (psram_ctrl->SR & BIT_BUSY);

	/* Enter User Mode */
	psram_ctrl->CTRLR0 |= BIT_USER_MODE;

	/* Disable SSIENR to program control registers*/
	psram_ctrl->SSIENR = 0;

	/* Set RX_NDF: frame number of receiving data. TX_NDF should be set in both transmit mode and receive mode.
		TX_NDF should be set to zero in receive mode to skip the TX_DATA phase. */
	psram_ctrl->RX_NDF = RX_NDF(read_len);
	psram_ctrl->TX_NDF = 0;
	//RTK_LOGD(TAG, "RX_NDF = %p %lx\n", &(psram_ctrl->RX_NDF), psram_ctrl->RX_NDF);

	//set rx mode
	psram_ctrl->CTRLR0 |= TMOD(3);
	//RTK_LOGD(TAG, "CTRLR0 = %p %lx\n", &(psram_ctrl->CTRLR0), psram_ctrl->CTRLR0);

	/* set flash_cmd: write cmd to fifo */
	if (type == MCM_PSRAM_VENDOR_APM) {
		psram_ctrl->DR[0].BYTE = 0x40;
		psram_ctrl->DR[0].BYTE = 0x40;
	} else {
		psram_ctrl->DR[0].BYTE = 0xe0;
		psram_ctrl->DR[0].BYTE = 0x00;
	}


	if (CR) {
		psram_ctrl->DR[0].BYTE = 0x01;
	} else {
		psram_ctrl->DR[0].BYTE = 0x00;    // IR reg read only
	}

	psram_ctrl->DR[0].BYTE = 0x00;
	psram_ctrl->DR[0].BYTE = 0x00;

	psram_ctrl->DR[0].BYTE = (u8)(addr & 0xFF);//addr


	/* Enable SPI_FLASH  User Mode */
	psram_ctrl->SSIENR = BIT_SPIC_EN;
	while (psram_ctrl->SSIENR & BIT_SPIC_EN) {
	};

	/* read data */
	rx_num = 0;

	while (rx_num < read_len) {
		read_data[rx_num] = psram_ctrl->DR[0].BYTE;
		rx_num += 1;
	}

	/* Wait transfer complete. When complete, SSIENR[0] and CTRLR0[31] is cleared by HW automatically. */
	while (psram_ctrl->SR & BIT_BUSY);

	/* Restore to auto mode */
	psram_ctrl->CTRLR0 &= ~BIT_USER_MODE;

}

/**
  * @brief set APM/Winbond PSRAM register value
  * @param  type: APM :0 Winbond:1
  * @param  addr: PSRAM register addr
  * @param  write_len: write data length
  * @param  write_data: write data buf
  * @retval None
  */
NON_DRAM_TEXT_SECTION
void PSRAM_REG_Write(u8 type, u32 addr, u32 write_len, u8 *write_data)
{
	SPIC_TypeDef *psram_ctrl = PSRAMC_DEV;
	u32 tx_num = 0;

	/* Wait spic busy done before switch to user mode */
	while (psram_ctrl->SR & BIT_BUSY);

	/* Enter User Mode */
	psram_ctrl->CTRLR0 |= BIT_USER_MODE;

	/* Disable SSIENR to program control registers*/
	psram_ctrl->SSIENR = 0;

	//set tx mode
	psram_ctrl->CTRLR0 &= ~(TMOD(3));

	/* Set RX_NDF: frame number of receiving data. TX_NDF should be set in both transmit mode and receive mode.
		TX_NDF should be set to zero in receive mode to skip the TX_DATA phase. */
	psram_ctrl->RX_NDF = 0;
	psram_ctrl->TX_NDF = TX_NDF(write_len);

	/* set flash_cmd: write cmd to fifo */
	if (type == MCM_PSRAM_VENDOR_APM) {

		psram_ctrl->DR[0].BYTE = 0xc0;
		psram_ctrl->DR[0].BYTE = 0xc0;
	} else {

		psram_ctrl->DR[0].BYTE = 0x60;
		psram_ctrl->DR[0].BYTE = 0x00;
	}

	psram_ctrl->DR[0].BYTE = 0x01;
	psram_ctrl->DR[0].BYTE = 0x00;
	psram_ctrl->DR[0].BYTE = 0x00;

	psram_ctrl->DR[0].BYTE = (u8)(addr & 0xFF);//addr

	/* fill data */
	for (tx_num = 0; tx_num < write_len; tx_num++) {
		psram_ctrl->DR[0].BYTE = (u8) * (write_data + tx_num);
	}

	/* Enable SPI_FLASH  User Mode */
	psram_ctrl->SSIENR = BIT_SPIC_EN;
	while (psram_ctrl->SSIENR & BIT_SPIC_EN) {
	};

	/* Wait transfer complete. When complete, SSIENR[0] and CTRLR0[31] is cleared by HW automatically. */
	while (psram_ctrl->SR & BIT_BUSY);

	/* Restore to auto mode */
	psram_ctrl->CTRLR0 &= ~BIT_USER_MODE;

}

/**
  * @brief set APM PSRAM register value
  * @param  cmd: APM PSRAM write mem cmd: 0x80 or 0xa0
  * @param  addr: APM PSRAM register addr
  * @param  write_len: write data length
  * @param  write_data: write data buf
  * @retval None
  */

void PSRAM_MEM_Write(u8 cmd, u32 addr, u32 write_len, u8 *write_data)
{
	SPIC_TypeDef *psram_ctrl = PSRAMC_DEV;
	u32 tx_num = 0;
	u32 txlen = write_len + (PsramInfo.Psram_Latency_Set - 1) * 2;

	/* Wait spic busy done before switch to user mode */
	while (psram_ctrl->SR & BIT_BUSY);

	/* Enter User Mode */
	psram_ctrl->CTRLR0 |= BIT_USER_MODE;

	/* Disable SSIENR to program control registers*/
	psram_ctrl->SSIENR = 0;

	//set tx mode
	psram_ctrl->CTRLR0 &= ~(TMOD(3));
	RTK_LOGI(TAG, "CTRLR0 = %p %lx\n", &(psram_ctrl->CTRLR0), psram_ctrl->CTRLR0);

	/* Set RX_NDF: frame number of receiving data. TX_NDF should be set in both transmit mode and receive mode.
		TX_NDF should be set to zero in receive mode to skip the TX_DATA phase. */
	psram_ctrl->RX_NDF = 0;
	psram_ctrl->TX_NDF = TX_NDF(txlen);

	addr = addr - PSRAM_BASE;

	/* set flash_cmd: write cmd to fifo */
	psram_ctrl->DR[0].BYTE = cmd;
	psram_ctrl->DR[0].BYTE = cmd;
	psram_ctrl->DR[0].BYTE = (u8)((addr & 0xFF000000) >> 24);
	psram_ctrl->DR[0].BYTE = (u8)((addr & 0xFF0000) >> 16);
	psram_ctrl->DR[0].BYTE = (u8)((addr & 0xFF00) >> 8);
	psram_ctrl->DR[0].BYTE = (u8)(addr & 0xFF);

	/* fill data */
	for (tx_num = 0; tx_num < (PsramInfo.Psram_Latency_Set - 1) * 2; tx_num++) {
		psram_ctrl->DR[0].BYTE = 0x0;
	}

	/* fill data */
	for (tx_num = 0; tx_num < write_len; tx_num++) {
		psram_ctrl->DR[0].BYTE = (u8) * (write_data + tx_num);
	}

	/* Enable SPI_FLASH  User Mode */
	psram_ctrl->SSIENR = BIT_SPIC_EN;

	while (psram_ctrl->SSIENR & BIT_SPIC_EN) {
	};

	/* Wait transfer complete. When complete, SSIENR[0] and CTRLR0[31] is cleared by HW automatically. */
	while (psram_ctrl->SR & BIT_BUSY);

	/* Restore to auto mode */
	psram_ctrl->CTRLR0 &= ~BIT_USER_MODE;

}

void PSRAM_calwindow(int window_start, int window_end, PCAL_InitTypeDef *PCAL_InitStruct)
{
	u32 suggest_phase = 0;
	int window_size = 0;
	int windowt_size, windowt_start, windowt_end;
	int JMAX_val = 0;
	int J_val = 0;
	int N_val = 0;
	int i, head, tail;
	u32 phase;
	for (i = 0; i < 4; i++) {
		head = i * 16;
		tail = head + 31;
		phase = BIT(i);
		windowt_start = head > window_start ? head : window_start;
		windowt_end = tail < window_end ? tail : window_end;
		windowt_size = windowt_end - windowt_start + 1;
		if (windowt_size > window_size) {

			window_size = windowt_size;
			N_val = (windowt_start + windowt_end - head - head) / 2;
			JMAX_val = (windowt_end - windowt_start) / 2 - 2;
			J_val = (windowt_end - windowt_start) / 2 - 2;
			suggest_phase = phase;
		}
	}

	if (window_size > PCAL_InitStruct->window_size) {
		PCAL_InitStruct->window_size = window_size;
		PCAL_InitStruct->CAL_JMAX = JMAX_val;
		PCAL_InitStruct->CAL_J = J_val;
		PCAL_InitStruct->CAL_N = N_val;
		PCAL_InitStruct->phase = suggest_phase;
	}

}

/**
  * @brief PSRAM FW calibration function
  * @retval None
  * @note cache will be disable during calibration
  */
bool PSRAM_calibration(void)
{
	PSPHY_TypeDef *psram_phy = PSRAMPHY_DEV;

	u32 caltempN = 0;
	u32 phase = 0;
	u32 tempPHYPara = psram_phy->PSPHY_CAL_PAR & (~PSPHY_MASK_CFG_CAL_N) & (~PSPHY_MASK_PRE_CAL_PHASE);
	const u32 *tempdatawr = PSRAM_CALIB_PATTERN;
	u32 tempdatard[6] = {0, 0, 0, 0, 0, 0};
	int window_start = -1;
	int window_end = -1;
	int window_size = 0;

	int windowt_start = -1;
	int windowt_size = 0;
	int windowt_end = -1;

	int phase_cnt = -1;

	/*Disable HW calibration*/
	psram_phy->PSPHY_CAL_CTRL &= (~PSPHY_BIT_CFG_CAL_EN);
	for (phase = 0x0; phase <= 0x8; phase = (phase == 0 ? phase + 1 : phase * 2)) {
		windowt_size = 0;
		windowt_start = -1;
		windowt_end = -1;
		RTK_LOGD(TAG, "===phase %lx =====\n", phase);

		for (caltempN = 0; caltempN < 32; caltempN++) {
			psram_phy->PSPHY_CAL_PAR = tempPHYPara | caltempN | PSPHY_PRE_CAL_PHASE(phase);
			/*PG needs to be recalibrated when the temperature changes sharply.
			To avoid accessing the secure address, the calibration address starts from __km4_bd_psram_start__.*/
			for (int i = 0; i < 6; i += 1) {
				HAL_WRITE32((u32)__km4_bd_psram_start__, i * 0x50000, tempdatawr[i]);
				DCache_CleanInvalidate((u32)__km4_bd_psram_start__ + i * 0x50000, CACHE_LINE_SIZE);
				tempdatard[i] = HAL_READ32((u32)__km4_bd_psram_start__, i * 0x50000);
			}

			if (_memcmp(tempdatard, PSRAM_CALIB_PATTERN, 24) == 0) {
				RTK_LOGD(TAG, "ok %lx %lx %lx %lx %lx %lx %lx\n", caltempN, tempdatard[0], tempdatard[1], tempdatard[2], tempdatard[3], tempdatard[4], tempdatard[5]);

				if (windowt_start < 0) {
					windowt_start = caltempN;
				}
				windowt_end = windowt_start + windowt_size;
				windowt_size ++;

				if (caltempN == (32 - 1)) {
					if (windowt_size > window_size) {
						window_start = windowt_start;
						window_end = windowt_end;
						window_size = windowt_size;
						phase_cnt = phase;
					}
				}
			} else {
				RTK_LOGD(TAG, "fail %lx %lx %lx %lx %lx %lx %lx\n", caltempN, tempdatard[0], tempdatard[1], tempdatard[2], tempdatard[3], tempdatard[4], tempdatard[5]);

				if (windowt_start >= 0) {
					if (windowt_size > window_size) {
						window_start = windowt_start;
						window_end = windowt_end;
						window_size = windowt_size;
						phase_cnt = phase;

					}
					windowt_start = -1;
					windowt_size = 0;
					windowt_end = -1;
				}
			}
		}
	}


	RTK_LOGI(TAG, "Cal win size %ld\n", window_size);

	if ((window_size) < 9) {
		return FALSE;
	}
	tempPHYPara &= (~0xfffff);
	tempPHYPara |= PSPHY_CFG_CAL_JMAX((window_end - window_start) / 2 - 2) | \
				   PSPHY_CFG_CAL_J((window_end - window_start) / 2 - 2)	| \
				   PSPHY_CFG_CAL_N((window_end + window_start) / 2) | \
				   PSPHY_PRE_CAL_PHASE(phase_cnt);

	psram_phy->PSPHY_CAL_PAR = tempPHYPara;

	/*start HW calibration*/
	psram_phy->PSPHY_CAL_CTRL |= PSPHY_BIT_CFG_CAL_EN;

	return TRUE;

}


/**
  * @brief  set psram into half sleep mode.
  * @param  type: 0:apm psram type 1: winbond psram type
  * @retval None
  * @note None
  */
NON_DRAM_TEXT_SECTION
void set_psram_sleep_mode(void)
{
	u8 psram_halfsleep[2];
	u8 mr4[2];

	RRAM_TypeDef *rram = RRAM_DEV;
	PSPHY_TypeDef *psram_phy = PSRAMPHY_DEV;
	psram_phy->PSPHY_CAL_CTRL = PSRAM_MASK_CAL_ITT;

	rram->psram_backup[0] = HAL_READ32((u32)__km4_bd_psram_start__, 0x0);
	rram->psram_backup[1] = HAL_READ32((u32)__km4_bd_psram_start__, 0x50000);
	rram->psram_backup[2] = HAL_READ32((u32)__km4_bd_psram_start__, 0x100000);
	rram->psram_backup[3] = HAL_READ32((u32)__km4_bd_psram_start__, 0x150000);
	rram->psram_backup[4] = HAL_READ32((u32)__km4_bd_psram_start__, 0x200000);
	rram->psram_backup[5] = HAL_READ32((u32)__km4_bd_psram_start__, 0x250000);

	if (rram->PSRAM_TYPE == MCM_PSRAM_VENDOR_APM) {
		psram_halfsleep[0] = 0xF0;
		psram_halfsleep[1] = 0xF0;

		/*enable apm slow refresh*/
		PSRAM_REG_Read(MCM_PSRAM_VENDOR_APM, 0x4, 2, mr4, 1);
		mr4[0] |= PSRAM_ENABLE_SLOW_REFRESH;
		mr4[1] = mr4[0];
		PSRAM_REG_Write(MCM_PSRAM_VENDOR_APM, 0x4, 2, mr4);
		PSRAM_REG_Write(MCM_PSRAM_VENDOR_APM, 0x06, 2, psram_halfsleep);

	} else {
		psram_halfsleep[0] = 0xFF;
		psram_halfsleep[1] = 0x20;

		PSRAM_REG_Write(MCM_PSRAM_VENDOR_WB, 0x1, 2, psram_halfsleep);
	}

}

/**
  * @brief  Wake PSRAM from half sleep mode.
  * @param  type: 0:apm psram type 1: winbond psram type
  * @retval None
  * @note None
  */
_OPTIMIZE_NONE_
NON_DRAM_TEXT_SECTION
void set_psram_wakeup_mode(void)
{
	u32 temp, rdata;
	u8 mr4[2];
	u8 psram_type;

	RRAM_TypeDef *rram = RRAM_DEV;
	PSPHY_TypeDef *psram_phy = PSRAMPHY_DEV;
	SPIC_TypeDef *psram_ctrl = PSRAMC_DEV;
	psram_type = rram->PSRAM_TYPE;

	/*1. auto read , trigger psram to exit sleep mode*/
	DCache_Invalidate((u32)__km4_bd_psram_start__, 32);
	rdata = HAL_READ32((u32)__km4_bd_psram_start__, 0);

	/* 2. when PG wakeup, fisrt read will cause phy hang, set PSPHY_PHY_INFO to return work correctly*/
	temp = psram_phy->PSPHY_PHY_INFO;
	psram_phy->PSPHY_PHY_INFO = temp;

	/*3. wait for Txhs until psram exit sleep mode*/
	if (psram_type == MCM_PSRAM_VENDOR_APM) {
		DelayUs(150);
	} else if (psram_type == MCM_PSRAM_VENDOR_WB) {
		DelayUs(100);
	}

	/* 4. read again. */
	DCache_Invalidate((u32)__km4_bd_psram_start__, 32);
	rdata = HAL_READ32((u32)__km4_bd_psram_start__, 0);

	/* 5. Check if the window has changed. */
	if (PSRAM_CHECK_INT(psram_phy->PSPHY_CAL_CTRL)) {
		rdata = psram_phy->PSPHY_CAL_CTRL;
		psram_phy->PSPHY_CAL_CTRL = rdata;
		PSRAM_calibration();
		HAL_WRITE32((u32)__km4_bd_psram_start__, 0x0, rram->psram_backup[0]);
		HAL_WRITE32((u32)__km4_bd_psram_start__, 0x50000, rram->psram_backup[1]);
		HAL_WRITE32((u32)__km4_bd_psram_start__, 0x100000, rram->psram_backup[2]);
		HAL_WRITE32((u32)__km4_bd_psram_start__, 0x150000, rram->psram_backup[3]);
		HAL_WRITE32((u32)__km4_bd_psram_start__, 0x200000, rram->psram_backup[4]);
		HAL_WRITE32((u32)__km4_bd_psram_start__, 0x250000, rram->psram_backup[5]);
	}

	/*6. backfill the value of the register*/
	psram_ctrl->TPR1 &= ~(MASK_CR_IDLE_WINDOW | MASK_CR_TPWR);

	/*7. disable apm slow refresh*/
	if (psram_type == MCM_PSRAM_VENDOR_APM) {
		PSRAM_REG_Read(MCM_PSRAM_VENDOR_APM, 0x4, 2, mr4, 1);
		mr4[0] &= ~PSRAM_ENABLE_SLOW_REFRESH;
		mr4[1] = mr4[0];
		PSRAM_REG_Write(MCM_PSRAM_VENDOR_APM, 0x4, 2, mr4);
	}
}