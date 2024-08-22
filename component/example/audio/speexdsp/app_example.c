/******************************************************************************
*
* Copyright(c) 2007 - 2018 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include "ameba_soc.h"
#include "example_resample.h"

void app_example(void)
{
	rtos_task_create(NULL, ((const char *)"autorun_task"), testresample, NULL, 1024*4, 1);
}
