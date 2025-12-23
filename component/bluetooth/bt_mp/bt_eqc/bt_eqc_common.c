#include <bt_eqc_api.h>
#include <stdio.h>
#include <string.h>
#include <osif.h>
#include <basic_types.h>
#include "bt_debug.h"
#if defined(CONFIG_BT_COEXIST)
#include "rtw_coex_host_api.h"
#else
//#error "Please Enable Coexist!!!"
#endif

uint8_t eqc_role = 0xFF;
uint8_t dut_address[6] = {0};
uint8_t eut_address[6] = {0};
uint8_t EQC_FLAG = EQC_DEINIT;
uint16_t eqc_connect_handle = 0xFFFF;
rtk_bt_le_addr_t conn_Event_addr = {0};
extern uint8_t DUT_address[8][6];
extern uint8_t EUT_address[8][6];
void *eqc_task_handle = NULL;
void *eqc_evt_queue_handle = NULL;
extern void hci_platform_cfg_bd_addr(uint8_t *bdaddr);

bool eqc_le_send_msg(E_EVENT_MSG *event_msg)
{
	if (eqc_evt_queue_handle) {
		if (false == osif_msg_send(eqc_evt_queue_handle, event_msg, 0)) {
			return false;
		}
	} else {
		BT_LOGE("eqc_evt_queue_handle is NULL!\r\n");
		return false;
	}
	return true;
}

void eqc_le_init(uint8_t *own_addr)
{
	bool bMngrResult = false;
	bool cfg_addr = false;
	//Retry Limit
	uint8_t iRetry_Limit = 3;
	uint8_t iRetry_Count = 0;

	for (uint8_t i = 0; i < 6; i ++) {
		if (own_addr[i] != 0x0) {
			cfg_addr = true;
			break;
		}
	}
	if (cfg_addr) {
		hci_platform_cfg_bd_addr(own_addr);
	}

	rtk_bt_eqc_power_on();

	iRetry_Count = 0;
	do {
		bMngrResult = rtk_bt_eqc_set_event_mask();
		if (bMngrResult) {
			break;
		}

		iRetry_Count++;
		BT_LOGA("Initial device - Retry Set Event mask ... %i\n\r", iRetry_Count);
	} while (iRetry_Count < iRetry_Limit);

	if (bMngrResult) {
		BT_LOGA("EQC LE TEST INIT SUCCESS...\n\r");
		EQC_FLAG = EQC_INIT;
	} else {
		BT_LOGE("EQC LE TEST INIT FAIL...\n\r");
		EQC_FLAG = EQC_INIT_FAIL;
	}
}

bool eqc_le_deinit(void)
{
	if (!(EQC_FLAG & EQC_INIT)) {
		return false;
	}
	rtk_bt_eqc_power_off();

	return true;
}

void eqc_le_check_connection_complete_event(uint8_t *buf, uint16_t buf_len)
{
	(void)buf_len;
	uint8_t  status = 0x0;
	uint16_t connection_interval = 0x0;
	uint16_t peri_latency = 0x0;
	uint16_t superv_timeout = 0x0;

	status = buf[3];
	eqc_connect_handle = (buf[5] << 8) + buf[4];
	eqc_role = buf[6];
	conn_Event_addr.type = buf[7];
	memcpy(conn_Event_addr.addr_val, &buf[8], 6);
	connection_interval = (buf[15] << 8) + buf[14];
	peri_latency = (buf[17] << 8) + buf[16];
	superv_timeout = (buf[19] << 8) + buf[18];

	if (0x0 == eqc_role) {
		BT_LOGA("As central role\n\r");
	} else if (0x1 == eqc_role) {
		BT_LOGA("As peripheral role\n\r");
	}

	BT_LOGA("status = 0x%02x\n\r", status);
	BT_LOGA("connect handle = 0x%02x\n\r", eqc_connect_handle);
	BT_LOGA("connection_interval = 0x%02x\n\r", connection_interval);
	BT_LOGA("peri_latency = 0x%02x\n\r", peri_latency);
	BT_LOGA("superv_timeout = 0x%02x\n\r", superv_timeout);
	BT_LOGA("peeraddresstype = 0x%02x\n\r", conn_Event_addr.type);
	BT_LOGA("peer_address: %02x:%02x:%02x:%02x:%02x:%02x \n\r", conn_Event_addr.addr_val[5], conn_Event_addr.addr_val[4], conn_Event_addr.addr_val[3],
			conn_Event_addr.addr_val[2], conn_Event_addr.addr_val[1], conn_Event_addr.addr_val[0]);

	if (status) {
		BT_LOGE("link conn fail status 0x%x\n\r", status);
		EQC_FLAG |= EQC_CLIEN_CONN_CANCEL;
		return;
	}

	//Check BD address is valid that not 0x000000000000
	for (uint8_t i = 0; i < 6; i++) {
		if (conn_Event_addr.addr_val[i] > 0x00) {
			EQC_FLAG |= EQC_CLIEN_CONN;
			break;
		}
	}
}

