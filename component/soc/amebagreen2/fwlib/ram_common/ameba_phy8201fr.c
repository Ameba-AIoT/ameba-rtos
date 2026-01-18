/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *const TAG = "PHY";


/* ========================================================================== */
/*                    RTL8201 Specific Definitions                            */
/* ========================================================================== */
/*
 * Page Selection for RTL8201FR.
 * Uses Register 31 to switch pages.
 * Values 0 and 7 are specific page indices for this PHY.
 */
#define RTL8201_REG_PAGESEL     31          /**< Page Select Register */
#define RTL8201_PAGE_0          0           /**< Page 0 (Standard Registers) */
#define RTL8201_PAGE_7          7           /**< Page 7 (Extension Registers) */

#define RTL8201_P7_RMII_MODE    16          /**< RMII Mode configuration register (on Page 7) */
#define RTL8201_P7_RMII_CLK_DIR (1 << 12)   /**< RMII Clock Direction bit */

/* RTL8201FR PHY ID (OUI + Model) */
#define RTL8201_PHY_ID_MATCH    0x001CC816

/* ========================================================================== */
/*                    Driver Implementation                                   */
/* ========================================================================== */

/**
 * @brief      Read from RTL8201FR PHY Register (Clause 22)
 */
static int phy_rtl8201fr_read(struct eth_phy_dev *phy, uint32_t reg, uint16_t *val)
{
	if (!phy || !phy->bus || !phy->bus->mdio_read) {
		return RTK_FAIL;
	}
	return phy->bus->mdio_read(phy->addr, reg, val);
}

/**
 * @brief      Write to RTL8201FR PHY Register (Clause 22)
 */
static int phy_rtl8201fr_write(struct eth_phy_dev *phy, uint32_t reg, uint16_t val)
{
	if (!phy || !phy->bus || !phy->bus->mdio_write) {
		return RTK_FAIL;
	}
	return phy->bus->mdio_write(phy->addr, reg, val);
}

/* ========================================================================== */
/*                Safe Access Helpers (Error Latching)                        */
/* ========================================================================== */

/**
 * @brief      Safe Read from RTL8201FR PHY Register (Error Latching)
 *
 * This function performs a read operation only if the previous error state
 * (*err) is RTK_SUCCESS. If an error occurs during this read, the *err
 * variable is updated. This allows for cleaner sequential logic without
 * checking return codes after every line.
 *
 * @param[in]  phy       Pointer to the Ethernet PHY device structure.
 * @param[in]  reg       Register address to read.
 * @param[in,out] err    Pointer to the cumulative error code.
 *
 * @return     The read value (uint16_t). Returns 0 if an error occurred
 *             or if *err was already set.
 */
static uint16_t phy_rtl8201fr_read_safe(struct eth_phy_dev *phy, uint32_t reg, int *err)
{
	uint16_t val = 0;
	int ret;

	/* If a previous error occurred, skip execution */
	if (*err != RTK_SUCCESS) {
		return 0;
	}

	ret = phy_rtl8201fr_read(phy, reg, &val);

	if (ret != RTK_SUCCESS) {
		*err = ret;
		return 0;
	}

	return val;
}

/**
 * @brief      Safe Write to RTL8201FR PHY Register (Error Latching)
 *
 * This function performs a write operation only if the previous error state
 * (*err) is RTK_SUCCESS. If an error occurs during this write, the *err
 * variable is updated.
 *
 * @param[in]  phy       Pointer to the Ethernet PHY device structure.
 * @param[in]  reg       Register address to write.
 * @param[in]  val       Data value to write.
 * @param[in,out] err    Pointer to the cumulative error code.
 */
static void phy_rtl8201fr_write_safe(struct eth_phy_dev *phy, uint32_t reg, uint16_t val, int *err)
{
	int ret;

	/* If a previous error occurred, skip execution */
	if (*err != RTK_SUCCESS) {
		return;
	}

	ret = phy_rtl8201fr_write(phy, reg, val);

	if (ret != RTK_SUCCESS) {
		*err = ret;
	}
}

