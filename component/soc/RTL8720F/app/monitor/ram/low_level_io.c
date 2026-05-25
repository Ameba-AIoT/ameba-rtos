/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "ameba_soc.h"

#ifdef CONFIG_TEST_NEWLIB_API
extern int test_newlib_write_hook(char *ptr, int len);
#endif

int _write(int file, char *ptr, int len)
{
	int nChars = 0;
	/* Check for stdout and stderr
	(only necessary if file descriptors are enabled.) */
	if (file != 1 && file != 2) {
		return -1;
	}

#ifdef CONFIG_TEST_NEWLIB_API
	int handled_len = test_newlib_write_hook(ptr, len);
	if (handled_len >= 0) {
		return handled_len;
	}
#endif

	for (/*Empty */; len > 0; --len) {
		if (PutChar) {
			PutChar(*ptr++);
		} else {
			ptr++;
		}
		++nChars;
	}
	return nChars;
}

int _read(int file, char *ptr, int len)
{
	int nChars = 0;
	/* Check for stdin
	(only necessary if FILE descriptors are enabled) */
	if (file != 0) {
		return -1;
	}
	for (/*Empty*/; len > 0; --len) {
		int c = DiagGetChar(TRUE);
		if ((c < 0) || (c == '\r')) {
			*ptr = '\0';
			break;
		}
		*(ptr++) = c;
		++nChars;
	}
	return nChars;
}
