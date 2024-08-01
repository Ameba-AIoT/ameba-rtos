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
#include "wifi_conf.h"

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

uint32_t hci_cfg_sw_val = 0xFF;    // Open BT Trace log & FW log use 0xDD
uint8_t bt_ant_switch = 0xFF;      // Select BT RF Patch

static uint8_t bt_manual_tx_power_gain_enable = 0;
static uint8_t bt_manual_gain_index_le;
static uint8_t bt_manual_gain_index_br;
static uint8_t bt_manual_gain_index_edr2m;
static uint8_t bt_manual_gain_index_edr3m;

static uint8_t hci_phy_efuse[HCI_PHY_EFUSE_LEN]  = {0};
static uint8_t hci_lgc_efuse[HCI_LGC_EFUSE_LEN]  = {0};
unsigned char hci_init_config_s0[] = {
	/* Header */
	0x55, 0xAB, 0x23, 0x87,

	/* Length */
	0x40, 0x00,

	/* BT MAC address */
	0x30, 0x00, 0x06, 0x8A, 0xD5, 0x23, 0x4C, 0xE0, 0x00,

	/* LOG Uart Baudrate 115200 */
	0x08, 0x00, 0x04, 0x00, 0xC2, 0x01, 0x00,

	/* HCI Uart Baudrate 3M */
	0x0C, 0x00, 0x04, 0x01, 0x80, 0x92, 0x04,

	/* Uart Flow Control */
	0x18, 0x00, 0x01, 0x5C,

	/* BT Wake Host */
	0x42, 0x00, 0x01, 0x09,

	/* Host Wake BT */
	0x43, 0x00, 0x01, 0x07,

	/* RF: phy_flow_ctrl_para(1), iqm_txgaink_module(1), iqm_txgain_flatk_module(4), iqm_txgain_10dBm_raw_index(1), lbt_ant_gain(1) */
	0x78, 0x02, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x53, 0x00,

	/* RF: iqm_max_txgain_1M(1), iqm_max_txgain_2M(1), iqm_max_txgain_3M(1), iqm_max_txgain_LE1M(1), iqm_max_txgain_LE2M(1), iqm_max_txgain_LE1M_adv(1), iqm_max_txgain_LE2M_adv(1), iqm_max_txgain_LR(1)*/
	/* S0(7dBm): */
	0x82, 0x02, 0x08, 0x4D, 0x51, 0x51, 0x4D, 0x4D, 0x4D, 0x4D, 0x4D,

	/* RF: iqm_max_txgain_LE1M_2402(1), iqm_max_txgain_LE1M_2480(1), iqm_max_txgain_LE2M_2402(1), iqm_max_txgain_LE2M_2480(1)*/
	0x8A, 0x02, 0x04, 0x70, 0x70, 0x70, 0x70
};
unsigned int hci_init_config_len_s0 = sizeof(hci_init_config_s0);

unsigned char hci_init_config_s1[] = {
	/* Header */
	0x55, 0xAB, 0x23, 0x87,

	/* Length */
	0x40, 0x00,

	/* BT MAC address */
	0x30, 0x00, 0x06, 0x8A, 0xD5, 0x23, 0x4C, 0xE0, 0x00,

	/* LOG Uart Baudrate 115200 */
	0x08, 0x00, 0x04, 0x00, 0xC2, 0x01, 0x00,

	/* HCI Uart Baudrate 3M */
	0x0C, 0x00, 0x04, 0x01, 0x80, 0x92, 0x04,

	/* Uart Flow Control */
	0x18, 0x00, 0x01, 0x5C,

	/* BT Wake Host */
	0x42, 0x00, 0x01, 0x09,

	/* Host Wake BT */
	0x43, 0x00, 0x01, 0x07,

	/* RF: phy_flow_ctrl_para(1), iqm_txgaink_module(1), iqm_txgain_flatk_module(4), iqm_txgain_10dBm_raw_index(1), lbt_ant_gain(1) */
	0x78, 0x02, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x52, 0x00,

	/* RF: iqm_max_txgain_1M(1), iqm_max_txgain_2M(1), iqm_max_txgain_3M(1), iqm_max_txgain_LE1M(1), iqm_max_txgain_LE2M(1), iqm_max_txgain_LE1M_adv(1), iqm_max_txgain_LE2M_adv(1), iqm_max_txgain_LR(1)*/
	/* S1(9dBm): */
	0x82, 0x02, 0x08, 0x4F, 0x53, 0x53, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F,

	/* RF: iqm_max_txgain_LE1M_2402(1), iqm_max_txgain_LE1M_2480(1), iqm_max_txgain_LE2M_2402(1), iqm_max_txgain_LE2M_2480(1)*/
	0x8A, 0x02, 0x04, 0x70, 0x70, 0x70, 0x70
};
unsigned int hci_init_config_len_s1 = sizeof(hci_init_config_s1);

