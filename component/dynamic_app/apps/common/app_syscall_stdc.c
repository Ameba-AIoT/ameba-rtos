/**
 ******************************************************************************
 * @file    app_syscall_posix.c
 * @brief   Contains APIs for system calls from the APP
 ******************************************************************************
 * @attention
 *
*/
// #include "basic_types.h"
#include "app_syscall.h"
#include "os_wrapper.h"

size_t __wrap_strlen(const char *str)
{
	return sys->strlen(str);
}

size_t __wrap_strnlen(const char *str, size_t maxlen)
{
	return sys->strnlen(str, maxlen);
}

char *__wrap_strcpy(char *dst0, const char *src0)
{
	return sys->strcpy(dst0, src0);
}

char *__wrap_strncpy(char *dst0, const char *src0, size_t count)
{
	return sys->strncpy(dst0, src0, count);
}

char *__wrap_strcat(char *dst, const char *src)
{
	return sys->strcat(dst, src);
}

char *__wrap_strncat(char *dst, const char *src, size_t n)
{
	return sys->strncat(dst, src, n);
}

int __wrap_strcmp(const char *s1,	const char *s2)
{
	return sys->strcmp(s1, s2);
}
int __wrap_stricmp(const char *str1, const char *str2)
{
	return sys->stricmp(str1, str2);
}
int __wrap_strncmp(const char *s1, const char *s2, size_t n)
{
	return sys->strncmp(s1, s2, n);
}
int __wrap_strnicmp(const char *s1, const char *s2, size_t n)
{
	return sys->strnicmp(s1, s2, n);
}
char *__wrap_strchr(const char *s, int n)
{
	return sys->strchr(s, n);
}
unsigned char *__wrap_strnchr(unsigned char *s, size_t len, unsigned char c)
{
	return sys->strnchr(s, len, c);
}
char *__wrap_strrchr(const char *s, int n)
{
	return sys->strrchr(s, n);
}
char *__wrap_strstr(const char *s1, const char *s2)
{
	return sys->strstr(s1, s2);
}
double __wrap_strtod(const char *__restrict s00, char **__restrict se)
{
	return sys->strtod(s00, se);
}
long int __wrap_strtol(const char *__restrict s, char **__restrict ptr, int base)
{
	return sys->strtol(s, ptr, base);
}
int __wrap_atoi(const char *s)
{
	return sys->atoi(s);
}
double __wrap_atof(const char *s)
{
	return sys->atof(s);
}
int __wrap_memcmp(const void *m1, const void *m2, size_t n)
{
	return sys->memcmp(m1, m2, n);
}
void *__wrap_memmove(void *dst_void, const void *src_void, size_t length)
{
	return sys->memmove(dst_void, src_void, length);
}
void *__wrap_memset(void *m, int c, size_t n)
{
	return sys->memset(m, c, n);
}
void *__wrap_memcpy(void *__restrict dst0, const void *__restrict src0, size_t len0)
{
	return sys->memcpy(dst0, src0, len0);
}
void __wrap_memclr(void *dest, size_t n)
{
	sys->memclr(dest, n);
}
void *__wrap_memchr(const void *src_void, int c, size_t length)
{
	return sys->memchr(src_void, c, length);
}
int __wrap_vsnprintf(char *__restrict str, size_t size, const char *__restrict fmt, va_list ap)
{
	return sys->vsnprintf(str, size, fmt, ap);
}
int __wrap_vscnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
	return sys->vscnprintf(buf, size, fmt, args);
}

int __wrap_vsprintf(char *__restrict str, const char *__restrict fmt, va_list ap)
{
	return sys->vsprintf(str, fmt, ap);
}

int __wrap_vprintf(const char *fmt, va_list ap)
{
	return sys->vprintf(fmt, ap);
}

int __wrap_sprintf(char *str, const char *fmt, ...)
{
	int ret;
	va_list ap;

	va_start(ap, fmt);
	ret = sys->sprintf(str, fmt, ap);
	va_end(ap);

	return ret;
}