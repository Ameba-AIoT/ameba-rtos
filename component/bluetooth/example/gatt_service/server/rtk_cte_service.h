/*
 *******************************************************************************
 * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef __RTK_CTE_SERVICE_H__
#define __RTK_CTE_SERVICE_H__

#ifdef __cplusplus
extern "C"  {
#endif

#include <bt_api_config.h>

#if defined(RTK_BLE_5_1_CTE_SUPPORT) && RTK_BLE_5_1_CTE_SUPPORT

#define RTK_BLE_CTE_NUM_ANT_ID_MAX  (0x0C)

typedef struct {
	uint8_t num_ant_ids;
	uint8_t ant_ids[RTK_BLE_CTE_NUM_ANT_ID_MAX];
} cte_antenna_t;

typedef struct {
	uint8_t  enable;
	uint16_t conn_handle;
} cte_conn_enable_t;

typedef enum {
	/* Enable adv cte param @refer uint8_t enable */
	CTE_SRV_PARAM_TYPE_ENABLE_ADV_CTE = 0,
	/* Enable connection cte param @refer cte_conn_enable_t */
	CTE_SRV_PARAM_TYPE_ENABLE_CONN_CTE = 1,
	/* Advertising CTE minimum length param @refer uint8_t adv_cte_min_len */
	CTE_SRV_PARAM_TYPE_ADV_CTE_MIN_LEN = 2,
	/* Advertising CTE minimum transmit count param @refer uint8_t adv_cte_min_tx_cnt */
	CTE_SRV_PARAM_TYPE_ADV_CTE_MIN_TX_CNT = 3,
	/* Advertising CTE transmit duration param @refer uint8_t adv_cte_tx_duration */
	CTE_SRV_PARAM_TYPE_ADV_CTE_TX_DURATION = 4,
	/* Advertising CTE interval param @refer uint16_t adv_cte_interval */
	CTE_SRV_PARAM_TYPE_ADV_CTE_INTERVAL = 5,
	/* Advertising CTE PHY param @refer uint8_t adv_cte_phy */
	CTE_SRV_PARAM_TYPE_ADV_CTE_PHY = 6,
	/* Advertising CTE param @refer uint8_t adv_cte_type */
	CTE_SRV_PARAM_TYPE_ADV_CTE_TYPE = 7,
	/* Connection cte type param @refer uint8_t conn_cte_type */
	CTE_SRV_PARAM_TYPE_CONN_CTE_TYPE = 8,
	/* Antenna param for cte connection state @refer cte_antenna_t */
	CTE_SRV_PARAM_TYPE_CONN_ANTENNA = 9,
	/* Antenna param for cte advertising state @refer cte_antenna_t */
	CTE_SRV_PARAM_TYPE_ADV_ANTENNA = 10,
} CTE_SRV_PARAM_TYPE_e;

uint16_t cte_srv_add(void);

uint16_t cte_srv_set_params(CTE_SRV_PARAM_TYPE_e param_type, uint16_t len, void *value);

void cte_srv_callback(uint8_t event, void *data);

void cte_srv_disconnect(uint16_t conn_handle);

void cte_srv_status_deinit(void);


#endif /* RTK_BLE_5_1_CTE_SUPPORT */

#ifdef __cplusplus
}
#endif

#endif  /* __RTK_CTE_SERVICE_H__ */