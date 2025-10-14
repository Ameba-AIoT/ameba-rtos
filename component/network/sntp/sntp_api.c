#include "ameba_soc.h"
#include "sntp_api.h"

extern int _gettimeofday(struct timeval *ptimeval, void *ptimezone);
extern int _settimeofday(struct timeval *ptimeval, void *ptimezone);

static uint32_t sntp_update_interval = 15000;

void sntp_set_update_interval(uint32_t interval_ms)
{
	if (interval_ms < 15000) {
		interval_ms = 15000;
	}
	sntp_update_interval = interval_ms;
}

uint32_t sntp_get_update_interval(void)
{
	return sntp_update_interval;
}

void sntp_set_system_time(uint32_t sec, uint32_t us)
{
	struct timeval tv = { .tv_sec = sec, .tv_usec = us };
	_settimeofday(&tv, NULL);
}

void sntp_get_system_time(uint32_t *sec, uint32_t *us)
{
	struct timeval tv = { .tv_sec = 0, .tv_usec = 0 };
	_gettimeofday(&tv, NULL);
	*(sec) = tv.tv_sec;
	*(us) = tv.tv_usec;
}