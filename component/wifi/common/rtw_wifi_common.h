
#ifndef _RTW_WIFI_COMMON_H
#define _RTW_WIFI_COMMON_H
#include "basic_types.h"
#include "rtw_autoconf.h"

#define TAG_WLAN_DRV   "WLAN"
#define TAG_WLAN_COEX  "COEX"
#define TAG_WLAN_INIC  "INIC"
#define TAG_WLAN_NAN   "NAN"

/*
 *	IEEE 802.3 Ethernet magic constants.  The frame sizes omit the preamble
 *	and FCS/CRC (frame check sequence).
 */

#define ETH_ALEN	6		/* Octets in one ethernet addr	 */
#define ETH_HLEN	14		/* Total octets in header.	 */
#define ETH_ZLEN	60		/* Min. octets in frame sans FCS */
#define ETH_DATA_LEN	1500		/* Max. octets in payload	 */
#define ETH_FRAME_LEN	1514		/* Max. octets in frame sans FCS */
#define ETH_TYPE_LEN		2
#define ETH_ARPHRD		1	/* ethernet hardware format */

#define WLAN_HDR_A4_QOS_HTC_LEN	36
#define WLAN_MAX_IV_LEN	8
#define WLAN_SNAP_HEADER	8
#define WLAN_MAX_MIC_LEN	8
#define WLAN_MAX_ICV_LEN	8
#define WLAN_MAX_PROTOCOL_OVERHEAD (WLAN_HDR_A4_QOS_HTC_LEN+WLAN_MAX_IV_LEN+\
							WLAN_SNAP_HEADER+WLAN_MAX_MIC_LEN+WLAN_MAX_ICV_LEN)/*=68*/

#if WIFI_LOGO_CERTIFICATION
#define RSNXE_MAX_LEN (257) /* https://jira.realtek.com/browse/RSWLANDIOT-9803 */
#else
#define RSNXE_MAX_LEN (18)/*Draft P802.11REVmd_D5.0 p1464*/
#endif

#define MAX_WPA_IE_LEN (257)
#define MAX_WPS_IE_LEN (512)
#define MAX_P2P_IE_LEN (256)
#define MAX_WFD_IE_LEN (128)
#define MAX_FT_IE_LEN (256)

enum _IFACE_TYPE {
	IFACE_PORT0, //mapping to port0 for C/D series chips
	IFACE_PORT1, //mapping to port1 for C/D series chip
	IFACE_PORT2,
	MAX_IFACE_PORT,
};

struct eth_drv_sg {
	unsigned int    buf;
	unsigned int     len;
};

#endif
