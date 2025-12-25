#include "ameba_soc.h"
#include "os_wrapper.h"
#include <stdio.h>

#define DEBOUNCE_TIME				CHIPEN_DBC_16MS

#define DTIMER_INITIAL_VALUE		0
#define DTIMER_TICKS_US				1
/* Note: Ensure the counter does not overflow the register bit field */
#define LONG_PRESS_THRESHOLD_5S		(5000000 * DTIMER_TICKS_US)

static u32 press_duration_us = 0;
static u8 reset_event_flag = 0;

static void chipen_pulse_task(void)
{
	while (reset_event_flag) {
		/* Wait chipen button release */
		while (CHIPEN_IsPress());

		press_duration_us = DTimestamp_Get();

		if (press_duration_us < LONG_PRESS_THRESHOLD_5S) {
			printf("short press event. \r\n");
		} else {
			printf("long press event. \r\n");
		}

		/* It's no need to check counter again, until pulse reset happens. */
		reset_event_flag = 0;
	}

	rtos_task_delete(NULL);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int example_raw_reset_chipen_pulse(void)
{
	CHIPEN_DebounceSet(DEBOUNCE_TIME);

	if (CHIPEN_WorkModeGet() != CHIPEN_PULSE_RESET_MODE) {
		printf("Configure CHIPEN work in pulse mode.\r\n");

		CHIPEN_WorkMode(CHIPEN_PULSE_RESET_MODE);

	} else {
		printf("CHIPEN pulse reset event happens.\r\n");

		DTimestamp_Set(DTIMER_INITIAL_VALUE);
		reset_event_flag = 1;

		if (rtos_task_create(NULL, "RESET CHIPEN PULSE DEMO", (rtos_task_t)chipen_pulse_task, NULL, 3072, (1)) != RTK_SUCCESS) {
			printf("Cannot create chipen_pulse_task demo task\n\r");
		}
	}

	return 0;
}
