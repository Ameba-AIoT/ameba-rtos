#ifndef _AMEBA_DIAGNOSE_HEAP_H_
#define _AMEBA_DIAGNOSE_HEAP_H_

#include "ameba_diagnose_types.h"
typedef struct tagRtkDiagHeapHandler_t RtkDiagHeapHandler_t;

void debug_print(const char* tag, const RtkDiagHeapHandler_t* handler);


RtkDiagHeapHandler_t* rtk_diag_heap_create(u32 capacity, rtk_diag_heap_length_getter_t getter, rtk_diag_heap_release_notice_t notice);
int rtk_diag_heap_destroy(RtkDiagHeapHandler_t* handler);
void* rtk_diag_heap_malloc(RtkDiagHeapHandler_t* handler, u32 length);
u32 rtk_diag_heap_free(RtkDiagHeapHandler_t* handler);

#endif