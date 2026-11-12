/**
 * @file      rtk_bt_avrcp.h
 * @brief     Bluetooth AVRCP type and function definition.
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_AVRCP_H__
#define __RTK_BT_AVRCP_H__

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @def       RTK_BT_DEFAULT_ABSOLUTE_VOLUME
 * @brief     Default absolute volume reported to the remote device before any volume change.
 */
#define RTK_BT_DEFAULT_ABSOLUTE_VOLUME 0x64

/**
 * @def       RTK_BT_AVRCP_ELEM_ATTR_MAX_NUM
 * @brief     Maximum number of media element attributes carried in one element attribute response.
 */
#define RTK_BT_AVRCP_ELEM_ATTR_MAX_NUM 8

/************************************** Data Types *************************************/

/**
 * @typedef   rtk_bt_avrcp_play_status_t
 * @brief     Bluetooth AVRCP playback status.
 */
typedef enum {
	RTK_BT_AVRCP_STATUS_STOPPED = 0x00,     /*!< Playback is stopped. */
	RTK_BT_AVRCP_STATUS_PLAYING = 0x01,     /*!< Playback is in progress. */
	RTK_BT_AVRCP_STATUS_PAUSED = 0x02,      /*!< Playback is paused. */
	RTK_BT_AVRCP_STATUS_FWD_SEEK = 0x03,    /*!< Seeking forward. */
	RTK_BT_AVRCP_STATUS_REV_SEEK = 0x04,    /*!< Seeking backward. */
	RTK_BT_AVRCP_STATUS_FAST_FWD = 0x05,    /*!< Fast forward in progress. */
	RTK_BT_AVRCP_STATUS_REWIND = 0x06,      /*!< Rewind in progress. */
	RTK_BT_AVRCP_STATUS_ERROR = 0xFF,       /*!< Playback status error. */
} rtk_bt_avrcp_play_status_t;

/**
 * @typedef   rtk_bt_avrcp_element_attr_t
 * @brief     Bluetooth AVRCP media element attribute ID.
 */
typedef enum {
	RTK_BT_AVRCP_ELEM_ATTR_TITLE = 0x01,                /*!< Title of the media. */
	RTK_BT_AVRCP_ELEM_ATTR_ARTIST = 0x02,               /*!< Artist name. */
	RTK_BT_AVRCP_ELEM_ATTR_ALBUM = 0x03,                /*!< Album name. */
	RTK_BT_AVRCP_ELEM_ATTR_TRACK = 0x04,                /*!< Track number of the media. */
	RTK_BT_AVRCP_ELEM_ATTR_TOTAL_TRACK = 0x05,          /*!< Total number of tracks of the album. */
	RTK_BT_AVRCP_ELEM_ATTR_GENRE = 0x06,                /*!< Genre of the media. */
	RTK_BT_AVRCP_ELEM_ATTR_PLAYING_TIME = 0x07,         /*!< Playing time of the media in ms. */
	RTK_BT_AVRCP_ELEM_ATTR_DEFAULT_COVER_ART = 0x08,    /*!< Default cover art handle of the media. */
} rtk_bt_avrcp_element_attr_t;

/**
 * @typedef   rtk_bt_avrcp_player_app_setting_attr_t
 * @brief     Bluetooth AVRCP player application setting attribute ID.
 */
typedef enum {
	RTK_BT_PLAYER_EQ = 0x01,             /*!< Equalizer setting. */
	RTK_BT_PLAYER_REPEAT_MODE = 0x02,    /*!< Repeat mode setting. */
	RTK_BT_PLAYER_SHUFFLE = 0x03,        /*!< Shuffle mode setting. */
	RTK_BT_PLAYER_SCAN = 0x04,           /*!< Scan mode setting. */
} rtk_bt_avrcp_player_app_setting_attr_t;

/**
 * @typedef   rtk_bt_avrcp_player_eq_t
 * @brief     Bluetooth AVRCP equalizer setting value.
 */
