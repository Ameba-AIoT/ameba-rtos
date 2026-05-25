/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef H_BOOTUTIL_MISC_
#define H_BOOTUTIL_MISC_

#include "fault_injection_hardening.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @page CFI_Design_Guide Control Flow Integrity (CFI) Design and Maintenance Guide
 *
 * ============================================================================
 * 1. Design Methodology
 * ============================================================================
 * This module utilizes the **X-Macro** technique to implement centralized CFI
 * management, aiming to reduce maintenance costs and minimize human errors
 * associated with manual modifications across multiple definitions.
 *
 * Core Design Principles:
 * - **Single Source of Truth**:
 *   All step definitions (Enum names, Magic Values, Tracking attributes) are
 *   defined solely in the `CFI_DEF_TABLE` macro. The Enum (`cfi_step_e`),
 *   counter array, Magic Value lookup table, and validation logic are all
 *   automatically generated from this table.
 *
 * - **Array-based Counters**:
 *   Replaced legacy complex bit-field unions with a simple `uint8_t counters[]`
 *   array. This optimizes `increase` operations to O(1) array indexing, removing
 *   switch-case logic and significantly improving code size and execution speed.
 *
 * - **Dual Verification Mechanism**:
 *   The CFI validation consists of two parts:
 *   1. **Accumulator Check**: Verifies if `current_cfi_cnt` equals the sum of
 *      Magic Values for all steps. This logic is auto-generated to ensure
 *      mathematical correctness.
 *   2. **Policy Check**: Verifies if the execution count of each step aligns
 *      with the hardware OTP/BootROM security configuration. This logic is
 *      business-specific and requires manual maintenance.
 *
 * ============================================================================
 * 2. Maintenance Guide
 * ============================================================================
 *
 * 2.1 Adding or Modifying Steps
 * ----------------------------------------------------------------------------
 * All modifications only require updating the `CFI_DEF_TABLE` macro.
 *
 * Format: `X(ENUM_SUFFIX, MAGIC_VALUE, IS_TRACKED)`
 *
 * - **CASE A: Adding a Fixed Step**
 *   - Definition: The step executes exactly once in the flow. It does not require
 *     individual execution count tracking (saves RAM); only the accumulator value matters.
 *   - Action: Add `X(NAME, 0x..., 0)`. Set the third parameter to `0`.
 *   - Example: `X(OTP_CHECK, 0x12345678, 0)`
 *
 * - **CASE B: Adding a Variable Step**
 *   - Definition: The step may execute 0 or multiple times (e.g., loops, branches).
 *     Precise execution count is required for policy checks.
 *   - Action: Add `X(NAME, 0x..., 1)`. Set the third parameter to `1`.
 *   - Example: `X(ECC_VERIFY, 0x87654321, 1)`
 *
 * - ** Key Principles**:
 *   1. **Magic Value Selection**: Must be a 32-bit random number. Ensure high
 *      Hamming Distance between values to prevent collision via bit-flip attacks.
 *   2. **Enum Order**: The order in the macro list determines the enum value.
 *      It is generally recommended to append new steps at the end to avoid
 *      shifting existing enum indices.
 *
 * 2.2 Modifying Policy Check Logic
 * ----------------------------------------------------------------------------
 * NOTE: ALL policy checks have been removed from `global_cfi_final_check`.
 *
 * The CFI validation now relies solely on the accumulator calculation mechanism
 * (PART 1 in `global_cfi_final_check`). This simplified approach:
 *
 * - Verifies that each security step was called the correct number of times
 * - Does not enforce whether steps should be called based on OTP/hardware configuration
 * - Reduces maintenance burden and coupling with hardware-specific policies
 *
 * The accumulator calculation automatically detects:
 * - Skipped steps (missing magic value contribution)
 * - Duplicate executions (magic value added multiple times)
 * - Bit-flip attacks (high Hamming distance between magic values)
 *
 * ============================================================================
 * 3. Verification Strategy
 * ============================================================================
 * To ensure the effectiveness of the CFI mechanism, include the following
 * scenarios in integration testing:
 *
 * 3.1 Positive Testing
 * - Verify that `global_cfi_final_check` returns `FIH_SUCCESS` during a standard
 *   Secure Boot flow (covering various Secure Boot ON/OFF combinations).
 *
 * 3.2 Negative Testing / Fault Injection
 * - **Skip Step Test**:
 *   Manually comment out a `global_cfi_increase` call.
 *   Expectation: Final Check fails (`FIH_FAILURE`) due to Accumulator Mismatch.
 *
 * - **Double Execution Test**:
 *   Call `increase` twice for a specific step.
 *   Expectation: Final Check fails due to Accumulator Mismatch AND/OR Policy
 *   Check failure (e.g., HUK count != 1).
 *
 * - **Policy Conflict Test**:
 *   Simulate hardware OTP as "Secure Boot ON", but skip the ECC step in code.
 *   Expectation: Policy Check (Part 1) detects `counters[ECC] == 0` -> Fail.
 *
 * - **Glitch Simulation Test**:
 *   Manually corrupt the value of `g_cfi.current_cfi_cnt` before the Final Check
 *   (simulating a voltage glitch causing a bit-flip).
 *   Expectation: Accumulator Check fails -> Fail.
 *
 * ============================================================================
 */

