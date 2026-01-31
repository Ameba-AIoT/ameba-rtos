/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_CACHE_H_
#define _AMEBA_CACHE_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup AmebaD_Platform
  * @{
  */

/** @defgroup CACHE
  * @brief CACHE modules
  * @{
  */

/** @addtogroup CACHE
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  *		-just support read cache.
  *		-32K bytes.
  *		-used for flash read and XIP.
  *
  *****************************************************************************************
  * how to use
  *****************************************************************************************
  *		Cache_Enable: enable/disable cache
  *		Cache_Flush: flush cache, you should Cache_Flush after flash write or flash erase
  *****************************************************************************************
  * @endverbatim
  */

/** @defgroup CACHE_Line_Aligned_define
  * @{
  */
#define CACHE_LINE_SIZE						        	32U
#define CACHE_LINE_ADDR_MSK					      	(~(CACHE_LINE_SIZE - 1U))
#define CACHE_LINE_ALIGNMENT(x)				    	(((u32)(x) + (CACHE_LINE_SIZE - 1U)) & CACHE_LINE_ADDR_MSK)

#define IS_CACHE_LINE_ALIGNED_ADDR(ADDR)		(((ADDR) & (CACHE_LINE_SIZE - 1U)) == 0)
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup CACHE_Exported_Functions FLash Cache Exported Functions
  * @{
  */

/**
  * @brief  Enable Icache.
  */
__STATIC_INLINE
void ICache_Enable(void)
{
	SCB_EnableICache();
}

/**
  * @brief  Disable Icache.
  */
__STATIC_INLINE
void ICache_Disable(void)
{
	SCB_DisableICache();
}

/**
  * @brief  Invalidate Icache.
  */
__STATIC_INLINE
void ICache_Invalidate(void)
{
	SCB_InvalidateICache();
}

/**
  * @brief  Check DCache Enabled or not.
  */
__STATIC_INLINE
u32 DCache_IsEnabled(void)
{
	return ((SCB->CCR & (u32)SCB_CCR_DC_Msk) ? 1 : 0);
}

/**
  * @brief  Enable Dcache.
  */
__STATIC_INLINE
void DCache_Enable(void)
{
	if (!DCache_IsEnabled()) { //if D-Cache is enabled, no need to invalidate D-Cache
		SCB_EnableDCache();
	}
}

/**
  * @brief  Disable Dcache.
  */
__STATIC_INLINE
void DCache_Disable(void)
{
	SCB_DisableDCache();
}

/**
  * @brief   D-Cache Invalidate by address.
  * @details Invalidates D-Cache for the given address
  * @param  Address    address (aligned to 32-byte boundary)
  * @param  Bytes   size of memory block (in number of bytes)
  *
  * @note  Dcache will be restored from memory.
  * @note  This can be used after DMA Rx, and CPU read DMA data from DMA buffer.
  * @note  if Address is 0xFFFFFFFF, it means dont care, it was used when all Dcache be Invalidated.
  */
__STATIC_INLINE
void DCache_Invalidate(u32 Address, u32 Bytes)
{
	u32 addr = Address, len = Bytes;

	if (DCache_IsEnabled() == 0) {
		return;
	}

	if ((Address == 0xFFFFFFFF) && (Bytes == 0xFFFFFFFF)) {
		SCB_InvalidateDCache();
	} else {
		if (!IS_CACHE_LINE_ALIGNED_ADDR(Address)) {
			addr = Address & CACHE_LINE_ADDR_MSK; //region is [Address, Address + Bytes)
			len = Address - addr + Bytes; // need add length of [addr, Address)
		}

		SCB_InvalidateDCache_by_Addr((u32 *)addr, len);
	}
}

/**
  * @brief   D-Cache Clean by address
  * @details Cleans D-Cache for the given address
  * @param  Address    address (aligned to 32-byte boundary)
  * @param  Bytes   size of memory block (in number of bytes)
  *
  * @note  Dcache will be write back to memory.
  * @note  This can be used before DMA Tx, after CPU write data to DMA buffer.
  * @note  if Address is 0xFFFFFFFF, it means dont care, it was used when all Dcache be cleaned.
  * @note  AmebaD cache is default read allocation and write through, so clean is not needed.
  */
__STATIC_INLINE
void DCache_Clean(u32 Address, u32 Bytes)
{
	u32 addr = Address, len = Bytes;

	if (DCache_IsEnabled() == 0) {
		return;
	}

	if ((Address == 0xFFFFFFFF) && (Bytes == 0xFFFFFFFF)) {
		SCB_CleanDCache();
	} else {
		if (!IS_CACHE_LINE_ALIGNED_ADDR(Address)) {
			addr = Address & CACHE_LINE_ADDR_MSK; //region is [Address, Address + Bytes)
			len = Address - addr + Bytes; // need add length of [addr, Address)
		}

		SCB_CleanDCache_by_Addr((u32 *)addr, len);
	}
}


/**
  * @brief   D-Cache Clean and Invalidate by address
  * @details Cleans and invalidates D_Cache for the given address
  * @param  Address    address (aligned to 32-byte boundary)
  * @param  Bytes   size of memory block (in number of bytes)
  *
  * @note  This can be used when you want to write back cache data and then Invalidate cache.
  * @note  if Address is 0xFFFFFFFF, it means dont care, it was used when all Dcache be cleaned.
  */
__STATIC_INLINE
void DCache_CleanInvalidate(u32 Address, u32 Bytes)
{
	u32 addr = Address, len = Bytes;

	if (DCache_IsEnabled() == 0) {
		return;
	}

	if ((Address == 0xFFFFFFFF) && (Bytes == 0xFFFFFFFF)) {
		SCB_CleanInvalidateDCache();
	} else {
		if (!IS_CACHE_LINE_ALIGNED_ADDR(Address)) {
			addr = Address & CACHE_LINE_ADDR_MSK; //region is [Address, Address + Bytes)
			len = Address - addr + Bytes; // need add length of [addr, Address)
		}

		SCB_CleanInvalidateDCache_by_Addr((u32 *)addr, len);
	}
}

/**
  * @brief  Disable/Enable I/D cache.
  * @param  Enable
  *   This parameter can be any combination of the following values:
  *		 @arg ENABLE cache enable & SPIC read 16bytes every read command
  *		 @arg DISABLE cache disable & SPIC read 4bytes every read command
  */
__STATIC_INLINE
void Cache_Enable(u32 Enable)
{
	if (Enable) {
		ICache_Enable();
		DCache_Enable();
	} else {
		ICache_Disable();
		DCache_Disable();
	}
}

/**
  * @brief  flush I/D cache.
  */
__STATIC_INLINE
void Cache_Flush(void)
{
	SCB_InvalidateICache();
	SCB_InvalidateDCache();
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif //_RTL8710B_CACHE_H_
