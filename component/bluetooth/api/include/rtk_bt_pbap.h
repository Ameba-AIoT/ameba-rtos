/**
 * @file      rtk_bt_pbap.h
 * @author    benling_xu@realsil.com.cn
 * @brief     Bluetooth PBAP type and function definition.
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_PBAP_H__
#define __RTK_BT_PBAP_H__

#include <rtk_bt_common.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup BT_Constants BT Constants
 * @{
 */
/** @addtogroup BT_PBAP_Constants BT PBAP Constants
 * @{
 */
/* Define the vCard property mask bits used as the phone book pull filter. */
#define RTK_BT_PBAP_PROPERTY_VERSION     (1 << 0)    /*!< vCard version property. */
#define RTK_BT_PBAP_PROPERTY_FN          (1 << 1)    /*!< Formatted name property. */
#define RTK_BT_PBAP_PROPERTY_N           (1 << 2)    /*!< Structured presentation of name property. */
#define RTK_BT_PBAP_PROPERTY_PHOTO       (1 << 3)    /*!< Associated image or photo property. */
#define RTK_BT_PBAP_PROPERTY_BDAY        (1 << 4)    /*!< Birthday property. */
#define RTK_BT_PBAP_PROPERTY_ADR         (1 << 5)    /*!< Delivery address property. */
#define RTK_BT_PBAP_PROPERTY_LABEL       (1 << 6)    /*!< Delivery address label property. */
#define RTK_BT_PBAP_PROPERTY_TEL         (1 << 7)    /*!< Telephone number property. */
#define RTK_BT_PBAP_PROPERTY_EMAIL       (1 << 8)    /*!< Electronic mail address property. */
#define RTK_BT_PBAP_PROPERTY_MAILER      (1 << 9)    /*!< Electronic mail type property. */
#define RTK_BT_PBAP_PROPERTY_TZ          (1 << 10)   /*!< Time zone property. */
#define RTK_BT_PBAP_PROPERTY_GEO         (1 << 11)   /*!< Geographic position property. */
#define RTK_BT_PBAP_PROPERTY_TITLE       (1 << 12)   /*!< Job title property. */
#define RTK_BT_PBAP_PROPERTY_ROLE        (1 << 13)   /*!< Role within the organization property. */
#define RTK_BT_PBAP_PROPERTY_LOGO        (1 << 14)   /*!< Organization logo property. */
#define RTK_BT_PBAP_PROPERTY_AGENT       (1 << 15)   /*!< vCard of the person representing the contact property. */
#define RTK_BT_PBAP_PROPERTY_ORG         (1 << 16)   /*!< Name of the organization property. */
#define RTK_BT_PBAP_PROPERTY_NOTE        (1 << 17)   /*!< Comments property. */
#define RTK_BT_PBAP_PROPERTY_REV         (1 << 18)   /*!< Revision property. */
#define RTK_BT_PBAP_PROPERTY_SOUND       (1 << 19)   /*!< Pronunciation of name property. */
#define RTK_BT_PBAP_PROPERTY_URL         (1 << 20)   /*!< Uniform resource locator property. */
#define RTK_BT_PBAP_PROPERTY_UID         (1 << 21)   /*!< Unique ID property. */
#define RTK_BT_PBAP_PROPERTY_KEY         (1 << 22)   /*!< Public encryption key property. */
#define RTK_BT_PBAP_PROPERTY_NICKNAME    (1 << 23)   /*!< Nickname property. */
#define RTK_BT_PBAP_PROPERTY_CATEGORIES  (1 << 24)   /*!< Categories property. */
#define RTK_BT_PBAP_PROPERTY_PROID       (1 << 25)   /*!< Product ID property. */
#define RTK_BT_PBAP_PROPERTY_CLASS       (1 << 26)   /*!< Class information property. */
#define RTK_BT_PBAP_PROPERTY_SORTSTRING  (1 << 27)   /*!< String used for sorting operations property. */
#define RTK_BT_PBAP_PROPERTY_DATETIME    (1 << 28)   /*!< Time stamp property. */
/**
 * @}
 */
/**
 * @}
 */

/************************************** Data Types *************************************/
/** @addtogroup BT_Types BT Types
 * @{
 */
/** @addtogroup BT_PBAP_Types BT PBAP Types
 * @{
 */
/**
 * @typedef   rtk_bt_pbap_repository
 * @brief     Bluetooth PBAP phone book repository.
 */
typedef enum {
	RTK_BT_PBAP_REPOSITORY_LOCAL    = (1 << 0),    /*!< Local phone memory repository. */
	RTK_BT_PBAP_REPOSITORY_SIM1     = (1 << 1),    /*!< SIM1 repository. */
} rtk_bt_pbap_repository;

/**
 * @typedef   rtk_bt_pbap_path
 * @brief     Bluetooth PBAP phone book folder path.
 */
