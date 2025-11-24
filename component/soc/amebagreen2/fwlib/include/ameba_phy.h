/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

enum  eth_phy_type {
	RTL_8721F,
	RTL_8721G,
};

enum  eth_mode {
	ETH_MAC_MODE,
	ETH_PHY_MODE,
};

enum  phy_clk_mode {
	EXTERNAL_50M,
	EXTERNAL_25M,
	INTERNAL_XTAL,
};

enum  phy_MMD_type {
	RTK_MMD_TYPE,

};

enum  phy_address {
	PHYID_0,
	PHYID_1,
	PHYID_2,
	PHYID_3,
};

enum phy_mmd_access_mode {
	PHY_MMD_ADDR,
	PHY_MMD_DATA_NO_POST_INC,
	PHY_MMD_DATA_RD_WR_POST_INC,
	PHY_MMD_DATA_RD_POST_INC
};

/* MMD register mapping table */
enum rtl8201fr_mmd_device_t {
	RTL8201FR_MMD_DEV_PCS = 3,      /* PCS (Physical Coding Sublayer) MMD device address */
	RTL8201FR_MMD_DEV_EEE_ADV = 7   /* EEE advertisement/ability MMD device address */
};

enum rtl8201fr_mmd_reg_t {
	RTL8201FR_MMD_REG_EEEPC1R  = 0x0,   /* EEE PCS Control 1 Register Offset */
	RTL8201FR_MMD_REG_EEEPS1R  = 0x01,  /* EEE PCS Status 1 Register Offset */
	RTL8201FR_MMD_REG_EEECR    = 0x14,  /* EEE Capability Register Offset */
	RTL8201FR_MMD_REG_EEEWER   = 0x16,  /* EEE Wake Error Register Offset */
	RTL8201FR_MMD_REG_EEEAR    = 0x3c,  /* EEE Advertisement Register Offset */
	RTL8201FR_MMD_REG_EEELPAR  = 0x3d   /* EEE Link Partner Ability Register Offset */
};

/* RTL8201FR Page0 Register 13: MACR Register (0x0D) */
#define RTL8201FR_MACR_FUNC_ADDR_MASK       (0x3 << 14)         	/*[15:14] Function selection for MMD access*/
#define RTL8201FR_MACR_FUNC_ADDR(x)         (((x) & 0x3) << 14)
#define RTL8201FR_MACR_FUNC_ADDR_GET(x)		(((x) & 0x3) >> 14)
#define RTL8201FR_MACR_DEVAD_MASK      		(0x1F << 0)				/*[4:0] MMD device address*/
#define RTL8201FR_MACR_DEVAD(x)        		(((x) & 0x1F) << 0)
#define RTL8201FR_MACR_DEVAD_GET(x)    		(((x) & 0x1F) >> 0)

/* RTL8201FR Page0 Register 14: MAADR (MMD Access Address/Data Register) */
#define RTL8201FR_MAADR_DATA_MASK        (0xFFFF << 0)     			/* [15:0] Address/Data field, used for MMD management via MACR/MAADR pair */
#define RTL8201FR_MAADR_DATA(x)          (((x) & 0xFFFF) << 0)
#define RTL8201FR_MAADR_DATA_GET(x)      (((x) & 0xFFFF) >> 0)


/* --- Page 4: Energy Efficient Ethernet (EEE) --- */
#define	RTL8201FR_PAGE4_REG_EEE_CAP_EN          (0x10)   /*!< EEE Capability Register (Page 4, Reg 21) */
#define RTL8201FR_PAGE4_REG_EEE_CAP             (0x15)   /*!< EEE Capability Register (Page 4, Reg 21) */

/* --- Page 7: Special/Custom registers --- */
#define RTL8201FR_PAGE7_REG_RMII_MODE_SET		(0x10)   /*!< LED Mode Register (Page 7, Reg 11) */
#define RTL8201FR_PAGE7_REG_CUSTOM_LED_SET      (0x11)   /*!< LED Mode Register (Page 7, Reg 11) */

/*******************************************************************************
 * RTL8201FR PAGE4 Registers
 ******************************************************************************/

/* Page4 Register 16: EEE Capability Enable Register */
#define RTL8201FR_P4_16_EEE_10_CAP           (1U << 13)   /* [13] Enable EEE 10M capability */
#define RTL8201FR_P4_16_EEE_NWAY_EN          (1U << 12)   /* [12] Enable Next Page Exchange for EEE 100M */
#define RTL8201FR_P4_16_TX_QUIET_EN          (1U << 9)    /* [9] Enable turning off 100TX when TX Quiet State (recommended for EEE) */
#define RTL8201FR_P4_16_RX_QUIET_EN          (1U << 8)    /* [8] Enable turning off 100RX when RX Quiet State (recommended for EEE) */

/* Page4 Register 21: EEE Capability Register */
#define RTL8201FR_P4_21_RG_DIS_LDVT          (1U << 12)   /* [12] Set 1: Disable line driver of the analog circuit */
#define RTL8201FR_P4_21_EEE_100_CAP_GET(x)   (((x) >> 0) & 0x1) /* [0] Indicate link partner supports EEE 100M (NWay result, Read Only) */


