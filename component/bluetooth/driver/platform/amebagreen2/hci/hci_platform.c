/*
 * Copyright (c) 2025 Realtek Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "osif.h"
#include "hci_controller.h"
#include "hci_uart.h"
#include "hci_platform.h"
#include "bt_debug.h"
#include "ameba.h"
#include "platform_autoconf.h"
#if defined(CONFIG_WLAN) && CONFIG_WLAN
#include "wifi_api.h"
extern int wifi_set_ips_internal(u8 enable);
#endif
#if defined(CONFIG_BT_COEXIST)
#include "rtw_coex_host_api.h"
#else
//#error "Please Enable Coexist!!!"
#endif

#define HCI_PHY_EFUSE_OFFSET       0x740
#define HCI_PHY_EFUSE_LEN          0x70
#define HCI_LGC_EFUSE_OFFSET       0x1B0
#define HCI_LGC_EFUSE_LEN          0x50
#define HCI_MAC_ADDR_LEN           6
#define HCI_CONFIG_SIGNATURE       0x8723ab55
#define HCI_CONFIG_HDR_LEN         6

#define HCI_CFG_BAUDRATE           BIT0
#define HCI_CFG_FLOWCONTROL        BIT1
#define HCI_CFG_BD_ADDR            BIT2
#define LEFUSE(x)                  ((x)-HCI_LGC_EFUSE_OFFSET)
#define PEFUSE(x)                  ((x)-HCI_PHY_EFUSE_OFFSET)

#define DBG_BT_VENDOR              0
#define DBG_BT_ON                  1

static uint8_t hci_phy_efuse[HCI_PHY_EFUSE_LEN]  = {0};
static uint8_t hci_lgc_efuse[HCI_LGC_EFUSE_LEN]  = {0};
unsigned char hci_init_config[] = {
	/* Header */
	0x55, 0xAB, 0x23, 0x87,

	/* Length */
	0x40, 0x00,

	/* BT MAC Address */
	0x30, 0x00, 0x06, 0x8A, 0xD5, 0x23, 0x4C, 0xE0, 0x00,

	/* LOG Uart Baudrate 115200 */
	0x08, 0x00, 0x04, 0x00, 0xC2, 0x01, 0x00,

	/* HCI Uart Baudrate 3M */
	0x0C, 0x00, 0x04, 0x01, 0x80, 0x92, 0x04,

	/* HCI Uart Flow Control */
	0x18, 0x00, 0x01, 0x5C,

	/* BT Wake Host */
	0x42, 0x00, 0x01, 0x09,

	/* Host Wake BT */
	0x43, 0x00, 0x01, 0x07,

	/* RF(unit:0.5dbm): phy_flow_ctrl_para(1), module_txgaink(1), module_txgain_flatk(4), lbt_threshold_ant_out(1), lbt_ant_gain(1) */
	0x78, 0x02, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00,

	/* RF(unit:0.5dbm): iqm_max_txpower_LE1M(1), iqm_max_txpower_LE2M(1), iqm_max_txpower_LE1M_adv(1), iqm_max_txpower_LE2M_adv(1), iqm_max_txpower_LR(1)*/
	0x85, 0x02, 0x05, 0x10, 0x10, 0x10, 0x10, 0x10,

	/* RF(unit:0.5dbm): iqm_max_txpower_LE1M_2402(1), iqm_max_txpower_LE1M_2480(1), iqm_max_txpower_LE2M_2402(1), iqm_max_txpower_LE2M_2480(1)*/
	0x8A, 0x02, 0x04, 0x10, 0x10, 0x10, 0x10
};

static uint8_t  hci_cfg_bd_addr[HCI_MAC_ADDR_LEN] = {0};
static uint32_t hci_cfg_flag                      = 0;

