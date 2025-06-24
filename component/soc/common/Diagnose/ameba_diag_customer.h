#ifndef _AMEBA_DIAG_CUSTOMER_H_
#define _AMEBA_DIAG_CUSTOMER_H_
#include "ameba_diag.h"


/**********************************************************************************************
 *                                  Event ID
 *********************************************************************************************/
enum diag_evt_wifi {
	DIAG_EVT_WIFI_DISCONN = RTK_EVENT_TYPE_WIFI_BASE + 0x01,
	DIAG_EVT_WIFI_NO_RX = RTK_EVENT_TYPE_WIFI_BASE + 0x02,
	DIAG_EVT_WIFI_NO_TX = RTK_EVENT_TYPE_WIFI_BASE + 0x03,
};

/**********************************************************************************************
 *                       Definication used by Event detail info
 *********************************************************************************************/
#pragma pack(1)
struct wifi_rxbd_t {
	u8 sw_index : 4;
	u8 hw_index : 4;
}
#pragma pack(0)

/**********************************************************************************************
 *                                 Event detail info
 *********************************************************************************************/
#pragma pack(1)
struct diag_evt_wifi_disconn { /*DIAG_EVT_WIFI_DISCONN*/
	u16 reason;/*enum rtw_disconn_rsn_e*/
}

struct diag_evt_wifi_no_rx { /*DIAG_EVT_WIFI_NO_RX*/
	u8 bb_cca_cck[5];
	u8 bb_cca_ofdm[5];
	u8 bb_rx_cck[5];
	u8 bb_rx_ofdm[5];
	u8 mac_rx_cck[5];
	u8 mac_rx_ofdm[5];
	struct wifi_rxbd_t rxbd[5];
	u8 cca_th;
}
#pragma pack()


#endif /* _AMEBA_DIAG_CUSTOMER_H_ */