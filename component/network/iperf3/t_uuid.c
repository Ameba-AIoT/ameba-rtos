/*
 * iperf, Copyright (c) 2014, The Regents of the University of
 * California, through Lawrence Berkeley National Laboratory (subject
 * to receipt of any required approvals from the U.S. Dept. of
 * Energy).  All rights reserved.
 *
 * If you have questions about your rights to use or distribute this
 * software, please contact Berkeley Lab's Technology Transfer
 * Department at TTD@lbl.gov.
 *
 * NOTICE.  This software is owned by the U.S. Department of Energy.
 * As such, the U.S. Government has been granted for itself and others
 * acting on its behalf a paid-up, nonexclusive, irrevocable,
 * worldwide license in the Software to reproduce, prepare derivative
 * works, and perform publicly and display publicly.  Beginning five
 * (5) years after the date permission to assert copyright is obtained
 * from the U.S. Department of Energy, and subject to any subsequent
 * five (5) year renewals, the U.S. Government is granted for itself
 * and others acting on its behalf a paid-up, nonexclusive,
 * irrevocable, worldwide license in the Software to reproduce,
 * prepare derivative works, distribute copies to the public, perform
 * publicly and display publicly, and to permit others to do so.
 *
 * This code is distributed under a BSD style license, see the LICENSE
 * file for complete information.
 */
#include <sys/time.h>

#include "platform_stdlib.h"

#include "iperf_util.h"

void uuid_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;
	rtos_time_delay_ms(5000);

	printf("\nuuid test\n");

	char     cookie[37];
	make_cookie(cookie);
	printf("cookie: '%s'\n", cookie);
	if (strlen(cookie) != 36) {
		printf("Not 36 characters long!\n");
	}

	rtos_task_delete(NULL);
}

void example_uuid(void)
{
	if (rtos_task_create(NULL, ((const char *)"uuid_thread"), uuid_thread, NULL, 524 * 4, 1) != RTK_SUCCESS) {
		printf("\n\r%s rtos_task_create(uuid_thread) failed", __FUNCTION__);
	}
	return;
}