/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _SDIO_HAL_H_
#define _SDIO_HAL_H_

/* Includes ------------------------------------------------------------------*/

/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
void SPDIO_HAL_Init(void *adapter);
void SPDIO_HAL_DeInit(void);
char SPDIO_HAL_Transmit(u8 *pdata, u16 size);

#endif /* _SDIO_HAL_H_ */

