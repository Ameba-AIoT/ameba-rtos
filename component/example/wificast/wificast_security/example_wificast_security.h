#ifndef _EXAMPLE_WIFICAST_SECURITY_H_
#define _EXAMPLE_WIFICAST_SECURITY_H_

#include "../wificast_defs.h"
#include "os_wrapper.h"
#include "PinNames.h"
#include "serial_api.h"
#include "serial_ex_api.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ecdh.h"

#define UART_IDX  0
#define UART_BAUD_RATE 1500000
#define UART_TX_PIN    PA_28
#define UART_RX_PIN    PA_29

#define UART_RX_BUF_SZ	200

/**
 * @brief Public key exchange info
 */
struct pubkey_exchange_info {
	u16 type;                          /* application message type */
	u8 mac[6];                         /* device mac address */
	u8 pubkey[MBEDTLS_ECP_MAX_PT_LEN]; /* ecdh public key */
	int pubkey_len;                    /* ecdh public key length */
} __attribute__((packed));

/**
 * @brief Keypair info
 */
struct example_keypair {
	u8 pubkey[MBEDTLS_ECP_MAX_PT_LEN];    /* ecdh public key */
	int pubkey_len;                       /* ecdh public key length */
	u8 privkey[MBEDTLS_ECP_MAX_BYTES];    /* ecdh private key */
	int privkey_len;                      /* ecdh private key length */
	u8 shared_key[MBEDTLS_ECP_MAX_PT_LEN];/* ecdh shared key */
	int shared_key_len;                   /* ecdh shared key length */
};

#endif /* _EXAMPLE_WIFICAST_SECURITY_H_ */