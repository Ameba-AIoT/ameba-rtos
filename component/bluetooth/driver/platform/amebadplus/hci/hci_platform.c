/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include "osif.h"
#include "hci_config.h"
#include "hci/hci_common.h"
#include "hci_uart.h"
#include "hci_platform.h"
#include "bt_debug.h"
#include "ameba.h"
#include "wifi_conf.h"
#include "platform_autoconf.h"

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

uint32_t hci_cfg_sw_val = 0xFF;                  // Open BT Trace log & FW log use 0xDD
bt_voltage_t bt_voltage_switch = VOLTAGE_3V3;    // Select BT voltage

//static const uint8_t hci_patch_buf[] = {0xff, 0xff, 0xff, 0xff};
//static uint32_t hci_patch_buf_len    = sizeof(hci_patch_buf);
static uint8_t hci_phy_efuse[HCI_PHY_EFUSE_LEN]  = {0};
static uint8_t hci_lgc_efuse[HCI_LGC_EFUSE_LEN]  = {0};
unsigned char hci_init_config_3v3[] = {
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

	/* RF: phy_flow_ctrl_para(1), iqm_txgaink_module(1), iqm_txgain_flatk_module(4), iqm_txgain_10dBm_raw_index(1), lbt_ant_gain(1) */
	0x78, 0x02, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x07,

	/* RF: iqm_max_txgain_LE1M(1), iqm_max_txgain_LE2M(1), iqm_max_txgain_LE1M_adv(1), iqm_max_txgain_LE2M_adv(1), iqm_max_txgain_LR(1) */
	0x85, 0x02, 0x05, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B,

	/* RF: iqm_max_txgain_LE1M_2402(1), iqm_max_txgain_LE1M_2480(1), iqm_max_txgain_LE2M_2402(1), iqm_max_txgain_LE2M_2480(1) */
	0x8A, 0x02, 0x04, 0x70, 0x70, 0x70, 0x70
};
unsigned int hci_init_config_len_3v3 = sizeof(hci_init_config_3v3);

unsigned char hci_init_config_1v8[] = {
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

	/* RF: phy_flow_ctrl_para(1), iqm_txgaink_module(1), iqm_txgain_flatk_module(4), iqm_txgain_10dBm_raw_index(1), lbt_ant_gain(1) */
	0x78, 0x02, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x45, 0x00,

	/* RF: iqm_max_txgain_LE1M(1), iqm_max_txgain_LE2M(1), iqm_max_txgain_LE1M_adv(1), iqm_max_txgain_LE2M_adv(1), iqm_max_txgain_LR(1) */
	0x85, 0x02, 0x05, 0x44, 0x44, 0x44, 0x44, 0x44,

	/* RF: iqm_max_txgain_LE1M_2402(1), iqm_max_txgain_LE1M_2480(1), iqm_max_txgain_LE2M_2402(1), iqm_max_txgain_LE2M_2480(1) */
	0x8A, 0x02, 0x04, 0x70, 0x70, 0x70, 0x70
};
unsigned int hci_init_config_len_1v8 = sizeof(hci_init_config_1v8);

unsigned char *hci_init_config = NULL;
unsigned int hci_init_config_len = 0;

static uint8_t  hci_cfg_bd_addr[HCI_MAC_ADDR_LEN] = {0};
static uint32_t hci_cfg_flag                      = 0;

