/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef __BT_AUDIO_NOISE_CANCELLATION_H__
#define __BT_AUDIO_NOISE_CANCELLATION_H__

#include <dlist.h>
#include <basic_types.h>

/*-------------------------------------------Notice !!!----------------------------------*/
/* the following menuconfig should be set when enable CONFIG_BT_AUDIO_NOISE_CANCELLATION */
/*         ./menuconfig.py -> CONFIG APPLICATION -> AI Config -> Enable AIVoice -> Select AFE Resource (afe_res_1mic)         */
#define CONFIG_BT_AUDIO_NOISE_CANCELLATION 0

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief     create new audio noise cancellation interface.
 * @param[in] channels: audio channel
 * @return
 *            - 0  : do presentation
 *            - 1: no need to do presentation
 */
uint16_t rtk_bt_audio_noise_cancellation_new(uint32_t channels);

/**
 * @brief     feed voice data to ai voice noise cancellation interface.
 * @param[in] data: pointer of voice data
 * @param[in] bytes: voice data length
 * @return
 *            cuccessfully feed voice data
 */
int rtk_bt_audio_noise_cancellation_feed(void *data, uint32_t bytes);

/**
 * @brief     get noise cancellation handled data.
 * @param[in] buffer: pointer of voice data
 * @param[in] size: voice data length
 * @return
 *            cuccessfully readed voice data
 */
uint32_t rtk_bt_audio_noise_cancellation_data_get(void *buffer, uint32_t size);

/**
 * @brief     destory noise cancellation interface
 * @param[in] none
 * @return
 *            - 0  : do presentation
 *            - 1: no need to do presentation
 */
uint16_t rtk_bt_audio_noise_cancellation_destroy(void);
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __BT_AUDIO_NOISE_CANCELLATION_H__ */
