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

/**
  * @brief  The enumeration lists the bss types. size u8
  */
enum {
	RTW_BSS_TYPE_INFRASTRUCTURE 	= 0, /**< Denotes infrastructure network                  */
	RTW_BSS_TYPE_WTN_HELPER          		= 1, /**< Denotes an wtn helper network           */
	RTW_BSS_TYPE_UNKNOWN
};

enum {
	RTW_WPS_TYPE_DEFAULT 		    		= 0,	/**< default type */
	RTW_WPS_TYPE_USER_SPECIFIED 		= 1,	/**< user specified type */
	RTW_WPS_TYPE_MACHINE_SPECIFIED   	= 2,	/**< machine specified type */
	RTW_WPS_TYPE_REKEY 			        	= 3,	/**< retry key type */
	RTW_WPS_TYPE_PUSHBUTTON 		    	= 4,	/**< push button type */
	RTW_WPS_TYPE_REGISTRAR_SPECIFIED 	= 5,	/**< register specified type */
	RTW_WPS_TYPE_NONE                   		= 6, 	/**< none */
	RTW_WPS_TYPE_WSC                    		= 7,	/**< wsc type */
};

enum {
	RTW_802_11_BAND_5GHZ   = 0, /**< Denotes 5GHz radio band */
	RTW_802_11_BAND_2_4GHZ = 1,  /**< Denotes 2.4GHz radio band */
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
	uint8_t          			   bss_type;         /**< val: RTW_BSS_TYPE_INFRASTRUCTURE, RTW_BSS_TYPE_WTN_HELPER*/
	uint32_t					       security;         /**< Security type, val: see enum rtw_security*/
	uint8_t        				   wps_type;         /**< val: RTW_SECURITY_OPEN, RTW_SECURITY_WEP_PSK...*/
	unsigned int                      channel;          /**< Radio channel that the AP beacon was received on                          */
	uint8_t					       band;             /**< val: RTW_802_11_BAND_5GHZ, RTW_802_11_BAND_2_4GHZ*/
	char	country_code[2];
	uint8_t		rom_rsvd[4];
} ;
#pragma pack()

#endif
