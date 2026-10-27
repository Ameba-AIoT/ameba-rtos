/*
 * Copyright (c) 2022 Realtek, LLC.
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

#ifndef AMEBA_BASE_RPC_SRC_RPC_COMMON_H
#define AMEBA_BASE_RPC_SRC_RPC_COMMON_H

#include "rpc_struct.h"
#include "rpc_hw_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

bool_t xdr_RPC_STRUCT(XDR *xdrs, RPC_STRUCT *objp);
RPCHwManager *GetRPCManager(void);

/*
 * Per-channel transmit serialization.
 *
 * The underlying ring-buffer write (IPC_Message_Queue_Send) is a non-atomic
 * read-modify-write of the channel's wptr (compute free space -> memcpy body ->
 * advance wptr). Two tasks writing the SAME channel concurrently - e.g. an app
 * task in RPC_ClientCall() and a ReceiverLoop thread in RPC_SendReply() - can
 * interleave and tear the packet: their bodies overwrite each other and wptr
 * advances by only one packet, desyncing the reader's rptr so every later read
 * parses garbage (program_id=0, "no handler ... drop N bytes"). Holding the
 * channel's send lock across the whole WriteBuffer() makes each header+body
 * write atomic w.r.t. wptr. Locks are per-channel so unrelated channels never
 * serialize against each other.
 */
void RPC_SendLock(int32_t channel_id);
void RPC_SendUnlock(int32_t channel_id);

#ifdef __cplusplus
}
#endif

#endif // AMEBA_BASE_RPC_SRC_RPC_COMMON_H
