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

#ifndef AMEBA_AUDIO_FOUNDATION_INCLUDE_FOUNDATION_ALOOPER_H
#define AMEBA_AUDIO_FOUNDATION_INCLUDE_FOUNDATION_ALOOPER_H

#include "osal_c/osal_errnos.h"
#include "foundation/afoundation_forward_decls.h"

#ifdef __cplusplus
extern "C" {
#endif

ALooper *ALooper_create(const char *name);
void ALooper_destroy(ALooper *looper);

void ALooper_setName(ALooper *looper, const char *name);

status_t ALooper_registerHandler(ALooper *looper, AHandler *handler);
void ALooper_unregisterHandler(int32_t handler_id);

status_t ALooper_start(ALooper *looper, bool run_local, int32_t priority);
status_t ALooper_stop(ALooper *looper);

status_t ALooper_post(ALooper *looper, AMessage *msg, int64_t delay_us);

AReplyToken* ALooper_createReplyToken(ALooper *looper);
status_t ALooper_awaitResponse(ALooper *looper, AReplyToken *reply_token, AMessage **response);
status_t ALooper_postReply(ALooper *looper, AReplyToken *reply_token, AMessage *reply);

#ifdef __cplusplus
}
#endif

#endif // AMEBA_AUDIO_FOUNDATION_INCLUDE_FOUNDATION_ALOOPER_H
