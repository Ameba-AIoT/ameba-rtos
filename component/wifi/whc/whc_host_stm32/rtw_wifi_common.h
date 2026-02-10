#ifndef _RTW_WIFI_COMMON_H
#define _RTW_WIFI_COMMON_H
#include <stdint.h>
#include "platform_autoconf.h"
//#include "rtw_autoconf.h"

#define MAX_ETH_DRV_SG    32
#define MAX_ETH_MSG    1540

#define FAIL	(-1)

#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    (!FALSE)
#endif

#ifndef UNUSED
#define UNUSED(X)      (void)X
#endif

typedef unsigned int	        uint;
typedef	signed int		        sint;

#define s8                      int8_t
#define u8                      uint8_t
#define s16                     int16_t
#define u16                     uint16_t
#define s32                     int32_t
#define u32                     uint32_t
#define s64                     int64_t
#define u64                     uint64_t

#define IN
#define OUT
#define INOUT
#define NDIS_OID uint
#define NDIS_STATUS uint


#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    (!FALSE)
#endif

#ifndef BIT
#define BIT(__n)       (1U<<(__n))
#endif

#define CONFIG_ARM_CORE_CM4	1
#define wifi_rom_mutex_create_static	rtos_mutex_create_static

#define RTK_LOGS(tag, format, ... ) printf(format, ##__VA_ARGS__)
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

#define N_BYTE_ALIGMENT(__Value, __Aligment) ((__Aligment == 1) ? (__Value) : (((__Value + __Aligment - 1) / __Aligment) * __Aligment))

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


typedef enum {
	DHCP_START = 0,
	DHCP_WAIT_ADDRESS,
	DHCP_ADDRESS_ASSIGNED,
	DHCP_RELEASE_IP,
	DHCP_STOP,
	DHCP_TIMEOUT
} DHCP_State_TypeDef;

//struct acs_mntr_rpt;
__inline static u16 _htons(u16 n)
{
	return ((n & 0xff) << 8) | ((n & 0xff00) >> 8);
}

/*Config for SKB Size*/
#define SKB_CACHE_SZ	32/*max(AP_Core_Cache, NP_Core_Cache)*/
#define SKB_ALIGNMENT	__attribute__((aligned(SKB_CACHE_SZ)))
#define TXDESC_SIZE	40
#define RXDESC_SIZE	24
#define RXPHYSTS_SIZE 32
#define WLAN_HW_INFO_LEN	56/*max(TXDESC_SIZE, RXDESC_SIZE+RXPHYSTS_SIZE)*/
#ifdef CONFIG_SPEAKER_TEST
#define WLAN_MAX_ETHFRM_LEN	2014/*max payload size of speaker frame*/
#else
#define WLAN_MAX_ETHFRM_LEN	1514/*max payload size of wifi frame*/
#endif


int wifi_is_connected_to_ap(void);
void rtw_psk_wpa_deinit(u8 port);
void rtw_wakelock_timeout(uint32_t timeoutms);
void wifi_event_init(void);
void wifi_indication(u32 event, u8 *evt_info, s32 evt_len);
void rtw_psk_disconnect_hdl(u8 *mac_addr, u8 iface_type);

#ifndef STA_WLAN_INDEX
#define STA_WLAN_INDEX	0
#endif

#ifndef SOFTAP_WLAN_INDEX
#define SOFTAP_WLAN_INDEX	1
#endif

#endif