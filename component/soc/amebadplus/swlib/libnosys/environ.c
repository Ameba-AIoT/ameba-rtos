/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Version of environ for no OS.
 */

char *__env[1] = { 0 };
char **environ = __env;
