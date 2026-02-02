/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _AMEBA_PHY_H_
#define _AMEBA_PHY_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file eth_phy.h
 * @brief Standard IEEE 802.3 Clause 22 PHY Register Definitions
 * @author Realtek Semiconductor Corp.
 */
/* ========================================================================== */
/*                        Basic Register Set (0-31)                           */
/* ========================================================================== */

#define MII_BMCR            0x00    /**< Basic Mode Control Register */
#define MII_BMSR            0x01    /**< Basic Mode Status Register */
#define MII_PHYSID1         0x02    /**< PHY Identifier 1 */
#define MII_PHYSID2         0x03    /**< PHY Identifier 2 */
#define MII_ADVERTISE       0x04    /**< Auto-Negotiation Advertisement */
#define MII_LPA             0x05    /**< Auto-Neg Link Partner Ability */
#define MII_EXPANSION       0x06    /**< Auto-Neg Expansion Register */
#define MII_NEXTPAGE        0x07    /**< Auto-Neg Next Page Transmit */
#define MII_LPNEXTPAGE      0x08    /**< Auto-Neg Link Partner Next Page */
#define MII_CTRL1000        0x09    /**< 1000BASE-T Control (Gigabit PHY only) */
#define MII_STAT1000        0x0A    /**< 1000BASE-T Status (Gigabit PHY only) */
#define MII_ESTATUS         0x0F    /**< Extended Status */
#define MII_MMD_CTRL        0x0D    /**< MMD Access Control (For indirect access) */
#define MII_MMD_DATA        0x0E    /**< MMD Access Data */

/* ========================================================================== */
/*                        Register Bit Definitions                            */
/* ========================================================================== */

/* --- BMCR: Basic Mode Control Register (0x00) --- */
#define BMCR_RESET          (1 << 15)   /**< 1=Software Reset */
#define BMCR_LOOPBACK       (1 << 14)   /**< 1=Loopback enabled */
#define BMCR_SPEED100       (1 << 13)   /**< 1=100Mbps, 0=10Mbps (ignored if AN enabled) */
#define BMCR_ANENABLE       (1 << 12)   /**< 1=Auto-Negotiation Enabled */
#define BMCR_PDOWN          (1 << 11)   /**< 1=Power Down */
#define BMCR_ISOLATE        (1 << 10)   /**< 1=Isolate PHY from MII */
#define BMCR_ANRESTART      (1 << 9)    /**< 1=Restart Auto-Negotiation */
#define BMCR_FULLDPLX       (1 << 8)    /**< 1=Full Duplex, 0=Half Duplex */
#define BMCR_CTST           (1 << 7)    /**< 1=Collision Test */
#define BMCR_SPEED1000      (1 << 6)    /**< 1=1000Mbps (MSB of Speed select) */

/* --- BMSR: Basic Mode Status Register (0x01) --- */
#define BMSR_100T4          (1 << 15)   /**< 100BASE-T4 Capable */
#define BMSR_100FULL        (1 << 14)   /**< 100BASE-TX Full Duplex Capable */
#define BMSR_100HALF        (1 << 13)   /**< 100BASE-TX Half Duplex Capable */
#define BMSR_10FULL         (1 << 12)   /**< 10BASE-T Full Duplex Capable */
#define BMSR_10HALF         (1 << 11)   /**< 10BASE-T Half Duplex Capable */
#define BMSR_ESTATEN        (1 << 8)    /**< Extended Status Support */
#define BMSR_ANEGCOMPLETE   (1 << 5)    /**< 1=Auto-Negotiation Complete */
#define BMSR_RFAULT         (1 << 4)    /**< Remote Fault */
#define BMSR_ANEGCAPABLE    (1 << 3)    /**< Auto-Negotiation Capable */
#define BMSR_LSTATUS        (1 << 2)    /**< Link Status (1=Up, 0=Down) */
#define BMSR_JABBER         (1 << 1)    /**< Jabber Detected */
#define BMSR_ERCAP          (1 << 0)    /**< Extended Register Capable */

/* --- ADVERTISE: Auto-Neg Advertisement (0x04) --- */
#define ADVERTISE_NPAGE     (1 << 15)   /**< Next Page */
#define ADVERTISE_RFAULT    (1 << 13)   /**< Remote Fault */
#define ADVERTISE_PAUSE_CAP (1 << 10)   /**< Pause Capable */
#define ADVERTISE_PAUSE_ASYM (1 << 11)  /**< Asymmetric Pause Capable */
#define ADVERTISE_100T4     (1 << 9)    /**< 100BASE-T4 */
#define ADVERTISE_100FULL   (1 << 8)    /**< 100BASE-TX Full Duplex */
#define ADVERTISE_100HALF   (1 << 7)    /**< 100BASE-TX Half Duplex */
#define ADVERTISE_10FULL    (1 << 6)    /**< 10BASE-T Full Duplex */
#define ADVERTISE_10HALF    (1 << 5)    /**< 10BASE-T Half Duplex */
#define ADVERTISE_CSMA      (1 << 0)    /**< Selector Field (00001=IEEE 802.3) */