void hci_platform_cfg_bd_addr(uint8_t *bdaddr)
{
	/* bdaddr is little endian, please print in reverse order from high address to low address to confirm habits */
	for (uint8_t i = 0; i < HCI_MAC_ADDR_LEN; i++) {
		hci_cfg_bd_addr[i] = bdaddr[i];
	}

	hci_cfg_flag |= HCI_CFG_BD_ADDR;
}
void hci_platform_bt_rf_calibration(void)
{
#if defined(CONFIG_BT_COEXIST)
	/* DAC DCK */
	if (hci_phy_efuse[PEFUSE(0x741)] != 0xFF) {
		BT_LOGA("hci_phy_efuse[PEFUSE(0x741)] = 0x%x\r\n", hci_phy_efuse[PEFUSE(0x741)]);
	}

	/* 0x741[2] 0 for bt dac dck valid */
	if ((hci_phy_efuse[PEFUSE(0x741)] & BIT2) == 0) {
		struct bt_rfk_param p_temp_pram = {0};

		/*DCK_KOSEN_Q_BT_0dB[5:0] = 0x754[7:2]
		DCK_KOSEN_I_BT_0dB[5:4] = 0x754[1:0]
		DCK_KOSEN_I_BT_0dB[3:0] = 0x753[7:4]
		DCK_DOS_Q_BT_0dB [5:2] = 0x753[3:0]
		DCK_DOS_Q_BT_0dB [1:0] = 0x752[7:6]
		DCK_DOS_I_BT_0dB [5:0] = 0x752[5:0]*/
		p_temp_pram.type = BT_DAC_DCK;
		/* DCK_KOSEN_Q_BT_0dB[5:0] */
		p_temp_pram.rfk_data1 = (hci_phy_efuse[PEFUSE(0x754)] & 0xFC) >> 2;
		/* DCK_DOS_Q_BT_0dB [5:0] */
		p_temp_pram.rfk_data2 = (hci_phy_efuse[PEFUSE(0x752)] & 0xC0) >> 6 | (hci_phy_efuse[PEFUSE(0x753)] & 0x0F) << 2;
		/* DCK_KOSEN_I_BT_0dB[5:0] */
		p_temp_pram.rfk_data3 = (hci_phy_efuse[PEFUSE(0x753)] & 0xF0) >> 4 | (hci_phy_efuse[PEFUSE(0x754)] & 0x03) << 4;
		/* DCK_DOS_I_BT_0dB [5:0] */
		p_temp_pram.rfk_data4 = hci_phy_efuse[PEFUSE(0x752)] & 0x3F;

		rtk_coex_btc_bt_rfk(&p_temp_pram, sizeof(struct bt_rfk_param));
	}

	/* TX LOK */
	/* 0x741[0] 0 for tx lok valid */
	if ((hci_phy_efuse[PEFUSE(0x741)] & BIT0) == 0) {
		struct bt_rfk_param p_temp_pram = {0};
		/*TXMOD_IDAC_IS[4:0] = 0x743[4:0]
		TXMOD_IDAC_QS[4:0] = 0x745[4:0]
		TXMOD_IDAC2_IS[3:0] = 0x744[3:0]
		TXMOD_IDAC2_QS[3:0] = 0x746[3:0]*/
		p_temp_pram.type = BT_LOK;
		/* TXMOD_IDAC_IS[4:0] */
		p_temp_pram.rfk_data1 = hci_phy_efuse[PEFUSE(0x743)] & 0x1F;
		/* TXMOD_IDAC_QS[4:0] */
		p_temp_pram.rfk_data2 = hci_phy_efuse[PEFUSE(0x745)] & 0x1F;
		/* TXMOD_IDAC2_IS[3:0] */
		p_temp_pram.rfk_data3 = hci_phy_efuse[PEFUSE(0x744)] & 0x0F;
		/* TXMOD_IDAC2_QS[3:0] */
		p_temp_pram.rfk_data4 = hci_phy_efuse[PEFUSE(0x746)] & 0x0F;

		rtk_coex_btc_bt_rfk(&p_temp_pram, sizeof(struct bt_rfk_param));
	}

	/* TX LOK Res */
	/* 0x741[4] 0 for tx lok res valid */
	if ((hci_phy_efuse[PEFUSE(0x741)] & BIT4) == 0) {
		struct bt_rfk_param p_temp_pram = {0};
		/*TXMOD_IDAC_BS[2:0] = 0x760[5:3]
		TXMOD_IDAC2_BS[2:0] = 0x760[2:0]*/
		/* LO_resolution */
		p_temp_pram.type = BT_LOK_RES;
		p_temp_pram.rfk_data1 = hci_phy_efuse[PEFUSE(0x760)];

		rtk_coex_btc_bt_rfk(&p_temp_pram, sizeof(struct bt_rfk_param));
	}
#endif
}
void hci_platform_bt_rx_dck(void)
{
#if defined(CONFIG_BT_COEXIST)
	struct bt_rfk_param p_temp_pram = {0};

	p_temp_pram.type = BT_RX_DCK;

	rtk_coex_btc_bt_rfk(&p_temp_pram, sizeof(struct bt_rfk_param));
#endif
}
int hci_platform_get_rx_adck_data(uint8_t *data, uint8_t len)
{
#if defined(CONFIG_BT_COEXIST)
	struct bt_rfk_param p_temp_pram = {0};

	p_temp_pram.type = BT_ADC_DCK;

	rtk_coex_btc_bt_rfk(&p_temp_pram, sizeof(struct bt_rfk_param));

	memcpy(data, &p_temp_pram.rfk_data1, len);

	return HCI_SUCCESS;
#else
	(void) data;
	(void) len;

	return HCI_FAIL;
#endif
}
int hci_platform_get_write_phy_efuse_data(uint8_t *data, uint8_t len)
{
	memcpy(data, hci_phy_efuse, len);
	return HCI_SUCCESS;
}

static uint8_t hci_platform_read_efuse(void)
{
	uint8_t i;

	/* Read Logic Efuse */
	if (RTK_FAIL == OTP_LogicalMap_Read(hci_lgc_efuse, HCI_LGC_EFUSE_OFFSET, HCI_LGC_EFUSE_LEN)) {
		BT_LOGE("OTP_LogicalMap_Read failed\r\n");
		return HCI_FAIL;
	}

	/* Read Physical Efuse */
	for (i = 0; i < HCI_PHY_EFUSE_LEN; i++) {
		OTP_Read8((HCI_PHY_EFUSE_OFFSET + i), (hci_phy_efuse + i));
	}
#if 0
	BT_DUMPA("Read Logic Efuse:\r\n", hci_lgc_efuse, HCI_LGC_EFUSE_LEN);
	BT_DUMPA("Read Phy Efuse:\r\n", hci_phy_efuse, HCI_PHY_EFUSE_LEN);
#endif

	return HCI_SUCCESS;
}

