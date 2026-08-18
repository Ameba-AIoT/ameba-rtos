/*
 *  PWM self-test firmware entry point
 *
 *  Build with -DUSE_MBED_PWM=1 to test the mbed_pwm example instead of
 *  the raw_pwm example.  The capture pin is controlled by the
 *  -DPWM_SELFTEST_CAP_PIN=<pin> define.
 *
 *  Self-test runs as a background task alongside the demo breathing loop.
 *
 *  Jumper:
 *     RTL8720F:  _PA_23 (PWM ch-2)  -->  _PA_24 (capture)
 *
 *  Copyright (c) 2025 Realtek Semiconductor Corp.
 */
#include "ameba_soc.h"      /* _PA_XX pin definitions */
#include "pwm_selftest.h"

#ifndef PWM_SELFTEST_CAP_PIN
#define PWM_SELFTEST_CAP_PIN  _PA_24
#endif

#ifdef USE_MBED_PWM
int example_mbed_pwm(void);
#define PWM_EXAMPLE_FUNC  example_mbed_pwm
#define PWM_SELFTEST_TAG  "MBED_PWM"
#else
int example_raw_pwm(void);
#define PWM_EXAMPLE_FUNC  example_raw_pwm
#define PWM_SELFTEST_TAG  "RAW_PWM"
#endif

void app_example(void)
{
	PWM_EXAMPLE_FUNC();

	pwm_selftest_start(&(pwm_selftest_cfg_t) {
		.cap_pin    = PWM_SELFTEST_CAP_PIN,
		.setup_ms   = 500,
		.window_ms  = 2000,
		.min_edges  = 80,
		.max_edges  = 120,
		.tag        = PWM_SELFTEST_TAG,
	});
}
