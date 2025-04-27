#include <stdio.h>
#include "ameba_soc.h"
#include "os_wrapper.h"

extern void aivoice_algo_offline_example(void);

static void aivoice_thread(void *param)
{
	(void)param;
	rtos_time_delay_ms(2000);

	printf("create example_aivoice_thread\n");
	aivoice_algo_offline_example();
	printf("exit example_aivoice_thread\n");

	rtos_task_delete(NULL);
}

void example_aivoice(void)
{
	if (rtos_task_create(NULL, "example_aivoice_thread", aivoice_thread, NULL, 1024 * 16, 1) != RTK_SUCCESS) {
		printf("\n\r%s rtos_task_create example_aivoice_thread failed", __FUNCTION__);
	}
}

void app_example(void)
{
	example_aivoice();
}
