
#include "ameba_diagnose_ring_array.h"

#include "os_wrapper.h"

struct tagRtkDiagRingArrayHandler {
	u32 capacity;
	u32 element_size;
	u32 head;
	u32 tail;
	u32 count;
	rtk_diag_ring_array_emplace_t emplace;
	u8 data[0];
} ;

RtkDiagRingArrayHandler_t *rtk_diag_ring_array_create(u32 capacity, u32 element_size, rtk_diag_ring_array_emplace_t emplace)
{
	RtkDiagRingArrayHandler_t *handler = (RtkDiagRingArrayHandler_t *)rtos_mem_malloc(sizeof(RtkDiagRingArrayHandler_t) + capacity * element_size);
	if (NULL == handler) {
		RTK_LOGA(NOTAG, "Failed to allocate memory for RtkDiagHeapHandler_t\n");
		return NULL;
	}

	handler->capacity = capacity;
	handler->element_size = element_size;
	handler->head = 0;
	handler->tail = 0;
	handler->count = 0;
	handler->emplace = emplace;
	return handler;
}

int rtk_diag_ring_array_destroy(RtkDiagRingArrayHandler_t *handler)
{
	if (NULL == handler) {
		RTK_LOGA(NOTAG, "RtkDiagRingArrayHandler_t null\n");
		return RTK_ERR_BADARG;
	}
	rtos_mem_free(handler);
	return RTK_SUCCESS;
}

int rtk_diag_ring_array_clear(RtkDiagRingArrayHandler_t *handler)
{
	if (NULL == handler) {
		RTK_LOGA(NOTAG, "RtkDiagRingArrayHandler_t null\n");
		return RTK_ERR_BADARG;
	}
	handler->head = 0;
	handler->tail = 0;
	handler->count = 0;
	return RTK_SUCCESS;
}

void rtk_diag_ring_array_emplace_back(RtkDiagRingArrayHandler_t *handler, const void *origin_data)
{
	if (handler == NULL || handler->emplace == NULL) {
		return;
	}

	if (handler->count == handler->capacity) {
		handler->head = (handler->head + 1) % handler->capacity;
	} else {
		handler->count++;
	}

	handler->emplace(handler->data + (handler->tail * handler->element_size), origin_data);
	handler->tail = (handler->tail + 1) % handler->capacity;
}

u32 rtk_diag_ring_array_size(const RtkDiagRingArrayHandler_t *handler)
{
	if (handler == NULL) {
		return 0;
	}
	return handler->count;
}

const void *rtk_diag_ring_array_view(const RtkDiagRingArrayHandler_t *handler, u32 index)
{
	if (handler == NULL || index >= handler->count) {
		return NULL;
	}
	u32 physical_index = (handler->head + index) % handler->capacity;
	return handler->data + (physical_index * handler->element_size);
}

void *rtk_diag_ring_array_pop(RtkDiagRingArrayHandler_t *handler)
{
	if (handler == NULL || handler->count == 0) {
		return NULL;
	}

	void *element_address = handler->data + (handler->head * handler->element_size);

	handler->head = (handler->head + 1) % handler->capacity;
	handler->count--;

	return element_address;
}