/**
  ******************************************************************************
  * @file    low_level_ospi_device.c
  * @author  MCD Application Team
  * @brief   This file contains device definition for low_level_ospi_device
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
#include "flash_layout.h"
#include "low_level_ospi_flash.h"
/* give access to all flash */
static struct ospi_flash_range access_vect[] =
{
  {0, OSPI_FLASH_TOTAL_SIZE}
};

struct low_level_ospi_device OSPI_FLASH0_DEV =
{
  .erase = { .nb = sizeof(access_vect) / sizeof(struct ospi_flash_range), .range = access_vect},
  .write = { .nb = sizeof(access_vect) / sizeof(struct ospi_flash_range), .range = access_vect},
  .read_error = 1
};



