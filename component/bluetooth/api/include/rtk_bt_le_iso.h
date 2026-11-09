/**
 * @file      rtk_bt_le_iso.h
 * @author    atonio_liu@realsil.com.cn
 * @brief     Bluetooth LE ISO type and function definition.
 * @copyright Copyright (c) 2025. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_LE_ISO_H__
#define __RTK_BT_LE_ISO_H__

#include <rtk_bt_def.h>
#include <basic_types.h>
#include <bt_api_config.h>
#include <rtk_bt_le_gap.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT
#include <rtk_bt_le_iso_def.h>
#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

/**
 * @typedef   rtk_bt_le_iso_role_t
 * @brief     Bluetooth LE ISO role definition.
 */
typedef enum {
	RTK_BLE_ISO_ROLE_UNKNOWN,            /*!< Unknown role. */
	RTK_BLE_ISO_ROLE_CIS_INITIATOR,      /*!< CIS - central role. */
	RTK_BLE_ISO_ROLE_CIS_ACCEPTOR,       /*!< CIS - peripheral role. */
	RTK_BLE_ISO_ROLE_BIS_BROADCASTER,    /*!< BIS - broadcaster role. */
	RTK_BLE_ISO_ROLE_BIS_RECEIVER,       /*!< BIS - receiver role. */
} rtk_bt_le_iso_role_t;

/**
 * @typedef   rtk_bt_le_iso_data_path_direction_t
 * @brief     Bluetooth LE ISO data path direction definition used when setting up a data path.
 */
typedef enum {
	RTK_BLE_ISO_DATA_PATH_ADD_INPUT  = 0x00,    /*!< Input (Host to Controller). */
	RTK_BLE_ISO_DATA_PATH_ADD_OUTPUT,           /*!< Output (Controller to Host). */
} rtk_bt_le_iso_data_path_direction_t;

/**
 * @typedef   rtk_bt_le_iso_data_path_direction_flag_t
 * @brief     Bluetooth LE ISO data path direction flags. Specify which directions have been configured, or combine the flags to specify which directions
 *            are to have the data path removed.
 */
typedef enum {
	RTK_BLE_ISO_DATA_PATH_INPUT_FLAG  = 0x01,    /*!< Input (Host to Controller). Remove input data path. */
	RTK_BLE_ISO_DATA_PATH_OUTPUT_FLAG,           /*!< Output (Controller to Host). Remove output data path. */
} rtk_bt_le_iso_data_path_direction_flag_t;

/**
 * @struct    rtk_bt_le_iso_setup_path_param_t
 * @brief     Bluetooth LE ISO data path setup parameters.
 */
typedef struct {
	uint16_t iso_conn_handle;                                   /*!< Connection handle of the ISO (CIS or BIS).
                                                                 *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_le_iso_data_path_direction_t data_path_direction;    /*!< Direction for which the data path is being configured.
                                                                 *   A BIS broadcaster uses @ref RTK_BLE_ISO_DATA_PATH_ADD_INPUT.
                                                                 *   A BIS receiver uses @ref RTK_BLE_ISO_DATA_PATH_ADD_OUTPUT. */
	uint8_t data_path_id;                                       /*!< Data transport path used.
                                                                 *   0x00: HCI.
                                                                 *   0x01 to 0xFE: Logical channel number, whose meaning is vendor-specific. */
	uint8_t codec_id[5];                                        /*!< Coding format used over the air.
                                                                 *   codec_id[0]: Coding format.
                                                                 *   codec_id[1], codec_id[2]: Company ID.
                                                                 *   codec_id[3], codec_id[4]: Vendor-defined codec ID.
                                                                 *   The company ID and vendor-defined codec ID are ignored if codec_id[0] is not 0xFF. */
	uint32_t controller_delay;                                  /*!< Controller delay in microseconds.
                                                                 *   Range: 0x000000 to 0x3D0900.
                                                                 *   Time Range: 0 s to 4 s. */
	uint8_t codec_config_len;                                   /*!< Length of the codec configuration in octets. */
	uint8_t *p_codec_config;                                    /*!< Pointer to the codec-specific configuration for the specified direction. */
} rtk_bt_le_iso_setup_path_param_t;

/**
 * @struct    rtk_bt_le_iso_remove_path_param_t
 * @brief     Bluetooth LE ISO data path removal parameters.
 */
typedef struct {
	uint16_t iso_conn_handle;                                        /*!< Connection handle of the ISO (CIS or BIS).
                                                                      *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_le_iso_data_path_direction_flag_t data_path_direction;    /*!< Directions for which the data path is to be removed.
                                                                      *   A BIS broadcaster uses @ref RTK_BLE_ISO_DATA_PATH_INPUT_FLAG.
                                                                      *   A BIS receiver uses @ref RTK_BLE_ISO_DATA_PATH_OUTPUT_FLAG. */
} rtk_bt_le_iso_remove_path_param_t;

/**
 * @struct    rtk_bt_le_iso_data_send_info_t
 * @brief     Bluetooth LE ISO data send information.
 */
typedef struct {
	uint16_t iso_conn_handle;    /*!< Connection handle of the ISO.
                                  *   Range: 0x0000 to 0x0EFF. */
	bool ts_flag;                /*!< Whether the time stamp is contained.
                                  *   - true: The time stamp is contained.
                                  *   - false: The time stamp is not contained. */
	uint32_t time_stamp;         /*!< Time stamp of the SDU, in microseconds.
                                  *   Used only when @ref ts_flag is true. */
	uint16_t pkt_seq_num;        /*!< Sequence number of the SDU. */
	uint16_t data_len;           /*!< Length of the SDU to be sent, in octets. */
	uint8_t *p_data;             /*!< Pointer to the data to be sent. */
} rtk_bt_le_iso_data_send_info_t;

/**
 * @typedef   rtk_bt_le_iso_isoch_data_pkt_status_t
 * @brief     Bluetooth LE ISO Packet status flag of a received ISO data packet reported by the Controller.
 */
typedef enum {
	RTK_BLE_ISO_ISOCH_DATA_PKT_STATUS_VALID_DATA = 0,             /*!< Valid data. The complete SDU was received correctly. */
	RTK_BLE_ISO_ISOCH_DATA_PKT_STATUS_POSSIBLE_ERROR_DATA = 1,    /*!< Possibly invalid data. The contents of the SDU may contain errors or part of the SDU
                                                                   *   may be missing. */
	RTK_BLE_ISO_ISOCH_DATA_PKT_STATUS_LOST_DATA = 2,              /*!< Lost data. Part(s) of the SDU were not received correctly. */
} rtk_bt_le_iso_isoch_data_pkt_status_t;

/**
 * @struct    rtk_bt_le_iso_direct_iso_data_ind_t
 * @brief     Bluetooth LE ISO Indication of a received ISO data packet.
 */
typedef struct {
	uint16_t iso_conn_handle;                                 /*!< Connection handle of the ISO.
                                                               *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_le_iso_isoch_data_pkt_status_t pkt_status_flag;    /*!< Packet status of the received SDU. */
	uint16_t  offset;                                         /*!< Offset from the start of @ref p_buf to the start of the SDU.
                                                               *   e.g. p_buf + offset indicates the start of the SDU. */
	uint16_t  iso_sdu_len;                                    /*!< Length of the SDU in octets. */
	uint16_t  pkt_seq_num;                                    /*!< Sequence number of the SDU. */
	bool      ts_flag;                                        /*!< Whether the time stamp is contained.
                                                               *   - true: The time stamp is contained.
                                                               *   - false: The time stamp is not contained. */
	uint32_t  time_stamp;                                     /*!< Time stamp of the SDU, in microseconds.
                                                               *   Valid when @ref ts_flag is true. */
	uint16_t  buf_len;                                        /*!< Length of the buffer pointed to by @ref p_buf, in octets. */
	uint8_t   *p_buf;                                         /*!< Pointer to the buffer holding the received data. */
} rtk_bt_le_iso_direct_iso_data_ind_t;

/**
 * @struct    rtk_bt_le_iso_data_send_done_t
 * @brief     Bluetooth LE ISO data send done information.
 */
typedef struct {
	uint16_t cause;              /*!< Result of the operation. 0 indicates success. */
	uint16_t iso_conn_handle;    /*!< Connection handle of the ISO.
                                  *   Range: 0x0000 to 0x0EFF. */
	uint16_t data_len;           /*!< Length of the data sent, in octets. */
	uint8_t *p_data;             /*!< Pointer to the data sent. */
} rtk_bt_le_iso_data_send_done_t;

/**
 * @struct    rtk_bt_le_iso_cig_cis_info_t
 * @brief     Bluetooth LE ISO CIG and CIS identifier information.
 */
typedef struct {
	uint8_t cig_id;    /*!< Identifier of a CIG. */
	uint8_t cis_id;    /*!< Identifier of a CIS. */
} rtk_bt_le_iso_cig_cis_info_t;

/**
 * @typedef   rtk_bt_le_iso_cis_connetion_state_t
 * @brief     Bluetooth LE ISO CIS connection state definition.
 */
typedef enum {
	RTK_BLE_ISO_ISOCH_STATE_IDLE,                 /*!< CIS idle. */
	RTK_BLE_ISO_ISOCH_STATE_CONNECTING,           /*!< CIS connecting. */
	RTK_BLE_ISO_ISOCH_STATE_CONN,                 /*!< CIS connected. */
	RTK_BLE_ISO_ISOCH_STATE_DISCONNECTING,        /*!< CIS disconnecting. */
	RTK_BLE_ISO_ISOCH_STATE_CONNECTING_CANCEL,    /*!< CIS connecting is being cancelled. */
} rtk_bt_le_iso_cis_connetion_state_t;

/**
 * @typedef   rtk_bt_le_iso_cis_role_t
 * @brief     Bluetooth LE ISO CIS role definition.
 */
typedef enum {
	RTK_BLE_ISO_ISOCH_ROLE_INITIATOR,    /*!< CIS - central role. */
	RTK_BLE_ISO_ISOCH_ROLE_ACCEPTOR,     /*!< CIS - peripheral role. */
} rtk_bt_le_iso_cis_role_t;

/**
 * @struct    rtk_bt_le_iso_cig_init_param_t
 * @brief     Bluetooth LE ISO CIG initialization parameters.
 */
