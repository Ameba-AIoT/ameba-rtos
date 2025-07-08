#ifndef _AMEBA_DIAGNOSE_TRANSFORM_H_
#define _AMEBA_DIAGNOSE_TRANSFORM_H_

#include "ameba_diagnose_protocol.h"

int rtk_diag_uart_send(const RtkDiagDataFrame_t *frame);
int rtk_diag_tcp_send(const RtkDiagDataFrame_t *frame);

#endif