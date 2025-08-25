/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ARM_BACKTRACE_H
#define ARM_BACKTRACE_H

void do_stack_backtrace(uint32_t *pc, uint32_t *sp, uint32_t *lr);
void get_call_stack(void **caller, uint32_t max_level);
void set_backtrace_depth(uint32_t backtrace_depth);
#endif