typedef struct {
	uint8_t cig_num;    /*!< Number of CIGs. */
	uint8_t cis_num;    /*!< Number of CISes. */
} rtk_bt_le_iso_cig_init_param_t;

/**
 * @struct    rtk_bt_le_iso_cig_get_conn_handle_param_t
 * @brief     Bluetooth LE ISO parameters for getting the ACL connection handle of a CIS.
 */
typedef struct {
	uint16_t cis_conn_handle;    /*!< Connection handle of the CIS.
                                  *   Range: 0x0000 to 0x0EFF. */
	uint16_t *p_conn_handle;     /*!< Pointer to store the ACL connection handle. */
} rtk_bt_le_iso_cig_get_conn_handle_param_t;

/**
 * @struct    rtk_bt_le_iso_cig_get_cis_info_param_t
 * @brief     Bluetooth LE ISO parameters for getting the CIG and CIS identifiers of a CIS.
 */
typedef struct {
	uint16_t cis_conn_handle;                    /*!< Connection handle of the CIS.
                                                  *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_le_iso_cig_cis_info_t *p_cis_info;    /*!< Pointer to store the CIG and CIS identifiers. */
} rtk_bt_le_iso_cig_get_cis_info_param_t;

/**
 * @struct    rtk_bt_le_iso_cis_channel_info_t
 * @brief     Bluetooth LE ISO CIS channel information.
 */
typedef struct {
	rtk_bt_le_iso_cis_connetion_state_t state;    /*!< CIS connection state. */
	rtk_bt_le_iso_cis_role_t  role;               /*!< CIS role. */
	uint8_t  data_path_flags;                     /*!< Directions of the data path that have been configured.
                                                   *   See @ref rtk_bt_le_iso_data_path_direction_flag_t. */
	uint8_t  data_path_adding_path;               /*!< Direction of the data path being set up.
                                                   *   See @ref rtk_bt_le_iso_data_path_direction_t. */
	bool     data_path_adding;                    /*!< Whether a data path is being set up.
                                                   *   - true: A data path setup is in progress.
                                                   *   - false: No data path setup is in progress. */
	uint8_t  data_path_removing_path;             /*!< Directions of the data path being removed.
                                                   *   See @ref rtk_bt_le_iso_data_path_direction_flag_t. */
	bool     data_path_removing;                  /*!< Whether a data path is being removed.
                                                   *   - true: A data path removal is in progress.
                                                   *   - false: No data path removal is in progress. */
	uint32_t cig_sync_delay;                      /*!< Maximum time, in microseconds, for transmission of PDUs of all CISes in a CIG event.
                                                   *   Range: 0x0000EA to 0x7FFFFF. */
	uint32_t cis_sync_delay;                      /*!< Maximum time, in microseconds, for transmission of PDUs of the specified CIS in a CIG event.
                                                   *   Range: 0x0000EA to 0x7FFFFF. */
	uint32_t transport_latency_m_to_s;            /*!< Actual transport latency, in microseconds, from central to peripheral.
                                                   *   Range: 0x0000EA to 0x7FFFFF. */
	uint32_t transport_latency_s_to_m;            /*!< Actual transport latency, in microseconds, from peripheral to central.
                                                   *   Range: 0x0000EA to 0x7FFFFF. */
	uint8_t phy_m_to_s;                           /*!< PHY selected for packets from the central to the peripheral.
                                                   *   0x01: LE 1M PHY used.
                                                   *   0x02: LE 2M PHY used.
                                                   *   0x03: LE Coded PHY used. */
	uint8_t phy_s_to_m;                           /*!< PHY selected for packets from the peripheral to the central.
                                                   *   0x01: LE 1M PHY used.
                                                   *   0x02: LE 2M PHY used.
                                                   *   0x03: LE Coded PHY used. */
	uint8_t nse;                                  /*!< Maximum number of subevents in each isochronous event. */
	uint8_t bn_m_to_s;                            /*!< Burst number for transmission from the central to the peripheral.
                                                   *   0x00: No isochronous data from the central to the peripheral.
                                                   *   0x01 to 0x0F: Burst number. */
	uint8_t bn_s_to_m;                            /*!< Burst number for transmission from the peripheral to the central.
                                                   *   0x00: No isochronous data from the peripheral to the central.
                                                   *   0x01 to 0x0F: Burst number. */
	uint8_t ft_m_to_s;                            /*!< Flush timeout, in multiples of the ISO interval for the CIS, for each payload sent from the central.
                                                   *   Range: 0x01 to 0xFF. */
	uint8_t ft_s_to_m;                            /*!< Flush timeout, in multiples of the ISO interval for the CIS, for each payload sent from the peripheral.
                                                   *   Range: 0x01 to 0xFF. */
	uint16_t max_pdu_m_to_s;                      /*!< Maximum size, in octets, of the payload from central to peripheral. */
	uint16_t max_pdu_s_to_m;                      /*!< Maximum size, in octets, of the payload from peripheral to central. */
	uint16_t iso_interval;                        /*!< Time between two consecutive CIS anchor points.
                                                   *   Range: 0x0004 to 0x0C80.
                                                   *   Time = N * 1.25 ms.
                                                   *   Time Range: 5 ms to 4 s. */
	bool acceptor_config_sdu_flag;                /*!< Whether the SDU parameters below are configured by the application.
                                                   *   Only used by @ref RTK_BLE_ISO_ISOCH_ROLE_ACCEPTOR.
                                                   *   - true: The SDU length is checked when the acceptor calls @ref rtk_bt_le_iso_data_send.
                                                   *   - false: Default. The SDU length is not checked when the acceptor calls @ref rtk_bt_le_iso_data_send. */
	uint32_t sdu_interval_m_to_s;                 /*!< Interval, in microseconds, between the start of consecutive SDUs from the central's Host for all CISes in the CIG.
                                                   *   Valid for the initiator role, and for the acceptor role if @ref acceptor_config_sdu_flag is true.
                                                   *   Range: 0x0000FF to 0x0FFFFF. */
	uint32_t sdu_interval_s_to_m;                 /*!< Interval, in microseconds, between the start of consecutive SDUs from the peripheral's Host for all CISes
                                                   *   in the CIG.
                                                   *   Valid for the initiator role, and for the acceptor role if @ref acceptor_config_sdu_flag is true.
                                                   *   Range: 0x0000FF to 0x0FFFFF. */
	uint16_t max_sdu_m_to_s;                      /*!< Maximum size, in octets, of the payload from the central's Host.
                                                   *   Valid for @ref RTK_BLE_ISO_ISOCH_ROLE_INITIATOR.
                                                   *   Valid for @ref RTK_BLE_ISO_ISOCH_ROLE_ACCEPTOR if @ref acceptor_config_sdu_flag is true. */
	uint16_t max_sdu_s_to_m;                      /*!< Maximum size, in octets, of the payload from the peripheral's Host.
                                                   *   Valid for @ref RTK_BLE_ISO_ISOCH_ROLE_INITIATOR.
                                                   *   Valid for @ref RTK_BLE_ISO_ISOCH_ROLE_ACCEPTOR if @ref acceptor_config_sdu_flag is true. */
} rtk_bt_le_iso_cis_channel_info_t;

/**
 * @struct    rtk_bt_le_iso_cig_get_isoch_info_param_t
 * @brief     Bluetooth LE ISO parameters for getting the channel information of a CIS.
 */
typedef struct {
	uint16_t cis_conn_handle;                          /*!< Connection handle of the CIS.
                                                        *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_le_iso_cis_channel_info_t *p_isoch_info;    /*!< Pointer to store the CIS channel information. */
} rtk_bt_le_iso_cig_get_isoch_info_param_t;

/**
 * @struct    rtk_bt_le_iso_cig_initiator_set_cis_acl_link_param_t
 * @brief     Bluetooth LE ISO parameters for binding a CIS to an ACL link on the initiator.
 */
typedef struct {
	uint8_t cis_id;          /*!< Identifier of a CIS. */
	uint16_t conn_handle;    /*!< Connection handle of the ACL connection.
                              *   Range: 0x0000 to 0x0EFF. */
} rtk_bt_le_iso_cig_initiator_set_cis_acl_link_param_t;

/**
 * @struct    rtk_bt_le_iso_cig_initiator_set_cig_param_t
 * @brief     Bluetooth LE ISO CIG parameters on the initiator.
 */
typedef struct {
	uint8_t  cig_id;              /*!< Identifier of a CIG. */
	uint32_t sdu_interval_m_s;    /*!< Interval, in microseconds, of periodic SDUs from the central's Host for all the CISes in the CIG.
                                   *   Ignored for all CISes that are unidirectional from peripheral to central.
                                   *   Range: 0x0000FF to 0x0FFFFF. */
	uint32_t sdu_interval_s_m;    /*!< Interval, in microseconds, of periodic SDUs from the peripheral's Host for all the CISes in the CIG.
                                   *   Ignored for all CISes that are unidirectional from central to peripheral.
                                   *   Range: 0x0000FF to 0x0FFFFF. */
	uint8_t  sca;                 /*!< Worst-case sleep clock accuracy of all the peripherals that will participate in the CIG.
                                   *   0x00: 251 ppm to 500 ppm.
                                   *   0x01: 151 ppm to 250 ppm.
                                   *   0x02: 101 ppm to 150 ppm.
                                   *   0x03: 76 ppm to 100 ppm.
                                   *   0x04: 51 ppm to 75 ppm.
                                   *   0x05: 31 ppm to 50 ppm.
                                   *   0x06: 21 ppm to 30 ppm.
                                   *   0x07: 0 ppm to 20 ppm. */
	uint8_t  packing;             /*!< Preferred method of arranging subevents of multiple CISes.
                                   *   0x00: Sequential.
                                   *   0x01: Interleaved. */
	uint8_t  framing;             /*!< Format of the CIS data PDUs.
                                   *   0x00: Unframed.
                                   *   0x01: Framed. */
	uint16_t latency_m_s;         /*!< Maximum transport latency, in milliseconds, from the central's Controller to the peripheral's Controller.
                                   *   Range: 0x0005 to 0x0FA0. */
	uint16_t latency_s_m;         /*!< Maximum transport latency, in milliseconds, from the peripheral's Controller to the central's Controller.
                                   *   Range: 0x0005 to 0x0FA0. */
} rtk_bt_le_iso_cig_initiator_set_cig_param_t;

