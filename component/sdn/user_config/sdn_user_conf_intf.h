#ifndef __SDN_USER_CONF_INFT_H__
#define __SDN_USER_CONF_INFT_H__

#include "platform_autoconf.h"

#ifdef CONFIG_BT_INIC
#define SDN_CONF_CLIENT_BT_RX_ACL_NUM                               8
#define SDN_CONF_CLIENT_BT_RX_CMD_NUM                               8
#else
#define SDN_CONF_CLIENT_BT_RX_ACL_NUM                               4
#define SDN_CONF_CLIENT_BT_RX_CMD_NUM                               4
#endif

#define SDN_CONF_CLIENT_154_RX_NUM                                  4

#define SDN_CONF_CLIENT_TX_NUM                                      8

#define SDN_INTF_MAX_DATA_LEN                                       256

#endif