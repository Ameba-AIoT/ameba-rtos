/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include "platform_autoconf.h"
#include <stdio.h>
#include <string.h>
#include <osif.h>
#include <basic_types.h>
#include <bt_api_config.h>
#include <bt_vendor_config.h>
#include <rtk_bt_gap.h>
#include "rtk_bt_def.h"
#include "rtk_bt_vendor.h"
#include "rtk_bt_le_gap.h"
#include "bt_debug.h"
#if defined(RTK_BLE_TX_SOF_EOF_INDICATION) && RTK_BLE_TX_SOF_EOF_INDICATION
#include "wifi_reg_page0.h"
#include "ameba_soc.h"
#define BT_SCB_IRQ_PRIO   3
typedef void(*p_func_cb)(uint8_t ind);
static p_func_cb g_rtk_bt_scoreboard_isr_cb = NULL;

extern int wifi_set_ips_internal(u8 enable);
static bool need_restore = false;
#endif

_WEAK void hci_platform_set_tx_power_gain_index(uint32_t index)
{
	(void)index;
	BT_LOGE("Set tx power gain is not supported!\r\n");
}

_WEAK void hci_platform_set_antenna(uint8_t ant)
{
	(void)ant;
	BT_LOGE("Set antenna is not supported!\r\n");
}

_WEAK void hci_platform_debug_enable(void)
{
	BT_LOGE("HCI debug is not supported!\r\n");
}

/* -------------------------------- Functions ------------------------------*/

void rtk_bt_set_bt_tx_power_gain_index(uint32_t index)
{
	hci_platform_set_tx_power_gain_index(index);
}

void rtk_bt_set_bt_antenna(uint8_t ant)
{
	hci_platform_set_antenna(ant);
}

void rtk_bt_hci_debug_enable(void)
{
	hci_platform_debug_enable();
}

void rtk_bt_sleep_mode(unsigned int mode)
{
#if defined(VENDOR_CMD_BT_SLEEP_MODE_SUPPORT) && VENDOR_CMD_BT_SLEEP_MODE_SUPPORT
	uint8_t data[4] = {0};
	rtk_bt_gap_vendor_cmd_param_t param;

	data[0] = mode & 0xFF;          //0:lps, 1:dlps, 2:active
	data[1] = (mode >> 8) & 0xFF;   //0:log off, 1:log on
	data[2] = (mode >> 16) & 0xFF;  //minimum adv slot[LSB] to enter sleep
	data[3] = (mode >> 24) & 0xFF;  //minimum adv slot[MSB] to enter sleep

	param.op = VENDOR_CMD_BT_SLEEP_MODE_OPCODE;
	param.len = 4;
	param.cmd_param = data;

	rtk_bt_gap_vendor_cmd_req(&param);
#else
	(void)mode;
	BT_LOGE("Set sleep mode is not supported!\r\n");
#endif
}

uint16_t rtk_bt_set_tx_power(rtk_bt_vendor_tx_power_param_t *tx_power)
{
#if defined(VENDOR_CMD_SET_TX_POWER_SUPPORT) && VENDOR_CMD_SET_TX_POWER_SUPPORT
	rtk_bt_gap_vendor_cmd_param_t param;
	uint8_t data[5] = {0};

	if (0 == tx_power->tx_power_type) {
		data[0] = SUB_CMD_SET_ADV_TX_POWER;
		data[1] = tx_power->adv_tx_power.type;
		data[2] = tx_power->tx_gain;
		param.len = 3;
	} else if (1 == tx_power->tx_power_type) {
		uint8_t conn_id = 0;
		if (RTK_BT_OK != rtk_bt_le_gap_get_conn_id(tx_power->conn_tx_power.conn_handle, &conn_id)) {
			BT_LOGE("%s: conn_handle %d is not connect!\r\n", __func__, tx_power->conn_tx_power.conn_handle);
			return RTK_BT_FAIL;
		}
		data[0] = SUB_CMD_SET_CONN_TX_POWER;
		data[1] = tx_power->conn_tx_power.conn_handle & 0xFF;
		data[2] = (tx_power->conn_tx_power.conn_handle >> 8) & 0xFF;
		data[3] = tx_power->conn_tx_power.is_reset;
		data[4] = tx_power->tx_gain;
		param.len = 5;
	} else {
		BT_LOGE("%s: wrong tx power type!\r\n", __func__);
		return RTK_BT_FAIL;
	}

	param.op = VENDOR_CMD_SET_TX_POWER_OPCODE;
	param.cmd_param = data;

	return rtk_bt_gap_vendor_cmd_req(&param);
#else
	(void)tx_power;
	BT_LOGE("Set tx power is not supported!\r\n");
	return RTK_BT_FAIL;
#endif
}

