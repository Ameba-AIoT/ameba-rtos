/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifndef I2C_EXT_H
#define I2C_EXT_H
#include "platform_autoconf.h"
#if defined (CONFIG_AMEBADPLUS)

//I2C0_SEL
#define MBED_I2C_MTR_SDA    _PB_15
#define MBED_I2C_MTR_SCL    _PB_16

//I2C0_SEL
#define MBED_I2C_SLV_SDA    _PB_15
#define MBED_I2C_SLV_SCL    _PB_16

#elif defined (CONFIG_AMEBAGREEN2)

//I2C0_SEL
#define MBED_I2C_MTR_SDA    _PB_0
#define MBED_I2C_MTR_SCL    _PB_2

//I2C0_SEL
#define MBED_I2C_SLV_SDA    _PB_0
#define MBED_I2C_SLV_SCL    _PB_2

#elif defined (CONFIG_AMEBAD)

//I2C0_SEL
#define MBED_I2C_MTR_SDA    _PA_26
#define MBED_I2C_MTR_SCL    _PA_25

//I2C0_SEL
#define MBED_I2C_SLV_SDA    _PA_26
#define MBED_I2C_SLV_SCL    _PA_25

#endif

#endif