/**
 * @struct    rtk_bt_le_iso_cig_initiator_set_cis_param_t
 * @brief     Bluetooth LE ISO CIS parameters on the initiator.
 */
typedef struct {
	uint8_t cis_id;          /*!< Identifier of a CIS. */
	uint16_t max_sdu_m_s;    /*!< Maximum size, in octets, of the payload from the central's Host.
                              *   Set to 0 if the CIS is unidirectional from peripheral to central. */
	uint16_t max_sdu_s_m;    /*!< Maximum size, in octets, of the payload from the peripheral's Host.
                              *   Set to 0 if the CIS is unidirectional from central to peripheral. */
	uint8_t  phy_m_s;        /*!< PHY used for transmission from the central to the peripheral.
                              *   Bit 0: LE 1M PHY.
                              *   Bit 1: LE 2M PHY.
                              *   Bit 2: LE Coded PHY. */
	uint8_t  phy_s_m;        /*!< PHY used for transmission from the peripheral to the central.
                              *   Bit 0: LE 1M PHY.
                              *   Bit 1: LE 2M PHY.
                              *   Bit 2: LE Coded PHY. */
	uint8_t  rtn_m_s;        /*!< Number of times every CIS data PDU should be retransmitted from the central to the peripheral. */
	uint8_t  rtn_s_m;        /*!< Number of times every CIS data PDU should be retransmitted from the peripheral to the central. */
} rtk_bt_le_iso_cig_initiator_set_cis_param_t;

/**
 * @struct    rtk_bt_le_iso_set_cig_cis_param_t
 * @brief     Bluetooth LE ISO CIG and CIS setting parameters.
 */
typedef struct {
	uint8_t cis_num;                                                                   /*!< Number of valid entries in set_cis_param. */
	rtk_bt_le_iso_cig_initiator_set_cig_param_t set_cig_param;                         /*!< CIG parameters. */
	rtk_bt_le_iso_cig_initiator_set_cis_param_t set_cis_param[RTK_BLE_CIS_MAX_NUM];    /*!< CIS parameters, one entry for each CIS. */
} rtk_bt_le_iso_set_cig_cis_param_t;

/**
 * @struct    rtk_bt_le_iso_cig_initiator_create_cis_by_cis_conn_handle_param_t
 * @brief     Bluetooth LE ISO parameters for creating CISes by CIS connection handle on the initiator.
 */
typedef struct {
	uint8_t cig_id;                 /*!< Identifier of a CIG. */
	uint8_t cis_count;              /*!< Number of CISes to create. */
	uint16_t *p_cis_conn_handle;    /*!< Pointer to the array of CIS connection handles. */
} rtk_bt_le_iso_cig_initiator_create_cis_by_cis_conn_handle_param_t;

/**
 * @struct    rtk_bt_le_iso_cig_initiator_get_cis_handle_param_t
 * @brief     Bluetooth LE ISO parameters for getting the CIS connection handle on the initiator.
 */
typedef struct {
	uint8_t cis_id;            /*!< Identifier of a CIS. */
	uint16_t *p_cis_handle;    /*!< Pointer to store the connection handle of the CIS. */
} rtk_bt_le_iso_cig_initiator_get_cis_handle_param_t;

/**
 * @struct    rtk_bt_le_iso_cig_acceptor_reject_cis_param_t
 * @brief     Bluetooth LE ISO parameters for rejecting a CIS request on the acceptor.
 */
typedef struct {
	uint16_t cis_conn_handle;    /*!< Connection handle of the CIS.
                                  *   Range: 0x0000 to 0x0EFF. */
	uint8_t reason;              /*!< Reason the CIS request was rejected.
                                  *   Any error code except success. */
} rtk_bt_le_iso_cig_acceptor_reject_cis_param_t;

/**
 * @struct    rtk_bt_le_iso_cig_acceptor_config_sdu_param_t
 * @brief     Bluetooth LE ISO SDU configuration parameters on the acceptor.
 */
typedef struct {
	uint16_t cis_conn_handle;         /*!< Connection handle of the CIS.
                                       *   Range: 0x0000 to 0x0EFF. */
	bool acceptor_config_sdu_flag;    /*!< Whether to check the SDU length when the acceptor calls @ref rtk_bt_le_iso_data_send.
                                       *   - true: The SDU length is checked against the SDU parameters below.
                                       *   - false: The SDU length is not checked. */
	uint32_t sdu_interval_m_s;        /*!< Interval, in microseconds, between the start of consecutive SDUs from the central's Host for all the CISes in the CIG.
                                       *   Range: 0x0000FF to 0x0FFFFF. */
	uint32_t sdu_interval_s_m;        /*!< Interval, in microseconds, between the start of consecutive SDUs from the peripheral's Host for all the CISes in the CIG.
                                       *   Range: 0x0000FF to 0x0FFFFF. */
	uint16_t max_sdu_m_s;             /*!< Maximum size, in octets, of the payload from the central's Host. */
	uint16_t max_sdu_s_m;             /*!< Maximum size, in octets, of the payload from the peripheral's Host. */
} rtk_bt_le_iso_cig_acceptor_config_sdu_param_t;

/**
 * @typedef   rtk_bt_le_iso_cig_acceptor_config_cis_req_action_t
 * @brief     Bluetooth LE ISO action taken by the acceptor when a CIS request is received.
 */
typedef enum {
	RTK_BLE_ISO_ACCEPTOR_CIS_REQ_ACTION_ACCEPT  = 0x00,    /*!< Accept the CIS request when the acceptor receives it from the initiator. */
	RTK_BLE_ISO_ACCEPTOR_CIS_REQ_ACTION_REJECT,            /*!< Reject the CIS request when the acceptor receives it from the initiator. */
	RTK_BLE_ISO_ACCEPTOR_CIS_REQ_ACTION_PENDING,           /*!< Pend the CIS request when the acceptor receives it from the initiator.
                                                            *   The application must then call
                                                            *   @ref rtk_bt_le_iso_cig_acceptor_accept_cis or @ref rtk_bt_le_iso_cig_acceptor_reject_cis
                                                            *   according to the usage scenario. */
} rtk_bt_le_iso_cig_acceptor_config_cis_req_action_t;

/**
 * @struct    rtk_bt_le_iso_cig_setup_data_path_done_t
 * @brief     Bluetooth LE ISO CIG setup data path done information.
 */
typedef struct {
	uint16_t cause;                                             /*!< Result of the operation. 0 indicates success. */
	uint16_t cis_conn_handle;                                   /*!< Connection handle of the CIS.
                                                                 *   Range: 0x0000 to 0x0EFF. */
	uint16_t conn_handle;                                       /*!< Connection handle of the ACL connection.
                                                                 *   Range: 0x0000 to 0x0EFF. */
	uint8_t cig_id;                                             /*!< Identifier of a CIG. Meaningless if cause is not 0. */
	uint8_t cis_id;                                             /*!< Identifier of a CIS. Meaningless if cause is not 0. */
	rtk_bt_le_iso_data_path_direction_t data_path_direction;    /*!< Direction of the data path that was set up. Meaningless if cause is not 0. */
} rtk_bt_le_iso_cig_setup_data_path_done_t;

/**
 * @struct    rtk_bt_le_iso_cig_remove_data_path_done_t
 * @brief     Bluetooth LE ISO CIG remove data path done information.
 */
typedef struct {
	uint16_t cause;                                                 /*!< Result of the operation. 0 indicates success. */
	uint16_t cis_conn_handle;                                       /*!< Connection handle of the CIS.
                                                                     *   Range: 0x0000 to 0x0EFF. */
	uint16_t conn_handle;                                           /*!< Connection handle of the ACL connection.
                                                                     *   Range: 0x0000 to 0x0EFF. */
	uint8_t cig_id;                                                 /*!< Identifier of a CIG. Meaningless if cause is not 0. */
	uint8_t cis_id;                                                 /*!< Identifier of a CIS. Meaningless if cause is not 0. */
	rtk_bt_le_iso_data_path_direction_flag_t data_path_direction    /*!< Directions of the data path that were removed. Meaningless if cause is not 0. */;
} rtk_bt_le_iso_cig_remove_data_path_done_t;

/**
 * @struct    rtk_bt_le_iso_cig_disconnect_rsp_t
 * @brief     Bluetooth LE ISO CIG disconnect response information.
 */
typedef struct {
	uint16_t cause;              /*!< Result of the operation. 0 indicates success. */
	uint16_t cis_conn_handle;    /*!< Connection handle of the CIS.
                                  *   Range: 0x0000 to 0x0EFF. */
	uint16_t conn_handle;        /*!< Connection handle of the ACL connection.
                                  *   Range: 0x0000 to 0x0EFF. */
	uint8_t cig_id;              /*!< Identifier of a CIG. Meaningless if cause is not 0. */
	uint8_t cis_id;              /*!< Identifier of a CIS. Meaningless if cause is not 0. */
} rtk_bt_le_iso_cig_disconnect_rsp_t;

/**
 * @struct    rtk_bt_le_iso_cig_disconnect_info_t
 * @brief     Bluetooth LE ISO CIG disconnect indication information.
 */
typedef struct {
	uint16_t cause;                    /*!< Disconnection reason. */
	uint16_t cis_conn_handle;          /*!< Connection handle of the CIS.
                                        *   Range: 0x0000 to 0x0EFF. */
	uint16_t conn_handle;              /*!< Connection handle of the ACL connection.
                                        *   Range: 0x0000 to 0x0EFF. */
	uint8_t cig_id;                    /*!< Identifier of a CIG. */
	uint8_t cis_id;                    /*!< Identifier of a CIS. */
	uint8_t input_path_remove_ind;     /*!< Whether the input (Host to Controller) data path of the CIS needs to be removed.
                                        *   1: The input data path needs to be removed.
                                        *   0: No input data path removal is needed. */
	uint8_t output_path_remove_ind;    /*!< Whether the output (Controller to Host) data path of the CIS needs to be removed.
                                        *   1: The output data path needs to be removed.
                                        *   0: No output data path removal is needed. */
} rtk_bt_le_iso_cig_disconnect_info_t;

