#include <stdio.h>
#include "mesh_data_dump.h"

void data_uart_dump(uint8_t *pbuffer, uint32_t len)
{
	printf("0x");
	for (uint32_t i = 0; i < len; i++) {
		printf("%02X", pbuffer[i]);
	}
	printf("\r\n");
}
