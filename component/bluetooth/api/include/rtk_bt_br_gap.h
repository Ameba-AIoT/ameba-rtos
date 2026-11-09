/**
 * @file      rtk_bt_br_gap.h
 * @brief     Bluetooth BR/EDR GAP part type and function definition.
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_BR_GAP_H__
#define __RTK_BT_BR_GAP_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_BR_LINK_NUM                 1         /*!< Maximum number of BR/EDR links. */
#define DEFAULT_SUPVISIONTIMEOUT        0x1f40    /*!< Default link supervision timeout. Time = N * 0.625 ms. */
#define RTK_BD_ADDR_LEN 6                         /*!< BR/EDR device address length(bytes). */
#define RTK_BT_GAP_DEVICE_NAME_LEN      (39+1)    /*!< Maximum length of the device name including the terminating NUL. A longer name is truncated. */

/************************************** Data Types *************************************/
/**
 * @typedef   rtk_bt_br_role_t
 * @brief     Bluetooth BR/EDR link role.
 */
typedef enum {
	RTK_BT_BR_GAP_ROLE_MASTER  = 0,    /*!< Local device is the central of the link. */
	RTK_BT_BR_GAP_ROLE_SLAVE,          /*!< Local device is the peripheral of the link. */
} rtk_bt_br_role_t;

/**
 * @typedef   rtk_bt_br_page_scan_t
 * @brief     Bluetooth BR/EDR page scan type.
 */
typedef enum {
	RTK_BT_BR_GAP_PAGE_SCAN_TYPE_STANDARD  = 0,    /*!< Standard page scan. */
	RTK_BT_BR_GAP_PAGE_SCAN_TYPE_INTERLACED,       /*!< Interlaced page scan. */
} rtk_bt_br_page_scan_t;

/**
 * @typedef   rtk_bt_br_inquiry_mode_t
 * @brief     Bluetooth BR/EDR inquiry result format.
 */
typedef enum {
	RTK_BT_BR_GAP_INQUIRY_MODE_STANDARD_RESULT = 0,    /*!< Standard inquiry result format. */
	RTK_BT_BR_GAP_INQUIRY_MODE_RESULT_WITH_RSSI,       /*!< Inquiry result format with RSSI. */
	RTK_BT_BR_GAP_INQUIRY_MODE_EXTENDED_RESULT,        /*!< Inquiry result format with RSSI or extended inquiry result format. */
} rtk_bt_br_inquiry_mode_t;

/**
 * @typedef   rtk_bt_br_io_cap_t
 * @brief     Bluetooth BR/EDR SM IO capability type definition.
 */
typedef enum {
	RTK_BR_IO_CAP_DISPALY_ONLY     = 0x00,    /*!< Display only. */
	RTK_BR_IO_CAP_DISPLAY_YES_NO   = 0x01,    /*!< Display with yes/no input. */
	RTK_BR_IO_CAP_KEYBOARD_ONLY    = 0x02,    /*!< Keyboard only. */
	RTK_BR_IO_CAP_NO_IN_NO_OUT     = 0x03,    /*!< No input and no output. */
	RTK_BR_IO_CAP_KEYBOARD_DISPALY = 0x04,    /*!< Keyboard and display. */
	RTK_BR_IO_CAP_MAX              = 0x05,    /*!< 0x05-0xFF Reserved for future use. */
} rtk_bt_br_io_cap_t;

/**
 * @typedef   rtk_bt_br_gap_radio_mode_t
 * @brief     Bluetooth BR/EDR radio mode.
 */
typedef enum {
	RTK_BT_BR_GAP_RADIO_MODE_VISIBLE_CONNECTABLE  = 0x01,    /*!< Enable both inquiry scan and page scan. */
	RTK_BT_BR_GAP_RADIO_MODE_VISIBLE              = 0x02,    /*!< Enable inquiry scan only. */
	RTK_BT_BR_GAP_RADIO_MODE_CONNECTABLE          = 0x03,    /*!< Enable page scan only. */
	RTK_BT_BR_GAP_RADIO_MODE_NONE_DISCOVERABLE    = 0x04,    /*!< Disable both inquiry scan and page scan. */
} rtk_bt_br_gap_radio_mode_t;

