/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_OTA_H_
#define _AMEBA_OTA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

/**
  * @brief  OTA Image ID
  */
#define OTA_IMGID_BOOT 0  /* Bootloader */
#define OTA_IMGID_APP  1  /* Application */
#define OTA_IMGID_MAX  2

#define MAX_IMG_NUM    OTA_IMGID_MAX

u8 ota_get_cur_index(u8 img_id);
u8 ota_check_bootloader_ota2(void);

#ifdef __cplusplus
}
#endif

#endif //_AMEBA_OTA_H_