void eqc_le_check_enhance_connection_complete_event(uint8_t *buf, uint16_t buf_len)
{
	(void)buf_len;
	uint8_t status = 0x0;
	uint8_t local_resolvable_private_address[6] = {0};
	uint8_t peer_resolvable_private_address[6] = {0};
	uint16_t connection_interval = 0x0;
	uint16_t peri_latency = 0x0;
	uint16_t superv_timeout = 0x0;
	uint8_t central_clock_accuracy = 0x0;

	status = buf[3];
	eqc_connect_handle = (buf[5] << 8) + buf[4];
	eqc_role = buf[6];
	conn_Event_addr.type = buf[7];
	memcpy(conn_Event_addr.addr_val, &buf[8], 6);
	memcpy(local_resolvable_private_address, &buf[14], 6);
	memcpy(peer_resolvable_private_address, &buf[20], 6);

	connection_interval = (buf[27] << 8) + buf[26];
	peri_latency = (buf[29] << 8) + buf[28];
	superv_timeout = (buf[31] << 8) + buf[30];
	central_clock_accuracy = buf[32];

	if (0x0 == eqc_role) {
		BT_LOGA("As central role\n\r");
	} else if (0x1 == eqc_role) {
		BT_LOGA("As peripheral role\n\r");
	}

	BT_LOGA("status = 0x%02x\n\r", status);
	BT_LOGA("connect handle = 0x%02x\n\r", eqc_connect_handle);
	BT_LOGA("connection_interval = 0x%02x\n\r", connection_interval);
	BT_LOGA("peri_latency = 0x%02x\n\r", peri_latency);
	BT_LOGA("superv_timeout = 0x%02x\n\r", superv_timeout);
	BT_LOGA("central_clock_accuracy = 0x%02x\n\r", central_clock_accuracy);
	BT_LOGA("peeraddresstype = 0x%02x\n\r", conn_Event_addr.type);
	BT_LOGA("peer_address: %02x:%02x:%02x:%02x:%02x:%02x\n\r", conn_Event_addr.addr_val[5], conn_Event_addr.addr_val[4], conn_Event_addr.addr_val[3],
			conn_Event_addr.addr_val[2], conn_Event_addr.addr_val[1], conn_Event_addr.addr_val[0]);
	BT_LOGA("local_resolvable_private_address: %02x:%02x:%02x:%02x:%02x:%02x\n\r", local_resolvable_private_address[0], local_resolvable_private_address[1],
			local_resolvable_private_address[2],
			local_resolvable_private_address[3], local_resolvable_private_address[4], local_resolvable_private_address[5]);

	BT_LOGA("peer_resolvable_private_address: %02x:%02x:%02x:%02x:%02x:%02x\n\r", peer_resolvable_private_address[0], peer_resolvable_private_address[1],
			peer_resolvable_private_address[2],
			peer_resolvable_private_address[3], peer_resolvable_private_address[4], peer_resolvable_private_address[5]);

	if (status) {
		BT_LOGE("link conn fail status 0x%x\n\r", status);
		EQC_FLAG |= EQC_CLIEN_CONN_CANCEL;
		return;
	}
	//Check BD address is valid that not 0x000000000000
	for (uint8_t i = 0; i < 6; i++) {
		if (conn_Event_addr.addr_val[i] > 0x00) {
			EQC_FLAG |= EQC_CLIEN_CONN;
			break;
		}
	}
}

