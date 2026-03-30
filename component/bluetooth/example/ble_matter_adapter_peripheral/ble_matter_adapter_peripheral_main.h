/*
 *******************************************************************************
 * Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef __BLE_MATTER_ADAPTER_PERIPHERAL_MAIN_H__
#define __BLE_MATTER_ADAPTER_PERIPHERAL_MAIN_H__

#if defined(CONFIG_BLE_MATTER_MULTI_ADV_ON) && (CONFIG_BLE_MATTER_MULTI_ADV_ON == 1)
#define BLE_MATTER_ADV				0
#define BLE_CUSTOMER_ADV			1
typedef struct {
	uint8_t		adv_datalen;
	uint8_t		scanrsp_datalen;
	uint8_t		*adv_data;
	uint8_t		*scanrsp_data;
	uint16_t	adv_int_min;
	uint16_t	adv_int_max;
} MATTER_ADV_PARAM;

typedef struct {
	uint8_t matter_adv_handle;
	uint8_t matter_adv_start;
	uint8_t matter_adv_create;
	uint16_t matter_conn_handle;
	uint8_t matter_adv_datalen;
	uint8_t matter_scanrsp_datalen;
	uint8_t *matter_adv_data;
	uint8_t *matter_scanrsp_data;

	uint8_t customer_adv_handle;
	uint8_t customer_adv_start;
	uint8_t customer_adv_create;
	uint16_t customer_conn_handle;
	uint8_t customer_adv_datalen;
	uint8_t customer_scanrsp_datalen;
	uint8_t *customer_adv_data;
	uint8_t *customer_scanrsp_data;
} MULTI_ADV_INFO;
#endif // CONFIG_BLE_MATTER_MULTI_ADV_ON

#ifdef __cplusplus
extern "C"  {
#endif

int ble_matter_adapter_peripheral_main(uint8_t enable);

int ble_matter_adapter_config_adv(uint16_t adv_int_min, uint16_t adv_int_max, uint8_t *padv_data, uint8_t padv_data_length);

int ble_matter_adapter_start_adv(void);

int ble_matter_adapter_stop_adv(void);

uint16_t ble_matter_adapter_get_mtu(uint16_t conn_handle);

int ble_matter_adapter_set_device_name(char *device_name);

int ble_matter_adapter_send_indication(uint16_t conn_handle, uint8_t *data, uint16_t data_length);

int ble_matter_adapter_disconnect(uint16_t conn_handle);

#if CONFIG_BLE_MATTER_MULTI_ADV_ON
int ble_customer_config_adv(uint16_t adv_int_min, uint16_t adv_int_max, uint8_t *padv_data, uint8_t padv_data_length, uint8_t *pscanrsp_data, uint8_t pcanrsp_datalen);

int ble_customer_start_adv(void);

int ble_customer_stop_adv(void);

void ble_customer_adv_setup(void);
#endif


#ifdef __cplusplus
}
#endif

#endif  /* __BLE_MATTER_ADAPTER_PERIPHERAL_MAIN_H__ */
