/**
 * @file      rtk_bt_pbap.h
 * @author    benling_xu@realsil.com.cn
 * @brief     Bluetooth LE GAP part type and function deffinition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_PBAP_H__
#define __RTK_BT_PBAP_H__

#include <rtk_bt_common.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * RTK_BT_PBAP_PROPERTY_MASK BT PBAP Property Mask
 */
#define RTK_BT_PBAP_PROPERTY_VERSION     (1 << 0)    /**< vCard Version */
#define RTK_BT_PBAP_PROPERTY_FN          (1 << 1)    /**< Formatted Name */
#define RTK_BT_PBAP_PROPERTY_N           (1 << 2)    /**< Structured Presentation of Name */
#define RTK_BT_PBAP_PROPERTY_PHOTO       (1 << 3)    /**< Associated Image or Photo */
#define RTK_BT_PBAP_PROPERTY_BDAY        (1 << 4)    /**< Birthday */
#define RTK_BT_PBAP_PROPERTY_ADR         (1 << 5)    /**< Delivery Address */
#define RTK_BT_PBAP_PROPERTY_LABEL       (1 << 6)    /**< Delivery */
#define RTK_BT_PBAP_PROPERTY_TEL         (1 << 7)    /**< Telephone Number */
#define RTK_BT_PBAP_PROPERTY_EMAIL       (1 << 8)    /**< Electronic Mail Address */
#define RTK_BT_PBAP_PROPERTY_MAILER      (1 << 9)    /**< Electronic Mail */
#define RTK_BT_PBAP_PROPERTY_TZ          (1 << 10)   /**< Time Zone */
#define RTK_BT_PBAP_PROPERTY_GEO         (1 << 11)   /**< Geographic Position */
#define RTK_BT_PBAP_PROPERTY_TITLE       (1 << 12)   /**< Job */
#define RTK_BT_PBAP_PROPERTY_ROLE        (1 << 13)   /**< Role within the Organization */
#define RTK_BT_PBAP_PROPERTY_LOGO        (1 << 14)   /**< Organization Logo */
#define RTK_BT_PBAP_PROPERTY_AGENT       (1 << 15)   /**< vCard of Person Representing */
#define RTK_BT_PBAP_PROPERTY_ORG         (1 << 16)   /**< Name of Organization */
#define RTK_BT_PBAP_PROPERTY_NOTE        (1 << 17)   /**< Comments */
#define RTK_BT_PBAP_PROPERTY_REV         (1 << 18)   /**< Revision */
#define RTK_BT_PBAP_PROPERTY_SOUND       (1 << 19)   /**< Pronunciation of Name */
#define RTK_BT_PBAP_PROPERTY_URL         (1 << 20)   /**< Uniform Resource Locator */
#define RTK_BT_PBAP_PROPERTY_UID         (1 << 21)   /**< Unique ID */
#define RTK_BT_PBAP_PROPERTY_KEY         (1 << 22)   /**< Public Encryption Key */
#define RTK_BT_PBAP_PROPERTY_NICKNAME    (1 << 23)   /**< Nickname */
#define RTK_BT_PBAP_PROPERTY_CATEGORIES  (1 << 24)   /**< Categories */
#define RTK_BT_PBAP_PROPERTY_PROID       (1 << 25)   /**< Product ID */
#define RTK_BT_PBAP_PROPERTY_CLASS       (1 << 26)   /**< Class information */
#define RTK_BT_PBAP_PROPERTY_SORTSTRING  (1 << 27)   /**< String used for sorting operations */
#define RTK_BT_PBAP_PROPERTY_DATETIME    (1 << 28)   /**< Time stamp */

typedef enum {
	RTK_BT_PBAP_REPOSITORY_LOCAL    = (1 << 0),
	RTK_BT_PBAP_REPOSITORY_SIM1     = (1 << 1),
} rtk_bt_pbap_repository;

typedef enum {
	RTK_BT_PBAP_PATH_ROOT           = 0x00,
	RTK_BT_PBAP_PATH_TELECOM        = 0x01,
	RTK_BT_PBAP_PATH_PB             = 0x02,
	RTK_BT_PBAP_PATH_ICH            = 0x03,
	RTK_BT_PBAP_PATH_OCH            = 0x04,
	RTK_BT_PBAP_PATH_MCH            = 0x05,
	RTK_BT_PBAP_PATH_CCH            = 0x06,
} rtk_bt_pbap_path;

typedef enum {
	RTK_BT_PBAP_PHONE_BOOK_PB           = 0x00,    /**< Main phone book object */
	RTK_BT_PBAP_PHONE_BOOK_ICH          = 0x01,    /**< Incoming calls history object */
	RTK_BT_PBAP_PHONE_BOOK_OCH          = 0x02,    /**< Outgoing calls history object */
	RTK_BT_PBAP_PHONE_BOOK_MCH          = 0x03,    /**< Missed calls history object */
	RTK_BT_PBAP_PHONE_BOOK_CCH          = 0x04,    /**< Combined (incoming, outgoing, and missed) calls history object */
	RTK_BT_PBAP_PHONE_BOOK_SPD          = 0x05,    /**< Speed-Dial object */
	RTK_BT_PBAP_PHONE_BOOK_FAV          = 0x06,    /**< Favorite contacts object */
} rtk_bt_pbap_phone_book;

