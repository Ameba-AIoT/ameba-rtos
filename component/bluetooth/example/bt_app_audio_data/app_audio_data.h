/*
*******************************************************************************
* Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#ifndef _APP_AUDIO_DATA_H_
#define _APP_AUDIO_DATA_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <rtk_bt_def.h>
#include "platform_autoconf.h"

extern const short birds_sing[];
extern uint32_t birds_sing_size;
#define CONFIG_BT_AUDIO_SOURCE_OUTBAND 0
#if defined(CONFIG_BT_AUDIO_SOURCE_OUTBAND) && CONFIG_BT_AUDIO_SOURCE_OUTBAND
/* Before USB dongle--> LE Audio/A2DP source test:
    Change the following Macros in app_audio_data.h
            change         CONFIG_BT_AUDIO_SOURCE_OUTBAND              to     1
            change         RTK_BT_AUDIO_SOURCE_OUTBAND_FROM_USB        to     1
    Change the following Macros in rtk_bt_le_audio_def.h
            change         RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT        to     0
            change         RTK_BLE_AUDIO_RECORD_SUPPORT                to     0
        (1)For testing USB-->BIS:
            change          RTK_BT_LE_AUDIO_BROADCAST_SOURCE_BIS_CODEC_CFG     to RTK_BT_LE_CODEC_CFG_ITEM_48_2
        (2)For testing USB-->CIS:
            change     RTK_BT_LE_MEDIA_CODEC_CFG_PREFER           to      RTK_BT_LE_CODEC_CFG_ITEM_48_2_BIT
            change     RTK_BT_LE_AUDIO_DEFAULT_CODEC_CFG_ITEM     to      RTK_BT_LE_CODEC_CFG_ITEM_48_2
    Config USB UAC before test : -->./menuconfig.py-->Enable USB-->USB Mode Device-->Enable UAC-->Select UAC Version 2.0
*/
#define RTK_BT_AUDIO_SOURCE_OUTBAND_FROM_USB 0
#define RTK_BT_AUDIO_SOURCE_OUTBAND_FROM_UART 0
#define RTK_BT_AUDIO_SOURCE_OUTBAND_FROM_AUDIO_HAL 0
#if defined(CONFIG_AUDIO_DEVICE_A2DP) && CONFIG_AUDIO_DEVICE_A2DP
#undef RTK_BT_AUDIO_SOURCE_OUTBAND_FROM_AUDIO_HAL
#define RTK_BT_AUDIO_SOURCE_OUTBAND_FROM_AUDIO_HAL 1
#endif
#if defined(RTK_BT_AUDIO_SOURCE_OUTBAND_FROM_USB) && RTK_BT_AUDIO_SOURCE_OUTBAND_FROM_USB
bool demo_usb_init(void);
bool demo_usb_deinit(void);
uint16_t demo_usb_read_buffer(uint8_t *buf, uint16_t len);
#elif defined(RTK_BT_AUDIO_SOURCE_OUTBAND_FROM_UART) && RTK_BT_AUDIO_SOURCE_OUTBAND_FROM_UART
bool demo_uart_init(void);
uint16_t demo_uart_read(uint8_t *buf, uint16_t len);
#endif
/* if no out band way is configured*/
#if (!defined(RTK_BT_AUDIO_SOURCE_OUTBAND_FROM_USB) || !RTK_BT_AUDIO_SOURCE_OUTBAND_FROM_USB) && \
    (!defined(RTK_BT_AUDIO_SOURCE_OUTBAND_FROM_UART) || !RTK_BT_AUDIO_SOURCE_OUTBAND_FROM_UART) && \
    (!defined(RTK_BT_AUDIO_SOURCE_OUTBAND_FROM_AUDIO_HAL) || !RTK_BT_AUDIO_SOURCE_OUTBAND_FROM_AUDIO_HAL)
#error "Please choose one bt audio out of band config when CONFIG_BT_AUDIO_SOURCE_OUTBAND enabled"
#endif
/* if not only one out band way is configured*/
#if ((RTK_BT_AUDIO_SOURCE_OUTBAND_FROM_USB + RTK_BT_AUDIO_SOURCE_OUTBAND_FROM_UART + RTK_BT_AUDIO_SOURCE_OUTBAND_FROM_AUDIO_HAL) > 1)
#error "Only one out of band bt audio way should be configured when CONFIG_BT_AUDIO_SOURCE_OUTBAND enabled"
#endif
#endif
#ifdef __cplusplus
}
#endif

#endif /* _APP_AUDIO_DATA_H_ */
