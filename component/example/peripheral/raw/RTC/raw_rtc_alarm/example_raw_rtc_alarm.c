/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "ameba_soc.h"
#include "main.h"
#include "os_wrapper.h"
#include <stdio.h>

u32 RTC_Handler(u32 *Data)
{
	UNUSED(Data);
	RTC_TimeTypeDef RTC_TimeStruct;

	/*clear alarm flag*/
	RTC_AlarmClear();

	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
	printf("RTC_ISR time: %d:%d:%d:%d (%d) \n", RTC_TimeStruct.RTC_Days,
		   RTC_TimeStruct.RTC_Hours,
		   RTC_TimeStruct.RTC_Minutes,
		   RTC_TimeStruct.RTC_Seconds,
		   RTC_TimeStruct.RTC_H12_PMAM);
	return 0;
}

void raw_rtc_alarm_demo(void)
{
	RTC_InitTypeDef RTC_InitStruct_temp;
	RTC_AlarmTypeDef RTC_AlarmStruct_temp;
	RTC_TimeTypeDef RTC_TimeStruct;

	/*enable RTC*/
	RCC_PeriphClockCmd(APBPeriph_RTC, APBPeriph_RTC_CLOCK, ENABLE);
	RTC_StructInit(&RTC_InitStruct_temp);
	RTC_Init(&RTC_InitStruct_temp);

	RTC_TimeStructInit(&RTC_TimeStruct);
	RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);

	/* set alarm */
	RTC_AlarmStructInit(&RTC_AlarmStruct_temp);
	RTC_AlarmStruct_temp.RTC_AlarmTime.RTC_Days = 1;
	RTC_AlarmStruct_temp.RTC_AlarmTime.RTC_Hours = 1;
	RTC_AlarmStruct_temp.RTC_AlarmTime.RTC_Minutes = 1;
	RTC_AlarmStruct_temp.RTC_AlarmTime.RTC_Seconds = 0;
	RTC_AlarmStruct_temp.RTC_AlarmMask = RTC_AlarmMask_Hours | RTC_AlarmMask_Minutes;
	RTC_AlarmStruct_temp.RTC_Alarm2Mask = RTC_Alarm2Mask_Days;

	RTC_SetAlarm(RTC_Format_BIN, &RTC_AlarmStruct_temp);
	RTC_AlarmCmd(ENABLE);

	InterruptRegister((IRQ_FUN)RTC_Handler, RTC_IRQ, NULL, 5);
	InterruptEn(RTC_IRQ, 5);

	rtos_task_delete(NULL);
}

int example_raw_rtc_alarm(void)
{
	if (rtos_task_create(NULL, "RAW_RTC_ALARM_TASK", (rtos_task_t)raw_rtc_alarm_demo, NULL, 2048, (1)) != SUCCESS) {
		printf("Create RAW_RTC_ALARM_TASK Err!!\n\r");
	}

	// rtos_sched_start();
	return 0;

}