/**
 * @struct    rtk_bt_le_iso_cig_read_iso_tx_sync_info_t
 * @brief     Bluetooth LE ISO CIG ISO TX synchronization information.
 */
typedef struct {
	uint16_t conn_handle;               /*!< Connection handle of the ACL connection.
                                         *   Range: 0x0000 to 0x0EFF. */
	uint8_t cig_id;                     /*!< Identifier of a CIG. */
	uint8_t cis_id;                     /*!< Identifier of a CIS. */
	uint16_t packet_sequence_number;    /*!< Packet sequence number of an SDU. */
	uint32_t time_stamp;                /*!< CIG reference point of a transmitted SDU, derived using the Controller's free running reference clock, in microseconds. */
	uint32_t time_offset;               /*!< Time offset, in microseconds, that is associated with a transmitted SDU. */
} rtk_bt_le_iso_cig_read_iso_tx_sync_info_t;

/**
 * @struct    rtk_bt_le_iso_cig_read_iso_tx_sync_t
 * @brief     Bluetooth LE ISO parameters for reading the ISO TX synchronization information of a CIS.
 */
typedef struct {
	uint16_t cis_conn_handle;                                     /*!< Connection handle of the CIS.
                                                                   *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_le_iso_cig_read_iso_tx_sync_info_t *p_tx_sync_info;    /*!< Pointer to store the ISO TX synchronization information. */
} rtk_bt_le_iso_cig_read_iso_tx_sync_t;

/**
 * @struct    rtk_bt_le_iso_cig_read_link_quality_info_t
 * @brief     Bluetooth LE ISO Link quality counters of a CIS.
 */
typedef struct {
	uint16_t conn_handle;                 /*!< Connection handle of the ACL connection associated with the CIS.
                                           *   Range: 0x0000 to 0x0EFF. */
	uint8_t cig_id;                       /*!< Identifier of the CIG. */
	uint8_t cis_id;                       /*!< Identifier of the CIS. */
	uint32_t tx_unacked_packets;          /*!< Number of CIS Data PDUs for which no acknowledgment was received by their flush point, although they were transmitted
                                           *   at least once. */
	uint32_t tx_flushed_packets;          /*!< Number of payloads that were not transmitted by their flush point. */
	uint32_t tx_last_subevent_packets;    /*!< Number of CIS Data PDUs transmitted in the last subevent of a CIS event.
                                           *   Applies to @ref RTK_BLE_ISO_ISOCH_ROLE_ACCEPTOR. */
	uint32_t retransmitted_packets;       /*!< Number of retransmitted CIS Data PDUs. */
	uint32_t crc_error_packets;           /*!< Number of packets received with a CRC error. */
	uint32_t rx_unreceived_packets;       /*!< Number of payloads that were not received by their flush point. */
	uint32_t duplicate_packets;           /*!< Number of retransmissions of CIS Data PDUs that were received. */
} rtk_bt_le_iso_cig_read_link_quality_info_t;

/**
 * @struct    rtk_bt_le_iso_cig_read_link_quality_t
 * @brief     Bluetooth LE ISO Parameters for reading the link quality of a CIS.
 */
typedef struct {
	uint16_t      cis_conn_handle;                                      /*!< Connection handle of the CIS.
                                                                         *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_le_iso_cig_read_link_quality_info_t *p_link_quality_info;    /*!< Buffer that receives the link quality info. */
} rtk_bt_le_iso_cig_read_link_quality_t;

/**
 * @struct    rtk_bt_le_iso_cig_initiator_add_cis_done_t
 * @brief     Bluetooth LE ISO information reported when adding a CIS to a CIG completes for the initiator.
 */
typedef struct {
	uint16_t      cause;     /*!< Result of the operation. 0 indicates success. */
	uint8_t       cig_id;    /*!< Identifier of the CIG. */
	uint8_t       cis_id;    /*!< Identifier of the CIS. */
} rtk_bt_le_iso_cig_initiator_add_cis_done_t;

/**
 * @struct    rtk_bt_le_iso_cig_initiator_set_cig_param_done_t
 * @brief     Bluetooth LE ISO information reported when setting the CIG parameters completes for the initiator.
 */
typedef struct {
	uint16_t      cause;     /*!< Result of the operation. 0 indicates success. */
	uint8_t       cig_id;    /*!< Identifier of the CIG. */
} rtk_bt_le_iso_cig_initiator_set_cig_param_done_t;

/**
 * @struct    rtk_bt_le_iso_cig_initiator_set_cis_param_done_t
 * @brief     Bluetooth LE ISO information reported when setting the CIS parameters completes for the initiator.
 */
typedef struct {
	uint16_t      cause;     /*!< Result of the operation. 0 indicates success. */
	uint8_t       cis_id;    /*!< Identifier of the CIS. */
} rtk_bt_le_iso_cig_initiator_set_cis_param_done_t;

/**
 * @struct    rtk_bt_le_iso_cig_initiator_start_setting_cis_info_t
 * @brief     Bluetooth LE ISO information of a CIS reported when the CIG start setting completes for the initiator.
 */
typedef struct {
	uint8_t       cis_id;             /*!< Identifier of the CIS. */
	uint16_t      cis_conn_handle;    /*!< Connection handle of the CIS.
                                       *   Range: 0x0000 to 0x0EFF. */
} rtk_bt_le_iso_cig_initiator_start_setting_cis_info_t;

/**
 * @struct    rtk_bt_le_iso_cig_initiator_start_setting_done_t
 * @brief     Bluetooth LE ISO information reported when the CIG start setting completes for the initiator.
 */
typedef struct {
	uint16_t cause;                                                       /*!< Result of the operation. 0 indicates success. */
	uint8_t  cig_id;                                                      /*!< Identifier of the CIG. */
	uint8_t  cis_count;                                                   /*!< Number of CISes in the CIG. */
	rtk_bt_le_iso_cig_initiator_start_setting_cis_info_t  cis_info[8];    /*!< Information of each CIS. The number of valid entries is given by cis_count. */
} rtk_bt_le_iso_cig_initiator_start_setting_done_t;

/**
 * @struct    rtk_bt_le_iso_cig_initiator_set_cis_acl_link_done_t
 * @brief     Bluetooth LE ISO information reported when binding a CIS to an ACL link completes for the initiator.
 */
typedef struct {
	uint16_t cause;          /*!< Result of the operation. 0 indicates success. */
	uint8_t  cis_id;         /*!< Identifier of the CIS. */
	uint16_t conn_handle;    /*!< Connection handle of the ACL connection.
                              *   Range: 0x0000 to 0x0EFF. */
} rtk_bt_le_iso_cig_initiator_set_cis_acl_link_done_t;

/**
 * @struct    rtk_bt_le_iso_cig_initiator_create_cis_info_t
 * @brief     Bluetooth LE ISO information of a CIS reported when creating the CISes for the initiator.
 */
typedef struct {
	uint8_t       cig_id;                         /*!< Identifier of the CIG. */
	uint8_t       cis_id;                         /*!< Identifier of the CIS. */
	rtk_bt_le_iso_cis_connetion_state_t state;    /*!< CIS connection state. */
	uint16_t      cis_conn_handle;                /*!< Connection handle of the CIS.
                                                   *   Range: 0x0000 to 0x0EFF. */
} rtk_bt_le_iso_cig_initiator_create_cis_info_t;

/**
 * @struct    rtk_bt_le_iso_cig_initiator_create_cis_done_t
 * @brief     Bluetooth LE ISO information reported when creating the CISes completes for the initiator.
 */
typedef struct {
	uint16_t          cause;                                      /*!< Result of the operation. 0 indicates success. */
	uint8_t           cis_count;                                  /*!< Number of CISes. */
	rtk_bt_le_iso_cig_initiator_create_cis_info_t cis_info[8];    /*!< Information of each CIS. The number of valid entries is given by cis_count. */
} rtk_bt_le_iso_cig_initiator_create_cis_done_t;

/**
 * @struct    rtk_bt_le_iso_cig_start_setting_cis_info_t
 * @brief     Bluetooth LE ISO information of a CIS reported when the CIG start setting completes.
 */
typedef struct {
	uint8_t  cis_id;             /*!< Identifier of the CIS. */
	uint16_t cis_conn_handle;    /*!< Connection handle of the CIS.
                                  *   Range: 0x0000 to 0x0EFF. */
} rtk_bt_le_iso_cig_start_setting_cis_info_t;

/**
 * @struct    rtk_bt_le_iso_cig_start_setting_info_t
 * @brief     Bluetooth LE ISO information reported when the CIG start setting completes. It carries the CIS connection handles needed to create the CISes later.
 */
typedef struct {
	uint16_t cause;                                                              /*!< Result of the operation. 0 indicates success. */
	uint8_t  cig_id;                                                             /*!< Identifier of the CIG. */
	uint8_t  cis_count;                                                          /*!< Number of CISes in the CIG. */
	rtk_bt_le_iso_cig_start_setting_cis_info_t cis_info[RTK_BLE_CIS_MAX_NUM];    /*!< Information of each CIS. The number of valid entries is given by cis_count. */
} rtk_bt_le_iso_cig_start_setting_info_t;

/**
 * @struct    rtk_bt_le_iso_cig_cis_established_info_t
 * @brief     Bluetooth LE ISO CIS established indication.
 */
