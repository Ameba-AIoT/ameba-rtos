/**
 * \file timing_alt.h
 *
 * \brief RTK timing implementation on top of the RTOS millisecond tick.
 */
#ifndef MBEDTLS_TIMING_ALT_H
#define MBEDTLS_TIMING_ALT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct mbedtls_timing_hr_time {
	uint32_t start_ms;
};

typedef struct mbedtls_timing_delay_context {
	struct mbedtls_timing_hr_time timer;
	uint32_t int_ms;
	uint32_t fin_ms;
} mbedtls_timing_delay_context;

#ifdef __cplusplus
}
#endif

#endif /* timing_alt.h */
