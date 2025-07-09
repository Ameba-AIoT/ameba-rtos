/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
// static const char *const TAG = "PHY";


void PHY_SW_RESET(void)
{
	Ethernet_Write_PhyReg(FEPHY_REG_PAGE_0, FEPHY_REG_ADDR_0, 0x8000);
	DelayMs(150);
}

void PHY_RESTART_AN(void)
{
	Ethernet_Write_PhyReg(FEPHY_REG_PAGE_0, FEPHY_REG_ADDR_0, 0x1200);
}


void PHY_SET_MMD_REG(u32 type, u32 device, u32 addr, u32 data)
{
	switch (type) {
	case RTK_MMD_TYPE:
		Ethernet_Write_PhyReg(FEPHY_REG_PAGE_0, FEPHY_REG_ADDR_13, device);
		Ethernet_Write_PhyReg(FEPHY_REG_PAGE_0, FEPHY_REG_ADDR_14, addr);
		Ethernet_Write_PhyReg(FEPHY_REG_PAGE_0, FEPHY_REG_ADDR_13, (device | RTK_MACR_DATA));
		Ethernet_Write_PhyReg(FEPHY_REG_PAGE_0, FEPHY_REG_ADDR_14, data);
		break;

	default:
		break;
	}
}

u32 PHY_GET_MMD_REG(u32 type, u32 device, u32 addr)
{
	u32 rvalue;
	switch (type) {
	case RTK_MMD_TYPE:
		Ethernet_Write_PhyReg(FEPHY_REG_PAGE_0, FEPHY_REG_ADDR_13, device);
		Ethernet_Write_PhyReg(FEPHY_REG_PAGE_0, FEPHY_REG_ADDR_14, addr);
		Ethernet_Write_PhyReg(FEPHY_REG_PAGE_0, FEPHY_REG_ADDR_13, (device | RTK_MACR_DATA));
		rvalue = Ethernet_Read_PhyReg(FEPHY_REG_PAGE_0, FEPHY_REG_ADDR_14);
		break;

	default:
		break;
	}
	return rvalue;
}

void PHY_ENABLE_EEE(u32 type, u32 STATUS)
{
	switch (type) {
	case RTL_8721F:
		if (STATUS) {
			Ethernet_Write_PhyReg(RTK8201F_EEE_CAPAB_PAGE, RTK8201F_EEE_CAPAB_REG, RTK8201F_EEE_CAPAB_EN);
			PHY_SET_MMD_REG(RTK_MMD_TYPE, RTK_EEEAR_DEVICE, RTK_EEEAR_ADDRESS, RTK_EEEAR_ADVERTISE_EEE);
			PHY_RESTART_AN();
		} else {


		}
		break;
	case RTL_8721G:
		if (STATUS) {

		} else {

		}
		break;
	default:
		break;
	}
}

void PHY_SET_EEE_MODE(u32 type, u32 mode)
{
	switch (type) {
	case RTL_8721F:
		if (mode == ETH_MAC_MODE) {
			Ethernet_Write_PhyReg(RTK8201F_EEE_MODE_PAGE, RTK8201F_EEE_MODE_REG, RTK8201F_EEE_MAC_MODE_SET);
		} else {


		}
		break;
	case RTL_8721G:
		break;
	default:
		break;
	}
}


void PHY_SET_REFCLK_DIR(u32 type, u32 MACDIR)
{
	switch (type) {
	case RTL_8721F:
		if (MACDIR == ETH_REFCLK_ON) {
			Ethernet_Write_PhyReg(RTK8201F_RMSR_MODE_PAGE, RTK8201F_RMSR_MODE_REG, RTK8201F_RMSR_MAC_OUTPUT);
		} else {
			Ethernet_Write_PhyReg(RTK8201F_RMSR_MODE_PAGE, RTK8201F_RMSR_MODE_REG, RTK8201F_PMSR_MAC_INPUT);
		}
		break;
	case RTL_8721G:
		break;
	default:
		break;
	}
}