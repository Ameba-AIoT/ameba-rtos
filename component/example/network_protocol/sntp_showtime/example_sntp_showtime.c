#include "lwip_netconf.h"
#include "log.h"
#include "lwip/apps/sntp.h"

#define TIME_MODE    1	//0: for UTC with microseconds, 1: for timezone with seconds
#define SNTP_SERVER               "pool.ntp.org"

static void show_time(void)
{
#if (TIME_MODE == 0)
	uint32_t sec;
	uint32_t us;
	SNTP_GET_SYSTEM_TIME(sec, us);
	time_t now = (time_t)sec;
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "%s + %d usec\n", ctime(&now), us);
#elif (TIME_MODE == 1)
	time_t now;
	struct tm timeinfo;
	int timezone = 8;	// use UTC+8(offset in hrs) timezone for example, 8 * 60 * 60(offset in seconds)
	time(&now);
	setenv("TZ", "CST-8", 1);
	tzset();
	timeinfo = *(localtime(&now));
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "%d-%d-%d %d:%d:%d UTC%s%d\n",
			 timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec,
			 (timezone > 0) ? "+" : "", timezone);
#endif
}

static void example_sntp_showtime_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;

	int should_stop = 0;

	// Delay to check successful WiFi connection and obtain of an IP address
	LwIP_Check_Connectivity();

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n====================Example: SNTP show time====================\r\n");

	sntp_setservername(0, SNTP_SERVER);

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
	if (rtos_task_create(NULL, ((const char *)"example_sntp_showtime_thread"), example_sntp_showtime_thread, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n\r%s rtos_task_create(example_sntp_showtime_thread) failed\n", __FUNCTION__);
	}
}
