/** @file
 * Copyright (c) 2019-2023, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

#ifndef _PAL_CONFIG_H_
#define _PAL_CONFIG_H_

#include "pal_crypto_config.h"
#include "pal_attestation_config.h"
#include "pal_storage_config.h"

/* Define PSA test suite dependent macros for non-cmake build */
#if !defined(PSA_CMAKE_BUILD)

/* Print verbosity = TEST */
#define VERBOSE 3

/* NSPE or SPE VAL build? */
#define VAL_NSPE_BUILD

/* NSPE or SPE TEST build? */
#define NONSECURE_TEST_BUILD

/* If not defined, skip watchdog programming */
#define WATCHDOG_AVAILABLE

/* Are Dynamic memory APIs available to secure partition? */
#define SP_HEAP_MEM_SUPP

/* PSA Isolation level supported by platform */
#define PLATFORM_PSA_ISOLATION_LEVEL 3
#endif /* PSA_CMAKE_BUILD */

/* Version of crypto spec used in attestation */
#define CRYPTO_VERSION_BETA3

/* Use hardcoded public key */
#define PLATFORM_OVERRIDE_ATTEST_PK

/*
 * Include of PSA defined Header files
 */
#ifdef IPC
/* psa/client.h: Contains the PSA Client API elements */
#include "psa/client.h"

/*
 * psa_manifest/sid.h:  Macro definitions derived from manifest files that map from RoT Service
 * names to Service IDs (SIDs). Partition manifest parse build tool must provide the implementation
 * of this file.
*/
#include "psa_manifest/sid.h"

/*
 * psa_manifest/pid.h: Secure Partition IDs
 * Macro definitions that map from Secure Partition names to Secure Partition IDs.
 * Partition manifest parse build tool must provide the implementation of this file.
*/
#include "psa_manifest/pid.h"
#endif

#ifdef CRYPTO
/* psa/crypto.h: Contains the PSA Crypto API elements */
#include "psa/crypto.h"
#endif

#if defined(INTERNAL_TRUSTED_STORAGE) || defined(STORAGE)
/* psa/internal_trusted_storage.h: Contains the PSA ITS API elements */
#include "psa/internal_trusted_storage.h"
#endif

#if defined(PROTECTED_STORAGE) || defined(STORAGE)
/* psa/protected_storage.h: Contains the PSA PS API elements */
#include "psa/protected_storage.h"
#endif

#ifdef INITIAL_ATTESTATION
/* psa/initial_attestation.h: Contains the PSA Initial Attestation API elements */
#include "psa/initial_attestation.h"
#endif

#endif /* _PAL_CONFIG_H_ */
