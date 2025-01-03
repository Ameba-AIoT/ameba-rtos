#include "basic_types.h"
#include "syscall.h"
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/errno.h"
#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/utils.h"

// #define malloc rtos_mem_malloc
// #define calloc rtos_mem_calloc
// #define realloc rtos_mem_realloc
// #define free rtos_mem_free

#define snprintf sys->snprintf
#define scnprintf sys->scnprintf
#define sscanf sys->sscanf

#define printf sys->log_write
#define APP_LOGI(tag, format, ...) do {               \
        printf("[%s-I] ", tag); \
        printf(format, ##__VA_ARGS__); \
    } while(0)

#define APP_LOGD(tag, format, ...) do {               \
        printf("[%s-D] ", tag); \
        printf(format, ##__VA_ARGS__); \
    } while(0)

#define APP_LOGE(tag, format, ...) do {               \
        printf("[%s-E] ", tag); \
        printf(format, ##__VA_ARGS__); \
    } while(0)

#define APP_LOGW(tag, format, ...) do {               \
        printf("[%s-W] ", tag); \
        printf(format, ##__VA_ARGS__); \
    } while(0)

void set_got_base(uint32_t app_got_base);
