/**
****************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
****************************************************************************************************
* @file      ftl.h
* @brief     flash transport layer is used as abstraction layer for user application to save read/write
*            parameters in flash.
* **************************************************************************************************
*/

#ifndef _FTL_H_
#define _FTL_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef  __cplusplus
extern  "C" {
#endif

#define FTL_WRITE_SUCCESS					(0x00)
#define FTL_WRITE_ERROR_INVALID_ADDR		(0x02)
#define FTL_WRITE_ERROR_NOT_INIT			(0x08)
#define FTL_READ_SUCCESS					(0x00)
#define FTL_READ_ERROR_INVALID_LOGICAL_ADDR	(0x01)
#define FTL_READ_ERROR_READ_NOT_FOUND		(0x02)
#define FTL_READ_ERROR_NOT_INIT				(0x05)
#define FTL_INIT_ERROR_ERASE_FAIL			(0x01)

uint32_t vfs_ftl_init(void);
/**
    * @brief    Save specified value to specified ftl offset
    * @param    pdata  specify data buffer
    * @param    offset specify FTL offset to store
    * @param    size   size to store
    * @return   status
    * @retval   0  status successful
    * @retval   otherwise fail
    * @note     FTL offset is pre-defined and no confict with ROM
    */
uint32_t ftl_save_to_storage(u8 *pdata, uint16_t offset, uint16_t size);

/**
    * @brief    Load specified ftl offset parameter to specified buffer
    * @param    pdata  specify data buffer
    * @param    offset specify FTL offset to load
    * @param    size   size to load
    * @return   status
    * @retval   0  status successful
    * @retval   otherwise fail
    * @note     FTL offset is pre-defined and no confict with ROM
    */
uint32_t ftl_load_from_storage(void *pdata, uint16_t offset, uint16_t size);

#ifdef  __cplusplus
}
#endif

#endif