/**
 * @brief      Safe Write to RTL8201FR MMD Register (Error Latching)
 *
 * Indirectly writes to an MMD register using the Clause 45 over Clause 22 mechanism.
 *
 * Sequence:
 * 1. Switch to Page 0.
 * 2. Set MMD Function to ADDRESS and select Device.
 * 3. Write the Register Address to Reg 14.
 * 4. Set MMD Function to DATA (No Post Increment) and select Device.
 * 5. Write the Data to Reg 14.
 *
 * @param[in]  phy       Pointer to the Ethernet PHY device structure.
 * @param[in]  dev_addr  MMD Device Address (e.g., MMD_AN).
 * @param[in]  reg_addr  MMD Register Address.
 * @param[in]  data      Data to write.
 * @param[in,out] err    Pointer to the cumulative error code.
 */
static void phy_rtl8201fr_mmd_write_safe(struct eth_phy_dev *phy, uint8_t dev_addr,
		uint16_t reg_addr, uint16_t data, int *err)
{
	/* 1. Select Page 0 */
	phy_rtl8201fr_write_safe(phy, RTL8201_REG_PAGESEL, RTL8201_PAGE_0, err);

	/* 2. Set Address Mode (Function = 00) and Select Device */
	phy_rtl8201fr_write_safe(phy, MII_MMD_CTRL, MMD_FUNC_ADDR | dev_addr, err);

	/* 3. Write Address Value */
	phy_rtl8201fr_write_safe(phy, MII_MMD_DATA, reg_addr, err);

	/* 4. Set Data Mode (Function = 01, No Post Inc) and Select Device */
	phy_rtl8201fr_write_safe(phy, MII_MMD_CTRL, MMD_FUNC_DATA | dev_addr, err);

	/* 5. Write Data */
	phy_rtl8201fr_write_safe(phy, MII_MMD_DATA, data, err);
}

/**
 * @brief      Safe Read from RTL8201FR MMD Register (Error Latching)
 *
 * Indirectly reads from an MMD register using the Clause 45 over Clause 22 mechanism.
 *
 * Sequence:
 * 1. Switch to Page 0.
 * 2. Set MMD Function to ADDRESS and select Device.
 * 3. Write the Register Address to Reg 14.
 * 4. Set MMD Function to DATA (No Post Increment) and select Device.
 * 5. Read the Data from Reg 14.
 *
 * @param[in]  phy       Pointer to the Ethernet PHY device structure.
 * @param[in]  dev_addr  MMD Device Address (e.g., MMD_AN).
 * @param[in]  reg_addr  MMD Register Address.
 * @param[in,out] err    Pointer to the cumulative error code.
 *
 * @return     The read value. Returns 0 if an error occurred.
 */
static uint16_t phy_rtl8201fr_mmd_read_safe(struct eth_phy_dev *phy, uint8_t dev_addr,
		uint16_t reg_addr, int *err)
{
	/* 1. Select Page 0 */
	phy_rtl8201fr_write_safe(phy, RTL8201_REG_PAGESEL, RTL8201_PAGE_0, err);

	/* 2. Set Address Mode (Function = 00) and Select Device */
	phy_rtl8201fr_write_safe(phy, MII_MMD_CTRL, MMD_FUNC_ADDR | dev_addr, err);

	/* 3. Write Address Value */
	phy_rtl8201fr_write_safe(phy, MII_MMD_DATA, reg_addr, err);

	/* 4. Set Data Mode (Function = 01, No Post Inc) and Select Device */
	phy_rtl8201fr_write_safe(phy, MII_MMD_CTRL, MMD_FUNC_DATA | dev_addr, err);

	/* 5. Read Data */
	return phy_rtl8201fr_read_safe(phy, MII_MMD_DATA, err);
}