typedef enum {
	RTK_BT_PBAP_PATH_ROOT           = 0x00,    /*!< Root folder. */
	RTK_BT_PBAP_PATH_TELECOM        = 0x01,    /*!< Telecom folder. */
	RTK_BT_PBAP_PATH_PB             = 0x02,    /*!< Main phone book (pb) folder. */
	RTK_BT_PBAP_PATH_ICH            = 0x03,    /*!< Incoming calls history (ich) folder. */
	RTK_BT_PBAP_PATH_OCH            = 0x04,    /*!< Outgoing calls history (och) folder. */
	RTK_BT_PBAP_PATH_MCH            = 0x05,    /*!< Missed calls history (mch) folder. */
	RTK_BT_PBAP_PATH_CCH            = 0x06,    /*!< Combined calls history (cch) folder. */
} rtk_bt_pbap_path;

/**
 * @typedef   rtk_bt_pbap_phone_book
 * @brief     Bluetooth PBAP phone book object.
 */
typedef enum {
	RTK_BT_PBAP_PHONE_BOOK_PB           = 0x00,    /*!< Main phone book object. */
	RTK_BT_PBAP_PHONE_BOOK_ICH          = 0x01,    /*!< Incoming calls history object. */
	RTK_BT_PBAP_PHONE_BOOK_OCH          = 0x02,    /*!< Outgoing calls history object. */
	RTK_BT_PBAP_PHONE_BOOK_MCH          = 0x03,    /*!< Missed calls history object. */
	RTK_BT_PBAP_PHONE_BOOK_CCH          = 0x04,    /*!< Combined (incoming, outgoing, and missed) calls history object. */
	RTK_BT_PBAP_PHONE_BOOK_SPD          = 0x05,    /*!< Speed-dial object. */
	RTK_BT_PBAP_PHONE_BOOK_FAV          = 0x06,    /*!< Favorite contacts object. */
} rtk_bt_pbap_phone_book;

/**
 * @struct    rtk_bt_pbap_set_pb_t
 * @brief     Bluetooth PBAP set phone book parameters.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< Address of the remote device. */
	rtk_bt_pbap_repository repos;                       /*!< Repository that contains the phone book. */
	rtk_bt_pbap_path path;                              /*!< Folder path to be set as the active phone book. */
} rtk_bt_pbap_set_pb_t;

/**
 * @struct    rtk_bt_pbap_pb_pull_t
 * @brief     Bluetooth PBAP phone book pull parameters.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< Address of the remote device. */
	rtk_bt_pbap_repository repos;                       /*!< Repository that contains the phone book. */
	rtk_bt_pbap_phone_book phone_book;                  /*!< Phone book object to be retrieved. */
	uint16_t start_offset;                              /*!< Offset of the first entry to be retrieved. */
	uint16_t max_list_count;                            /*!< Maximum number of entries to be retrieved. */
	uint64_t filter;                                    /*!< Property filter attribute mask of 64 bits.
                                                         *   Each bit corresponds to a property defined by the RTK_BT_PBAP_PROPERTY_xxx macros.
                                                         *   0: Filter is disabled. */
} rtk_bt_pbap_pb_pull_t;

/**
 * @struct    rtk_bt_pbap_pb_size_get_t
 * @brief     Bluetooth PBAP phone book size get parameters.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< Address of the remote device. */
	rtk_bt_pbap_repository repos;                       /*!< Repository that contains the phone book. */
	rtk_bt_pbap_phone_book phone_book;                  /*!< Phone book object whose size is queried. */
} rtk_bt_pbap_pb_size_get_t;

/**
 * @struct    rtk_bt_pbap_pull_caller_id_name_t
 * @brief     Bluetooth PBAP caller ID name pull parameters.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< Address of the remote device. */
	char number[20];                                    /*!< Phone number of the caller to be looked up. */
} rtk_bt_pbap_pull_caller_id_name_t;

/**
 * @struct    rtk_bt_pbap_conn_ind_t
 * @brief     Bluetooth PBAP connection indication.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< Address of the remote device. */
} rtk_bt_pbap_conn_ind_t;

/**
 * @struct    rtk_bt_pbap_conn_fail_ind_t
 * @brief     Bluetooth PBAP connection failure indication.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< Address of the remote device. */
	uint16_t cause;                                     /*!< Cause code of the connection failure. */
} rtk_bt_pbap_conn_fail_ind_t;

/**
 * @struct    rtk_bt_pbap_disconn_cmpl_t
 * @brief     Bluetooth PBAP disconnection completion indication.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< Address of the remote device. */
	uint16_t cause;                                     /*!< Cause code of the disconnection. */
} rtk_bt_pbap_disconn_cmpl_t;

/**
 * @struct    rtk_bt_pbap_set_pb_cmpl_t
 * @brief     Bluetooth PBAP set phone book completion indication.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< Address of the remote device. */
	bool result;                                        /*!< Result of setting the phone book.
                                                         *   - true: The phone book is set successfully.
                                                         *   - false: Failed to set the phone book. */
	rtk_bt_pbap_path path;                              /*!< Folder path reported by the remote device. */
} rtk_bt_pbap_set_pb_cmpl_t;