extern int wifi_set_ips_internal(u8 enable);

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
	// DAC DCK
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
		//DCK_KOSEN_Q_BT_0dB[5:0]
		p_temp_pram.rfk_data1 = (hci_phy_efuse[PEFUSE(0x754)] & 0xFC) >> 2;
		//DCK_DOS_Q_BT_0dB [5:0]
		p_temp_pram.rfk_data2 = (hci_phy_efuse[PEFUSE(0x752)] & 0xC0) >> 6 | (hci_phy_efuse[PEFUSE(0x753)] & 0x0F) << 2;
		//DCK_KOSEN_I_BT_0dB[5:0]
		p_temp_pram.rfk_data3 = (hci_phy_efuse[PEFUSE(0x753)] & 0xF0) >> 4 | (hci_phy_efuse[PEFUSE(0x754)] & 0x03) << 4;
		//DCK_DOS_I_BT_0dB [5:0]
		p_temp_pram.rfk_data4 = hci_phy_efuse[PEFUSE(0x752)] & 0x3F;

		wifi_btcoex_bt_rfk(&p_temp_pram);
	}

	//TX LOK
	/* 0x741[0] 0 for tx lok valid */
	if ((hci_phy_efuse[PEFUSE(0x741)] & BIT0) == 0) {
		struct bt_rfk_param p_temp_pram = {0};
		/*TXMOD_IDAC_IS[4:0] = 0x743[4:0]
		TXMOD_IDAC_QS[4:0] = 0x745[4:0]
		TXMOD_IDAC2_IS[3:0] = 0x744[3:0]
		TXMOD_IDAC2_QS[3:0] = 0x746[3:0]*/
		p_temp_pram.type = BT_LOK;
		//TXMOD_IDAC_IS[4:0]
		p_temp_pram.rfk_data1 = hci_phy_efuse[PEFUSE(0x743)] & 0x1F;
		//TXMOD_IDAC_QS[4:0]
		p_temp_pram.rfk_data2 = hci_phy_efuse[PEFUSE(0x745)] & 0x1F;
		//TXMOD_IDAC2_IS[3:0]
		p_temp_pram.rfk_data3 = hci_phy_efuse[PEFUSE(0x744)] & 0x0F;
		//TXMOD_IDAC2_QS[3:0]
		p_temp_pram.rfk_data4 = hci_phy_efuse[PEFUSE(0x746)] & 0x0F;

		wifi_btcoex_bt_rfk(&p_temp_pram);
	}

	//TX LOK Res
	/* 0x741[4] 0 for tx lok res valid */
	if ((hci_phy_efuse[PEFUSE(0x741)] & BIT4) == 0) {
		struct bt_rfk_param p_temp_pram = {0};
		/*TXMOD_IDAC_BS[2:0] = 0x760[5:3]
		TXMOD_IDAC2_BS[2:0] = 0x760[2:0]*/
		/* LO_resolution */
		p_temp_pram.type = BT_LOK_RES;
		p_temp_pram.rfk_data1 = hci_phy_efuse[PEFUSE(0x760)];

		wifi_btcoex_bt_rfk(&p_temp_pram);
	}
}

int hci_platform_get_write_phy_efuse_data(uint8_t *data, uint8_t len)
{
	memcpy(data, hci_phy_efuse, len);
	return HCI_SUCCESS;
}

int hci_platform_get_rx_adck_data(uint8_t *data, uint8_t len)
{
	struct bt_rfk_param p_temp_pram = {0};

	p_temp_pram.type = BT_ADC_DCK;

	wifi_btcoex_bt_rfk(&p_temp_pram);

	memcpy(data, &p_temp_pram.rfk_data1, len);

	return HCI_SUCCESS;
}

bool hci_platform_check_lmp_subver(uint16_t lmp_subver)
{
	if (HCI_DEFAULT_LMP_SUBVER == lmp_subver) {
		return true;
	} else {
		return false;
	}
}

static void hci_platform_read_voltage(void)
{
	uint32_t value = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_RSVD_FOR_SW1);
	if (value & AON_BIT_WIFI_RF_1833_SEL) {
		bt_voltage_switch = VOLTAGE_3V3;
	} else {
		bt_voltage_switch = VOLTAGE_1V8;
	}
}

