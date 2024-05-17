/*
 * Copyright (c) 2019-2022 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * \file platform_ns_device_definition.h
 * \brief Definitions for peripherals used on the
 * non-secure side of application.
 */

#ifndef __PLATFORM_NS_DEVICE_DEFINITION_H__
#define __PLATFORM_NS_DEVICE_DEFINITION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "device_cfg.h"

/* ======= Defines peripheral configuration structures ======= */

/* UART CMSDK driver structures */
#include "uart_cmsdk_drv.h"
extern struct uart_cmsdk_dev_t UART0_CMSDK_DEV_NS;

#ifdef __cplusplus
}
#endif

#endif  /* __PLATFORM_NS_DEVICE_DEFINITION_H__ */
