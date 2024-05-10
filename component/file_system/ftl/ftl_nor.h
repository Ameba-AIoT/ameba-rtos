/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      ftl_rom.h
   * @brief     ftl header file for rom project
   * @details
   * @author
   * @date
   * @version
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
  */

/*============================================================================*
  *                   Define to prevent recursive inclusion
  *============================================================================*/

#ifndef _FTL_ROM_H_
#define _FTL_ROM_H_


#ifdef  __cplusplus
extern  "C" {
#endif


/** @defgroup  FTL    Flash Transport Layer
    * @brief simple implementation of file system for flash
    * @{
    */
/** @defgroup FTL_Exported_Macros FTL Exported Macros
    * @brief
    * @{
    */
#define FTL_WRITE_SUCCESS             (0x00)
#define FTL_WRITE_ERROR_IN_INTR       (0x01)
#define FTL_WRITE_ERROR_INVALID_ADDR  (0x02)
#define FTL_WRITE_ERROR_OUT_OF_SPACE  (0x03)
#define FTL_WRITE_ERROR_READ_BACK     (0x04)
#define FTL_WRITE_ERROR_VERIFY        (0x05)
#define FTL_WRITE_ERROR_INVALID_PARAMETER   (0x06)
#define FTL_WRITE_ERROR_ERASE_FAIL     (0x07)
#define FTL_WRITE_ERROR_NOT_INIT       (0x08)
#define FTL_WRITE_ERROR_NEED_GC        (0x09)
#define FTL_READ_SUCCESS                    (0x00)
#define FTL_READ_ERROR_INVALID_LOGICAL_ADDR (0x01)
#define FTL_READ_ERROR_READ_NOT_FOUND       (0x02)
#define FTL_READ_ERROR_PARSE_ERROR          (0x03)
#define FTL_READ_ERROR_INVALID_PARAMETER    (0x04)
#define FTL_READ_ERROR_NOT_INIT             (0x05)

#define FTL_INIT_ERROR_ERASE_FAIL     (0x01)
/** End of FTL_Exported_Macros
    * @}
    */

#define ERROR_MUTEX_GET_TIMEOUT -2	/*get mutex fail. please try again*/
typedef enum {
	FTL_LEVEL_ERROR = 0, /**< Error */
	FTL_LEVEL_WARN  = 1, /**< Warning */
	FTL_LEVEL_INFO	   = 2, /**< Information */
	FTL_LEVEL_TRACE = 3, /**< Trace Data */
	FTL_LEVEL_NUMs	   = 4	/**< Level Number */
} FTL_LEVEL_DEFINE;

/*============================================================================*
  *                                   Types
  *============================================================================*/
/** @defgroup FTL_Exported_Types Flash Transport Layer Exported Types
    * @brief
    * @{
    */
typedef enum {
	FTL_IOCTL_DEBUG = 0,  /**< IO code for ftl debug */
	FTL_IOCTL_CLEAR_ALL = 2,  /**< IO code for clear ftl section*/
	FTL_IOCTL_ERASE_INVALID_PAGE = 3,  /**< IO code to erase invalid page*/
	FTL_IOCTL_ENABLE_GC_IN_IDLE = 4,  /**< IO code to enable garbage collection in idle task*/
	FTL_IOCTL_DISABLE_GC_IN_IDLE = 5,  /**< IO code to disable garbage collection in idle task*/
	FTL_IOCTL_DO_GC_IN_APP = 6,  /**< IO code to do garbage collection in app*/
} T_FTL_IOCTL_CODE;

/** End of FTL_Exported_Types
    * @}
    */



#define FTL_SUCCESS       (0x00)
#define FTL_ERROR_INVALID_LOGICAL_ADDR 0xF0
#define FTL_ERROR_OUT_OF_SPACE        (0xF1)
#define FTL_ERROR_PAGE_FORMAT         (0xF2)
#define FTL_ERROR_PAGE_END_FORMAT     (0xF3)


/*============================================================================*
  *                                   Types
  *============================================================================*/

/*============================================================================*
  *                                Functions
  *============================================================================*/
uint32_t nor_ftl_init(uint32_t u32PageStartAddr, uint8_t pagenum);
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
uint32_t nor_ftl_save_to_storage(void *pdata, uint16_t offset, uint16_t size);

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
uint32_t nor_ftl_load_from_storage(void *pdata, uint16_t offset, uint16_t size);
void nor_ftl_garbage_collect_in_idle(void);

/**
    * @brief    Control function entry for ftl
    * @param    cmd    command code for different operation
    * @param    p1     command parameter @ref FTL_IO_CTL_CODE
    * @param    p2     extended command parameters
    * @return   results of control
    * @retval   0  status successful
    * @retval   otherwise fail
    */
uint32_t ftl_ioctl(uint32_t cmd, uint32_t p1, uint32_t p2);

/** @} */ /* End of group FTL_Exported_Functions */

static inline void flash_set_bit(uint32_t *addr, uint32_t bit)
{
	(*addr) &= ~bit;
}

static inline bool flash_get_bit(uint32_t flag, uint32_t bit)
{
	return (flag & bit) ? _FALSE : _TRUE;
}

#ifdef  __cplusplus
}
#endif

#endif // _FTL_ROM_H_