/**
 * @typedef   rtk_bt_bd_addr_t
 * @brief     Bluetooth device address type definition.
 */
typedef struct {
	uint8_t addr[RTK_BD_ADDR_LEN];    /*!< Device address, 6 octets. */
} rtk_bt_bd_addr_t;

/**
 * @struct    T_APP_BR_LINK
 * @brief     Bluetooth BR/EDR link connection database.
 */
typedef struct {
	void                *timer_handle_role_switch;     /*!< Reserved for future use. */
	void                *timer_handle_later_avrcp;     /*!< Reserved for future use. */
	uint8_t             bd_addr[6];                    /*!< Address of the remote device, 6 octets. */
	bool                used;                          /*!< Whether this link entry is in use.
                                                        *   - true: The entry is in use.
                                                        *   - false: The entry is free. */
	bool                acl_conn_ind;                  /*!< Whether the link was created by an incoming ACL connection.
                                                        *   - true: The remote device initiated the ACL connection.
                                                        *   - false: The local device initiated the ACL connection. */
	uint8_t             role;                          /*!< Local role on the link. */
	uint8_t             id;                            /*!< Index of this entry in the link table. */
	void                *a2dp_track_handle;            /*!< Reserved for future use. */
	void                *sco_track_handle;             /*!< Reserved for future use. */
	bool                is_streaming;                  /*!< Reserved for future use */
	bool                reg_play_status_flag;          /*!< Whether the remote device has registered for play status change notification.
                                                        *   - true: The remote device has registered.
                                                        *   - false: The remote device has not registered. */
	uint8_t             avrcp_play_status;             /*!< AVRCP play status of the remote device. */
	uint16_t            handle;                        /*!< Connection handle of the ACL connection. */
	uint16_t            sco_handle;                    /*!< Connection handle of the SCO connection. */
	uint32_t            connected_profile;             /*!< Reserved for future use. */
	uint8_t             a2dp_codec_type;               /*!< A2DP codec type in use. */
	uint8_t             hfp_codec_type;                /*!< HFP codec type in use. */
	uint16_t            remote_hfp_brsf_capability;    /*!< Supported features bitmap reported by the remote HFP audio gateway. */
	bool                is_inband_ring;                /*!< Whether the remote audio gateway supports in-band ring tone.
                                                        *   - true: In-band ring tone is supported.
                                                        *   - false: In-band ring tone is not supported. */
	uint8_t             streaming_fg;                  /*!< Streaming flag.
                                                        *   1: Streaming has started.
                                                        *   0: Streaming has not started. */
	uint16_t            rfc_spp_frame_size;            /*!< SPP TX/RX MTU size. */
	uint8_t             rfc_spp_credit;                /*!< Remaining SPP TX credits. */
	union {
		struct {
			uint8_t     sampling_frequency;            /*!< SBC sampling frequency. */
			uint8_t     channel_mode;                  /*!< SBC channel mode. */
			uint8_t     block_length;                  /*!< SBC block length. */
			uint8_t     subbands;                      /*!< SBC number of subbands. */
			uint8_t     allocation_method;             /*!< SBC allocation method. */
			uint8_t     min_bitpool;                   /*!< SBC minimum bitpool value. */
			uint8_t     max_bitpool;                   /*!< SBC maximum bitpool value. */
		} sbc;                                         /*!< SBC codec information. */
		struct {
			uint8_t     object_type;                   /*!< AAC object type. */
			uint16_t    sampling_frequency;            /*!< AAC sampling frequency. */
			uint8_t     channel_number;                /*!< AAC number of channels. */
			bool        vbr_supported;                 /*!< Whether AAC variable bit rate is supported.
                                                        *   - true: Variable bit rate is supported.
                                                        *   - false: Variable bit rate is not supported. */
			uint32_t    bit_rate;                      /*!< AAC bit rate. */
		} aac;                                         /*!< AAC codec information. */
		struct {
			uint8_t     info[12];                      /*!< Vendor codec information, 12 octets. */
		} vendor;                                      /*!< Vendor codec information. */
	} a2dp_codec_info;                                 /*!< A2DP codec information. The valid member depends on a2dp_codec_type. */
	union {
		struct {
			uint8_t     channel_mode;                  /*!< mSBC channel mode. */
			int         sample_rate;                   /*!< mSBC sample rate. */
			uint8_t     bitpool;                       /*!< mSBC bitpool value. */
			uint8_t     allocation_method;             /*!< mSBC allocation method. */
			uint8_t     subbands;                      /*!< mSBC number of subbands. */
			uint8_t     block_length;                  /*!< mSBC block length. */
		} msbc;                                        /*!< mSBC codec information. */
		struct {
			uint8_t     channel_num;                   /*!< CVSD number of channels. */
			int         sample_rate;                   /*!< CVSD sample rate. */
			uint8_t     frame_duration;                /*!< CVSD frame duration. */
		} cvsd;                                        /*!< CVSD codec information. */
	} hfp_sco_codec_info;                              /*!< HFP SCO codec information. The valid member depends on hfp_codec_type. */
} T_APP_BR_LINK;

