/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Stub version of chown.
 */

#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#include <sys/types.h>
#undef errno
extern int errno;
#include "warning.h"

int
_chown(const char *path,
	   uid_t owner,
	   gid_t group)
{
	errno = ENOSYS;
	return -1;
}

stub_warning(_chown)