unsigned char *hci_init_config = NULL;
unsigned int hci_init_config_len = 0;

static uint8_t  hci_cfg_bd_addr[HCI_MAC_ADDR_LEN] = {0};
static uint32_t hci_cfg_flag                      = 0;

extern int wifi_set_ips_internal(u8 enable);

void hci_platform_cfg_bd_addr(uint8_t *bdaddr)
{
	for (uint8_t i = 0; i < HCI_MAC_ADDR_LEN; i++) {
		hci_cfg_bd_addr[i] = bdaddr[i];
	}

	hci_cfg_flag |= HCI_CFG_BD_ADDR;
}

int hci_platform_get_write_phy_efuse_data(uint8_t *data, uint8_t len)
{
	(void)len;
	uint8_t *pefuse = hci_phy_efuse;
	uint8_t *pdata = data;

	memcpy(pdata, pefuse, 22);
	pdata += 22;
	pefuse += 23;
	memcpy(pdata, pefuse, 1);
	pdata += 1;
	pefuse += 2;
	memcpy(pdata, pefuse, 26);
	pdata += 26;
	pefuse += 27;
	memcpy(pdata, pefuse, 60);

	return HCI_SUCCESS;
}

uint8_t hci_platform_get_rom_ver(void)
{
	return EFUSE_GetChipVersion() + 1; // 2: B cut, 3: C cut
}

bool hci_platform_check_lmp_subver(uint16_t lmp_subver)
{
	if (HCI_DEFAULT_LMP_SUBVER == lmp_subver) {
		return true;
	} else {
		return false;
	}
}