static uint8_t hci_platform_read_efuse(void)
{
	uint8_t i, *pbuf;

	/* Read Logic Efuse */
	pbuf = osif_mem_alloc(RAM_TYPE_DATA_ON, 1024);
	if (!pbuf || _FAIL == OTP_LogicalMap_Read(pbuf, 0, OTP_LMAP_LEN)) {
		BT_LOGE("OTP_LogicalMap_Read failed\r\n");
		if (pbuf) {
			osif_mem_free(pbuf);
		}
		return HCI_FAIL;
	}

	memcpy(hci_lgc_efuse, pbuf + HCI_LGC_EFUSE_OFFSET, HCI_LGC_EFUSE_LEN);

	/* Read Physical Efuse */
	for (i = 0; i < HCI_PHY_EFUSE_LEN; i++) {
		OTP_Read8((HCI_PHY_EFUSE_OFFSET + i), (hci_phy_efuse + i));
	}
#if 0
	BT_DUMPA("Read Logic Efuse:\r\n", hci_lgc_efuse, HCI_LGC_EFUSE_LEN);
	BT_DUMPA("Read Phy Efuse:\r\n", hci_phy_efuse, HCI_PHY_EFUSE_LEN);
#endif
	if (pbuf) {
		osif_mem_free(pbuf);
	}

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

	if (bt_voltage_switch == VOLTAGE_3V3) {
		hci_init_config = hci_init_config_3v3;
		hci_init_config_len = hci_init_config_len_3v3;
	} else {
		hci_init_config = hci_init_config_1v8;
		hci_init_config_len = hci_init_config_len_1v8;
	}

	if (hci_init_config_len <= HCI_CONFIG_HDR_LEN) {
		return HCI_IGNORE;
	}

	p = hci_init_config;
	memcpy((void *)&config_header, (void *)p, sizeof(uint32_t));
	if (HCI_CONFIG_SIGNATURE != config_header) {
		return HCI_FAIL;
	}

	memcpy((void *)&config_length, (void *)(p + 4), sizeof(uint16_t));
	if (config_length != (uint16_t)(hci_init_config_len - HCI_CONFIG_HDR_LEN)) {
		/* Fix the len, just avoid the length is not corect */
		p[4] = (uint8_t)((hci_init_config_len - HCI_CONFIG_HDR_LEN) >> 0);
		p[5] = (uint8_t)((hci_init_config_len - HCI_CONFIG_HDR_LEN) >> 8);
	}

	p += HCI_CONFIG_HDR_LEN;
	while (p < hci_init_config + hci_init_config_len) {
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
	set_reg_value(0x41008200, BIT1, 1);             //enable BT Power Cut
	osif_delay(5);
	set_reg_value(0x41008200, BIT17, 0);            //disable ISO of BT
	osif_delay(5);
	set_reg_value(0x41008208, BIT2, 1);             //enable WL RFAFE control circuit
	osif_delay(5);
	set_reg_value(0x41008480, BIT5 | BIT6, 3);      //enable RFAFE
	osif_delay(5);
	set_reg_value(0x410084A8, BIT4, 1);             //enable WL_CKI_80M_RFC, default=1, can skip
	osif_delay(5);
	set_reg_value(0x410084A8, BIT3, 0);             //when WL RFAFE enter power off, keep WLRFC not power off
	osif_delay(5);
	set_reg_value(0x41008208, BIT1, 1);             //release BTON reset
	osif_delay(5);
	if (HCI_BT_KEEP_WAKE) {
		set_reg_value(0x41008280, BIT14, 1);        //enable HOST_WAKE_BT No GPIO
		osif_delay(5);
		set_reg_value(0x41008280, BIT13, 1);        //HOST_WAKE_BT
		osif_delay(5);
	}
}

void bt_power_off(void)
{
	set_reg_value(0x41008280, BIT5, 1);             //request poff xtal and swr
	osif_delay(5);
	set_reg_value(0x41008280, BIT5, 0);             //disable request poff xtal and swr, for next POFF flow
	osif_delay(5);
	set_reg_value(0x41008208, BIT1, 0);             //assert BTON reset
	osif_delay(5);
	if (!(wifi_is_running(WLAN0_IDX) || wifi_is_running(WLAN1_IDX))) {
		set_reg_value(0x41008480, BIT5 | BIT6, 0);  //disable RFAFE (if WIFI active, keep 2'b11)
		osif_delay(5);
		set_reg_value(0x41008208, BIT2, 0);         //disable WL RFAFE control circuit (if WIFI active, keep 1'b1)
		osif_delay(5);
	}
	set_reg_value(0x41008200, BIT17, 1);            //enable ISO of BT
	osif_delay(5);
	set_reg_value(0x41008200, BIT1, 0);             //disable BT Power Cut
	osif_delay(5);
}

void hci_platform_low_power_setting(void)
{
	set_reg_value(0x4100827C, 0x0007FFFF, 0x4C4F3);         //set 0x4100827C[18:0]=4C4F3, LPS mode divisor
	osif_delay(5);
	set_reg_value(0x41008280, BIT2 | BIT1 | BIT0, 2);       //BT_SWR_STS_LPS: set SWR state2 (0.8/PC/PFM) when BT in LPS mode
	osif_delay(5);
	set_reg_value(0x41008280, BIT12 | BIT11 | BIT10, 1);    //BT_XTAL_MODE_LPS: set XTAL state (XTAL LPS, gating XTAL 40M) when BT in LPS mode
	osif_delay(5);
	set_reg_value(0x41008284, BIT15, 0);                    //BT_CKSL_CKANA
	osif_delay(5);
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
	uint32_t lock_status;

	if (!(wifi_is_running(WLAN0_IDX) || wifi_is_running(WLAN1_IDX))) {
		BT_LOGE("WiFi is OFF! Please Restart BT after Wifi on!\r\n");
		return false;
	}

	if (!hci_is_mp_mode()) {
		wifi_set_lps_enable(FALSE);
		wifi_set_ips_internal(FALSE);
	}

	lock_status = pmu_get_wakelock_status();
	if (!(lock_status & ((0x01) << PMU_BT_DEVICE))) {
		BT_LOGA("Acuqire BT PMU LOCK \r\n");
		pmu_acquire_wakelock(PMU_BT_DEVICE);
	}

	return true;
}

void rtk_bt_post_enable(void)
{
	uint32_t lock_status;

	lock_status = pmu_get_wakelock_status();
	if (lock_status & ((0x01) << PMU_BT_DEVICE)) {
		BT_LOGA("Release BT PMU LOCK \r\n");
		pmu_release_wakelock(PMU_BT_DEVICE);
	}

	if (!hci_is_mp_mode()) {
		wifi_set_lps_enable(wifi_user_config.lps_enable);
		wifi_set_ips_internal(wifi_user_config.ips_enable);
	}
}

void hci_platform_external_fw_log_pin(void)
{
	Pinmux_Config(_PA_12, PINMUX_FUNCTION_BT);
	PAD_PullCtrl(_PA_12, GPIO_PuPd_UP);
}

uint8_t hci_platform_init(void)
{
	// Move to rtk_bt_enable() to avoid rtk upperstack memory leak when fail
	// if (rtk_bt_pre_enable() == false) {
	//  BT_LOGE("rtk_bt_pre_enable fail!\r\n");
	//  return HCI_FAIL;
	// }

	if (!CHECK_CFG_SW(CFG_SW_BT_FW_LOG)) {
		rtk_bt_fw_log_open();
		BT_LOGA("FW LOG OPEN\r\n");
#if 0
		hci_platform_external_fw_log_pin();
#endif
	}

	/* Read Voltage */
	hci_platform_read_voltage();

	/* Read Efuse and Parse Configbuf */
	if (HCI_FAIL == hci_platform_read_efuse()) {
		return HCI_FAIL;
	}

	if (HCI_FAIL == hci_platform_parse_config()) {
		return HCI_FAIL;
	}

	/* BT Controller Reset */
	hci_platform_controller_reset();

	/* UART Init */
	if (HCI_FAIL == hci_uart_open()) {
		return HCI_FAIL;
	}

	return HCI_SUCCESS;
}

void hci_platform_deinit(void)
{
	/* BT Controller Power Off */
	bt_power_off();

	/* UART Deinit */
	hci_uart_close();

	if (!CHECK_CFG_SW(CFG_SW_BT_FW_LOG)) {
		rtk_bt_fw_log_close();
	}
}

void hci_platform_get_config(uint8_t **buf, uint16_t *len)
{
	*buf = hci_init_config;
	*len = hci_init_config_len;
}

void hci_platform_force_uart_rts(bool op)
{
	hci_uart_force_rts(op);
}

void hci_platform_debug_enable(void)
{
#if 0
	uint32_t data = 0;
	/* keep bt wakeup from dlps/lps 0x41008280[14:13]=11 */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_BT_CTRL0, HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_BT_CTRL0) | LSYS_BIT_HOST_WAKE_BT_POL | LSYS_BIT_HOST_WAKE_BT);

	while (1) {
		osif_delay(100);
		data = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_BT_CTRL1) & 0x7FFF; // 0x41008284 [14:0]
		if (data == 5) {
			/* bt active */
			BT_LOGD("bt active\r\n");
			break;
		} else {
			BT_LOGD("wait for bt wakeup from sleep, bt pmc state=0x%x\r\n", data);
		}
	}

	BT_LOGD("BT wakeup from sleep\r\n");