typedef struct {
	uint16_t cause;                    /*!< Result of the operation. 0 indicates success. */
	uint16_t conn_handle;              /*!< Connection handle of the ACL connection associated with the CIS.
                                        *   Range: 0x0000 to 0x0EFF. */
	uint8_t cig_id;                    /*!< Identifier of the CIG. */
	uint8_t cis_id;                    /*!< Identifier of the CIS. */
	uint16_t cis_conn_handle;          /*!< Connection handle of the CIS.
                                        *   Range: 0x0000 to 0x0EFF. */
	uint32_t cig_sync_delay;           /*!< The maximum time, in microseconds, for transmission of PDUs of all CISes in a CIG event. */
	uint32_t cis_sync_delay;           /*!< The maximum time, in microseconds, for transmission of PDUs of the specified CIS in a CIG event. */
	uint32_t transport_latency_m_s;    /*!< The actual transport latency, in microseconds, from central to peripheral. */
	uint32_t transport_latency_s_m;    /*!< The actual transport latency, in microseconds, from peripheral to central. */
	uint8_t phy_m_s;                   /*!< The transmitter PHY of packets from the central.
                                        *   0x01: LE 1M.
                                        *   0x02: LE 2M.
                                        *   0x03: LE Coded. */
	uint8_t phy_s_m;                   /*!< The transmitter PHY of packets from the peripheral.
                                        *   0x01: LE 1M.
                                        *   0x02: LE 2M.
                                        *   0x03: LE Coded. */
	uint8_t nse;                       /*!< Maximum number of subevents in each isochronous event. */
	uint8_t bn_m_s;                    /*!< The burst number for central to peripheral transmission.
                                        *   0x00: No isochronous data from the central to the peripheral.
                                        *   0x01 to 0x0F: Burst number. */
	uint8_t bn_s_m;                    /*!< The burst number for peripheral to central transmission.
                                        *   0x00: No isochronous data from the peripheral to the central.
                                        *   0x01 to 0x0F: Burst number. */
	uint8_t ft_m_s;                    /*!< The flush timeout, in multiples of the ISO interval for the CIS, for each payload sent from the central to the peripheral. */
	uint8_t ft_s_m;                    /*!< The flush timeout, in multiples of the ISO interval for the CIS, for each payload sent from the peripheral to the central. */
	uint16_t max_pdu_m_s;              /*!< Maximum size, in octets, of the payload from central to peripheral. */
	uint16_t max_pdu_s_m;              /*!< Maximum size, in octets, of the payload from peripheral to central. */
	uint16_t iso_interval;             /*!< The time between two consecutive CIS anchor points.
                                        *   Range: 0x0004 to 0x0C80.
                                        *   Time = N * 1.25 ms.
                                        *   Time Range: 5 ms to 4 s. */
} rtk_bt_le_iso_cig_cis_established_info_t;

/**
 * @struct    rtk_bt_le_iso_cig_setup_data_path_info_t
 * @brief     Bluetooth LE ISO CIG data path setup indication.
 */
typedef struct {
	uint16_t cause;                 /*!< Result of the operation. 0 indicates success. */
	uint16_t cis_conn_handle;       /*!< Connection handle of the CIS.
                                     *   Range: 0x0000 to 0x0EFF. */
	uint8_t conn_id;                /*!< Connection ID of the ACL connection associated with the CIS. */
	uint8_t cig_id;                 /*!< Identifier of the CIG. */
	uint8_t cis_id;                 /*!< Identifier of the CIS. */
	uint8_t data_path_direction;    /*!< Direction of the data path that was set up. */
} rtk_bt_le_iso_cig_setup_data_path_info_t;

/**
 * @struct    rtk_bt_le_iso_cig_remove_data_path_info_t
 * @brief     Bluetooth LE ISO CIG data path removal indication.
 */
typedef struct {
	uint16_t cause;                 /*!< Result of the operation. 0 indicates success. */
	uint16_t cis_conn_handle;       /*!< Connection handle of the CIS.
                                     *   Range: 0x0000 to 0x0EFF. */
	uint8_t conn_id;                /*!< Connection ID of the ACL connection associated with the CIS. */
	uint8_t cig_id;                 /*!< Identifier of the CIG. */
	uint8_t cis_id;                 /*!< Identifier of the CIS. */
	uint8_t data_path_direction;    /*!< Direction of the data path that was removed. */
} rtk_bt_le_iso_cig_remove_data_path_info_t;

/**
 * @struct    rtk_bt_le_iso_cig_initiator_remove_cig_done_t
 * @brief     Bluetooth LE ISO information reported when removing a CIG completes for the initiator.
 */
typedef struct {
	uint16_t cause;    /*!< Result of the operation. 0 indicates success. */
	uint8_t cig_id;    /*!< Identifier of the CIG. */
} rtk_bt_le_iso_cig_initiator_remove_cig_done_t;

/**
 * @struct    rtk_bt_le_iso_cig_acceptor_request_cis_ind_t
 * @brief     Bluetooth LE ISO CIS request indication for the acceptor.
 */
typedef struct {
	uint16_t cis_conn_handle;                                             /*!< Connection handle of the CIS.
                                                                           *   Range: 0x0000 to 0x0EFF. */
	uint16_t conn_handle;                                                 /*!< Connection handle of the ACL connection.
                                                                           *   Range: 0x0000 to 0x0EFF. */
	uint8_t cig_id;                                                       /*!< Identifier of the CIG. */
	uint8_t cis_id;                                                       /*!< Identifier of the CIS. */
	rtk_bt_le_iso_cig_acceptor_config_cis_req_action_t cis_req_action;    /*!< Action configured for the CIS request. */
} rtk_bt_le_iso_cig_acceptor_request_cis_ind_t;

/**
 * @struct    rtk_bt_le_iso_cig_acceptor_accept_cis_done_t
 * @brief     Bluetooth LE ISO information reported when accepting a CIS request completes for the acceptor.
 */
typedef struct {
	uint16_t cause;              /*!< Result of the operation. 0 indicates success. */
	uint16_t cis_conn_handle;    /*!< Connection handle of the CIS.
                                  *   Range: 0x0000 to 0x0EFF. */
	uint16_t conn_handle;        /*!< Connection handle of the ACL connection. Meaningless if cause is not 0.
                                  *   Range: 0x0000 to 0x0EFF. */
	uint8_t cig_id;              /*!< Identifier of the CIG. Meaningless if cause is not 0. */
	uint8_t cis_id;              /*!< Identifier of the CIS. Meaningless if cause is not 0. */
} rtk_bt_le_iso_cig_acceptor_accept_cis_done_t;

/**
 * @typedef   rtk_bt_le_iso_cig_acceptor_reject_cis_done_t
 * @brief     Bluetooth LE ISO information reported when rejecting a CIS request completes for the acceptor.
 */
typedef rtk_bt_le_iso_cig_acceptor_accept_cis_done_t rtk_bt_le_iso_cig_acceptor_reject_cis_done_t  ;

/**
 * @typedef   rtk_bt_le_iso_bis_role_t
 * @brief     Bluetooth LE ISO BIS role definition.
 */
typedef enum {
	RTK_BLE_ISO_ISOCH_ROLE_BROADCASTER = 1,    /*!< BIS broadcaster role. */
	RTK_BLE_ISO_ISOCH_ROLE_RECEIVER,           /*!< BIS receiver role. */
} rtk_bt_le_iso_bis_role_t;

/**
 * @struct    rtk_bt_le_iso_big_init_param_t
 * @brief     Bluetooth LE ISO BIG initialization parameters.
 */
typedef struct {
	uint8_t big_num;    /*!< Number of BIGs. */
	uint8_t bis_num;    /*!< Number of BISes. */
} rtk_bt_le_iso_big_init_param_t;

/**
 * @struct    rtk_bt_le_iso_big_broadcaster_param_t
 * @brief     Bluetooth LE ISO BIG parameters of the broadcaster.
 */
typedef struct {
	uint8_t num_bis;                   /*!< Total number of BISes in the BIG. */
	uint32_t sdu_interval;             /*!< The interval, in microseconds, of periodic SDUs.
                                        *   Range: 0x0000FF to 0x0FFFFF. */
	uint16_t max_sdu;                  /*!< Maximum size of an SDU, in octets. */
	uint16_t max_transport_latency;    /*!< Maximum transport latency, in milliseconds.
                                        *   Range: 0x0005 to 0x0FA0. */
	uint8_t rtn;                       /*!< The number of times that every BIS Data PDU should be retransmitted.
                                        *   Range: 0x00 to 0x1E. */
	uint8_t phy;                       /*!< A bit field that indicates the PHY used for transmission of PDUs of BISes in the BIG.
                                        *   Bit 0: LE 1M.
                                        *   Bit 1: LE 2M.
                                        *   Bit 2: LE Coded. */
	uint8_t  packing;                  /*!< The preferred method of arranging subevents of multiple BISes.
                                        *   0x00: Sequential.
                                        *   0x01: Interleaved. */
	uint8_t  framing;                  /*!< The format for sending BIS Data PDUs.
                                        *   0x00: Unframed.
                                        *   0x01: Framed. */
	uint8_t encryption;                /*!< The encryption mode of the BISes.
                                        *   0x00: Unencrypted.
                                        *   0x01: Encrypted. */
	uint8_t broadcast_code[16];        /*!< Used to generate the encryption key for encrypting payloads of all BISes. */
} rtk_bt_le_iso_big_broadcaster_param_t;

/**
 * @struct    rtk_bt_le_iso_big_broadcaster_create_param_t
 * @brief     Bluetooth LE ISO Parameters for creating a BIG broadcaster.
 */
typedef struct {
	uint8_t adv_handle;                                    /*!< Handle identifying the periodic advertising set.
                                                            *   Range: 0x00 to 0xEF. */
	rtk_bt_le_iso_big_broadcaster_param_t *p_big_param;    /*!< BIG creation parameters. */
	uint8_t *big_handle;                                   /*!< Buffer that receives the handle identifying the BIG. */
} rtk_bt_le_iso_big_broadcaster_create_param_t;

/**
 * @struct    rtk_bt_le_iso_big_bis_conn_handle_info_t
 * @brief     Bluetooth LE ISO Connection handle information of a BIS.
 */
typedef struct {
	uint8_t bis_idx;             /*!< Index of the BIS in the BIG. */
	uint16_t bis_conn_handle;    /*!< Connection handle of the BIS.
                                  *   Range: 0x0000 to 0x0EFF. */
} rtk_bt_le_iso_big_bis_conn_handle_info_t;

/**
 * @struct    rtk_bt_le_iso_big_broadcaster_create_cmpl_info_t
 * @brief     Bluetooth LE ISO BIG creation result of the broadcaster.
 */
