
#include "os_wrapper.h"
#include "rtw_wifi_constants.h"
#include "wifi_conf.h"
#include "lwip_netconf.h"
#include "ameba_ota.h"

#define PORT	8082
static const char *host = "192.168.31.193";  //"m-apps.oss-cn-shenzhen.aliyuncs.com"
static const char *resource = "ota_all.bin";     //"051103061600.bin"

extern void sys_reset(void);

void update_ota_task(void *param)
{
	(void)param;

#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
	rtos_create_secure_context(configMINIMAL_SECURE_STACK_SIZE);
#endif
	rtos_time_delay_ms(1000);

	ota_printf(_OTA_INFO_, "\n\r\n\r\n\r\n\r<<<<<< OTA Example >>>>>>>\n\r\n\r\n\r\n\r");

	while (!((wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS) && (*(u32 *)LwIP_GetIP(0) != IP_ADDR_INVALID))) {
		ota_printf(_OTA_INFO_, "Wait for WIFI connection ...\n");
		rtos_time_delay_ms(1000);
	}

	int ret = -1;
	ota_context *ctx = NULL;

	ctx = (ota_context *)rtos_mem_malloc(sizeof(ota_context));
	if (ctx == NULL) {
		ota_printf(_OTA_ERR_, "[%s] ctx malloc failed", __FUNCTION__);
		goto exit;
	}

	memset(ctx, 0, sizeof(ota_context));

	ret = ota_update_init(ctx, (char *)host, PORT, (char *)resource, OTA_HTTP);
	if (ret != 0) {
		ota_printf(_OTA_ERR_, "[%s] ota_update_init failed", __FUNCTION__);
		goto exit;
	}

	ret = ota_update_start(ctx);

	ota_printf(_OTA_INFO_, "[%s] Update task exit", __FUNCTION__);
	if (!ret) {
		ota_printf(_OTA_INFO_, "[%s] Ready to reboot", __FUNCTION__);
		rtos_time_delay_ms(20);
		sys_reset();
	}

exit:
	ota_update_deinit(ctx);
	if (ctx) {
		rtos_mem_free(ctx);
	}
	rtos_task_delete(NULL);
}

void example_ota(void)
{
	rtos_task_t task;
	if (rtos_task_create(&task, ((const char *)"update_ota_task"), update_ota_task, NULL, 1024 * 4, 1) != SUCCESS) {
		ota_printf(_OTA_ERR_, "\n\r[%s] Create update task failed", __FUNCTION__);
	}
}
