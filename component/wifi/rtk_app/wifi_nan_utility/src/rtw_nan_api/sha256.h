#ifndef SHA256
#define SHA256

#include "rtw_nan_cmd_api.h"
#include <string.h>
#define SHA256_BLOCK_SIZE 64

#define LOAD32B(addr) \
		((uint32_t)((addr)[0] << 24) | ((addr)[1] << 16) | \
		((addr)[2] << 8) | (addr)[3])

#define STORE64B(addr, data) \
do { (addr)[0] = (uint8_t)((data) >> 56); (addr)[1] = (uint8_t)((data) >> 48);	\
	 (addr)[2] = (uint8_t)((data) >> 40); (addr)[3] = (uint8_t)((data) >> 32);	\
	 (addr)[4] = (uint8_t)((data) >> 24); (addr)[5] = (uint8_t)((data) >> 16);	\
	 (addr)[6] = (uint8_t)((data) >> 8);  (addr)[7] = (uint8_t)((data) & 0xff);	\
	} while (0)

#define STORE32B(addr, data) \
do { (addr)[0] = (uint8_t)(((data) >> 24) & 0xff); \
	 (addr)[1] = (uint8_t)(((data) >> 16) & 0xff); \
	 (addr)[2] = (uint8_t)(((data) >> 8) & 0xff); \
	 (addr)[3] = (uint8_t)((data) & 0xff); } while (0)

struct sha256_state {
	uint64_t length;
	uint32_t state[8], curlen;
	uint8_t buf[SHA256_BLOCK_SIZE];
};

/**
 * SHA256 Hashing
 * @addr: pointers to the data area
 * @len: Lengths of the data block
 * @res: Buffer for the digest
 * Returns: 0 on success, -1 of failure
 */
int sha256(const uint8_t *addr, const size_t len,
		   uint8_t *res);

/* Initialize the hash state */
void sha256_init(struct sha256_state *md);

/**
 * Process a block of memory though the hash
 * @param md     The hash state
 * @param in     The data to hash
 * @param inlen  The length of the data (octets)
 * @return CRYPT_OK if successful
*/
int sha256_process(struct sha256_state *md, const uint8_t *in,
				   uint64_t inlen);

/**
 * Terminate the hash to get the digest
 * @param md  The hash state
 * @param out [out] The destination of the hash (32 bytes)
 * @return CRYPT_OK if successful
*/
int sha256_done(struct sha256_state *md, uint8_t *out);

#endif
