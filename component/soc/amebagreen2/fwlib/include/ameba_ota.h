/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_OTA_H_
#define _AMEBA_OTA_H_

#include "ameba_secure_boot.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Ameba_Platform
  * @{
  */
#define OTA_CLEAR_PATTERN	0

#define BUF_SIZE			2048								/*the size of the buffer used for receiving firmware data from server*/
#define HEADER_BAK_LEN		32
#define HEADER_LEN			8
#define SUB_HEADER_LEN			24

/* Exported constants --------------------------------------------------------*/

/** @defgroup OTA_Exported_Constants OTA Exported Constants
  * @{
  */
/** @defgroup OTA_system_parameter_definitions
  * @{
  */
#define OTA_IMGID_BOOT		0
#define OTA_IMGID_APP		1
#define OTA_IMGID_MAX		2

#define MAX_IMG_NUM			OTA_IMGID_MAX

/* OTA download type */
#define OTA_USER			0
#define OTA_HTTP			1
#define OTA_HTTPS			2
#define OTA_VFS				3

/* OTA download status */
#define OTA_RET_ERR			-1
#define OTA_RET_OK			0
#define OTA_RET_FINISH		1
/**
  * @}
  */


/**
  * @}
  */


/** @defgroup OTA
  * @brief OTA driver modules
  * @{
  */

/* Exported Types --------------------------------------------------------*/
/** @defgroup OTA_Exported_Types OTA Exported Types
  * @{
  */

/**
  * @brief  OTA http response result structure definition
  */
typedef struct {
	u32	status_code;
	u32	header_len;
	u8	*body;
	u32	body_len;
	u8	*header_bak;
	u32	parse_status;
} http_response_result_t;

/**
  * @brief  OTA firmware file header structure definition
  */
typedef struct {
	u32	FwVer;	/*!< Specifies the OTA firmware verision.
	                         This parameter is in first Dword in the firmware file. */
	u32	HdrNum;/*!< Specifies the OTA firmware header number.
	                         This parameter indicates how many headers in firmware file. */
} update_file_hdr;

/**
  * @brief  OTA firmware file image header structure definition
  */
typedef struct {
	u8	Signature[4];	/*!< Specifies the OTA image signature, the value is "OTA".
	                         	This parameter is used to identify the OTA header needed. */
	u32	ImgHdrLen;	/*!< Specifies the OTA image header length.
	                         	This parameter indicates the Image Header Length. */
	u32	Checksum;	/*!< Specifies the OTA image checksum.
	                         	This parameter is used to judge whether the image received is correct. */
	u32  ImgLen;		/*!< Specifies the OTA image length. */
	u32  Offset;		/*!< Specifies the the location in the total firmware file. */
	u32  ImgID;    /*!< Specifies the image ID of the corresponding image. */
} update_file_img_hdr;

/**
  * @brief  OTA firmware file download information structure definition
  */
typedef struct {
	u32 ImgId;		/*!< Specifies the Image ID.*/

	u32	FlashAddr;	/*!< Specifies the Flash Address.
	                         	This parameter is used to write the Image to the flash. */
	u32	ImgOffset;	/*!< Specifies the Image location in Firmware header.
	                         	This parameter indicates the Image location in firmware file. */
	u32	ImageLen;	/*!< Specifies the OTA image length. */
} update_dw_info;

/**
  * @brief  OTA target image header structure definition
  */
typedef struct {
	update_file_hdr	FileHdr;			/*!< Specifies the firmware file header. */
	update_file_img_hdr	FileImgHdr[MAX_IMG_NUM];	/*!< Specifies the target OTA image firmware file header. */
	Manifest_TypeDef Manifest[MAX_IMG_NUM];			/*!< Specifies the manifest of target image. */
	u8 ValidImgCnt;						/*!< Specifies valid image number in file. */
} update_ota_target_hdr;

/**
  * @brief  OTA target image download control information structure definition
  */
typedef struct {
	u8 NextImgFg;			/*!< Specifies the Flag that Buffer belong to next image. */
	u8 NextImgBuf[BUF_SIZE];/*!< Specifies the Buffer belong to next image. */
	int NextImgLen;			/*!< Specifies the Buffer Length belong to next image. */
	int ReadBytes;		/*!< Specifies the Bytes already received. */
	int RemainBytes;		/*!< Specifies the Remain Bytes of OTA firmware. */
	u32 ImgId;		/*!< Specifies the Image ID.*/
	u32 ImgOffset;		/*!< Specifies the Offset of Manifest in firmware. */
	u32	ImageLen;	/*!< Specifies the OTA image length. */
	u32 FlashAddr;			/*!< Specifies the Flash Address.
									This parameter is used to write the Image to the flash. */
	int SigCnt;		/*!< Specifies the Manifest received length. */
	u8 SigFg;		/*!< Specifies the Flag that Manifest received finished. */
	u8 SkipBootOTAFg;	/*!< Specifies the Flag that skip update the bootloader. */
	u8 FirstBufFg;		/*!< Specifies the Flag that exist a buffer before downloading. */
	u8 IsGetOTAHdr;		/*!< Specifies the Flag that get ota target header. */
	u8 IsDnldInit;		/*!< Specifies the Flag that download initialize. */
	u8 targetIdx;		/*!< Specifies the ota target index. */
	int index;			/*!< Specifies the current image index. */
} update_ota_ctrl_info;

/**
  * @brief  OTA ssl structure definition
  */
typedef struct {
	mbedtls_ssl_context ssl;
	mbedtls_ssl_config conf;
	mbedtls_x509_crt ca;
	mbedtls_x509_crt cert;
	mbedtls_pk_context key;
} update_tls;

/**
  * @brief  OTA http redirect connection structure definition
  */
typedef struct {
	char *url;
	int len;
	u16 port;
	char *host;
	char *resource;
} update_redirect_conn;

typedef void (*ota_progress_cb_t)(int percent);

/**
  * @brief  OTA context structure definition
  */
typedef struct {
	char *host;
	int port;
	char *resource;
	char *ca_cert;
	char *client_cert;
	char *private_key;
	int fd;
	u8 type;
	update_tls *tls;
	update_ota_ctrl_info *otactrl;
	update_redirect_conn *redirect;
	update_ota_target_hdr *otaTargetHdr;
	ota_progress_cb_t progress_cb;
} ota_context;

/* Exported functions --------------------------------------------------------*/
/** @defgroup OTA_Exported_Functions OTA Exported Functions
  * @{
  */
u32 ota_get_cur_index(u32 idx);
u32 verify_ota_checksum(update_ota_target_hdr *pOtaTgtHdr, u8 targetIdx, int index);
u32 ota_update_manifest(update_ota_target_hdr *pOtaTgtHdr, u32 ota_target_index, int index);
int ota_update_init(ota_context *ctx, char *host, int port, char *resource, u8 type);
void ota_update_deinit(ota_context *ctx);
int ota_update_start(ota_context *ctx);
int ota_update_fw_program(ota_context *ctx, u8 *buf, u32 len);
int ota_update_register_progress_cb(ota_context *ctx, ota_progress_cb_t cb);

#define OTA_GET_FWVERSION(address) \
	(HAL_READ16(SPI_FLASH_BASE, address + 22) << 16) | HAL_READ16(SPI_FLASH_BASE, address + 20)

#define _OTA_INFO_	3
#define _OTA_WARN_	2
#define _OTA_ERR_	1

#define OTA_TAG	"OTA"

#define ota_printf(level, fmt, arg...)     \
do {\
	if(level == _OTA_INFO_) \
		RTK_LOGI(OTA_TAG, fmt"\r\n" , ##arg);\
	else if(level == _OTA_WARN_) \
		RTK_LOGW(OTA_TAG, fmt"\r\n" , ##arg);\
	else if(level == _OTA_ERR_) \
		RTK_LOGE(OTA_TAG, fmt"\r\n" , ##arg);\
}while(0)

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

#endif //_AMEBA_OTA_H_