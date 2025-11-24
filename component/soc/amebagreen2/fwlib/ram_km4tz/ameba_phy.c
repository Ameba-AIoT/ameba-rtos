/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
static const char *const TAG = "PHY";


int PHY_SoftWareReset(uint8_t phy_id)
{
	int ret = 0;

	/* Select page 0 */
	ret = Ethernet_WritePhyReg(phy_id, FEPHY_REG_ADDR_31, FEPHY_REG_PAGE_0);
	if (ret != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to select page 0. PHY(%d)", phy_id);
		return ret;
	}

	/* Write reset command to Basic Control Register */
	ret = Ethernet_WritePhyReg(phy_id, FEPHY_REG_ADDR_0, PHY_RESET);
	if (ret != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to write software reset. PHY(%d)", phy_id);
		return ret;
	}

	/* Wait for reset to complete as required by the datasheet */
	DelayMs(20);
	return RTK_SUCCESS;
}

int PHY_RestartAutoNego(uint8_t phy_id)
{
	int ret = 0;

	/* Select page 0 */
	ret = Ethernet_WritePhyReg(phy_id, FEPHY_REG_ADDR_31, FEPHY_REG_PAGE_0);
	if (ret != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to select page 0. PHY(%d)", phy_id);
		return ret;
	}

	/* Restart auto-negotiation */
	ret = Ethernet_WritePhyReg(phy_id, FEPHY_REG_ADDR_0, EN_AUTO_GEN | RESTART_AUTO_GEN);
	if (ret != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to restart auto-negotiation. PHY(%d)", phy_id);
		return ret;
	}
	return RTK_SUCCESS;
}

int PHY_SetMmdReg(uint8_t phy_id, uint8_t mmd_dev, uint16_t mmd_addr, uint16_t data)
{
	int ret = 0;

	/* Select page 0 */
	ret = Ethernet_WritePhyReg(phy_id, FEPHY_REG_ADDR_31, FEPHY_REG_PAGE_0);
	if (ret != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to select page 0. PHY(%d)", phy_id);
		return ret;
	}
	/* Set address mode and select MMD device */
	ret = Ethernet_WritePhyReg(phy_id, FEPHY_REG_ADDR_13, RTL8201FR_MACR_FUNC_ADDR(PHY_MMD_ADDR) | mmd_dev);
	if (ret != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to set address mode. PHY(%d), DEV(%d)", phy_id, mmd_dev);
		return ret;
	}
	/* Write address value */
	ret = Ethernet_WritePhyReg(phy_id, FEPHY_REG_ADDR_14, mmd_addr);
	if (ret != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to set MMD address. PHY(%d), ADDR(0x%X)", phy_id, mmd_addr);
		return ret;
	}
	/* Set data mode and select MMD device */
	ret = Ethernet_WritePhyReg(phy_id, FEPHY_REG_ADDR_13, RTL8201FR_MACR_FUNC_ADDR(PHY_MMD_DATA_NO_POST_INC) | mmd_dev);
	if (ret != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to set data mode. PHY(%d), DEV(%d)", phy_id, mmd_dev);
		return ret;
	}
	/* Write data to MMD register */
	ret = Ethernet_WritePhyReg(phy_id, FEPHY_REG_ADDR_14, data);
	if (ret != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to write data to MMD register. PHY(%d), DATA(0x%X)", phy_id, data);
		return ret;
	}
	return RTK_SUCCESS;
}

int PHY_GetMmdReg(uint8_t phy_id, uint8_t mmd_dev, uint16_t mmd_addr, uint16_t *p_data)
{
	int ret = 0;
	uint16_t data = 0;

	/* Select page 0 */
	ret = Ethernet_WritePhyReg(phy_id, FEPHY_REG_ADDR_31, FEPHY_REG_PAGE_0);
	if (ret != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to select page 0. PHY(%d)", phy_id);
		return ret;
	}
	/* Set address mode and select MMD device */
	ret = Ethernet_WritePhyReg(phy_id, FEPHY_REG_ADDR_13, RTL8201FR_MACR_FUNC_ADDR(PHY_MMD_ADDR) | mmd_dev);
	if (ret != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to set address mode. PHY(%d), DEV(%d)", phy_id, mmd_dev);
		return ret;
	}
	/* Write address value */
	ret = Ethernet_WritePhyReg(phy_id, FEPHY_REG_ADDR_14, mmd_addr);
	if (ret != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to set MMD address. PHY(%d), ADDR(0x%X)", phy_id, mmd_addr);
		return ret;
	}
	/* Set data mode and select MMD device */
	ret = Ethernet_WritePhyReg(phy_id, FEPHY_REG_ADDR_13, RTL8201FR_MACR_FUNC_ADDR(PHY_MMD_DATA_NO_POST_INC) | mmd_dev);
	if (ret != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to set data mode. PHY(%d), DEV(%d)", phy_id, mmd_dev);
		return ret;
	}
	/* Read data from MMD register */
	ret = Ethernet_ReadPhyReg(phy_id, FEPHY_REG_ADDR_14, &data);
	if (ret != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to read data from MMD register. PHY(%d)", phy_id);
		return ret;
	}

	if (p_data) {
		*p_data = data;
	}
	return RTK_SUCCESS;
}

