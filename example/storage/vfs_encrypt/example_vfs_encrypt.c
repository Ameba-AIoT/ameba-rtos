#include "ameba_soc.h"
#include "os_wrapper.h"
#include "mbedtls/aes.h"
#include "ctr.h"
#include "vfs.h"
#include "example_vfs_encrypt.h"

extern void vfs_set_user_encrypt_callback(char *prefix, vfs_enc_callback_t encrypt_func, vfs_dec_callback_t decrypt_func, unsigned char iv_len);

#define DUMP_BUFFER 0
#define AES_KEY_LEN 16
static const unsigned char aeskey[32] = {
	0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
	0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
	0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
	0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
} ;

static unsigned char iv[16] ALIGNMTO(CACHE_LINE_SIZE);
static unsigned char stream_block[16] ALIGNMTO(CACHE_LINE_SIZE);

#if DUMP_BUFFER
static void dump_buf(char *info, uint8_t *buf, uint32_t len)
{
	printf("%s(len=%lu):", info, len);
	for (uint32_t i = 0; i < len; i++) {
		printf("%s%02X%s", i % 16 == 0 ? "\n     " : " ",
			   buf[i], i == len - 1 ? "\n" : "");
	}
}
#endif


// AES-CTR encryption - supports arbitrary length and random access
// Uses file offset to calculate correct counter for random access support
int vfs_mbedtls_aes_ctr_encrypt(unsigned char *input, unsigned char *output, unsigned int len, unsigned long offset)
{
	mbedtls_aes_context ctx;
	unsigned char *input_align;
	unsigned char *output_align;
	int msg_len = len < CACHE_LINE_SIZE ? CACHE_LINE_SIZE : (len / CACHE_LINE_SIZE + 1) * CACHE_LINE_SIZE;
	unsigned long block_index;
	size_t nc_off;
	int ret;

	input_align = rtos_mem_calloc(msg_len, sizeof(unsigned char));
	output_align = rtos_mem_calloc(msg_len, sizeof(unsigned char));
	memcpy(input_align, input, len);

#if DUMP_BUFFER
	dump_buf("write encrypt before", input, len);
#endif

	// Calculate block index and offset within block
	block_index = offset / 16;
	nc_off = offset % 16;

	// Set nonce_counter: high 12 bytes are nonce (0), low 4 bytes are counter
	// Counter is stored in big-endian: lowest byte at iv[15], highest at iv[12]
	iv[15] = (block_index >> 0) & 0xFF;
	iv[14] = (block_index >> 8) & 0xFF;
	iv[13] = (block_index >> 16) & 0xFF;
	iv[12] = (block_index >> 24) & 0xFF;

	mbedtls_aes_init(&ctx);
	mbedtls_aes_setkey_enc(&ctx, aeskey, AES_KEY_LEN * 8);

	// If nc_off > 0, pre-generate stream_block using current iv's counter
	// because mbedtls_aes_crypt_ctr uses stream_block[offset:] directly when offset > 0
	if (nc_off > 0) {
		mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_ENCRYPT, iv, stream_block);
		mbedtls_ctr_increment_counter(iv);
	}

	ret = mbedtls_aes_crypt_ctr(&ctx, msg_len, &nc_off, iv, stream_block, input_align, output_align);

	memcpy(output, output_align, len);

#if DUMP_BUFFER
	dump_buf("write encrypt after", output, len);
#endif

	mbedtls_aes_free(&ctx);
	memset(iv, 0, 16);
	memset(stream_block, 0, 16);
	rtos_mem_free(input_align);
	rtos_mem_free(output_align);
	return ret;
}