#endif
	//pad reg 0x410089F4[0]=0, SWD pinmux disable
	HAL_WRITE32(PINMUX_REG_BASE, REG_SWD_SDD_CTRL, HAL_READ32(PINMUX_REG_BASE, REG_SWD_SDD_CTRL) & (~PAD_BIT_SWD_PMUX_EN));

	Pinmux_Config(_PB_30, PINMUX_FUNCTION_BT_UART_TXD);  //bt tx, func60 -> external uart rx
	Pinmux_Config(_PA_30, PINMUX_FUNCTION_BT);  //bt rx -> external uart tx
	Pinmux_Config(_PB_31, PINMUX_FUNCTION_BT_UART_RTS);  //bt rts, func61 -> external uart cts
	Pinmux_Config(_PA_31, PINMUX_FUNCTION_BT);  //bt cts -> external uart rts

	BT_LOGD("external uart pins: rx<->_PB_30,tx<->_PA_30,cts<->_PB_31,rts<->_PA_31\r\n");

	//lp_sys reg 0x41008280[0]=1, indicate BT use external uart
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_BT_CTRL0, HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_BT_CTRL0) | LSYS_BIT_BT_USE_EXT_UART);
}

static void hci_platform_bt_debug_bit(uint8_t bt_dbg_port, char *pad)
{
	(void)pad;

	switch (bt_dbg_port) {
	case 0:
		Pinmux_Config(_PB_30, PINMUX_FUNCTION_DEBUG);
		break;
	case 1:
		Pinmux_Config(_PB_31, PINMUX_FUNCTION_DEBUG);
		break;
	case 2:
		Pinmux_Config(_PA_30, PINMUX_FUNCTION_DEBUG);
		break;
	case 3:
		Pinmux_Config(_PA_31, PINMUX_FUNCTION_DEBUG);
		break;
	case 4:
		Pinmux_Config(_PB_20, PINMUX_FUNCTION_DEBUG);
		break;
	case 5:
		Pinmux_Config(_PB_21, PINMUX_FUNCTION_DEBUG);
		break;
	case 6:
		Pinmux_Config(_PB_17, PINMUX_FUNCTION_DEBUG);
		break;
	case 7:
		Pinmux_Config(_PB_18, PINMUX_FUNCTION_DEBUG);
		break;
	case 8:
		Pinmux_Config(_PB_19, PINMUX_FUNCTION_DEBUG);
		break;
	case 9:
		Pinmux_Config(_PB_16, PINMUX_FUNCTION_DEBUG);
		break;
	case 10:
		Pinmux_Config(_PA_12, PINMUX_FUNCTION_DEBUG);
		break;
	case 11:
		Pinmux_Config(_PA_13, PINMUX_FUNCTION_DEBUG);
		break;
	case 12:
		Pinmux_Config(_PA_14, PINMUX_FUNCTION_DEBUG);
		break;
	case 13:
		Pinmux_Config(_PA_15, PINMUX_FUNCTION_DEBUG);
		break;
	case 14:
		Pinmux_Config(_PA_16, PINMUX_FUNCTION_DEBUG);
		break;
	case 15:
		Pinmux_Config(_PA_17, PINMUX_FUNCTION_DEBUG);
		break;
	case 16:
		Pinmux_Config(_PA_18, PINMUX_FUNCTION_DEBUG);
		break;
	case 17:
		Pinmux_Config(_PA_19, PINMUX_FUNCTION_DEBUG);
		break;
	case 18:
		Pinmux_Config(_PA_20, PINMUX_FUNCTION_DEBUG);
		break;
	case 19:
		Pinmux_Config(_PA_21, PINMUX_FUNCTION_DEBUG);
		break;
	case 20:
		Pinmux_Config(_PA_22, PINMUX_FUNCTION_DEBUG);
		break;
	case 21:
		Pinmux_Config(_PA_23, PINMUX_FUNCTION_DEBUG);
		break;
	case 22:
		Pinmux_Config(_PA_26, PINMUX_FUNCTION_DEBUG);
		break;
	case 23:
		Pinmux_Config(_PA_27, PINMUX_FUNCTION_DEBUG);
		break;
	case 24:
		Pinmux_Config(_PA_28, PINMUX_FUNCTION_DEBUG);
		break;
	case 25:
		Pinmux_Config(_PA_29, PINMUX_FUNCTION_DEBUG);
		break;
	case 26:
		Pinmux_Config(_PB_0, PINMUX_FUNCTION_DEBUG);
		break;
	case 27:
		Pinmux_Config(_PB_1, PINMUX_FUNCTION_DEBUG);
		break;
	case 28:
		Pinmux_Config(_PB_2, PINMUX_FUNCTION_DEBUG);
		break;
	case 29:
		Pinmux_Config(_PB_3, PINMUX_FUNCTION_DEBUG);
		break;
	case 30:
		Pinmux_Config(_PB_6, PINMUX_FUNCTION_DEBUG);
		break;
	case 31:
		Pinmux_Config(_PB_7, PINMUX_FUNCTION_DEBUG);
		break;
	default:
		BT_LOGA("Invalid setting BT debug port, wrong BT debug port[%d].\r\n", bt_dbg_port);
		break;
	}
}