static uint8_t bt_manual_tx_power_gain_enable = 0;
static uint8_t bt_manual_gain_index = 0xFE;

void hci_platform_set_tx_power_gain_index(uint32_t index)
{
	bt_manual_tx_power_gain_enable = 1;
	bt_manual_gain_index = (uint8_t)(index & 0xFF);
}

static uint8_t hci_platform_parse_config(void)
{
	uint8_t *p, i;
	uint16_t entry_offset, entry_len, config_length;
	uint32_t config_header;

	if (sizeof(hci_init_config) <= HCI_CONFIG_HDR_LEN) {
		return HCI_IGNORE;
	}

	p = hci_init_config;
	memcpy((void *)&config_header, (void *)p, sizeof(uint32_t));
	if (HCI_CONFIG_SIGNATURE != config_header) {
		return HCI_FAIL;
	}

	memcpy((void *)&config_length, (void *)(p + 4), sizeof(uint16_t));
	if (config_length != (uint16_t)(sizeof(hci_init_config) - HCI_CONFIG_HDR_LEN)) {
		/* Fix the len, just avoid the length is not corect */
		p[4] = (uint8_t)((sizeof(hci_init_config) - HCI_CONFIG_HDR_LEN) >> 0);
		p[5] = (uint8_t)((sizeof(hci_init_config) - HCI_CONFIG_HDR_LEN) >> 8);
	}

	p += HCI_CONFIG_HDR_LEN;
	while (p < hci_init_config + sizeof(hci_init_config)) {
		memcpy((void *)&entry_offset, (void *)p, sizeof(uint16_t));
		entry_len = *(uint8_t *)(p + 2);
		p += 3;

		switch (entry_offset) {
		case 0x000c:
			hci_set_work_baudrate(p);
			break;
		case 0x0018:
			/* MP Mode, Close Flow Control */
			if (hci_is_mp_mode()) {
				p[0] = p[0] & (~BIT2);
			}
			break;
		case 0x0030:
			/* Set ConfigBuf MacAddr, Use Customer Assign or Efuse */
			if (hci_cfg_flag & HCI_CFG_BD_ADDR) {
				for (i = 0; i < HCI_MAC_ADDR_LEN; i++) {
					p[i] = hci_cfg_bd_addr[i];
				}
			} else {
				if ((hci_lgc_efuse[4] != 0xff) || (hci_lgc_efuse[5] != 0xff) || (hci_lgc_efuse[6] != 0xff) || \
					(hci_lgc_efuse[7] != 0xff) || (hci_lgc_efuse[8] != 0xff) || (hci_lgc_efuse[9] != 0xff)) {
					for (i = 0; i < HCI_MAC_ADDR_LEN; i++) {
						p[i] = hci_lgc_efuse[4 + HCI_MAC_ADDR_LEN - 1 - i];
					}
				}
			}
			BT_LOGA("Bluetooth init BT_ADDR in cfgbuf [%02x:%02x:%02x:%02x:%02x:%02x]\r\n",
					p[5], p[4], p[3], p[2], p[1], p[0]);
			break;
		case 0x0278:
			if (hci_lgc_efuse[LEFUSE(0x1be)] != 0xff) {
				p[0] = hci_lgc_efuse[LEFUSE(0x1be)];
				if (hci_lgc_efuse[LEFUSE(0x1be)] & BIT1) {
					p[1] = hci_lgc_efuse[LEFUSE(0x1bf)];
				}

				if (hci_lgc_efuse[LEFUSE(0x1be)] & BIT2) {
					p[2] = hci_lgc_efuse[LEFUSE(0x1c0)];
					p[3] = hci_lgc_efuse[LEFUSE(0x1c1)];
					p[4] = hci_lgc_efuse[LEFUSE(0x1c2)];
					p[5] = hci_lgc_efuse[LEFUSE(0x1c3)];
				}

				if (hci_lgc_efuse[LEFUSE(0x1be)] & BIT5) {
					p[6] = hci_lgc_efuse[LEFUSE(0x1c4)];
					p[7] = hci_lgc_efuse[LEFUSE(0x1c5)];
				}
			}
			break;
		case 0x0285:
			for (i = 0; i < entry_len; i++) {
				if (bt_manual_tx_power_gain_enable) {
					BT_LOGA("0x285 gain 0x%02x\r\n", bt_manual_gain_index);
					p[i] = bt_manual_gain_index;
				} else {
					if (hci_lgc_efuse[LEFUSE(0x1c7 + i)] != 0xff) {
						p[i] = hci_lgc_efuse[LEFUSE(0x1c7 + i)];
					}
				}
			}
			break;
		case 0x028a:
			for (i = 0; i < entry_len; i++) {
				if (bt_manual_tx_power_gain_enable) {
					BT_LOGA("0x28a gain 0x%02x\r\n", bt_manual_gain_index);
					p[i] = bt_manual_gain_index;
				} else {
					if (hci_lgc_efuse[LEFUSE(0x1cc + i)] != 0xff) {
						p[i] = hci_lgc_efuse[LEFUSE(0x1cc + i)];
					}
				}
			}
			break;
		default:
			break;
		}

		p +=  entry_len;
	}

	return HCI_SUCCESS;
}

