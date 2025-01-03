/**
 * *****************************************************************************
 * @file    entropy_hardware_alt.c
 * @author  
 * @version V1.0.0
 * @date    2023-09-11
 * @brief   MBEDTLS_ENTROPY_HARDWARE_ALT 
 * *****************************************************************************
 * @attention
 * 
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 * 
 * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#include "entropy_poll.h"
#include "ameba_soc.h"

int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen)
{
    UNUSED(data);

    TRNG_get_random_bytes(output, len);

    *olen = len;
    return 0;
}


