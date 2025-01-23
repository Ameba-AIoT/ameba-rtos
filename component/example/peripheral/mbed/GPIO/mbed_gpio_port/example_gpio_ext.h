/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifndef GPIO_EXT_H
#define GPIO_EXT_H

#include "platform_autoconf.h"

#define PORT_OUTPUT_TEST	1 // 0: input test, 1: output test
#define GPIO_TEST_PORT		0 // 0: GPIO_PORT_A, 1: GPIO_PORT_B

#define LED_PATTERN_NUM		12

/*
pin_mask: each bit map to one pin: 0: pin disable; 1: pin enable
led_pattern[]: each bit map to one pin for output test: 0- pin output low; 1- pin output high
*/

#if defined (CONFIG_AMEBASMART)
//each bit map to one pin: 0: pin disable; 1: pin enable
uint32_t pin_mask = 0xFF;

//each bit map to one pin for output test: 0- pin output low; 1- pin output high
const uint32_t led_pattern[LED_PATTERN_NUM] = {0xFF, 0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F, 0xFF, 0x0, 0x0};

#elif defined (CONFIG_AMEBALITE)

#if GPIO_TEST_PORT	//portB
uint32_t pin_mask = 0x1C;
const uint32_t led_pattern[LED_PATTERN_NUM] = {0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x18, 0x14, 0xC, 0x1C, 0x0};

#else	//portA
uint32_t pin_mask = 0xF0000003;
const uint32_t led_pattern[LED_PATTERN_NUM] = {0xF0000003, 0xF0000002, 0xF0000001, 0xE0000003, 0xD0000003, 0xB0000003, 0x70000003, 0xF0000003, 0xF0000003, 0xF0000003, 0xF0000003, 0x0};

#endif

#elif defined (CONFIG_AMEBADPLUS)
uint32_t pin_mask = 0x0FF00000;

//each bit map to one pin: 0: pin disable; 1: pin enable
const uint32_t led_pattern[LED_PATTERN_NUM] = {0x0FF00000, 0x0FE00000, 0x0FD00000, 0x0FB00000, 0x0F700000, 0x0EF00000, 0x0DF00000, 0x0BF00000, 0x0FF00000, 0x0FE00000, 0x0EF00000, 0x0EB00000};

#elif defined (CONFIG_AMEBAD)
//for PortA
uint32_t pin_mask = 0x20FF000;
const uint32_t led_pattern[LED_PATTERN_NUM] = {0x20FF000, 0x20FE000, 0x20FD000, 0x20FB000, 0x20F7000, 0x20EF000, 0x20DF000, 0x20BF000, 0x207F000, 0x00FF000, 0x20FF000, 0x0000000};

#endif
extern int example_mbed_gpio_port(void);
#endif