typedef struct {
	uint16_t cause;                                                                        /*!< Result of the operation. 0 indicates success. */
	uint8_t  big_handle;                                                                   /*!< Used to identify the BIG.
                                                                                            *   Range: 0x00 to 0xEF. */
	uint8_t adv_handle;                                                                    /*!< Handle identifying the periodic advertising set.
                                                                                            *   Range: 0x00 to 0xEF. */
	uint32_t big_sync_delay;                                                               /*!< The maximum time, in microseconds, for transmission of PDUs of all BISes
                                                                                            *   in a BIG event. */
	uint32_t transport_latency_big;                                                        /*!< The actual transport latency, in microseconds. */
	uint8_t phy;                                                                           /*!< The PHY used to create the BIG, @ref rtk_bt_le_phy_type_t. */
	uint8_t nse;                                                                           /*!< The number of subevents in each BIS event in the BIG. */
	uint8_t bn;                                                                            /*!< Burst number: the number of new payloads in each BIS event. */
	uint8_t pto;                                                                           /*!< Pre-transmission offset: the offset, in units of ISO_Interval,
                                                                                            *   used for pre-transmission of future data. */
	uint8_t irc;                                                                           /*!< Immediate repetition count.
                                                                                            *   The number of times a scheduled data packet is sent. In other words,
                                                                                            *   the number of groups that carry the data of the current BIS event. */
	uint16_t max_pdu;                                                                      /*!< Maximum size, in octets, of the payload. */
	uint16_t iso_interval;                                                                 /*!< The time between two consecutive BIG anchor points.
                                                                                            *   Range: 0x0004 to 0x0C80.
                                                                                            *   Time = N * 1.25 ms.
                                                                                            *   Time Range: 5 ms to 4 s. */
	uint8_t num_bis;                                                                       /*!< Total number of BISes in the BIG. */
	rtk_bt_le_iso_big_bis_conn_handle_info_t bis_conn_handle_info[RTK_BLE_BIS_MAX_NUM];    /*!< Connection handle information of each BIS. */
} rtk_bt_le_iso_big_broadcaster_create_cmpl_info_t;

/**
 * @struct    rtk_bt_le_iso_big_broadcaster_terminate_done_t
 * @brief     Bluetooth LE ISO BIG broadcaster terminate result.
 */
typedef struct {
	uint8_t big_handle;    /*!< Identify the BIG.
                            *   Range: 0x00 to 0xEF. */
	uint8_t adv_handle;    /*!< Handle identifying the periodic advertising set.
                            *   Range: 0x00 to 0xEF. */
	uint8_t cause;         /*!< Result of the operation. 0 indicates success. */
} rtk_bt_le_iso_big_broadcaster_terminate_done_t;

/**
 * @typedef   rtk_bt_le_iso_big_broadcaster_sync_state_t
 * @brief     Bluetooth LE ISO BIG broadcaster synchronization state.
 */
typedef enum {
	RTK_BLE_ISO_BIG_BOARDCASTER_SYNC_STATE_IDLE = 0x00,                                           /*!< Idle, no isochronous broadcasting. */
	RTK_BLE_ISO_BIG_BOARDCASTER_SYNC_STATE_CREATING_EXT_ADV_STATE_PA_ADV_STATE_IDLE = 0x01,       /*!< Start isochronous broadcasting when extended
                                                                                                   *   advertising and periodic advertising of the specified advertising
                                                                                                   *   set are disabled. Temporary state, the result has
                                                                                                   *   not been received yet. */
	RTK_BLE_ISO_BIG_BOARDCASTER_SYNC_STATE_WAIT_EXT_ADV_STATE_PA_ADV_STATE_ADVERTISING = 0x02,    /*!< Isochronous broadcasting is not started until the extended
                                                                                                   *   advertising and periodic advertising of the specified advertising
                                                                                                   *   set are enabled. */
	RTK_BLE_ISO_BIG_BOARDCASTER_SYNC_STATE_CREATING_PA_ADV_STATE_IDLE = 0x03,                     /*!< Start isochronous broadcasting when the periodic advertising
                                                                                                   *   of the specified advertising set is disabled. Temporary state,
                                                                                                   *   the result has not been received yet. */
	RTK_BLE_ISO_BIG_BOARDCASTER_SYNC_STATE_WAIT_PA_ADV_STATE_ADVERTISING = 0x04,                  /*!< Isochronous broadcasting is not started until the periodic
                                                                                                   *   advertising of the specified advertising set is enabled. */
	RTK_BLE_ISO_BIG_BOARDCASTER_SYNC_STATE_CREATING_EXT_ADV_STATE_IDLE = 0x05,                    /*!< Start isochronous broadcasting when the extended advertising
                                                                                                   *   of the specified advertising set is disabled. Temporary state,
                                                                                                   *   the result has not been received yet. */
	RTK_BLE_ISO_BIG_BOARDCASTER_SYNC_STATE_WAIT_EXT_ADV_STATE_ADVERTISING = 0x06,                 /*!< Isochronous broadcasting is not started until the extended
                                                                                                   *   advertising of the specified advertising set is enabled. */
	RTK_BLE_ISO_BIG_BOARDCASTER_SYNC_STATE_CREATING = 0x07,                                       /*!< Start isochronous broadcasting when extended
                                                                                                   *   advertising and periodic advertising of the specified advertising
                                                                                                   *   set are enabled. Temporary state, the result has
                                                                                                   *   not been received yet. */
	RTK_BLE_ISO_BIG_BOARDCASTER_SYNC_STATE_BROADCASTING = 0x08,                                   /*!< Isochronous broadcasting. */
	RTK_BLE_ISO_BIG_BOARDCASTER_SYNC_STATE_TERMINATING = 0x09,                                    /*!< Terminate isochronous broadcasting. Temporary state, the result has
                                                                                                   *   not been received yet. */
} rtk_bt_le_iso_big_broadcaster_sync_state_t;

/**
 * @struct    rtk_bt_le_iso_big_broadcaster_sync_state_ind_t
 * @brief     Bluetooth LE ISO BIG broadcaster synchronization state indication.
 */
typedef struct {
	uint8_t big_handle;                               /*!< Identify the BIG.
                                                       *   Range: 0x00 to 0xEF. */
	uint8_t adv_handle;                               /*!< Handle identifying the periodic advertising set.
                                                       *   Range: 0x00 to 0xEF. */
	uint8_t sync_state;                               /*!< Synchronization state, @ref rtk_bt_le_iso_big_broadcaster_sync_state_t. */
	uint8_t cause;                                    /*!< Result code of the state change. 0 indicates success. */
	uint16_t bis_num;                                 /*!< Number of valid entries in bis_conn_handle.
                                                       *   Only valid when the BIG is in the broadcasting state. */
	uint16_t bis_conn_handle[RTK_BLE_BIS_MAX_NUM];    /*!< Connection handles of the BISes, used to set up the data path.
                                                       *   Only valid when the BIG is in the broadcasting state.
                                                       *   Range of each element: 0x0000 to 0x0EFF. */
} rtk_bt_le_iso_big_broadcaster_sync_state_ind_t;

/**
 * @struct    rtk_bt_le_iso_big_broadcaster_read_tx_sync_info_t
 * @brief     Bluetooth LE ISO TX synchronization info of a BIS.
 */
typedef struct {
	uint8_t  big_handle;                /*!< Used to identify the BIG.
                                         *   Range: 0x00 to 0xEF. */
	uint8_t  adv_handle;                /*!< Handle identifying the periodic advertising set.
                                         *   Range: 0x00 to 0xEF. */
	uint16_t packet_sequence_number;    /*!< The packet sequence number of an SDU. */
	uint32_t time_stamp;                /*!< The BIG anchor point of a transmitted SDU, in microseconds.
                                         *   Derived using the controller's free running reference clock. */
	uint32_t time_offset;               /*!< The time offset, in microseconds, that is associated with a transmitted SDU. */
} rtk_bt_le_iso_big_broadcaster_read_tx_sync_info_t;

/**
 * @struct    rtk_bt_le_iso_big_broadcaster_read_tx_sync_t
 * @brief     Bluetooth LE ISO Parameters for reading the TX synchronization info of a BIS.
 */
typedef struct {
	uint16_t bis_conn_handle;                                             /*!< Connection handle of the BIS.
                                                                           *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_le_iso_big_broadcaster_read_tx_sync_info_t *p_tx_sync_info;    /*!< Buffer that receives the TX sync info. */
} rtk_bt_le_iso_big_broadcaster_read_tx_sync_t;

/**
 * @struct    rtk_bt_le_iso_big_receiver_create_sync_param_t
 * @brief     Bluetooth LE ISO BIG synchronization parameters of the receiver.
 */
typedef struct {
	uint8_t encryption;                   /*!< The encryption mode of the BIG.
                                           *   0x00: Unencrypted.
                                           *   0x01: Encrypted. */
	uint8_t broadcast_code[16];           /*!< Used to generate the session key to decrypt payloads of an encrypted BIS. */
	uint8_t mse;                          /*!< The maximum number of subevents that the controller should use to receive data payloads in each interval for a BIS.
                                           *   0x00: The controller can schedule reception of any number of subevents up to NSE.
                                           *   0x01 to 0x1F: Maximum number of subevents that should be used to receive data payloads in each BIS event. */
	uint16_t big_sync_timeout;            /*!< The maximum permitted time between successful receptions of BIS PDUs.
                                           *   Range: 0x000A to 0x4000.
                                           *   Time = N * 10 ms.
                                           *   Time Range: 100 ms to 163.84 s. */
	uint8_t  num_bis;                     /*!< Total number of BISes to synchronize. */
	uint8_t  bis[RTK_BLE_BIS_MAX_NUM];    /*!< A list of indices corresponding to the BISes to synchronize.
                                           *   Currently ignored; BISes 1 to num_bis are synchronized. */
} rtk_bt_le_iso_big_receiver_create_sync_param_t;

/**
 * @struct    rtk_bt_le_iso_big_receiver_create_sync_t
 * @brief     Bluetooth LE ISO Parameters for synchronizing to a BIG.
 */
typedef struct {
	uint16_t sync_handle;                                            /*!< Identify the periodic advertising train.
                                                                      *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_le_iso_big_receiver_create_sync_param_t *p_sync_param;    /*!< BIG synchronization parameters. */
	uint8_t *p_big_handle;                                           /*!< Buffer that receives the handle identifying the BIG. */
} rtk_bt_le_iso_big_receiver_create_sync_t;

/**
 * @struct    rtk_bt_le_iso_big_receiver_sync_established_info_t
 * @brief     Bluetooth LE ISO BIG synchronization establishment result of the receiver.
 */