/**
 * @struct    rtk_bt_pbap_set_pb_t
 * @brief     pbap set phone book struct.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< address */
	rtk_bt_pbap_repository repos;                       /*!< repository*/
	rtk_bt_pbap_path path;                                /*!< path*/
} rtk_bt_pbap_set_pb_t;

/**
 * @struct    rtk_bt_pbap_pb_pull_t
 * @brief     pbap phone book pull struct.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< address */
	rtk_bt_pbap_repository repos;                       /*!< repository */
	rtk_bt_pbap_phone_book phone_book;                /*!< The phone book object to be retrieved */
	uint16_t start_offset;                              /*!< The offset of the first entry to be retrieved compared to */
	uint16_t max_list_count;                            /*!< The maximum number of entries to be retrieved */
	uint64_t filter;                                    /*!< The filter attribute mask of 64 bits, and 0 for disabling filter \ref RTK_BT_PBAP_PROPERTY_MASK */
} rtk_bt_pbap_pb_pull_t;

/**
 * @struct    rtk_bt_pbap_pb_size_get_t
 * @brief     pbap phone book size get struct.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< address */
	rtk_bt_pbap_repository repos;                       /*!< repository */
	rtk_bt_pbap_phone_book phone_book;                /*!< The phone book object to be retrieved */
} rtk_bt_pbap_pb_size_get_t;

/**
 * @struct    rtk_bt_pbap_pull_caller_id_name_t
 * @brief     pbap pull caller id name struct.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< address */
	char number[20];                                    /*!< phone number */
} rtk_bt_pbap_pull_caller_id_name_t;

/**
 * @struct    rtk_bt_pbap_conn_ind_t
 * @brief     pbap connection indication.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< address */
} rtk_bt_pbap_conn_ind_t;

/**
 * @struct    rtk_bt_pbap_conn_fail_ind_t
 * @brief     pbap connection fail indication.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< address */
	uint16_t cause;                                     /*!< cause */
} rtk_bt_pbap_conn_fail_ind_t;

/**
 * @struct    rtk_bt_pbap_disconn_cmpl_t
 * @brief     pbap disconnection completion indication.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< address */
	uint16_t cause;                                     /*!< cause */
} rtk_bt_pbap_disconn_cmpl_t;

/**
 * @struct    rtk_bt_pbap_set_pb_cmpl_t
 * @brief     pbap set phone book completion indication.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< address */
	bool result;                                        /*!< result */
	rtk_bt_pbap_path path;                              /*!< cause */
} rtk_bt_pbap_set_pb_cmpl_t;

/**
 * @struct    rtk_bt_pbap_get_pb_cmpl_t
 * @brief     pbap get phone book completion indication.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< address */
	uint8_t *p_data;                                    /*!< pointer of data */
	uint16_t data_len;                                  /*!< data length */
	uint16_t pb_size;                                   /*!< phone book size */
	uint8_t new_missed_calls;                           /*!< new missed calls */
	bool data_end;                                      /*!< indicate data ends */
} rtk_bt_pbap_get_pb_cmpl_t;

/**
 * @struct    rtk_bt_pbap_caller_id_name_t
 * @brief     pbap caller id name indication.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< address */
	uint8_t *name_ptr;                                  /*!< pointer of name */
	uint16_t name_len;                                  /*!< name length */
} rtk_bt_pbap_caller_id_name_t;

/**
 * @struct    rtk_bt_pbap_get_pb_size_cmpl
 * @brief     pbap get phone book size completion indication.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< address */
	uint16_t pb_size;                                   /*!< phone book size */
	uint8_t new_missed_calls;                           /*!< new missed calls */
} rtk_bt_pbap_get_pb_size_cmpl;

/* ------------------------------ Functions Declaration ------------------------------ */
/**
 * @defgroup  bt_pbap BT PBAP APIs
 * @brief     BT PBAP related function APIs
 * @ingroup   BT_APIs
 * @{
 */

/**
 * @brief     send pbap connect request.
 * @param[in] bd_addr: bt address
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_pbap_connect(uint8_t *bd_addr);

/**
 * @brief     disconnect pbap.
 * @param[in] bd_addr: bt address
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_pbap_disconnect(uint8_t *bd_addr);

/**
 * @brief     send a request to set active phone book.
 * @param[in] p_set_pb_t: set phone book struct
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_pbap_set_phone_book(rtk_bt_pbap_set_pb_t *p_set_pb_t);

/**
 * @brief     send a request to pull phone book.
 * @param[in] p_pb_pull_t: phone book struct
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_pbap_phone_book_pull(rtk_bt_pbap_pb_pull_t *p_pb_pull_t);

/**
 * @brief     send a request to pull more data from the remote server.
 * @param[in] bd_addr: bt address
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_pbap_phone_book_pull_continue(uint8_t *bd_addr);

/**
 * @brief     send a request to get phone book size.
 * @param[in] p_pb_size_t: phone book size struct
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_pbap_phone_book_size_get(rtk_bt_pbap_pb_size_get_t *p_pb_size_t);

/**
 * @brief     send a request to pull the vCard listing in current phonebook by phone number.
 * @param[in] p_pull_caller_id_name_t: pull caller id name struct
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_pbap_pull_caller_id_name(rtk_bt_pbap_pull_caller_id_name_t *p_pull_caller_id_name_t);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_PBAP_H__ */