/**
 * @struct    T_APP_DB
 * @brief     Bluetooth BR/EDR application database.
 */
typedef struct {
	T_APP_BR_LINK       br_link[MAX_BR_LINK_NUM];    /*!< BR/EDR link table. */
	rtk_bt_bd_addr_t    factory_addr;                /*!< Local factory address. */
} T_APP_DB;

/**
 * @struct    rtk_bt_br_gap_default_param_t
 * @brief     Bluetooth BR/EDR GAP default parameters.
 */
typedef struct {
	uint16_t mtu_size;    /*!< Default MTU size in octets. */
} rtk_bt_br_gap_default_param_t;

/**
 * @struct    rtk_bt_br_bd_addr_t
 * @brief     Bluetooth BR/EDR device address type definition.
 */
typedef struct {
	uint8_t addr[RTK_BD_ADDR_LEN];    /*!< Device address, 6 octets. */
} rtk_bt_br_bd_addr_t;

/**
 * @struct    rtk_bt_br_page_param_t
 * @brief     Bluetooth BR/EDR page parameters.
 */
typedef struct {
	uint8_t     pagescan_type;        /*!< Page scan type.
                                       *   0: Standard page scan.
                                       *   1: Interlaced page scan. */
	uint16_t    pagescan_interval;    /*!< Page scan interval.
                                       *   Range: 0x0012 to 0x1000.
                                       *   Time = N * 0.625 ms.
                                       *   Time Range: 11.25 ms to 2.56 s. */
	uint16_t    pagescan_window;      /*!< Page scan window.
                                       *   Range: 0x0011 to 0x1000.
                                       *   Time = N * 0.625 ms.
                                       *   Time Range: 10.625 ms to 2.56 s. */
	uint16_t    page_timeout;         /*!< Page timeout.
                                       *   Range: 0x0001 to 0xFFFF.
                                       *   Time = N * 0.625 ms.
                                       *   Time Range: 0.625 ms to 40.96 s. */
} rtk_bt_br_page_param_t;

/**
 * @struct    rtk_bt_br_inquiry_param_t
 * @brief     Bluetooth BR/EDR inquiry parameters.
 */
typedef struct {
	uint8_t     inquiry_mode;            /*!< Inquiry result format.
                                          *   0: Standard result.
                                          *   1: Result with RSSI.
                                          *   2: Result with RSSI or extended inquiry result. */
	uint8_t     inquiryscan_type;        /*!< Inquiry scan type.
                                          *   0: Standard inquiry scan.
                                          *   1: Interlaced inquiry scan. */
	uint16_t    inquiryscan_window;      /*!< Inquiry scan window.
                                          *   Range: 0x0011 to 0x1000.
                                          *   Time = N * 0.625 ms.
                                          *   Time Range: 10.625 ms to 2.56 s. */
	uint16_t    inquiryscan_interval;    /*!< Inquiry scan interval.
                                          *   Range: 0x0012 to 0x1000.
                                          *   Time = N * 0.625 ms.
                                          *   Time Range: 11.25 ms to 2.56 s. */
} rtk_bt_br_inquiry_param_t;

