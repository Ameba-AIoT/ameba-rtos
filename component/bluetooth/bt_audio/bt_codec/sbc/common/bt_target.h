/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#ifndef __BT_TARGET_H__
#define __BT_TARGET_H__

#define SBC_DEC_INCLUDED 1
#define SBC_ENC_INCLUDED 1
#define SBC_PLC_INCLUDED 1
#if defined(SBC_PLC_INCLUDED) && SBC_PLC_INCLUDED
#define BTM_MSBC_FRAME_DATA_SIZE 57
#define HF_SBC_DEC_RAW_DATA_SIZE 240
#endif
#ifndef SBC_NO_PCM_CPY_OPTION
#define SBC_NO_PCM_CPY_OPTION 0
#endif
#ifndef BT_BLE_DYNAMIC_ENV_MEMORY
#define BT_BLE_DYNAMIC_ENV_MEMORY 0
#endif

#endif /* __BT_TARGET_H__ */