typedef enum {
	RTK_BT_PLAYER_EQ_OFF = 0x01,    /*!< Equalizer is off. */
	RTK_BT_PLAYER_EQ_ON = 0x02,     /*!< Equalizer is on. */
} rtk_bt_avrcp_player_eq_t;

/**
 * @typedef   rtk_bt_avrcp_player_repeat_mode_t
 * @brief     Bluetooth AVRCP repeat mode setting value.
 */
typedef enum {
	RTK_BT_PLAYER_REPEAT_MODE_OFF = 0x01,        /*!< Repeat is off. */
	RTK_BT_PLAYER_SINGLE_TRACK_REPEAT = 0x02,    /*!< Repeat the current track. */
	RTK_BT_PLAYER_ALL_TRACK_REPEAT = 0x03,       /*!< Repeat all tracks. */
	RTK_BT_PLAYER_GROUP_REPEAT = 0x04,           /*!< Repeat the current group. */
} rtk_bt_avrcp_player_repeat_mode_t;

/**
 * @typedef   rtk_bt_avrcp_player_shuffle_mode_t
 * @brief     Bluetooth AVRCP shuffle mode setting value.
 */
typedef enum {
	RTK_BT_PLAYER_SHUFFLE_OFF = 0x01,          /*!< Shuffle is off. */
	RTK_BT_PLAYER_ALL_TRACK_SHUFFLE = 0x02,    /*!< Shuffle all tracks. */
	RTK_BT_PLAYER_GROUP_SHUFFLE = 0x03,        /*!< Shuffle the tracks in the current group. */
} rtk_bt_avrcp_player_shuffle_mode_t;

/**
 * @typedef   rtk_bt_avrcp_player_scan_t
 * @brief     Bluetooth AVRCP scan mode setting value.
 */
typedef enum {
	RTK_BT_PLAYER_SCAN_OFF = 0x01,          /*!< Scan is off. */
	RTK_BT_PLAYER_ALL_TRACK_SCAN = 0x02,    /*!< Scan all tracks. */
	RTK_BT_PLAYER_GROUP_SCAN = 0x03,        /*!< Scan the tracks in the current group. */
} rtk_bt_avrcp_player_scan_t;

/**
 * @struct    rtk_bt_avrcp_play_status_change_req_t
 * @brief     Bluetooth AVRCP play status change request parameters.
 */
typedef struct {
	uint8_t bd_addr[6];                   /*!< Bluetooth address of the remote device. */
	rtk_bt_avrcp_play_status_t status;    /*!< Playback status. */
} rtk_bt_avrcp_play_status_change_req_t;

/**
 * @struct    rtk_bt_avrcp_play_status_change_reg_rsp_t
 * @brief     Bluetooth AVRCP play status change register response parameters.
 */
typedef struct {
	uint8_t bd_addr[6];                   /*!< Bluetooth address of the remote device. */
	rtk_bt_avrcp_play_status_t status;    /*!< Playback status. */
} rtk_bt_avrcp_play_status_change_reg_rsp_t;

/**
 * @struct    rtk_bt_avrcp_app_setting_values_t
 * @brief     Bluetooth AVRCP application setting values list request parameters.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Bluetooth address of the remote device. */
	uint8_t attr_id;       /*!< Application setting attribute ID whose values are queried.
                            *   Value: @ref rtk_bt_avrcp_player_app_setting_attr_t. */
} rtk_bt_avrcp_app_setting_values_t;

/**
 * @struct    rtk_bt_avrcp_app_setting_value_get_t
 * @brief     Bluetooth AVRCP application setting value get request parameters.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Bluetooth address of the remote device. */
	uint8_t attr_num;      /*!< Number of attribute IDs in attr_list. */
	uint8_t *attr_list;    /*!< Attribute ID list, each element is an application setting attribute ID.
                            *   Value of each element: @ref rtk_bt_avrcp_player_app_setting_attr_t. */
} rtk_bt_avrcp_app_setting_value_get_t;

