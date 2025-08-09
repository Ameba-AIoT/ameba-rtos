#ifndef _RTW_CTRL_WIFI_TYPES_H_
#define _RTW_CTRL_WIFI_TYPES_H_


#ifndef u8
#define u8                      uint8_t
#endif
#ifndef u16
#define u16                     uint16_t
#endif
#ifndef u32
#define u32                     uint32_t
#endif

#if 0
#include <wifi_api_types.h>
#endif

#if 1
#define RTW_ESSID_MAX_SIZE  32
struct rtw_ssid {
	unsigned char       len;     /**< SSID length, i.e., equal to the length of `val`. The length of ssid should not > @ref RTW_ESSID_MAX_SIZE.  */
	unsigned char       val[RTW_ESSID_MAX_SIZE + 1]; /**< SSID name (AP name).*/
};

struct rtw_mac {
	unsigned char       octet[6]; /**< Unique 6-byte MAC address. */
};


struct __attribute__((packed)) rtw_scan_result {
	struct rtw_ssid         ssid;             /**< Service Set Identification (i.e. Name of Access Point). */
	struct rtw_mac          bssid;            /**< Basic Service Set Identification (i.e. MAC address of Access Point). */
	signed short            signal_strength;  /**< Receive Signal Strength Indication in dBm. <-90=Very poor, >-30=Excellent. */
	u8
	bss_type;         /**< The bss type. The noraml type is infrastructure BSS. Val: @ref RTW_BSS_TYPE_INFRASTRUCTURE, @ref RTW_BSS_TYPE_WTN_HELPER.*/
	u32                     security;         /**< The security type of this AP. Val: @ref RTW_SECURITY_OPEN, @ref RTW_SECURITY_WEP_PSK...*/
	u8
	wps_type;         /**< The WPS(Wi-Fi Protected Setup) types supported by this AP. Val: @ref RTW_WPS_TYPE_DEFAULT, @ref RTW_WPS_TYPE_USER_SPECIFIED...*/
	unsigned int            channel;          /**< Radio channel that the AP beacon was received on. */
	u8                      band;             /**< The frequency ranges used by this AP. Val: @ref RTW_BAND_ON_5G, @ref RTW_BAND_ON_24G. */
	/** The wireless spectrum management regulations of which region followed by the AP. `country_code` is coded
	 * according to ISO 3166 standard. Specific values can refer to ameba_wifi_country_code_table_usrcfg.c.\n
	 * e.g. China: country_code[0] = 'C', country_code[1] = 'N'. */
	char                    country_code[2];
	char                    wireless_mode;    /**< The wireless mode of this AP. Val: @ref RTW_80211_B, @ref RTW_80211_A...*/
	u8                      rom_rsvd[3];
};

#define BIT(__n)       (1U<<(__n))

enum rtw_security_flag {
	WEP_ENABLED          = BIT(0),     /**< wep enable */
	TKIP_ENABLED         = BIT(1),     /**< tkip enable */
	AES_ENABLED          = BIT(2),     /**< aes enable */
	AES_CMAC_ENABLED     = BIT(4),     /**< aes cmac enable */
	ENTERPRISE_ENABLED   = BIT(5),     /**< enterprise enable */
	OWE_ENABLED          = BIT(6),     /**< owe enable */
	SHARED_ENABLED       = BIT(15),    /**< shared enable */
	WPA_SECURITY         = BIT(21),    /**< wpa */
	WPA2_SECURITY        = BIT(22),    /**< wpa2 */
	WPA3_SECURITY        = BIT(23),    /**< wpa3 */
};

enum rtw_security {
	RTW_SECURITY_OPEN               = 0,                                                            /**< Open security                           */
	RTW_SECURITY_WEP_PSK            = (WEP_ENABLED),                                                /**< WEP Security with open authentication   */
	RTW_SECURITY_WEP_SHARED         = (WEP_ENABLED | SHARED_ENABLED),                               /**< WEP Security with shared authentication */