#if defined(RTK_BLE_TX_SOF_EOF_INDICATION) && RTK_BLE_TX_SOF_EOF_INDICATION
uint16_t rtk_bt_le_sof_eof_ctrl(uint16_t conn_handle, uint8_t enable)
{
#if defined(VENDOR_CMD_SOF_EOF_CTRL_SUPPORT) && VENDOR_CMD_SOF_EOF_CTRL_SUPPORT
	uint8_t data[3] = {0};
	rtk_bt_gap_vendor_cmd_param_t param = {0};

	data[0] = enable;           //0:disable, 1:enable
	data[1] = (conn_handle) & 0xFF;
	data[2] = (conn_handle >> 8) & 0xFF;

	param.op = VENDOR_CMD_SOF_EOF_CTRL_OPCODE;
	param.len = 3;
	param.cmd_param = data;

	return rtk_bt_gap_vendor_cmd_req(&param);
#else
	(void)conn_handle;
	(void)enable;
	BT_LOGE("SOF and EOF control is not supported!\r\n");
	return RTK_BT_FAIL;
#endif
}

void rtk_bt_scoreboard_isr_handler(void)
{
	rtk_bt_le_sof_eof_ind_t ind = RTK_BT_LE_TX_SOF;
	uint32_t value = 0;
	if (HAL_READ32(WIFI_REG_BASE, REG_FSISR_V1) & BIT_FS_BTON_STS_UPDATE_INT) {
		//Clear BT dedicated SCB int ISR, MAC Reg 0x44[0]
		HAL_WRITE32(WIFI_REG_BASE, REG_FSISR_V1, BIT_FS_BTON_STS_UPDATE_INT);
		value = HAL_READ32(WIFI_REG_BASE, REG_SCOREBOARD_RD_BT2WL);
		//SCOREBOARD: bit 17: 0:SOF, 1:EOF; bit 17 is valid when bit 18 is 1
		//            bit 18: 0:disable, 1:enable
		//BT_LOGA("[KM4] BT2WL dedicated SCB int data is: 0x%x\r\n", value);
		if (value & BIT18) {
			if (value & BIT17) {
				ind = RTK_BT_LE_TX_EOF;
			} else {
				ind = RTK_BT_LE_TX_SOF;
			}
			if (g_rtk_bt_scoreboard_isr_cb) {
				g_rtk_bt_scoreboard_isr_cb((uint8_t)ind);
			} else {
				BT_LOGE("g_rtk_bt_scoreboard_isr_cb is not defined\r\n");
			}
		}
	}
}

static bool PeriphClockReq(u32 APBPeriph_in, u32 APBPeriph_Clock_in)
{
	u32 ClkRegIndx = (APBPeriph_Clock_in >> 30) & 0x03;
	u32 APBPeriph_Clock = APBPeriph_Clock_in & (~(BIT(31) | BIT(30)));

	u32 FuncRegIndx = (APBPeriph_in >> 30) & 0x03;
	u32 APBPeriph = APBPeriph_in & (~(BIT(31) | BIT(30)));

	u32 Reg = 0;

	//clock
	switch (ClkRegIndx) {
	case 0x0:
		Reg = REG_LSYS_CKE_GRP0;
		break;
	case 0x1:
		Reg = REG_LSYS_CKE_GRP1;
		break;
	case 0x2:
		Reg = REG_LSYS_CKE_GRP2;
		break;
	case 0x3:
		Reg = REG_AON_CLK;
		break;
	}

	if (APBPeriph_Clock_in != APBPeriph_CLOCK_NULL) {
		//BT_LOGA("reg = 0x%x bit= 0x%x read reg = 0x%x &_reg = 0x%x !!_reg = 0x%x\r\n",Reg, APBPeriph_Clock, HAL_READ32(SYSTEM_CTRL_BASE_LP, Reg),(HAL_READ32(SYSTEM_CTRL_BASE_LP, Reg) & APBPeriph_Clock),!!(HAL_READ32(SYSTEM_CTRL_BASE_LP, Reg) & APBPeriph_Clock));
		return !!(HAL_READ32(SYSTEM_CTRL_BASE_LP, Reg) & APBPeriph_Clock);
	}

	//function
	switch (FuncRegIndx) {
	case 0x0:
		Reg = REG_LSYS_FEN_GRP0;
		break;
	case 0x1:
		Reg = REG_LSYS_FEN_GRP1;
		break;
	case 0x2:
		Reg = REG_LSYS_FEN_GRP2;
		break;
	case 0x3:
		Reg = REG_AON_FEN;
		break;
	}

	if (APBPeriph_in != APBPeriph_NULL) {
		//BT_LOGA("reg = 0x%x bit= 0x%x read reg = 0x%x &_reg = 0x%x !!_reg = 0x%x\r\n",Reg, APBPeriph, HAL_READ32(SYSTEM_CTRL_BASE_LP, Reg),(HAL_READ32(SYSTEM_CTRL_BASE_LP, Reg) & APBPeriph),!!(HAL_READ32(SYSTEM_CTRL_BASE_LP, Reg) & APBPeriph));
		return !!(HAL_READ32(SYSTEM_CTRL_BASE_LP, Reg) & APBPeriph);
	}

	return false;
}

