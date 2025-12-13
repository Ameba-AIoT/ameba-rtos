/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
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

#ifndef AMEBA_AUDIO_FOUNDATION_INCLUDE_FOUNDATION_AHANDLER_H
#define AMEBA_AUDIO_FOUNDATION_INCLUDE_FOUNDATION_AHANDLER_H

#include <stdint.h>

#include "foundation/afoundation_forward_decls.h"

#ifdef __cplusplus
extern "C" {
#endif

struct AHandler {
    int32_t handler_id;
    ALooper *looper;
    void (*onMessageReceived)(AHandler *me, AMessage *msg);
};

void AHandler_init(
    AHandler *handler,
    void (*onMessageReceived)(AHandler *me, AMessage *msg)
);

#ifdef __cplusplus
}
#endif

#endif // AMEBA_AUDIO_FOUNDATION_INCLUDE_FOUNDATION_AHANDLER_H
