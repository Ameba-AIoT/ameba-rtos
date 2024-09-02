/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include "osif.h"
#include "hci_config.h"
#include "hci/hci_common.h"
#include "hci_platform.h"
#include "bt_debug.h"

#define USE_HCI_H4 1
#if defined(USE_HCI_H4) && USE_HCI_H4
#define RESERVE_LEN 1
#elif defined(USE_HCI_H5) && USE_HCI_H5
#define RESERVE_LEN 4
#else
#define RESERVE_LEN 0
#endif

#if defined(hci_platform_START_RF_CALIBRATION) && hci_platform_START_RF_CALIBRATION
static uint8_t hci_process_start_rf_calibration(uint16_t opcode)
{
	(void)opcode;

	hci_platform_bt_rf_calibration();

	return HCI_SUCCESS;
}
#endif

#if defined(hci_platform_START_IQK) && hci_platform_START_IQK
static uint8_t hci_process_start_iqk(uint16_t opcode)
{
	/* OpCode: 0xFD4A, Data Len: Cmd(7), Event(6) */
	uint8_t buf_raw[RESERVE_LEN + 7];
	uint8_t *buf = buf_raw + RESERVE_LEN;

	if (HCI_SUCCESS == hci_platform_check_iqk()) {
		return HCI_IGNORE;
	}

	for (uint8_t i = 0; i < HCI_START_IQK_TIMES; i++) {
		buf[0] = (uint8_t)(opcode >> 0);
		buf[1] = (uint8_t)(opcode >> 8);
		buf[2] = (uint8_t)(HCI_IQK_DATA_LEN);
		buf[3] = (uint8_t)(hci_iqk_data[i].offset);
		buf[4] = (uint8_t)(hci_iqk_data[i].value >> 0);
		buf[5] = (uint8_t)(hci_iqk_data[i].value >> 8);
		buf[6] = (uint8_t)(0);

		if (HCI_SUCCESS != hci_sa_send(HCI_CMD, buf, 7, true)) {
			return HCI_FAIL;
		}

		/* Check Resp: OpCode and Status */
		if (buf[3] != (uint8_t)(opcode >> 0) || buf[4] != (uint8_t)(opcode >> 8) || buf[5] != 0x00) {
			return HCI_FAIL;
		}
	}

	if (HCI_SUCCESS != hci_platform_start_iqk()) {
		return HCI_FAIL;
	}

	return HCI_SUCCESS;
}
#endif

static uint8_t hci_process_read_local_ver(uint16_t opcode)
{
	/* OpCode: 0x1001, Data Len: Cmd(3), Event(14) */
	uint8_t buf_raw[RESERVE_LEN + 14];
	uint8_t *buf = buf_raw + RESERVE_LEN;

	buf[0] = (uint8_t)(opcode >> 0);
	buf[1] = (uint8_t)(opcode >> 8);
	buf[2] = (uint8_t)(0);

	if (HCI_SUCCESS != hci_sa_send(HCI_CMD, buf, 3, true)) {
		return HCI_FAIL;
	}

	/* Check Resp: OpCode and Status */
	if (buf[3] != (uint8_t)(opcode >> 0) || buf[4] != (uint8_t)(opcode >> 8) || buf[5] != 0x00) {
		return HCI_FAIL;
	}

	/* Get LMP Subversion and Check */
	uint16_t lmp_subver = ((uint16_t)buf[12]) | (((uint16_t)buf[13]) << 8);
	if (hci_platform_check_lmp_subver(lmp_subver) == false) {
		return HCI_FAIL;
	}

	return HCI_SUCCESS;
}

static uint8_t hci_process_read_rom_ver(uint16_t opcode)
{
	/* OpCode: 0xFC6D, Data Len: Cmd(3), Event(7) */
	uint8_t buf_raw[RESERVE_LEN + 7];
	uint8_t *buf = buf_raw + RESERVE_LEN;

	buf[0] = (uint8_t)(opcode >> 0);
	buf[1] = (uint8_t)(opcode >> 8);
	buf[2] = (uint8_t)(0);

	if (HCI_SUCCESS != hci_sa_send(HCI_CMD, buf, 3, true)) {
		return HCI_FAIL;
	}

	/* Check Resp: OpCode and Status */
	if (buf[3] != (uint8_t)(opcode >> 0) || buf[4] != (uint8_t)(opcode >> 8) || buf[5] != 0x00) {
		return HCI_FAIL;
	}

	/* Get Chip Id (Rom_Ver+1) and Find Patch */
	hci_patch_set_chipid(buf[6] + 1);

	return HCI_SUCCESS;
}