#define ADVERTISE_ALL       (ADVERTISE_10HALF | ADVERTISE_10FULL | \
                             ADVERTISE_100HALF | ADVERTISE_100FULL)

/* --- LPA: Link Partner Ability (0x05) --- */
/* Same bit definitions as ADVERTISE register */
#define LPA_NPAGE           (1 << 15)
#define LPA_RFAULT          (1 << 13)
#define LPA_PAUSE_CAP       (1 << 10)
#define LPA_PAUSE_ASYM      (1 << 11)
#define LPA_100T4           (1 << 9)
#define LPA_100FULL         (1 << 8)
#define LPA_100HALF         (1 << 7)
#define LPA_10FULL          (1 << 6)
#define LPA_10HALF          (1 << 5)
#define LPA_LPACK           (1 << 14)   /**< Link Partner Acknowledge */

/* --- CTRL1000: 1000BASE-T Control (0x09) --- */
#define CTL1000_TEST_MASK   (7 << 13)   /**< Test Mode mask */
#define CTL1000_AS_MASTER   (1 << 12)   /**< Force Master Configuration */
#define CTL1000_ENABLE_MS   (1 << 11)   /**< Master/Slave Manual Configuration Enable */
#define CTL1000_ADVERT_FULL (1 << 9)    /**< Advertise 1000BASE-T Full Duplex */
#define CTL1000_ADVERT_HALF (1 << 8)    /**< Advertise 1000BASE-T Half Duplex */

/* --- STAT1000: 1000BASE-T Status (0x0A) --- */
#define STAT1000_MS_FAULT   (1 << 15)   /**< Master/Slave Configuration Fault */
#define STAT1000_MS_RES     (1 << 14)   /**< Master/Slave Configuration Resolution */
#define STAT1000_LPA_FULL   (1 << 11)   /**< Link Partner 1000BASE-T Full Duplex */
#define STAT1000_LPA_HALF   (1 << 10)   /**< Link Partner 1000BASE-T Half Duplex */
#define STAT1000_IDLE_ERR   (0xFF << 0) /**< Idle Error Count */

/* --- ESTATUS: Extended Status (0x0F) --- */
#define ESTATUS_1000_XFULL  (1 << 15)   /**< 1000BASE-X Full Duplex */
#define ESTATUS_1000_XHALF  (1 << 14)   /**< 1000BASE-X Half Duplex */
#define ESTATUS_1000_TFULL  (1 << 13)   /**< 1000BASE-T Full Duplex */
#define ESTATUS_1000_THALF  (1 << 12)   /**< 1000BASE-T Half Duplex */

/* ========================================================================== */
/*                     MMD (MDIO Manageable Device) IDs                       */
/* ========================================================================== */
/* Used for accessing EEE registers and other extended features */
#define MMD_PMA_PMD         1
#define MMD_WIS             2
#define MMD_PCS             3
#define MMD_PHY_XS          4
#define MMD_DTE_XS          5
#define MMD_AN              7   /**< Auto-Negotiation (EEE Advertisement here) */
#define MMD_VEND1           30
#define MMD_VEND2           31
#define MMD_EEEAR           60  /**< MMD EEE Advertisement Register (Clause 45, Dev 7, Reg 60) */

/* --- MMD Access Control Bits (Reg 0x0D) --- */
#define MMD_FUNC_ADDR       (0x0000)    /**< Function: Address */
#define MMD_FUNC_DATA       (0x4000)    /**< Function: Data, no post increment */
#define MMD_FUNC_DATA_INC   (0x8000)    /**< Function: Data, post increment */
#define MMD_FUNC_DATA_INC_WR (0xC000)   /**< Function: Data, post inc on write only */

/* --- EEE Advertisement (MMD 7, Reg 60) --- */
#define EEE_ADVERT_100      (1 << 1)    /**< Advertise 100BASE-TX EEE */
#define EEE_ADVERT_1000     (1 << 2)    /**< Advertise 1000BASE-T EEE */


/* --- Private definitions for new phy --- */

extern const struct eth_phy_ops phy_rtl8201fr_ops;

#ifdef __cplusplus
}
#endif

#endif /* _AMEBA_PHY_H_ */