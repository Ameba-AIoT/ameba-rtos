#include "lwip_netconf.h"
#include "ameba_soc.h"
#include "ota_api.h"
#include "vfs.h"


static const char *RESOURCE = "/ota_all.bin";
extern void sys_reset(void);

void ota_task(void *param)
{
	(void)param;

	rtos_time_delay_ms(2000);

#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
	rtos_create_secure_context(configMINIMAL_SECURE_STACK_SIZE);
#endif

	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\r\n\r\n\r\n\r<<<<<< OTA VFS Example >>>>>>>\n\r\n\r\n\r\n\r");

	int ret = -1;
	ota_context_t *ctx = NULL;
	char *prefix;
	char path[128] = {0};

	ctx = (ota_context_t *)rtos_mem_malloc(sizeof(ota_context_t));
	if (ctx == NULL) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "[%s] ctx malloc failed\n", __FUNCTION__);
		goto exit;
	}

	memset(ctx, 0, sizeof(ota_context_t));

	prefix = find_vfs_tag(VFS_REGION_3);
	DiagSnPrintf(path, sizeof(path), "%s:%s", prefix, RESOURCE);

	ret = ota_init(ctx, NULL, NULL, (char *)path, OTA_VFS);
	if (ret != OTA_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "[%s] ota_init failed\n", __FUNCTION__);
		goto exit;
	}

	ret = ota_start(ctx);

	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "[%s] OTA task exit\n", __FUNCTION__);
	if (ret == OTA_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "[%s] Ready to reboot\n", __FUNCTION__);
		rtos_time_delay_ms(20);
		sys_reset();
	}

exit:
	ota_deinit(ctx);
	if (ctx) {
		rtos_mem_free(ctx);
	}
	rtos_task_delete(NULL);
}

void example_ota_vfs(void)
{
	if (rtos_task_create(NULL, ((const char *)"ota_task"), ota_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\r[%s] Create ota_task failed", __FUNCTION__);
	}
}
