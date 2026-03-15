#ifndef _EXAMPLE_WIFICAST_DEBUG_H_
#define _EXAMPLE_WIFICAST_DEBUG_H_

#include "../wificast_defs.h"
#include "os_wrapper.h"


/**
 * @brief Console command
 */
struct command_arg_s {
	u8 command[UART_LOG_CMD_BUFLEN]; /* console command receive from uart */
} __attribute__((packed));

/**
 * @brief Console log info
 */
struct log_info_s {
	const char *tag;        /* log tag */
	rtk_log_level_t level;  /* log level */
	int size;               /* log size */
	char *data;             /* log data */
} __attribute__((packed));

#endif /* _EXAMPLE_WIFICAST_DEBUG_H_ */