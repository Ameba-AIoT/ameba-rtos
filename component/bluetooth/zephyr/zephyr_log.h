#ifndef  __ZEPHYR_LOG_H__
#define  __ZEPHYR_LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_LEVEL_NONE 0U
#define LOG_LEVEL_ERR  1U
#define LOG_LEVEL_WRN  2U
#define LOG_LEVEL_INF  3U
#define LOG_LEVEL_DBG  4U

#define ZLOG_STACK_MODE_OFF       0
#define ZLOG_STACK_MODE_PRINTF    1
#define ZLOG_STACK_MODE_ARGS      2

/* Can be configured */
#define ZLOG_SNOOP_ON 	          0
#define ZLOG_STACK_MODE           ZLOG_STACK_MODE_OFF
#define ZLOG_MAX_LEVEL            LOG_LEVEL_DBG

void z_printf(const char *format, ...);

void zephyr_btlog_init(void);

void zephyr_btlog_deinit(void);

/* create snoop packet and send to host computer */
void btsnoop_send(uint8_t type, uint8_t *data, uint32_t len, bool c2h);

void zephyr_stacklog_args_send(char level, int log_idx, char num, ...);

uint32_t zlog_level_get(void);

#ifdef __cplusplus
}
#endif

#endif /* __ZEPHYR_LOG_H__ */