static void hci_platform_bt_debug_shift_bit(uint8_t original, uint8_t mapping)
{
	switch (mapping) {
	case 0:
		set_reg_value(0x410089E8, BIT0 | BIT1 | BIT2 | BIT3 | BIT4, original);
		break;
	case 1:
		set_reg_value(0x410089E8, BIT8 | BIT9 | BIT10 | BIT11 | BIT12, original);
		break;
	case 2:
		set_reg_value(0x410089E8, BIT16 | BIT17 | BIT18 | BIT19 | BIT20, original);
		break;
	case 3:
		set_reg_value(0x410089E8, BIT24 | BIT25 | BIT26 | BIT27 | BIT28, original);
		break;
	case 4:
		set_reg_value(0x410089EC, BIT0 | BIT1 | BIT2 | BIT3 | BIT4, original);
		break;
	case 5:
		set_reg_value(0x410089EC, BIT8 | BIT9 | BIT10 | BIT11 | BIT12, original);
		break;
	case 6:
		set_reg_value(0x410089EC, BIT16 | BIT17 | BIT18 | BIT19 | BIT20, original);
		break;
	case 7:
		set_reg_value(0x410089EC, BIT24 | BIT25 | BIT26 | BIT27 | BIT28, original);
		break;
	default:
		BT_LOGA("Invalid setting BT debug port shift, wrong shift mapping (%d).\r\n", mapping);
		break;
	}
}

