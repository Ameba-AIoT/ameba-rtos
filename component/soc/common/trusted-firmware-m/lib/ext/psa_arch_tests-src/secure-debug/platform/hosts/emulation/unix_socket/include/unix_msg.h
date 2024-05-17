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

#ifndef PSA_ADAC_UNIX_MSG_H
#define PSA_ADAC_UNIX_MSG_H

#include <stdint.h>
#include <stdbool.h>

#if !defined(_MSC_VER)
#include <sys/types.h>
#else // !defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif // !defined(_MSC_VER)

bool unix_socket_init(void);

int unix_socket_server(const char *path);
int unix_socket_client(const char *path);

void unix_socket_close(int fd);

ssize_t nwrite(int fd, const uint8_t *buf, size_t count);
ssize_t nread(int fd, uint8_t *buf, size_t count);

#endif //PSA_ADAC_UNIX_MSG_H