/**
 * @brief      Parse Link Status from Registers
 *
 * Calculates the actual speed and duplex based on BMCR (if forced)
 * or LPA/Advertise (if Auto-Negotiation occurred).
 *
 * @param[in]  bmsr      Basic Mode Status Register value.
 * @param[in]  bmcr      Basic Mode Control Register value.
 * @param[in]  lpa       Link Partner Ability Register value.
 * @param[in]  adv       Auto-Negotiation Advertisement Register value.
 * @param[out] state     Pointer to the link state structure to populate.
 */
static void phy_rtl8201_parse_link_status(uint16_t bmsr, uint16_t bmcr, uint16_t lpa,
		uint16_t adv, phy_link_state_t *state)
{
	/* Auto-Negotiation enabled: Calculate intersection of LPA and Advertisement */
	uint16_t common = lpa & adv;
	/* Check Link Status bit in BMSR */
	state->link_up = (bmsr & BMSR_LSTATUS) ? true : false;

	if (!state->link_up) {
		state->speed = 0;
		state->duplex = 0;
		return;
	}

	/* If Auto-Negotiation is disabled, read forced configuration from BMCR */
	if (!(bmcr & BMCR_ANENABLE)) {
		state->speed = (bmcr & BMCR_SPEED100) ? PHY_SPEED_100M : PHY_SPEED_10M;
		state->duplex = (bmcr & BMCR_FULLDPLX) ? PHY_DUPLEX_FULL : PHY_DUPLEX_HALF;
		return;
	}

	if (common & ADVERTISE_100FULL) {
		state->speed = PHY_SPEED_100M;
		state->duplex = PHY_DUPLEX_FULL;
	} else if (common & ADVERTISE_100HALF) {
		state->speed = PHY_SPEED_100M;
		state->duplex = PHY_DUPLEX_HALF;
	} else if (common & ADVERTISE_10FULL) {
		state->speed = 	PHY_SPEED_10M;
		state->duplex = PHY_DUPLEX_FULL;
	} else {
		state->speed = 	PHY_SPEED_10M;
		state->duplex = PHY_DUPLEX_HALF;
	}
}

/* ========================================================================== */
/*                Public API                        						  */
/* ========================================================================== */

/**
 * @brief      Initialize RTL8201FR PHY
 *
 * Verifies the PHY ID and performs a software reset.
 *
 * Sequence:
 * 1. Read PHYSID1 and PHYSID2.
 * 2. Verify against RTL8201 model ID.
 * 3. Call internal reset function.
 *
 * @param[in]  phy       Pointer to the Ethernet PHY device structure.
 *
 * @return     RTK_SUCCESS on success, error code otherwise.
 */
static int phy_rtl8201_init(struct eth_phy_dev *phy)
{
	int err = RTK_SUCCESS;
	uint16_t id1, id2;
	uint32_t phy_id;

	if (!phy || !phy->ops || !phy->ops->reset) {
		return RTK_FAIL;
	}

	/* 1. Read PHY Identifier */
	/* Select Page 0 */
	phy_rtl8201fr_write_safe(phy, RTL8201_REG_PAGESEL, RTL8201_PAGE_0, &err);

	id1 = phy_rtl8201fr_read_safe(phy, MII_PHYSID1, &err);
	id2 = phy_rtl8201fr_read_safe(phy, MII_PHYSID2, &err);

	if (err != RTK_SUCCESS) {
		return err;
	}

	/* 2. Verify ID */
	phy_id = ((uint32_t)id1 << 16) | id2;
	if (phy_id  != RTL8201_PHY_ID_MATCH) {
		RTK_LOGE(TAG, "RTL8201 ID Mismatch. Read: 0x%08X", phy_id);
		return RTK_FAIL;
	}

	/* 3. Perform Software Reset */
	return phy->ops->reset(phy);
}

