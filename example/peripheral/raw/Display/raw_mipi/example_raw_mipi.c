#include "ameba_soc.h"
#include "os_wrapper.h"
#include <stdio.h>
void MIPIDemoShow(void);

void mipi_demo(void)
{
	MIPIDemoShow();

	rtos_task_delete(NULL);
}

void example_raw_mipi(void)
{
	printf("MIPI demo main \n");
	MCM_MemTypeDef memInfo = ChipInfo_MCMInfo();

	if (DDR_AUTOGATING && (memInfo.mem_type & MCM_TYPE_DDR)) {
		printf("DDR_AUTOGATING shall be disabled to avoid dram in self-refresh.\n");
	}

	// create demo Task
	if (rtos_task_create(NULL, "MIPI DEMO", (rtos_task_t)mipi_demo, NULL, (2048), (1)) != RTK_SUCCESS) {
		printf("Cannot create demo task\n");
	}
}