static void hci_platform_bt_gpio_pad(uint8_t bt_gpio, char *pad)
{
	switch (bt_gpio) {
	case 1:
		if (strcmp(pad, "PA19") == 0) {
			Pinmux_Config(_PA_19, PINMUX_FUNCTION_BT_IO);
		} else if (strcmp(pad, "PA27") == 0) {
			Pinmux_Config(_PA_27, PINMUX_FUNCTION_BT_IO);
		} else {
			BT_LOGA("Invalid setting BT GPIO, wrong pad[%s], this BT GPIO will mapping to PA19.\r\n", pad);
			Pinmux_Config(_PA_19, PINMUX_FUNCTION_BT_IO);
		}
		break;
	case 3:
		if (strcmp(pad, "PA20") == 0) {
			Pinmux_Config(_PA_20, PINMUX_FUNCTION_BT_IO);
		} else if (strcmp(pad, "PA28") == 0) {
			Pinmux_Config(_PA_28, PINMUX_FUNCTION_BT_IO);
		} else {
			BT_LOGA("Invalid setting BT GPIO, wrong pad[%s], this BT GPIO will mapping to PA20.\r\n", pad);
			Pinmux_Config(_PA_20, PINMUX_FUNCTION_BT_IO);
		}
		break;
	case 4:
		if (strcmp(pad, "PA21") == 0) {
			Pinmux_Config(_PA_21, PINMUX_FUNCTION_BT_IO);
		} else if (strcmp(pad, "PA29") == 0) {
			Pinmux_Config(_PA_29, PINMUX_FUNCTION_BT_IO);
		} else {
			BT_LOGA("Invalid setting BT GPIO, wrong pad[%s], this BT GPIO will mapping to PA21.\r\n", pad);
			Pinmux_Config(_PA_21, PINMUX_FUNCTION_BT_IO);
		}
		break;
	case 5:
		if (strcmp(pad, "PA22") == 0) {
			Pinmux_Config(_PA_22, PINMUX_FUNCTION_BT_IO);
		} else if (strcmp(pad, "PA30") == 0) {
			Pinmux_Config(_PA_30, PINMUX_FUNCTION_BT_IO);
		} else {
			BT_LOGA("Invalid setting BT GPIO, wrong pad[%s], this BT GPIO will mapping to PA22.\r\n", pad);
			Pinmux_Config(_PA_22, PINMUX_FUNCTION_BT_IO);
		}
		break;
	case 6:
		if (strcmp(pad, "PA23") == 0) {
			Pinmux_Config(_PA_23, PINMUX_FUNCTION_BT_IO);
		} else if (strcmp(pad, "PA31") == 0) {
			Pinmux_Config(_PA_31, PINMUX_FUNCTION_BT_IO);
		} else {
			BT_LOGA("Invalid setting BT GPIO, wrong pad[%s], this BT GPIO will mapping to PA23.\r\n", pad);
			Pinmux_Config(_PA_23, PINMUX_FUNCTION_BT_IO);
		}
		break;
	case 8:
		if (strcmp(pad, "PA26") == 0) {
			Pinmux_Config(_PA_26, PINMUX_FUNCTION_BT_IO);
		} else if (strcmp(pad, "PB0") == 0) {
			Pinmux_Config(_PB_0, PINMUX_FUNCTION_BT_IO);
		} else {
			BT_LOGA("Invalid setting BT GPIO, wrong pad[%s], this BT GPIO will mapping to PA26.\r\n", pad);
			Pinmux_Config(_PA_26, PINMUX_FUNCTION_BT_IO);
		}
		break;
	default:
		BT_LOGA("Invalid setting BT GPIO, wrong BT GPIO[%d].\r\n", bt_gpio);
		break;
	}
}

