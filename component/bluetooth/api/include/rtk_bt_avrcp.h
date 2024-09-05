/**
 * @file      rtk_bt_avrcp.h
 * @author    benling_xu@realsil.com.cn
 * @brief     Bluetooth LE GAP part type and function deffinition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_AVRCP_H__
#define __RTK_BT_AVRCP_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define RTK_BT_DEFAULT_ABSOLUTE_VOLUME 0x64
/* ------------------------------- Data Types ------------------------------- */

/**
 * @typedef   rtk_bt_avrcp_play_status_t
 * @brief     avrcp play status
 */
typedef enum {
	RTK_BT_AVRCP_STATUS_STOPPED = 0x00,                    /*!< stop */
	RTK_BT_AVRCP_STATUS_PLAYING = 0x01,                    /*!< play */
	RTK_BT_AVRCP_STATUS_PAUSED = 0x02,                     /*!< pause */
	RTK_BT_AVRCP_STATUS_FWD_SEEK = 0x03,                   /*!< forward seek */
	RTK_BT_AVRCP_STATUS_REV_SEEK = 0x04,                   /*!< rewind seek */
	RTK_BT_AVRCP_STATUS_FAST_FWD = 0x05,                   /*!< fast forward */
	RTK_BT_AVRCP_STATUS_REWIND = 0x06,                     /*!< rewind */
	RTK_BT_AVRCP_STATUS_ERROR = 0xFF,                      /*!< error */
} rtk_bt_avrcp_play_status_t;

/**
 * @struct    rtk_bt_avrcp_absolute_volume_set_t
 * @brief     Bluetooth AVRCP absolute volume set.
 */
typedef struct {
	uint8_t bd_addr[6];
	uint8_t volume;
} rtk_bt_avrcp_absolute_volume_set_t;

/**
 * @struct    rtk_bt_avrcp_volume_change_req_t
 * @brief     Bluetooth AVRCP volume change request.
 */
typedef struct {
	uint8_t bd_addr[6];
	uint8_t volume;
} rtk_bt_avrcp_volume_change_req_t;

/**
 * @struct    rtk_bt_avrcp_volume_t
 * @brief     Bluetooth AVRCP volume status.
 */
typedef struct {
	uint8_t volume;                                 /*!< indicate play volume */
} rtk_bt_avrcp_volume_t;

/**
 * @struct    rtk_bt_avrcp_sub_event_t
 * @brief     Bluetooth AVRCP play status.
 */
typedef struct {
	uint8_t avrcp_play_status;                      /*!< indicate play status */
} rtk_bt_avrcp_sub_event_t;

/**
 * @struct    rtk_bt_avrcp_digital_interface_command_t
 * @brief     Bluetooth AVRCP passthrough command.
 */
typedef struct {
	uint8_t bd_addr[6];                     /*!< address */
} rtk_bt_avrcp_digital_interface_command_t;

/**
 * @struct    rtk_bt_avrcp_conn_ind_t
 * @brief     avrcp connection indication.
 */
typedef struct {
	uint8_t bd_addr[6];                             /*!< address */
} rtk_bt_avrcp_conn_ind_t;

/**
 * @struct    rtk_bt_avrcp_conn_cmpl_t
 * @brief     avrcp connection completion indication.
 */
typedef struct {
	uint8_t bd_addr[6];                             /*!< address */
} rtk_bt_avrcp_conn_cmpl_t;

/**
 * @struct    rtk_bt_avrcp_disconn_cmpl_t
 * @brief     avrcp disconnection completion indication.
 */
typedef struct {
	uint8_t bd_addr[6];                             /*!< address */
} rtk_bt_avrcp_disconn_cmpl_t;

/* ------------------------------ Functions Declaration ------------------------------ */
/**
 * @defgroup  bt_avrcp BT AVRCP APIs
 * @brief     BT AVRCP related function APIs
 * @ingroup   BT_APIs
 * @{
 */

/**
 * @brief     send avrcp connect request.
 * @param[in] bd_addr: bt address
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_avrcp_connect(uint8_t *bd_addr);

/**
 * @brief     send avrcp disconnect request.
 * @param[in] bd_addr: bt address
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_avrcp_disconnect(uint8_t *bd_addr);

/**
 * @brief     send avrcp play request.
 * @param[in] bd_addr: bt address
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_avrcp_play(uint8_t *bd_addr);

/**
 * @brief     send avrcp pause request.
 * @param[in] bd_addr: bt address
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_avrcp_pause(uint8_t *bd_addr);

/**
 * @brief     send avrcp stop request.
 * @param[in] bd_addr: bt address
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_avrcp_stop(uint8_t *bd_addr);

/**
 * @brief     send avrcp forward request.
 * @param[in] bd_addr: bt address
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_avrcp_forward(uint8_t *bd_addr);

/**
 * @brief     send avrcp backward request.
 * @param[in] bd_addr: bt address
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_avrcp_backward(uint8_t *bd_addr);

/**
 * @brief     send avrcp rewind start request.
 * @param[in] bd_addr: bt address
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_avrcp_rewind_start(uint8_t *bd_addr);

/**
 * @brief     send avrcp rewind stop request.
 * @param[in] bd_addr: bt address
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_avrcp_rewind_stop(uint8_t *bd_addr);

/**
 * @brief     send avrcp fast forward start request.
 * @param[in] bd_addr: bt address
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_avrcp_fast_forward_start(uint8_t *bd_addr);

/**
 * @brief     send avrcp fast forward stop request.
 * @param[in] bd_addr: bt address
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_avrcp_fast_forward_stop(uint8_t *bd_addr);

/**
 * @brief     send avrcp absolute volume set.
 * @param[in] bd_addr: bt address
 * @param[in] volume: volume
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_avrcp_absolute_volume_set(uint8_t *bd_addr, uint8_t volume);

/**
 * @brief     send avrcp volume change request.
 * @param[in] bd_addr: bt address
 * @param[in] volume: volume
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_avrcp_volume_change_req(uint8_t *bd_addr, uint8_t volume);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_AVRCP_H__ */