/*******************************************************************************
 * RTL8201FR PAGE7 Registers
 ******************************************************************************/

/* Page7 Register 16: RMII Mode Setting Register (RMSR) */
#define RTL8201FR_P7_16_RMII_CLK_DIR		 (1U << 12)			 /* [12] This bit sets type of TXC in RMII mode. */
#define RTL8201FR_P7_16_RMII_TX_OFFSET(x)    (((x) & 0xF) << 8)  /* [11:8] Adjust RMII TX interface timing */
#define RTL8201FR_P7_16_RMII_RX_OFFSET(x)    (((x) & 0xF) << 4)  /* [7:4]  Adjust RMII RX interface timing */
#define RTL8201FR_P7_16_RMII_MODE            (1U << 3)           /* [3]    1: Set RMII mode (Must keep to 1) */
#define RTL8201FR_P7_16_RMII_RXDV_SEL        (1U << 2)           /* [2]    1: CRS_DV pin is RXDV; 0: CRS_DV is CRS_DV signal */
#define RTL8201FR_P7_16_RMII_RXDSEL          (1U << 1)           /* [1]    1: RMII data with SSD error; 0: RMII data only */

/*******************************************************************************
 * RTL8201FR MMD Register Mapping (accessed via MACR/MAADR)
 ******************************************************************************/
/* Device 3, Offset 0: EEEPCS Control 1 Register */
#define RTL8201FR_EEEPC1R_CLK_STOP_EN      (0x1 << 10)   			/* [10] Enable RXC clock stop in LPI mode */

/* Device 3, Offset 1: EEEPCS Status 1 Register (Read Only) */
#define RTL8201FR_EEEPS1R_TX_LPI_RCVD_GET(x)    (((x) >> 11) & 0x1)	/* [11] TX PCS received LPI indication */
#define RTL8201FR_EEEPS1R_RX_LPI_RCVD_GET(x)    (((x) >> 10) & 0x1)	/* [10] RX PCS received LPI indication */
#define RTL8201FR_EEEPS1R_TX_LPI_IND_GET(x)     (((x) >> 9) & 0x1) 	/* [9] TX PCS LPI indication in progress */
#define RTL8201FR_EEEPS1R_RX_LPI_IND_GET(x)     (((x) >> 8) & 0x1) 	/* [8] RX PCS LPI indication in progress */
#define RTL8201FR_EEEPS1R_CLK_STOP_CAP_GET(x)   (((x) >> 6) & 0x1) 	/* [6] LPI TXC clock stop capability */

/* Device 3, Offset 0x14: EEE Capability Register (Read Only) */
#define RTL8201FR_EEECR_100BT_EEE_SUP_GET(x)    (((x) >> 1) & 0x1)  /* [1] 100Base-TX EEE support */

/* Device 3, Offset 0x16: EEE Wake Error Register (Read Clear)*/
#define RTL8201FR_EEEWER_WAKE_ERR_CNT_MASK      (0xFFFF)       		/* [15:0] EEE Wake error counter */
#define RTL8201FR_EEEWER_WAKE_ERR_CNT(x)        ((x) & 0xFFFF)
#define RTL8201FR_EEEWER_WAKE_ERR_CNT_GET(x)    ((x) & 0xFFFF)

/* Device 7, Offset 0x3c: EEE Advertisement Register */
#define RTL8201FR_EEEAR_100BT_EEE_ADV_MASK      (0x1 << 1)     		/* [1] Advertise 100Base-TX EEE capability */
#define RTL8201FR_EEEAR_100BT_EEE_ADV(x)        (((x) & 0x1) << 1)
#define RTL8201FR_EEEAR_100BT_EEE_ADV_GET(x)    (((x) >> 1) & 0x1)

/* Device 7, Offset 0x3d: EEE Link Partner Ability Register */
#define RTL8201FR_EEELPAR_LP_100BT_EEE_GET(x)   (((x) >> 1) & 0x1)  /* [1] Link partner advertises 100Base-TX EEE */


/* RTL8211F Page 0xA43 Register 0x19: PHY Specific Control Register 2*/
#define RTL8211F_PHY_MODE_PAGE					(0xA43)
#define RTL8211F_PHYCR2_ADDR					(0x19)
#define RTL8211F_PHCR2_PHY_MODE					(0x0863)
#define RTL8211F_PHCR2_MAC_MODE					(0x0843)


int PHY_SoftWareReset(uint8_t phy_id);
int PHY_RestartAutoNego(uint8_t phy_id);
int PHY_SetMmdReg(uint8_t phy_id, uint8_t mmd_dev, uint16_t mmd_addr, uint16_t data);
int PHY_GetMmdReg(uint8_t phy_id, uint8_t mmd_dev, uint16_t mmd_addr, uint16_t *p_data);
int PHY_EnableEEE(uint8_t phy_id, u32 status);
int PHY_SetEEEMode(uint8_t phy_id, uint8_t eth_mode);
int PHY_SetRefclkDir(uint8_t phy_id, u32 mac_dir);