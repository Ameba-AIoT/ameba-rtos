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

#ifndef AMEBA_AUDIO_FOUNDATION_INCLUDE_FOUNDATION_AMESSAGE_H
#define AMEBA_AUDIO_FOUNDATION_INCLUDE_FOUNDATION_AMESSAGE_H

#include <stdint.h>
#include <sys/types.h>

#include "osal_c/osal_errnos.h"
#include "foundation/afoundation_forward_decls.h"

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------------------
// AReplyToken
AReplyToken *AReplyToken_create(ALooper *looper);
void AReplyToken_destroy(AReplyToken *token);

ALooper* AReplyToken_getLooper(AReplyToken *token);
status_t AReplyToken_setReply(AReplyToken *token, AMessage *message);
bool AReplyToken_retrieveReply(AReplyToken *token, AMessage **reply);

AReplyToken* AReplyToken_get(AReplyToken *token);
void AReplyToken_put(AReplyToken *token);

// ----------------------------------------------------------------------
// AMessage
AMessage* AMessage_create(uint32_t what, AHandler *handler);
void AMessage_destroy(AMessage *message);

AMessage* AMessage_get(AMessage *message);
void AMessage_put(AMessage *message);

void AMessage_setWhat(AMessage *message, uint32_t what);
uint32_t AMessage_what(AMessage *message);

void AMessage_deliver(AMessage *message);
status_t AMessage_post(AMessage *message, int64_t delay_us);
status_t AMessage_postAndAwaitResponse(AMessage *message, AMessage **response);
bool AMessage_senderAwaitsResponse(AMessage *message, AReplyToken **reply_token);
status_t AMessage_postReply(AMessage *message, AReplyToken *reply_token);
AMessage* AMessage_duplicate(AMessage *message);

void AMessage_setInt32(AMessage *message, const char *name, int32_t value);
bool AMessage_findInt32(AMessage *message, const char *name, int32_t *value);
void AMessage_setInt64(AMessage *message, const char *name, int64_t value);
bool AMessage_findInt64(AMessage *message, const char *name, int64_t *value);
void AMessage_setSize(AMessage *message, const char *name, size_t value);
bool AMessage_findSize(AMessage *message, const char *name, size_t *value);
void AMessage_setFloat(AMessage *message, const char *name, float value);
bool AMessage_findFloat(AMessage *message, const char *name, float *value);
void AMessage_setDouble(AMessage *message, const char *name, double value);
bool AMessage_findDouble(AMessage *message, const char *name, double *value);
void AMessage_setPointer(AMessage *message, const char *name, void *value);
bool AMessage_findPointer(AMessage *message, const char *name, void **value);

#ifdef __cplusplus
}
#endif

#endif // AMEBA_AUDIO_FOUNDATION_INCLUDE_FOUNDATION_AMESSAGE_H