// AES-CTR decryption - supports arbitrary length and random access
int vfs_mbedtls_aes_ctr_decrypt(unsigned char *input, unsigned char *output, unsigned int len, unsigned long offset)
{
	mbedtls_aes_context ctx;
	unsigned char *input_align;
	unsigned char *output_align;
	int msg_len = len < CACHE_LINE_SIZE ? CACHE_LINE_SIZE : (len / CACHE_LINE_SIZE + 1) * CACHE_LINE_SIZE;
	unsigned long block_index;
	size_t nc_off;
	int ret;

	input_align = rtos_mem_calloc(msg_len, sizeof(unsigned char));
	output_align = rtos_mem_calloc(msg_len, sizeof(unsigned char));
	memcpy(input_align, input, len);

#if DUMP_BUFFER
	dump_buf("read decrypt before", input, len);
#endif

	// Calculate block index and offset within block
	block_index = offset / 16;
	nc_off = offset % 16;

	// Set nonce_counter: high 12 bytes are nonce (0), low 4 bytes are counter
	// Counter is stored in big-endian: lowest byte at iv[15], highest at iv[12]
	iv[15] = (block_index >> 0) & 0xFF;
	iv[14] = (block_index >> 8) & 0xFF;
	iv[13] = (block_index >> 16) & 0xFF;
	iv[12] = (block_index >> 24) & 0xFF;

	mbedtls_aes_init(&ctx);
	mbedtls_aes_setkey_enc(&ctx, aeskey, AES_KEY_LEN * 8);

	// If nc_off > 0, pre-generate stream_block using current iv's counter
	if (nc_off > 0) {
		mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_ENCRYPT, iv, stream_block);
		mbedtls_ctr_increment_counter(iv);
	}

	ret = mbedtls_aes_crypt_ctr(&ctx, msg_len, &nc_off, iv, stream_block, input_align, output_align);

	memcpy(output, output_align, len);

#if DUMP_BUFFER
	dump_buf("read decrypt after", output, len);
#endif

	mbedtls_aes_free(&ctx);
	memset(iv, 0, 16);
	memset(stream_block, 0, 16);
	rtos_mem_free(input_align);
	rtos_mem_free(output_align);
	return ret;
}

void example_vfs_encrypt_thread(void *param)
{
	(void)param;
	rtos_time_delay_ms(3000);
	char filename[] = "vfs_file_1";
	char content[] = "VFS module works normally with encrypt function !!!";
	char path[128] = {0};
	char buffer[64] = {0};
	char *prefix;
	FILE *finfo;
	int res = 0;

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n====================Example: VFS_ENCRYPT====================\r\n");

	prefix = find_vfs_tag(VFS_REGION_1);

	vfs_set_user_encrypt_callback(prefix, vfs_mbedtls_aes_ctr_encrypt, vfs_mbedtls_aes_ctr_decrypt, 16);

	DiagSnPrintf(path, sizeof(path), "%s:%s", prefix, filename);

	finfo = fopen(path, "w");
	if (finfo == NULL) {
		printf("[%s] fopen() failed\r\n", __FUNCTION__);
		goto exit;
	}

	res = fwrite(content, strlen(content), 1, finfo);
	if (res != (int)strlen(content)) {
		printf("[%s] fwrite() failed, err is %d\r\n", __FUNCTION__, res);
	} else {
		printf("[%s] fwrite() succeeded, write %d characters:%s\r\n", __FUNCTION__, res, content);
	}

	fclose(finfo);

	finfo = fopen(path, "r");
	res = fread(buffer, strlen(content), 1, finfo);
	if (res < 0) {
		printf("[%s] fread() failed, err is %d\r\n", __FUNCTION__, res);
	} else {
		if (memcmp(buffer, content, strlen(content)) != 0) {
			printf("[%s] fread() content error, read buffer is %s\r\n", __FUNCTION__, buffer);
		} else {
			printf("[%s] fread() succeeded, read %d characters:%s\r\n", __FUNCTION__, strlen(buffer), buffer);
		}
	}

	fclose(finfo);

	res = remove(path);
	if (res < 0) {
		printf("[%s] remove() file failed, err is %d\r\n", __FUNCTION__, res);
	} else {
		printf("[%s] remove() file succeeded\r\n", __FUNCTION__);
	}

exit:
	rtos_task_delete(NULL);
}

void example_vfs_encrypt(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_vfs_encrypt_thread"), example_vfs_encrypt_thread, NULL, 2048 * 4, 1) != RTK_SUCCESS) {
		printf("\n\r%s rtos_task_create(example_vfs_encrypt_thread) failed", __FUNCTION__);
	}
}
