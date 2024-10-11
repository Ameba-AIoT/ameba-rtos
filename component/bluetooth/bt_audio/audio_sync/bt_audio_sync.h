/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef __BT_AUDIO_SYNC_H__
#define __BT_AUDIO_SYNC_H__

#include <dlist.h>
#include <basic_types.h>
#include "bt_audio_intf.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define RTK_BT_AUDIO_SYNC_MAX_GAP_NS 2500000
#define RTK_BT_AUDIO_SYNC_PPM_STEMP 1.03481

/**
 * @typedef   bt_audio_pres_act_t
 * @brief     audio presentation compensation act
 */
typedef enum {
	BT_AUDIO_PRES_ACT_ENABLE = 0,
	BT_AUDIO_PRES_ACT_DISABLE = 1,
	BT_AUDIO_PRES_ACT_HOLD = 2,
} bt_audio_pres_act_t;

/**
 * @brief     do audio xrun handle(when audio buffer is empty).
 * @param[in] track: pointer of audio track struct
 * @param[in] data: pointer of decoded data
 * @param[in] size: data length
 * @return
 *            - 0  : do presentation
 *            - 1: no need to do presentation
 */
uint16_t rtk_bt_audio_handle_xrun(rtk_bt_audio_track_t *track, uint8_t *data, uint16_t size);

/**
 * @brief     do audio presentation compenstaion check.
 * @param[in] track: pointer of audio track struct
 * @param[in] ts_us: time stamp(micro seconds)
 * @param[in] ppdata: pointer of decoded data's pointer
 * @param[in] pdata_size: pointer of decoded data size
 * @return
 *            - 0  : do presentation
 *            - 1: no need to do presentation
 */
uint16_t rtk_bt_audio_presentation_compensation(rtk_bt_audio_track_t *track, uint32_t ts_us, uint8_t **ppdata, uint32_t *pdata_size);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __BT_AUDIO_SYNC_H__ */
