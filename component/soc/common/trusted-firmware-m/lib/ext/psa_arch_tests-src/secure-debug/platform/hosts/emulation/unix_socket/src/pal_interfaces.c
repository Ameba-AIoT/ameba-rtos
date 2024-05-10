/** @file
 * Copyright (c) 2022 Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

#include "unix_msg.h"
#include <pal_interfaces.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(_MSC_VER)
    #include <unistd.h>
#else // !defined(_MSC_VER)
    #include <io.h>

    // Disable warning about POSIX function names.
    #pragma warning(disable : 4996)
#endif // !defined(_MSC_VER)

#define PAL_STATUS_UNSUPPORTED_FUNC      0xFF

typedef enum {
    PAL_STATUS_SUCCESS = 0x0,
    PAL_STATUS_ERROR   = 0x80
} pal_status_t;

static int _fd;

int pal_print(const char *str, int32_t data)
{
    if (printf(str, data) < 0)
    {
        return PAL_STATUS_ERROR;
    }
    return PAL_STATUS_SUCCESS;
}

void pal_terminate_simulation(void)
{
    ;
}

int pal_system_reset(void)
{
    return PAL_STATUS_UNSUPPORTED_FUNC;
}

int pal_msg_interface_init(void *ctx)
{
    unix_socket_init();

    if (ctx == NULL)
        return -1;

    _fd = *((int *) ctx);
    return 0;
}

int pal_msg_interface_free(void *ctx)
{
    unix_socket_close(_fd);
    _fd = -1;
    return 0;
}

int pal_message_send(uint8_t buffer[], size_t size)
{
    return (nwrite(_fd, (uint8_t *) buffer, size) == size ? 0 : -1);
}

int pal_message_receive(uint8_t buffer[], size_t size)
{
    return nread(_fd, buffer, size);
}

