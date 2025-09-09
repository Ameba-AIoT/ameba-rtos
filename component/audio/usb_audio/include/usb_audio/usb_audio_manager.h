/*
 * Copyright (c) 2021 Realtek, LLC.
 * All rights reserved.
 *
 * Licensed under the Realtek License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License from Realtek
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AMEBA_AUDIO_USB_AUDIO_USB_AUDIO_MANAGER
#define AMEBA_AUDIO_USB_AUDIO_USB_AUDIO_MANAGER

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    /* Usb is connected to device. */
    USB_AUDIO_EVENT_CONNECT,

    /* Usb is disconnected to device. */
    USB_AUDIO_EVENT_DISCONNECT,

    /* Usb is ready for use. */
    USB_AUDIO_EVENT_READY,

    /* Usb encounters an error. */
    USB_AUDIO_EVENT_ERROR,
} UsbAudioEventType;

typedef struct UsbAudioManager UsbAudioManager;
typedef void (*UsbAudioEventCallback)(UsbAudioEventType event, void *user_data);

/**
 * @brief Get the UsbAudioManager handler.
 * @return a UsbAudioManager object pointer.
 */
UsbAudioManager* GetUsbAudioManager(void);

/**
 * @brief Register the UsbAudioManager callback.
 * @param manager The UsbAudioManager object pointer.
 * @param callback The UsbAudioEventCallback used to receive events.
 */
void UsbAudioManager_RegisterEventCallback(UsbAudioManager *manager, UsbAudioEventCallback callback, void *user_data);

/**
 * @brief Start usb audio manager.
 * @param manager The UsbAudioManager object pointer.
 * @return Returns OSAL_OK if the operation is successful.
 */
int UsbAudioManager_Start(UsbAudioManager *manager);

/**
 * @brief Stop usb audio manager.
 * @param manager The UsbAudioManager object pointer.
 * @return Returns OSAL_OK if the operation is successful.
 */
int UsbAudioManager_Stop(UsbAudioManager *manager);

/**
 * @brief Release usb audio manager.
 * @param manager The UsbAudioManager object pointer.
 */
void ReleaseUsbAudioManager(void);

#ifdef __cplusplus
}
#endif

#endif /* AMEBA_AUDIO_USB_AUDIO_USB_AUDIO_MANAGER */