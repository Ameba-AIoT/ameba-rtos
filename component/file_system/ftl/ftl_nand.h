/**
****************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
****************************************************************************************************
* @file      ftl.h
* @brief     flash transport layer is used as abstraction layer for user application to save read/write
*            parameters in flash.
* @note      ftl is dedicate block in flash, only used for save read/write value, and offset here is
*            logical offset which is defined for whole ftl section.If value is only for one time read,
*            refer to fs_load_app_data_8 or other APIs in flash_device.h
* @author    Brenda_li
* @date      2016-12-27
* @version   v1.0
* **************************************************************************************************
*/

#ifndef _FTL_H_
#define _FTL_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef  __cplusplus
extern  "C" {
#endif  // __cplusplus

/** @defgroup  FTL    Flash Transport Layer
    * @brief simple implementation of file system for flash
    * @{
    */
/** @defgroup FTL_Exported_Macros FTL Exported Macros
    * @brief
    * @{
    */
#define NAND_FTL_WRITE_SUCCESS             (0x00)
#define NAND_FTL_WRITE_ERROR_INVALID_ADDR  (0x01)

#define NAND_FTL_READ_SUCCESS                    (0x00)
#define NAND_FTL_READ_ERROR_INVALID_LOGICAL_ADDR (0x01)
#define NAND_FTL_READ_ERROR_READ_NOT_FOUND       (0x02)
#define NAND_FTL_READ_NAND_FAIL          		 (0x03)

#define NAND_FTL_ERROR_MUTEX_GET_TIMEOUT -2	/*get mutex fail. please try again*/

#define NAND_FTL_ERROR_NOT_INIT      			 (0x08)
//#define NAND_FTL_ERROR_      			 (0x09)


#define NAND_FTL_STATUS_OK            			 (0x00)
#define NAND_FTL_SUCCESS       				 	 (0x00)
#define NAND_FTL_ERROR_INVALID_LOGICAL_ADDR	 	  0xF0
#define NAND_FTL_ERROR_OUT_OF_SPACE        		 (0xF1)
#define NAND_FTL_ERROR_PAGE_FORMAT         		 (0xF2)
#define NAND_FTL_ERROR_PAGE_END_FORMAT     		 (0xF3)
#define NAND_FTL_BOUNDARY_ERROR             	 (0xF4)


#define NAND_ADDR_TO_BYTE_ADDR(addr) 		((addr) & BIT_LEN_MASK_32(NAND_PAGE_SIZE_MAIN_BIT_EXP))
#define NAND_ADDR_TO_PAGE_ADDR(addr) 		((addr) >> NAND_PAGE_SIZE_MAIN_BIT_EXP)


#define BLOCK_NUMBER 					3
#define Alloc_page_number				2 /*If the user wants to expand the ftl size, they can add this value by+1*/
#define BLOCK_SHIFT_BIT					5 /*the bit number of BLOCK_SHIFT*/


/*fix value*/
#define NAND_PAGE_NUM_INBLOCK			64
#define NAND_PAGE_SIZE           		0x800
#define MAX_logical_size_unit 			0x780
#define NAND_BLOCK_SIZE					0x20000
#define PATTERN_SIZE  					48
#define PATTERN_OFFSET  				0x780
#define FTL_ASSERT(x)   				assert_param(x)

/*Adjustable value*/
#define Flag_BIT_NUM	  				Alloc_page_number*MAX_logical_size_unit/4/8
#define NAND_MAX_LGSIZE		 			Alloc_page_number*MAX_logical_size_unit
#define NAND_LOGPAGE_SIZE 				Alloc_page_number*NAND_PAGE_SIZE
#define BLOCK_SHIFT						NAND_BLOCK_SIZE/NAND_PAGE_SIZE/Alloc_page_number
#define NEW_BLOCK_CHECK(x)				(x % BLOCK_SHIFT==0) ? 1 : 0
#define GET_PRE_BLOCK_ID(x)				((x>>BLOCK_SHIFT_BIT)+BLOCK_NUMBER-1)%BLOCK_NUMBER
#define RESVED_SIZE						Alloc_page_number*NAND_PAGE_SIZE-NAND_MAX_LGSIZE-Flag_BIT_NUM-PATTERN_SIZE
struct Page_T {
	u8 Data[Alloc_page_number * NAND_PAGE_SIZE];
};

struct single_Page_T {
	u8 Data[NAND_PAGE_SIZE];
};


typedef struct {
	u8 payload[NAND_MAX_LGSIZE];
	u8 bitflag[Flag_BIT_NUM];
	u8 rsvd[RESVED_SIZE];
	u8 used[PATTERN_SIZE];
} Pageinfo_Def;


typedef struct {
	struct single_Page_T page_data[Alloc_page_number];

} Sectioninfo_Def;


/** End of FTL_Exported_Macros
    * @}
    */

typedef enum {
	NAND_FTL_LEVEL_ERROR = 0, /**< Error */
	NAND_FTL_LEVEL_WARN  = 1, /**< Warning */
	NAND_FTL_LEVEL_INFO	   = 2, /**< Information */
	NAND_FTL_LEVEL_TRACE = 3, /**< Trace Data */
	NAND_FTL_LEVEL_NUMs	   = 4	/**< Level Number */
} NAND_FTL_LEVEL_DEFINE;

#define NAND_FTL_PRINT_LEVEL					NAND_FTL_LEVEL_ERROR

#define NAND_FTL_PRINTF(LEVEL, pFormat, ...)     do {\
   if (LEVEL <= NAND_FTL_PRINT_LEVEL)\
        RTK_LOGS(#LEVEL, pFormat"\r\n", ##__VA_ARGS__);\
}while(0)


/*============================================================================*
  *                                Functions
  *============================================================================*/
/** @defgroup FTL_Exported_Functions Flash Transport Layer Exported Functions
    * @brief
    * @{
    */
/**
    * @brief    Save specified value to specified ftl offset
    * @param    pbuffer  specify data buffer
    * @param    logical_addr specify FTL offset to store
    *     @arg  Min: 0
    *     @arg  Max: 2000
    * @param    size   size to store
    *     @arg  Min: 4
    *     @arg  Max: 2000
    * @return   status
    * @retval   0  status successful
    * @retval   otherwise fail
    * @note     FTL offset is pre-defined and no conflict
    */
uint32_t nand_ftl_save_to_storage(void *pbuffer, uint16_t logical_addr, uint16_t size);


/**
    * @brief    Load specified ftl offset parameter to specified buffer
    * @param    pbuffer  specify data buffer
    * @param    logical_addr specify FTL offset to load
    *     @arg  Min: 0
    *     @arg  Max: 2000
    * @param    size   size to load
    *     @arg  Min: 4
    *     @arg  Max: 2000
    * @return   status
    * @retval   0  status successful
    * @retval   otherwise fail
    * @note     FTL offset is pre-defined and no conflict
    */
uint32_t nand_ftl_load_from_storage(void *pbuffer, uint16_t logical_addr, uint16_t size);

uint32_t nand_ftl_init(uint32_t u32PageStartAddr, uint8_t pagenum);



/** @} */ /* End of group FTL */

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif // _FTL_H_