static uint8_t _hci_process_set_baudrate(uint16_t opcode, bool use_default)
{
	/* OpCode: 0xFC17, Data Len: Cmd(7), Event(6) */
	uint8_t buf_raw[RESERVE_LEN + 7];
	uint8_t *buf = buf_raw + RESERVE_LEN;

	buf[0] = (uint8_t)(opcode >> 0);
	buf[1] = (uint8_t)(opcode >> 8);
	buf[2] = sizeof(uint32_t);
	hci_get_baudrate(&buf[3], use_default);

	if (HCI_SUCCESS != hci_sa_send(HCI_CMD, buf, 7, true)) {
		return HCI_FAIL;
	}

	/* Check Resp: OpCode and Status */
	if (buf[3] != (uint8_t)(opcode >> 0) || buf[4] != (uint8_t)(opcode >> 8) || buf[5] != 0x00) {
		return HCI_FAIL;
	}

	/* Set Host-side baudrate */
	if (HCI_SUCCESS != hci_update_uart_baudrate(use_default)) {
		return HCI_FAIL;
	}

	return HCI_SUCCESS;
}

static uint8_t hci_process_update_baudrate(uint16_t opcode)
{
	return _hci_process_set_baudrate(opcode, false);
}

#if defined(hci_platform_MP_RESET_BAUDRATE) && hci_platform_MP_RESET_BAUDRATE
static uint8_t hci_process_reset_baudrate(uint16_t opcode)
{
	if (hci_is_mp_mode()) {
		return _hci_process_set_baudrate(opcode, true);
	}

	return HCI_IGNORE;
}
#endif

#if defined(hci_platform_DOWNLOAD_PATCH) && hci_platform_DOWNLOAD_PATCH
static uint8_t hci_process_download_patch(uint16_t opcode)
{
	/* OpCode: 0xFC20, Data Len: Cmd(256), Event(7) */
	uint8_t ret = HCI_SUCCESS;
	uint8_t buf_raw[RESERVE_LEN + 256];
	uint8_t *buf = buf_raw + RESERVE_LEN;

	ret = hci_downlod_patch_init();
	if (HCI_SUCCESS != ret) {
		goto dl_patch_done;
	}

	while (1) {
		buf[0] = (uint8_t)(opcode >> 0);
		buf[1] = (uint8_t)(opcode >> 8);
		ret = hci_get_patch_cmd_len(&buf[2]);
		if (HCI_SUCCESS != ret) {
			goto dl_patch_done;
		}

		ret = hci_get_patch_cmd_buf(&buf[3], buf[2]);
		if (HCI_SUCCESS != ret) {
			goto dl_patch_done;
		}

		ret = hci_sa_send(HCI_CMD, buf, buf[2] + 3, true);
		if (HCI_SUCCESS != ret) {
			goto dl_patch_done;
		}

		/* Check Resp: OpCode and Status */
		if (buf[3] != (uint8_t)(opcode >> 0) || buf[4] != (uint8_t)(opcode >> 8) || buf[5] != 0x00) {
			goto dl_patch_done;
		}

		/* Check the last patch fragment */
		if (buf[6] & 0x80) {
			break;
		}
	}

dl_patch_done:
	hci_downlod_patch_done();

	return ret;
}
#endif

#if defined(hci_platform_START_RX_DCK) && hci_platform_START_RX_DCK
static uint8_t hci_process_start_rx_dck(uint16_t opcode)
{
	(void)opcode;

	hci_platform_bt_rx_dck();

	return HCI_SUCCESS;
}
#endif

#if defined(hci_platform_WRITE_PHY_EFUSE) && hci_platform_WRITE_PHY_EFUSE
static uint8_t hci_process_write_phy_efuse(uint16_t opcode)
{
	/* OpCode: 0xFD91, Data Len: Cmd(HCI_WRITE_PHY_EFUSE_LEN+3), Event(6) */
	uint8_t buf_raw[RESERVE_LEN + HCI_WRITE_PHY_EFUSE_LEN + 3];
	uint8_t *buf = buf_raw + RESERVE_LEN;

	buf[0] = (uint8_t)(opcode >> 0);
	buf[1] = (uint8_t)(opcode >> 8);
	buf[2] = (uint8_t)(HCI_WRITE_PHY_EFUSE_LEN);
	if (HCI_SUCCESS != hci_platform_get_write_phy_efuse_data(&buf[3], buf[2])) {
		return HCI_FAIL;
	}

	if (HCI_SUCCESS != hci_sa_send(HCI_CMD, buf, HCI_WRITE_PHY_EFUSE_LEN + 3, true)) {
		return HCI_FAIL;
	}

	/* Check Resp: OpCode and Status */
	if (buf[3] != (uint8_t)(opcode >> 0) || buf[4] != (uint8_t)(opcode >> 8) || buf[5] != 0x00) {
		return HCI_FAIL;
	}

	return HCI_SUCCESS;
}
#endif