/**
 * @struct    rtk_bt_pbap_get_pb_cmpl_t
 * @brief     Bluetooth PBAP get phone book completion indication.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< Address of the remote device. */
	uint8_t *p_data;                                    /*!< Pointer to the received phone book data.
                                                         *   Valid only when data_len is not 0. */
	uint16_t data_len;                                  /*!< Length of the data pointed to by p_data, in bytes. */
	uint16_t pb_size;                                   /*!< Number of entries in the phone book. */
	uint8_t new_missed_calls;                           /*!< Number of new missed calls. */
	bool data_end;                                      /*!< Whether the received data is the last fragment.
                                                         *   - true: No more data follows.
                                                         *   - false: More data follows. */
} rtk_bt_pbap_get_pb_cmpl_t;

/**
 * @struct    rtk_bt_pbap_caller_id_name_t
 * @brief     Bluetooth PBAP caller ID name indication.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< Address of the remote device. */
	uint8_t *name_ptr;                                  /*!< Pointer to the caller name.
                                                         *   Valid only when name_len is not 0. */
	uint16_t name_len;                                  /*!< Length of the caller name pointed to by name_ptr, in bytes. */
} rtk_bt_pbap_caller_id_name_t;

/**
 * @struct    rtk_bt_pbap_get_pb_size_cmpl
 * @brief     Bluetooth PBAP get phone book size completion indication.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< Address of the remote device. */
	uint16_t pb_size;                                   /*!< Number of entries in the phone book. */
	uint8_t new_missed_calls;                           /*!< Number of new missed calls. */
} rtk_bt_pbap_get_pb_size_cmpl;

/**
 * @}
 */
/**
 * @}
 */

/********************************* Functions Declaration *******************************/
/** @addtogroup BT_Functions BT Functions
 * @{
 */
/** @addtogroup BT_PBAP_Functions BT PBAP Functions
 * @{
 */

/**
 * @brief     Send a PBAP connection request to the remote device.
 *            The result is reported asynchronously via event @ref RTK_BT_PBAP_EVT_CONN_CMPL.
 * @param[in] bd_addr: Address of the remote device.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - If the connection fails, event @ref RTK_BT_PBAP_EVT_CONN_FAIL is reported instead.
 */
uint16_t rtk_bt_pbap_connect(uint8_t *bd_addr);

/**
 * @brief     Disconnect the PBAP connection with the remote device.
 *            The result is reported asynchronously via event @ref RTK_BT_PBAP_EVT_DISCONN_CMPL.
 * @param[in] bd_addr: Address of the remote device.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_pbap_disconnect(uint8_t *bd_addr);

/**
 * @brief     Send a request to set the active phone book on the remote device.
 *            The result is reported asynchronously via event @ref RTK_BT_PBAP_EVT_SET_PHONE_BOOK_CMPL.
 * @param[in] p_set_pb_t: Set phone book parameters.
 *                        Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_pbap_set_phone_book(rtk_bt_pbap_set_pb_t *p_set_pb_t);

/**
 * @brief     Send a request to pull the phone book from the remote device.
 *            The result is reported asynchronously via event @ref RTK_BT_PBAP_EVT_GET_PHONE_BOOK_CMPL.
 * @param[in] p_pb_pull_t: Phone book pull parameters.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_pbap_phone_book_pull(rtk_bt_pbap_pb_pull_t *p_pb_pull_t);

/**
 * @brief     Send a request to pull more phone book data from the remote device.
 *            The result is reported asynchronously via event @ref RTK_BT_PBAP_EVT_GET_PHONE_BOOK_CMPL.
 * @param[in] bd_addr: Address of the remote device.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_pbap_phone_book_pull_continue(uint8_t *bd_addr);

/**
 * @brief     Send a request to abort the ongoing phone book pull.
 * @param[in] bd_addr: Address of the remote device.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_pbap_phone_book_pull_abort(uint8_t *bd_addr);

/**
 * @brief     Send a request to get the phone book size of the remote device.
 *            The result is reported asynchronously via event @ref RTK_BT_PBAP_EVT_GET_PHONE_BOOK_SIZE_CMPL.
 * @param[in] p_pb_size_t: Phone book size query parameters.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_pbap_phone_book_size_get(rtk_bt_pbap_pb_size_get_t *p_pb_size_t);

/**
 * @brief     Send a request to pull the vCard listing of the current phone book by phone number to get the caller name.
 *            The result is reported asynchronously via event @ref RTK_BT_PBAP_EVT_CALLER_ID_NAME.
 * @param[in] p_pull_caller_id_name_t: Caller ID name pull parameters.
 *                                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_pbap_pull_caller_id_name(rtk_bt_pbap_pull_caller_id_name_t *p_pull_caller_id_name_t);

/**
 * @}
 */
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_PBAP_H__ */