/**
 * @struct    rtk_bt_br_inquiry_start_t
 * @brief     Bluetooth BR/EDR inquiry start parameters.
 */
typedef struct {
	bool        limited_inquiry;    /*!< Whether to perform a limited inquiry.
                                     *   - true: Limited inquiry.
                                     *   - false: General inquiry. */
	uint8_t     timeout;            /*!< Inquiry duration.
                                     *   Range: 0x01 to 0x30.
                                     *   Time = N * 1.28 s.
                                     *   Time Range: 1.28 s to 61.44 s. */
} rtk_bt_br_inquiry_start_t;

/**
 * @struct    rtk_bt_br_auto_sniff_mode_t
 * @brief     Bluetooth BR/EDR auto sniff mode parameters.
 */
typedef struct {
	uint8_t     bd_addr[6];    /*!< Address of the remote device, 6 octets. */
	bool        enable;        /*!< Whether to enter sniff mode automatically.
                                *   - true: Enable auto sniff mode.
                                *   - false: Disable auto sniff mode. */
} rtk_bt_br_auto_sniff_mode_t;

/**
 * @struct    rtk_bt_br_sniff_mode_t
 * @brief     Bluetooth BR/EDR sniff mode parameters.
 */
typedef struct {
	uint8_t     enter;            /*!< Sniff mode action.
                                   *   0: Exit sniff mode.
                                   *   1: Enter sniff mode. */
	uint8_t     bd_addr[6];       /*!< Address of the remote device, 6 octets. */
	uint16_t    min_interval;     /*!< Minimum sniff interval. Only even values are valid.
                                   *   Range: 0x0002 to 0xFFFE.
                                   *   Time = N * 0.625 ms.
                                   *   Time Range: 1.25 ms to 40.9 s. */
	uint16_t    max_interval;     /*!< Maximum sniff interval. Only even values are valid.
                                   *   Range: 0x0002 to 0xFFFE.
                                   *   Time = N * 0.625 ms.
                                   *   Time Range: 1.25 ms to 40.9 s.
                                   *   Must be larger than min_interval. */
	uint16_t    sniff_attempt;    /*!< Number of baseband receive slots for sniff attempt.
                                   *   Range: 0x0001 to 0x7FFF. */
	uint16_t    sniff_timeout;    /*!< Number of baseband receive slots for sniff timeout.
                                   *   Range: 0x0000 to 0x7FFF. */
} rtk_bt_br_sniff_mode_t;

/**
 * @typedef   rtk_bt_br_qos_type_t
 * @brief     Bluetooth BR/EDR link QoS service type.
 */
typedef enum {
	RTK_BT_BR_QOS_TYPE_NO_TRAFFIC    = 0x00,    /*!< No traffic. */
	RTK_BT_BR_QOS_TYPE_BEST_EFFORT   = 0x01,    /*!< Best effort. */
	RTK_BT_BR_QOS_TYPE_GUARANTEED    = 0x02,    /*!< Guaranteed. */
} rtk_bt_br_qos_type_t;

/**
 * @struct    rtk_bt_br_link_qos_t
 * @brief     Bluetooth BR/EDR link QoS parameters.
 */
typedef struct {
	uint8_t              bd_addr[6];    /*!< Address of the remote device, 6 octets. */
	rtk_bt_br_qos_type_t type;          /*!< QoS service type. */
	uint16_t             tpoll;         /*!< Poll interval.
                                         *   Range: 0x0006 to 0x1000.
                                         *   Time = N * 0.625 ms.
                                         *   Time Range: 3.75 ms to 2.56 s. */
} rtk_bt_br_link_qos_t;

/**
 * @struct    rtk_bt_br_security_param_t
 * @brief     Bluetooth BR/EDR SM security parameter type definition.
 */
