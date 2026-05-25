#ifndef __BLE_LL_ADDR_H__
#define __BLE_LL_ADDR_H__

#include "string.h"
#include "stdbool.h"

#define BT_ADDR_LE_PUBLIC                   0x00
#define BT_ADDR_LE_RANDOM                   0x01
#define BT_ADDR_LE_PUBLIC_IDENTITY          0x02
#define BT_ADDR_LE_RANDOM_IDENTITY          0x03
#define BT_ADDR_LE_UNRESOLVED               0xFE
#define BT_ADDR_LE_ANONYMOUS                0xFF


#define BT_ADDR_LE_IDENTITY_FLAG            0x02

#define BT_ADDR_SIZE            6


enum  ble_ll_add_type {
	BLE_LL_NON_RESOLVED_PRIVATE_ADDR = 0x0,
	BLE_LL_RESOLVED_PRIVATE_ADDR = 0x1,
	BLE_LL_ADDR_RFU = 0x2,
	BLE_LL_STATIC_ADDR = 0x3,
};

struct bt_le_addr_t {
	uint8_t type;
	uint8_t addr[BT_ADDR_SIZE];
};

#define BLE_LL_SET_ADDR_TYPE(addr, type) \
    ((addr)[5] = (((addr)[5]) & 0x3F) | ((type) << 6))

#define is_bt_static_addr(__addr)  (((*((uint8_t *)(__addr) + 5)) & 0xC0) == 0xC0)
#define is_bt_rpa_addr(__addr)  (((*((uint8_t *)(__addr) + 5)) & 0xC0) == 0x40)
#define is_bt_nrpa_addr(__addr)  (((*((uint8_t *)(__addr) + 5)) & 0xC0) == 0x00)

static inline bool is_same_addr(uint8_t type1, uint8_t type2, uint8_t *addr1, uint8_t *addr2)
{
	return ((type1 == type2) && !memcmp(addr1, addr2, BT_ADDR_SIZE));
}

uint8_t *ble_ll_get_own_id_addr(uint8_t addr_type);
void ble_ll_set_address(uint8_t addr_type, uint8_t *paddr);

#endif