/* ========================================================================= */
/*                           CONFIGURATION                                   */
/* ========================================================================= */

/**
 * @name CFI Magic Values
 * @{
 */
#define SBOOT_GLOBAL_CFI_FLAG_Init  (0x89AB4CD2U)

/**
 * @brief Master X-Macro Table.
 * @details define the steps, magic values, and whether they are tracked counters.
 *
 * Format: X(ENUM_SUFFIX, MAGIC_VALUE, IS_TRACKED)
 * - ENUM_SUFFIX: Appended to CFI_STEP_
 * - MAGIC_VALUE: 32-bit random value
 * - IS_TRACKED : 1 = Variable step (count recorded), 0 = Fixed step (always assumed 1 in calculation)
 */
#define CFI_DEF_TABLE(X) \
    X(OTP,      0x41E39A67U, 0) /* Fixed */ \
    X(OTF,      0xB25C7F09U, 1) /* Variable */ \
    X(ECC,      0x6D8A134EU, 1) /* Variable */ \
    X(PQC,      0x90F2E8B5U, 1) /* Variable */ \
    X(IMG_HASH, 0x185EAC45U, 1) /* Variable */ \
    X(HUK,      0x37C5D18FU, 1) /* Variable */

/** @} */

/* ========================================================================= */
/*                       AUTO-GENERATED TYPES                                */
/* ========================================================================= */

/**
 * @brief CFI Step Enumeration.
 * @note  Auto-generated: CFI_STEP_OTP, CFI_STEP_OTF, etc.
 */
typedef enum {
#define GEN_ENUM(name, val, tracked) CFI_STEP_##name,
	CFI_DEF_TABLE(GEN_ENUM)
#undef GEN_ENUM
	CFI_STEP_MAX
} cfi_step_e;

/**
 * @brief Global CFI Context Structure.
 * @details Replaces the complex union with a simple array.
 */
typedef struct global_fih_cfi {
	volatile size_t current_cfi_cnt;      /**< Main accumulator */
	volatile uint8_t counters[CFI_STEP_MAX]; /**< Simple counter array */
} global_cfi_t;

fih_ret boot_fih_memequal(const void *s1, const void *s2, volatile size_t n, volatile size_t s1_size);
fih_ret global_cfi_init(volatile global_cfi_t *g_cfi, size_t init_val);
fih_ret global_cfi_increase(volatile global_cfi_t *g_cfi, cfi_step_e step);
fih_ret global_cfi_final_check(volatile global_cfi_t *g_cfi);

#ifdef __cplusplus
}
#endif

#endif /* H_BOOTUTIL_MISC_ */