/**
 * @brief      Perform Software Reset
 *
 * Writes the RESET bit to BMCR and waits for completion.
 *
 * Sequence:
 * 1. Select Page 0.
 * 2. Write BMCR_RESET bit.
 * 3. Delay for hardware stabilization.
 *
 * @param[in]  phy       Pointer to the Ethernet PHY device structure.
 *
 * @return     RTK_SUCCESS on success, error code otherwise.
 */
static int phy_rtl8201_reset(struct eth_phy_dev *phy)
{
	int err = RTK_SUCCESS;

	/* Select Page 0 */
	phy_rtl8201fr_write_safe(phy, RTL8201_REG_PAGESEL, RTL8201_PAGE_0, &err);

	/* Write Reset Bit */
	phy_rtl8201fr_write_safe(phy, MII_BMCR, BMCR_RESET, &err);

	if (err == RTK_SUCCESS) {
		/* Wait for reset to complete (Datasheet requirement: ~10-20ms) */
		DelayMs(20);
	}
	return err;
}

/**
 * @brief      Configure Link Parameters
 *
 * Sets up Auto-Negotiation advertisement or forces speed/duplex.
 *
 * Logic:
 * - If autoneg_en is true: Configure ADVERTISE register and restart AN.
 * - If autoneg_en is false: Force SPEED/DUPLEX bits in BMCR.
 *
 * @param[in]  phy       Pointer to the Ethernet PHY device structure.
 * @param[in]  cfg       Pointer to link configuration structure.
 *
 * @return     RTK_SUCCESS on success, error code otherwise.
 */
static int phy_rtl8201_cfg_link(struct eth_phy_dev *phy, const phy_link_config_t *cfg)
{
	int err = RTK_SUCCESS;
	uint16_t bmcr = 0;
	uint16_t advertise = 0;

	/* Select Page 0 */
	phy_rtl8201fr_write_safe(phy, RTL8201_REG_PAGESEL, RTL8201_PAGE_0, &err);

	if (cfg->autoneg_en) {
		/* --- Auto-Negotiation Mode --- */
		advertise = ADVERTISE_CSMA; /* IEEE 802.3 Selector */

		if (cfg->advertise_100m) {
			advertise |= (ADVERTISE_100FULL | ADVERTISE_100HALF);
		}
		if (cfg->advertise_10m) {
			advertise |= (ADVERTISE_10FULL | ADVERTISE_10HALF);
		}

		/* Update Advertisement Register */
		phy_rtl8201fr_write_safe(phy, MII_ADVERTISE, advertise, &err);

		/* Configure BMCR to Enable and Restart AN */
		bmcr = BMCR_ANENABLE | BMCR_ANRESTART;
	} else {
		/* --- Forced Mode --- */
		if (cfg->speed == PHY_SPEED_100M) {
			bmcr |= BMCR_SPEED100;
		}
		if (cfg->duplex == PHY_DUPLEX_FULL) {
			bmcr |= BMCR_FULLDPLX;
		}
		/* Note: BMCR_ANENABLE is 0 here */
	}

	/* Write to BMCR */
	phy_rtl8201fr_write_safe(phy, MII_BMCR, bmcr, &err);

	return err;
}

/**
 * @brief      Get Current Link Status
 *
 * Reads BMSR (twice for latching) and resolves speed/duplex.
 *
 * Sequence:
 * 1. Read BMSR twice to clear Latching Low status.
 * 2. Read BMCR, LPA, and ADVERTISE registers.
 * 3. Parse values to determine actual link state.
 *
 * @param[in]  phy       Pointer to the Ethernet PHY device structure.
 * @param[out] state     Pointer to link state structure.
 *
 * @return     RTK_SUCCESS on success, error code otherwise.
 */