typedef struct {
	rtk_bt_br_io_cap_t io_cap;    /*!< IO capability of the local device. */
	uint8_t oob_data_flag;        /*!< Out of band data flag.
                                   *   1: OOB data is present.
                                   *   0: OOB data is not present. */
	uint8_t bond_flag;            /*!< Bonding flag.
                                   *   1: Bonding is enabled.
                                   *   0: Bonding is disabled. */
	uint8_t br_bond_flag;         /*!< BR/EDR general bonding flag.
                                   *   1: General bonding is supported.
                                   *   0: General bonding is not supported. */
	uint8_t mitm_flag;            /*!< Man-in-the-middle protection flag.
                                   *   1: MITM protection is required.
                                   *   0: MITM protection is not required. */
	uint8_t sec_pair_flag;        /*!< Secure Connections pairing flag.
                                   *   1: Secure Connections pairing is supported.
                                   *   0: Secure Connections pairing is not supported. */
	uint8_t br_sec_pair_flag;     /*!< BR/EDR Secure Connections pairing flag.
                                   *   1: BR/EDR Secure Connections pairing is supported.
                                   *   0: BR/EDR Secure Connections pairing is not supported. */
} rtk_bt_br_security_param_t;

/**
 * @struct    rtk_bt_br_acl_sniff_t
 * @brief     Bluetooth BR/EDR ACL sniff mode information.
 */
typedef struct {
	uint8_t     bd_addr[6];    /*!< Address of the remote device, 6 octets. */
	uint16_t    interval;      /*!< Sniff interval.
                                *   Time = N * 0.625 ms. */
} rtk_bt_br_acl_sniff_t;

/**
 * @struct    rtk_bt_br_acl_disc_t
 * @brief     Bluetooth BR/EDR ACL disconnection information.
 */
typedef struct {
	uint8_t     bd_addr[6];    /*!< Address of the remote device, 6 octets. */
	uint16_t    cause;         /*!< Disconnection reason code. */
} rtk_bt_br_acl_disc_t;

/**
 * @struct    rtk_bt_br_link_key_req_t
 * @brief     Bluetooth BR/EDR link key request information.
 */
typedef struct {
	uint8_t     found;         /*!< Whether a link key exists for the remote device.
                                *   1: The link key is found.
                                *   0: The link key is not found. */
	uint8_t     bd_addr[6];    /*!< Address of the remote device, 6 octets. */
} rtk_bt_br_link_key_req_t;

/**
 * @struct    rtk_bt_br_bond_key_t
 * @brief     Bluetooth BR/EDR bond key.
 */
typedef struct {
	uint8_t     bd_addr[6];      /*!< Address of the remote device, 6 octets. */
	uint8_t     link_key[16];    /*!< Link key, 16 octets. */
	uint8_t     key_type;        /*!< Link key type. */
} rtk_bt_br_bond_key_t;

/**
 * @struct    rtk_bt_br_bond_addr_t
 * @brief     Bluetooth BR/EDR bonded device address query.
 */
typedef struct {
	uint8_t     bd_addr[6];    /*!< Address of the bonded device, 6 octets. */
	uint8_t     priority;      /*!< Priority of the bonded device.
                                *   Bond priority number starts at 1, rather than 0.
                                *   Bond priority is descending with the corresponding number increasing. */
} rtk_bt_br_bond_addr_t;

/**
 * @struct    rtk_bt_br_bond_index_t
 * @brief     Bluetooth BR/EDR bond index query.
 */
typedef struct {
	uint8_t     bd_addr[6];    /*!< Address of the bonded device, 6 octets. */
	uint8_t     index;         /*!< Bond index. Starts at 0. */
} rtk_bt_br_bond_index_t;

/**
 * @struct    rtk_bt_br_inquiry_result_t
 * @brief     Bluetooth BR/EDR inquiry result.
 */
typedef struct {
	uint8_t     bd_addr[6];                          /*!< Address of the inquired device, 6 octets. */
	uint32_t    cod;                                 /*!< Class of Device of the inquired device. */
	int8_t      rssi;                                /*!< RSSI of the inquiry response.
                                                      *   Range: -127 to +20 dBm.
                                                      *   0x7F: RSSI information is unavailable. */
	uint8_t     name[RTK_BT_GAP_DEVICE_NAME_LEN];    /*!< Device name of the inquired device. */
	uint8_t     eir_len;                             /*!< Length of the extended inquiry response data in octets. */
	uint8_t     *p_eir;                              /*!< Extended inquiry response data.
                                                      *   NULL: No extended inquiry response data is present. */
} rtk_bt_br_inquiry_result_t;

