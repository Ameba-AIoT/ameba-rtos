#include "platform_stdlib.h"
#include "basic_types.h"
#include "sntp/sntp.h"
#include "rtw_wifi_defs.h"
#include "wifi_conf.h"
#include "lwip_netconf.h"
#include "os_wrapper.h"

#define TIME_MODE    1	//0: for UTC with microseconds, 1: for timezone with seconds

#if (defined(CONFIG_SYSTEM_TIME64) && CONFIG_SYSTEM_TIME64)
static void show_time(void)
{
#if (TIME_MODE == 0)
	unsigned int update_tick = 0;
	long long update_sec = 0, update_usec = 0;

	sntp_get_lasttime(&update_sec, &update_usec, &update_tick);

	if (update_tick) {
		long long tick_diff_sec, tick_diff_ms, current_sec, current_usec;
		unsigned int current_ms = rtos_time_get_current_system_time_ms();

		tick_diff_sec = (current_ms - update_tick) / 1000;
		tick_diff_ms = (current_ms - update_tick) % 1000;
		update_sec += tick_diff_sec;
		update_usec += (tick_diff_ms * 1000);
		current_sec = update_sec + update_usec / 1000000;
		current_usec = update_usec % 1000000;
		RTK_LOGS(NOTAG, "%s + %d usec\n", ctime(&current_sec), current_usec);
	}
#elif (TIME_MODE == 1)
	int timezone = 8;	// use UTC+8 timezone for example
	struct tm tm_now = sntp_gen_system_time(timezone);
	RTK_LOGS(NOTAG, "%d-%d-%d %d:%d:%d UTC%s%d\n",
			 tm_now.tm_year, tm_now.tm_mon, tm_now.tm_mday, tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec,
			 (timezone > 0) ? "+" : "", timezone);
#endif
}
#else
static void show_time(void)
{
#if (TIME_MODE == 0)
	unsigned int update_tick = 0;
	time_t update_sec = 0, update_usec = 0;

	sntp_get_lasttime(&update_sec, &update_usec, &update_tick);

	if (update_tick) {
		time_t tick_diff_sec, tick_diff_ms, current_sec, current_usec;
		unsigned int current_ms = rtos_time_get_current_system_time_ms();

		tick_diff_sec = (current_ms - update_tick) / 1000;
		tick_diff_ms = (current_ms - update_tick) % 1000;
		update_sec += tick_diff_sec;
		update_usec += (tick_diff_ms * 1000);
		current_sec = update_sec + update_usec / 1000000;
		current_usec = update_usec % 1000000;
		RTK_LOGS(NOTAG, "%s + %d usec\n", ctime(&current_sec), current_usec);
	}
#elif (TIME_MODE == 1)
	int timezone = 8 * 3600;	// use UTC+8(offset in hrs) timezone for example, 8 * 60 * 60(offset in seconds)
	struct tm tm_now = sntp_gen_system_time(timezone);
	RTK_LOGS(NOTAG, "%d-%d-%d %d:%d:%d UTC%s%d\n",
			 tm_now.tm_year, tm_now.tm_mon, tm_now.tm_mday, tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec,
			 (timezone / 3600 > 0) ? "+" : "", timezone / 3600);
#endif
}
#endif

static void example_sntp_showtime_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;

	int should_stop = 0;
	// Delay to wait for IP by DHCP
	while (!((wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS) && (*(u32 *)LwIP_GetIP(0) != IP_ADDR_INVALID))) {
		RTK_LOGS(NOTAG, "Wait for WIFI connection ...\n");
		rtos_time_delay_ms(2000);
	}
	RTK_LOGS(NOTAG, "\nExample: SNTP show time\n");

	sntp_init();

	while (1) {
		show_time();
		rtos_time_delay_ms(1000);
		if (should_stop) {
			break;
		}
	}

	rtos_task_delete(NULL);
}

void example_sntp_showtime(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_sntp_showtime_thread"), example_sntp_showtime_thread, NULL, 1024 * 4, 1) != SUCCESS) {
		RTK_LOGS(NOTAG, "\n\r%s rtos_task_create failed\n", __FUNCTION__);
	}
}
