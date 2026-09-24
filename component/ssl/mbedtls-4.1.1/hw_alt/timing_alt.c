/**
 * *****************************************************************************
 * @file    timing_alt.c
 * @brief   MBEDTLS_TIMING_ALT implementation over the RTOS millisecond tick.
 * *****************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#include "mbedtls/build_info.h"

#if defined(MBEDTLS_TIMING_ALT)

#include "mbedtls/timing.h"

extern uint32_t rtos_time_get_current_system_time_ms(void);

unsigned long long mbedtls_timing_get_timer(struct mbedtls_timing_hr_time *val, int reset)
{
	uint32_t now = rtos_time_get_current_system_time_ms();

	if (reset) {
		val->start_ms = now;
		return 0;
	}

	return (unsigned long long)(now - val->start_ms);
}

void mbedtls_timing_set_delay(void *data, uint32_t int_ms, uint32_t fin_ms)
{
	mbedtls_timing_delay_context *ctx = (mbedtls_timing_delay_context *) data;

	ctx->int_ms = int_ms;
	ctx->fin_ms = fin_ms;

	if (fin_ms != 0) {
		(void) mbedtls_timing_get_timer(&ctx->timer, 1);
	}
}

int mbedtls_timing_get_delay(void *data)
{
	mbedtls_timing_delay_context *ctx = (mbedtls_timing_delay_context *) data;
	unsigned long long elapsed_ms;

	if (ctx->fin_ms == 0) {
		return -1;
	}

	elapsed_ms = mbedtls_timing_get_timer(&ctx->timer, 0);

	if (elapsed_ms >= ctx->fin_ms) {
		return 2;
	}

	if (elapsed_ms >= ctx->int_ms) {
		return 1;
	}

	return 0;
}

uint32_t mbedtls_timing_get_final_delay(const mbedtls_timing_delay_context *data)
{
	return data->fin_ms;
}

#endif /* MBEDTLS_TIMING_ALT */
