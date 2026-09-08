/**
 * @file FreeRTOS_POSIX_portable.h
 * @brief Port-specific configuration of FreeRTOS+POSIX.
 */

#ifndef _FREERTOS_POSIX_PORTABLE_H_
#define _FREERTOS_POSIX_PORTABLE_H_

/*
 * GNU Tools for Ameba already defines mode_t, clockit_t,
 * timer_t and tm. Use GNU Tools <sys/types.h> definitions.
 */
#define posixconfigENABLE_CLOCKID_T 0
#define posixconfigENABLE_MODE_T    0
#define posixconfigENABLE_TIMER_T   0
#define posixconfigENABLE_TM        0

/*
 * GNU Tools for Ameba already defines timespec and itimerspec.
 */
#define posixconfigENABLE_TIMESPEC 0
/* sys/_timespec.h uses __time_t which is defined in sys/_types.h.
 * Include sys/_types.h first to ensure __time_t is visible regardless of
 * the C library (picolibc, newlib …). It carries its own include guard. */
#include <sys/_types.h>
#include <sys/_timespec.h>

#define posixconfigENABLE_ITIMERSPEC 0
#ifdef __PICOLIBC__
/* picolibc sys/timespec.h lacks struct itimerspec; define it directly. */
struct itimerspec {
    struct timespec it_interval;
    struct timespec it_value;
};
#else
#include <sys/timespec.h>
#endif

/* Define _SYS__PTHREADTYPES_H_ to disable GNU Tools for Ameba
 * definitions that differ from FreeRTOS+POSIX implementation.
 */
#define _SYS__PTHREADTYPES_H_
#include <sys/types.h>

#endif // _FREERTOS_POSIX_PORTABLE_H_
