/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _OTA_INTERNAL_H_
#define _OTA_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ameba_secure_boot.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "ameba_ota.h"

/** @addtogroup Ameba_Platform
  * @{
  */
#define OTA_TAG            "OTA"

#define OTA_BUF_SIZE        2048   /*the size of the buffer used for receiving firmware data from server*/
#define OTA_HEADER_BAK_LEN  32
#define OTA_HEADER_LEN      8
#define OTA_SUB_HEADER_LEN  24

/* ========== Type Aliases ========== */

// /**
//   * @brief  OTA Image ID
//   */
// typedef enum {
// 	OTA_IMGID_BOOT = 0,  /* Bootloader */
// 	OTA_IMGID_APP = 1,   /* Application */
// 	OTA_IMGID_APIMG = 2, /* AP Image (Linux)*/
// 	OTA_IMGID_MAX,
// } ota_imgid_t;

/**
 * @brief  OTA Transport Type
 */
typedef enum {
	OTA_USER = 0,  /* User-defined */
	OTA_HTTP = 1,  /* HTTP transport */
	OTA_HTTPS = 2, /* HTTPS transport */
	OTA_VFS = 3,   /* VFS file system */
} ota_type_t;

/**
  * @brief  OTA Manifest type alias
  * @note   Equivalent to AuthHeader_TypeDef
  */
typedef AuthHeader_TypeDef ota_manifest_t;

/**
  * @brief  OTA Return Values
  */
typedef enum {
	OTA_ERR = -1,
	OTA_OK = 0,
	OTA_FINISH = 1,
} ota_ret_t;


/**
  * @brief  OTA firmware file header structure definition
  */
typedef struct {
	u32	FwVer;  /*!< Specifies the OTA firmware verision.
	                         This parameter is in first Dword in the firmware file. */
	u32	HdrNum; /*!< Specifies the OTA firmware header number.
	                         This parameter indicates how many headers in firmware file. */
} ota_hdr_t;

/**
  * @brief  OTA firmware file image header structure definition
  */
typedef struct {
	u8 Signature[4];  /*!< Specifies the OTA image signature, the value is "OTA".
	                         	This parameter is used to identify the OTA header needed. */
	u32 ImgHdrLen;    /*!< Specifies the OTA image header length.
	                         	This parameter indicates the Image Header Length. */
	u32 Checksum;     /*!< Specifies the OTA image checksum.
	                         	This parameter is used to judge whether the image received is correct. */
	u32 ImgLen;       /*!< Specifies the OTA image length. */
	u32 Offset;       /*!< Specifies the the location in the total firmware file. */
	u32 ImgID;        /*!< Specifies the image ID of the corresponding image. */
} ota_sub_hdr_t;

/**
  * @brief  OTA header manager structure definition
  */
typedef struct {
	ota_hdr_t FileHdr;                      /*!< Specifies the firmware file header. */
	ota_sub_hdr_t FileImgHdr[OTA_IMGID_MAX];/*!< Specifies the target OTA image firmware file header. */
	ota_manifest_t Manifest[OTA_IMGID_MAX]; /*!< Specifies the manifest of target image. */
	u8 ValidImgCnt;                         /*!< Specifies valid image number in file. */
} ota_hdr_manager_t;

/**
  * @brief  OTA download control information structure definition
  */
typedef struct {
	u8 NextImgFg;           /*!< Specifies the Flag that Buffer belong to next image. */
	u8 NextImgBuf[OTA_BUF_SIZE];  /*!< Specifies the Buffer belong to next image. */
	int NextImgLen;         /*!< Specifies the Buffer Length belong to next image. */
	int ReadBytes;          /*!< Specifies the Bytes already received. */
	int RemainBytes;        /*!< Specifies the Remain Bytes of OTA firmware. */
	u32 ImgId;              /*!< Specifies the Image ID.*/
	u32 ImgOffset;          /*!< Specifies the Offset of Manifest in firmware. */
	u32 ImageLen;           /*!< Specifies the OTA image length. */
	u32 FlashAddr;          /*!< Specifies the Flash Address.
									This parameter is used to write the Image to the flash. */
	int SigCnt;             /*!< Specifies the Manifest received length. */
	u8 SigFg;               /*!< Specifies the Flag that Manifest received finished. */
	u8 SkipBootOTAFg;       /*!< Specifies the Flag that skip update the bootloader. */

	u8 IsGetOTAHdr;         /*!< Specifies the Flag that get ota target header. */
	u8 IsDnldInit;          /*!< Specifies the Flag that download initialize. */
	u8 slotIdx;           /*!< Specifies the ota target slot index. */
	u8 index;              /*!< Specifies the current image index. */
} ota_download_ctrl_t;

/**
  * @brief  TLS Context
  */
typedef struct {
	mbedtls_ssl_context ssl;
	mbedtls_ssl_config  conf;
	mbedtls_x509_crt    ca;
	mbedtls_x509_crt    cert;
	mbedtls_pk_context  key;
} ota_tls_t;

/**
  * @brief  HTTP Redirect Connection
  */
typedef struct {
	char *url;
	int len;
	u16 port;
	char *host;
	char *resource;
} ota_http_redirect_t;

