/**
 ******************************************************************************
 * @file    syscall.c
 * @brief   Contains APIs for system calls from the APP
 ******************************************************************************
 * @attention
 *
 * LICENSE
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Rohit Gujarathi rgujju.github.io
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/
#include "ameba.h"
#include "os_wrapper.h"
#include "syscall.h"

#ifdef KERNEL
#ifdef ARM_CORE_CA32
/* include apcore/spinlock.h for padding a cache line fully*/
#include "spinlock.h"
/**
 * @brief The CA32 has two cores that need to be locked
 * when printing to avoid interrupting each other
 */
static spinlock_t print_lock;
#endif

static const char *TAG = "SYSCALL";
// rtos_queue_t api_q;

void print_test(uint8_t led_num, uint8_t led_status)
{
	RTK_LOGI(TAG, "led_num: %d, led_status: %d\n", led_num, led_status);
	RTK_LOGI(TAG, "free heap size: %d bytes\n", rtos_mem_get_free_heap_size());
}

int apps_log_write(const char *fmt, ...)
{
	int ret;
	va_list ap;

#ifdef ARM_CORE_CA32
	u32 isr_status = spin_lock_irqsave(&print_lock);
#endif

	va_start(ap, fmt);
	if (CPU_InInterrupt()) {
		ret = DiagVprintf(fmt, ap);
	} else {
		ret = vprintf(fmt, ap);
		fflush(stdout);
	}
	va_end(ap);

#ifdef ARM_CORE_CA32
	spin_unlock_irqrestore(&print_lock, isr_status);
#endif

	return ret;
}

int apps_log_write_with_ap(const char *fmt, va_list args)
{
	int ret;

#ifdef ARM_CORE_CA32
	u32 isr_status = spin_lock_irqsave(&print_lock);
#endif

	if (CPU_InInterrupt()) {
		ret = DiagVprintf(fmt, args);
	} else {
		ret = vprintf(fmt, args);
		fflush(stdout);
	}

#ifdef ARM_CORE_CA32
	spin_unlock_irqrestore(&print_lock, isr_status);
#endif

	return ret;
}

unsigned char *
sys_strnchr(unsigned char *s, size_t len, unsigned char c)
{
	while (len && *s++ != c) {
		--len;
	}

	return len ? s : NULL;
}

void sys_memclr(void *dest, size_t n)
{
	memset(dest, 0, n);
}

int sys_vscnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
	int i;

	i = vsnprintf(buf, size, fmt, args);

	if (i < (int)size) {
		return i;
	}
	if (size != 0) {
		return size - 1;
	}
	return 0;
}

int sys_scnprintf(char *buf, size_t size, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = sys_vscnprintf(buf, size, fmt, args);
	va_end(args);

	return i;
}

sys_struct sys = {
	.print_test = print_test,
	syscall_nanosleep,
	syscall_clock_gettime,
	rtos_mem_malloc,
	rtos_mem_zmalloc,
	rtos_mem_calloc,
	rtos_mem_realloc,
	rtos_mem_free,
	rtos_mem_get_free_heap_size,
	syscall_pthread_self,
	syscall_pthread_exit,
	syscall_pthread_attr_init,
	syscall_pthread_attr_destroy,
	syscall_pthread_attr_setschedpolicy,
	syscall_pthread_attr_getschedpolicy,
	syscall_pthread_attr_setstacksize,
	syscall_pthread_attr_getstacksize,
	syscall_pthread_attr_setdetachstate,
	syscall_pthread_attr_getdetachstate,
	syscall_pthread_attr_setschedparam,
	syscall_pthread_attr_getschedparam,
	syscall_pthread_attr_setpriority,
	syscall_pthread_create,
	syscall_pthread_mutex_init,
	syscall_pthread_mutex_destroy,
	syscall_pthread_mutex_lock,
	syscall_pthread_mutex_unlock,
	syscall_pthread_cond_init,
	syscall_pthread_cond_destroy,
	syscall_pthread_cond_signal,
	syscall_pthread_cond_wait,
	syscall_pthread_cond_timedwait,
	syscall_pthread_key_create,
	syscall_pthread_key_delete,
	syscall_pthread_setspecific,
	syscall_pthread_getspecific,
	syscall_pthread_clrspecific,
	syscall_pthread_setname_np,
	syscall_pthread_join,
	syscall_pthread_detach,
	/* lwip */
	syscall_lwip_socket,
	syscall_lwip_bind,
	syscall_lwip_listen,
	syscall_lwip_connect,
	syscall_lwip_accept,
	syscall_lwip_recvfrom,
	syscall_lwip_read,
	syscall_lwip_recv,
	syscall_lwip_sendto,
	syscall_lwip_write,
	syscall_lwip_send,
	syscall_lwip_close,
	syscall_lwip_shutdown,
	syscall_lwip_getpeername,
	syscall_lwip_getsockname,
	syscall_lwip_getsockopt,
	syscall_lwip_setsockopt,
	syscall_lwip_recvmsg,
	syscall_lwip_sendmsg,
	syscall_lwip_select,
	syscall_lwip_poll,
	syscall_lwip_ioctl,
	syscall_lwip_inet_ntop,
	syscall_lwip_inet_pton,
	syscall_lwip_readv,
	syscall_lwip_writev,
	syscall_lwip_fcntl,
	syscall_lwip_errno,
	syscall_lwip_if_indextoname,
	syscall_lwip_if_nametoindex,
	syscall_lwip_htons,
	syscall_lwip_htonl,
	syscall_lwip_getaddrinfo,
	syscall_lwip_freeaddrinfo,
	syscall_ipaddr_addr,
	syscall_lwiperf_start_tcp_server,
	syscall_lwiperf_start_tcp_client,
	syscall_lwiperf_server_start,
	syscall_ip4addr_ntoa,
	syscall_ip4addr_aton,
	/* stdc */
	strlen,
	strnlen,
	strcpy,
	strncpy,
	strcat,
	strncat,
	strcmp,
	strcasecmp,
	strncmp,
	strncasecmp,
	strchr,
	sys_strnchr,
	strrchr,
	strstr,
	strtod,
	strtol,
	atoi,
	atof,
	memcmp,
	memmove,
	memset,
	memcpy,
	sys_memclr,
	memchr,
	vsnprintf,
	sys_vscnprintf,
	snprintf,
	sys_scnprintf,
	vsprintf,
	DiagVSprintf,
	vprintf,
	sscanf,
	/* printf */
	.log_write = apps_log_write,
	apps_log_write_with_ap,
	/* vfs */
	syscall_fopen,
	syscall_fclose,
	syscall_fread,
	syscall_fwrite,
	syscall_fseek,
	syscall_fflush,
	syscall_remove,
	syscall_rename,
	syscall_feof,
	syscall_ferror,
	syscall_ftell,
	syscall_ftruncate,
	syscall_fputc,
	syscall_fputs,
	syscall_fgets,
	syscall_opendir,
	syscall_readdir,
	syscall_closedir,
	syscall_rmdir,
	syscall_mkdir,
	syscall_stat,
	syscall_rand,
};

#else
extern int test22(void);
int test33(void)
{
	return test22();
}
// The section ".sys_struct" will be at the beginning of the tinf which will be replaced
// The value DUMMY_SYS_ADDR is a dummy value which will be sanity checked before
// being replaced with actual sys_struct address on the mcu during app loading
const sys_struct *sys __attribute__((__section__(".sys_struct"))) = (sys_struct *)(DUMMY_SYS_ADDR);

#endif

