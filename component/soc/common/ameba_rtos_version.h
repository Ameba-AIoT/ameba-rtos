/**
  ******************************************************************************
  * @file    ameba_rtos_version.h
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */
#ifndef AMEBA_RTOS_VERSION_H
#define AMEBA_RTOS_VERSION_H

/** Major version number (X.x.x) */
#define AMEBA_RTOS_VERSION_MAJOR   1
/** Minor version number (x.X.x) */
#define AMEBA_RTOS_VERSION_MINOR   0
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