	RTW_SECURITY_WPA_TKIP_PSK       = (WPA_SECURITY | TKIP_ENABLED),                                /**< WPA Security with TKIP                  */
	RTW_SECURITY_WPA_AES_PSK        = (WPA_SECURITY | AES_ENABLED),                                 /**< WPA Security with AES                   */
	RTW_SECURITY_WPA_MIXED_PSK      = (WPA_SECURITY | AES_ENABLED | TKIP_ENABLED),                  /**< WPA Security with AES & TKIP            */
	RTW_SECURITY_WPA2_TKIP_PSK      = (WPA2_SECURITY | TKIP_ENABLED),                               /**< WPA2 Security with TKIP                 */
	RTW_SECURITY_WPA2_AES_PSK       = (WPA2_SECURITY | AES_ENABLED),                                /**< WPA2 Security with AES                  */
	RTW_SECURITY_WPA2_MIXED_PSK     = (WPA2_SECURITY | AES_ENABLED | TKIP_ENABLED),                 /**< WPA2 Security with AES & TKIP           */
	RTW_SECURITY_WPA_WPA2_TKIP_PSK  = (WPA_SECURITY | WPA2_SECURITY | TKIP_ENABLED),                /**< WPA/WPA2 Security with TKIP             */
	RTW_SECURITY_WPA_WPA2_AES_PSK   = (WPA_SECURITY | WPA2_SECURITY | AES_ENABLED),                 /**< WPA/WPA2 Security with AES              */
	RTW_SECURITY_WPA_WPA2_MIXED_PSK = (WPA_SECURITY  | WPA2_SECURITY | TKIP_ENABLED | AES_ENABLED), /**< WPA/WPA2 Security with AES & TKIP       */
	RTW_SECURITY_WPA3_AES_PSK    = (WPA3_SECURITY | AES_ENABLED),               /**< WPA3-SAE with AES security            */
	RTW_SECURITY_WPA3_OWE    = (WPA3_SECURITY | OWE_ENABLED | AES_ENABLED),             /**< WPA3-OWE with AES security            */
	RTW_SECURITY_WPA2_WPA3_MIXED = (WPA2_SECURITY | WPA3_SECURITY | AES_ENABLED), /**< WPA3-SAE/WPA2 with AES security         */
	RTW_SECURITY_WPA2_AES_CMAC      = (WPA2_SECURITY | AES_CMAC_ENABLED),                           /**< WPA2 Security with AES and Management Frame Protection */
};


enum rtw_wireless_mode {
	RTW_80211_INVALID = 0,
	RTW_80211_B       = BIT(0),
	RTW_80211_A       = BIT(1),
	RTW_80211_G       = BIT(2),
	RTW_80211_N       = BIT(3),
	RTW_80211_AC      = BIT(4),
	RTW_80211_AX      = BIT(5),

	/* Type for current wireless mode */
	RTW_80211_BG      = (RTW_80211_B | RTW_80211_G),
	RTW_80211_GN      = (RTW_80211_G | RTW_80211_N),
	RTW_80211_AN      = (RTW_80211_A | RTW_80211_N),
	RTW_80211_BN      = (RTW_80211_B | RTW_80211_N),
	RTW_80211_BGN     = (RTW_80211_B | RTW_80211_G | RTW_80211_N),
	RTW_80211_BGAX    = (RTW_80211_B | RTW_80211_G | RTW_80211_AX),
	RTW_80211_GAX     = (RTW_80211_G | RTW_80211_AX),
	RTW_80211_A_AC    = (RTW_80211_A | RTW_80211_AC),
	RTW_80211_A_AX    = (RTW_80211_A | RTW_80211_AX),

	/* Capability -Type for registry default wireless mode */
	RTW_80211_AGN     = (RTW_80211_A | RTW_80211_G | RTW_80211_N),
	RTW_80211_ABGN    = (RTW_80211_A | RTW_80211_B | RTW_80211_G | RTW_80211_N),
	RTW_80211_ANAC    = (RTW_80211_A | RTW_80211_N | RTW_80211_AC),
	RTW_80211_24G_MIX = (RTW_80211_B | RTW_80211_G | RTW_80211_N  | RTW_80211_AX),
	RTW_80211_5G_MIX  = (RTW_80211_A | RTW_80211_N | RTW_80211_AC | RTW_80211_AX),
	RTW_80211_MAX     = (RTW_80211_24G_MIX | RTW_80211_5G_MIX),
};


struct whc_rtw_cli_wifi_status {
	u8              ssid[33];
	u8              bssid[6];
	u8              channel;
};

#endif




#endif
