/*
 *******************************************************************************
 * Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef __LLS_SERVICE_H__
#define __LLS_SERVICE_H__

#ifdef __cplusplus
extern "C"  {
#endif

#include <rtk_bt_def.h>

/* Alert Level characteristic values (Link Loss Service spec v1.0.1, section 3.1).
 * Only the following 3 levels are defined; 0x03~0xFF are Reserved for Future Use. */
typedef enum {
	LLS_ALERT_LEVEL_NO_ALERT   = 0x00,   /* No Alert */
	LLS_ALERT_LEVEL_MILD_ALERT = 0x01,   /* Mild Alert */
	LLS_ALERT_LEVEL_HIGH_ALERT = 0x02,   /* High Alert */
} rtk_lls_alert_level_t;

void link_loss_srv_callback(uint8_t event, void *data);

uint16_t link_loss_srv_add(void);

uint8_t link_loss_srv_get_alert_level(void);

#ifdef __cplusplus
}
#endif

#endif  /* __LLS_SERVICE_H__ */
