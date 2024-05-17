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

#if defined(CONFIG_BT_AUDIO_SOURCE_OUTBAND) && CONFIG_BT_AUDIO_SOURCE_OUTBAND
bool demo_uart_init(void);
uint16_t demo_uart_read(uint8_t *buf);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _APP_AUDIO_DATA_H_ */