static bool rtk_bt_scoreboard_isr_enable(void)
{
	bool wmac_clk_on = false, wl_on = false;
	/****BT_SCB_IRQ (67# NP/AP INT VECTOR)*****/
	if (false == InterruptRegister((IRQ_FUN)rtk_bt_scoreboard_isr_handler, BT_SCB_IRQ, (uint32_t)NULL, BT_SCB_IRQ_PRIO)) {
		BT_LOGE("InterruptRegister for BT_SCB_IRQ fail\r\n");
		return false;
	}
	InterruptEn(BT_SCB_IRQ, BT_SCB_IRQ_PRIO);

	wmac_clk_on = PeriphClockReq(APBPeriph_NULL, APBPeriph_WMAC_CLOCK);
	wl_on = PeriphClockReq(APBPeriph_WLON, APBPeriph_CLOCK_NULL);

	//maybe use b_mac_pwr_ctrl_on related api later
	//scoreboard_isr need wl on and wmac clk on
	if (wmac_clk_on == false || wl_on == false) {
		wifi_set_ips_internal(FALSE);
		need_restore = true;
	}

	//Clear BT dedicated SCB int ISR, MAC Reg 0x44[0]
	HAL_WRITE32(WIFI_REG_BASE, REG_FSISR_V1, BIT_FS_BTON_STS_UPDATE_INT);
	BT_LOGA("Clear BT dedicated SCB int ISR\r\n");

	//Enable BT dedicated SCB int IMR, MAC Reg 0x40[0]
	HAL_WRITE32(WIFI_REG_BASE, REG_FSIMR_V1, HAL_READ32(WIFI_REG_BASE, REG_FSIMR_V1) | BIT_FS_BTON_STS_UPDATE_INT_EN);
	BT_LOGA("Enable BT dedicated SCB int IMR\r\n");

	return true;
}

static bool rtk_bt_scoreboard_isr_disable(void)
{
	InterruptDis(BT_SCB_IRQ);
	InterruptUnRegister(BT_SCB_IRQ);

	//Clear BT dedicated SCB int ISR, MAC Reg 0x44[0]
	HAL_WRITE32(WIFI_REG_BASE, REG_FSISR_V1, BIT_FS_BTON_STS_UPDATE_INT);
	BT_LOGA("Clear BT dedicated SCB int ISR\r\n");

	//Disable BT dedicated SCB int IMR, MAC Reg 0x40[0] = 0
	HAL_WRITE32(WIFI_REG_BASE, REG_FSIMR_V1, HAL_READ32(WIFI_REG_BASE, REG_FSIMR_V1) & ~BIT_FS_BTON_STS_UPDATE_INT_EN);
	BT_LOGA("Disable BT dedicated SCB int IMR\r\n");

	if (need_restore) {
		wifi_set_ips_internal(TRUE);
	}

	return true;
}

uint16_t rtk_bt_le_sof_eof_ind(uint16_t conn_handle, uint8_t enable, void *cb)
{
	uint16_t ret = RTK_BT_OK;
	uint8_t conn_id = 0;

	if (0 != rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id)) {
		BT_LOGE("%s: conn_handle %d is not connect!\r\n", __func__, conn_handle);
		return RTK_BT_FAIL;
	}

	if (enable == 1) {
		if (g_rtk_bt_scoreboard_isr_cb) {
			BT_LOGE("bt_scoreboard_isr is enabled!\r\n");
			return RTK_BT_OK;
		}
		//enable bt scoreboard isr
		if (false == rtk_bt_scoreboard_isr_enable()) {
			return RTK_BT_FAIL;
		}

		//save the callback function
		g_rtk_bt_scoreboard_isr_cb = (p_func_cb)cb;

	} else if (enable == 0) {
		if (g_rtk_bt_scoreboard_isr_cb == NULL) {
			BT_LOGE("bt_scoreboard_isr not enable!\r\n");
			return RTK_BT_OK;
		}

		//clear the callback function
		g_rtk_bt_scoreboard_isr_cb = NULL;

		//disable bt scoreboard isr
		if (false == rtk_bt_scoreboard_isr_disable()) {
			return RTK_BT_FAIL;
		}
	} else {
		BT_LOGE("%s unsupport value %d!\r\n", __func__, enable);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	//set the vendor cmd to inform fw start indicate sof and eof
	ret = rtk_bt_le_sof_eof_ctrl(conn_handle, enable);
	if (ret != RTK_BT_OK) {
		BT_LOGE("%s rtk_bt_le_sof_eof_ctrl fail, ret = 0x%x\r\n", __func__, ret);
		return ret;
	}

	return RTK_BT_OK;
}

#endif