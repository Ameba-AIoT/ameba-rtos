/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ameba_ota.h"
#include "lwip_netconf.h"
#include "flash_api.h"

u32 IMG_ADDR[OTA_IMGID_MAX][2] = {0}; /* IMG Flash Physical Address use for OTA */

/**
  * @brief  get current image2 location
  * @param  image index
  * @retval  The retval can be one of the followings:
  *              OTA_INDEX_1: current images located in OTA1 address space
  *              OTA_INDEX_2: current images located in OTA2 address space
  */
u32 ota_get_cur_index(u32 idx)
{
	u32 PhyAddr;

	RSIP_REG_TypeDef *RSIP = ((RSIP_REG_TypeDef *) RSIP_REG_BASE);
	u32 AddrStart = RSIP->FLASH_MMU[idx].RSIP_REMAPxOR;

	if (IMG_ADDR[idx][OTA_INDEX_1] == 0) {
		flash_get_layout_info(IMG_BOOT, &IMG_ADDR[OTA_IMGID_BOOT][OTA_INDEX_1], NULL);
		flash_get_layout_info(IMG_BOOT_OTA2, &IMG_ADDR[OTA_IMGID_BOOT][OTA_INDEX_2], NULL);
		flash_get_layout_info(IMG_APP_OTA1, &IMG_ADDR[OTA_IMGID_APP][OTA_INDEX_1], NULL);
		flash_get_layout_info(IMG_APP_OTA2, &IMG_ADDR[OTA_IMGID_APP][OTA_INDEX_2], NULL);
		flash_get_layout_info(IMG_DSP, &IMG_ADDR[OTA_IMGID_DSP][OTA_INDEX_1], NULL);
		flash_get_layout_info(IMG_DSP, &IMG_ADDR[OTA_IMGID_DSP][OTA_INDEX_2], NULL);
	}

	if (AddrStart & RSIP_BIT_REMAP_x_ENABLE) {
		PhyAddr = (AddrStart >> 9) << 5;

		if (idx == OTA_IMGID_APP) {
			/*add certificate and manifest*/
			PhyAddr = PhyAddr - 0x2000;

			if (PhyAddr == IMG_ADDR[idx][OTA_INDEX_1]) {
				return OTA_INDEX_1;
			} else {
				return OTA_INDEX_2;
			}
		} else {
			/*add manifest*/
			PhyAddr = PhyAddr - 0x1000;

			if (PhyAddr == IMG_ADDR[idx][OTA_INDEX_1]) {
				return OTA_INDEX_1;
			} else {
				return OTA_INDEX_2;
			}
		}
	}

	return OTA_INDEX_1;
}

/**
  * @brief  check if bootloader OTA2 address is valid
  * @param  none
  * The retval can be one of the followings:
  *              0: OTA2 address is valid
  *              1: OTA2 address is invalid
  */
u8 ota_checkbootloader_ota2(void)
{
	u16 tmp;
	u32 ImgAddr;

	tmp = HAL_READ16(OTPC_REG_BASE, SEC_OTA_ADDR);

	/* Take default OTP value as invalid address */
	if (tmp != 0xFFFF) {
		ImgAddr = tmp << 12;
	} else {
		ImgAddr = 0;
	}

	/* check if OTA2 addr is valid. The OTA2 addr in OTP should be a multiples of 4KB. */
	return ((ImgAddr == IMG_ADDR[OTA_IMGID_BOOT][OTA_INDEX_2]) ? 0 : 1);
}

/**
  * @brief	  parse firmware file header and get the desired OTA header
  * @param   buf: point to buffer for receiving OTA header of new firmware
  * @param   len: data length to be received from server
  * @param   pOtaTgtHdr:point to target image OTA  header
  * @retval 0: receive fail, 1: receive ok
  */
u32 get_ota_tartget_header(ota_context *ctx, u8 *buf, u32 len)
{
	update_file_img_hdr *ImgHdr;
	update_file_hdr *pOtaFileHdr;
	update_ota_target_hdr *pOtaTgtHdr = ctx->otaTargetHdr;
	u8 *pTempAddr;
	u32 i = 0;

	/*check if buf and len is valid or not*/
	if ((len < HEADER_LEN) || (!buf)) {
		return 0;
	}

	pOtaFileHdr = (update_file_hdr *)buf;
	pTempAddr = buf + HEADER_LEN;
	pOtaTgtHdr->ValidImgCnt = 0;

	if (len < (pOtaTgtHdr->FileHdr.HdrNum * SUB_HEADER_LEN + HEADER_LEN)) {
		return 0;
	}

	/*get the target OTA header from the new firmware file header*/
	for (i = 0; i < pOtaFileHdr->HdrNum; i++) {
		pTempAddr = buf + HEADER_LEN + SUB_HEADER_LEN * i;
		ImgHdr = (update_file_img_hdr *)(pTempAddr);

		if (strncmp("OTA", (const char *)ImgHdr->Signature, 3) != 0) {
			return 0;
		}

		_memcpy((void *)(u8 *)(&pOtaTgtHdr->FileImgHdr[pOtaTgtHdr->ValidImgCnt]), (void *)pTempAddr, sizeof(update_file_img_hdr));
		pOtaTgtHdr->ValidImgCnt++;
	}

	if (pOtaTgtHdr->ValidImgCnt == 0) {
		ota_printf(_OTA_INFO_, "[%s] no valid image\n", __FUNCTION__);
		return 0;
	}

	return 1;
}

/**
  * @brief  check if update image length exceeds the layout
  * @param  pOtaTgtHdr: point to target image OTA  header
  * The retval can be one of the followings:
  *              1: update image length is valid
  *              0: update image length is invalid
  */
