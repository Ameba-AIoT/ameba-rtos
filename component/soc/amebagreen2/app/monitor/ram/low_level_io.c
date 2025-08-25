/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "ameba_soc.h"

int _write(int file, char *ptr, int len)
{
	int nChars = 0;
	/* Check for stdout and stderr
	(only necessary if file descriptors are enabled.) */
	if (file != 1 && file != 2) {
		return -1;
	}
	for (/*Empty */; len > 0; --len) {
		DiagPutChar(*ptr++);
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
