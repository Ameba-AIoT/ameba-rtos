/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
//#include "ameba_soc.h"
//#include "main.h"
#include "rtc_api.h"
#include "os_wrapper.h"
#include <stdio.h>
#include <time.h>
#include "timer_api.h"

alarm_irq_handler rtc_handler(void)
{
	time_t t;
	struct tm *timeinfo;

	t = rtc_read();
	timeinfo = localtime(&t);

	printf("alarm time = %d-%d-%d %d:%d:%d\n",
		   timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour,
		   timeinfo->tm_min, timeinfo->tm_sec);
	return 0;
}

void mbed_rtc_alarm_demo(void)
{
	time_t t = 0;
	struct tm *timeinfo;
	alarm_t alarm;

	rtc_init();
	rtc_write(t);
	timeinfo = localtime(&t);

	printf("now time = %d-%d-%d %d:%d:%d\n",
		   timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour,
		   timeinfo->tm_min, timeinfo->tm_sec);

	alarm.hour = 0;
	alarm.yday = 0;
	alarm.min = 0;
	alarm.sec = 10;
	rtc_set_alarm(&alarm, (alarm_irq_handler) rtc_handler);

	while (1);
}

int main(void)
{
	if (rtos_task_create(NULL, "MBED_RTC_ALARM_TASK", (rtos_task_t)mbed_rtc_alarm_demo, NULL, 3072, (1)) != SUCCESS) {
		printf("Create MBED_RTC_ALARM_TASK Err!!\n\r");
	}

	rtos_sched_start();
	return 0;
}