/**
 * @brief  HTTP Response Result
 */
typedef struct {
	u32  status_code;        /* HTTP status code */
	u32  parse_status;       /* Parse status */
	u32  body_len;           /* Response body length */
	u32  header_len;         /* Response header length */
	u8  *header_bak;         /* Response header backup */
	u8  *body;               /* Response body pointer */
} ota_http_response_t;

/* ========== Register function =========== */
typedef void (*ota_progress_cb_t)(int percent);
typedef int (*ota_user_read_func_t)(u8 *buf, int len);
typedef int (*ota_user_open_func_t)(void);

/**
  * @brief  OTA context structure definition
  */
typedef struct {
	/** @brief Server hostname or IP address (e.g., "ota.example.com" or "192.168.1.100") */
	char *host;

	/** @brief Server port number (typically 80 for HTTP, 443 for HTTPS) */
	int port;

	/** @brief Resource path on server (e.g., "/image/ota_all.bin") */
	char *resource;

	/** @brief CA certificate for server authentication (PEM format, NULL if not using TLS) */
	char *ca_cert;

	/** @brief Client certificate for mutual TLS authentication (PEM format, NULL if not required) */
	char *client_cert;

	/** @brief Private key for client certificate (PEM format, NULL if not required) */
	char *private_key;

	/** @brief Socket file descriptor for network connection (-1 if not connected) */
	int fd;

	/** @brief Download type identifier */
	u8 type;

	/** @brief TLS context for secure connections (NULL if type != OTA_TYPE_HTTPS) */
	ota_tls_t *tls;

	/** @brief Download control structure managing transfer state and statistics */
	ota_download_ctrl_t *otaCtrl;

	/** @brief HTTP redirect handler for 302 responses (NULL if redirect not supported) */
	ota_http_redirect_t *redirect;

	/** @brief OTA header manager for manifest/header handling */
	ota_hdr_manager_t *otaHdrManager;

	/** @brief Progress callback function invoked during download (NULL for default)
	 */
	ota_progress_cb_t progress_cb;

	/** @brief User-defined read function for custom data sources (NULL for default, set for type == OTA_TYPE_USER)
	 */
	ota_user_read_func_t user_read_func;

	/** @brief User-defined open function for custom connection handling (NULL for default, set for type == OTA_TYPE_USER)
	 */
	ota_user_open_func_t user_open_func;
} ota_context_t;


/* ========== Storage Layer API ========== */

/**
 * @brief  Initialize storage layer
 */
int ota_storage_init(void);

/**
 * @brief  Get target image address
 */
u32 ota_storage_get_image_addr(u8 imgId, u8 slotIdx);

/**
 * @brief  Verify image by checksum
 */
int ota_storage_verify_checksum(ota_hdr_manager_t *pOtaTgtHdr, u8 slotIdx, int index);

/**
 * @brief  Flash stream write
 */
int ota_storage_write(u32 addr, u8 *data, u32 len);

/**
 * @brief  Flash stream read
 */
int ota_storage_read(u32 addr, u8 *data, u32 len);

/**
 * @brief  Flash erase sector
 */
void ota_storage_erase_sector(u32 addr);

/**
 * @brief  Update Manifest
 */
int ota_storage_update_manifest(ota_hdr_manager_t *pOtaTgtHdr, u8 slotIdx, u8 hdrIdx);

/**
 * @brief  Check layout
 */
int ota_storage_check_layout(ota_hdr_manager_t *hdr);

/* ========== Transport Layer API ========== */

/**
 * @brief  Initialize transport layer
 */
int ota_transport_init(ota_context_t *ctx, char *host, int port, char *resource);

/**
 * @brief  Transport connection
 */
int ota_transport_connect(ota_context_t *ctx);

/**
 * @brief  Transport disconnection
 */
void ota_transport_disconnect(ota_context_t *ctx);

/**
 * @brief  Read data
 */
int ota_transport_read(ota_context_t *ctx, u8 *buf, int len);

/**
 * @brief  Write data
 */
int ota_transport_write(ota_context_t *ctx, u8 *buf, int len);


/* HTTP/HTTPS */
int ota_http_connect(ota_context_t *ctx);
void ota_http_close(ota_context_t *ctx);
int ota_http_write(ota_context_t *ctx, u8 *buf, int len);
int ota_http_read(ota_context_t *ctx, u8 *buf, int len);

int ota_http_tls_init(ota_context_t *ctx);
void ota_http_tls_free(ota_context_t *ctx);
int ota_http_tls_write(ota_context_t *ctx, u8 *buf, int len);
int ota_http_tls_read(ota_context_t *ctx, u8 *buf, int len);

/* VFS */
int ota_vfs_open(ota_context_t *ctx);
void ota_vfs_close(ota_context_t *ctx);
int ota_vfs_read(ota_context_t *ctx, u8 *buf, int len);

void *rtos_mem_malloc(uint32_t size);
void *rtos_mem_zmalloc(uint32_t size);
void rtos_mem_free(void *pbuf);

#ifdef __cplusplus
}
#endif

#endif //_OTA_INTERNAL_H_
