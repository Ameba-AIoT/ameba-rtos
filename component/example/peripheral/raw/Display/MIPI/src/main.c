#include "ameba_soc.h"
#include "main.h"
#include "ameba_mipi_show.c"
#include "os_wrapper.h"
#include <stdio.h>

void mipi_demo(void)
{
	MIPIDemoShow();

	rtos_task_delete(NULL);
}

int main(void)
{
	printf("MIPI demo main \n");

	if (DDR_AUTOGATING && (ChipInfo_MemoryType() == Memory_Type_DDR)) {
		printf("DDR_AUTOGATING shall be disabled to avoid dram in self-refresh.\n");
	}

	// create demo Task
	if (rtos_task_create(NULL, "MIPI DEMO", (rtos_task_t)mipi_demo, NULL, (2048), (1)) != SUCCESS) {
		printf("Cannot create demo task\n");
	}

#ifdef CONFIG_PLATFORM_FREERTOS
	rtos_sched_start();
#endif

	while (1);
}
