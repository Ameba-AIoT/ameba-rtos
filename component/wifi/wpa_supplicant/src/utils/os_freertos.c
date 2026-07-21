/*
 * OS specific functions for UNIX/POSIX systems
 * Copyright (c) 2005-2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */
#include "utils/os.h"

//#ifdef CONFIG_WPS

#ifndef OS_NO_C_LIB_DEFINES
char *os_strdup(const char *string_copy_from)
{
	char *string_copy_to = NULL;
	string_copy_to = os_zalloc(strlen(string_copy_from) + 1);
	os_memcpy((void *)string_copy_to, string_copy_from, strlen(string_copy_from));
	string_copy_to[strlen(string_copy_from)] = '\0';
	return string_copy_to;
}
#endif

size_t os_strlcpy(char *dest, const char *src, size_t siz)
{
	const char *s = src;
	size_t left = siz;

	if (left) {
		/* Copy string up to the maximum size of the dest buffer */
		while (--left != 0) {
			if ((*dest++ = *s++) == '\0') {
				break;
			}
		}
	}

	if (left == 0) {
		/* Not enough room for the string; force NUL-termination */
		if (siz != 0) {
			*dest = '\0';
		}
		while (*s++)
			; /* determine total src string length */
	}

	return s - src - 1;
}

void *os_memdup(const void *src, size_t len)
{
	void *r = os_malloc(len);

	if (r) {
		os_memcpy(r, src, len);
	}
	return r;
}

int os_get_random(unsigned char *buf, size_t len)
{
	//TODO implement it
	TRNG_get_random_bytes(buf, len);
	return 0;
}

int os_get_time(struct os_time *t)
{
	unsigned int tt = rtos_time_get_current_system_time_ms();
	t->sec = (os_time_t)(tt / 1000);
	t->usec = (os_time_t)(tt % 1000) * 1000;
	return 0;
}

int os_get_reltime(struct os_reltime *t)
{
	os_get_time((struct os_time *)t);
	return 0;
}

void *os_xqueue_create(unsigned long uxQueueLength, unsigned long uxItemSize)
{
	void *xQueue = NULL;
	rtos_queue_create(&xQueue, uxQueueLength, uxItemSize);
	return xQueue;
}

int os_xqueue_receive(xqueue_handle_t xQueue, void *const pvBuffer, unsigned long xSecsToWait)
{
	return rtos_queue_receive(xQueue, pvBuffer, xSecsToWait * 1000);
}

void os_xqueue_delete(xqueue_handle_t xQueue)
{
	rtos_queue_delete(xQueue);
}

int os_xqueue_send(xqueue_handle_t xQueue, const void *const pvItemToQueue, unsigned long xSecsToWait)
{
	return rtos_queue_send(xQueue, (void *)pvItemToQueue, xSecsToWait * 1000);
}
//#endif
