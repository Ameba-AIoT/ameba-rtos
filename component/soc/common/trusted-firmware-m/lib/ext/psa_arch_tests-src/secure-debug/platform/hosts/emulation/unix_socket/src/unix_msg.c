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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#if !defined(_MSC_VER)
    #include <unistd.h>
    #include <sys/socket.h>
    #include <sys/un.h>
#else // !defined(_MSC_VER)
    #include <WinSock2.h>
    #include <afunix.h>
    #include <io.h>

    // Disable warning about POSIX function names.
    #pragma warning(disable : 4996)
#endif // !defined(_MSC_VER)

void unix_socket_close(int fd)
{
    if (fd != -1) {
#if defined(_MSC_VER)
        closesocket(fd);
#else
        close(fd);
#endif
    }
}

bool unix_socket_init(void)
{
#if defined(_MSC_VER)
    static bool did_init_winsock;

    did_init_winsock = false;
    if (!did_init_winsock) {
        WSADATA wsa_data = { 0 };

        int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);

        if (result != 0) {
            printf("WSAStartup failed with error: %d\n", result);
            return false;
        }
    }
#endif
    return true;
}

int unix_socket_server(const char *path)
{
    struct sockaddr_un addr;
    int fd;

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("Error while calling socket()");
        goto error;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

    if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
        perror("Error while calling bind()");
        goto error;
    }

    if (listen(fd, 1) == -1) {
        perror("Error while calling listen()");
        goto error;
    }
    return fd;

error:
    unix_socket_close(fd);
    return -1;
}

int unix_socket_client(const char *path)
{
    struct sockaddr_un addr;
    int fd;

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("Error while calling socket()");
        goto error;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

    if (connect(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("Error while calling connect()");
        goto error;
    }
    return fd;

error:
    unix_socket_close(fd);
    return -1;
}

ssize_t nwrite(int fd, const uint8_t *buf, size_t count)
{
    const char *ptr = (const char *) buf;
    ssize_t t;
    size_t left = count;

    while (left) {
        t = send(fd, ptr, left, 0);
        if (t  < 0)
            return -1;
        left -= t;
        ptr += t;
    }
    return count;
}

ssize_t nread(int fd, uint8_t *buf, size_t count)
{
    char *ptr = (char *) buf; /* Removes warning on pointer operation */
    ssize_t t = 0;
    size_t left = count;

    while (left) {
        t = recv(fd, ptr, left, 0);
        if (t < 0)
            return -1;
        left -= t;
        ptr += t;
    }
    return count;
}