/**
 * @struct    rtk_bt_avrcp_app_setting_value_set_t
 * @brief     Bluetooth AVRCP application setting value set request parameters.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Bluetooth address of the remote device. */
	uint8_t attr_id;       /*!< Application setting attribute ID to set.
                            *   Value: @ref rtk_bt_avrcp_player_app_setting_attr_t. */
	uint8_t setting;       /*!< Setting value to apply to the attribute selected by attr_id.
                            *   Value: @ref rtk_bt_avrcp_player_eq_t, @ref rtk_bt_avrcp_player_repeat_mode_t, @ref rtk_bt_avrcp_player_shuffle_mode_t or
                            *   @ref rtk_bt_avrcp_player_scan_t, matching attr_id. */
} rtk_bt_avrcp_app_setting_value_set_t;

/**
 * @struct    rtk_bt_avrcp_absolute_volume_set_t
 * @brief     Bluetooth AVRCP absolute volume set parameters.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Bluetooth address of the remote device. */
	uint8_t volume;        /*!< Absolute volume.
                            *   Range: 0x00 to 0x7F. */
} rtk_bt_avrcp_absolute_volume_set_t;

/**
 * @struct    rtk_bt_avrcp_volume_change_req_t
 * @brief     Bluetooth AVRCP volume change request parameters.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Bluetooth address of the remote device. */
	uint8_t volume;        /*!< Absolute volume.
                            *   Range: 0x00 to 0x7F. */
} rtk_bt_avrcp_volume_change_req_t;

/**
 * @struct    rtk_bt_avrcp_get_element_attr_req_t
 * @brief     Bluetooth AVRCP get element attributes request parameters.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Bluetooth address of the remote device. */
	uint8_t num;           /*!< Number of attribute IDs in p_attr. */
	uint8_t *p_attr;       /*!< Attribute ID list to retrieve.
                            *   Value of each element: @ref rtk_bt_avrcp_element_attr_t. */
} rtk_bt_avrcp_get_element_attr_req_t;

/**
 * @struct    rtk_bt_avrcp_element_attr
 * @brief     Bluetooth AVRCP media element attribute entry.
 */
typedef struct {
	uint32_t attribute_id;        /*!< Attribute ID.
                                   *   Value: @ref rtk_bt_avrcp_element_attr_t. */
	uint16_t character_set_id;    /*!< Character set ID of the attribute text. */
	uint16_t length;              /*!< Length of the attribute text in octets. */
	uint8_t *p_buf;               /*!< Attribute text buffer. */
} rtk_bt_avrcp_element_attr;

/**
 * @struct    rtk_bt_avrcp_element_attr_info_t
 * @brief     Bluetooth AVRCP media element attributes information.
 */
typedef struct {
	uint8_t bd_addr[6];                                                /*!< Bluetooth address of the remote device. */
	uint8_t state;                                                     /*!< Result state of the get element attributes response.
                                                                        *   0: Succeed.
                                                                        *   Others: Failed. */
	uint8_t num_of_attr;                                               /*!< Number of valid entries in attr.
                                                                        *   Range: 0 to @ref RTK_BT_AVRCP_ELEM_ATTR_MAX_NUM. */
	rtk_bt_avrcp_element_attr attr[RTK_BT_AVRCP_ELEM_ATTR_MAX_NUM];    /*!< Attribute entries. */
} rtk_bt_avrcp_element_attr_info_t;

/**
 * @struct    rtk_bt_avrcp_app_setting_attrs_list_t
 * @brief     Bluetooth AVRCP application setting attributes list.
 */
typedef struct {
	uint8_t bd_addr[6];     /*!< Bluetooth address of the remote device. */
	uint8_t state;          /*!< Result state of the application setting attributes list response.
                             *   0: Succeed.
                             *   Others: Failed. */
	uint8_t num_of_attr;    /*!< Number of attribute IDs in p_attr_id. */
	uint8_t *p_attr_id;     /*!< Application setting attribute ID list.
                             *   Value of each element: @ref rtk_bt_avrcp_player_app_setting_attr_t. */
} rtk_bt_avrcp_app_setting_attrs_list_t;

/**
 * @struct    rtk_bt_avrcp_app_setting_values_list_t
 * @brief     Bluetooth AVRCP application setting values list.
 */
