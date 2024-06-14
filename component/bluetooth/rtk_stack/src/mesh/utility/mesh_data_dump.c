#include <stdio.h>
#include "mesh_data_dump.h"

void data_uart_dump(uint8_t *pbuffer, uint32_t len)
{
	BT_DUMPHEXA("0x", pbuffer, len, false);
}
