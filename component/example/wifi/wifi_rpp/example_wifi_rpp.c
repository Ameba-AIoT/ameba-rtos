#include "example_wifi_rpp.h"
#include "wifi_api.h"

extern void wtn_zrpp_start(void);
int ble_wifimate_device_main(uint8_t enable, uint16_t timeout);

void wifi_rpp_task(void *param)
{
	UNUSED(param);

	wtn_zrpp_start();
	ble_wifimate_device_main(1, 60);

	rtos_task_delete(NULL);
}

void example_wifi_rpp(void)
{
	if (rtos_task_create(NULL, ((const char *)"wifi_rpp_task"), wifi_rpp_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n\r[%s] Create wifi provisioning task failed", __FUNCTION__);
	}
}
