#include "ameba_soc.h"
#include "os_wrapper.h"
#include "kv.h"
#include "example_kv.h"

void example_kv_thread(void *param)
{
	(void) param;
	rtos_time_delay_ms(2000);
	char key[] = "T_FILE";
	const char val[] = "KV module works normally !!!";
	char *buffer;
	int res = 0;

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n====================Example: KV====================\r\n");

	res = rt_kv_set(key, val, strlen(val));
	if (res != (int)strlen(val)) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "rt_kv_set() failed\r\n");
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "rt_kv_set() success, write %d characters:%s\r\n", strlen(val), val);
	}

	buffer = (char *)rtos_mem_zmalloc(strlen(val) * sizeof(char));

	res = rt_kv_get(key, buffer, strlen(val));

	if (res != (int)strlen(val)) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "rt_kv_get() failed\r\n");
	} else {
		if (memcmp(buffer, val, strlen(val)) == 0) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "rt_kv_get() success, read %d characters:%s\r\n", strlen(val), buffer);
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "rt_kv_get() fail, content has been changed\r\n");
		}
	}

	res = rt_kv_delete(key);
	if (res) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "rt_kv_delete() failed\r\n");
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "rt_kv_delete() success\r\n");
	}

	rtos_mem_free(buffer);
	rtos_task_delete(NULL);
}

void example_kv(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_kv_thread"), example_kv_thread, NULL, 2048 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n\r%s rtos_task_create(example_kv_thread) failed", __FUNCTION__);
	}
}
