#ifndef _MESH_DATA_DUMP_H_
#define _MESH_DATA_DUMP_H_

#include <stdio.h>
#include <stdint.h>
#include <bt_debug.h>

#define data_uart_debug  BT_LOGA

void data_uart_dump(uint8_t *pbuffer, uint32_t len);

#endif //_MESH_DATA_DUMP_H_

