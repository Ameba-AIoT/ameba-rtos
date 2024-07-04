/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "rtc_api.h"
#include "wait_api.h"
#include "os_wrapper.h"
#include <stdio.h>

/* Set calendar value, e.g. 2020.12.24 19:30:50 */
#define YEAR	(2020)	/* 1900 ~ */
#define MON		(12)	/* 1-12 */
#define MDAY	(24)	/* 1-31 */
#define HOUR	(19)	/* 0-23 */
#define MINUTE		(30)	/* 0-59 */
#define SEC		(50)	/* 0-59 */

void mbed_rtc_demo(void)
{
	struct tm info, *timeinfo;
	time_t seconds;

	rtc_init();

	info.tm_year = YEAR - 1900;	/* actual_year - 1900 */
	info.tm_mon = MON - 1;		/* 0-11, where 0 represents January */
	info.tm_mday = MDAY;
	info.tm_hour = HOUR;
	info.tm_min = MINUTE;
	info.tm_sec = SEC;
	info.tm_isdst = -1;

	/* Get timestamp (seconds from 1970.1.1 00:00:00) */
	seconds = mktime(&info);

	if (seconds != -1) {
		printf(" Set Calendar Time %llu \n\n", (u64)seconds);
		rtc_write(seconds);
	} else {
		printf("Error: unable to make time using mktime\n");
	}

	while (1) {
		seconds = rtc_read();
		timeinfo = localtime(&seconds);

		printf("Time as seconds since January 1, 1970 = %llu\n", (u64)seconds);

		printf("Time as a basic string = %s", ctime(&seconds));

		printf("Time as a custom formatted string = %d-%d-%d %d:%d:%d\n\n",
			   timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour,
			   timeinfo->tm_min, timeinfo->tm_sec);

		wait(1.0);
	}
}

int main(void)
{
	if (SUCCESS != rtos_task_create(NULL, "MBED_RTC_DEMO_TASK", (rtos_task_t)mbed_rtc_demo, (void *)NULL, 3072, (1))) {
		printf("Create MBED_RTC_DEMO_TASK Err!!\n");
	}

	rtos_sched_start();
	return 0;
}