static void bt_power_on(void)
{
	set_reg_value(SYSTEM_CTRL_BASE + REG_LSYS_PWC, BIT1 | BIT2, 3);                 /* enable BT Power Cut */
	osif_delay_us(40);
	set_reg_value(SYSTEM_CTRL_BASE + REG_LSYS_PWC, BIT17 | BIT18, 0);               /* disable ISO of BT */
	set_reg_value(SYSTEM_CTRL_BASE + REG_LSYS_FEN_GRP1, BIT2, 1);                   /* enable WL RFAFE control circuit */
	set_reg_value((u32)(&(WLAFE_BASE->WLAFE_ANAPAR_POW_MAC_0)), BIT5 | BIT6, 3);    /* enable RFAFE */
	set_reg_value((u32)(&(WLAFE_BASE->WLAFE_WLRFC_CTRL)), BIT4, 1);                 /* enable WL_CKI_80M_RFC, default=1, can skip */
	set_reg_value((u32)(&(WLAFE_BASE->WLAFE_WLRFC_CTRL)), BIT3, 0);                 /* when WL RFAFE enter power off, keep WLRFC not power off */
	set_reg_value(SYSTEM_CTRL_BASE + REG_LSYS_FEN_GRP1, BIT1, 1);                   /* release BTON reset */
	osif_delay_us(100);
	if (HCI_BT_KEEP_WAKE) {
		set_reg_value(SYSTEM_CTRL_BASE + REG_LSYS_BT_CTRL0, BIT13, 1);              /* enable HOST_WAKE_BT */
	}
}

void bt_power_off(void)
{
	uint8_t retry;
	set_reg_value(SYSTEM_CTRL_BASE + REG_LSYS_BT_CTRL0, BIT13, 1);                  /* enable HOST_WAKE_BT */
	osif_delay(3);
	for (retry = 0; retry < 9; retry++) {
		osif_delay(1);
		if ((HAL_READ32(SYSTEM_CTRL_BASE + REG_LSYS_BT_CTRL1, 0) & (BIT3 | BIT2 | BIT1 | BIT0)) == 4) { /* polling BTPMC_PWON */
			break;
		}
	}

	if (retry >= 9) {
		BT_LOGE("Timeout for waiting BT active before power off\r\n");
	}

	set_reg_value(SYSTEM_CTRL_BASE + REG_LSYS_BT_CTRL0, BIT5, 1);                   /* request poff xtal and swr */
	osif_delay_us(100);
	set_reg_value(SYSTEM_CTRL_BASE + REG_LSYS_BT_CTRL0, BIT5, 0);                   /* disable request poff xtal and swr, for next POFF flow */
	set_reg_value(SYSTEM_CTRL_BASE + REG_LSYS_FEN_GRP1, BIT1, 0);                   /* assert BTON reset */
#if defined(CONFIG_WLAN) && CONFIG_WLAN
	if (!(wifi_is_running(STA_WLAN_INDEX) || wifi_is_running(SOFTAP_WLAN_INDEX)))
#endif
	{
		set_reg_value((u32)(&(WLAFE_BASE->WLAFE_ANAPAR_POW_MAC_0)), BIT5 | BIT6, 0); /* disable RFAFE (if WIFI active, keep 2'b11) */
		set_reg_value(SYSTEM_CTRL_BASE + REG_LSYS_FEN_GRP1, BIT2, 0);                /* disable WL RFAFE control circuit (if WIFI active, keep 1'b1) */
	}
	set_reg_value(SYSTEM_CTRL_BASE + REG_LSYS_PWC, BIT17 | BIT18, 3);                /* enable ISO of BT and PD_SHRM */
	set_reg_value(SYSTEM_CTRL_BASE + REG_LSYS_PWC, BIT1 | BIT2, 0);                  /* disable BT Power Cut & PD_SHRM Power Cut */
	set_reg_value(SYSTEM_CTRL_BASE + REG_LSYS_BT_CTRL0, BIT13, 0);                   /* disable HOST_WAKE_BT */
}

void hci_platform_low_power_setting(void)
{
	set_reg_value(SYSTEM_CTRL_BASE + REG_LSYS_BT_CTRL1, BIT15, 0); /* Suggested by Fiona Shen.switch bt_ana_clk from osc4m to ana_100k when BT enter LPS DD_SEC */
}

void hci_platform_controller_reset(void)
{
	/* Set low power */
	hci_platform_low_power_setting();

	/* BT Controller Power */
	bt_power_on();

	BT_LOGD("BT Reset OK!\r\n");
}

bool rtk_bt_pre_enable(void)
{
#if defined(CONFIG_WLAN) && CONFIG_WLAN
	if (!(wifi_is_running(STA_WLAN_INDEX) || wifi_is_running(SOFTAP_WLAN_INDEX))) {
		BT_LOGE("WiFi is OFF! Please Restart BT after Wifi on!\r\n");
		return false;
	}

	if (hci_is_wifi_need_leave_ps()) {
		wifi_set_lps_enable(FALSE);
		wifi_set_ips_internal(FALSE);
	}
#endif

	return true;
}

