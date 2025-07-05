#ifndef SDIO_HOST_STM32_HAL_H
#define SDIO_HOST_STM32_HAL_H

#include "sdio_host_common.h"

typedef struct {
	unsigned char stuff_bits;
	unsigned int  ocr : 24;
} sdio_cmd5_argument_t;

typedef struct {
	unsigned int  _unique2         : 9; /* 0-8   */
	unsigned int  register_address : 17; /* 9-25  */
	unsigned int  _unique          : 2; /* 26-27 */
	unsigned int  function_number  : 3; /* 28-30 */
	unsigned int  rw_flag          : 1; /* 31    */
} sdio_cmd5x_argument_t;

typedef struct {
	uint8_t       write_data;           /* 0 - 7 */
	unsigned int  _stuff2          : 1; /* 8     */
	unsigned int  register_address : 17; /* 9-25  */
	unsigned int  _stuff           : 1; /* 26    */
	unsigned int  raw_flag         : 1; /* 27    */
	unsigned int  function_number  : 3; /* 28-30 */
	unsigned int  rw_flag          : 1; /* 31    */
} sdio_cmd52_argument_t;

typedef struct {
	unsigned int  count            : 9; /* 0-8   */
	unsigned int  register_address : 17; /* 9-25  */
	unsigned int  op_code          : 1; /* 26    */
	unsigned int  block_mode       : 1; /* 27    */
	unsigned int  function_number  : 3; /* 28-30 */
	unsigned int  rw_flag          : 1; /* 31    */
} sdio_cmd53_argument_t;

typedef union {
	uint32_t              value;
	sdio_cmd5_argument_t  cmd5;
	sdio_cmd5x_argument_t cmd5x;
	sdio_cmd52_argument_t cmd52;
	sdio_cmd53_argument_t cmd53;
} sdio_cmd_argument_t;

#define BUS_FUNCTION_MASK (0x3)  /* Update this if adding functions */
#define BIT_MASK( x )         (( 1 << x ) - 1 )

#endif