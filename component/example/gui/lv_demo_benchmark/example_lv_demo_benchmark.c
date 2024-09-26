<<<<<<< HEAD   (bbd1c2 [common][media] fix the player example warning)
=======


#include "ameba_soc.h"

#include "lvgl.h"
#include "lv_hal.h"
#include "lv_demo_benchmark.h"


void lv_demo_benchmark_task(void* param)
{
	UNUSED(param);
	printf("lv_demo_benchmark start\n");

	lv_hal_init();

	lv_demo_benchmark();

	/*Handle LitlevGL tasks (tickless mode)*/
	while (1) {
		int time = lv_task_handler();
		rtos_time_delay_ms(time);
	}
}

#ifdef CMD_TEST
u32 lv_demo_benchmark_show(IN  u16 argc, IN  u8  *argv[])
{
	UNUSED(argc);
	UNUSED(argv);
#else
u32 lv_demo_benchmark_show(void)
{
#endif
	rtos_task_create(NULL, "lv_demo_benchmark_task", lv_demo_benchmark_task, NULL, 1024 * 4, 1);
	return _TRUE;
}


#ifdef CMD_TEST
CMD_TABLE_DATA_SECTION
const COMMAND_TABLE lv_demo_benchmark_cmd_table[] = {
	{
		(const u8 *)"lv_demo_benchmark_show",  1, lv_demo_benchmark_show, (const u8 *)"\t lv_demo_benchmark_show\n"
	},
};
#else
void app_example(void)
{
	lv_demo_benchmark_show();
}
#endif
>>>>>>> CHANGE (6580cb [ameba][gui] fix the example warning)
