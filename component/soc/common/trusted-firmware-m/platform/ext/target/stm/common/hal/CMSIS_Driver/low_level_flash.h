/**
  ******************************************************************************
  * @file    low_level_flash.h
  * @author  MCD Application Team
  * @brief   This file contains device definition for low_level_flash driver
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020-2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
#ifndef __LOW_LEVEL_FLASH_H
#define __LOW_LEVEL_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif
#include "stm32hal.h"

struct flash_range
{
  uint32_t base;
  uint32_t limit;
};
struct flash_vect
{
  uint32_t nb;
  struct flash_range *range;
};
struct low_level_device
{
  struct flash_vect erase;
  struct flash_vect write;
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U) && !defined(LOCAL_LOADER_CONFIG)
  struct flash_vect secure;
#endif
#if !defined(LOCAL_LOADER_CONFIG)
  uint32_t read_error;
#endif
};

extern struct low_level_device FLASH0_DEV;

#if !defined(LOCAL_LOADER_CONFIG)
void NMI_Handler(void);
#endif

#endif /* __LOW_LEVEL_FLASH_H */


