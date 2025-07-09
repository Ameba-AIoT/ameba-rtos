#include "ameba_diagnose_transform.h"
#include "ameba_diagnose_types.h"

int rtk_diag_uart_send(const RtkDiagDataFrame_t *frame)
{
	uint8_t *src = (uint8_t *)frame;
	u16 len = RTK_DIAG_FRAME_STRUCTURE_REAL_SIZE(frame);
	for (u16 i = 0; i < len; i++) {
		LOGUART_PutChar(src[i]);
	}
	return RTK_SUCCESS;
}

int rtk_diag_tcp_send(const RtkDiagDataFrame_t *frame)
{
	(void)frame;
	return RTK_SUCCESS;
}