void rtk_bt_post_enable(void)
{
#if defined(CONFIG_WLAN) && CONFIG_WLAN
	if (hci_is_wifi_need_leave_ps()) {
		wifi_set_lps_enable(wifi_user_config.lps_enable);
		wifi_set_ips_internal(wifi_user_config.ips_enable);
	}
#endif
}

void hci_platform_external_fw_log_pin(void)
{
	Pinmux_Config(_PB_19, PINMUX_FUNCTION_BT_FWLOG);
	PAD_PullCtrl(_PB_19, GPIO_PuPd_UP);
}

uint8_t hci_platform_open(void)
{
	if (!CHECK_CFG_SW(CFG_SW_BT_FW_LOG)) {
		rtk_bt_fw_log_open();
		BT_LOGA("FW LOG OPEN\r\n");
#if 0
		hci_platform_external_fw_log_pin();
#endif
	}

	/* Read Efuse and Parse Configbuf */
	if (HCI_FAIL == hci_platform_read_efuse()) {
		return HCI_FAIL;
	}

	if (HCI_FAIL == hci_platform_parse_config()) {
		return HCI_FAIL;
	}

	/* BT Controller Reset */
	hci_platform_controller_reset();

	/* UART Open */
	if (HCI_FAIL == hci_uart_open()) {
		return HCI_FAIL;
	}

	return HCI_SUCCESS;
}

void hci_platform_close(void)
{
	/* BT Controller Power Off */
	bt_power_off();

	/* UART Close */
	hci_uart_close();

	if (!CHECK_CFG_SW(CFG_SW_BT_FW_LOG)) {
		rtk_bt_fw_log_close();
	}
}

void hci_platform_free(void)
{
	/* UART Free */
	hci_uart_free();
}

void hci_platform_get_config(uint8_t **buf, uint16_t *len)
{
	*buf = hci_init_config;
	*len = sizeof(hci_init_config);
}

void hci_platform_force_uart_rts(bool op)
{
	hci_uart_force_rts(op);
}

void hci_platform_debug_enable(void)
{
	/* keep bt controller active */
	set_reg_value(SYSTEM_CTRL_BASE + REG_LSYS_BT_CTRL0, LSYS_BIT_HOST_WAKE_BT, 1);
	Pinmux_Config(_PA_25, PINMUX_FUNCTION_BT);  /* bt tx */
	Pinmux_Config(_PA_26, PINMUX_FUNCTION_BT);  /* bt rx */
	Pinmux_Config(_PA_18, PINMUX_FUNCTION_BT);  /* bt rts */
	Pinmux_Config(_PA_19, PINMUX_FUNCTION_BT);  /* bt cts */
	BT_LOGD("external uart pins: rx<->PA_25, tx<->PA_26, cts<->PA_18, rts<->PA_16\r\n");
	/* BT use external uart */
	set_reg_value(SYSTEM_CTRL_BASE + REG_LSYS_BT_CTRL0, LSYS_BIT_BT_USE_EXT_UART, 1);
}

