#ifndef _AMEBA_DIAGNOSE_PROTOCOL_H_
#define _AMEBA_DIAGNOSE_PROTOCOL_H_

#include "ameba_diagnose_types.h"

int rtk_diag_proto_init(u32 payload_capacity, rtk_diag_queue_next_event_getter_t getter);
void rtk_diag_proto_deinit(void);

const RtkDiagDataFrame_t* rtk_diag_proto_pack_error(u8 cmd_type, u8 error);
const RtkDiagDataFrame_t* rtk_diag_proto_pack_data(const RtkDiagEvent_t* event, u16 global_offset, u16 local_offset);

#endif