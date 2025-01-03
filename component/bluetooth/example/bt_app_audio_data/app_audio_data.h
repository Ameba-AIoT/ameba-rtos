/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
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

extern const short birds_sing[];
extern uint32_t birds_sing_size;
#define CONFIG_BT_AUDIO_SOURCE_OUTBAND 0
//Config USB UAC before test : -->./menuconfig.py-->CA32 config-->Config USB-->Enable USB-->Enable UAC
#define AUDIO_SOURCE_OUTBAND_FROM_USB  0

#if defined(CONFIG_BT_AUDIO_SOURCE_OUTBAND) && CONFIG_BT_AUDIO_SOURCE_OUTBAND
#if defined(AUDIO_SOURCE_OUTBAND_FROM_USB) && AUDIO_SOURCE_OUTBAND_FROM_USB
bool demo_usb_init(void);
bool demo_usb_deinit(void);
uint16_t demo_usb_read_buffer(uint8_t *buf, uint16_t len);
#else
bool demo_uart_init(void);
uint16_t demo_uart_read(uint8_t *buf);
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* _APP_AUDIO_DATA_H_ */
