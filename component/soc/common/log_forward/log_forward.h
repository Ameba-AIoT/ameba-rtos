/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _LOG_FORWARD_H_
#define _LOG_FORWARD_H_

#include "basic_types.h"

/*
 * Maximum bytes of one log message delivered to output_fn.
 * Format buffers and the reader buffer are all sized to this value, so a
 * single message is always self-contained (no partial lines on the host).
 * Real log lines are far below this; longer lines are truncated, not dropped.
 */
#define LOG_FWD_LINE_MAX  128

typedef void (*rtk_log_output_fn)(const u8 *buf, u32 len);

void rtk_log_forward_init(rtk_log_output_fn fn);
void rtk_log_forward_enable(void);
void rtk_log_forward_disable(void);

#endif /* _LOG_FORWARD_H_ */
