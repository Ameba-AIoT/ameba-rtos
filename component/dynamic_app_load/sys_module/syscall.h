/**
 ******************************************************************************
 * @file    syscall.h
 * @brief   Contains APIs for system calls from the APP
 ******************************************************************************
*/
#ifndef _SYSCALL_H_
#define _SYSCALL_H_

/* C standard library includes. */
#include <stddef.h>
#include <stdarg.h>

// #include "ameba.h"
#include "syscall_posix.h"
#include "syscall_socket.h"
#include "syscall_vfs.h"
/**
    @brief  This struct contains APIs for calling system functions
            It is basically a jump table.
            Depending on weather the KERNEL is built or the libs for the app,
            different sys_struct definitions are compiled.
            For Kernel:
                The complete struct with each member is compiled
            For userlib for the app:
                Only a 'const sys_struct* sys' pointer is defined. The address
                of this pointer is made to point to the actual sys_struct in the
                kernel at load time in the LoadApp API
*/
typedef struct {
	void (*print_test)(uint8_t led_num, uint8_t led_status);
	/* clock */
	int (*nanosleep)(const void *rqtp, void *rmtp);
	int (*clock_gettime)(int clock_id, void *tp);
	/* memory */
	void *(*rtos_mem_malloc)(uint32_t size);
	void *(*rtos_mem_zmalloc)(uint32_t size);
	void *(*rtos_mem_calloc)(uint32_t elementNum, uint32_t elementSize);
	void *(*rtos_mem_realloc)(void *pbuf, uint32_t size);
	void (*rtos_mem_free)(void *pbuf);
	uint32_t (*rtos_mem_get_free_heap_size)(void);
	/* pthread */
	sys_pthread_t (*pthread_self)(void);
	void (*pthread_exit)(void *value_ptr);
	int (*pthread_attr_init)(void *attr);
	int (*pthread_attr_destroy)(void *attr);
	int (*pthread_attr_setschedpolicy)(void *attr, int policy);
	int (*pthread_attr_getschedpolicy)(void const *attr, int *policy);
	int (*pthread_attr_setstacksize)(void *attr, size_t stack_size);
	int (*pthread_attr_getstacksize)(void const *attr, size_t *stack_size);
	int (*pthread_attr_setdetachstate)(void *attr, int state);
	int (*pthread_attr_getdetachstate)(void const *attr, int *state);
	int (*pthread_attr_setschedparam)(void *attr, void const *param);
	int (*pthread_attr_getschedparam)(void const *attr, void *param);
	int (*pthread_attr_setpriority)(void *attr, uint8_t priority);
	int (*pthread_create)(sys_pthread_t *tid, const void *attr,
						  void *(*start)(void *), void *arg, uint32_t got_base_addr);

	int (*pthread_mutex_init)(void *mutex, const void *attr);
	int (*pthread_mutex_destroy)(void *mutex);
	int (*pthread_mutex_lock)(void *mutex);
	int (*pthread_mutex_unlock)(void *mutex);

	int (*pthread_cond_init)(void *cond, void *attr);
	int (*pthread_cond_destroy)(void *cond);
	int (*pthread_cond_signal)(void *cond);
	int (*pthread_cond_wait)(void *cond, void *mutex);
	int (*pthread_cond_timedwait)(void *cond, void *mutex, void *abstime);

	int (*pthread_key_create)(sys_pthread_key_t *key, void (*destructor)(void *));
	int (*pthread_key_delete)(sys_pthread_key_t key);
	int (*pthread_setspecific)(sys_pthread_key_t key, const void *value);
	void *(*pthread_getspecific)(sys_pthread_key_t key);
	void (*pthread_clrspecific)(sys_pthread_key_t key);

	int (*pthread_setname_np)(sys_pthread_t thread, const char *name);
	int (*pthread_join)(sys_pthread_t thread, void **retval);
	int (*pthread_detach)(sys_pthread_t thread);
	/* lwip */
	int (*lwip_socket)(int domain, int type, int protocol);
	int (*lwip_bind)(int s, void *name, int namelen);
	int (*lwip_listen)(int s, int backlog);
	int (*lwip_connect)(int s, void *name, int namelen);
	int (*lwip_accept)(int s, void *addr, void *addrlen);
	int (*lwip_recvfrom)(int s, void *mem, int len, int flags, void *from, int *fromlen);
	int (*lwip_read)(int s, void *mem, int len);
	int (*lwip_recv)(int s, void *mem, int len, int flags);
	int (*lwip_sendto)(int s, void *data, int size, int flags, void *to, int tolen);
	int (*lwip_write)(int s, void *data, int size);
	int (*lwip_send)(int s, void *data, int size, int flags);
	int (*lwip_close)(int s);
	int (*lwip_shutdown)(int s, int how);
	int (*lwip_getpeername)(int s, void *name, int *namelen);
	int (*lwip_getsockname)(int s, void *name, int *namelen);
	int (*lwip_getsockopt)(int s, int level, int optname, void *optval, int *optlen);
	int (*lwip_setsockopt)(int s, int level, int optname, void *optval, int optlen);
	int (*lwip_recvmsg)(int s, void *message, int flags);
	int (*lwip_sendmsg)(int s,  void *message, int flags);
	int (*lwip_select)(int maxfdp1, void *readset, void *writeset, void *exceptset, void *timeout);
	int (*lwip_poll)(void *fds, int nfds, int timeout);
	int (*lwip_ioctl)(int s, long cmd, void *argp);
	const char *(*lwip_inet_ntop)(int af, void *src, char *dst, int size);
	int (*lwip_inet_pton)(int af, void *src, void *dst);
	int (*lwip_readv)(int s, void *iov, int iovcnt);
	int (*lwip_writev)(int s, void *iov, int iovcnt);
	int (*lwip_fcntl)(int s, int cmd, int val);
	int (*lwip_errno)(void);
	char *(*lwip_if_indextoname)(unsigned int ifindex, void *ifname);
	unsigned int (*lwip_if_nametoindex)(void *ifname);
	unsigned short (*lwip_htons)(unsigned short n);
	unsigned int (*lwip_htonl)(unsigned int n);
	int (*lwip_getaddrinfo)(void *nodename, void *servname, void *hints, void **res);
	void (*lwip_freeaddrinfo)(void *ai);
	unsigned int (*ipaddr_addr)(const char *cp);
	void *(*lwiperf_start_tcp_server)(void *local_addr, unsigned short local_port, void *report_fn, void *report_arg);
	void *(*lwiperf_start_tcp_client)(void *remote_addr, unsigned short remote_port, int type, void *report_fn, void *report_arg);
	void (*lwiperf_server_start)(void);
	char *(*ip4addr_ntoa)(void *addr);
	int (*ip4addr_aton)(const char *cp, void *addr);
	/* stdc*/
	size_t (*strlen)(const char *str);
	size_t (*strnlen)(const char *str, size_t maxlen);
	char *(*strcpy)(char *dst0, const char *src0);
	char *(*strncpy)(char *dst0, const char *src0, size_t count);
	char *(*strcat)(char *dst, const char *src);
	char *(*strncat)(char *dst, const char *src, size_t n);
	int (*strcmp)(const char *s1,	const char *s2);
	int (*stricmp)(const char *str1, const char *str2);
	int (*strncmp)(const char *s1, const char *s2, size_t n);
	int (*strnicmp)(const char *s1, const char *s2, size_t n);
	char *(*strchr)(const char *s, int n);
	unsigned char *(*strnchr)(unsigned char *s, size_t len, unsigned char c);
	char *(*strrchr)(const char *s, int n);
	char *(*strstr)(const char *s1, const char *s2);
	double (*strtod)(const char *__restrict s00, char **__restrict se);
	long int (*strtol)(const char *__restrict s, char **__restrict ptr, int base);
	int (*atoi)(const char *s);
	double (*atof)(const char *s);
	int (*memcmp)(const void *m1, const void *m2, size_t n);
	void *(*memmove)(void *dst_void, const void *src_void, size_t length);
	void *(*memset)(void *m, int c, size_t n);
	void *(*memcpy)(void *__restrict dst0, const void *__restrict src0, size_t len0);
	void (*memclr)(void *dest, size_t n);
	void *(*memchr)(const void *src_void, int c, size_t length);
	int (*vsnprintf)(char *__restrict str, size_t size, const char *__restrict fmt, va_list ap);
	int (*vscnprintf)(char *buf, size_t size, const char *fmt, va_list args);
	int (*snprintf)(char *__restrict str, size_t size, const char *__restrict fmt, ...);
	int (*scnprintf)(char *buf, size_t size, const char *fmt, ...);
	int (*vsprintf)(char *__restrict str, const char *__restrict fmt, va_list ap);
	int (*sprintf)(char *str, const char *fmt, va_list ap);
	int (*vprintf)(const char *fmt, va_list ap);

	int (*sscanf)(const char *buf, const char *fmt, ...);

	/* printf */
	int (*log_write)(const char *fmt, ...);
	int (*log_write_with_ap)(const char *fmt, va_list args);
	/* vfs */
	void *(*fopen)(const char *filename, const char *mode);
	int (*fclose)(void *stream);
	int (*fread)(void *ptr, int size, int count, void *stream);
	int (*fwrite)(const void *ptr, int size, int count, void *stream);
	int (*fseek)(void *stream, long int offset, int origin);
	int (*fflush)(void *stream);
	int (*remove)(const char *filename);
	int (*rename)(const char *oldname, const char *newname);
	int (*feof)(void *stream);
	int (*ferror)(void *stream);
	long int (*ftell)(void *stream);
	long int (*ftruncate)(void *stream, int length);
	int (*fputc)(int character, void *stream);
	int (*fputs)(const char *str, void *stream);
	char *(*fgets)(char *str, int num, void *stream);
	void *(*opendir)(const char *name);
	void *(*readdir)(void *pdir);
	int (*closedir)(void *dirp);
	int (*rmdir)(const char *path);
	int (*mkdir)(const char *pathname, int mode);
	int (*stat)(const char *path, void *buf);
	uint32_t (*rand)(void);
} sys_struct;

typedef struct {
	uint8_t magic[8];              // magic unique to recognize module
	uint32_t type;                 // module type
	uint32_t got_base_addr;        // global offset table(got) address
	void (*set_got_base)(uint32_t app_got_base);         // api to set got address
} apps_common_magic_t;

typedef struct {
	apps_common_magic_t magic;
	/* export function pointer */
	uint32_t test_pthread;
	uint32_t test_socket;
	uint32_t test_pass_params;
	uint32_t app_main;
	// uint32_t rsvd[3];
} apps_1_struct_t;

typedef struct {
	apps_common_magic_t magic;
	/* export function pointer */
	void (*test_pthread)(void);
} apps_2_struct_t;

typedef enum {
	AMOD_TYPE_MOD_TEST_1 = 0x0,
	AMOD_TYPE_MOD_TEST_2 = 0x1,
} apps_module_type_t;

#ifdef KERNEL
extern sys_struct sys;
#else
extern const sys_struct *sys;
#endif

#define DUMMY_SYS_ADDR 0xA5A5A5A5

#endif