static uint8_t hci_platform_read_efuse(void)
{
	uint8_t i, *pbuf, val;

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

	/* Parse BT RF PATH */
	if (bt_ant_switch == 0xFF) { /* Priority of ATM2=ant is higher than efuse in mp image. */
		/* Bit[5]: Radio on/off type
		0: combine with WiFi/BT SEL Shared Port
		1: individual /BT SEL Dedicated Port */
		val = pbuf[0x133];
		if (val != 0xff) {
			if (val & BIT5) {
				bt_ant_switch = ANT_S0;
			} else {
				bt_ant_switch = ANT_S1;
			}
		} else {
			bt_ant_switch = ANT_S0; /* default use S0 if efuse is not programed */
		}
	}

	BT_LOGA("bt ant %d\r\n", bt_ant_switch);

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

void hci_platform_set_tx_power_gain_index(uint32_t index)
{
	bt_manual_tx_power_gain_enable = 1;
	bt_manual_gain_index_le = (uint8_t)(index & 0xFF);
	bt_manual_gain_index_br = (uint8_t)((index >> 8) & 0xFF);
	bt_manual_gain_index_edr2m = (uint8_t)((index >> 16) & 0xFF);
	bt_manual_gain_index_edr3m = (uint8_t)((index >> 24) & 0xFF);
}

void hci_platform_set_antenna(uint8_t ant)
{
	bt_ant_switch = ant;
}

static uint8_t hci_platform_parse_config(void)
{
	uint8_t *p, i;
	uint16_t entry_offset, entry_len;

	if (bt_ant_switch == ANT_S0) {
		hci_init_config = hci_init_config_s0;
		hci_init_config_len = hci_init_config_len_s0;
	} else {
		hci_init_config = hci_init_config_s1;
		hci_init_config_len = hci_init_config_len_s1;
	}

	if (hci_init_config_len <= HCI_CONFIG_HDR_LEN) {
		return HCI_IGNORE;
	}

	p = hci_init_config;
	if (HCI_CONFIG_SIGNATURE != *(uint32_t *)(p)) {
		return HCI_FAIL;
	}

	*(uint16_t *)(p + 4) = (uint16_t)(hci_init_config_len - HCI_CONFIG_HDR_LEN);

	p += HCI_CONFIG_HDR_LEN;
	while (p < hci_init_config + hci_init_config_len) {
		entry_offset = *(uint16_t *)(p);
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
		case 0x0282:
			for (i = 0; i < entry_len; i++) {
				if (bt_manual_tx_power_gain_enable) {
					if (i == 0) { // br
						BT_LOGA("0x0282 BR gain 0x%02x\r\n", bt_manual_gain_index_br);
						p[i] = bt_manual_gain_index_br;
					} else if (i == 1) { // edr 2m
						BT_LOGA("0x0282 EDR 2M gain 0x%02x\r\n", bt_manual_gain_index_edr2m);
						p[i] = bt_manual_gain_index_edr2m;
					} else if (i == 2) { // edr 3m
						BT_LOGA("0x0282 EDR 3M gain 0x%02x\r\n", bt_manual_gain_index_edr3m);
						p[i] = bt_manual_gain_index_edr3m;
					} else { // le
						BT_LOGA("0x0282 LE gain 0x%02x\r\n", bt_manual_gain_index_le);
						p[i] = bt_manual_gain_index_le;
					}
				} else if (hci_lgc_efuse[LEFUSE(0x1c8 + i)] != 0xff) {
					p[i] = hci_lgc_efuse[LEFUSE(0x1c8 + i)];
				}
			}
			break;
		case 0x028a:
			for (i = 0; i < entry_len; i++) {
				if (bt_manual_tx_power_gain_enable) {
					BT_LOGA("0x028a LE gain 0x%02x\r\n", bt_manual_gain_index_le);
					p[i] = bt_manual_gain_index_le;
				} else if (hci_lgc_efuse[LEFUSE(0x1d0 + i)] != 0xff) {
					p[i] = hci_lgc_efuse[LEFUSE(0x1d0 + i)];
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
	set_reg_value(0x42008200, BIT9, 1);                 //enable power cut of BTON
	osif_delay(5);
	set_reg_value(0x42008204, BIT9, 0);                 //disable ISO of BTON
	osif_delay(5);
	set_reg_value(0x42008250, BIT21 | BIT22, 3);
	osif_delay(5);
	set_reg_value(0x42008250, BIT1 | BIT2, 3);          //enable BT AFE & BT S0 RF, BT S1 also enable S0 RF
	osif_delay(5);
	if (bt_ant_switch == ANT_S0) {
		set_reg_value(0x42008250, BIT0, 0);             //BT use BT RFAFE
		osif_delay(5);
	} else {
		set_reg_value(0x42008250, BIT0, 1);             //BT use WL RFAFE
		osif_delay(5);
		set_reg_value(0x42008208, BIT24, 1);            //enable WL RFAFE control circuit
		osif_delay(5);
		set_reg_value(0x42008940, BIT5 | BIT6, 3);      //enable WL RFAFE
		osif_delay(5);
	}
	set_reg_value(0x42008200, BIT25, 1);                //Release BTON por,BT Memory
	osif_delay(5);
	set_reg_value(0x42008208, BIT13, 1);                //Release BTON reset
	osif_delay(5);
	if (HCI_BT_KEEP_WAKE) {
		set_reg_value(0x42008250, BIT14, 1);            //enable HOST_WAKE_BT No GPIO
		osif_delay(5);
		set_reg_value(0x42008250, BIT13, 1);            //HOST_WAKE_BT
		osif_delay(5);
	}
}

void bt_power_off(void)
{
	set_reg_value(0x42008208, BIT13, 0);                //BTON reset
	osif_delay(5);
	set_reg_value(0x42008200, BIT25, 0);                //release BTON por, BT memory
	osif_delay(5);
	if (bt_ant_switch == ANT_S0) {
		set_reg_value(0x42008250, BIT1 | BIT2, 0);      //disable BT AFE & BT S0 RF
		osif_delay(5);
	} else {
#if defined(CONFIG_WLAN) && CONFIG_WLAN
		if (!(wifi_is_running(WLAN0_IDX) || wifi_is_running(WLAN1_IDX)))
#endif
		{
			set_reg_value(0x42008940, BIT5 | BIT6, 0);  //disable RFAFE (if WIFI active, keep 2'b11)
			osif_delay(5);
			set_reg_value(0x42008208, BIT24, 0);        //disable WL RFAFE control circuit (if WIFI active, keep 1'b1)
			osif_delay(5);
		}
		set_reg_value(0x42008250, BIT1 | BIT2, 0);      //disable BT AFE & BT S0 RF
		osif_delay(5);
	}
	set_reg_value(0x42008204, BIT9, 1);                 //enable ISO of BTON
	osif_delay(5);
	set_reg_value(0x42008200, BIT9, 0);                 //disable power cut of BTON
	osif_delay(5);
}

void hci_platform_controller_reset(void)
{
	/* BT Controller Power */
	bt_power_on();

	BT_LOGD("BT Reset OK!\r\n");
}

bool rtk_bt_pre_enable(void)
{
	uint32_t lock_status;

#if defined(CONFIG_WLAN) && CONFIG_WLAN
	if (bt_ant_switch == ANT_S1) {
		if (!(wifi_is_running(WLAN0_IDX) || wifi_is_running(WLAN1_IDX))) {
			BT_LOGE("WiFi is OFF! Please Restart BT after Wifi on!\r\n");
			return false;
		}

		if (!hci_is_mp_mode()) {
			wifi_set_lps_enable(FALSE);
			wifi_set_ips_internal(FALSE);
		}
	}
#endif

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

#if defined(CONFIG_WLAN) && CONFIG_WLAN
	if (bt_ant_switch == ANT_S1) {
		if (!hci_is_mp_mode()) {
			wifi_set_lps_enable(wifi_user_config.lps_enable);
			wifi_set_ips_internal(wifi_user_config.ips_enable);
		}
	}
#endif
}

uint8_t hci_platform_init(void)
{
	/* Read Efuse and Parse Configbuf */
	/* bt_ant_switch is set in hci_platform_read_efuse(), used in hci_platform_parse_config() rtk_bt_pre_enable() */
	if (HCI_FAIL == hci_platform_read_efuse()) {
		return HCI_FAIL;
	}

	if (HCI_FAIL == hci_platform_parse_config()) {
		return HCI_FAIL;
	}

	if (rtk_bt_pre_enable() == false) {
		BT_LOGE("rtk_bt_pre_enable fail!\r\n");
		return HCI_FAIL;
	}

	if (!CHECK_CFG_SW(CFG_SW_BT_FW_LOG)) {
		rtk_bt_fw_log_open();
		BT_LOGA("FW LOG OPEN\r\n");
#if 0
		Pinmux_Config(_PB_10, PINMUX_FUNCTION_UART_RTSCTS);
		PAD_PullCtrl(_PB_10, GPIO_PuPd_UP);
#endif
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

uint8_t hci_platform_record_chipid(uint8_t chipid)
{
	if (chipid == 2 && hci_platform_get_rom_ver() >= 3) {
		return 3;
	}
	return chipid;
}

void hci_platform_get_config(uint8_t **buf, uint16_t *len)
{
	*buf = hci_init_config;
	*len = hci_init_config_len;
}

void hci_platform_debug_enable(void)
{
	uint32_t data;

	/* keep bt wakeup from dlps/lps */
	set_reg_value(0x42008250, BIT14, 1);        //enable HOST_WAKE_BT No GPIO
	osif_delay(5);
	set_reg_value(0x42008250, BIT13, 1);        //HOST_WAKE_BT
	osif_delay(5);
	while (1) {
		osif_delay(100);
		data = HAL_READ32(0x42008254, 0) & 0x1F; // 0x42008254 [0:4]
		if (data == 4) {
			/* bt active */
			BT_LOGD("bt active\r\n");
			break;
		} else {
			BT_LOGD("wait for bt wakeup from sleep\r\n");
		}
	}
	BT_LOGD("BT wakeup from sleep\r\n");

	set_reg_value(0x42008BF8, BIT0, 0);                                     //swd close
	osif_delay(5);
	set_reg_value(0x42008A08, BIT0 | BIT1 | BIT2 | BIT3 | BIT4, 24);        //Uart tx PA2
	osif_delay(5);
	set_reg_value(0x42008A0C, BIT0 | BIT1 | BIT2 | BIT3 | BIT4, 24);        //Uart rx PA3
	osif_delay(5);
	set_reg_value(0x42008A10, BIT0 | BIT1 | BIT2 | BIT3 | BIT4, 24);        //RTS PA4
	osif_delay(5);
	set_reg_value(0x42008A14, BIT0 | BIT1 | BIT2 | BIT3 | BIT4, 24);        //CTS PA5
	osif_delay(5);
	set_reg_value(0x42008250, BIT19, 1);                                    //pinmux
	osif_delay(5);
}