u8 ota_checkimage_layout(update_ota_target_hdr *pOtaTgtHdr)
{
	u32 end_addr = 0;
	u8 targetIdx = OTA_INDEX_1;

	for (int index = 0; index < pOtaTgtHdr->ValidImgCnt; index++) {
		if (ota_get_cur_index(pOtaTgtHdr->FileImgHdr[index].ImgID) == OTA_INDEX_1) {
			targetIdx = OTA_INDEX_2;
		}

		if (pOtaTgtHdr->FileImgHdr[index].ImgID == OTA_IMGID_BOOT) {
			if (targetIdx == OTA_INDEX_1) {
				flash_get_layout_info(IMG_BOOT, NULL, &end_addr);
			} else {
				flash_get_layout_info(IMG_BOOT_OTA2, NULL, &end_addr);
			}
		} else if (pOtaTgtHdr->FileImgHdr[index].ImgID == OTA_IMGID_APP) {
			if (targetIdx == OTA_INDEX_1) {
				flash_get_layout_info(IMG_APP_OTA1, NULL, &end_addr);
			} else {
				flash_get_layout_info(IMG_APP_OTA2, NULL, &end_addr);
			}
		} else if (pOtaTgtHdr->FileImgHdr[index].ImgID == OTA_IMGID_DSP) {
			flash_get_layout_info(IMG_DSP, NULL, &end_addr);
		}

		u32 start_addr = IMG_ADDR[pOtaTgtHdr->FileImgHdr[index].ImgID][targetIdx];

		if ((end_addr - start_addr) < pOtaTgtHdr->FileImgHdr[index].ImgLen) {
			ota_printf(_OTA_ERR_, "ImgID: %lu, OTA%d start addr: 0x%08X, end addr: 0x%08X, OTA image Length(%d) > Layout(%d)!!!\n",
					   pOtaTgtHdr->FileImgHdr[index].ImgID, targetIdx + 1, (unsigned int)start_addr, (unsigned int)end_addr, pOtaTgtHdr->FileImgHdr[index].ImgLen,
					   (end_addr - start_addr));
			return 0;
		}
	}

	return 1;
}

/**
  * @brief	  verify new firmware checksum.
  * @param  addr: new image address
  * @param  len: new image length
  * @param  signature: point to signature strings
  * @param  pOtaTgtHdr: point to target image OTA  header
  * @retval 0: verify fail, 1: verify ok
  */
u32 verify_ota_checksum(update_ota_target_hdr *pOtaTgtHdr, u8 targetIdx, int index)
{
	u32 i;
	u32 flash_checksum = 0;
	u32 addr;
	u32 len;
	Manifest_TypeDef *manifest = NULL;

	addr = IMG_ADDR[pOtaTgtHdr->FileImgHdr[index].ImgID][targetIdx];
	len = pOtaTgtHdr->FileImgHdr[index].ImgLen - sizeof(Manifest_TypeDef);
	manifest = &pOtaTgtHdr->Manifest[index];

	/*add signature's checksum*/
	for (i = 0; i < sizeof(Manifest_TypeDef); i++) {
		flash_checksum += *((u8 *)manifest + i);
	}

	/* add flash data's checksum */
	for (i = 0; i < len; i++) {
		flash_checksum += *((u8 *)(addr + i + sizeof(Manifest_TypeDef)));

	}

	if (flash_checksum != pOtaTgtHdr->FileImgHdr[index].Checksum) {
		ota_printf(_OTA_ERR_, "OTA image(%08x) checksum error!!!\nCalculated checksum 0x%8x, host checksum 0x%8x\n", (unsigned int)addr, \
				   (unsigned int)flash_checksum, (unsigned int)pOtaTgtHdr->FileImgHdr[index].Checksum);
		return 0;
	} else {
		ota_printf(_OTA_INFO_, "OTA image(%08x) checksum ok!!!\n", (unsigned int)addr);
	}
	return 1;
}

/**
  * @brief	  update manifest.
  * @param  addr: new image address
  * @retval 0: update fail, 1: update ok
  */
u32 ota_update_manifest(update_ota_target_hdr *pOtaTgtHdr, u32 ota_target_index, int index)
{
	u32 addr;
	Manifest_TypeDef *manifest = NULL;
	flash_t flash;

#if OTA_CLEAR_PATTERN
	u8 ota_old_index = ota_target_index ^ 1;
	u8 empty_sig[8] = {0x0};
#endif

	addr = IMG_ADDR[pOtaTgtHdr->FileImgHdr[index].ImgID][ota_target_index];
	manifest = &pOtaTgtHdr->Manifest[index];

	ota_printf(_OTA_INFO_, "update addr: 0x%08x\n", (unsigned int)addr);
	ota_printf(_OTA_INFO_, "update version major: %d, minor: %d\n", manifest->MajorImgVer, manifest->MinorImgVer);

	/*write the manifest, write pattern finally*/
	flash_stream_write(&flash, addr - SPI_FLASH_BASE + 8, sizeof(Manifest_TypeDef) - 8, (u8 *)manifest + 8);
	flash_stream_write(&flash, addr - SPI_FLASH_BASE, 8, (u8 *)manifest->Pattern);

	if (pOtaTgtHdr->FileImgHdr[index].ImgID == OTA_IMGID_DSP) {
		goto finish;
	}

#if OTA_CLEAR_PATTERN
	if (strncmp("OTA", (const char *)pOtaTgtHdr->FileImgHdr[index].Signature, 3) == 0) {
		addr = IMG_ADDR[pOtaTgtHdr->FileImgHdr[index].ImgID][ota_old_index];
	} else {
		return 0;
	}

	/*clear the old FW pattern to 0 finally*/
	ota_printf(_OTA_INFO_, "ImgID: %lu, clear the old FW pattern, addr: 0x%08x\n", pOtaTgtHdr->FileImgHdr[index].ImgID, (unsigned int)addr);
	flash_stream_write(&flash, addr - SPI_FLASH_BASE, 8, empty_sig);
#endif
finish:
	ota_printf(_OTA_INFO_, "[%s] Update OTA success!\n", __FUNCTION__);

	return 1;
}

int parser_url(char *url, char *host, u16 *port, char *resource, int len)
{
	if (url) {
		char *http = NULL, *pos = NULL;

		http = strstr(url, "http://");
		if (http) { // remove http
			url += strlen("http://");
		}
		_memset(host, 0, len);

		pos = strstr(url, ":");	// get port
		if (pos) {
			_memcpy(host, url, (pos - url));
			pos += 1;
			*port = atoi(pos);
		} else {
			pos = strstr(url, "/");
			if (pos) {
				_memcpy(host, url, (pos - url));
				url = pos;
			}
			*port = 80;
		}
		ota_printf(_OTA_INFO_, "server: %s\n\r", host);
		ota_printf(_OTA_INFO_, "port: %d\n\r", *port);

		_memset(resource, 0, len);
		pos = strstr(url, "/");
		if (pos) {
			_memcpy(resource, pos + 1, strlen(pos + 1));
		}
		ota_printf(_OTA_INFO_, "resource: %s\n\r", resource);

		return 0;
	}
	return -1;
}

static int ota_ssl_random(void *p_rng, u8 *output, size_t output_len)
{
	(void) p_rng;
	TRNG_get_random_bytes(output, output_len);
	return 0;
}

static int ota_ssl_verify(void *data, mbedtls_x509_crt *crt, int depth, int *flags)
{
	(void) data;
	(void) depth;
	char *buf = NULL;

	buf = (char *)rtos_mem_zmalloc(1024);
	if (!buf) {
		return 0;
	}

	mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "", crt);

	if (*flags) {
		ota_printf(_OTA_INFO_, "\nERROR: certificate verify\n%s\n", buf);
	} else {
		ota_printf(_OTA_INFO_, "\nCertificate verified\n%s\n", buf);
	}
	rtos_mem_free(buf);
	return 0;
}