void eqc_le_check_disconnection_complete_event(uint8_t *buf, uint16_t buf_len)
{
	(void)buf_len;
	uint8_t status = 0x0;
	uint8_t disc_reason = 0x0;
	uint8_t IsValid = 0;
	uint16_t handle = 0x0;

	status = buf[2];
	handle = (buf[4] << 8) + buf[3];
	disc_reason = buf[5];

	BT_LOGA("status = 0x%02x\n\r", status);
	BT_LOGA("disc_handle = 0x%02x\n\r", handle);
	BT_LOGA("disc_reason 0x%0x\n\r", disc_reason);

	if (0x0 == status) {
		IsValid = 1;
	}

	if (1 == IsValid) {
		if (1 == eqc_role) {
			rtk_bt_eqc_le_start_advertising(1);
		} else if (0 == eqc_role) {
			if (0x3e == disc_reason) {
				EQC_FLAG |= EQC_CLIEN_DISCONN_3E;
			} else {
				EQC_FLAG |= EQC_CLIEN_DISCONN_OT;
			}
		}
	}
}

void eqc_le_handle_event_from_hci(E_EVENT_MSG *event_msg)
{
	uint8_t ucEvent[256] = {0};
	uint8_t event_code = 0;
	uint8_t subevent_code = 0;
	uint8_t len = event_msg->len;

	memcpy(ucEvent, event_msg->buf, len);
	event_code = ucEvent[0];
	subevent_code = ucEvent[2];

	if (0x3E == event_code) {   //LE meta event
		switch (subevent_code) {
		case 0x01: {  //LE Connection Complete event
			eqc_le_check_connection_complete_event(ucEvent, len);
		}
		break;
		case 0x0A:    //HCI_LE_Enhanced_Connection_Complete [v1] event
		case 0x29: {  //HCI_LE_Enhanced_Connection_Complete [v2] event
			eqc_le_check_enhance_connection_complete_event(ucEvent, len);
		}
		break;
		default: {
			BT_LOGA("subevent code 0x%x\r\n", subevent_code);
		}
		break;
		}
	} else if (0x05 == event_code) {   //HCI_Disconnection_Complete
		eqc_le_check_disconnection_complete_event(ucEvent, len);
	} else {
		BT_LOGA("event code 0x%x\r\n", event_code);
	}

	if (event_msg && event_msg->buf) {
		osif_mem_free(event_msg->buf);
	}
}

void eqc_le_taskentry(void *ctx)
{
	(void)ctx;
	E_EVENT_MSG event_msg;
	while (true) {
		if (true == osif_msg_recv(eqc_evt_queue_handle, &event_msg, 0xFFFFFFFF)) {
			if (0xFF == event_msg.len) {//deinit, break the while loop
				eqc_le_deinit();
				break;
			}
			if (0xFE == event_msg.len) {//init
				eqc_le_init(dut_address);
			} else {
				eqc_le_handle_event_from_hci(&event_msg);
			}
		}
	}

	osif_msg_queue_delete(eqc_evt_queue_handle);
	EQC_FLAG = EQC_DEINIT;
	BT_LOGA("eqc_le_task deinit\n\r");
	osif_task_delete(NULL);
}

void eqc_le_task_init(void)
{
	if (false == osif_msg_queue_create(&eqc_evt_queue_handle, 0x20, sizeof(E_EVENT_MSG))) {
		BT_LOGE("evt_queue_handle create fail!\r\n");
		return;
	}

	if (false == osif_task_create(&eqc_task_handle, "eqc_le_task", eqc_le_taskentry, NULL, 256 * 6, 4)) {
		BT_LOGE("eqc_task_handle create fail!\r\n");
		return;
	}
}

bool eqc_le_start_advertising(void)
{
	uint8_t bMngrResult = false;
	bMngrResult = rtk_bt_eqc_le_start_advertising(1);

	if (!bMngrResult) {
		BT_LOGE("Start Le advertising fail\n\r");
		return false;
	} else {
		BT_LOGA("Start Le advertising success\n\r");
		return true;
	}
}

void eqc_le_wait_bt_status(uint8_t flag)
{
	uint32_t ulTime_Limit = 2;
	uint32_t ulTime_Start = 0;
	uint32_t ulTime_End = 0;
	uint32_t ulTime_Dur = 0;

	ulTime_Start = osif_sys_time_get();
	do {
		if (EQC_FLAG & flag) {
			break;
		}

		osif_delay(50);
		ulTime_End = osif_sys_time_get();
		ulTime_Dur = (ulTime_End - ulTime_Start) / 1000;
	} while (ulTime_Dur < ulTime_Limit);
}

