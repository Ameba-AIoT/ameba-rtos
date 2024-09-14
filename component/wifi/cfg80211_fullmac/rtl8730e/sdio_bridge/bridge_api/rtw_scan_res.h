#ifndef __RTW_SCAN_RES_H__
#define __RTW_SCAN_RES_H__

#define WEP_ENABLED		0x0001		/**< wep enable */
#define TKIP_ENABLED		0x0002		/**< tkip enable */
#define AES_ENABLED		0x0004		/**< aes enable */
#define WSEC_SWFLAG		0x0008		/**< WSEC SWFLAG */
#define AES_CMAC_ENABLED	0x0010		/**< aes cmac enable */
#define ENTERPRISE_ENABLED	0x0020		/**< enterprise enable */
#define SHARED_ENABLED		0x00008000	/**< shared enable */
#define WPA_SECURITY		0x00200000	/**< wpa */
#define WPA2_SECURITY		0x00400000	/**< wpa2 */
#define WPA3_SECURITY		0x00800000	/**< wpa3 */
#define WPS_ENABLED		0x10000000	/**< wps  enable*/

enum rtw_bss_type {
	RTW_BSS_TYPE_INFRASTRUCTURE 	= 0, /**< Denotes infrastructure network                  */
	RTW_BSS_TYPE_ADHOC          		= 1, /**< Denotes an 802.11 ad-hoc IBSS network           */
	RTW_BSS_TYPE_ANY            			= 2, /**< Denotes either infrastructure or ad-hoc network */
	RTW_BSS_TYPE_UNKNOWN        		= 0xFFFFFFFF /**< May be returned by scan function if BSS type is unknown. Do not pass this to the Join function */
};

enum rtw_wps_type {
	RTW_WPS_TYPE_DEFAULT 		    		= 0x0000,	/**< default type */
	RTW_WPS_TYPE_USER_SPECIFIED 		= 0x0001,	/**< user specified type */
	RTW_WPS_TYPE_MACHINE_SPECIFIED   	= 0x0002,	/**< machine specified type */
	RTW_WPS_TYPE_REKEY 			        	= 0x0003,	/**< retry key type */
	RTW_WPS_TYPE_PUSHBUTTON 		    	= 0x0004,	/**< push button type */
	RTW_WPS_TYPE_REGISTRAR_SPECIFIED 	= 0x0005,	/**< register specified type */
	RTW_WPS_TYPE_NONE                   		= 0x0006, 	/**< none */
	RTW_WPS_TYPE_WSC                    		= 0x0007,	/**< wsc type */
	RTW_WPS_TYPE_NOUSE					= 0xffffffff		/**< unsed type */
};

enum rtw_802_11_band {
	RTW_802_11_BAND_5GHZ   = 0, /**< Denotes 5GHz radio band */
	RTW_802_11_BAND_2_4GHZ = 1,  /**< Denotes 2.4GHz radio band */
	RTW_BAND_NOUSE			= 0xFFFFFFFF
};

enum rtw_security {
	RTW_SECURITY_OPEN               = 0,                                                            /**< Open security                           */
	RTW_SECURITY_WEP_PSK            = (WEP_ENABLED),                                                /**< WEP Security with open authentication   */
	RTW_SECURITY_WEP_SHARED         = (WEP_ENABLED | SHARED_ENABLED),                               /**< WEP Security with shared authentication */

	RTW_SECURITY_WPA_TKIP_PSK       = (WPA_SECURITY | TKIP_ENABLED),                                /**< WPA Security with TKIP                  */
	RTW_SECURITY_WPA_AES_PSK        = (WPA_SECURITY | AES_ENABLED),                                 /**< WPA Security with AES                   */
	RTW_SECURITY_WPA_MIXED_PSK      = (WPA_SECURITY | AES_ENABLED | TKIP_ENABLED),                  /**< WPA Security with AES & TKIP            */
	RTW_SECURITY_WPA2_TKIP_PSK		= (WPA2_SECURITY | TKIP_ENABLED),								/**< WPA2 Security with TKIP				 */
	RTW_SECURITY_WPA2_AES_PSK       = (WPA2_SECURITY | AES_ENABLED),                                /**< WPA2 Security with AES                  */
	RTW_SECURITY_WPA2_MIXED_PSK     = (WPA2_SECURITY | AES_ENABLED | TKIP_ENABLED),                 /**< WPA2 Security with AES & TKIP           */
	RTW_SECURITY_WPA_WPA2_TKIP_PSK  = (WPA_SECURITY | WPA2_SECURITY | TKIP_ENABLED),                /**< WPA/WPA2 Security with TKIP             */
	RTW_SECURITY_WPA_WPA2_AES_PSK   = (WPA_SECURITY | WPA2_SECURITY | AES_ENABLED),                 /**< WPA/WPA2 Security with AES              */
	RTW_SECURITY_WPA_WPA2_MIXED_PSK = (WPA_SECURITY  | WPA2_SECURITY | TKIP_ENABLED | AES_ENABLED), /**< WPA/WPA2 Security with AES & TKIP       */
	RTW_SECURITY_WPA3_AES_PSK	 = (WPA3_SECURITY | AES_ENABLED),				  /**< WPA3-SAE with AES security			   */
	RTW_SECURITY_WPA2_WPA3_MIXED = (WPA2_SECURITY | WPA3_SECURITY | AES_ENABLED), /**< WPA3-SAE/WPA2 with AES security		   */
	RTW_SECURITY_WPA2_AES_CMAC      = (WPA2_SECURITY | AES_CMAC_ENABLED),                           /**< WPA2 Security with AES and Management Frame Protection */
};

struct _rtw_ssid_t {
	unsigned char		len;     /**< SSID length */
	unsigned char		val[33]; /**< SSID name (AP name)  */
};

struct _rtw_mac_t {
	unsigned char		octet[6]; /**< Unique 6-byte MAC address */
};

#pragma pack(1)
struct rtw_scan_result {
	struct _rtw_ssid_t              SSID;             /**< Service Set Identification (i.e. Name of Access Point)                    */
	struct _rtw_mac_t               BSSID;            /**< Basic Service Set Identification (i.e. MAC address of Access Point)       */
	signed short		                  signal_strength;  /**< Receive Signal Strength Indication in dBm. <-90=Very poor, >-30=Excellent */
	enum rtw_bss_type          bss_type;         /**< Network type                                                              */
	enum rtw_security          security;         /**< Security type                                                             */
	enum rtw_wps_type          wps_type;         /**< WPS type                                                                  */
	unsigned int                      channel;          /**< Radio channel that the AP beacon was received on                          */
	enum rtw_802_11_band       band;             /**< Radio band                                                                */
	char	country_code[2];
	uint8_t		rom_rsvd[4];
} ;
#pragma pack()

#endif
