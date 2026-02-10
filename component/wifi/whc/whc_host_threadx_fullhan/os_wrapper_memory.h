/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __OS_WRAPPER_MEMORY_H__
#define __OS_WRAPPER_MEMORY_H__

/**
 * @brief  Define memory alignment settings. This macro will be checked during compilation to ensure
 * that it is consistent with the configuration of the operating system.
 */

void *rt_malloc(unsigned long nbytes);

void rt_free(void *ptr);

#endif