uint8_t eqc_le_client_judge_disconn(void)
{
	uint8_t bMngrResult = EQC_FAIL;
	uint32_t ulTime_Limit = 1;
	uint32_t ulTime_Start = 0;
	uint32_t ulTime_End = 0;
	uint32_t ulTime_Dur = 0;

	ulTime_Start = osif_sys_time_get();
	do {
		if (EQC_FLAG & EQC_CLIEN_DISCONN_OT) {
			bMngrResult = EQC_DISC_FAIL;
			goto TEST_END;
		} else if (EQC_FLAG & EQC_CLIEN_DISCONN_3E) {
			bMngrResult = EQC_DISC_3E_FAIL;
			goto TEST_END;
		}
		osif_delay(50);
		ulTime_End = osif_sys_time_get();
		ulTime_Dur = (ulTime_End - ulTime_Start) / 1000;

	} while (ulTime_Dur < ulTime_Limit);

	bMngrResult = EQC_SUCCESS;

TEST_END:
	return bMngrResult;
}

uint8_t eqc_le_client_connectDevice(uint8_t *btaddr)
{
	uint8_t bMngrResult = false;
	uint32_t ulTime_Limit = 3;
	uint32_t ulTime_Start = 0;
	uint32_t ulTime_End = 0;
	uint32_t ulTime_Dur = 0;

	bMngrResult = rtk_bt_eqc_le_connect(btaddr);

	if (!bMngrResult) {
		BT_LOGE("LE Connect Device fail...\n\r");
		bMngrResult = EQC_API_FAIL;
		return bMngrResult;
	}

	ulTime_Start = osif_sys_time_get();
	do {
		if (EQC_FLAG & EQC_CLIEN_CONN) {  // conn success
			bMngrResult = eqc_le_client_judge_disconn();
			goto TEST_END;
		}

		osif_delay(100);
		ulTime_End = osif_sys_time_get();
		ulTime_Dur = (ulTime_End - ulTime_Start) / 1000;
	} while (ulTime_Dur < ulTime_Limit);

	bMngrResult = EQC_WAIT_FAIL;

TEST_END:
	return bMngrResult;
}

uint8_t eqc_le_read_current_channelmap(uint16_t conn_handle)
{
	uint8_t bMngrResult = false;
	bMngrResult = rtk_bt_eqc_le_read_current_channel_map(conn_handle);

	if (!bMngrResult) {
		BT_LOGE("LE Read current channel map fail...\n\r");
		return EQC_FAIL;
	} else {
		BT_LOGA("LE Read current channel map succcess...\n\r");
		return EQC_SUCCESS;
	}
}

uint8_t eqc_le_disconnect_device(uint16_t conn_handle)
{
	uint8_t bMngrResult = false;
	uint32_t ulTime_Limit = 2;
	uint32_t ulTime_Start = 0;
	uint32_t ulTime_End = 0;
	uint32_t ulTime_Dur = 0;
	uint8_t disc_reson = 0x13;

	bMngrResult = rtk_bt_eqc_le_disconnect(conn_handle, disc_reson);

	if (!bMngrResult) {
		BT_LOGE("Disconnect LE Device fail...\n\r");
		bMngrResult = EQC_API_FAIL;
		goto TEST_END;
	}

	ulTime_Start = osif_sys_time_get();
	do {
		BT_LOGA("Wait disconnect LE device...\n\r");
		if (EQC_FLAG & EQC_CLIEN_DISCONN_OT) {  // conn success
			bMngrResult = EQC_SUCCESS;
			goto TEST_END;
		}
		osif_delay(100);
		ulTime_End = osif_sys_time_get();
		ulTime_Dur = (ulTime_End - ulTime_Start) / 1000;

	} while (ulTime_Dur < ulTime_Limit);

TEST_END:
	return bMngrResult;
}

