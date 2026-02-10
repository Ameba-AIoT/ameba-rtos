#ifndef _RTW_WHC_HOST_COMMON_H
#define _RTW_WHC_HOST_COMMON_H

#include <stdint.h>

#define s8                      int8_t
#define u8                      uint8_t
#define s16                     int16_t
#define u16                     uint16_t
#define s32                     int32_t
#define u32                     uint32_t
#define s64                     int64_t
#define u64                     uint64_t

#include "os_wrapper.h"

#include "whc_sdio_host.h"
#include "rtw_sdio_drvio.h"

#include <stdio.h>

#if defined(CONFIG_WHC_CMD_PATH)
#include "whc_host_app.h"
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    (!FALSE)
#endif

typedef unsigned int	        uint;
typedef	signed int		        sint;

#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    (!FALSE)
#endif

#ifndef BIT
#define BIT(__n)       (1U<<(__n))
#endif

struct eth_drv_sg {
	unsigned int    buf;
	unsigned int     len;
};


#ifndef STA_WLAN_INDEX
#define STA_WLAN_INDEX	0
#endif

#ifndef SOFTAP_WLAN_INDEX
#define SOFTAP_WLAN_INDEX	1
#endif

int wifi_is_connected_to_ap(void);
void rtw_psk_wpa_deinit(uint8_t port);
void rtw_wakelock_timeout(uint32_t timeoutms);
void wifi_event_init(void);
void wifi_indication(u32 event, uint8_t *evt_info, s32 evt_len);
void rtw_psk_disconnect_hdl(uint8_t *mac_addr, uint8_t iface_type);

#endif

