/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

u32 LOG_PRINTF_BUFFER(const char *fmt);
u32 LOG_PRINTF_BUFFER_INIT(u32 thread_init);
u32 LOG_BUFF_SUSPEND(void);
u32 LOG_BUFF_RESUME(void);

#define LOG_BUFFER_NUM					6
#define LOG_BUFFER_SIZE				512
typedef struct {
	char buffer[LOG_BUFFER_SIZE];
	/* please define member after buffer */
} log_buffer_t;
extern log_buffer_t log_buffer[];

typedef u32(*DIAG_PRINT_BUF_FUNC)(const char *fmt);

extern DIAG_PRINT_BUF_FUNC ConfigDebugBufferGet;
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