uint8_t eqc_le_client_connect_cancel(void)
{
	uint8_t bMngrResult = false;
	uint32_t ulTime_Limit = 3;
	uint32_t ulTime_Start = 0;
	uint32_t ulTime_End = 0;
	uint32_t ulTime_Dur = 0;

	EQC_FLAG = EQC_INIT;
	bMngrResult = rtk_bt_eqc_le_connect_cancel();

	if (!bMngrResult) {
		BT_LOGE("LE Cancel Connect Device fail...\n\r");
		bMngrResult = EQC_API_FAIL;
		return bMngrResult;
	}

	ulTime_Start = osif_sys_time_get();
	do {
		if (EQC_FLAG & EQC_CLIEN_CONN_CANCEL) {  // conn cancel success
			bMngrResult = EQC_SUCCESS;
			goto TEST_END;
		}

		osif_delay(100);
		ulTime_End = osif_sys_time_get();
		ulTime_Dur = (ulTime_End - ulTime_Start) / 1000;
	} while (ulTime_Dur < ulTime_Limit);

	bMngrResult = EQC_FAIL;

TEST_END:
	return bMngrResult;
}

bool eqc_le_client_test(uint8_t *btaddr)
{
	uint8_t bMngrResult = EQC_FAIL;
	uint8_t Func_Code = false;
	uint8_t ulFail_Count = 0;
	uint8_t ulFail_Limit = 3;
	bool ret = false;
	if (EQC_FLAG != EQC_INIT) {
		BT_LOGE("EQC ble client is not ready\n\r");
		Func_Code = false;
		goto TEST_END;
	}

	ulFail_Count = 0;
	do {
		BT_LOGA("\nStart connect to LE device....\n\r");
		bMngrResult = EQC_FAIL;
		EQC_FLAG = EQC_INIT;
		bMngrResult = eqc_le_client_connectDevice(btaddr);

		switch (bMngrResult) {
		case EQC_API_FAIL:
		case EQC_DISC_FAIL: {//connect success & disconn(not 0x3e)
			BT_LOGA("bMngrResult = %d\n\r", bMngrResult);
			Func_Code = false;
			goto TEST_END;
			break;
		}
		case EQC_SUCCESS: {   //connect success & no disconn
			ret = true;
			break;
		}
		case EQC_WAIT_FAIL: {
			BT_LOGA("Start to Cancel connect....\n\r");
			if (EQC_SUCCESS != eqc_le_client_connect_cancel()) {
				BT_LOGE("Cancel connect to LE device fail\n\r");
				Func_Code = false;
				goto TEST_END;
			} else {
				BT_LOGA("Cancel connect to LE device success\n\r");
			}
			break;
		}
		case EQC_DISC_3E_FAIL: {
			BT_LOGA("EQC_DISC_3E_FAIL\n\r");
			break;
		}
		default: {
			BT_LOGA("bMngrResult = 0x%x\n\r", bMngrResult);
			break;
		}
		}
		if (ret) {
			break;
		}
		ulFail_Count ++;
	} while (ulFail_Count < ulFail_Limit);

	if (ulFail_Count >= ulFail_Limit) {
		BT_LOGE("EQC client conn Fail!\n\r");
		Func_Code = false;
		goto TEST_END;
	}
	BT_LOGA("EQC client conn Success!\n\r");
	EQC_FLAG |= EQC_CLIEN_CONN;

	BT_LOGA("\nStart to check channel map....\n\r");
	ulFail_Count = 0;
	do {
		bMngrResult = EQC_FAIL;
		bMngrResult = eqc_le_read_current_channelmap(eqc_connect_handle);
		if (bMngrResult) {
			break;
		}
		ulFail_Count ++;
		BT_LOGA("Retry Get channel map ... %d\n\r", ulFail_Count);
	} while ((ulFail_Count < ulFail_Limit));

	if (ulFail_Count >= ulFail_Limit) {
		BT_LOGE("EQC client check channal map Fail!\n\n\r");
		Func_Code = false;
		goto TEST_END;
	}

	EQC_FLAG |= EQC_CLIEN_CHANMAP;

	BT_LOGA("\nStart to disconnect LE device....\n\r");
	bMngrResult = EQC_FAIL;
	bMngrResult = eqc_le_disconnect_device(eqc_connect_handle);
	if (EQC_SUCCESS != bMngrResult) {
		BT_LOGE("LE Disconnect fail\n\r");
		Func_Code = false;
	}
	BT_LOGA("EQC client disconnect to LE device success\n\r");
	Func_Code = true;

TEST_END:
	EQC_FLAG = EQC_INIT;
	return Func_Code;
}

