/** @file
 * Copyright (c) 2022, Arm Limited or its affiliates. All rights reserved.
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

#include "platform.h"
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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PAL_STATUS_UNSUPPORTED_FUNC      0xFF

typedef enum {
    PAL_STATUS_SUCCESS = 0x0,
    PAL_STATUS_ERROR   = 0x80
} pal_status_t;

static int sockfd;
static struct sockaddr_in serveraddr;
static struct hostent *server;

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
    udp_socket_desc_t *desc = NULL;

    if (ctx == NULL)
        return -1;
    else
        desc = (udp_socket_desc_t *)(ctx);

    // Creating socket file descriptor
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(-1);
    }

    // gethostbyname: get the server's DNS entry
    server = gethostbyname(desc->hostname);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host as %s\n", desc->hostname);
        exit(-1);
    }

    // Build the server's Internet address
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(desc->port_num);

    return 0;
}

int pal_msg_interface_free(void *ctx)
{
    close(sockfd);
    return 0;
}

int pal_message_send(uint8_t buffer[], size_t size)
{
    sendto(sockfd, (const char *)buffer, size, 0, (const struct sockaddr *) &serveraddr,
                                                   sizeof(serveraddr));
    return (int)size;
}

int pal_message_receive(uint8_t buffer[], size_t size)
{
    int n = 0, len = 0;
	do {
		n = recvfrom(sockfd, (char *)buffer, 4096, MSG_WAITALL,
                             (struct sockaddr *) &serveraddr, &len);
	} while (n == 0);

	if (len < 0) {
		close(sockfd);
		exit(-1);
	}
	return size;
}