/**
 * @struct    rtk_bt_br_inquiry_cmpl_t
 * @brief     Bluetooth BR/EDR inquiry completion information.
 */
typedef struct {
	uint16_t cause;    /*!< Result code of the inquiry; 0x00 on success. */
} rtk_bt_br_inquiry_cmpl_t;

/**
 * @struct    rtk_bt_br_remote_name_rsp_t
 * @brief     Bluetooth BR/EDR remote device name response.
 */
typedef struct {
	uint8_t     bd_addr[6];                          /*!< Address of the remote device, 6 octets. */
	uint16_t    cause;                               /*!< Result code of the remote name request; 0x00 on success. */
	uint8_t     name[RTK_BT_GAP_DEVICE_NAME_LEN];    /*!< Device name of the remote device. */
} rtk_bt_br_remote_name_rsp_t;

/**
 * @struct    rtk_bt_br_acl_conn_fail_t
 * @brief     Bluetooth BR/EDR ACL connection failure information.
 */
typedef struct {
	uint8_t     bd_addr[6];    /*!< Address of the remote device, 6 octets. */
	uint16_t    cause;         /*!< Failure reason code. */
} rtk_bt_br_acl_conn_fail_t;

/**
 * @struct    rtk_bt_br_link_read_rssi_rsp
 * @brief     Bluetooth BR/EDR link RSSI read response.
 */
