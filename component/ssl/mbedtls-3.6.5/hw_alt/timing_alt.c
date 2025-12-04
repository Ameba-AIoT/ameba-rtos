#include "common.h"

#if defined(MBEDTLS_TIMING_C)

#include "mbedtls/timing.h"

#if defined(MBEDTLS_TIMING_ALT)

extern uint32_t rtos_time_get_current_system_time_ms(void);
unsigned long mbedtls_timing_get_timer(struct mbedtls_timing_hr_time *val, int reset)
{
	unsigned long delta;
	uint32_t offset;
	uint32_t *t = (uint32_t *) val;

	offset = rtos_time_get_current_system_time_ms();

	if(reset)
	{
		*t = offset;
		return 0;
	}

	delta = offset - *t;

	return delta;
}

/*
 * Set delays to watch
 */
void mbedtls_timing_set_delay( void *data, uint32_t int_ms, uint32_t fin_ms )
{
	mbedtls_timing_delay_context *ctx = (mbedtls_timing_delay_context *) data;

	ctx->int_ms = int_ms;
	ctx->fin_ms = fin_ms;

    if( fin_ms != 0 )
		(void) mbedtls_timing_get_timer( &ctx->timer, 1 );
}

/*
 * Get number of delays expired
 */
int mbedtls_timing_get_delay( void *data )
{
	mbedtls_timing_delay_context *ctx = (mbedtls_timing_delay_context *) data;
	unsigned long elapsed_ms;

	if( ctx->fin_ms == 0 )
		return -1;

    elapsed_ms = mbedtls_timing_get_timer( &ctx->timer, 0 );

	if( elapsed_ms >= ctx->fin_ms )
		return 2;

	if( elapsed_ms >= ctx->int_ms )
		return 1;

	return 0;
}
#endif
#endif