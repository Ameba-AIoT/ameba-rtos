/*
 *  PWM self-test implementation
 *
 *  Counts rising edges on a GPIO capture pin over a sampling window to
 *  verify that a PWM output pad is actually toggling at the expected
 *  frequency.  Intended for single-board jumper-loopback setups.
 *
 *  Copyright (c) 2025 Realtek Semiconductor Corp.
 */
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "pwm_selftest.h"

static volatile uint32_t  s_edge_cnt;
static pwm_selftest_cfg_t s_cfg;      /* value copy, survives app_example() return */

/* ---------------------------------------------------------------------------
 *  GPIO interrupt handler — called on every rising edge
 * --------------------------------------------------------------------------- */
static void pwm_selftest_cap_irq(uint32_t id, uint32_t event)
{
	(void)id;
	(void)event;
	s_edge_cnt++;
}

/* ---------------------------------------------------------------------------
 *  Self-test task
 * --------------------------------------------------------------------------- */
static void pwm_selftest_task(void *param)
{
	GPIO_InitTypeDef cap_int;
	uint32_t cnt;

	(void)param;

	/* Let the PWM demo bring the output up */
	rtos_time_delay_ms(s_cfg.setup_ms);

	/* Configure capture pin as rising-edge GPIO interrupt */
	GPIO_INTConfig(s_cfg.cap_pin, DISABLE);

	cap_int.GPIO_Pin         = s_cfg.cap_pin;
	cap_int.GPIO_Mode        = GPIO_Mode_INT;
	cap_int.GPIO_PuPd        = GPIO_PuPd_DOWN;
	cap_int.GPIO_ITTrigger   = GPIO_INT_Trigger_EDGE;
	cap_int.GPIO_ITPolarity  = GPIO_INT_POLARITY_ACTIVE_HIGH;

	InterruptRegister((IRQ_FUN)GPIO_INTHandler, GPIOA_IRQ, (uint32_t)GPIOA_BASE, 6);
	InterruptEn(GPIOA_IRQ, 6);
	GPIO_Init(&cap_int);
	GPIO_UserRegIrq(s_cfg.cap_pin, (void *)pwm_selftest_cap_irq, &cap_int);
	GPIO_INTConfig(s_cfg.cap_pin, ENABLE);

	/* Sampling window */
	s_edge_cnt = 0;
	rtos_time_delay_ms(s_cfg.window_ms);
	cnt = s_edge_cnt;

	GPIO_INTConfig(s_cfg.cap_pin, DISABLE);

	/* Report */
	if (cnt >= s_cfg.min_edges && cnt <= s_cfg.max_edges) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "%s_PASS edges=%u\n",
				 s_cfg.tag, cnt);
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "%s_FAIL edges=%u (expect [%u, %u])\n",
				 s_cfg.tag, cnt, s_cfg.min_edges, s_cfg.max_edges);
	}

	rtos_task_delete(NULL);
}

/* ---------------------------------------------------------------------------
 *  Public API
 * --------------------------------------------------------------------------- */
void pwm_selftest_start(const pwm_selftest_cfg_t *cfg)
{
	s_cfg = *cfg;   /* value copy — cfg may be stack-allocated by caller */

	if (rtos_task_create(NULL, "PWM_SELFTEST",
						 (rtos_task_t)pwm_selftest_task,
						 NULL, (2048), (1)) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "%s_FAIL reason=task\n", cfg->tag);
	}
}
