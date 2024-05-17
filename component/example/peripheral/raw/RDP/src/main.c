#include "ameba_soc.h"
#include "main.h"
#include "os_wrapper.h"
#include <stdio.h>

NS_ENTRY u32 rdp_protection_entry(u32 idx);
u32 NS_ENTRY rdp_no_protection_call(void *fp, u32 idx);

u32 no_protection_func(u32 data)
{
	u32 result;

	result = data / 5 + 3;
	result *= 2;
	result += 8;

	return result;
}

void rdp_demo(void)
{
	/*wait other CPU boot Done*/
	rtos_time_delay_ms(1000);

	if (SYSCFG_OTP_RDPEn() == FALSE) {
		printf("RDP bit In OTP is not enabled!\n");
		goto end;
	}

	int i = 0;
	u32 rdp_result;
	u32 no_rdp_result;

	/* Tasks are not created with a secure context. Any task that is going to call secure functions must call
		rtos_create_secure_context() to allocate itself a secure context before it calls any secure function. */
	rtos_create_secure_context(1024);

	for (i = 0; i < 32; i++) {
		rdp_result = rdp_protection_entry(i);
		no_rdp_result = rdp_no_protection_call(no_protection_func, i);

		if (rdp_result != no_rdp_result) {
			printf("rdp call fail!\n");
			printf("rdp_result = 0x%x, no_rdp_result=0x%x\n", (unsigned int)rdp_result, (unsigned int)no_rdp_result);
			goto end;
		}
	}

	printf("rdp call succeed!\n");

end:
	rtos_task_delete(NULL);
}

int main(void)
{
#ifdef CONFIG_TRUSTZONE_EN
	printf("RDP demo main \n");

	// create demo Task
	if (rtos_task_create(NULL, "RDP DEMO", (rtos_task_t)rdp_demo, NULL, 2048, (1)) != SUCCESS) {
		printf("Cannot create demo task\n");
		goto end_demo;
	}

#ifdef PLATFORM_FREERTOS
	rtos_sched_start();
#endif

end_demo:
	while (1);
#else
	printf("please enable trustzone in menuconfig \n");
#endif
}
