/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AMEBA_RTOS_VERSION_H
#define AMEBA_RTOS_VERSION_H

/** Major version number (X.x.x) */
#define AMEBA_RTOS_VERSION_MAJOR   1
/** Minor version number (x.X.x) */
#define AMEBA_RTOS_VERSION_MINOR   2
/** Patch version number (x.x.X) */
#define AMEBA_RTOS_VERSION_PATCH   0

/**
 * Macro to convert version number into an integer
 */
#define AMEBA_RTOS_VERSION_VAL(major, minor, patch) ((major << 16) | (minor << 8) | (patch))

/**
 * Current ameba-rtos version, as an integer
 */
#define AMEBA_RTOS_VERSION()  AMEBA_RTOS_VERSION_VAL(AMEBA_RTOS_VERSION_MAJOR, \
                                             AMEBA_RTOS_VERSION_MINOR, \
                                             AMEBA_RTOS_VERSION_PATCH)

#define ameba_rtos_get_version() \
	do { \
		RTK_LOGI("VER", "AMEBA-RTOS SDK VERSION: %d.%d.%d\n", AMEBA_RTOS_VERSION_MAJOR, AMEBA_RTOS_VERSION_MINOR, AMEBA_RTOS_VERSION_PATCH); \
	} while(0)
#endif