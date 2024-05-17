/**
 ******************************************************************************
 * @file    stsafea_service_stub.h
 * @author  MCD Application Team
 * @version V1.0.0
 * @brief   Implementation file of psa_drv_se interface.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */


#ifndef STSAFEA_SERVICE_STUB_H_
#define STSAFEA_SERVICE_STUB_H_

#include "stm32u5xx.h"

int32_t HW_IO_Init(void);
void    BSP_TimeDelay(uint32_t msDelay);
int32_t BSP_I2C_Init(void);
int32_t BSP_I2C_DeInit(void);
int32_t BSP_I2C_Send(uint16_t DevAddr, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C_Recv(uint16_t DevAddr, uint8_t *pData, uint16_t Length);
#endif
