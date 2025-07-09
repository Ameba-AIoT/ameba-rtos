/**
  ******************************************************************************
  * @file    os_cfg.c
  * @author
  * @version V1.0.0
  * @date    2023-8-15
  * @brief   This file provides firmware functions to define the following
  *          functionalities:
  *           - freertos configuration variable.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */


#include "os_cfg.h"

struct platform_os_cfg os_cfg;


void SecureContext_Init_rom(void)
{
	return;
}

void SecureInit_DePrioritizeNSExceptions_rom(void)
{
	return;
}

SecureContextHandle_t SecureContext_AllocateContext_rom(uint32_t ulSecureStackSize)
{
	UNUSED(ulSecureStackSize);

	return NULL;
}

void SecureContext_FreeContext_rom(SecureContextHandle_t xSecureContextHandle)
{
	UNUSED(xSecureContextHandle);

	return;
}

void SecureContext_LoadContext_rom(SecureContextHandle_t xSecureContextHandle)
{
	UNUSED(xSecureContextHandle);

	return;

}

void SecureContext_SaveContext_rom(SecureContextHandle_t xSecureContextHandle)
{
	UNUSED(xSecureContextHandle);

	return;
}

void vPortAllocateSecureContext_rom(uint32_t ulSecureStackSize)
{
	UNUSED(ulSecureStackSize);

	return;
}

void vPortCleanUpTCB_rom(uint32_t *pulTCB)
{
	UNUSED(pulTCB);

	return;
}

void traceTaskCreate_rom(uint32_t *pxNewTCB)
{
	UNUSED(pxNewTCB);

	return;
}

void traceTaskDelete_rom(uint32_t *pxTCB)
{
	UNUSED(pxTCB);

	return;
}


void newlib_assign_1_rom(uint32_t *pxNewLib_reent)
{
	UNUSED(pxNewLib_reent);

	return;
}

void newlib_assign_2_rom(uint32_t *pxNewLib_reent)
{
	UNUSED(pxNewLib_reent);

	return;
}

void newlib_reent_init_ptr_rom(uint32_t *pxNewLib_reent)
{
	UNUSED(pxNewLib_reent);

	return;
}

void newlib_reclaim_reent_rom(uint32_t *pxNewLib_reent)
{
	UNUSED(pxNewLib_reent);

	return;
}

SecureContextHandle_t (*patch_SecureContext_AllocateContext)(uint32_t ulSecureStackSize);
void (*patch_SecureContext_FreeContext)(SecureContextHandle_t xSecureContextHandle);
void (*patch_SecureInit_DePrioritizeNSExceptions)(void);
void (*patch_SecureContext_LoadContext)(SecureContextHandle_t xSecureContextHandle);
void (*patch_SecureContext_SaveContext)(SecureContextHandle_t xSecureContextHandle);
void (*patch_SecureContext_Init)(void);
void (*patch_vPortAllocateSecureContext)(uint32_t ulSecureStackSize);
void (*patch_vPortCleanUpTCB)(uint32_t *pulTCB);
void (*patch_traceTaskCreate)(uint32_t *pxNewTCB);
void (*patch_traceTaskDelete)(uint32_t *pxTCB);
void (*patch_newlib_assign_1)(uint32_t *pxNewLib_reent);
void (*patch_newlib_assign_2)(uint32_t *pxNewLib_reent);
void (*patch_newlib_reent_init_ptr)(uint32_t *pxNewLib_reent);
void (*patch_newlib_reclaim_reent)(uint32_t *pxNewLib_reent);
u32 (*patch_portDisable_Interrupts)(void);
void (*patch_portEnable_Interrupts)(uint32_t val);

