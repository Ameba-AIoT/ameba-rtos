/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

enum  eth_phy_type {
	RTL_8721F,
	RTL_8721G,
};

enum  eee_mode {
	EEE_MAC_MODE,
	EEE_PHY_MODE,
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

#define RTK_EEEPC1R_DEVICE 0x3
#define RTK_EEEPC1R_ADDRESS 0x0

#define RTK_EEEPS1R_DEVICE 0x3
#define RTK_EEEPS1R_ADDRESS 0x1

#define RTK_EEECR_DEVICE 0x3
#define RTK_EEECR_ADDRESS 0x14

#define RTK_EEEWER_DEVICE 0x3
#define RTK_EEEWER_ADDRESS 0x16

#define RTK_EEEAR_DEVICE 0x7
#define RTK_EEEAR_ADDRESS 0x3c
#define RTK_EEEAR_ADVERTISE_EEE 0x2
#define RTK_EEEAR_NOT_ADVERTISE_EEE 0x0

#define RTK_EEELPAR_DEVICE 0x7
#define RTK_EEELPAR_ADDRESS 0x3d


#define RTK8201F_EEE_CAPAB_PAGE 0x4
#define RTK8201F_EEE_CAPAB_REG 0x10
#define RTK8201F_EEE_CAPAB_EN 0x7377

#define RTK8201F_EEE_MODE_PAGE 0x7
#define RTK8201F_EEE_MODE_REG 0x14
#define RTK8201F_EEE_MAC_MODE_SET 0xD5
#define RTK8201F_EEE_PHY_MODE_SET 0x10D5

#define RTK8201F_RMSR_MODE_PAGE 0x7
#define RTK8201F_RMSR_MODE_REG 0x10
#define RTK8201F_RMSR_MAC_OUTPUT 0x1ffa
#define RTK8201F_PMSR_MAC_INPUT 0xffa

#define RTK_MACR_DATA BIT(14)

void PHY_SW_RESET(void);
void PHY_RESTART_AN(void);
void PHY_SET_MMD_REG(u32 type, u32 device, u32 addr, u32 data);
u32 PHY_GET_MMD_REG(u32 type, u32 device, u32 addr);
void PHY_ENABLE_EEE(u32 type, u32 STATUS);
void PHY_SET_EEE_MODE(u32 type, u32 mode);
void PHY_SET_REFCLK_DIR(u32 type, u32 MACDIR);