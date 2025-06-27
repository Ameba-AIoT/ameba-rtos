/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ATCMD_NETWORK_H
#define ATCMD_NETWORK_H

extern int cmd_iperf(int argc, char **argv);
extern int cmd_ping(int argc, char **argv);

void print_network_at(void);
void at_network_init(void);

#endif /* ATCMD_NETWORK_H */