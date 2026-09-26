/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include <zephyr_sys.h>
#include <osif.h>
#include <zephyr/sys/util.h>

uint32_t sys_rand32_get(void)
{
    uint32_t val;
    osif_rand(&val, 4);
    return val;
}
