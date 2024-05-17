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

// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pal_interfaces.h"
#include "platform.h"

#define PORT     7777

int32_t val_entry(void);

extern uint8_t buffer[4096];
uint8_t buffer[4096];
char *key_file, *chain_file;

// Driver code
int main(int argc, char *argv[])
{
	char *hostname;
	int portno;
    udp_socket_desc_t sock_desc;

    if (argc < 5) {
		printf("Usage:\n\tpsa_adac_test <keyfile> <chainfile> <hostname> <port>\n\n");
        exit(-1);
    }

    key_file = argv[1];
    chain_file = argv[2];
    sock_desc.hostname = argv[3];
    sock_desc.port_num = atoi(argv[4]);

    msg_interface_init((void *) &sock_desc, buffer, sizeof(buffer));

    val_entry();
    msg_interface_free(NULL);

    return 0;
}