typedef struct {
	uint8_t bd_addr[6];      /*!< Bluetooth address of the remote device. */
	uint8_t state;           /*!< Result state of the application setting values list response.
                              *   0: Succeed.
                              *   Others: Failed. */
	uint8_t num_of_value;    /*!< Number of values in p_value. */
	uint8_t *p_value;        /*!< Value list supported by the queried application setting attribute. */
} rtk_bt_avrcp_app_setting_values_list_t;

/**
 * @struct    rtk_bt_avrcp_app_setting_t
 * @brief     Bluetooth AVRCP application setting attribute and value pair.
 */
typedef struct {
	uint8_t attr;     /*!< Application setting attribute ID.
                       *   Value: @ref rtk_bt_avrcp_player_app_setting_attr_t. */
	uint8_t value;    /*!< Current value of the attribute. */
} rtk_bt_avrcp_app_setting_t;

/**
 * @struct    rtk_bt_avrcp_app_setting_get_rsp_t
 * @brief     Bluetooth AVRCP application setting get response.
 */
typedef struct {
	uint8_t bd_addr[6];                           /*!< Bluetooth address of the remote device. */
	uint8_t state;                                /*!< Result state of the application setting get response.
                                                   *   0: Succeed.
                                                   *   Others: Failed. */
	uint8_t num_of_attr;                          /*!< Number of entries in p_app_setting. */
	rtk_bt_avrcp_app_setting_t *p_app_setting;    /*!< Attribute and value pair list. */
} rtk_bt_avrcp_app_setting_get_rsp_t;

/**
 * @struct    rtk_bt_avrcp_cover_art_data_ind_t
 * @brief     Bluetooth AVRCP cover art data indication.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Bluetooth address of the remote device. */
	uint8_t *p_data;       /*!< Cover art data fragment. */
	uint16_t data_len;     /*!< Length of the data fragment in octets. */
	bool data_end;         /*!< Whether this fragment is the last one of the cover art.
                            *   - true: Last fragment.
                            *   - false: More fragments follow. */
} rtk_bt_avrcp_cover_art_data_ind_t;

/**
 * @struct    rtk_bt_avrcp_volume_t
 * @brief     Bluetooth AVRCP volume status.
 */
typedef struct {
	uint8_t volume;    /*!< Current play volume.
                        *   Range: 0x00 to 0x7F. */
} rtk_bt_avrcp_volume_t;

/**
 * @struct    rtk_bt_avrcp_sub_event_t
 * @brief     Bluetooth AVRCP play status change notification.
 */
typedef struct {
	uint8_t avrcp_play_status;    /*!< New play status.
                                   *   Value: @ref rtk_bt_avrcp_play_status_t. */
} rtk_bt_avrcp_sub_event_t;

/**
 * @struct    rtk_bt_avrcp_track_changed_t
 * @brief     Bluetooth AVRCP track changed notification.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Bluetooth address of the remote device. */
	uint64_t track_id;     /*!< Identifier of the new playing track. */
} rtk_bt_avrcp_track_changed_t;

/**
 * @struct    rtk_bt_avrcp_digital_interface_command_t
 * @brief     Bluetooth AVRCP pass-through command.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Bluetooth address of the remote device. */
} rtk_bt_avrcp_digital_interface_command_t;

/**
 * @struct    rtk_bt_avrcp_conn_ind_t
 * @brief     Bluetooth AVRCP connection indication.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Bluetooth address of the remote device. */
} rtk_bt_avrcp_conn_ind_t;

/**
 * @struct    rtk_bt_avrcp_conn_cmpl_t
 * @brief     Bluetooth AVRCP connection completion.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Bluetooth address of the remote device. */
} rtk_bt_avrcp_conn_cmpl_t;

/**
 * @struct    rtk_bt_avrcp_disconn_cmpl_t
 * @brief     Bluetooth AVRCP disconnection completion.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Bluetooth address of the remote device. */
} rtk_bt_avrcp_disconn_cmpl_t;

