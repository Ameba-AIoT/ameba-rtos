#ifndef __BT_INIC_DEFS_H__
#define __BT_INIC_DEFS_H__

/* BT ID base should bigger than WHC_WIFI_EVT_MAX. */
#define WHC_BT_ID_BASE 0xa5a5a5b0
enum INIC_BT_ID_TYPE {
	INIC_BT_HOST_TX = WHC_BT_ID_BASE,
	INIC_BT_HOST_RX,

	INIC_BT_ID_MAX
};

#define HCI_SPI_PKT_HDR_LEN             8

#endif /* __BT_INIC_DEFS_H__ */