typedef struct {
	uint16_t cause;                                                                        /*!< Result of the operation. 0 indicates success. */
	uint8_t  big_handle;                                                                   /*!< The identifier of the BIG.
                                                                                            *   Range: 0x00 to 0xEF. */
	uint8_t sync_id;                                                                       /*!< Identify the periodic advertising train. */
	uint16_t sync_handle;                                                                  /*!< Sync handle identifying the periodic advertising train.
                                                                                            *   Range: 0x0000 to 0x0EFF. */
	uint32_t transport_latency_big;                                                        /*!< The actual transport latency, in microseconds. */
	uint8_t nse;                                                                           /*!< The number of subevents in each BIS event in the BIG. */
	uint8_t bn;                                                                            /*!< Burst number: the number of new payloads in each BIS event. */
	uint8_t pto;                                                                           /*!< Pre-transmission offset: the offset, in units of ISO_Interval,
                                                                                            *   used for pre-transmission of future data. */
	uint8_t irc;                                                                           /*!< Immediate repetition count.
                                                                                            *   The number of times a scheduled data packet is sent. In other words,
                                                                                            *   the number of groups that carry the data of the current BIS event. */
	uint16_t max_pdu;                                                                      /*!< Maximum size, in octets, of the payload. */
	uint16_t iso_interval;                                                                 /*!< The time between two consecutive BIG anchor points.
                                                                                            *   Range: 0x0004 to 0x0C80.
                                                                                            *   Time = N * 1.25 ms.
                                                                                            *   Time Range: 5 ms to 4 s. */
	uint8_t num_bis;                                                                       /*!< The number of BISes in the synchronized BIG. */
	rtk_bt_le_iso_big_bis_conn_handle_info_t bis_conn_handle_info[RTK_BLE_BIS_MAX_NUM];    /*!< Connection handle information of each BIS. */
} rtk_bt_le_iso_big_receiver_sync_established_info_t;

/**
 * @typedef   rtk_bt_le_iso_big_receiver_sync_state_t
 * @brief     Bluetooth LE ISO BIG receiver synchronization state.
 */
typedef enum {
	RTK_BLE_ISO_BIG_RECEIVER_SYNC_STATE_TERMINATED = 0x00,       /*!< Terminated. */
	RTK_BLE_ISO_BIG_RECEIVER_SYNC_STATE_SYNCHRONIZING = 0x01,    /*!< Start synchronizing to a BIG. Temporary state, the result has not been received yet. */
	RTK_BLE_ISO_BIG_RECEIVER_SYNC_STATE_SYNCHRONIZED = 0x02,     /*!< Synchronized to a BIG. */
	RTK_BLE_ISO_BIG_RECEIVER_SYNC_STATE_TERMINATING = 0x03,      /*!< Terminate synchronization to the BIG. Temporary state, the result has not been received yet. */
} rtk_bt_le_iso_big_receiver_sync_state_t;

/**
 * @struct    rtk_bt_le_iso_big_receiver_sync_state_ind_t
 * @brief     Bluetooth LE ISO BIG receiver synchronization state indication.
 */
typedef struct {
	uint8_t big_handle;                               /*!< Identify the BIG.
                                                       *   Range: 0x00 to 0xEF. */
	uint8_t sync_state;                               /*!< Synchronization state, @ref rtk_bt_le_iso_big_receiver_sync_state_t. */
	uint8_t sync_id;                                  /*!< Identify the periodic advertising train. */
	uint16_t sync_handle;                             /*!< Sync handle identifying the periodic advertising train.
                                                       *   Range: 0x0000 to 0x0EFF. */
	uint8_t cause;                                    /*!< Result code of the state change. 0 indicates success. */
	uint16_t bis_num;                                 /*!< Number of valid entries in bis_conn_handle.
                                                       *   Only valid when the BIG is in the synchronized state. */
	uint16_t bis_conn_handle[RTK_BLE_BIS_MAX_NUM];    /*!< Connection handles of the BISes.
                                                       *   Only valid when the BIG is in the synchronized state.
                                                       *   Range of each element: 0x0000 to 0x0EFF. */
} rtk_bt_le_iso_big_receiver_sync_state_ind_t;

/**
 * @struct    rtk_bt_le_iso_big_receiver_read_link_quality_info_t
 * @brief     Bluetooth LE ISO Link quality counters of a BIS.
 */
typedef struct {
	uint8_t  big_handle;                  /*!< The identifier of the BIG.
                                           *   Range: 0x00 to 0xEF. */
	uint8_t sync_id;                      /*!< Identify the periodic advertising train. */
	uint16_t sync_handle;                 /*!< Sync handle identifying the periodic advertising train.
                                           *   Range: 0x0000 to 0x0EFF. */
	uint32_t tx_unacked_packets;          /*!< The value shall be ignored for BIS. */
	uint32_t tx_flushed_packets;          /*!< The value shall be ignored for BIS. */
	uint32_t tx_last_subevent_packets;    /*!< The value shall be ignored for BIS. */
	uint32_t retransmitted_packets;       /*!< The value shall be ignored for BIS. */
	uint32_t crc_error_packets;           /*!< Number of packets received with a CRC error. */
	uint32_t rx_unreceived_packets;       /*!< Number of payloads that were not received by the end of the event they are associated with. */
	uint32_t duplicate_packets;           /*!< The value shall be ignored for BIS. */
} rtk_bt_le_iso_big_receiver_read_link_quality_info_t;

/**
 * @struct    rtk_bt_le_iso_big_setup_data_path_info_t
 * @brief     Bluetooth LE ISO BIG data path setup indication.
 */
typedef struct {
	uint16_t cause;                 /*!< Result of the operation. 0 indicates success. */
	uint16_t bis_conn_handle;       /*!< Connection handle of the BIS.
                                     *   Range: 0x0000 to 0x0EFF. */
	uint8_t big_handle;             /*!< Used to identify the BIG.
                                     *   Range: 0x00 to 0xEF. */
	uint8_t data_path_direction;    /*!< Direction of the data path that was set up. */
} rtk_bt_le_iso_big_setup_data_path_info_t;

/**
 * @struct    rtk_bt_le_iso_big_remove_data_path_info_t
 * @brief     Bluetooth LE ISO BIG data path removal indication.
 */
typedef struct {
	uint16_t cause;                 /*!< Result of the operation. 0 indicates success. */
	uint16_t bis_conn_handle;       /*!< Connection handle of the BIS.
                                     *   Range: 0x0000 to 0x0EFF. */
	uint8_t big_handle;             /*!< Used to identify the BIG.
                                     *   Range: 0x00 to 0xEF. */
	uint8_t data_path_direction;    /*!< Direction of the data path that was removed. */
} rtk_bt_le_iso_big_remove_data_path_info_t;

/**
 * @typedef   rtk_bt_le_iso_sync_type_t
 * @brief     Bluetooth LE ISO sync information type.
 */
typedef enum {
	RTK_BT_LE_ISO_PA_SYNC_STATE_SYNCHRONIZED,    /*!< Periodic advertising synchronization is established. */
	RTK_BT_LE_ISO_PA_SYNC_STATE_TERMINATED,      /*!< Periodic advertising synchronization is terminated. */
	RTK_BT_LE_ISO_GAP_EVT_DISCONN_IND            /*!< A connection is disconnected. */
} rtk_bt_le_iso_sync_type_t;

/**
 * @struct    rtk_bt_le_iso_sync_info_t
 * @brief     Bluetooth LE ISO sync information.
 */
typedef struct {
	rtk_bt_le_iso_sync_type_t type;          /*!< Type of the sync information; selects the valid member of info. */
	union {
		rtk_bt_le_pa_sync_ind_t pa_sync;     /*!< Periodic advertising sync state information. */
		rtk_bt_le_disconn_ind_t disc_ind;    /*!< ACL disconnection information. */
	} info;                                  /*!< Sync information payload. */
} rtk_bt_le_iso_sync_info_t;

/**
 * @struct    rtk_bt_le_iso_conn_handle_info_t
 * @brief     Bluetooth LE ISO connection handle information.
 */
typedef struct {
	uint16_t iso_handle_num;                                           /*!< Number of valid entries in iso_handle. */
	uint16_t iso_handle[RTK_BLE_BIS_MAX_NUM + RTK_BLE_CIS_MAX_NUM];    /*!< ISO connection handles.
                                                                        *   Range of each element: 0x0000 to 0x0EFF. */
} rtk_bt_le_iso_conn_handle_info_t;

/**
 * @struct    rtk_bt_le_iso_get_conn_handle_t
 * @brief     Bluetooth LE ISO get connection handle parameter.
 */
typedef struct {
	rtk_bt_le_iso_role_t role;                /*!< ISO role to query. */
	rtk_bt_le_iso_conn_handle_info_t info;    /*!< ISO connection handle information of the role. */
} rtk_bt_le_iso_get_conn_handle_t;

/**
 * @struct    rtk_bt_le_iso_big_receiver_read_link_quality_t
 * @brief     Bluetooth LE ISO BIG receiver read link quality parameter.
 */