/**
 * @struct    rtk_bt_avrcp_get_play_status_rsp_t
 * @brief     Bluetooth AVRCP get play status response.
 */
typedef struct {
	uint8_t bd_addr[6];      /*!< Bluetooth address of the remote device. */
	uint8_t state;           /*!< Result state of the get play status response.
                              *   0: Succeed.
                              *   Others: Failed. */
	uint8_t play_status;     /*!< Playback status.
                              *   Value: @ref rtk_bt_avrcp_play_status_t. */
	uint32_t length_ms;      /*!< Total length of the playing media in ms. */
	uint32_t position_ms;    /*!< Current playing position in ms. */
} rtk_bt_avrcp_get_play_status_rsp_t;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  bt_avrcp BT AVRCP APIs
 * @brief     BT AVRCP related function APIs.
 * @ingroup   BT_APIs
 * @{
 */

/**
 * @brief     Connect the AVRCP channel to a remote device.
 * @param[in] bd_addr: Bluetooth address of the remote device.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_avrcp_connect(uint8_t *bd_addr);

/**
 * @brief     Disconnect the AVRCP channel from a remote device.
 * @param[in] bd_addr: Bluetooth address of the remote device.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_avrcp_disconnect(uint8_t *bd_addr);

/**
 * @brief     Send an AVRCP play status change request to remote CT device.
 * @param[in] bt_addr: Bluetooth address of the remote device.
 *                     Must not be NULL.
 * @param[in] play_status: Playback status to notify.
 *                         Value: @ref rtk_bt_avrcp_play_status_t.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The remote controller shall have registered for play status change notification, otherwise this API fails with @ref RTK_BT_FAIL.
 */
uint16_t rtk_bt_avrcp_play_status_change_req(uint8_t *bt_addr, uint8_t play_status);

/**
 * @brief     Send an AVRCP play status change response after received register notification play status change command.
 * @param[in] bd_addr: Bluetooth address of the remote device.
 *                     Must not be NULL.
 * @param[in] play_status: Playback status to notify.
 *                         Value: @ref rtk_bt_avrcp_play_status_t.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_avrcp_play_status_change_reg_rsp(uint8_t *bd_addr, uint8_t play_status);

/**
 * @brief     Send a play command to the remote device.
 * @param[in] bd_addr: Bluetooth address of the remote device.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_avrcp_play(uint8_t *bd_addr);

/**
 * @brief     Send a pause command to the remote device.
 * @param[in] bd_addr: Bluetooth address of the remote device.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_avrcp_pause(uint8_t *bd_addr);

/**
 * @brief     Send a stop command to the remote device.
 * @param[in] bd_addr: Bluetooth address of the remote device.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_avrcp_stop(uint8_t *bd_addr);

/**
 * @brief     Send a forward (next track) command to the remote device.
 * @param[in] bd_addr: Bluetooth address of the remote device.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_avrcp_forward(uint8_t *bd_addr);

/**
 * @brief     Send a backward (previous track) command to the remote device.
 * @param[in] bd_addr: Bluetooth address of the remote device.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_avrcp_backward(uint8_t *bd_addr);

/**
 * @brief     Send a rewind start command to the remote device.
 * @param[in] bd_addr: Bluetooth address of the remote device.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_avrcp_rewind_start(uint8_t *bd_addr);

/**
 * @brief     Send a rewind stop command to the remote device.
 * @param[in] bd_addr: Bluetooth address of the remote device.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_avrcp_rewind_stop(uint8_t *bd_addr);

/**
 * @brief     Send a fast forward start command to the remote device.
 * @param[in] bd_addr: Bluetooth address of the remote device.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_avrcp_fast_forward_start(uint8_t *bd_addr);

/**
 * @brief     Send a fast forward stop command to the remote device.
 * @param[in] bd_addr: Bluetooth address of the remote device.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_avrcp_fast_forward_stop(uint8_t *bd_addr);

/**
 * @brief     Set the absolute volume of the remote device.
 * @param[in] bd_addr: Bluetooth address of the remote device.
 *                     Must not be NULL.
 * @param[in] volume: Absolute volume.
 *                    Range: 0x00 to 0x7F.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_avrcp_absolute_volume_set(uint8_t *bd_addr, uint8_t volume);

/**
 * @brief     Send an AVRCP volume change request to remote TG device.
 * @param[in] bd_addr: Bluetooth address of the remote device.
 *                     Must not be NULL.
 * @param[in] volume: Absolute volume.
 *                    Range: 0x00 to 0x7F.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_avrcp_volume_change_req(uint8_t *bd_addr, uint8_t volume);

/**
 * @brief     Send an AVRCP get element attribute request.
 * @param[in] bd_addr: Bluetooth address of the remote device.
 *                     Must not be NULL.
 * @param[in] num: Number of attribute IDs in p_attr.
 *                 Range: 1 to @ref RTK_BT_AVRCP_ELEM_ATTR_MAX_NUM.
 * @param[in] p_attr: Attribute ID list to retrieve.
 *                    Value of each element: @ref rtk_bt_avrcp_element_attr_t.
 *                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_avrcp_get_element_attr(uint8_t *bd_addr, uint8_t num, uint8_t *p_attr);

/**
 * @brief     Get the player application setting attributes supported by the remote device.
 *            The result is reported asynchronously via event @ref RTK_BT_AVRCP_EVT_APP_SETTING_ATTRS_LIST_RSP.
 * @param[in] bd_addr: Bluetooth address of the remote device.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_avrcp_app_setting_attrs_list(uint8_t *bd_addr);

/**
 * @brief     Get the values supported by a player application setting attribute of the remote device.
 *            The result is reported asynchronously via event @ref RTK_BT_AVRCP_EVT_APP_SETTING_VALUES_LIST_RSP.
 * @param[in] bd_addr: Bluetooth address of the remote device.
 *                     Must not be NULL.
 * @param[in] attr_id: Application setting attribute ID to query.
 *                     Value: @ref rtk_bt_avrcp_player_app_setting_attr_t.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_avrcp_app_setting_values_list(uint8_t *bd_addr, uint8_t attr_id);

/**
 * @brief     Get the current values of player application setting attributes from the remote device.
 *            The result is reported asynchronously via event @ref RTK_BT_AVRCP_EVT_APP_SETTING_GET_RSP.
 * @param[in] bd_addr: Bluetooth address of the remote device.
 *                     Must not be NULL.
 * @param[in] attr_num: Number of attribute IDs in attr_list.
 * @param[in] attr_list: Application setting attribute ID list.
 *                      Value of each element: @ref rtk_bt_avrcp_player_app_setting_attr_t.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_avrcp_app_setting_value_get(uint8_t *bd_addr, uint8_t attr_num, uint8_t *attr_list);

/**
 * @brief     Set the value of a player application setting attribute of the remote device.
 * @param[in] bd_addr: Bluetooth address of the remote device.
 *                     Must not be NULL.
 * @param[in] attr_id: Application setting attribute ID to set.
 *                     Value: @ref rtk_bt_avrcp_player_app_setting_attr_t.
 * @param[in] setting: Setting value to apply to the attribute selected by attr_id.
 *                     Value: @ref rtk_bt_avrcp_player_eq_t, @ref rtk_bt_avrcp_player_repeat_mode_t, @ref rtk_bt_avrcp_player_shuffle_mode_t
 *                     or @ref rtk_bt_avrcp_player_scan_t.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_avrcp_app_setting_value_set(uint8_t *bd_addr, uint8_t attr_id, uint8_t setting);

/**
 * @brief     Get the playback status of the remote device. The result is reported asynchronously via event @ref RTK_BT_AVRCP_EVT_GET_PLAY_STATUS_RSP_EVENT.
 * @param[in] bd_addr: Bluetooth address of the remote device.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_avrcp_get_play_status_req(uint8_t *bd_addr);

/**
 * @brief     Connect the AVRCP cover art channel to a remote device.
 * @param[in] bd_addr: Bluetooth address of the remote device.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_avrcp_cover_art_connect(uint8_t *bd_addr);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_AVRCP_H__ */
