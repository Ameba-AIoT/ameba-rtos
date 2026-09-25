/**
 * *****************************************************************************
 * @file    crypto_driver_config_rtk.h
 * @brief   Per-SoC RTK hardware driver feature flags for TF-PSA-Crypto 1.x.
 *
 * This file declares which PSA transparent driver entry points are backed by
 * RTK hardware on the current SoC.  It is kept separate from
 * crypto_config_rtk.h (which declares PSA_WANT_* application requirements)
 * because the two sets of macros vary along different axes:
 *
 *   PSA_WANT_*            — application requirement, same across SoCs
 *   RTK_PSA_*_DRIVER      — hardware capability, SoC-specific
 *
 * When a driver macro is NOT defined for a given SoC, PSA falls back to the
 * software builtin implementation; the algorithm remains functional.
 *
 * Usage convention:
 *   - Add a CONFIG_<SOC> guard when porting a new SoC.
 *   - PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT must be defined whenever at least
 *     one RTK_PSA_*_DRIVER or RTK_PKE_RSA_EXP_MOD is defined; the PSA wrapper
 *     compile-time branches depend on it.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#ifndef PSA_CRYPTO_DRIVER_CONFIG_RTK_H
#define PSA_CRYPTO_DRIVER_CONFIG_RTK_H

/*
 * RLE1509 and RTL8720F share the same LALU crypto engine and PKE engine,
 * so all driver paths are available on both.  Add separate guards for
 * SoCs with different hardware capabilities when porting.
 */
#if defined(CONFIG_RLE1509) || defined(CONFIG_RTL8720F)

/* Hash: hardware SHA-224/256/384/512 via crypto_sha2_* ROM API. */
#define RTK_PSA_HASH_DRIVER

/* Symmetric cipher: AES ECB/CBC single-shot and multipart;
 * CTR single-shot (multipart falls to software — stream cipher). */
#define RTK_PSA_CIPHER_DRIVER

/* AEAD: full multipart AES-GCM and AES-CCM. */
#define RTK_PSA_AEAD_DRIVER

/* Asymmetric sign/verify: randomized ECDSA over SECP/Brainpool curves. */
#define RTK_PSA_ECDSA_DRIVER

/* Key agreement: ECDH over SECP/Brainpool curves. */
#define RTK_PSA_ECDH_DRIVER

/* HMAC: single-shot mac_compute/verify (16/24/32-byte keys only). */
#define RTK_PSA_MAC_DRIVER

/* RSA modular exponentiation via pke_rsa_exp_mod (bignum-layer hook). */
#define RTK_PKE_RSA_EXP_MOD

#endif /* CONFIG_RLE1509 || CONFIG_RTL8720F */

/*
 * Gates the transparent-accelerator branches of the PSA cipher/AEAD wrappers.
 * Must be defined whenever at least one RTK_PSA_*_DRIVER is defined.
 */
#if defined(RTK_PSA_HASH_DRIVER) || defined(RTK_PSA_CIPHER_DRIVER)  || \
    defined(RTK_PSA_AEAD_DRIVER) || defined(RTK_PSA_ECDSA_DRIVER)   || \
    defined(RTK_PSA_ECDH_DRIVER) || defined(RTK_PSA_MAC_DRIVER)     || \
    defined(RTK_PKE_RSA_EXP_MOD)
#define PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#endif

#endif /* PSA_CRYPTO_DRIVER_CONFIG_RTK_H */
