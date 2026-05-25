#ifndef __SDN_WATCHDOG_H__
#define __SDN_WATCHDOG_H__

#define SDN_WATCHDOG_INTERVAL   2000

void sdn_watchdog_init(void);
void sdn_watchdog_deinit(void);
void sdn_watchdog_stop(void);
void sdn_watchdog_resume(void);
#endif