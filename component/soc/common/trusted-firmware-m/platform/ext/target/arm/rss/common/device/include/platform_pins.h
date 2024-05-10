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
 * \file platform_pins.h
 * \brief This file defines all the pins for this platform.
 */

#ifndef __PLATFORM_PINS_H__
#define __PLATFORM_PINS_H__

/* AHB GPIO pin names */
enum arm_gpio_pin_name_t {
    AHB_GPIO0_0 = 0U,
    AHB_GPIO0_1,
    AHB_GPIO0_2,
    AHB_GPIO0_3,
    AHB_GPIO0_4,
    AHB_GPIO0_5,
    AHB_GPIO0_6,
    AHB_GPIO0_7,
    AHB_GPIO1_0 = 0U,
    AHB_GPIO1_1,
    AHB_GPIO1_2,
    AHB_GPIO1_3,
    AHB_GPIO1_4,
    AHB_GPIO1_5,
    AHB_GPIO1_6,
    AHB_GPIO1_7,
};

#endif  /* __PLATFORM_PINS_H__ */
