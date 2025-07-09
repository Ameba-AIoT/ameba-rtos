/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_FULLMAC_H
#define __USBD_FULLMAC_H

/* Includes ------------------------------------------------------------------*/

/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

int Fullmac_USBD_Init(void);

int Fullmac_USBD_DeInit(void);

int Fullmac_USBD_Process(void);

#endif /* __USBD_FULLMAC_H */