typedef struct {
	uint16_t bis_conn_handle;                                                    /*!< Connection handle of the BIS.
                                                                                  *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_le_iso_big_receiver_read_link_quality_info_t *p_link_quality_info;    /*!< Read link quality info result. */
} rtk_bt_le_iso_big_receiver_read_link_quality_t;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  bt_le_iso BT LE ISO APIs
 * @brief     BT LE ISO related function APIs.
 * @ingroup   BT_APIs
 * @{
 */

/**
 * @brief     Configure a CIG and its CIS(es) on the central side. The result is reported asynchronously via event @ref RTK_BT_LE_ISO_EVT_CIG_START_SETTING_INFO.
 * @param[in] param: CIG and CIS configuration parameters.
 *                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_iso_cig_start_setting(rtk_bt_le_iso_set_cig_cis_param_t *param);

/**
 * @brief     Set up the isochronous data path of a CIS. The result is reported asynchronously via event @ref RTK_BT_LE_ISO_EVT_CIG_SETUP_DATA_PATH.
 * @param[in] param: CIS data path setup parameters.
 *                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_iso_cig_setup_path(rtk_bt_le_iso_setup_path_param_t *param);

/**
 * @brief     Send an ISO data packet (SDU) over an established CIS or BIS.
 * @param[in] info: ISO data send parameters.
 *                  Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_iso_data_send(rtk_bt_le_iso_data_send_info_t *info);

/**
 * @brief     Get the isochronous channel information of the specified CIS.
 * @param[in] cis_conn_handle: Connection handle of the CIS.
 *                             Range: 0x0000 to 0x0EFF.
 * @param[out] p_info: Isochronous channel information of the CIS.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_iso_cig_get_isoch_info(uint16_t cis_conn_handle, rtk_bt_le_iso_cis_channel_info_t *p_info);

/**
 * @brief     Associate a CIS with an ACL connection and request creation of the CIS(es) of the specified CIG. The result is reported asynchronously
 *            via event @ref RTK_BT_LE_ISO_EVT_CIG_CIS_ESTABLISHED_INFO.
 * @param[in] cis_id: Identifier of the CIS to associate with the ACL connection.
 * @param[in] cig_id: Identifier of the CIG.
 * @param[in] conn_handle: Connection handle of the ACL connection.
 *                         Range: 0x0000 to 0x0EFF.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_iso_cig_initiator_create_cis_by_cig_id(uint8_t cis_id, uint8_t cig_id, uint16_t conn_handle);

/**
 * @brief     Associate a CIS with an ACL connection and request creation of one or more CISes specified by their CIS connection handles. The result is reported
 *            asynchronously via event @ref RTK_BT_LE_ISO_EVT_CIG_CIS_ESTABLISHED_INFO.
 * @param[in] cis_id: Identifier of the CIS to associate with the ACL connection.
 * @param[in] cig_id: Identifier of the CIG.
 * @param[in] cis_count: Number of CISes to create, equal to the number of entries in p_cis_conn_handle.
 * @param[in] conn_handle: Connection handle of the ACL connection.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] p_cis_conn_handle: Array of connection handles of the CISes to create.
 *                               Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_iso_cig_initiator_create_cis_by_cis_conn_handle(uint8_t cis_id, uint8_t cig_id, uint8_t cis_count, uint16_t conn_handle,
																   uint16_t *p_cis_conn_handle);

/**
 * @brief     Terminate a CIS connection. The result is reported asynchronously via event @ref RTK_BT_LE_ISO_EVT_CIG_DISCONNECT_INFO.
 * @param[in] cis_conn_handle: Connection handle of the CIS.
 *                             Range: 0x0000 to 0x0EFF.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_iso_cig_disconnect(uint16_t cis_conn_handle);

/**
 * @brief     Remove a CIG on the central side.
 * @param[in] cig_id: Identifier of the CIG.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_iso_cig_initiator_remove_cig(uint8_t cig_id);

/**
 * @brief     Accept a CIS request received by the peripheral. The result is reported asynchronously via event @ref RTK_BT_LE_ISO_EVT_CIG_CIS_ESTABLISHED_INFO.
 * @param[in] cis_conn_handle: Connection handle of the CIS.
 *                             Range: 0x0000 to 0x0EFF.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_iso_cig_acceptor_accept_cis(uint16_t cis_conn_handle);

/**
 * @brief     Reject a CIS request received by the peripheral.
 * @param[in] cis_conn_handle: Connection handle of the CIS.
 *                             Range: 0x0000 to 0x0EFF.
 * @param[in] reason: Reason for rejecting the CIS request, an error code other than success.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_iso_cig_acceptor_reject_cis(uint16_t cis_conn_handle, uint8_t reason);

/**
 * @brief     Configure how the peripheral handles an incoming CIS request.
 * @param[in] cis_req_action: Action to take when a CIS request is received.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The default action is @ref RTK_BLE_ISO_ACCEPTOR_CIS_REQ_ACTION_PENDING.
 *            - When the action is pending, the application is notified with event @ref RTK_BT_LE_ISO_EVT_CIG_ACCEPTOR_REQUEST_CIS_IND and must then call
 *              @ref rtk_bt_le_iso_cig_acceptor_accept_cis or @ref rtk_bt_le_iso_cig_acceptor_reject_cis.
 */
uint16_t rtk_bt_le_iso_cig_acceptor_config_cis_req_action(rtk_bt_le_iso_cig_acceptor_config_cis_req_action_t cis_req_action);

/**
 * @brief     Register the CIG acceptor callback to the lower stack, so that CIS requests from the central are reported to the application.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_iso_cig_acceptor_register_callback(void);

/**
 * @brief     Remove the data path(s) of a CIS. The result is reported asynchronously via event @ref RTK_BT_LE_ISO_EVT_CIG_REMOVE_DATA_PATH.
 * @param[in] cis_conn_handle: Connection handle of the CIS.
 *                             Range: 0x0000 to 0x0EFF.
 * @param[in] data_path_direction: Direction(s) of the data path to remove.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_iso_cig_remove_path(uint16_t cis_conn_handle, rtk_bt_le_iso_data_path_direction_flag_t data_path_direction);

/**
 * @brief     Read the timestamp and time offset of the last transmitted SDU on a CIS, together with its packet sequence number.
 * @param[in] cis_conn_handle: Connection handle of the CIS.
 *                             Range: 0x0000 to 0x0EFF.
 * @param[out] p_tx_sync_info: TX synchronization information of the CIS.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_iso_cig_read_iso_tx_sync(uint16_t cis_conn_handle, rtk_bt_le_iso_cig_read_iso_tx_sync_info_t *p_tx_sync_info);

/**
 * @brief     Read the link quality counters of a CIS.
 * @param[in] cis_conn_handle: Connection handle of the CIS.
 *                             Range: 0x0000 to 0x0EFF.
 * @param[out] p_link_quality_info: Link quality information of the CIS.
 *                                  Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_iso_cig_read_link_quality(uint16_t cis_conn_handle, rtk_bt_le_iso_cig_read_link_quality_info_t *p_link_quality_info);

/**
 * @brief     Set up the isochronous data path of a BIS. The result is reported asynchronously via event @ref RTK_BT_LE_ISO_EVT_BIG_SETUP_DATA_PATH.
 * @param[in] param: BIS data path setup parameters.
 *                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_iso_big_setup_path(rtk_bt_le_iso_setup_path_param_t *param);

/**
 * @brief     Remove the data path(s) of a BIS. The result is reported asynchronously via event @ref RTK_BT_LE_ISO_EVT_BIG_REMOVE_DATA_PATH.
 * @param[in] bis_conn_handle: Connection handle of the BIS.
 *                             Range: 0x0000 to 0x0EFF.
 * @param[in] data_path_direction: Direction(s) of the data path to remove.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_iso_big_remove_path(uint16_t bis_conn_handle, rtk_bt_le_iso_data_path_direction_flag_t data_path_direction);

/**
 * @brief     Create a BIG as broadcaster on the specified periodic advertising set. The result is reported asynchronously
 *            via event @ref RTK_BT_LE_ISO_EVT_BIG_BROADCASTER_CREATE_CMPL_INFO.
 * @param[in] adv_handle: Handle of the periodic advertising set that carries the BIG.
 *                        Range: 0x00 to 0xEF.
 * @param[in] big_param: BIG creation parameters.
 *                       Must not be NULL.
 * @param[out] big_handle: Handle of the created BIG.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_iso_big_broadcaster_create(uint8_t adv_handle, rtk_bt_le_iso_big_broadcaster_param_t *big_param, uint8_t *big_handle);

/**
 * @brief     Terminate a BIG created as broadcaster. The result is reported asynchronously via event @ref RTK_BT_LE_ISO_EVT_BIG_BROADCASTER_SYNC_STATE_IND.
 * @param[in] big_handle: Handle of the BIG.
 *                        Range: 0x00 to 0xEF.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_iso_big_broadcaster_terminate(uint8_t big_handle);

/**
 * @brief     Read the timestamp and time offset of the last transmitted SDU on a BIS, together with its packet sequence number.
 * @param[in] bis_conn_handle: Connection handle of the BIS.
 *                             Range: 0x0000 to 0x0EFF.
 * @param[out] p_tx_sync_info: TX synchronization information of the BIS.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_iso_big_broadcaster_read_tx_sync(uint16_t bis_conn_handle, rtk_bt_le_iso_big_broadcaster_read_tx_sync_info_t *p_tx_sync_info);

/**
 * @brief     Synchronize to a BIG announced by a synchronized periodic advertising train. The result is reported asynchronously
 *            via event @ref RTK_BT_LE_ISO_EVT_BIG_RECEIVER_SYNC_ESTABLISHED_INFO.
 * @param[in] sync_handle: Handle of the periodic advertising sync.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] sync_param: BIG synchronization parameters.
 *                        Must not be NULL.
 * @param[out] p_big_handle: Handle of the BIG.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_iso_big_receiver_create_sync(uint16_t sync_handle, rtk_bt_le_iso_big_receiver_create_sync_param_t *sync_param, uint8_t *p_big_handle);

/**
 * @brief     Terminate synchronization to a BIG. The result is reported asynchronously via event @ref RTK_BT_LE_ISO_EVT_BIG_RECEIVER_SYNC_STATE_IND.
 * @param[in] big_handle: Handle of the BIG.
 *                        Range: 0x00 to 0xEF.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_iso_big_receiver_terminate_sync(uint8_t big_handle);

/**
 * @brief     Read the link quality counters of a BIS.
 * @param[in] bis_conn_handle: Connection handle of the BIS.
 *                             Range: 0x0000 to 0x0EFF.
 * @param[out] p_link_quality_info: Link quality information of the BIS.
 *                                  Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_iso_big_receiver_read_iso_link_quality(uint16_t bis_conn_handle, rtk_bt_le_iso_big_receiver_read_link_quality_info_t *p_link_quality_info);

/**
 * @brief     Notify the ISO module of a periodic advertising sync state change or an ACL disconnection, so that it can update its internal ISO state.
 * @param[in] p_info: Sync information to notify.
 *                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_iso_sync_info(rtk_bt_le_iso_sync_info_t *p_info);

/**
 * @brief     Get the active ISO connection handles of the specified ISO role.
 * @param[in] role: ISO role to query.
 * @param[out] p_info: ISO connection handle information.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_iso_get_iso_conn_handle(rtk_bt_le_iso_role_t role, rtk_bt_le_iso_conn_handle_info_t *p_info);

/**
 * @}
 */

#endif

#ifdef __cplusplus
}
#endif

#endif  /* __RTK_BT_LE_ISO_H__ */
