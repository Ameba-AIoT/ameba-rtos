/*
 *******************************************************************************
 * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef __RTK_CTE_CLIENT_H__
#define __RTK_CTE_CLIENT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <bt_api_config.h>

#if defined(RTK_BLE_5_1_CTE_SUPPORT) && RTK_BLE_5_1_CTE_SUPPORT

#define CTE_CLIENT_SHOW_DETAIL      1

typedef enum {
	CTE_CHAR_CTE_ENABLE = 0,
	CTE_CHAR_CTE_ADV_MIN_LEN = 1,
	CTE_CHAR_CTE_ADV_MIN_TX_CNT = 2,
	CTE_CHAR_CTE_ADV_TX_DURATION = 3,
	CTE_CHAR_CTE_ADV_INTERVAL = 4,
	CTE_CHAR_CTE_ADV_PHY = 5,
	CTE_CHAR_NUM,
} cte_charac_index_e;

uint16_t cte_client_add(void);

rtk_bt_evt_cb_ret_t cte_client_app_callback(uint8_t event, void *data);

uint16_t cte_client_attach_conn(uint16_t conn_handle);

uint16_t cte_client_detach_conn(uint16_t conn_handle);

uint16_t cte_client_delete(void);

#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
uint16_t cte_client_srv_discover(uint16_t conn_handle);
#endif

uint16_t cte_client_write_charac(uint16_t conn_handle,
								 cte_charac_index_e char_idx,
								 uint16_t len,
								 uint8_t *data);


#endif /* RTK_BLE_5_1_CTE_SUPPORT */

#ifdef __cplusplus
}
#endif


#endif  /* __RTK_CTE_CLIENT_H__ */