static void hci_platform_bt_debug_bit(uint8_t bt_dbg_port, char *pad)
{
	switch (bt_dbg_port) {
	case 0:
		if (strcmp(pad, "PA3") == 0) {
			Pinmux_Config(_PA_3, PINMUX_FUNCTION_DEBUG);
		} else if (strcmp(pad, "PC3") == 0) {
			Pinmux_Config(_PC_3, PINMUX_FUNCTION_DEBUG);
		} else {
			BT_LOGE("Invalid setting BT debug port, wrong pad[%s], BT debug port[%d] will mapping to PA3.\r\n", pad, bt_dbg_port);
			Pinmux_Config(_PA_3, PINMUX_FUNCTION_DEBUG);
		}
		break;
	case 1:
		if (strcmp(pad, "PA4") == 0) {
			Pinmux_Config(_PA_4, PINMUX_FUNCTION_DEBUG);
		} else if (strcmp(pad, "PC4") == 0) {
			Pinmux_Config(_PC_4, PINMUX_FUNCTION_DEBUG);
		} else {
			BT_LOGE("Invalid setting BT debug port, wrong pad[%s], BT debug port[%d] will mapping to PA4.\r\n", pad, bt_dbg_port);
			Pinmux_Config(_PA_4, PINMUX_FUNCTION_DEBUG);
		}
		break;
	case 2:
		if (strcmp(pad, "PA5") == 0) {
			Pinmux_Config(_PA_5, PINMUX_FUNCTION_DEBUG);
		} else if (strcmp(pad, "PC5") == 0) {
			Pinmux_Config(_PC_5, PINMUX_FUNCTION_DEBUG);
		} else {
			BT_LOGE("Invalid setting BT debug port, wrong pad[%s], BT debug port[%d] will mapping to PA5.\r\n", pad, bt_dbg_port);
			Pinmux_Config(_PA_5, PINMUX_FUNCTION_DEBUG);
		}
		break;
	case 3:
		if (strcmp(pad, "PA18") == 0) {
			Pinmux_Config(_PA_18, PINMUX_FUNCTION_DEBUG);
		} else if (strcmp(pad, "PC6") == 0) {
			Pinmux_Config(_PC_6, PINMUX_FUNCTION_DEBUG);
		} else {
			BT_LOGE("Invalid setting BT debug port, wrong pad[%s], BT debug port[%d] will mapping to PA18.\r\n", pad, bt_dbg_port);
			Pinmux_Config(_PA_18, PINMUX_FUNCTION_DEBUG);
		}
		break;
	case 4:
		if (strcmp(pad, "PA19") == 0) {
			Pinmux_Config(_PA_19, PINMUX_FUNCTION_DEBUG);
		} else if (strcmp(pad, "PC7") == 0) {
			Pinmux_Config(_PC_7, PINMUX_FUNCTION_DEBUG);
		} else {
			BT_LOGE("Invalid setting BT debug port, wrong pad[%s], BT debug port[%d] will mapping to PA19.\r\n", pad, bt_dbg_port);
			Pinmux_Config(_PA_19, PINMUX_FUNCTION_DEBUG);
		}
		break;
	case 5:
		Pinmux_Config(_PA_25, PINMUX_FUNCTION_DEBUG);
		break;
	case 6:
		Pinmux_Config(_PA_26, PINMUX_FUNCTION_DEBUG);
		break;
	case 7:
		if (strcmp(pad, "PA2") == 0) {
			Pinmux_Config(_PA_2, PINMUX_FUNCTION_DEBUG);
		} else if (strcmp(pad, "PB20") == 0) {
			Pinmux_Config(_PB_20, PINMUX_FUNCTION_DEBUG);
		} else if (strcmp(pad, "PC2") == 0) {
			Pinmux_Config(_PC_2, PINMUX_FUNCTION_DEBUG);
		} else {
			BT_LOGE("Invalid setting BT debug port, wrong pad[%s], BT debug port[%d] will mapping to PC2.\r\n", pad, bt_dbg_port);
			Pinmux_Config(_PC_2, PINMUX_FUNCTION_DEBUG);
		}
		break;
	case 8:
		Pinmux_Config(_PA_14, PINMUX_FUNCTION_DEBUG);
		break;
	case 9:
		Pinmux_Config(_PA_15, PINMUX_FUNCTION_DEBUG);
		break;
	case 10:
		Pinmux_Config(_PA_16, PINMUX_FUNCTION_DEBUG);
		break;
	case 11:
		Pinmux_Config(_PA_17, PINMUX_FUNCTION_DEBUG);
		break;
	case 12:
		Pinmux_Config(_PA_20, PINMUX_FUNCTION_DEBUG);
		break;
	case 13:
		Pinmux_Config(_PA_21, PINMUX_FUNCTION_DEBUG);
		break;
	case 14:
		Pinmux_Config(_PA_22, PINMUX_FUNCTION_DEBUG);
		break;
	case 15:
		Pinmux_Config(_PA_23, PINMUX_FUNCTION_DEBUG);
		break;
	case 16:
		Pinmux_Config(_PA_24, PINMUX_FUNCTION_DEBUG);
		break;
	case 17:
		Pinmux_Config(_PA_27, PINMUX_FUNCTION_DEBUG);
		break;
	case 18:
		Pinmux_Config(_PA_28, PINMUX_FUNCTION_DEBUG);
		break;
	case 19:
		Pinmux_Config(_PB_13, PINMUX_FUNCTION_DEBUG);
		break;
	case 20:
		Pinmux_Config(_PB_14, PINMUX_FUNCTION_DEBUG);
		break;
	case 21:
		Pinmux_Config(_PB_15, PINMUX_FUNCTION_DEBUG);
		break;
	case 22:
		Pinmux_Config(_PB_16, PINMUX_FUNCTION_DEBUG);
		break;
	case 23:
		Pinmux_Config(_PB_17, PINMUX_FUNCTION_DEBUG);
		break;
	case 24:
		Pinmux_Config(_PB_18, PINMUX_FUNCTION_DEBUG);
		break;
	case 25:
		Pinmux_Config(_PB_19, PINMUX_FUNCTION_DEBUG);
		break;
	case 26:
		Pinmux_Config(_PB_21, PINMUX_FUNCTION_DEBUG);
		break;
	case 27:
		Pinmux_Config(_PB_22, PINMUX_FUNCTION_DEBUG);
		break;
	case 28:
		Pinmux_Config(_PB_23, PINMUX_FUNCTION_DEBUG);
		break;
	case 29:
		Pinmux_Config(_PB_24, PINMUX_FUNCTION_DEBUG);
		break;
	case 30:
		Pinmux_Config(_PB_25, PINMUX_FUNCTION_DEBUG);
		break;
	case 31:
		Pinmux_Config(_PB_26, PINMUX_FUNCTION_DEBUG);
		break;
	default:
		BT_LOGE("Invalid setting BT debug port, wrong BT debug port[%d].\r\n", bt_dbg_port);
		break;
	}
}