int ota_update_tls_new(ota_context *ctx)
{
	int ret = -1;

	ctx->tls = (update_tls *)rtos_mem_malloc(sizeof(update_tls));
	if (!ctx->tls) {
		ota_printf(_OTA_ERR_, "%s, tls malloc failed", __func__);
		return -1;
	}

	mbedtls_ssl_context *ssl = &ctx->tls->ssl;
	mbedtls_ssl_config *conf = &ctx->tls->conf;
	mbedtls_x509_crt *ca = &ctx->tls->ca;
	mbedtls_x509_crt *cert = &ctx->tls->cert;
	mbedtls_pk_context *key = &ctx->tls->key;

	ota_printf(_OTA_INFO_, "  . Setting up the SSL/TLS structure...");

	mbedtls_ssl_init(ssl);
	mbedtls_ssl_config_init(conf);
	mbedtls_x509_crt_init(ca);
	mbedtls_x509_crt_init(cert);
	mbedtls_pk_init(key);

	if ((ret = mbedtls_ssl_config_defaults(conf,
										   MBEDTLS_SSL_IS_CLIENT,
										   MBEDTLS_SSL_TRANSPORT_STREAM,
										   MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {

		ota_printf(_OTA_ERR_, "ERROR: mbedtls_ssl_config_defaults ret(%d)\n", ret);
		return -1;
	}

	mbedtls_ssl_conf_rng(conf, ota_ssl_random, NULL);

	if ((ret = mbedtls_ssl_setup(ssl, conf)) != 0) {
		ota_printf(_OTA_ERR_, "ERROR: mbedtls_ssl_setup ret(%d)\n", ret);
		return -1;
	}

	ota_printf(_OTA_INFO_, " ok\n");

	mbedtls_ssl_set_bio(ssl, &ctx->fd, mbedtls_net_send, mbedtls_net_recv, NULL);

	if (ctx->ca_cert != NULL) {
		if (mbedtls_x509_crt_parse(ca, (const unsigned char *)ctx->ca_cert, strlen(ctx->ca_cert) + 1) != 0) {
			ota_printf(_OTA_ERR_, "parse ca_crt failed!");
			return -1;
		}
		mbedtls_ssl_conf_ca_chain(conf, ca, NULL);
		mbedtls_ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_REQUIRED);
		mbedtls_ssl_conf_verify(conf, (int (*)(void *, mbedtls_x509_crt *, int, uint32_t *))ota_ssl_verify, NULL);
		ota_printf(_OTA_INFO_, "ca_crt parse done");
	} else {
		mbedtls_ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_NONE);
	}

	if (ctx->client_cert != NULL && ctx->private_key != NULL) {
		if (mbedtls_x509_crt_parse(cert, (const unsigned char *)ctx->client_cert, strlen(ctx->client_cert) + 1) != 0) {
			ota_printf(_OTA_ERR_, "parse client_crt failed!");
			return -1;
		}

		if (mbedtls_pk_parse_key(key, (const unsigned char *)ctx->private_key, strlen(ctx->private_key) + 1, NULL, 0, NULL, NULL) != 0) {
			ota_printf(_OTA_ERR_, "parse private_key failed!");
			return -1;
		}

		mbedtls_ssl_conf_own_cert(conf, cert, key);
	}

	return 0;
}

int ota_update_conn_write(ota_context *ctx, u8 *data, int data_len)
{
	if (ctx->type == OTA_HTTPS) {
		return mbedtls_ssl_write(&ctx->tls->ssl, data, data_len);
	}
	return write(ctx->fd, data, data_len);
}

int ota_update_conn_read(ota_context *ctx, u8 *data, int data_len)
{
	int bytes_rcvd = -1;

	if (ctx->fd < 0) {
		ota_printf(_OTA_INFO_, "[%s], socket is invalid\n", __FUNCTION__);
		return bytes_rcvd;
	}

	if (ctx->type == OTA_HTTPS) {
		bytes_rcvd = mbedtls_ssl_read(&ctx->tls->ssl, data, data_len);
	} else {
		bytes_rcvd = read(ctx->fd, data, data_len);
	}
	if (bytes_rcvd < 0) {
		ota_printf(_OTA_INFO_, "[%s], Close HTTP Socket[%d].\n", __FUNCTION__, ctx->fd);
		return -2;
	}
	return bytes_rcvd;
}

int ota_update_http_send_request(ota_context *ctx)
{
	u8 *request = NULL;
	int ret = -1;

	request = (u8 *)rtos_mem_malloc(strlen("GET /") + strlen(ctx->resource) + strlen(" HTTP/1.1\r\nHost: ")
									+ strlen(ctx->host) + strlen("\r\n\r\n") + 1);
	sprintf((char *)request, "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", ctx->resource, ctx->host);
	ret = ota_update_conn_write(ctx, request, strlen((char *)request));
	rtos_mem_free(request);
	return ret;
}

/**
  * @brief  parse http response.
  * @param  response: the http response got from server
  * @param  response_len: The length of http response
  * @param  result: The struct that store the useful info from the http response
  * @retval  1:only got status code;3:got status code and content length,but not get the full header;4: got all info;-1:failed
  */
int ota_update_http_parse_response(ota_context *ctx, u8 *response, u32 response_len, http_response_result_t *result)
{
	update_redirect_conn *redirect = ctx->redirect;
	char *ptr_body_end;

	redirect->port = 0;
	//Get status code
	if (0 == result->parse_status) { //didn't get the http response
		char *crlf_ptr = strstr((char *)response, "\r\n");
		if (crlf_ptr == NULL) { //didn't get the status code
			return -1;
		}
		result->status_code = atoi((char const *)response + 9); //get the status code
		if (result->status_code == 200) {
			result->parse_status = 1;
		} else if (result->status_code == 302) {
			char *tmp = NULL;
			const char *location1 = "LOCATION";
			const char *location2 = "Location";
			ota_printf(_OTA_INFO_, "response 302:%s \r\n", response);

			if ((tmp = strstr((char *)response, location1)) || (tmp = strstr((char *)response, location2))) {
				redirect->len = strlen(tmp + 10);
				ota_printf(_OTA_INFO_, "Location len = %d\r\n", redirect->len);
				if (redirect->url == NULL) {
					redirect->url = (char *)rtos_mem_malloc(redirect->len);
					if (redirect->url == NULL) {
						return -1;
					}
				}
				_memset(redirect->url, 0, redirect->len);
				_memcpy(redirect->url, tmp + 10, strlen(tmp + 10));
			}
			rtos_mem_free(ctx->host);
			ctx->host = (char *)rtos_mem_zmalloc(redirect->len);
			if (ctx->host == NULL) {
				return -1;
			}
			rtos_mem_free(ctx->resource);
			ctx->resource = (char *)rtos_mem_zmalloc(redirect->len);
			if (ctx->resource == NULL) {
				return -1;
			}

			if (parser_url(redirect->url, ctx->host, &redirect->port, ctx->resource, redirect->len) < 0) {
				rtos_mem_free(redirect->url);
				return -1;
			}
			rtos_mem_free(redirect->url);
			return -1;
		} else {
			ota_printf(_OTA_INFO_, "[%s] The http response status code is %lu", __FUNCTION__, result->status_code);
			return -1;
		}
	}

	//if didn't receive the full http header
	if (3 == result->parse_status) { //didn't get the http response
		if (NULL != (ptr_body_end = strstr((char *)response, "\r\n\r\n"))) {
			result->parse_status = 4;
			result->header_len = (u32)ptr_body_end - (u32)response + 4;
			result->body = response + result->header_len;
		}

		if (3 == result->parse_status) {//Still didn't receive the full header
			result->header_bak = rtos_mem_malloc(HEADER_BAK_LEN + 1);
			_memset(result->header_bak, 0, strlen((const char *)result->header_bak));
			_memcpy(result->header_bak, response + response_len - HEADER_BAK_LEN, HEADER_BAK_LEN);
		}
	}

	ota_printf(_OTA_INFO_, "response: %s\n", response);

	//Get Content-Length
	if (1 == result->parse_status) { //didn't get the content length
		char *tmp_ptr;

		/* parse response_content_len */
		if (NULL != (tmp_ptr = strstr((char *)response, "Content-Length")) ||
			NULL != (tmp_ptr = strstr((char *)response, "CONTENT-LENGTH"))) {
			result->body_len = atoi(tmp_ptr + strlen("Content-Length: "));
			result->parse_status = 2;
		}
		if (NULL != (ptr_body_end = strstr((char *)response, "\r\n\r\n"))) {
			if (result->parse_status == 2) { //get the full header and the content length
				result->parse_status = 4;
				result->header_len = (u32)ptr_body_end - (u32)response + 4;
				result->body = response + result->header_len;
			} else { //there are no content length in header
				ota_printf(_OTA_INFO_, "[%s] No Content-Length in header", __FUNCTION__);
				return -1;
			}
		}

		if (1 == result->parse_status) {//didn't get the content length and the full header
			result->header_bak = rtos_mem_malloc(HEADER_BAK_LEN + 1);
			_memset(result->header_bak, 0, strlen((char *)result->header_bak));
			_memcpy(result->header_bak, response + response_len - HEADER_BAK_LEN, HEADER_BAK_LEN);
		} else if (2 == result->parse_status) { //didn't get the full header but get the content length
			result->parse_status = 3;
			result->header_bak = rtos_mem_malloc(HEADER_BAK_LEN + 1);
			_memset(result->header_bak, 0, strlen((char *)result->header_bak));
			_memcpy(result->header_bak, response + response_len - HEADER_BAK_LEN, HEADER_BAK_LEN);
		}
	}

	return result->parse_status;
}

int ota_update_http_recv_response(ota_context *ctx)
{
	int read_bytes = 0;
	int ret = -1;
	http_response_result_t rsp_result = {0};
	u32 idx = 0;
	u8 *buf = NULL;
	int buf_size = BUF_SIZE;

	buf = (u8 *)rtos_mem_zmalloc(buf_size);
	if (!buf) {
		ota_printf(_OTA_ERR_, "[%s] Alloc buffer failed\n", __FUNCTION__);
		goto exit;
	}

	while (3 >= rsp_result.parse_status) { //still read header
		if (0 == rsp_result.parse_status) { //didn't get the http response
			_memset(buf, 0, buf_size);
			read_bytes = ota_update_conn_read(ctx, buf, buf_size);
			if (read_bytes <= 0) {
				ota_printf(_OTA_ERR_, "[%s] Read socket failed\n", __FUNCTION__);
				goto exit;
			}
			idx = read_bytes;
			_memset(&rsp_result, 0, sizeof(rsp_result));
			if (ota_update_http_parse_response(ctx, buf, idx, &rsp_result) == -1) {
				goto exit;
			}
		} else if ((1 == rsp_result.parse_status) || (3 == rsp_result.parse_status)) { //just get the status code
			_memset(buf, 0, buf_size);
			_memcpy(buf, rsp_result.header_bak, HEADER_BAK_LEN);
			rtos_mem_free(rsp_result.header_bak);
			rsp_result.header_bak = NULL;
			read_bytes = ota_update_conn_read(ctx, buf + HEADER_BAK_LEN, (buf_size - HEADER_BAK_LEN));
			if (read_bytes <= 0) {
				ota_printf(_OTA_ERR_, "[%s] Read socket failed\n", __FUNCTION__);
				goto exit;
			}
			idx = read_bytes + HEADER_BAK_LEN;
			if (ota_update_http_parse_response(ctx, buf, read_bytes + HEADER_BAK_LEN, &rsp_result) == -1) {
				goto exit;
			}
		}
	}

	if (0 == rsp_result.body_len) {
		ota_printf(_OTA_INFO_, "[%s] New firmware size = 0 !\n", __FUNCTION__);
		goto exit;
	} else {
		ota_printf(_OTA_INFO_, "[%s] Download new firmware begin, total size : %lu\n", __FUNCTION__, rsp_result.body_len);
	}

	ota_printf(_OTA_INFO_, "idx = %lu, rsp_result.header_len = %lu\n", idx, rsp_result.header_len);

	ret = idx - rsp_result.header_len;
	/* remove http header_len from buf*/
	if (ret > 0) {
		ctx->otactrl->NextImgLen = ret;
		_memset(ctx->otactrl->NextImgBuf, 0, BUF_SIZE);
		_memcpy((void *)ctx->otactrl->NextImgBuf, (void *)(buf + rsp_result.header_len), ret);
		ctx->otactrl->NextImgFg = 1;
	}

exit:
	if (buf) {
		rtos_mem_free(buf);
	}
	return ret;
}

/**
  * @brief  connect to the OTA http server.
  * @param  server_socket: the socket used
  * @param  host: host address of the OTA server
  * @param  port: port of the OTA server
  * @retval  -1 when connect fail, socket value when connect success
  */
int ota_update_http_connect_server(ota_context *ctx)
{
	struct sockaddr_in server_addr;
	struct hostent *server;
	int fd = -1;
	int ret = -1;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		ota_printf(_OTA_ERR_, "[%s] Create socket failed", __FUNCTION__);
		return -1;
	}
	ctx->fd = fd;
	ota_printf(_OTA_INFO_, "[%s] Create socket: %d success!\n", __FUNCTION__, ctx->fd);

	server = gethostbyname(ctx->host);
	if (server == NULL) {
		ota_printf(_OTA_ERR_, "[ERROR] Get host ip failed\n");
		return -1;
	}

	_memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(ctx->port);
	_memcpy((void *)&server_addr.sin_addr, (void *)server->h_addr, 4);

	if (connect(ctx->fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		ota_printf(_OTA_ERR_, "[%s] Socket connect failed", __FUNCTION__);
		return -1;
	}

	ota_printf(_OTA_INFO_, "[%s] Connect success!\n", __FUNCTION__);

	if (ctx->type == OTA_HTTPS) {
		ret = ota_update_tls_new(ctx);
		if (ret != 0) {
			ota_printf(_OTA_ERR_, "[%s] tls create failed", __FUNCTION__);
			return -1;
		}

		ota_printf(_OTA_INFO_, "  . Performing the SSL/TLS handshake...");

		if ((ret = mbedtls_ssl_handshake(&ctx->tls->ssl)) != 0) {
			ota_printf(_OTA_INFO_, "ERROR: mbedtls_ssl_handshake ret(-0x%x)", -ret);
			return -1;
		}
		ota_printf(_OTA_INFO_, " ok\n");
		ota_printf(_OTA_INFO_, "  . Use ciphersuite %s\n", mbedtls_ssl_get_ciphersuite(&ctx->tls->ssl));
	}
	return 0;
}

int ota_update_http_prepare(ota_context *ctx)
{
	int ret = -1;

	ret = ota_update_http_connect_server(ctx);
	if (ret != 0) {
		return -1;
	}

	ret = ota_update_http_send_request(ctx);
	if (!ret) {
		ota_printf(_OTA_ERR_, "[%s] Send HTTP request failed\n", __FUNCTION__);
		return -1;
	}

	ret = ota_update_http_recv_response(ctx);
	if (ret < 0) {
		ota_printf(_OTA_ERR_, "[%s] Parse HTTP response failed\n", __FUNCTION__);
		return -1;
	}

	return 0;
}

int ota_update_http_close(ota_context *ctx)
{
	if (ctx->fd >= 0) {
		close(ctx->fd);
	}

	if (ctx->type == OTA_HTTPS) {
		if (ctx->tls) {
			mbedtls_x509_crt_free(&ctx->tls->ca);
			mbedtls_x509_crt_free(&ctx->tls->cert);
			mbedtls_pk_free(&ctx->tls->key);
			mbedtls_ssl_free(&ctx->tls->ssl);
			mbedtls_ssl_config_free(&ctx->tls->conf);
			rtos_mem_free(ctx->tls);
			ctx->tls = NULL;
		}
	}
	return 0;
}

/**
* @brief  receive OTA firmware file header from server
* @param  Recvbuf: pointer to buffer for receiving OTA header of firmware file
* @param  writelen:the length already read from server
* @param  len: data length to be received from server
* @param  pOtaTgtHdr: point to target image OTA  header
* @param  socket: socket handler
* @retval  0:failed;1:success
*/
u32 recv_ota_file_header(ota_context *ctx, u8 *Recvbuf, u32 writelen, u32 *len)
{
	int read_bytes = 0;
	u32 TempLen;
	u8 *buf;
	update_file_hdr *pOtaFileHdr;
	update_ota_target_hdr *pOtaTgtHdr = ctx->otaTargetHdr;

	buf = Recvbuf + writelen;
	/*receive the remaining OTA firmware file header info if needed*/
	if (writelen < HEADER_LEN) {
		TempLen = HEADER_LEN - writelen;
		while (TempLen) {
			read_bytes = ota_update_conn_read(ctx, buf, TempLen);
			if (read_bytes < 0) {
				ota_printf(_OTA_ERR_, "[%s] read socket failed\n", __FUNCTION__);
				return 0;
			}
			if (read_bytes == 0) {
				break;
			}
			TempLen -= read_bytes;
			buf += read_bytes;
			writelen += read_bytes;
		}
	}

	pOtaFileHdr = (update_file_hdr *)(Recvbuf);

	pOtaTgtHdr->FileHdr.FwVer = pOtaFileHdr->FwVer;
	pOtaTgtHdr->FileHdr.HdrNum = pOtaFileHdr->HdrNum;

	ota_printf(_OTA_INFO_, "ota header num: %lu\n", pOtaTgtHdr->FileHdr.HdrNum);

	if (pOtaTgtHdr->FileHdr.HdrNum > MAX_IMG_NUM) {
		ota_printf(_OTA_ERR_, "ota header num: %lu is invaild\n", pOtaTgtHdr->FileHdr.HdrNum);
		return 0;
	}

	/*read the remaining Header info*/
	if (writelen < (pOtaFileHdr->HdrNum * SUB_HEADER_LEN + HEADER_LEN)) {
		buf = Recvbuf + writelen;
		TempLen = pOtaFileHdr->HdrNum * SUB_HEADER_LEN + HEADER_LEN - writelen;

		while (TempLen) {
			read_bytes = ota_update_conn_read(ctx, buf, TempLen);
			TempLen -= read_bytes;
			buf += read_bytes;
		}
	}

	*len = (pOtaFileHdr->HdrNum * SUB_HEADER_LEN) + HEADER_LEN;
	return 1;
}

void download_percentage(ota_context *ctx, int cur_size, int total_size)
{
	static int percent_bak = -1;
	int percent = cur_size * 100 / total_size;
	if (percent_bak != percent && percent % 10 == 0) {
		percent_bak = percent;
		ota_printf(_OTA_INFO_, "ota download percentage: %d", percent);
	}
	if (ctx->progress_cb) {
		ctx->progress_cb(percent);
	}
}

void download_parameter_init(ota_context *ctx)
{
	update_ota_ctrl_info *otaCtrl = ctx->otactrl;
	update_ota_target_hdr *pOtaTgtHdr = ctx->otaTargetHdr;

	otaCtrl->ImgId = pOtaTgtHdr->FileImgHdr[otaCtrl->index].ImgID;

	/* -----step4: check OTA index we should update according to image2-------*/
	if (ota_get_cur_index(otaCtrl->ImgId) == OTA_INDEX_1) {
		otaCtrl->targetIdx = OTA_INDEX_2;
	} else {
		otaCtrl->targetIdx = OTA_INDEX_1;
	}
	ota_printf(_OTA_INFO_, "[%s] OTA%d address space will be upgraded\n", __FUNCTION__, otaCtrl->targetIdx + 1);

	/*update flag status*/
	otaCtrl->SigFg = 0;
	otaCtrl->SigCnt = 0;
	otaCtrl->SkipBootOTAFg = 0;

	/*init download information buffer, get OTA image and Write New Image to flash, skip the manifest,
		not write manifest first for power down protection*/
	otaCtrl->FlashAddr = IMG_ADDR[otaCtrl->ImgId][otaCtrl->targetIdx] - SPI_FLASH_BASE;
	otaCtrl->ImageLen = pOtaTgtHdr->FileImgHdr[otaCtrl->index].ImgLen;
	otaCtrl->ImgOffset = pOtaTgtHdr->FileImgHdr[otaCtrl->index].Offset;

	ota_printf(_OTA_INFO_, "ImgId: %lu\n", otaCtrl->ImgId);
	ota_printf(_OTA_INFO_, "FlashAddr: 0x%08x\n", (unsigned int)otaCtrl->FlashAddr);
	ota_printf(_OTA_INFO_, "ImageLen: %lu\n", otaCtrl->ImageLen);
	ota_printf(_OTA_INFO_, "ImgOffset: %lu\n", otaCtrl->ImgOffset);

	/*initialize the reveiving counter*/
	otaCtrl->ImgOffset = 0;
	otaCtrl->ReadBytes = 0;
	otaCtrl->RemainBytes = otaCtrl->ImageLen - sizeof(Manifest_TypeDef);/*skip the manifest structure*/
	otaCtrl->FlashAddr = otaCtrl->FlashAddr + sizeof(Manifest_TypeDef);/*skip the manifest structure*/
	/*check bootloader OTA2*/
	if (otaCtrl->ImgId == OTA_IMGID_BOOT && otaCtrl->targetIdx == OTA_INDEX_2) {
		otaCtrl->SkipBootOTAFg = ota_checkbootloader_ota2();
	}

	ota_printf(_OTA_INFO_, "ReadBytes: %d, ImgOffset: %lu\n", otaCtrl->ReadBytes, otaCtrl->ImgOffset);
}

int download_packet_process(ota_context *ctx, u8 *buf, int len)
{
	update_ota_ctrl_info *otaCtrl = ctx->otactrl;
	update_ota_target_hdr *pOtaTgtHdr = ctx->otaTargetHdr;
	static Manifest_TypeDef *manifest = NULL;
	static int manifest_size = sizeof(Manifest_TypeDef);
	static u32 write_sector = 0;
	static u32 next_erase_sector = 0;
	static int size = 0;
	static flash_t flash;
	int TempCnt = 0;

	if (otaCtrl->IsDnldInit == 0) {
		/*downloading parse the OTA and RDP image from the data stream sent by server*/
		download_parameter_init(ctx);
		otaCtrl->IsDnldInit = 1;
		manifest = &pOtaTgtHdr->Manifest[otaCtrl->index];
		write_sector = 0;
		next_erase_sector = 0;
		size = 0;
	}

	otaCtrl->ReadBytes += len;

	if (otaCtrl->ReadBytes > (int)otaCtrl->ImgOffset) {
		if (!otaCtrl->SigFg) {
			/*reach the desired image, the first packet process*/
			otaCtrl->SigFg = 1;
			TempCnt = otaCtrl->ReadBytes - otaCtrl->ImgOffset;
			otaCtrl->SigCnt = TempCnt < manifest_size ? TempCnt : manifest_size;
			_memcpy((void *)(u8 *)manifest, (void *)(buf + (len - TempCnt)), otaCtrl->SigCnt);

			if (otaCtrl->SigCnt < manifest_size || TempCnt - manifest_size == 0) {
				return size;
			}

			buf = buf + (len - TempCnt + manifest_size);
			len = TempCnt - manifest_size;
		} else {
			/*normal packet process*/
			if (otaCtrl->SigCnt < manifest_size) {
				if (len < (int)(manifest_size - otaCtrl->SigCnt)) {
					_memcpy((void *)((u8 *)manifest + otaCtrl->SigCnt), (void *)buf, len);
					otaCtrl->SigCnt += len;
					return size;
				} else {
					ota_printf(_OTA_INFO_, "manifest_size: %d, otaCtrl->SigCnt: %d, len: %d, (manifest_size - otaCtrl->SigCnt): %d\n",
							   manifest_size, otaCtrl->SigCnt, len, manifest_size - otaCtrl->SigCnt);

					_memcpy((void *)((u8 *)manifest + otaCtrl->SigCnt), (void *)buf, manifest_size - otaCtrl->SigCnt);

					buf = buf + (manifest_size - otaCtrl->SigCnt);
					len -= (manifest_size - otaCtrl->SigCnt) ;
					otaCtrl->SigCnt = manifest_size;

					if (!len) {
						return size;
					}
				}
			}
		}
	}

	otaCtrl->RemainBytes -= len;
	if (otaCtrl->RemainBytes < 0) {
		otaCtrl->NextImgLen = -otaCtrl->RemainBytes;
		len = len - (-otaCtrl->RemainBytes);
		/* next firmware content */
		if (otaCtrl->NextImgLen > 0) {
			_memcpy((void *)otaCtrl->NextImgBuf, (void *)(buf + len), otaCtrl->NextImgLen);
			otaCtrl->NextImgFg = 1;
		}
	}

	/* skip bootloader */
	if (otaCtrl->SkipBootOTAFg == 1) {
		size += len;
		return size;
	}

	/* erase flash */
	write_sector = (otaCtrl->ImageLen - otaCtrl->RemainBytes - 1 + manifest_size) / 4096;
	if (write_sector >= next_erase_sector) {
		flash_erase_sector(&flash, otaCtrl->FlashAddr + write_sector * 4096);
		next_erase_sector++;
	}
	flash_stream_write(&flash, otaCtrl->FlashAddr + size, len, buf);
	size += len;
	return size;
}

int ota_update_fw_program(ota_context *ctx, u8 *buf, u32 len)
{
	update_ota_ctrl_info *otaCtrl = ctx->otactrl;
	int size = 0;

	if (!otaCtrl->IsGetOTAHdr) {
		u32 RevHdrLen = 0;
		int writelen = 0;
		int MaxHdrLen = HEADER_LEN + SUB_HEADER_LEN * OTA_IMGID_MAX;
		u8 *temp = NULL;

		if (otaCtrl->NextImgFg == 1) {
			writelen = len + otaCtrl->NextImgLen;
		} else {
			writelen = len;
		}
		/* temp is used for cat the NextImgBuf with buf */
		temp = (u8 *)rtos_mem_zmalloc(writelen + MaxHdrLen);
		if (!temp) {
			ota_printf(_OTA_ERR_, "[%s] Alloc buffer failed\n", __FUNCTION__);
			return OTA_RET_ERR;
		}

		if (otaCtrl->NextImgFg == 1) {
			_memcpy(temp, otaCtrl->NextImgBuf, otaCtrl->NextImgLen);
			_memcpy(temp + otaCtrl->NextImgLen, buf, len);
		} else {
			_memcpy(temp, buf, len);
		}

		/*----------------step1: receive firmware file header---------------------*/
		if (!recv_ota_file_header(ctx, temp, writelen, &RevHdrLen)) {
			rtos_mem_free(temp);
			ota_printf(_OTA_ERR_, "[%s] rev firmware header failed", __FUNCTION__);
			return OTA_RET_ERR;
		}

		/* -------step2: parse firmware file header and get the target OTA image header-----*/
		if (!get_ota_tartget_header(ctx, temp, RevHdrLen)) {
			rtos_mem_free(temp);
			ota_printf(_OTA_ERR_, "[%s] get OTA header failed\n", __FUNCTION__);
			return OTA_RET_ERR;
		}

		if (!ota_checkimage_layout(ctx->otaTargetHdr)) {
			rtos_mem_free(temp);
			ota_printf(_OTA_ERR_, "[%s] check image layout failed\n", __FUNCTION__);
			return OTA_RET_ERR;
		}

		if (writelen <= (int)RevHdrLen) {
			rtos_mem_free(temp);
			otaCtrl->IsGetOTAHdr = 1;
			ota_printf(_OTA_INFO_, "[%s] get ota header, writelen: %d, RevHdrLen: %d", __func__, writelen, (int)RevHdrLen);
			return OTA_RET_OK;
		}

		if (otaCtrl->NextImgFg) {
			if (otaCtrl->NextImgLen > (int)RevHdrLen) {
				/* remove ota header from NextImgBuf*/
				_memcpy((void *)ctx->otactrl->NextImgBuf, (void *)(ctx->otactrl->NextImgBuf + RevHdrLen), ctx->otactrl->NextImgLen - RevHdrLen);
				ctx->otactrl->NextImgLen -= RevHdrLen;
			} else {
				/* remove ota header part from buf*/
				buf += (RevHdrLen - otaCtrl->NextImgLen);
				len -= (RevHdrLen - otaCtrl->NextImgLen);
				otaCtrl->NextImgFg = 0;
			}
		} else {
			/* remove ota header part from buf*/
			buf += RevHdrLen;
			len -= RevHdrLen;
		}
		otaCtrl->IsGetOTAHdr = 1;
		rtos_mem_free(temp);
		ota_printf(_OTA_INFO_, "[%s] get ota header, writelen: %d, RevHdrLen: %d", __func__, writelen, (int)RevHdrLen);
	}

	if (otaCtrl->NextImgFg == 1) {
		size = download_packet_process(ctx, otaCtrl->NextImgBuf, otaCtrl->NextImgLen);
		otaCtrl->NextImgFg = 0;
		ota_printf(_OTA_INFO_, "%s, size: %d\n", __func__, size);
	}

	size = download_packet_process(ctx, buf, len);
	download_percentage(ctx, size, otaCtrl->ImageLen);

	if (otaCtrl->RemainBytes <= 0) {
		size += sizeof(Manifest_TypeDef);    //add the manifest length
		download_percentage(ctx, size, otaCtrl->ImageLen);

		ota_printf(_OTA_INFO_, "Update file size: %d bytes, start addr:0x%08x\n", size, (unsigned int)(otaCtrl->FlashAddr + SPI_FLASH_BASE));
		if ((u32)(size) != otaCtrl->ImageLen) {
			ota_printf(_OTA_ERR_, "download new firmware failed\n");
			return OTA_RET_ERR;
		}

		if (otaCtrl->SkipBootOTAFg) {
			ota_printf(_OTA_WARN_, "Bootloader OTA2 address is invalid, skip Bootloader OTA2\n");
			goto download_app;
		}

		/*----------step4: verify checksum and update signature-----------------*/
		if (!verify_ota_checksum(ctx->otaTargetHdr, otaCtrl->targetIdx, otaCtrl->index)) {
			return OTA_RET_ERR;
		}

		if (!ota_update_manifest(ctx->otaTargetHdr, otaCtrl->targetIdx, otaCtrl->index)) {
			ota_printf(_OTA_ERR_, "Change signature failed\n");
			return OTA_RET_ERR;
		}
download_app:
		/*check if another image is needed to download*/
		if (otaCtrl->index < ctx->otaTargetHdr->ValidImgCnt - 1) {
			otaCtrl->index++;
			otaCtrl->IsDnldInit = 0;
			ota_printf(_OTA_INFO_, "[%s] download image index : %d", __func__, otaCtrl->index);
		} else {
			ota_printf(_OTA_INFO_, "[%s] download image end, total image num: %d", __func__, ctx->otaTargetHdr->ValidImgCnt);
			return OTA_RET_FINISH; // download end
		}
	}

	return OTA_RET_OK;
}

int ota_update_s2_download_fw(ota_context *ctx)
{
	u8 *buf;
	update_ota_ctrl_info *otaCtrl = ctx->otactrl;
	int read_bytes;
	int ret = -1;

	/*allocate buffer for downloading image from server*/
	buf = (u8 *)rtos_mem_malloc(BUF_SIZE);
	if (!buf) {
		ota_printf(_OTA_ERR_, "[%s] Alloc buffer failed\n", __FUNCTION__);
		return -1;
	}

	otaCtrl->index = 0;
	otaCtrl->IsGetOTAHdr = 0;
	otaCtrl->IsDnldInit = 0;
	ota_printf(_OTA_INFO_, "[%s] download image index : %d", __func__, otaCtrl->index);

	while (1) {
		_memset(buf, 0, BUF_SIZE);
		read_bytes = ota_update_conn_read(ctx, buf, BUF_SIZE);
		if (read_bytes == 0) {
			ret = 0;
			break; // Read end
		}
		if (read_bytes < 0) {
			ota_printf(_OTA_ERR_, "[%s] Read socket failed", __FUNCTION__);
			ret = -1;
			break;
		}
		ret = ota_update_fw_program(ctx, buf, read_bytes);
		if (ret == OTA_RET_FINISH) {
			ret = 0;
			ota_printf(_OTA_INFO_, "[%s] end", __FUNCTION__);
			break;
		}
		if (ret == OTA_RET_ERR) {
			ota_printf(_OTA_INFO_, "[%s] error", __FUNCTION__);
			break;
		}
	}

	rtos_mem_free(buf);
	return ret;

}

int ota_update_s1_prepare(ota_context *ctx)
{
	int ret = -1;
	update_redirect_conn *redirect = ctx->redirect;

	/*----------------connect and send request to server---------------------*/
	if (ctx->type == OTA_HTTP || ctx->type == OTA_HTTPS) {
		ret = ota_update_http_prepare(ctx);
		if (ret < 0) {
			/* redirect.port != 0 means there is redirect URL can be downloaded*/
			if (redirect->port != 0) {
				ota_printf(_OTA_INFO_, "OTA redirect host: %s, port: %d, resource: %s\n\r", ctx->host, ctx->port, ctx->resource);
				ret = ota_update_http_prepare(ctx);
			}

			if (ret < 0) {
				ota_printf(_OTA_ERR_, "[%s] HTTP prepare failed\n", __FUNCTION__);
				return -1;
			}
		}

	}

	return ret;
}

int ota_update_start(ota_context *ctx)
{
	int ret = -1;

	if (ctx->type == OTA_USER) {
		return ret;
	}

	/*----------------step1: download prepare---------------------*/
	ret = ota_update_s1_prepare(ctx);
	if (ret < 0) {
		goto update_ota_exit;
	}

	/*----------------step2: download firmware---------------------*/
	ret = ota_update_s2_download_fw(ctx);

update_ota_exit:
	if (ctx->type == OTA_HTTP || ctx->type == OTA_HTTPS) {
		ota_update_http_close(ctx);
	}
	return ret;
}

int ota_update_register_progress_cb(ota_context *ctx, ota_progress_cb_t cb)
{
	if (!ctx) {
		ota_printf(_OTA_ERR_, "%s, ctx is null", __func__);
		return -1;
	}
	ctx->progress_cb = cb;
	return 0;
}

int ota_update_connection_params_init(ota_context *ctx, char *host, int port, char *resource)
{
	if (ctx->type == OTA_USER) {
		return 0;
	}

	if (ctx->type == OTA_HTTP || ctx->type == OTA_HTTPS) {
		if (!host) {
			ota_printf(_OTA_ERR_, "%s, host can't be null", __func__);
			return -1;
		} else {
			ctx->host = (char *)rtos_mem_malloc(strlen(host) + 1);
			if (ctx->host) {
				strcpy(ctx->host, host);
			} else {
				ota_printf(_OTA_ERR_, "%s, host malloc failed", __func__);
				return -1;
			}
		}
	}

	if (!resource) {
		ota_printf(_OTA_ERR_, "%s, resourse can't be null", __func__);
		return -1;
	}

	ctx->resource = (char *)rtos_mem_malloc(strlen(resource) + 1);
	if (ctx->resource) {
		strcpy(ctx->resource, resource);
	} else {
		ota_printf(_OTA_ERR_, "%s, resource malloc failed", __func__);
		return -1;
	}
	ctx->port = port;
	ctx->fd = -1;
	return 0;
}

int ota_update_init(ota_context *ctx, char *host, int port, char *resource, u8 type)
{
	update_ota_ctrl_info *otactrl = NULL;
	update_redirect_conn *redirect = NULL;
	update_ota_target_hdr *otaTargetHdr = NULL;

	flash_get_layout_info(IMG_BOOT, &IMG_ADDR[OTA_IMGID_BOOT][OTA_INDEX_1], NULL);
	flash_get_layout_info(IMG_BOOT_OTA2, &IMG_ADDR[OTA_IMGID_BOOT][OTA_INDEX_2], NULL);
	flash_get_layout_info(IMG_APP_OTA1, &IMG_ADDR[OTA_IMGID_APP][OTA_INDEX_1], NULL);
	flash_get_layout_info(IMG_APP_OTA2, &IMG_ADDR[OTA_IMGID_APP][OTA_INDEX_2], NULL);
	flash_get_layout_info(IMG_DSP, &IMG_ADDR[OTA_IMGID_DSP][OTA_INDEX_1], NULL);
	flash_get_layout_info(IMG_DSP, &IMG_ADDR[OTA_IMGID_DSP][OTA_INDEX_2], NULL);

	if (!ctx) {
		ota_printf(_OTA_ERR_, "%s, ctx can't be null", __func__);
		return -1;
	}

	ctx->type = type;
	if (ota_update_connection_params_init(ctx, host, port, resource) != 0) {
		goto exit;
	}

	otactrl = (update_ota_ctrl_info *)rtos_mem_malloc(sizeof(update_ota_ctrl_info));
	if (otactrl) {
		_memset(otactrl, 0, sizeof(update_ota_ctrl_info));
		ctx->otactrl = otactrl;
	} else {
		ota_printf(_OTA_ERR_, "%s, otactrl malloc failed", __func__);
		goto exit;
	}

	redirect = (update_redirect_conn *)rtos_mem_malloc(sizeof(update_redirect_conn));
	if (redirect) {
		_memset(redirect, 0, sizeof(update_redirect_conn));
		ctx->redirect = redirect;
	} else {
		ota_printf(_OTA_ERR_, "%s, redirect malloc failed", __func__);
		goto exit;
	}

	otaTargetHdr = (update_ota_target_hdr *)rtos_mem_malloc(sizeof(update_ota_target_hdr));
	if (otaTargetHdr) {
		_memset(otaTargetHdr, 0, sizeof(update_ota_target_hdr));
		ctx->otaTargetHdr = otaTargetHdr;
	} else {
		ota_printf(_OTA_ERR_, "%s, otaTargetHdr malloc failed", __func__);
		goto exit;
	}

	ota_printf(_OTA_INFO_, "host: %s(%d), resource: %s\n", ctx->host ? ctx->host : "null", ctx->port, resource ? ctx->resource : "null");
	return 0;
exit:
	ota_update_deinit(ctx);
	return -1;
}

void ota_update_deinit(ota_context *ctx)
{
	if (ctx == NULL) {
		return;
	}

	if (ctx->host) {
		rtos_mem_free(ctx->host);
	}

	if (ctx->resource) {
		rtos_mem_free(ctx->resource);
	}

	if (ctx->ca_cert) {
		rtos_mem_free(ctx->ca_cert);
	}

	if (ctx->client_cert) {
		rtos_mem_free(ctx->client_cert);
	}

	if (ctx->private_key) {
		rtos_mem_free(ctx->private_key);
	}

	if (ctx->otactrl) {
		rtos_mem_free(ctx->otactrl);
	}

	if (ctx->redirect) {
		rtos_mem_free(ctx->redirect);
	}

	if (ctx->otaTargetHdr) {
		rtos_mem_free(ctx->otaTargetHdr);
	}
}

