/**
 * @file  psa_selftest.h
 * @brief Common declarations for mbedTLS 4.1.1 PSA driver selftest.
 *
 * Each module exports one function: selftest_xxx().
 * Returns 0 on all-pass, number-of-failures otherwise.
 */

#ifndef PSA_SELFTEST_H
#define PSA_SELFTEST_H

/* Run all subtests; print per-case result via RTK_LOGS. */
int selftest_hash(void);
int selftest_cipher(void);
int selftest_aead(void);
int selftest_mac(void);
int selftest_ecdsa(void);
int selftest_ecdh(void);

#endif /* PSA_SELFTEST_H */
