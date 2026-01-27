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

u8 WB_WR_INIT_LATENCY_SPEC[8] = {
	WB_WR_INIT_LATENCY_3CLK,
	WB_WR_INIT_LATENCY_4CLK,
	WB_WR_INIT_LATENCY_5CLK,
	WB_WR_INIT_LATENCY_6CLK,
	WB_WR_INIT_LATENCY_7CLK,
	0, //NULL
	0, //NULL
	WB_WR_INIT_LATENCY_10CLK
};

u8 WB_RD_CMD_TYPE[3] = {
	WB_RD_CMD_WRAPPED,
	WB_RD_CMD_WRAPPED,
	WB_RD_CMD_LINEAR
};
u8 WB_WR_CMD_TYPE[3] = {
	WB_WR_CMD_WRAPPED,
	WB_WR_CMD_WRAPPED,
	WB_WR_CMD_LINEAR
};
static u32 PSRAM_CALIB_PATTERN[6] = {
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
		info->Psram_DQ16 = meminfo->dram_info.dqx;

		switch (meminfo->dram_info.model) {
		case MCM_PSRAM_VENDOR_WB:
			info->Psram_Vendor = MCM_PSRAM_VENDOR_WB;
			switch (meminfo->dram_info.density) {
			case MCM_PSRAM_SIZE_32Mb:
				info->Psram_Size	   = 4 * 1024 * 1024;
				info->Psram_Page_size  = PSRAM_PAGE128;
				info->Psram_Clk_Limit  = PSRAM_DEVICE_CLK_200;
				info->Psram_Type       = PSRAM_TYPE_WB955;
				info->Psram_PDEX_CMD0 = PSRAM_WB955_PDEX_CMD0;
				info->Psram_PDX_TIME = Psram_WB955_TEXTHS;
				info->Psram_PDE_TIME =  Psram_WB955_THSIN;
				break;
			case MCM_PSRAM_SIZE_128Mb:
				info->Psram_Size	   = 16 * 1024 * 1024;
				info->Psram_Page_size  = PSRAM_PAGE2048;
				info->Psram_Clk_Limit  = PSRAM_DEVICE_CLK_250;
				info->Psram_Type       = PSRAM_TYPE_WB957;
				info->Psram_PDEX_CMD0 = PSRAM_WB_PDEX_CMD0;
				info->Psram_PDX_TIME = Psram_WB957_TEXTHS;
				info->Psram_PDE_TIME = Psram_WB957_THSIN;
				break;
			case MCM_PSRAM_SIZE_256Mb:
				info->Psram_Size	   = 32 * 1024 * 1024;
				info->Psram_Page_size  = PSRAM_PAGE1024;
				info->Psram_Clk_Limit  = PSRAM_DEVICE_CLK_250;
				info->Psram_Type       = PSRAM_TYPE_WB958;
				info->Psram_PDEX_CMD0 = PSRAM_WB_PDEX_CMD0;
				info->Psram_PDX_TIME = Psram_WB958_TEXTHS;
				info->Psram_PDE_TIME = Psram_WB958_THSIN;
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
			case MCM_PSRAM_SIZE_128Mb:
				info->Psram_Size	   = 16 * 1024 * 1024;
				info->Psram_Page_size  = PSRAM_PAGE2048;
				info->Psram_Clk_Limit  = PSRAM_DEVICE_CLK_200;
				info->Psram_Type       = PSRAM_TYPE_APM128;
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
	rram->PSRAM_DQ = info->Psram_DQ16;
	RTK_LOGD(TAG, "rram->PSRAM_TYPE  = 0x%x\n", rram->PSRAM_TYPE);
}

/**
  * @brief get psram clk info
  * @param  none
  * @retval none
  */
void PSRAM_CLK_Update(void)
{
	u32 PsramClk = 0;

	MCM_MemTypeDef meminfo = ChipInfo_MCMInfo();
	ChipInfo_InitPsramInfoFromMemInfo(&meminfo, &PsramInfo);

	if (PsramInfo.Psram_Vendor == MCM_PSRAM_VENDOR_NOTCARE) {
		RTK_LOGE(TAG, "Invalid Psram\n");
		return;
	}

	u32 Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_GRP0);
	u32 ClkSel = LSYS_GET_CKSL_PSRAM(Temp);

	if (SYSCFG_CHIPType_Get() ==  CHIP_TYPE_FPGA) {
		PsramClk = 80000000;
	} else {

		if (ClkSel == CKSL_PSRAM_XTAL) {

			PsramClk = XTAL_ClkGet();

		} else {
			PsramClk = PSRAMC_ClkGet();
		}
	}

	PsramInfo.Psram_Clk_Set = PsramClk / 2;

	if (!PsramClk) {
		RTK_LOGE(TAG, "Invalid PsramClk\n");
		return ;
	}

	PsramInfo.PSRAMC_Clk_Unit = 1000000000000 / PsramClk;//unit ps
	PsramInfo.Psram_Resume_Cnt = Psram_RESUME_TIME * 1000 / (PsramInfo.PSRAMC_Clk_Unit);

	if (PsramInfo.Psram_Vendor == MCM_PSRAM_VENDOR_WB) {
		PsramInfo.Psram_PDEX_CMD1 = PSRAM_WB_PDEX_CMD1;

		if (PsramInfo.Psram_Clk_Set <= 85000000) {
			PsramInfo.Psram_Latency_Set = 4;
			PsramInfo.Psram_TRWR = Psram_WB_TRWR133;
		} else if (PsramInfo.Psram_Clk_Set <= 104000000) {
			PsramInfo.Psram_Latency_Set = 4;
			PsramInfo.Psram_TRWR = Psram_WB_TRWR133;
		} else if (PsramInfo.Psram_Clk_Set <= 133000000) {
			PsramInfo.Psram_Latency_Set = 5;
			PsramInfo.Psram_TRWR = Psram_WB_TRWR133;
		} else if (PsramInfo.Psram_Clk_Set <= 166000000) {
			PsramInfo.Psram_Latency_Set = 6;
			PsramInfo.Psram_TRWR = Psram_WB_TRWR166;
		} else if (PsramInfo.Psram_Clk_Set <= 200000000) {
			if (PsramInfo.Psram_Type == PSRAM_TYPE_WB955) {
				PsramInfo.Psram_Latency_Set = 6;
				PsramInfo.Psram_TRWR = Psram_WB_TRWR166;
			} else {
				PsramInfo.Psram_Latency_Set = 7;
				PsramInfo.Psram_TRWR = Psram_WB_TRWR200;
			}
		} else if ((PsramInfo.Psram_Clk_Set <= 250000000) && (PsramInfo.Psram_Size == PSRAM_SIZE_256Mb)) {
			PsramInfo.Psram_Latency_Set = 7;
			PsramInfo.Psram_TRWR = Psram_WB_TRWR250;
		} else if ((PsramInfo.Psram_Clk_Set <= 250000000) && (PsramInfo.Psram_Size == PSRAM_SIZE_128Mb)) {
			PsramInfo.Psram_Latency_Set = 10;
			PsramInfo.Psram_TRWR = Psram_WB_TRWR250;
		}

	} else {
		PsramInfo.Psram_PDEX_CMD1 = PSRAM_APM_PDEX_CMD1;
		PsramInfo.Psram_PDEX_CMD0 = PSRAM_APM_PDEX_CMD0;
		PsramInfo.Psram_PDX_TIME = Psram_APM_TEXTHS;
		PsramInfo.Psram_PDE_TIME = Psram_APM_THSIN;

		if (PsramInfo.Psram_Clk_Set <= 66000000) {
			PsramInfo.Psram_Latency_Set = 3;
			PsramInfo.Psram_CSHI = Psram_APM_CSHI133;
		} else if (PsramInfo.Psram_Clk_Set <= 109000000) {
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

	RTK_LOGI(TAG, "PSRAM CLK: %ldMHz, DQ%ld, Size: %ldMB\n", \
			 PsramClk / 1000000, \
			 PsramInfo.Psram_DQ16 == MCM_PSRAM_DQ16 ? 16 : 8, \
			 PsramInfo.Psram_Size / 1024 / 1024);
}


/**
  * @brief  Fills each PSPHY_InitStruct member with its default value.
  * @param  PSPHY_InitStruct: pointer to an PSPHY_InitStruct structure which will be initialized.
  * @retval None
  */
void PSRAM_PHY_StructInit(PSPHY_InitTypeDef *PSPHY_InitStruct)
{
	// 0x04
	PSPHY_InitStruct->PSRAMP_PRE_CAL_PHASE =
		0x1; /* only for postsim, the signal rwds generated by the PSRAM SIM model will be 400ps faster than dq [7:0] (data cannot be collected), and this will be compensated back by adding one level of pre_cal */
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
	if (PsramInfo.Psram_DQ16 == MCM_PSRAM_DQ16) {
		PSPHY_InitStruct->PSRAMP_DQ16_EN = 0x1;
	} else {
		PSPHY_InitStruct->PSRAMP_DQ16_EN = 0x0;
	}

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

	//0x24
	psram_phy->PSPHY_SL1_CAL_PAR = PSPHY_PRE_CAL_PHASE(PSPHY_InitStruct->PSRAMP_PRE_CAL_PHASE) | \
								   PSPHY_CFG_CAL_JMIN(PSPHY_InitStruct->PSRAMP_CFG_CAL_JMIN) | \
								   PSPHY_CFG_CAL_JMAX(PSPHY_InitStruct->PSRAMP_CFG_CAL_JMAX) | \
								   PSPHY_CFG_CAL_J(PSPHY_InitStruct->PSRAMP_CFG_CAL_J) | \
								   PSPHY_CFG_CAL_N(PSPHY_InitStruct->PSRAMP_CFG_CAL_N);

	//0x18
	psram_phy->PSPHY_PHY_CTRL = PSPHY_PSRAM_SW_RST_N(PSPHY_InitStruct->PSRAMP_SW_RST_N) | \
								PSPHY_RWDS_CHK_OPT(PSPHY_InitStruct->PSRAMP_RWDS_CHK_OPT) | \
								PSPHY_RWDS_PD_EN(PSPHY_InitStruct->PSRAMP_RWDS_PD_EN) | \
								PSPHY_PSRAM_DQ16_EN(PSPHY_InitStruct->PSRAMP_DQ16_EN) | \
								PSPHY_LATENCY_TYPE(PSPHY_InitStruct->PSRAMP_LATENCY_TYPE) | \
								PSPHY_CAL_RWDS_PHASE(PSPHY_InitStruct->PSRAMP_CAL_RWDS_PHASE) | \
								PSPHY_RFIFO_RDY_DLY(PSPHY_InitStruct->PSRAMP_RFIFO_RDY_DLY);

}


void PSRAM_InfoDump(void)
{
	SPIC_TypeDef *psram_ctrl = PSRAMC_DEV;
	PSPHY_TypeDef *psram_phy = PSRAMPHY_DEV;

//	RTK_LOGI(TAG, "BDnum is 0x%x\n", ChipInfo_BDNum());

	RTK_LOGI(TAG, "SSIENR\t0x%x=0x%x\n", &psram_ctrl->SSIENR, psram_ctrl->SSIENR);
	RTK_LOGI(TAG, "CTRLR0\t0x%x=0x%x\n", &psram_ctrl->CTRLR0, psram_ctrl->CTRLR0);
	RTK_LOGI(TAG, "BAUDR\t0x%x=0x%x\n", &psram_ctrl->BAUDR, psram_ctrl->BAUDR);
	RTK_LOGI(TAG, "FBAUDR\t0x%x=0x%x\n", &psram_ctrl->FBAUDR, psram_ctrl->FBAUDR);
	RTK_LOGI(TAG, "VALID_CMD\t0x%x=0x%x\n", &psram_ctrl->VALID_CMD, psram_ctrl->VALID_CMD);
	RTK_LOGI(TAG, "CTRLR2\t0x%x=0x%x\n", &psram_ctrl->CTRLR2, psram_ctrl->CTRLR2);
	RTK_LOGI(TAG, "DEVICE_INFO\t0x%x=0x%x\n", &psram_ctrl->DEVICE_INFO, psram_ctrl->DEVICE_INFO);
	RTK_LOGI(TAG, "TPR0\t0x%x=0x%x\n", &psram_ctrl->TPR0, psram_ctrl->TPR0);
	RTK_LOGI(TAG, "TPR1\t0x%x=0x%x\n", &psram_ctrl->TPR1, psram_ctrl->TPR1);
	RTK_LOGI(TAG, "TPR2\t0x%x=0x%x\n", &psram_ctrl->TPR2, psram_ctrl->TPR2);

	RTK_LOGI(TAG, "READ_FAST_SINGLE\t0x%x=0x%x\n", &psram_ctrl->READ_FAST_SINGLE, psram_ctrl->READ_FAST_SINGLE);
	RTK_LOGI(TAG, "WRITE_SIGNLE\t0x%x=0x%x\n", &psram_ctrl->WRITE_SIGNLE, psram_ctrl->WRITE_SIGNLE);
	RTK_LOGI(TAG, "AUTO_LENGTH\t0x%x=0x%x\n", &psram_ctrl->AUTO_LENGTH, psram_ctrl->AUTO_LENGTH);
	RTK_LOGI(TAG, "USER_LENGTH\t0x%x=0x%x\n", &psram_ctrl->USER_LENGTH, psram_ctrl->USER_LENGTH);
	RTK_LOGI(TAG, "AUTO_LENGTH2\t0x%x=0x%x\n", &psram_ctrl->AUTO_LENGTH2, psram_ctrl->AUTO_LENGTH2);
	RTK_LOGI(TAG, "SR\t0x%x=0x%x\n", &psram_ctrl->SR, psram_ctrl->SR);
	RTK_LOGI(TAG, "PDEX_CTRL0\t0x%x=0x%x\n", &psram_ctrl->PDEX_CTRL0, psram_ctrl->PDEX_CTRL0);
	RTK_LOGI(TAG, "PDEX_CTRL1\t0x%x=0x%x\n", &psram_ctrl->PDEX_CTRL1, psram_ctrl->PDEX_CTRL1);
	RTK_LOGI(TAG, "PDEX_CTRL2\t0x%x=0x%x\n", &psram_ctrl->PDEX_CTRL2, psram_ctrl->PDEX_CTRL2);
	RTK_LOGI(TAG, "SLOT_STAVRE\t0x%x=0x%x\n", &psram_ctrl->SLOT_STAVRE, psram_ctrl->SLOT_STAVRE);
	RTK_LOGI(TAG, "WBUF_ID\t0x%x=0x%x\n", &psram_ctrl->WBUF_ID, psram_ctrl->WBUF_ID);

	RTK_LOGI(TAG, "psram_ctrl->SEC_REGION[0].SEC_BASE_x\t0x%x=0x%x\n", &psram_ctrl->SEC_REGION[0].SEC_BASE_x, psram_ctrl->SEC_REGION[0].SEC_BASE_x);
	RTK_LOGI(TAG, "psram_ctrl->SEC_REGION[0].SEC_END_x\t0x%x=0x%x\n", &psram_ctrl->SEC_REGION[0].SEC_END_x, psram_ctrl->SEC_REGION[0].SEC_END_x);
	RTK_LOGI(TAG, "psram_ctrl->SEC_REGION[0].SEC_NONCE_x\t0x%x=0x%x\n", &psram_ctrl->SEC_REGION[0].SEC_NONCE_x, psram_ctrl->SEC_REGION[0].SEC_NONCE_x);

	RTK_LOGI(TAG, "psram_ctrl->SEC_REGION[1].SEC_BASE_x\t0x%x=0x%x\n", &psram_ctrl->SEC_REGION[1].SEC_BASE_x, psram_ctrl->SEC_REGION[1].SEC_BASE_x);
	RTK_LOGI(TAG, "psram_ctrl->SEC_REGION[1].SEC_END_x\t0x%x=0x%x\n", &psram_ctrl->SEC_REGION[1].SEC_END_x, psram_ctrl->SEC_REGION[1].SEC_END_x);
	RTK_LOGI(TAG, "psram_ctrl->SEC_REGION[1].SEC_NONCE_x\t0x%x=0x%x\n", &psram_ctrl->SEC_REGION[1].SEC_NONCE_x, psram_ctrl->SEC_REGION[1].SEC_NONCE_x);

	RTK_LOGI(TAG, "psram_ctrl->SEC_REGION[2].SEC_BASE_x\t0x%x=0x%x\n", &psram_ctrl->SEC_REGION[2].SEC_BASE_x, psram_ctrl->SEC_REGION[2].SEC_BASE_x);
	RTK_LOGI(TAG, "psram_ctrl->SEC_REGION[2].SEC_END_x\t0x%x=0x%x\n", &psram_ctrl->SEC_REGION[2].SEC_END_x, psram_ctrl->SEC_REGION[2].SEC_END_x);
	RTK_LOGI(TAG, "psram_ctrl->SEC_REGION[2].SEC_NONCE_x\t0x%x=0x%x\n", &psram_ctrl->SEC_REGION[2].SEC_NONCE_x, psram_ctrl->SEC_REGION[2].SEC_NONCE_x);

	RTK_LOGI(TAG, "psram_ctrl->SEC_REGION[3].SEC_BASE_x\t0x%x=0x%x\n", &psram_ctrl->SEC_REGION[3].SEC_BASE_x, psram_ctrl->SEC_REGION[3].SEC_BASE_x);
	RTK_LOGI(TAG, "psram_ctrl->SEC_REGION[3].SEC_END_x\t0x%x=0x%x\n", &psram_ctrl->SEC_REGION[3].SEC_END_x, psram_ctrl->SEC_REGION[3].SEC_END_x);
	RTK_LOGI(TAG, "psram_ctrl->SEC_REGION[3].SEC_NONCE_x\t0x%x=0x%x\n", &psram_ctrl->SEC_REGION[3].SEC_NONCE_x, psram_ctrl->SEC_REGION[3].SEC_NONCE_x);

	RTK_LOGI(TAG, "psram_ctrl->SEC_REGION[4].SEC_BASE_x\t0x%x=0x%x\n", &psram_ctrl->SEC_REGION[4].SEC_BASE_x, psram_ctrl->SEC_REGION[4].SEC_BASE_x);
	RTK_LOGI(TAG, "psram_ctrl->SEC_REGION[4].SEC_END_x\t0x%x=0x%x\n", &psram_ctrl->SEC_REGION[4].SEC_END_x, psram_ctrl->SEC_REGION[4].SEC_END_x);
	RTK_LOGI(TAG, "psram_ctrl->SEC_REGION[4].SEC_NONCE_x\t0x%x=0x%x\n", &psram_ctrl->SEC_REGION[4].SEC_NONCE_x, psram_ctrl->SEC_REGION[4].SEC_NONCE_x);

	RTK_LOGI(TAG, "psram_ctrl->SEC_REGION[5].SEC_BASE_x\t0x%x=0x%x\n", &psram_ctrl->SEC_REGION[5].SEC_BASE_x, psram_ctrl->SEC_REGION[5].SEC_BASE_x);
	RTK_LOGI(TAG, "psram_ctrl->SEC_REGION[5].SEC_END_x\t0x%x=0x%x\n", &psram_ctrl->SEC_REGION[5].SEC_END_x, psram_ctrl->SEC_REGION[5].SEC_END_x);
	RTK_LOGI(TAG, "psram_ctrl->SEC_REGION[5].SEC_NONCE_x\t0x%x=0x%x\n", &psram_ctrl->SEC_REGION[5].SEC_NONCE_x, psram_ctrl->SEC_REGION[5].SEC_NONCE_x);
	RTK_LOGI(TAG, "PSPHY_CAL_CTRL\t0x%x=0x%x\n", &psram_phy->PSPHY_CAL_CTRL, psram_phy->PSPHY_CAL_CTRL);
	RTK_LOGI(TAG, "PSPHY_CAL_PAR\t0x%x=0x%x\n", &psram_phy->PSPHY_CAL_PAR, psram_phy->PSPHY_CAL_PAR);
	RTK_LOGI(TAG, "PSPHY_PHY_CTRL\t0x%x=0x%x\n", &psram_phy->PSPHY_PHY_CTRL, psram_phy->PSPHY_PHY_CTRL);
	RTK_LOGI(TAG, "PSPHY_DBG_SEL\t0x%x=0x%x\n", &psram_phy->PSPHY_DBG_SEL, psram_phy->PSPHY_DBG_SEL);
	RTK_LOGI(TAG, "========================================================================\n");


	if (PsramInfo.Psram_Vendor == MCM_PSRAM_VENDOR_WB) {
		u8 psramir0[2] = {0, 0};
		u8 psramcr0[2] = {0, 0};
		u8 psramcr1[2] = {0, 0};

		PSRAM_REG_Read(MCM_PSRAM_VENDOR_WB, 0x0, 2, psramir0, 0, PsramInfo.Psram_DQ16);
		PSRAM_REG_Read(MCM_PSRAM_VENDOR_WB, 0x0, 2, psramcr0, 1, PsramInfo.Psram_DQ16);
		PSRAM_REG_Read(MCM_PSRAM_VENDOR_WB, 0x1, 2, psramcr1, 1, PsramInfo.Psram_DQ16);

		RTK_LOGI(TAG, "IR0=[0x%x, 0x%x], CR0=[0x%x, 0x%x], CR1=[0x%x, 0x%x]\n",
				 psramir0[1], psramir0[0], psramcr0[1], psramcr0[0], psramcr1[1], psramcr1[0]);
	} else {
		u8 mr0 = 0, mr4 = 0, mr8 = 0;

		PSRAM_REG_Read(MCM_PSRAM_VENDOR_APM, 0x0, 1, &mr0, 0, PsramInfo.Psram_DQ16);
		PSRAM_REG_Read(MCM_PSRAM_VENDOR_APM, 0x4, 1, &mr4, 0, PsramInfo.Psram_DQ16);
		PSRAM_REG_Read(MCM_PSRAM_VENDOR_APM, 0x8, 1, &mr8, 0, PsramInfo.Psram_DQ16);

		RTK_LOGI(TAG, "MR0=0x%x, MR4=0x%x, MR8=0x%x\n", mr0, mr4, mr8);
	}
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

	/* 0x08 Disable psramc */
	psram_ctrl->SSIENR = 0;

	/*0x00 set ddr , channel number, check time*/
	psram_ctrl->CTRLR0 = MASK_DDR_EN | MASK_ADDR_CH | CK_MTIMES(0x1) | MASK_CMD_CH;

	/*0x14 set Tpsram = 2*Tpsramc*/
	psram_ctrl->BAUDR = SCKDV(Psram_ClkDiv2);

	/* 0x120 enable sequentical read and write, if it needs to be closed, set BIT_SEQ_TRANS_DIS */
	psram_ctrl->VALID_CMD = BIT_FRD_SINGLE | BIT_CTRLR0_CH;
	/* 0x15c enable wbuff */
	//psram_ctrl->WBUF_ID = 0xFFFFFFFF;

	/* 0x200~0x208 enable scramble fun for the whole psram region*/
	psram_ctrl->SEC_REGION[0].SEC_BASE_x = 0x0;
	psram_ctrl->SEC_REGION[0].SEC_END_x = ((PsramInfo.Psram_Size) | SEC_Rx_MODE(0x0) | BIT_SEC_Rx_EN);
	psram_ctrl->SEC_REGION[0].SEC_NONCE_x = _rand();

	/*0x150 set psram cmd, cmd len,  cs active time for PDE and PDX*/
	psram_ctrl->PDEX_CTRL0 = PDE_CMD0(PsramInfo.Psram_PDEX_CMD0);
	psram_ctrl->PDEX_CTRL1 = PDE_CMD1(PsramInfo.Psram_PDEX_CMD1);
	psram_ctrl->PDEX_CTRL2 = (PDE_CMD_LEN(PSRAM_PDE_CMD_LEN) | PDX_CMD_LEN(PSRAM_PDX_CMD_LEN(40)));
	psram_ctrl->TPR2 = (PDE_CS_H_CNT(CAL_PDEX_CS_H_CNT(40, PsramInfo.Psram_PDE_TIME)) | \
						PDX_CS_H_CNT(CAL_PDEX_CS_H_CNT(40, PsramInfo.Psram_PDX_TIME)));

	/*disable CGATE_EN_PDEX,  let pdex clk freerun.*/
	if ((EFUSE_GetChipVersion()) == SYSCFG_CUT_VERSION_A) {
		psram_ctrl->ICG_EN &= ~BIT_CGATE_EN_PDEX;
	}

	if (PsramInfo.Psram_Vendor == MCM_PSRAM_VENDOR_WB) {
		if (PsramInfo.Psram_DQ16 == MCM_PSRAM_DQ16) {
			/*0x134 set page size , channel number and cmd type*/
			psram_ctrl->DEVICE_INFO = BIT_DATA_UNIT_4B | BIT_DQ16_DATA_CH | BIT_JEDEC_P2CMF | BIT_PSRAM | ATOM_SIZE(0x2) | BIT_RD_PAGE_EN | \
									  BIT_WR_PAGE_EN | PAGE_SIZE(PsramInfo.Psram_Page_size);
		} else {
			psram_ctrl->DEVICE_INFO = BIT_DATA_UNIT_2B | BIT_JEDEC_P2CMF | BIT_PSRAM | ATOM_SIZE(0x1) | BIT_RD_PAGE_EN | \
									  BIT_WR_PAGE_EN | PAGE_SIZE(PsramInfo.Psram_Page_size);
			//only for 8IO
			psram_ctrl->CTRLR0 |= MASK_DATA_CH;
		}
		/*0x110 set 32 fifo entry, every entry has 2bytes,  set variable/fix mode*/
		psram_ctrl->CTRLR2 = BIT_SO_DNUM | TX_FIFO_ENTRY(0x5) | RX_FIFO_ENTRY(0x5) | BIT_DM_ACT | BIT_DM_CA_DIS | \
							 WR_VL_EN(!PSRSAM_FIX_LATENCY) | RD_VL_EN(!PSRSAM_FIX_LATENCY) | RD_WEIGHT(0x2);

		//0x134 no support
		psram_ctrl->TPR1 &= ~(MASK_IDLE_WINDOW | MASK_TPWR);
		//0x140 Tcss = 1.5*2*Tclk
		psram_ctrl->TPR1 |= CS_ACTIVE_SETUP(0x2);
		/* 0x138
			When Tj<85℃, Tcem<=4us. When 85℃<Tj >125℃, Tcem<=1us
			sequentical timeout = CS_SEQ_TIMEOUT*4*Tclk, the maximum value of CS_SEQ_TIMEOUT is 0xff
			cs active hold:  TCSH =0~ -,     CS_ACTIVE_HOLD = 0
			cs high time:    Tcshi = 6ns~ -
			TRWR = 35ns~ - (cmd + tcss + cs high + addr)
			According spec, CS_H_WR_DUM_LEN = cs_high_time/Tclk - 2. So CS_H_RD_DUM_LEN = (TRWR-3T(cmd+addr)-3T(Tcs))/Tclk -2
		*/
		psram_ctrl->TPR0 = (CS_TCEM(Psram_Tcem_T85 * 1000 / PsramInfo.PSRAMC_Clk_Unit / 32 - 2)) | \
						   (CS_SEQ_TIMEOUT(Psram_Seq_timeout)) | \
						   (CS_ACTIVE_HOLD(Psram_WB_CSH)) | \
						   (CS_H_WR_DUM_LEN(((8 * PsramInfo.PSRAMC_Clk_Unit > PsramInfo.Psram_TRWR * 1000) ? 2 : ((PsramInfo.Psram_TRWR * 1000 + PsramInfo.PSRAMC_Clk_Unit - 1) /
											 PsramInfo.PSRAMC_Clk_Unit - 3 - 3 - 2)))) | \
						   (CS_H_RD_DUM_LEN(((8 * PsramInfo.PSRAMC_Clk_Unit > PsramInfo.Psram_TRWR * 1000) ? 2 : ((PsramInfo.Psram_TRWR * 1000 + PsramInfo.PSRAMC_Clk_Unit - 1) /
											 PsramInfo.PSRAMC_Clk_Unit - 3 - 3 - 2))));

		/*0xe0 set psram auto read cmd*/
		psram_ctrl->READ_FAST_SINGLE = FRD_CMD(WB_RD_CMD_TYPE[PsramInfo.Psram_Type]);
		/*0xf4 set psram auto write cmd*/
		psram_ctrl->WRITE_SIGNLE = WR_CMD(WB_WR_CMD_TYPE[PsramInfo.Psram_Type]);
		/*0x11c set auto cmd len and addr len */
		psram_ctrl->AUTO_LENGTH = AUTO_CMD_LENGTH(0x1) | AUTO_ADDR_LENGTH(0x5);
		/* 0x118 set user cmd len and addr len */
		psram_ctrl->USER_LENGTH = USER_ADDR_LENGTH(0x5) | USER_CMD_LENGHT(0x1);
	} else { //apm
		if (PsramInfo.Psram_DQ16 == MCM_PSRAM_DQ16) {
			/*0x134 set page size , channel number and cmd type*/
			psram_ctrl->DEVICE_INFO =  BIT_DATA_UNIT_2B | BIT_DQ16_DATA_CH | BIT_PSRAM | ATOM_SIZE(0x2) | BIT_RD_PAGE_EN | \
									   BIT_WR_PAGE_EN | PAGE_SIZE(PsramInfo.Psram_Page_size);
		} else {
			psram_ctrl->DEVICE_INFO =  BIT_PSRAM | ATOM_SIZE(0x1) | BIT_RD_PAGE_EN | \
									   BIT_WR_PAGE_EN | PAGE_SIZE(PsramInfo.Psram_Page_size);
			//only for 8IO
			psram_ctrl->CTRLR0 |= MASK_DATA_CH;
		}
		/*0x110 set 32 fifo entry, every entry has 2bytes,  set variable/fix mode*/
		psram_ctrl->CTRLR2 = BIT_SO_DNUM | TX_FIFO_ENTRY(0x5) | RX_FIFO_ENTRY(0x5) | BIT_DM_ACT | \
							 WR_VL_EN(!PSRSAM_FIX_LATENCY) | RD_VL_EN(!PSRSAM_FIX_LATENCY) | RD_WEIGHT(0x2);

		/* 0x138
			default is fast refresh: Tcem <= 1us
			sequentical timeout = CS_SEQ_TIMEOUT*4*Tclk, the maximum value of CS_SEQ_TIMEOUT is 0xff
			CS_ACTIVE_HOLD  tCHD:2ns~-
			CS_H_WR_DUM_LEN: tCPH/Tclk-2
		*/
		psram_ctrl->TPR0 = (CS_TCEM(Psram_Tcem_T85 * 1000 / PsramInfo.PSRAMC_Clk_Unit / 32 - 2)) | \
						   (CS_SEQ_TIMEOUT(Psram_Seq_timeout)) | \
						   (CS_ACTIVE_HOLD(Psram_APM_CSH)) | \
						   (CS_H_WR_DUM_LEN((PsramInfo.Psram_CSHI * 1000 / PsramInfo.PSRAMC_Clk_Unit) > 0 ? (((PsramInfo.Psram_CSHI * 1000 + PsramInfo.PSRAMC_Clk_Unit - 1) /
											PsramInfo.PSRAMC_Clk_Unit) - 2) : 1)) | \
						   (CS_H_RD_DUM_LEN((PsramInfo.Psram_CSHI * 1000 / PsramInfo.PSRAMC_Clk_Unit) > 0 ? (((PsramInfo.Psram_CSHI * 1000 + PsramInfo.PSRAMC_Clk_Unit - 1) /
											PsramInfo.PSRAMC_Clk_Unit) - 2) : 1));

		/*0xe0 set psram auto read cmd*/
		psram_ctrl->READ_FAST_SINGLE = FRD_CMD(APM_RD_CMD);
		/*0xf4 set psram auto write cmd*/
		psram_ctrl->WRITE_SIGNLE = WR_CMD(APM_WR_CMD);
		/*0x11c set auto cmd len and addr len */
		psram_ctrl->AUTO_LENGTH = AUTO_CMD_LENGTH(0x2) | AUTO_ADDR_LENGTH(0x4);
		/* 0x118 set user cmd len and addr len */
		psram_ctrl->USER_LENGTH = USER_ADDR_LENGTH(0x4) | USER_CMD_LENGHT(0x2);
	}

	if (PSRSAM_FIX_LATENCY) {
		/*0x11c auto read latency, inphy cycle = rfifo delay + 2 */
		psram_ctrl->AUTO_LENGTH |= RD_DUMMY_LENGTH(2 * 2 * PsramInfo.Psram_Latency_Set - 1) | \
								   IN_PHYSICAL_CYC(PSPHY_RFIFO_READY_DELAY_FIX + 2);
		if (PsramInfo.Psram_Vendor == MCM_PSRAM_VENDOR_WB) {
			/*0x13c auto write latency, 2xLC latency */
			psram_ctrl->AUTO_LENGTH2 = WR_DUMMY_LENGTH(2 * 2 * PsramInfo.Psram_Latency_Set - 2);
		} else {
			/*0x13c auto write latency, apm does not support 2xLC write */
			psram_ctrl->AUTO_LENGTH2 = WR_DUMMY_LENGTH(2 * PsramInfo.Psram_Latency_Set - 2);
		}
		/*0x118 user read latency, 2xLC latency */
		psram_ctrl->USER_LENGTH |= USER_RD_DUMMY_LENGTH(2 * 2 * PsramInfo.Psram_Latency_Set - 1);

	} else {
		/*0x11c auto read latency,1xLC latency */
		psram_ctrl->AUTO_LENGTH |= RD_DUMMY_LENGTH(2 * PsramInfo.Psram_Latency_Set - 1) ;
		/*0x13c auto write latency, 1xLC latency */
		psram_ctrl->AUTO_LENGTH2 = WR_DUMMY_LENGTH(2 * PsramInfo.Psram_Latency_Set - 2);
		/*0x118 user read latency, 1xLC latency */
		psram_ctrl->USER_LENGTH |= USER_RD_DUMMY_LENGTH(2 * PsramInfo.Psram_Latency_Set - 1);
	}
}

void PSRAM_APM_DEVIC_Init(void)
{
	u8 mr0[2];
	u8 mr4[2];
	u8 mr8[2];
	mr0[0] = PSRAM_READ_LATENCY_CODE(PsramInfo.Psram_Latency_Set - 3) | \
			 PSRAM_LT_SELECT(PSRSAM_FIX_LATENCY) | \
			 PSRAM_Drive_Strength(APM_Drive_Strength_100ohms); //0:variable latency 1:fix latency

	mr4[0] = PSRAM_WRITE_LATENCY_CODE(APM_WR_INIT_LATENCY_SPEC[PsramInfo.Psram_Latency_Set - 3]) | PSRAM_Refresh_Setting(APM_SLOW_REFRESH);

	mr8[0] = PSRAM_ENBALE_DQ16;

	mr0[1] = mr0[0];
	mr4[1] = mr4[0];
	mr8[1] = mr8[0];


	PSRAM_REG_Write(MCM_PSRAM_VENDOR_APM, 0x0, 1, mr0, PsramInfo.Psram_DQ16);
	PSRAM_REG_Write(MCM_PSRAM_VENDOR_APM, 0x4, 1, mr4, PsramInfo.Psram_DQ16);
	if (PsramInfo.Psram_DQ16 == MCM_PSRAM_DQ16) {
		PSRAM_REG_Write(MCM_PSRAM_VENDOR_APM, 0x8, 1, mr8, PsramInfo.Psram_DQ16);
	}
}

void PSRAM_WB_DEVIC_Init(void)
{
	u8 data[2];

	if (PsramInfo.Psram_Type == PSRAM_TYPE_WB957) {

		data[0] = PSRAM_BIT8_WB_BURST_LENGTH(PSRAM_BURST_BIT8_CLR) | \
				  PSRAM_WB_RESERVER | \
				  PSRAM_WB_DRIVE_STRENGTH(WB_Drive_Strength_010) | \
				  PSRAM_WB_DPD_EN(WB_NORMAL_OPE);

		data[1] = PSRAM_WB_BURST_LENGTH(WB_BURST_LEN_2048B) | \
				  PSRAM_WB_HyBURSE_EN | \
				  PSRAM_WB_FIX_LATENCY_EN(PSRSAM_FIX_LATENCY) | \
				  PSRAM_WB_INIT_LATENCY(WB_WR_INIT_LATENCY_SPEC[PsramInfo.Psram_Latency_Set - 3]);
	} else {
		data[0] = PSRAM_BIT8_WB_BURST_LENGTH(PSRAM_BURST_BIT8_SET) | \
				  PSRAM_WB_RESERVER | \
				  PSRAM_WB_DRIVE_STRENGTH(WB_Drive_Strength_010) | \
				  PSRAM_WB_DPD_EN(WB_NORMAL_OPE);	 // keep default driving stength

		data[1] = PSRAM_WB_BURST_LENGTH(WB_BURST_LEN_128B) | \
				  PSRAM_WB_HyBURSE_EN | \
				  PSRAM_WB_FIX_LATENCY_EN(PSRSAM_FIX_LATENCY) | \
				  PSRAM_WB_INIT_LATENCY(WB_WR_INIT_LATENCY_SPEC[PsramInfo.Psram_Latency_Set - 3]);
	}

	PSRAM_REG_Write(MCM_PSRAM_VENDOR_WB, PSRAM_WB_CR0, 2, data,  PsramInfo.Psram_DQ16);
}

/**
  * @brief Get APM/Winbond PSRAM register value
  * @param  type: APM :0 Winbond:1
  * @param  addr: APM/Winbond PSRAM register addr
  * @param  read_len: read data length
  * @param  read_data: read data output buf
  * @retval None
  * Data len NDF	DQ		type	cmd	read	addr		data
  * 2	4	DQ16	wb958	0x00e0	0x0001	0xaddr 00	0x00 data[0]		0x00 data[1]
  * 2	2	DQ8		wb955	0x00e0	0x0001	0xaddr 00	0xdata[1] data[0]
  * 2	4	DQ8		wb955	0x0060	0x0001	0xaddr 00	0xdata[1] data[0]	0x00 data[1]
  * 1	4	DQ16	APM128	0x4040	0x0001	0xaddr 00	0x00 data[0]		0x0000
  * 1	2	DQ8		APM64	0x4040	0x0001	0xaddr 00	0x00 data[0]
  */
NON_DRAM_TEXT_SECTION
void PSRAM_REG_Read(u32 type, u32 addr, u32 read_len, u8 *read_data, u32 CR, u8 DQ_16)
{
	SPIC_TypeDef *psram_ctrl = PSRAMC_DEV;
	u16 rxdata[2] = {0};
	u32 rx_ndf = (DQ_16 == MCM_PSRAM_DQ16) ? 4 : 2;
	u16 command = (type == MCM_PSRAM_VENDOR_APM) ? 0x4040 : 0x00e0;
	u16 control_reg_select = CR ? 0x0001 : 0x0000;
	u16 address = (u16)((addr << 8) & 0xFF00);

	/* Wait for SPIC not busy before switch to user mode */
	while (psram_ctrl->SR & BIT_BUSY);

	/* Enter User Mode */
	psram_ctrl->CTRLR0 |= BIT_USER_MODE;

	/* Configure for reading */
	psram_ctrl->SSIENR = 0;
	psram_ctrl->RX_NDF = RX_NDF(rx_ndf);
	psram_ctrl->TX_NDF = 0;
	psram_ctrl->CTRLR0 |= TMOD(3);

	/* Set command, control register selection, and address */
	psram_ctrl->DR[0].HALF = command;
	psram_ctrl->DR[0].HALF = control_reg_select;
	psram_ctrl->DR[0].HALF = address;

	/* Enable SPI_FLASH User Mode */
	psram_ctrl->SSIENR = BIT_SPIC_EN;
	while (psram_ctrl->SSIENR & BIT_SPIC_EN);

	/* Read data */
	rxdata[0] = psram_ctrl->DR[0].HALF & 0xFFFF;

	if (rx_ndf >= 4) {
		rxdata[1] = psram_ctrl->DR[0].HALF & 0xFFFF;
	}

	/* Extract read data */
	if (type == MCM_PSRAM_VENDOR_APM) {
		read_data[0] = (u8)(rxdata[0] & 0xFF);
	} else {
		if (DQ_16 == MCM_PSRAM_DQ16) {
			read_data[0] = (u8)(rxdata[0] & 0xFF);
			read_data[1] = (u8)(rxdata[1] & 0xFF);
		} else {
			_memcpy(read_data, rxdata, read_len);
		}
	}

	/* Wait until transfer is complete (hardware automatically clears bits) */
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
  * @note
  * Data len NDF	DQ		type	cmd	write	addr		data
  * 2	4	DQ16	wb958	0x0060	0x0001	0xaddr 00	0x00 data[0]		0x00 data[1]
  * 2	2	DQ8		wb955	0x0060	0x0001	0xaddr 00	0xdata[1] data[0]
  * 2	4	DQ8		wb955	0x0060	0x0001	0xaddr 00	0xdata[1] data[0]	0x00 data[1]
  * 1	4	DQ16	APM128	0xC0C0	0x0001	0xaddr 00	0x00 data[0]		0x0000
  * 1	2	DQ8		APM64	0xC0C0	0x0001	0xaddr 00	0x00 data[0]
  */
NON_DRAM_TEXT_SECTION
void PSRAM_REG_Write(u32 type, u32 addr, u32 write_len, u8 *write_data, u8 DQ_16)
{
	SPIC_TypeDef *psram_ctrl = PSRAMC_DEV;
	u32 tx_num = 0;
	u16 command = (type == MCM_PSRAM_VENDOR_APM) ? 0xc0c0 : 0x0060;
	u16 control_reg_select = 0x0001;
	u16 addr_data = (u16)((addr << 8) & 0xFF00);
	u32 tx_ndf = (DQ_16 == MCM_PSRAM_DQ16) ? 4 : 2;

	/* Wait for SPIC not busy before switching to user mode */
	while (psram_ctrl->SR & BIT_BUSY);

	/* Enter User Mode */
	psram_ctrl->CTRLR0 |= BIT_USER_MODE;

	/* Disable SSIENR to program control registers */
	psram_ctrl->SSIENR = 0;

	/* Set TX mode and configure TX/RX NDF */
	psram_ctrl->CTRLR0 &= ~TMOD(3);
	psram_ctrl->RX_NDF = 0;
	psram_ctrl->TX_NDF = TX_NDF(tx_ndf);

	/* Write command, control register, and address */
	psram_ctrl->DR[0].HALF = command;
	psram_ctrl->DR[0].HALF = control_reg_select;
	psram_ctrl->DR[0].HALF = addr_data;

	/* Fill data */
	while (tx_num < write_len) {
		u16 data = (u16)(write_data[tx_num++] & 0x00FF);
		if (DQ_16 != MCM_PSRAM_DQ16 && type != MCM_PSRAM_VENDOR_APM && tx_num < write_len) {
			data |= (u16)((write_data[tx_num++] << 8) & 0xFF00);
		}
		psram_ctrl->DR[0].HALF = data;

		if (type == MCM_PSRAM_VENDOR_APM && DQ_16 == MCM_PSRAM_DQ16) {
			psram_ctrl->DR[0].HALF = 0x0000;
		}
	}

	/* Enable SPI_FLASH User Mode */
	psram_ctrl->SSIENR = BIT_SPIC_EN;
	while (psram_ctrl->SSIENR & BIT_SPIC_EN);

	/* Wait for transfer to complete */
	while (psram_ctrl->SR & BIT_BUSY);

	/* Restore to auto mode */
	psram_ctrl->CTRLR0 &= ~BIT_USER_MODE;
}

u32 PSRAM_PATTEN_CHECK(u32 group, u32 *data)
{
	u32 patteno, patteni;
	for (u32 i = 0; i < 6; i++) {
		patteni = (PSRAM_CALIB_PATTERN[i] >> group * 8) & 0x000000ff;
		patteno = (data[i] >> group * 8) & 0x000000ff;
		if (patteno != patteni) {
			return FALSE;
		}
	}
	return TRUE;
}

/**
  * @brief PSRAM SW calibration function
  * @param DQnum: psram dq number select
  * @retval TRUE/FALSE
  * @note cache will be disable during calibration
  */
u32 PSRAM_SW_Calibration(u32 DQnum)
{
	PSPHY_TypeDef *psram_phy = PSRAMPHY_DEV;

	u32 caltempN = 0;
	u32 phase = 0;
	u32 tempPHYPara, calres;
	u32 *tempdatawr = PSRAM_CALIB_PATTERN;
	u32 tempdatard[6] = {0, 0, 0, 0, 0, 0};
	int window_start = -1;
	int window_end = -1;
	int window_size = 0;

	int windowt_start = -1;
	int windowt_size = 0;
	int windowt_end = -1;

	int phase_cnt = -1;
	u32 phase_num = 8;

	/*Disable HW calibration*/
	if ((DQnum == PSRAM_DQ16_SL0) || (DQnum == PSRAM_DQ8_SL0)) {
		psram_phy->PSPHY_CAL_CTRL &= (~PSPHY_BIT_CFG_CAL_EN);
		tempPHYPara = psram_phy->PSPHY_CAL_PAR & (~PSPHY_MASK_CFG_CAL_N);
		psram_phy->PSPHY_PRE_CAL_PAR &= (~PSPHY_MASK_PRE_CAL_PHASE);
	} else {
		psram_phy->PSPHY_SL1_CAL_CTRL &= (~PSPHY_BIT_SL1_CFG_CAL_EN);
		tempPHYPara = psram_phy->PSPHY_SL1_CAL_PAR & (~PSPHY_MASK_SL1_CFG_CAL_N) ;
		psram_phy->PSPHY_SL1_PRE_CAL_PAR &= (~PSPHY_MASK_SL1_PRE_CAL_PHASE);
	}

	for (phase = 0x0; phase < phase_num; phase ++) {
		windowt_size = 0;
		windowt_start = -1;
		windowt_end = -1;
		// RTK_LOGI(TAG, "===phase %x =====\n", phase);

		for (caltempN = 0; caltempN < 32; caltempN++) {

			if ((DQnum == PSRAM_DQ16_SL0) || (DQnum == PSRAM_DQ8_SL0)) {
				psram_phy->PSPHY_PRE_CAL_PAR = PSPHY_PRE_CAL_PHASE(phase);
				psram_phy->PSPHY_CAL_PAR = tempPHYPara | caltempN;
			} else {
				psram_phy->PSPHY_SL1_PRE_CAL_PAR = PSPHY_SL1_PRE_CAL_PHASE(phase);
				psram_phy->PSPHY_SL1_CAL_PAR = tempPHYPara | caltempN;
			}

			for (int i = 0; i < 6; i += 1) {
				HAL_WRITE32(PSRAM_BASE, i * 0x50000, tempdatawr[i]);
				DCache_CleanInvalidate(PSRAM_BASE + i * 0x50000, CACHE_LINE_SIZE);
				tempdatard[i] = HAL_READ32(PSRAM_BASE, i * 0x50000);
			}

			if (DQnum == PSRAM_DQ16_SL0) {
				calres = (PSRAM_PATTEN_CHECK((u32)PSRAM_CAL_DQ0_7, tempdatard) == TRUE) & (PSRAM_PATTEN_CHECK((u32)PSRAM_CAL_DQ16_23, tempdatard) == TRUE);
			} else if (DQnum == PSRAM_DQ16_SL1) {
				calres = (PSRAM_PATTEN_CHECK((u32)PSRAM_CAL_DQ8_15, tempdatard) == TRUE) & (PSRAM_PATTEN_CHECK((u32)PSRAM_CAL_DQ24_31, tempdatard) == TRUE);
			} else {
				calres = (_memcmp(tempdatard, PSRAM_CALIB_PATTERN, 24) == 0 ? 1 : 0);
			}

			if (calres) {
				//RTK_LOGI(TAG, "ok %x %x %x %x %x %x %x\n", caltempN, tempdatard[0], tempdatard[1], tempdatard[2], tempdatard[3], tempdatard[4], tempdatard[5]);
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
				//RTK_LOGI(TAG, "fail %x %x %x %x %x %x %x\n", caltempN, tempdatard[0], tempdatard[1], tempdatard[2], tempdatard[3], tempdatard[4], tempdatard[5]);
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
				   PSPHY_CFG_CAL_N((window_end + window_start) / 2);

	if ((DQnum == PSRAM_DQ16_SL0) || (DQnum == PSRAM_DQ8_SL0)) {
		psram_phy->PSPHY_CAL_PAR = tempPHYPara;
		psram_phy->PSPHY_PRE_CAL_PAR = phase_cnt;
		/*start HW calibration*/
		psram_phy->PSPHY_CAL_CTRL |= PSPHY_BIT_CFG_CAL_EN;
	} else {
		psram_phy->PSPHY_SL1_CAL_PAR = tempPHYPara;
		psram_phy->PSPHY_SL1_PRE_CAL_PAR = phase_cnt;
		psram_phy->PSPHY_SL1_CAL_CTRL |= PSPHY_BIT_CFG_CAL_EN;
	}

	return TRUE;
}

u32 PSRAM_calibration(void)
{
	if (PsramInfo.Psram_DQ16 == MCM_PSRAM_DQ16) {
		return ((PSRAM_SW_Calibration(PSRAM_DQ16_SL0)) & (PSRAM_SW_Calibration(PSRAM_DQ16_SL1)));
	} else {
		return PSRAM_SW_Calibration(PSRAM_DQ8_SL0);
	}
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

	PSPHY_TypeDef *psram_phy = PSRAMPHY_DEV;
	psram_phy->PSPHY_CAL_CTRL = PSRAM_MASK_CAL_ITT;
	RRAM_TypeDef *rram = RRAM_DEV;

	u8 DQnumber = rram->PSRAM_DQ;
	u8 psram_vendor = rram->PSRAM_TYPE;

	if (psram_vendor == MCM_PSRAM_VENDOR_APM) {

		psram_halfsleep[0] = 0xF0;
		psram_halfsleep[1] = 0xF0;

		/*enable apm slow refresh*/
		PSRAM_REG_Read(MCM_PSRAM_VENDOR_APM, 0x4, 1, mr4, 1, DQnumber);
		mr4[0] |= PSRAM_ENABLE_SLOW_REFRESH;
		mr4[1] = mr4[0];
		PSRAM_REG_Write(MCM_PSRAM_VENDOR_APM, 0x4, 1, mr4, DQnumber);
		PSRAM_REG_Write(MCM_PSRAM_VENDOR_APM, 0x06, 1, psram_halfsleep, DQnumber);

	} else {
		psram_halfsleep[0] = 0xFF;
		psram_halfsleep[1] = 0x20;

		PSRAM_REG_Write(MCM_PSRAM_VENDOR_WB, 0x1, 2, psram_halfsleep, DQnumber);
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

	PSPHY_TypeDef *psram_phy = PSRAMPHY_DEV;
	RRAM_TypeDef *rram = RRAM_DEV;

	u8 DQnumber = rram->PSRAM_DQ;
	u8 psram_vendor = rram->PSRAM_TYPE;

	/*1. auto read , trigger psram to exit sleep mode*/
	DCache_Invalidate((u32)__km4tz_bd_psram_start__, 32);
	rdata = HAL_READ32((u32)__km4tz_bd_psram_start__, 0);

	/* 2. when PG wakeup, fisrt read will cause phy hang, set PSPHY_PHY_INFO to return work correctly*/
	temp = psram_phy->PSPHY_PHY_INFO;
	psram_phy->PSPHY_PHY_INFO = temp;

	/*3. wait for Txhs until psram exit sleep mode*/
	if (psram_vendor == MCM_PSRAM_VENDOR_APM) {
		DelayUs(150);
	} else  if (psram_vendor == MCM_PSRAM_VENDOR_WB) {
		DelayUs(100);
	}

	/* 4. read again. */
	DCache_Invalidate((u32)__km4tz_bd_psram_start__, 32);
	rdata = HAL_READ32((u32)__km4tz_bd_psram_start__, 0);
	UNUSED(rdata);

	/*7. disable apm slow refresh*/
	if (psram_vendor == MCM_PSRAM_VENDOR_APM) {
		PSRAM_REG_Read(MCM_PSRAM_VENDOR_APM, 0x4, 1, mr4, 1, DQnumber);
		mr4[0] &= ~PSRAM_ENABLE_SLOW_REFRESH;
		mr4[1] = mr4[0];
		PSRAM_REG_Write(MCM_PSRAM_VENDOR_APM, 0x4, 1, mr4, DQnumber);
	}
}


/**
  * @brief  set psram to enter/exit half sleep mode.
  * @param  NewState, ENABLE or DISABLE.
  * @retval None
  * @note Trigger spic automatically enter/exit half-sleep mode,
          and during the enter/exit sleep half-period, spic will intercept all access to PSRAM
  */
NON_DRAM_TEXT_SECTION
void PSRAM_HalfSleep_PDEX(u32 NewState)
{
	u32 psram_ctrl;
	u32 psramtype = 0;
	RRAM_TypeDef *rram = RRAM_DEV;

	/*polling pdex_ack, avoid pdex_cmd conflict*/
	PSRAM_CHECK_WITH_TIMEOUT((LSYS_GET_PSRAM_PDEX_REQ(HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL)) == 0x0), 0xFFFFFF);

	psram_ctrl = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL);
	/*send cmd*/
	if (NewState == ENABLE) {
		psramtype = rram->PSRAM_TYPE;
		psram_ctrl |= LSYS_BIT_PSRAM_PDEX_CMD;
	} else {
		psram_ctrl &= ~LSYS_BIT_PSRAM_PDEX_CMD;
	}

	/*send cmd*/
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL, psram_ctrl);

	/*send request   avoiding issues caused by asyncs*/
	psram_ctrl |= LSYS_BIT_PSRAM_PDEX_REQ;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL, psram_ctrl);

	/*Ensure that PDE is completed before entering PG, the designer deduced 173us based on SPIC internal, but some have errors.*/
	if ((NewState == ENABLE) & (psramtype == MCM_PSRAM_VENDOR_APM)) {
		DelayUs(173);
	}

}