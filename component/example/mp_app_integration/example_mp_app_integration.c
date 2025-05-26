#include "example_mp_app_integration.h"

void at_markmp(void *arg)
{
	UNUSED(arg);
	BKUP_Write(BKUP_REG1, BKUP_Read(BKUP_REG1) | BIT(0));
	at_printf("\r\n%sOK\r\n", "+MARKMP");
}

log_item_t at_custom_items[ ] = {
	{"+MARKMP", at_markmp, {NULL, NULL}},
};

void at_custom_init(void)
{
	atcmd_service_add_table(at_custom_items, sizeof(at_custom_items) / sizeof(at_custom_items[0]));
}

static void execute_user_app(void)
{
	RTK_LOGI(NOTAG, "\nExecute user app\n", __FUNCTION__);
}

static void mp_app_integration_task(void *param)
{
	(void) param;

	if ((BKUP_Read(BKUP_REG1) & BIT(0)) == 0) {
		execute_user_app();
	}

	rtos_task_delete(NULL);
}

void example_mp_app_integration(void)
{
	rtos_task_t task;
	if (rtos_task_create(&task, ((const char *)"mp_app_integration_task"), mp_app_integration_task, NULL, 1024, 1) != RTK_SUCCESS) {
		RTK_LOGI(NOTAG, "\n%s rtos_task_create failed\n", __FUNCTION__);
	}
}
