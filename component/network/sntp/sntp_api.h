#ifndef __SNTP_H__
#define __SNTP_H__

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

void sntp_set_update_interval(uint32_t interval_ms);
uint32_t sntp_get_update_interval(void);

void sntp_set_system_time(uint32_t sec, uint32_t us);
void sntp_get_system_time(uint32_t *sec, uint32_t *us);

#ifdef __cplusplus
}
#endif

#endif /* __SNTP_H__ */