static void hci_platform_debug_port_pre_enable(uint8_t bt_sel)
{
	set_reg_value(0x410089F4, BIT0, 0);                // pad reg 0x410089F4[0]=0, SWD pinmux disable
	osif_delay(5);
	set_reg_value(0x410089E4, BIT31, 1);               // 0x4100C9E4[31]=1, debug port enable
	osif_delay(5);
	set_reg_value(0x410089E4, BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7, 0x6A); // 0x410089E4[7:0]=0x6A, debug port sel
	osif_delay(5);
	if (bt_sel == DBG_BT_VENDOR) {                     // 0x41008280[4]=0, sel BT TOP for debug port
		set_reg_value(0x41008280, BIT4, 0);
	} else if (bt_sel == DBG_BT_ON) {                  // 0x41008280[4]=1, sel BT TOP for PMC
		set_reg_value(0x41008280, BIT4, 1);
	}
	osif_delay(5);
}

void hci_platform_debug_port_mask_enable(uint8_t bt_sel, uint32_t bt_dbg_mask)
{
	uint8_t i = 0;
	uint32_t mask = bt_dbg_mask;

	hci_platform_debug_port_pre_enable(bt_sel);
	for (i = 0; i < 32 && mask != 0; i++) {            // pinmux, to PINMUX_FUNCTION_DEBUG
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
	hci_platform_bt_debug_bit(bt_dbg_port, pad);       // pinmux, to PINMUX_FUNCTION_DEBUG
	osif_delay(5);
}

void hci_platform_debug_port_shift(uint8_t original, uint8_t mapping)
{
	// shift original to mapping
	hci_platform_bt_debug_shift_bit(original, mapping);
	osif_delay(5);
}

void hci_platform_gpio_enable(uint8_t bt_gpio, char *pad)
{
	set_reg_value(0x410089F4, BIT0, 0);               // pad reg 0x410089F4[0]=0, SWD pinmux disable
	osif_delay(5);
	hci_platform_bt_gpio_pad(bt_gpio, pad);           // pinmux, to PINMUX_FUNCTION_BT_IO
	osif_delay(5);
}