static int phy_rtl8201_get_link(struct eth_phy_dev *phy, phy_link_state_t *state)
{
	int err = RTK_SUCCESS;
	uint16_t bmsr, bmcr, lpa, adv;

	if (!state) {
		return RTK_FAIL;
	}

	/* Select Page 0 */
	phy_rtl8201fr_write_safe(phy, RTL8201_REG_PAGESEL, RTL8201_PAGE_0, &err);

	/* Read BMSR twice to clear Latching Low status */
	phy_rtl8201fr_read_safe(phy, MII_BMSR, &err);
	bmsr = phy_rtl8201fr_read_safe(phy, MII_BMSR, &err);

	/* Read Control and Ability registers for resolution */
	bmcr = phy_rtl8201fr_read_safe(phy, MII_BMCR, &err);
	lpa  = phy_rtl8201fr_read_safe(phy, MII_LPA, &err);
	adv  = phy_rtl8201fr_read_safe(phy, MII_ADVERTISE, &err);

	if (err != RTK_SUCCESS) {
		return err;
	}

	/* Parse register values into abstract state */
	phy_rtl8201_parse_link_status(bmsr, bmcr, lpa, adv, state);

	return RTK_SUCCESS;
}

/**
 * @brief      Restart Auto-Negotiation
 *
 * Sets the RESTART_AN bit in BMCR.
 *
 * @param[in]  phy       Pointer to the Ethernet PHY device structure.
 *
 * @return     RTK_SUCCESS on success, error code otherwise.
 */
static int phy_rtl8201_autoneg_restart(struct eth_phy_dev *phy)
{
	int err = RTK_SUCCESS;
	uint16_t val;

	phy_rtl8201fr_write_safe(phy, RTL8201_REG_PAGESEL, RTL8201_PAGE_0, &err);

	/* Read-Modify-Write BMCR */
	val = phy_rtl8201fr_read_safe(phy, MII_BMCR, &err);
	val |= (BMCR_ANENABLE | BMCR_ANRESTART);
	phy_rtl8201fr_write_safe(phy, MII_BMCR, val, &err);

	return err;
}

/**
 * @brief      Configure PHY Clock Source (RMII Mode)
 *
 * Maps abstract Clock Source to RTL8201 Page 7 RMII settings.
 *
 * Mapping:
 * - PHY_CLK_OUTPUT: PHY drives 50MHz (REFCLK_OUT).
 * - PHY_CLK_INPUT:  PHY receives 50MHz (REFCLK_IN).
 *
 * Sequence:
 * 1. Switch to Page 7.
 * 2. Modify RMII Mode Register.
 * 3. Restore Page 0.
 *
 * @param[in]  phy       Pointer to the Ethernet PHY device structure.
 * @param[in]  src       Clock source configuration.
 *
 * @return     RTK_SUCCESS on success, error code otherwise.
 */
static int phy_rtl8201_cfg_clock(struct eth_phy_dev *phy, phy_clock_source_t src)
{
	int err = RTK_SUCCESS;
	uint16_t val;

	/* 1. Switch to Page 7 (Extension Registers) */
	phy_rtl8201fr_write_safe(phy, RTL8201_REG_PAGESEL, RTL8201_PAGE_7, &err);

	/* 2. Read RMII Mode Register */
	val = phy_rtl8201fr_read_safe(phy, RTL8201_P7_RMII_MODE, &err);

	if (err == RTK_SUCCESS) {
		if (src != PHY_CLK_XTAL) {
			val |= RTL8201_P7_RMII_CLK_DIR;  /* Set bit: Input Mode */
		} else {
			val &= ~RTL8201_P7_RMII_CLK_DIR; /* Clear bit: Output Mode */
		}
		/* 3. Write Back */
		phy_rtl8201fr_write_safe(phy, RTL8201_P7_RMII_MODE, val, &err);
	}

	/* 4. Restore Page 0 (Critical cleanup) */
	phy_rtl8201fr_write_safe(phy, RTL8201_REG_PAGESEL, RTL8201_PAGE_0, &err);

	return err;
}

