#include "vfs.h"
#include "example_vfs_encrypt.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "mbedtls/aes.h"

#define DUMP_BUFFER 0
#define AES_KEY_LEN 16
static const unsigned char aeskey[32] = {
	0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
	0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
	0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
	0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
} ;

static void dump_buf(char *info, uint8_t *buf, uint32_t len)
{
	printf("%s", info);
	for (int i = 0; i < len; i++) {
		printf("%s%02X%s", i % 16 == 0 ? "\n     " : " ",
			   buf[i], i == len - 1 ? "\n" : "");
	}
}

//output buffer should be 16 bytes aligned for aes cbc demand
int vfs_mbedtls_aes_cbc_encrypt(unsigned char *input, unsigned char *output, unsigned int len)
{
	unsigned char Iv[16] = {0};
	mbedtls_aes_context ctx;
	unsigned char *message;
	unsigned short msglen = (len / 16 + 1) * 16;

	message = rtos_mem_calloc(msglen, sizeof(unsigned char));
	memcpy(message, input, len);

#if DUMP_BUFFER
	dump_buf("write encrypt before", message, msglen);
#endif

	mbedtls_aes_init(&ctx);
	mbedtls_aes_setkey_enc(&ctx, aeskey, AES_KEY_LEN * 8);
	mbedtls_aes_crypt_cbc(&ctx, 1, msglen, Iv, message, output);
	mbedtls_aes_free(&ctx);

#if DUMP_BUFFER
	dump_buf("write encrypt after", output, msglen);
#endif

	rtos_mem_free(message);

	return 0;
}

//input buffer should be 16 bytes aligned for aes cbc demand
int vfs_mbedtls_aes_cbc_decrypt(unsigned char *input, unsigned char *output, unsigned int len)
{
	unsigned char Iv[16] = {0};
	mbedtls_aes_context ctx;
	unsigned char *aesdecsw;
	unsigned short msglen = (len / 16 + 1) * 16;

	aesdecsw = rtos_mem_calloc(msglen, sizeof(unsigned char));

#if DUMP_BUFFER
	dump_buf("read decrypt before ", input, msglen);
#endif

	mbedtls_aes_init(&ctx);
	mbedtls_aes_setkey_dec(&ctx, aeskey, AES_KEY_LEN * 8);
	mbedtls_aes_crypt_cbc(&ctx, 0, msglen, Iv, input, aesdecsw);
	mbedtls_aes_free(&ctx);

#if DUMP_BUFFER
	dump_buf("read decrypt after ", aesdecsw, msglen);
#endif

	memcpy(output, aesdecsw, len);
	rtos_mem_free(aesdecsw);

	return 0;
}

void example_vfs_encrypt_thread(void *param)
{
	rtos_time_delay_ms(3000);
	char key[] = "vfs_key";
	char val[] = "VFS module works normally with encrypt function !!!";
	char path[128] = {0};
	char buffer[64] = {0};
	char *prefix;
	vfs_file *finfo;
	int res = 0;

	prefix = find_vfs_tag(VFS_REGION_1);

	vfs_set_user_encrypt_callback(prefix, vfs_mbedtls_aes_cbc_encrypt, vfs_mbedtls_aes_cbc_decrypt, 16);

	snprintf(path, sizeof(path), "%s:%s", prefix, key);
	finfo = (vfs_file *)fopen(path, "w");
	if (finfo == NULL) {
		printf("[%s] fopen failed \r\n", __FUNCTION__);
	}

	res = fwrite(val, strlen(val), 1, (FILE *)finfo);
	if (res != (strlen(val) / 16 + 1) * 16) {
		printf("[%s] fwrite failed,err is %d!!\r\n", __FUNCTION__, res);
	} else {
		printf("[%s] fwrite succeeded !!!\r\n", __FUNCTION__);
	}

	fclose((FILE *)finfo);

	finfo = (vfs_file *)fopen(path, "r");
	res = fread(buffer, strlen(val), 1, (FILE *)finfo);
	if (res < 0) {
		printf("[%s] fread failed,err is %d!!!\r\n", __FUNCTION__, res);
	} else {
		if (memcmp(buffer, val, strlen(val)) != 0) {
			printf("read buffer is %s \r\n", buffer);
			printf("[%s] fread content error !!!\r\n", __FUNCTION__);
		} else {
			printf("[%s] fread succeeded !!!\r\n", __FUNCTION__);
		}
	}

	fclose((FILE *)finfo);

	res = remove(path);
	if (res < 0) {
		printf("[%s] remove file failed,err is %d!!!\r\n", __FUNCTION__, res);
	} else {
		printf("[%s] remove file succeeded !!!\r\n", __FUNCTION__);
	}

	rtos_task_delete(NULL);
}

void example_vfs_encrypt(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_vfs_encrypt_thread"), example_vfs_encrypt_thread, NULL, 2048 * 4, 1) != SUCCESS) {
		printf("\n\r%s rtos_task_create(example_kv_thread) failed", __FUNCTION__);
	}
}
