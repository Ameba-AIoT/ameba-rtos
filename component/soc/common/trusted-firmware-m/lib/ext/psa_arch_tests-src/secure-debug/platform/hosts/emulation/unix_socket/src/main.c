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

#include <stdio.h>
#include <stdlib.h>
#include "pal_interfaces.h"
#include "unix_msg.h"

int32_t val_entry(void);

/**
    @brief    - PSA C main function, used for generating host-side test binaries.
    @param    - argc    : the number of command line arguments.
                argv    : array containing command line arguments.
    @return   - error status
**/
extern uint8_t buffer[4096];
uint8_t buffer[4096];
char *key_file, *chain_file;

int main(int argc, char *argv[])
{
    if (argc < 4) {
        printf("Usage:\n\tpsa_adac_test <keyfile> <chainfile> <socket>\n\n");
        exit(-1);
    }

    key_file = argv[1];
    chain_file = argv[2];
    char *socket_path = argv[3];
    int fd;

    fd = unix_socket_client(socket_path);
    if (-1 == fd)
        exit(-1);

    msg_interface_init((void *) &fd, buffer, sizeof(buffer));

    return val_entry();

    msg_interface_free(NULL);
    return 0;
}