#if defined(hci_platform_WRITE_RX_ADCK) && hci_platform_WRITE_RX_ADCK
static uint8_t hci_process_write_rx_adck(uint16_t opcode)
{
	/* OpCode: 0xFD20, Data Len: Cmd(4), Event(6) */
	uint8_t buf_raw[RESERVE_LEN + 6];
	uint8_t *buf = buf_raw + RESERVE_LEN;

	buf[0] = (uint8_t)(opcode >> 0);
	buf[1] = (uint8_t)(opcode >> 8);
	buf[2] = (uint8_t)(1);
	if (HCI_SUCCESS != hci_platform_get_rx_adck_data(&buf[3], buf[2])) {
		return HCI_FAIL;
	}

#if 0
	BT_DUMPA("hci_process_write_rx_adck HCI CMD:", buf, RESERVE_LEN + 4);
#endif

	if (HCI_SUCCESS != hci_sa_send(HCI_CMD, buf, 4, true)) {
		return HCI_FAIL;
	}

	/* Check Resp: OpCode and Status */
	if (buf[3] != (uint8_t)(opcode >> 0) || buf[4] != (uint8_t)(opcode >> 8) || buf[5] != 0x00) {
		BT_DUMPA("hci_process_write_rx_adck HCI Event:", buf, RESERVE_LEN + 6);
		return HCI_FAIL;
	}

	return HCI_SUCCESS;
}
#endif

#if defined(hci_platform_SET_CUT_VER) && hci_platform_SET_CUT_VER
static uint8_t hci_process_set_cut_ver(uint16_t opcode)
{
	/* OpCode: 0xFDAC, Data Len: Cmd(4), Event(6) */
	uint8_t buf_raw[RESERVE_LEN + 6];
	uint8_t *buf = buf_raw + RESERVE_LEN;

	buf[0] = (uint8_t)(opcode >> 0);
	buf[1] = (uint8_t)(opcode >> 8);
	buf[2] = (uint8_t)(1);
	buf[3] = hci_platform_get_rom_ver();

	if (HCI_SUCCESS != hci_sa_send(HCI_CMD, buf, 4, true)) {
		return HCI_FAIL;
	}

	/* Check Resp: OpCode and Status */
	if (buf[3] != (uint8_t)(opcode >> 0) || buf[4] != (uint8_t)(opcode >> 8) || buf[5] != 0x00) {
		return HCI_FAIL;
	}

	return HCI_SUCCESS;
}
#endif

static uint8_t hci_process_hci_reset(uint16_t opcode)
{
	/* OpCode: 0x0C03, Data Len: Cmd(3), Event(6) */
	uint8_t buf_raw[RESERVE_LEN + 6];
	uint8_t *buf = buf_raw + RESERVE_LEN;

	buf[0] = (uint8_t)(opcode >> 0);
	buf[1] = (uint8_t)(opcode >> 8);
	buf[2] = (uint8_t)(0);

	if (HCI_SUCCESS != hci_sa_send(HCI_CMD, buf, 3, true)) {
		return HCI_FAIL;
	}

	/* Check Resp: OpCode and Status */
	if (buf[3] != (uint8_t)(opcode >> 0) || buf[4] != (uint8_t)(opcode >> 8) || buf[5] != 0x00) {
		return HCI_FAIL;
	}

	return HCI_SUCCESS;
}

static struct {
	uint16_t opcode;
	uint8_t (*func)(uint16_t opcode);
} hci_process_table[] = {
#if defined(hci_platform_START_RF_CALIBRATION) && hci_platform_START_RF_CALIBRATION
	{0,      hci_process_start_rf_calibration},
#endif
#if defined(hci_platform_START_IQK) && hci_platform_START_IQK
	{0xFD4A, hci_process_start_iqk},
#endif
	{0x1001, hci_process_read_local_ver},
	{0xFC6D, hci_process_read_rom_ver},
	{0xFC17, hci_process_update_baudrate},
#if defined(hci_platform_DOWNLOAD_PATCH) && hci_platform_DOWNLOAD_PATCH
	{0xFC20, hci_process_download_patch},
#endif
#if defined(hci_platform_MP_RESET_BAUDRATE) && hci_platform_MP_RESET_BAUDRATE
	{0xFC17, hci_process_reset_baudrate},
#endif
#if defined(hci_platform_START_RX_DCK) && hci_platform_START_RX_DCK
	{0,      hci_process_start_rx_dck},
#endif
#if defined(hci_platform_WRITE_PHY_EFUSE) && hci_platform_WRITE_PHY_EFUSE
	{0xFD91, hci_process_write_phy_efuse},
#endif
#if defined(hci_platform_WRITE_RX_ADCK) && hci_platform_WRITE_RX_ADCK
	{0xFD20, hci_process_write_rx_adck},
#endif
#if defined(hci_platform_SET_CUT_VER) && hci_platform_SET_CUT_VER
	{0xFDAC, hci_process_set_cut_ver},
#endif
	{0x0C03, hci_process_hci_reset},
	{0,      NULL}
};

uint8_t hci_process(void)
{
	uint8_t step = 0;

	while (hci_process_table[step].func) {
		if (HCI_FAIL == hci_process_table[step].func(hci_process_table[step].opcode)) {
			BT_LOGE("Opcode 0x%x fail!\r\n", hci_process_table[step].opcode);
			return HCI_FAIL;
		}
		step++;
	}

	return HCI_SUCCESS;
}
