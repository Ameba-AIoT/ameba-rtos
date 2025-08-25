/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Stub version of _exit.  */

#include <limits.h>
#include "config.h"
#include <_ansi.h>
#include <_syslist.h>

void
_exit(int rc)
{
	/* Default stub just causes a divide by 0 exception.  */
	int x = rc / INT_MAX;
	x = 4 / x;

	/* Convince GCC that this function never returns.  */
	for (;;)
		;
}
