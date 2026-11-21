#ifndef _EXAMPLE_WIFICAST_TX_SLEEP_H_
#define _EXAMPLE_WIFICAST_TX_SLEEP_H_

#include "../wificast_defs.h"
#include "os_wrapper.h"
#include "ameba_pmu.h"

#define SLEEP_DATA_LEN         (1024)  /* data length of each tx, including the frame head */
#define DATA_PERIOD_MS         (50)   /* period of kicking the tx task, ms */
#define WIFI_READY_TIMEOUT_MS  (30000)  /* max time to wait for wifi to start, ms */

#endif /* _EXAMPLE_WIFICAST_TX_SLEEP_H_ */
