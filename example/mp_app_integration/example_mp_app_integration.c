#include "example_mp_app_integration.h"

void at_markmp(u16 argc, char **argv)
{
	UNUSED(argc);
	UNUSED(argv);
	BKUP_Write(BKUP_REG1, BKUP_Read(BKUP_REG1) | BIT(0));
	at_printf("\r\n%sOK\r\n", "+MARKMP");
}

ATCMD_APONLY_TABLE_DATA_SECTION
const log_item_t at_custom_items[ ] = {
	{"+MARKMP", at_markmp},
};

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
