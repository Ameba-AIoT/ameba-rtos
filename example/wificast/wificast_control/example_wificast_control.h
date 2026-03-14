#ifndef _EXAMPLE_WIFICAST_CONTROL_H_
#define _EXAMPLE_WIFICAST_CONTROL_H_

#include "../wificast_defs.h"
#include "os_wrapper.h"
#include "PinNames.h"
#include "gpio_api.h"

#define LED_PIN    PA_22

/**
 * @brief Control status.
 */
typedef enum {
	EXAMPLE_CONTROL_INIT = 0,
	EXAMPLE_CONTROL_BIND,
} control_status_t;

#endif /* _EXAMPLE_WIFICAST_CONTROL_H_ */