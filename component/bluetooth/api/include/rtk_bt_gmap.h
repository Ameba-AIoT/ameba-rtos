/**
 * @file      rtk_bt_gmap.h
 * @author    michael_kong@realsil.com.cn
 * @brief     Bluetooth LE Audio Gaming Audio Profile type and function definition
 * @copyright Copyright (c) 2023. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_GMAP_H__
#define __RTK_BT_GMAP_H__

#include <app_bt_le_audio_common.h>

#ifdef __cplusplus
extern "C"
{
#endif  /* __cplusplus */

#if (defined(CONFIG_BT_GMAP_SUPPORT) && CONFIG_BT_GMAP_SUPPORT)


#define GMAS_CHAR_UUID_UGG 0x2C01
#define GMAS_CHAR_UUID_UGT 0x2C02
#define GMAS_CHAR_UUID_BGS 0x2C03
#define GMAS_CHAR_UUID_BGR 0x2C04
/* ---------------------------------- Date Types ------------------------------------- */
typedef struct {
	uint16_t conn_handle;
	uint16_t cid;
	uint16_t char_uuid;
} rtk_bt_le_audio_gmas_read_ind_t;

typedef struct {
	uint16_t conn_handle;
	bool    is_found;
	bool    load_from_ftl;
} rtk_bt_le_audio_gmas_client_dis_t;

typedef struct {
	uint16_t conn_handle;
	uint8_t  gmap_role;
	uint16_t cause;
} rtk_bt_le_audio_gmas_client_read_role_result_t;

typedef struct {
	uint16_t conn_handle;
	uint16_t char_uuid;
	uint8_t  features;
	uint16_t cause;
} rtk_bt_le_audio_gmas_client_read_features_result_t;

/* ------------------------------ Functions Declaration ------------------------------ */
uint16_t rtk_bt_le_audio_gmas_client_read_role(uint16_t conn_handle);

uint16_t rtk_bt_le_audio_gmas_client_read_features(uint16_t conn_handle, uint8_t gmap_role);

uint16_t rtk_bt_gmap_broadcast_game_receiver_cfg(uint8_t sound_channel, uint8_t *p_neighbor_addr);

uint16_t rtk_bt_gmap_unicast_game_terminal_cfg(uint8_t channel, uint8_t *p_neighbor_addr);

#endif /* CONFIG_BT_GMAP_SUPPORT */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __RTK_BT_GMAP_H__ */