#ifndef __SDN_MEM_CONF_H__
#define __SDN_MEM_CONF_H__

/* Number of hci cmd */
#define SDN_BT_HCI_CMD_RX_NUM                        2

/* Number of hci event
 * When PA sync is enable, 6 is suggested. Otherwise 4 is enough. */
#define SDN_BT_HCI_EVT_RX_NUM                        6

/* TRX packet number for each ble link */
#define BLE_LL_RX_DATA_NUM_PER_LINK                  5
#define BLE_LL_TX_ACL_NUM_PER_LINK                   5
#define BLE_LL_TX_CTRL_NUM_PER_LINK                  3

/* Size of resolving list.
 * Ignored when privacy is disabled */
#define BLE_LL_RESOLVE_LIST_SIZE                     8

/* Extended ADV setting.
 * Ignored when ext ADV is disabled */
#define BLE_LL_MAX_ADV_SET                           3
#define BLE_LL_MAX_ADV_TX_CHAIN_NUM                  4
#define BLE_LL_MAX_ADV_RX_CHAIN_NUM                  4

/* PA sync setting.
 * Ignored when PA Sync is disabled */
#define BLE_LL_MAX_PA_SYNC_SET_NUM                   2
#define BLE_LL_MAX_PA_SYNC_CHAIN_NUM                 7

/* Settings for 15.4 */
#define SDN_CONF_CLIENT_154_TX_NUM                   4
#define SDN_CONF_CLIENT_154_RX_NUM                   4
#define SDN_INTF_MAX_154_LEN                         255

#endif