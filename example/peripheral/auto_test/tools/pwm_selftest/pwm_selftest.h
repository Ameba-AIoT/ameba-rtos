/*
 *  PWM self-test public interface
 *
 *  Single-board jumper-loopback self-test for any PWM output:
 *   - Jumper a PWM output pin to a GPIO capture pin on the same board.
 *   - pwm_selftest_start() counts rising edges on the capture pin over a
 *     fixed window and prints PASS/FAIL via RTK_LOGS.
 *
 *  Usage (inside an #ifdef block in the demo code, or in a test-only
 *  app_example.c):
 *
 *      pwm_selftest_start(&(pwm_selftest_cfg_t){
 *          .cap_pin    = _PA_24,
 *          .setup_ms   = 500,
 *          .window_ms  = 2000,
 *          .min_edges  = 80,
 *          .max_edges  = 120,
 *          .tag        = "RAW_PWM",
 *      });
 *
 *  Copyright (c) 2025 Realtek Semiconductor Corp.
 */
#ifndef PWM_SELFTEST_H
#define PWM_SELFTEST_H

#include <stdint.h>

typedef struct {
	uint32_t cap_pin;       /* GPIO pin to capture rising edges on */
	uint32_t setup_ms;      /* delay before sampling starts (let PWM stabilise) */
	uint32_t window_ms;     /* sampling window in ms */
	uint32_t min_edges;     /* pass threshold: minimum edges in window */
	uint32_t max_edges;     /* pass threshold: maximum edges in window */
	const char *tag;        /* print tag, e.g. "RAW_PWM" → prints RAW_PWM_PASS */
} pwm_selftest_cfg_t;

/**
 * @brief Start the self-test task (non-blocking).
 *
 * Spawns a background task that waits @p setup_ms, captures rising edges
 * on @p cap_pin for @p window_ms, then prints PASS/FAIL.  The task
 * self-destructs after reporting.
 */
void pwm_selftest_start(const pwm_selftest_cfg_t *cfg);

#if defined (CONFIG_RTL8720F)
#define PWM_SELFTEST_CAP_PIN	_PA_24
#endif

#endif /* PWM_SELFTEST_H */
