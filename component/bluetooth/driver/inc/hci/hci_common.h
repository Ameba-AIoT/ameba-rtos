/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef _HCI_COMMON_H_
#define _HCI_COMMON_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <platform_autoconf.h>

#define HCI_FAIL      0
#define HCI_SUCCESS   1
#define HCI_IGNORE    2

uint8_t hci_downlod_patch_init(void);
uint8_t hci_get_patch_cmd_buf(uint8_t *cmd_buf, uint8_t cmd_len);
uint8_t hci_get_patch_cmd_len(uint8_t *cmd_len);
void hci_downlod_patch_done(void);
void hci_patch_set_chipid(uint8_t chipid);

void hci_set_mp(bool is_mp);
bool hci_check_mp(void);

void hci_set_work_baudrate(uint8_t *baudrate);
void hci_get_baudrate(uint8_t *baudrate, bool use_default_rate);
uint8_t hci_update_uart_baudrate(bool use_default_rate);

void set_reg_value(uint32_t reg_address, uint32_t Mask, uint32_t val);

bool hci_controller_enable(void);
void hci_controller_disable(void);
void hci_controller_free(void);
bool hci_controller_is_enabled(void);

#if defined(CONFIG_MP_INCLUDED) && CONFIG_MP_INCLUDED
#if defined(CONFIG_MP_SHRINK) && CONFIG_MP_SHRINK
#define hci_is_mp_mode() true
#else
#define hci_is_mp_mode hci_check_mp
#endif
#else /* CONFIG_MP_INCLUDED */
#define hci_is_mp_mode() false
#endif /* CONFIG_MP_INCLUDED */

#endif /* _HCI_COMMON_H_ */
