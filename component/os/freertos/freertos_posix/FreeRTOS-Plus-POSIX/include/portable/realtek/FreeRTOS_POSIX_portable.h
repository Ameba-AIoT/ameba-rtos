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
#define posixconfigENABLE_MODE_T 0
#define posixconfigENABLE_TIMER_T 0
#define posixconfigENABLE_TM 0

/*
 * GNU Tools for Ameba already defines timespec and itimerspec.
 */
#define posixconfigENABLE_TIMESPEC 0
#include <sys/_timespec.h>

#define posixconfigENABLE_ITIMERSPEC 0
#include <sys/timespec.h>

/* Define _SYS__PTHREADTYPES_H_ to disable GNU Tools for Ameba
 * definitions that differ from FreeRTOS+POSIX implementation.
 */
#define _SYS__PTHREADTYPES_H_
#include <sys/types.h>

#endif // _FREERTOS_POSIX_PORTABLE_H_