bool bt_eqc_client_deinit(uint8_t deinit_flag)
{
	///deinit
	E_EVENT_MSG deinit_event_msg = {0xFF, NULL};
	eqc_le_send_msg(&deinit_event_msg);
	eqc_le_wait_bt_status(deinit_flag);

	memset(dut_address, 0, sizeof(dut_address));
	memset(eut_address, 0, sizeof(dut_address));

	if (EQC_DEINIT != EQC_FLAG) {
		BT_LOGE("device deinit fail[%d]\n\r", EQC_FLAG);
		return false;
	}

#if defined(CONFIG_BT_COEXIST)
	rtk_coex_btc_set_pta(PTA_WIFI, PTA_HOST_BT, COMMON_ACTION);
#else
	BT_LOGE("BT_COEXIST disabled! ignore set_pta!");
#endif
	return true;
}

bool bt_eqc_client_start_and_test(uint32_t bt_option)
{
	uint8_t init_flag = EQC_INIT_FAIL | EQC_INIT;
	uint8_t deinit_flag = EQC_DEINIT;
	bool ret = true;
	if (EQC_DEINIT != EQC_FLAG) {
		BT_LOGE("device is not ready to init[%d]\n\r", EQC_FLAG);
		return false;
	}

	memcpy(dut_address, &DUT_address[bt_option], 6);
	memcpy(eut_address, &EUT_address[bt_option], 6);

	eqc_le_task_init();
	E_EVENT_MSG init_event_msg = {0xFE, NULL};
	eqc_le_send_msg(&init_event_msg);
	eqc_le_wait_bt_status(init_flag);

	if (EQC_INIT != EQC_FLAG) {
		BT_LOGE("device init fail[%d]\n\r", EQC_FLAG);
		return false;
	}
#if defined(CONFIG_BT_COEXIST)
	rtk_coex_btc_set_pta(PTA_BT, PTA_HOST_BT, COMMON_ACTION);
#else
	BT_LOGE("BT_COEXIST disabled! ignore set_pta!");
#endif

	if (!eqc_le_client_test(eut_address)) {
		BT_LOGE("eqc_le_client_test fail\n\r");
		bt_eqc_client_deinit(deinit_flag);
		return false;
	}

	ret = bt_eqc_client_deinit(deinit_flag);

	return ret;
}

bool bt_eqc_server_start(void)
{
	uint8_t init_flag = EQC_INIT_FAIL | EQC_INIT;
	if (EQC_DEINIT != EQC_FLAG) {
		BT_LOGE("device is not ready to init[%d]\n\r", EQC_FLAG);
		return false;
	}

	eqc_le_task_init();
	E_EVENT_MSG event_msg = {0xFE, NULL};

	eqc_le_send_msg(&event_msg);

	eqc_le_wait_bt_status(init_flag);

	if (EQC_INIT != EQC_FLAG) {
		BT_LOGE("device init fail[%d]\n\r", EQC_FLAG);
		return false;
	}

#if defined(CONFIG_BT_COEXIST)
	rtk_coex_btc_set_pta(PTA_BT, PTA_HOST_BT, COMMON_ACTION);
#else
	BT_LOGE("BT_COEXIST disabled! ignore set_pta!");
#endif

	if (!eqc_le_start_advertising()) {
		BT_LOGE("device start advertising fail\n\r");
		return false;
	}

	return true;
}

bool bt_eqc_server_stop(void)
{
	uint8_t deinit_flag = EQC_DEINIT;
	if (!(EQC_FLAG & EQC_INIT)) {
		BT_LOGE("device is not ready to deinit[%d]\n\r", EQC_FLAG);
		return false;
	}

	E_EVENT_MSG event_msg = {0xFF, NULL};
	eqc_le_send_msg(&event_msg);
	eqc_le_wait_bt_status(deinit_flag);
	if (EQC_DEINIT != EQC_FLAG) {
		BT_LOGE("device deinit fail[%d]\n\r", EQC_FLAG);
		return false;
	}

#if defined(CONFIG_BT_COEXIST)
	rtk_coex_btc_set_pta(PTA_WIFI, PTA_HOST_BT, COMMON_ACTION);
#else
	BT_LOGE("BT_COEXIST disabled! ignore set_pta!");
#endif

	return true;
}