/**
 * @brief      Configure Energy Efficient Ethernet (EEE)
 *
 * Enables or disables EEE advertisement via MMD registers.
 *
 * Sequence:
 * 1. Write Advertisement value to MMD 7, Reg 60.
 * 2. Restart Auto-Negotiation to update capability.
 *
 * @param[in]  phy       Pointer to the Ethernet PHY device structure.
 * @param[in]  mode      Enable or Disable EEE.
 *
 * @return     RTK_SUCCESS on success, error code otherwise.
 */
static int phy_rtl8201_cfg_eee(struct eth_phy_dev *phy, phy_eee_mode_t mode)
{
	int err = RTK_SUCCESS;
	uint16_t adv_val = 0;

	/* Prepare Advertisement Value (Only 100BASE-TX supported for EEE on 8201) */
	if (mode == PHY_EEE_ENABLE_AN) {
		adv_val = EEE_ADVERT_100;
	}

	/* 1. Write EEE Advertisement to MMD (Clause 45 via Clause 22) */
	/* Device 7 (MMD_AN), Register 60 (MMD_EEEAR) */
	phy_rtl8201fr_mmd_write_safe(phy, MMD_AN, MMD_EEEAR, adv_val, &err);
	phy_rtl8201fr_mmd_read_safe(phy, MMD_AN, MMD_EEEAR, &err);
	/* 2. Restart Auto-Negotiation to advertise new capabilities */
	if (err == RTK_SUCCESS) {
		phy_rtl8201_autoneg_restart(phy);
	}

	return err;
}

/**
 * @brief      Enable/Disable Digital Loopback
 *
 * Sets or clears the LOOPBACK bit in BMCR.
 *
 * @param[in]  phy       Pointer to the Ethernet PHY device structure.
 * @param[in]  enable    True to enable loopback, False to disable.
 *
 * @return     RTK_SUCCESS on success, error code otherwise.
 */
static int phy_rtl8201_set_loopback(struct eth_phy_dev *phy, bool enable)
{
	int err = RTK_SUCCESS;
	uint16_t val;

	phy_rtl8201fr_write_safe(phy, RTL8201_REG_PAGESEL, RTL8201_PAGE_0, &err);

	val = phy_rtl8201fr_read_safe(phy, MII_BMCR, &err);

	if (enable) {
		val |= BMCR_LOOPBACK;
	} else {
		val &= ~BMCR_LOOPBACK;
	}

	phy_rtl8201fr_write_safe(phy, MII_BMCR, val, &err);

	return err;
}

/**
 * @brief      Register Link State Callback
 *
 * Stores the callback function pointer for link status updates.
 *
 * @param[in]  phy       Pointer to the Ethernet PHY device structure.
 * @param[in]  cb        Callback function pointer.
 * @param[in]  user_data User data to pass to the callback.
 *
 * @return     RTK_SUCCESS on success.
 */
static int phy_rtl8201_set_link_callback(struct eth_phy_dev *phy, phy_link_cb_t cb, void *user_data)
{
	if (!phy) {
		return RTK_FAIL;
	}
	UNUSED(cb);
	UNUSED(user_data);
	/* Assuming phy structure has these fields available */
	/* phy->cb_link_state = cb; */
	/* phy->cb_user_data = user_data; */
	// phy_rtl8201_get_link(phy, NULL); //dummy to avoid warning
	return RTK_SUCCESS;
}
extern const struct eth_mdio_ops eth_mdio_bus;

const struct eth_phy_ops phy_rtl8201fr_ops = {
	.init               = phy_rtl8201_init,
	.reset              = phy_rtl8201_reset,
	.cfg_link           = phy_rtl8201_cfg_link,
	.get_link           = phy_rtl8201_get_link,
	.autoneg_restart    = phy_rtl8201_autoneg_restart,
	.cfg_clock          = phy_rtl8201_cfg_clock,
	.cfg_eee            = phy_rtl8201_cfg_eee,
	.set_loopback       = phy_rtl8201_set_loopback,
	.set_link_callback  = phy_rtl8201_set_link_callback,
};