static void hci_platform_bt_debug_shift_bit(uint8_t original, uint8_t mapping)
{
	switch (mapping) {
	case 0:
		set_reg_value(PINMUX_REG_BASE + REG_DBG_GPIO_MAP_0, BIT0 | BIT1 | BIT2 | BIT3 | BIT4, original);
		break;
	case 1:
		set_reg_value(PINMUX_REG_BASE + REG_DBG_GPIO_MAP_0, BIT8 | BIT9 | BIT10 | BIT11 | BIT12, original);
		break;
	case 2:
		set_reg_value(PINMUX_REG_BASE + REG_DBG_GPIO_MAP_0, BIT16 | BIT17 | BIT18 | BIT19 | BIT20, original);
		break;
	case 3:
		set_reg_value(PINMUX_REG_BASE + REG_DBG_GPIO_MAP_0, BIT24 | BIT25 | BIT26 | BIT27 | BIT28, original);
		break;
	case 4:
		set_reg_value(PINMUX_REG_BASE + REG_DBG_GPIO_MAP_1, BIT0 | BIT1 | BIT2 | BIT3 | BIT4, original);
		break;
	case 5:
		set_reg_value(PINMUX_REG_BASE + REG_DBG_GPIO_MAP_1, BIT8 | BIT9 | BIT10 | BIT11 | BIT12, original);
		break;
	case 6:
		set_reg_value(PINMUX_REG_BASE + REG_DBG_GPIO_MAP_1, BIT16 | BIT17 | BIT18 | BIT19 | BIT20, original);
		break;
	case 7:
		set_reg_value(PINMUX_REG_BASE + REG_DBG_GPIO_MAP_1, BIT24 | BIT25 | BIT26 | BIT27 | BIT28, original);
		break;
	default:
		BT_LOGE("Invalid setting BT debug port shift, wrong shift mapping (%d).\r\n", mapping);
		break;
	}
}

static void hci_platform_bt_gpio_pad(uint8_t bt_gpio, char *pad)
{
	switch (bt_gpio) {
	case 1:
		if (strcmp(pad, "PA5") == 0) {
			Pinmux_Config(_PA_5, PINMUX_FUNCTION_BT_IO);
		} else if (strcmp(pad, "PA15") == 0) {
			Pinmux_Config(_PA_15, PINMUX_FUNCTION_BT_IO);
		} else if (strcmp(pad, "PB5") == 0) {
			Pinmux_Config(_PB_5, PINMUX_FUNCTION_BT_IO);
		} else if (strcmp(pad, "PC4") == 0) {
			Pinmux_Config(_PC_4, PINMUX_FUNCTION_BT_IO);
		} else {
			BT_LOGE("Invalid setting BT GPIO, wrong pad[%s], BT GPIO[%d] will mapping to PA5.\r\n", pad, bt_gpio);
			Pinmux_Config(_PA_5, PINMUX_FUNCTION_BT_IO);
		}
		break;
	case 3:
		if (strcmp(pad, "PA16") == 0) {
			Pinmux_Config(_PA_16, PINMUX_FUNCTION_BT_IO);
		} else if (strcmp(pad, "PA18") == 0) {
			Pinmux_Config(_PA_18, PINMUX_FUNCTION_BT_IO);
		} else if (strcmp(pad, "PB6") == 0) {
			Pinmux_Config(_PB_6, PINMUX_FUNCTION_BT_IO);
		} else if (strcmp(pad, "PC5") == 0) {
			Pinmux_Config(_PC_5, PINMUX_FUNCTION_BT_IO);
		} else {
			BT_LOGE("Invalid setting BT GPIO, wrong pad[%s], BT GPIO[%d] will mapping to PA16.\r\n", pad, bt_gpio);
			Pinmux_Config(_PA_16, PINMUX_FUNCTION_BT_IO);
		}
		break;
	case 4:
		if (strcmp(pad, "PA17") == 0) {
			Pinmux_Config(_PA_17, PINMUX_FUNCTION_BT_IO);
		} else if (strcmp(pad, "PA19") == 0) {
			Pinmux_Config(_PA_19, PINMUX_FUNCTION_BT_IO);
		} else if (strcmp(pad, "PB7") == 0) {
			Pinmux_Config(_PB_7, PINMUX_FUNCTION_BT_IO);
		} else if (strcmp(pad, "PC6") == 0) {
			Pinmux_Config(_PC_6, PINMUX_FUNCTION_BT_IO);
		} else {
			BT_LOGE("Invalid setting BT GPIO, wrong pad[%s], BT GPIO[%d] will mapping to PA17.\r\n", pad, bt_gpio);
			Pinmux_Config(_PA_17, PINMUX_FUNCTION_BT_IO);
		}
		break;
	case 5:
		if (strcmp(pad, "PA4") == 0) {
			Pinmux_Config(_PA_4, PINMUX_FUNCTION_BT_IO);
		} else if (strcmp(pad, "PA20") == 0) {
			Pinmux_Config(_PA_20, PINMUX_FUNCTION_BT_IO);
		} else if (strcmp(pad, "PB8") == 0) {
			Pinmux_Config(_PB_8, PINMUX_FUNCTION_BT_IO);
		} else if (strcmp(pad, "PB17") == 0) {
			Pinmux_Config(_PB_17, PINMUX_FUNCTION_BT_IO);
		} else if (strcmp(pad, "PC7") == 0) {
			Pinmux_Config(_PC_7, PINMUX_FUNCTION_BT_IO);
		} else {
			BT_LOGE("Invalid setting BT GPIO, wrong pad[%s], BT GPIO[%d] will mapping to PA4.\r\n", pad, bt_gpio);
			Pinmux_Config(_PA_4, PINMUX_FUNCTION_BT_IO);
		}
		break;
	case 6:
		if (strcmp(pad, "PA3") == 0) {
			Pinmux_Config(_PA_3, PINMUX_FUNCTION_BT_IO);
		} else if (strcmp(pad, "PA21") == 0) {
			Pinmux_Config(_PA_21, PINMUX_FUNCTION_BT_IO);
		} else if (strcmp(pad, "PB9") == 0) {
			Pinmux_Config(_PB_9, PINMUX_FUNCTION_BT_IO);
		} else if (strcmp(pad, "PB18") == 0) {
			Pinmux_Config(_PB_18, PINMUX_FUNCTION_BT_IO);
		} else if (strcmp(pad, "PC3") == 0) {
			Pinmux_Config(_PC_3, PINMUX_FUNCTION_BT_IO);
		} else {
			BT_LOGE("Invalid setting BT GPIO, wrong pad[%s], BT GPIO[%d] will mapping to PA3.\r\n", pad, bt_gpio);
			Pinmux_Config(_PA_3, PINMUX_FUNCTION_BT_IO);
		}
		break;
	case 8:
		if (strcmp(pad, "PB10") == 0) {
			Pinmux_Config(_PB_10, PINMUX_FUNCTION_BT_IO);
		} else if (strcmp(pad, "PB19") == 0) {
			Pinmux_Config(_PB_19, PINMUX_FUNCTION_BT_IO);
		} else if (strcmp(pad, "PC2") == 0) {
			Pinmux_Config(_PC_2, PINMUX_FUNCTION_BT_IO);
		} else {
			BT_LOGE("Invalid setting BT GPIO, wrong pad[%s], BT GPIO[%d] will mapping to PB10.\r\n", pad, bt_gpio);
			Pinmux_Config(_PB_10, PINMUX_FUNCTION_BT_IO);
		}
		break;
	default:
		BT_LOGE("Invalid setting BT GPIO, wrong BT GPIO[%d].\r\n", bt_gpio);
		break;
	}
}

