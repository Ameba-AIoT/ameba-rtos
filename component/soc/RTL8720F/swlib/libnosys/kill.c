/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Stub version of kill.
 */

#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#undef errno
extern int errno;
#include "warning.h"

int
_kill(int pid,
	  int sig)
{
	errno = ENOSYS;
	return -1;
}

stub_warning(_kill)
