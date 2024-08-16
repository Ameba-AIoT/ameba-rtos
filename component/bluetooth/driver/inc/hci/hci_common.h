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

/* hci packet type */
#define HCI_CMD                     0x01
#define HCI_ACL                     0x02
#define HCI_SCO                     0x03
#define HCI_EVT                     0x04
#define HCI_ISO                     0x05

/* hci packet header struct */
struct hci_acl_hdr {
	uint16_t handle;
	uint16_t len;
} __attribute__((packed));

struct hci_sco_hdr {
	uint16_t handle;
	uint8_t len;
} __attribute__((packed));

struct hci_iso_hdr {
	uint16_t handle;
	uint16_t len;
} __attribute__((packed));

struct hci_cmd_hdr {
	uint16_t opcode;
	uint8_t  param_len;
} __attribute__((packed));

struct hci_evt_hdr {
	uint8_t  evt;
	uint8_t  len;
} __attribute__((packed));

#define HCI_HDR_MAX_SIZE 4

/****** functions called by hci_process.c ******/
uint8_t hci_downlod_patch_init(void);
uint8_t hci_get_patch_cmd_buf(uint8_t *cmd_buf, uint8_t cmd_len);
uint8_t hci_get_patch_cmd_len(uint8_t *cmd_len);
void hci_downlod_patch_done(void);
void hci_patch_set_chipid(uint8_t chipid);
void hci_set_work_baudrate(uint8_t *baudrate);
void hci_get_baudrate(uint8_t *baudrate, bool use_default_rate);
uint8_t hci_update_uart_baudrate(bool use_default_rate);
/**********************************************/

void hci_set_mp(bool is_mp);
bool hci_check_mp(void);
void set_reg_value(uint32_t reg_address, uint32_t Mask, uint32_t val);
uint8_t hci_get_hdr_len(uint8_t type);
uint16_t hci_get_body_len(const void *hdr, uint8_t type);

/********** APIs called by hci driver *********/
bool hci_controller_enable(void);
void hci_controller_disable(void);
void hci_controller_free(void);
bool hci_controller_is_enabled(void);
/**********************************************/

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
