
#include "app_syscall.h"

static const char *TAG = "APP_START";

void set_got_base(uint32_t app_got_base)
{
	__asm volatile
	(
		" mov r9, r0                                         \n"
	);
}

// just to verify if the relocation is actually working
SECTION(".AppStart")
void AppStart(uint32_t *app_got_base)
{
	__asm volatile
	(
		" mov r9, r0                                         \n"
	);
	APP_LOGI(TAG, "AppStart, %d\n", strlen(TAG));
}