int PHY_EnableEEE(uint8_t phy_id, u32 status)
{
	int ret = 0;

	/* Reset PHY and select page 0 */
	PHY_SoftWareReset(phy_id);

	if (status != DISABLE) {
		/* Enable 100BASE-TX EEE capability */
		ret = PHY_SetMmdReg(phy_id, RTL8201FR_MMD_DEV_EEE_ADV, RTL8201FR_MMD_REG_EEEAR, RTL8201FR_EEEAR_100BT_EEE_ADV(1));
		if (ret != RTK_SUCCESS) {
			return ret;
		}

		/* Restart auto-negotiation */
		ret = PHY_RestartAutoNego(phy_id);
		if (ret != RTK_SUCCESS) {
			return ret;
		}
	} else {
		/* Disable 100BASE-TX EEE capability */
		ret = PHY_SetMmdReg(phy_id, RTL8201FR_MMD_DEV_EEE_ADV, RTL8201FR_MMD_REG_EEEAR, RTL8201FR_EEEAR_100BT_EEE_ADV(0));
		if (ret != RTK_SUCCESS) {
			return ret;
		}

		/* Reset PHY, enable and restart auto-negotiation */
		ret = Ethernet_WritePhyReg(phy_id, FEPHY_REG_ADDR_0, PHY_RESET | EN_AUTO_GEN | RESTART_AUTO_GEN);
		if (ret != RTK_SUCCESS) {
			return ret;
		}
		DelayMs(20);
	}

	return RTK_SUCCESS;
}

int PHY_SetEEEMode(uint8_t phy_id, uint8_t eth_mode)
{
	int ret = 0;

	/* Reset PHY and select page 0 */
	PHY_SoftWareReset(phy_id);

	/* Select MAC/PHY mode configuration page */
	ret = Ethernet_WritePhyReg(phy_id, FEPHY_REG_ADDR_31, RTL8211F_PHY_MODE_PAGE);
	if (ret != RTK_SUCCESS) {
		return ret;
	}

	if (eth_mode == ETH_MAC_MODE) {
		ret = Ethernet_WritePhyReg(phy_id, RTL8211F_PHYCR2_ADDR, RTL8211F_PHCR2_MAC_MODE);
		if (ret != RTK_SUCCESS) {
			return ret;
		}
	} else {
		ret = Ethernet_WritePhyReg(phy_id, RTL8211F_PHYCR2_ADDR, RTL8211F_PHCR2_PHY_MODE);
		if (ret != RTK_SUCCESS) {
			return ret;
		}
	}

	return RTK_SUCCESS;
}

int PHY_SetRefclkDir(uint8_t phy_id, u32 mac_dir)
{
	int ret = 0;
	uint16_t data = 0;

	/* Select page 7 */
	ret = Ethernet_WritePhyReg(phy_id, FEPHY_REG_ADDR_31, FEPHY_REG_PAGE_7);
	if (ret != RTK_SUCCESS) {
		return ret;
	}

	/* Read RMII mode register */
	ret = Ethernet_ReadPhyReg(phy_id, RTL8201FR_PAGE7_REG_RMII_MODE_SET, &data);
	if (ret != RTK_SUCCESS) {
		return ret;
	}

	if (mac_dir == ETH_REFCLK_ON) {
		data |= RTL8201FR_P7_16_RMII_CLK_DIR;
	} else {
		data &= ~RTL8201FR_P7_16_RMII_CLK_DIR;
	}

	/* Write clock direction configuration back */
	ret = Ethernet_WritePhyReg(phy_id, RTL8201FR_PAGE7_REG_RMII_MODE_SET, data);
	if (ret != RTK_SUCCESS) {
		return ret;
	}

	return RTK_SUCCESS;
}