static void hci_platform_debug_port_pre_enable(uint8_t bt_sel)
{
	set_reg_value(PINMUX_REG_BASE + REG_SWD_SDD_CTRL, BIT0, 0);      /* pad reg 0x4080A9F4[0]=0, SWD pinmux disable */
	osif_delay(5);
	set_reg_value(PINMUX_REG_BASE + REG_DBG_PORT_CTRL, BIT31, 1);    /* 0x4080A9E4[31]=1, debug port enable */
	osif_delay(5);
	set_reg_value(PINMUX_REG_BASE + REG_DBG_PORT_CTRL, BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7, 0x86); /* 0x4080A9E4[7:0]=0x86, debug port sel */
	osif_delay(5);
	if (bt_sel == DBG_BT_VENDOR) {                     /* 0x4080A2B8[4]=0, sel BT TOP for debug port */
		set_reg_value(SYSTEM_CTRL_BASE + REG_LSYS_BT_CTRL0, BIT4, 0);
	} else if (bt_sel == DBG_BT_ON) {                  /* 0x4080A2B8[4]=1, sel BT TOP for PMC */
		set_reg_value(SYSTEM_CTRL_BASE + REG_LSYS_BT_CTRL0, BIT4, 1);
	}
	osif_delay(5);
}

void hci_platform_debug_port_mask_enable(uint8_t bt_sel, uint32_t bt_dbg_mask)
{
	uint8_t i = 0;
	uint32_t mask = bt_dbg_mask;

	hci_platform_debug_port_pre_enable(bt_sel);
	for (i = 0; i < 32 && mask != 0; i++) {            /* pinmux, to PINMUX_FUNCTION_DEBUG */
		if ((mask & ((uint32_t)0x1)) == 1) {
			hci_platform_bt_debug_bit(i, NULL);
			osif_delay(5);
		}
		mask >>= 1;
	}
}

void hci_platform_debug_port_pad_enable(uint8_t bt_sel, uint8_t bt_dbg_port, char *pad)
{
	hci_platform_debug_port_pre_enable(bt_sel);
	hci_platform_bt_debug_bit(bt_dbg_port, pad);       /* pinmux, to PINMUX_FUNCTION_DEBUG */
	osif_delay(5);
}

void hci_platform_debug_port_shift(uint8_t original, uint8_t mapping)
{
	/* shift original to mapping */
	hci_platform_bt_debug_shift_bit(original, mapping);
	osif_delay(5);
}

void hci_platform_gpio_enable(uint8_t bt_gpio, char *pad)
{
	set_reg_value(PINMUX_REG_BASE + REG_SWD_SDD_CTRL, BIT0, 0);  /* pad reg 0x4080A9F4[0]=0, SWD pinmux disable */
	osif_delay(5);
	hci_platform_bt_gpio_pad(bt_gpio, pad);                      /* pinmux, to PINMUX_FUNCTION_BT_IO */
	osif_delay(5);
}