typedef struct {
	uint8_t     bd_addr[6];    /*!< Address of the remote device, 6 octets. */
	uint16_t    cause;         /*!< Result code of the RSSI read; 0x00 on success. */
	int8_t      rssi;          /*!< RSSI of the link, valid only when the read succeeds.
                                *   Range: -127 to +20 dBm.
                                *   0x7F: RSSI information is unavailable. */
} rtk_bt_br_link_read_rssi_rsp;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  bt_br_gap BT BR GAP APIs
 * @brief     BT BR GAP related function APIs.
 * @ingroup   BT_APIs
 * @{
 */

/**
 * @brief     Set the BR/EDR default parameters.
 * @param[in] param: Default parameters.
 *                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API shall be called before @ref rtk_bt_enable.
 */
uint16_t rtk_bt_br_gap_set_default_param(rtk_bt_br_gap_default_param_t *param);

/**
 * @brief     Get the address of the local device.
 * @param[out] paddr: Address of the local device.
 *                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_br_gap_get_bd_addr(rtk_bt_br_bd_addr_t *paddr);

/**
 * @brief     Set the Bluetooth device name.
 * @param[in] name: Device name string.
 *                  Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_br_gap_set_device_name(const uint8_t *name);

/**
 * @brief     Set the initial page parameters.
 * @param[in] page_param: Page parameters.
 *                        Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API shall be called before @ref rtk_bt_enable.
 */
uint16_t rtk_bt_br_gap_set_page_param(rtk_bt_br_page_param_t *page_param);

/**
 * @brief     Configure the page scan parameters after BT is enabled.
 * @param[in] type: Page scan type.
 *                  0: Standard page scan.
 *                  1: Interlaced page scan.
 * @param[in] interval: Page scan interval.
 *                      Range: 0x0012 to 0x1000.
 *                      Time = N * 0.625 ms.
 *                      Time Range: 11.25 ms to 2.56 s.
 * @param[in] window: Page scan window.
 *                    Range: 0x0011 to 0x1000.
 *                    Time = N * 0.625 ms.
 *                    Time Range: 10.625 ms to 2.56 s.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_br_gap_cfg_page_param(uint8_t type, uint16_t interval, uint16_t window);

/**
 * @brief     Set the initial inquiry parameters.
 * @param[in] inquiry_param: Inquiry parameters.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API shall be called before @ref rtk_bt_enable.
 */
uint16_t rtk_bt_br_gap_set_inquiry_param(rtk_bt_br_inquiry_param_t *inquiry_param);

/**
 * @brief     Configure the inquiry scan parameters after BT is enabled.
 * @param[in] type: Inquiry scan type.
 *                  0: Standard inquiry scan.
 *                  1: Interlaced inquiry scan.
 * @param[in] interval: Inquiry scan interval.
 *                      Range: 0x0012 to 0x1000.
 *                      Time = N * 0.625 ms.
 *                      Time Range: 11.25 ms to 2.56 s.
 * @param[in] window: Inquiry scan window.
 *                    Range: 0x0011 to 0x1000.
 *                    Time = N * 0.625 ms.
 *                    Time Range: 10.625 ms to 2.56 s.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_br_gap_cfg_inquiry_param(uint8_t type, uint16_t interval, uint16_t window);

/**
 * @brief     Set the BR/EDR security parameters.
 * @param[in] p_sec_param: Security parameters.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_br_sm_set_security_param(rtk_bt_br_security_param_t *p_sec_param);

/**
 * @brief     Disconnect the BR/EDR ACL connection with a remote device.
 *            The result is reported asynchronously via event @ref RTK_BT_BR_GAP_ACL_DISCONN.
 * @param[in] bd_addr: Address of the remote device, 6 octets.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_br_gap_disconnect(uint8_t *bd_addr);

/**
 * @brief     Start a BR/EDR inquiry to discover nearby devices. Each device found is reported asynchronously via event @ref RTK_BT_BR_GAP_INQUIRY_RESULT.
 *            The end of the inquiry is reported via event @ref RTK_BT_BR_GAP_INQUIRY_CMPL.
 * @param[in] inquiry_start_param: Inquiry start parameters.
 *                                 Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_br_gap_start_inquiry(rtk_bt_br_inquiry_start_t *inquiry_start_param);

/**
 * @brief     Get the device name of a remote device. The result is reported asynchronously via event @ref RTK_BT_BR_GAP_REMOTE_NAME_RSP.
 * @param[in] bd_addr: Address of the remote device, 6 octets.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_br_gap_get_remote_name(uint8_t *bd_addr);

/**
 * @brief     Get the maximum number of devices that can be bonded.
 * @param[out] pmax_bond_num: Maximum number of bonded devices.
 *                            Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_br_gap_max_bond_num_get(uint8_t *pmax_bond_num);

/**
 * @brief     Get the number of devices currently bonded.
 * @param[out] pbond_num: Number of bonded devices.
 *                        Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_br_gap_bond_num_get(uint8_t *pbond_num);

/**
 * @brief     Get the address of a bonded device by its bond priority.
 * @param[in] priority: Bond priority of the device.
 *                      Range: 1 to the maximum number of bonded devices.
 * @param[out] bd_addr: Address of the bonded device, 6 octets.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Bond priority number starts at 1, rather than 0.
 */
uint16_t rtk_bt_br_gap_bond_addr_get(uint8_t priority, uint8_t *bd_addr);

/**
 * @brief     Get the bond index of a bonded device from its address.
 * @param[out] pindex: Bond index of the device.
 *                     Bond index starts at 0.
 *                     Must not be NULL.
 * @param[in] bd_addr: Address of the bonded device, 6 octets.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_br_gap_bond_index_get(uint8_t *pindex, uint8_t *bd_addr);

/**
 * @brief     Get the link key of a bonded device from the bond storage.
 * @param[in] bd_addr: Address of the bonded device, 6 octets.
 *                     Must not be NULL.
 * @param[out] key_type: Type of the link key.
 *                       Must not be NULL.
 * @param[out] bond_key: Link key, 16 octets.
 *                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_br_gap_bond_key_get(uint8_t *bd_addr, uint8_t *key_type, uint8_t *bond_key);

/**
 * @brief     Save a link key into the bond storage.
 * @param[in] pbond_key_t: Bond key to save.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_br_gap_bond_key_set(rtk_bt_br_bond_key_t *pbond_key_t);

/**
 * @brief     Raise the bond priority of a bonded device to the highest in the bond storage.
 * @param[in] bd_addr: Address of the bonded device, 6 octets.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_br_gap_bond_priority_set(uint8_t *bd_addr);

/**
 * @brief     Delete the bond information of a device from the bond storage.
 * @param[in] bd_addr: Address of the bonded device, 6 octets.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_br_gap_bond_delete(uint8_t *bd_addr);

/**
 * @brief     Delete the bond information of all devices from the bond storage.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_br_gap_bond_clear(void);

/**
 * @brief     Set the pin code used for legacy pairing.
 * @param[in] pin_code: Pin code.
 *                      Must not be NULL.
 * @param[in] length: Length of the pin code in octets.
 *                    Maximum: 4.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_br_gap_set_pincode(uint8_t *pin_code, uint32_t length);

/**
 * @brief     Set the BR/EDR radio mode.
 * @param[in] radio_mode: Radio mode.
 *                        Range: 0x01 to 0x04.
 *                        The values are defined by @ref rtk_bt_br_gap_radio_mode_t.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_br_gap_set_radio_mode(uint8_t radio_mode);

/**
 * @brief     Configure whether the link with a remote device enters sniff mode automatically.
 * @param[in] bd_addr: Address of the remote device, 6 octets.
 *                     Must not be NULL.
 * @param[in] enable: Whether to enter sniff mode automatically.
 *                    - true: Enable auto sniff mode.
 *                    - false: Disable auto sniff mode.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_br_gap_set_auto_sniff_mode(uint8_t *bd_addr, bool enable);

/**
 * @brief     Enter or exit sniff mode on the link with a remote device.
 * @param[in] enter: Sniff mode action.
 *                   0: Exit sniff mode.
 *                   1: Enter sniff mode.
 * @param[in] bd_addr: Address of the remote device, 6 octets.
 *                     Must not be NULL.
 * @param[in] min_interval: Minimum sniff interval. Only used when entering sniff mode.
 *                          Only even values are valid.
 *                          Range: 0x0002 to 0xFFFE.
 *                          Time = N * 0.625 ms.
 *                          Time Range: 1.25 ms to 40.9 s.
 * @param[in] max_interval: Maximum sniff interval. Only used when entering sniff mode.
 *                          Only even values are valid.
 *                          Range: 0x0002 to 0xFFFE.
 *                          Time = N * 0.625 ms.
 *                          Time Range: 1.25 ms to 40.9 s.
 *                          Must be larger than min_interval.
 * @param[in] sniff_attempt: Number of baseband receive slots for sniff attempt. Only used when entering sniff mode.
 *                           Range: 0x0001 to 0x7FFF.
 * @param[in] sniff_timeout: Number of baseband receive slots for sniff timeout. Only used when entering sniff mode.
 *                           Range: 0x0000 to 0x7FFF.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_br_gap_set_sniff_mode(uint8_t enter, uint8_t *bd_addr, uint16_t min_interval, uint16_t max_interval, uint16_t sniff_attempt,
									  uint16_t sniff_timeout);

/**
 * @brief     Set the QoS of the link with a remote device.
 * @param[in] bd_addr: Address of the remote device, 6 octets.
 *                     Must not be NULL.
 * @param[in] type: QoS service type.
 * @param[in] tpoll: Poll interval.
 *                   Range: 0x0006 to 0x1000.
 *                   Time = N * 0.625 ms.
 *                   Time Range: 3.75 ms to 2.56 s.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_br_gap_set_link_qos(uint8_t *bd_addr, rtk_bt_br_qos_type_t type, uint16_t tpoll);

/**
 * @brief     Set the Class of Device of the local device.
 * @param[in] bt_cod: Class of Device value.
 *                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API shall be called before @ref rtk_bt_enable.
 */
uint16_t rtk_bt_br_gap_set_cod(uint32_t *bt_cod);

/**
 * @brief     Set the link supervision timeout.
 * @param[in] supv_timeout: Link supervision timeout.
 *                          Range: 0x0001 to 0xFFFF.
 *                          Time = N * 0.625 ms.
 *                          Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API shall be called before @ref rtk_bt_enable.
 */
uint16_t rtk_bt_br_gap_set_supvisiontimeout(uint16_t *supv_timeout);

/**
 * @brief     Read the RSSI of the link with a remote device. The result is reported asynchronously via event @ref RTK_BT_BR_GAP_LINK_RSSI_INFO.
 * @param[in] bd_addr: Address of the remote device, 6 octets.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_br_gap_read_rssi(uint8_t *bd_addr);
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_BR_GAP_H__ */
