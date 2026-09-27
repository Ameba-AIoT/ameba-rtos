/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file    ameba_solocfg.c
 * @brief   SOLO dual-core isolation - user configuration.
 *
 * This is the ONLY file a customer needs to touch to tune the SOLO isolation
 * applied at boot (see ameba_solo_cfg.h). It describes two things:
 *
 *   - solo_km4tz_periph[] : peripherals reserved for the secure KM4TZ core.
 *     End the list with SOLO_PERIPH_MAX. Anything not listed is shared.
 *   - solo_km4ns_flash/sram/psram[] : the memory regions (address ranges) the
 *     non-secure KM4NS core is allowed to access. Each table has exactly
 *     SOLO_REGION_NUM (4) entries; set entries you do not use to
 *     {0xFFFFFFFF, 0xFFFFFFFF}. Everything not listed stays private to KM4TZ.
 *     KM4NS runs from its flash image1 (boot stub) and image2 (app), and keeps
 *     its data in its own SRAM/PSRAM image2 segment - expose exactly those.
 *
 * The linker symbols below track the current image layout automatically; edit
 * the addresses freely if a custom layout is used.
 */

#include "ameba_soc.h"
#include "ameba_solo_cfg.h"

/* Peripherals reserved for the secure KM4TZ core only. */
const SOLO_PERIPH_TypeDef solo_km4tz_periph[] = {
	/* <-- add peripheral which IOT core is not allowed to access here */
	SOLO_PERIPH_MAX,	/* list terminator - keep last */
};

/* Flash regions KM4NS may access (see the note inside for why it is fully open). */
const SOLO_REGION_TypeDef solo_km4ns_flash[SOLO_REGION_NUM] = {
	{0x00000000, 0x0FFFFFFF},
	{0xFFFFFFFF, 0xFFFFFFFF},
	{0xFFFFFFFF, 0xFFFFFFFF},
	{0xFFFFFFFF, 0xFFFFFFFF},
};

/* SRAM regions KM4NS may access: the shared fixed area at the base + the KM4NS
 * image2/boot span (see the note inside); KM4TZ's private SRAM in between is
 * kept isolated. */
const SOLO_REGION_TypeDef solo_km4ns_sram[SOLO_REGION_NUM] = {
	{0x20000000, 0x20001FFF},
	{(u32)__km4ns_sram_start__, (u32)__km4ns_sram_end__ - 1},
	{0xFFFFFFFF, 0xFFFFFFFF},
	{0xFFFFFFFF, 0xFFFFFFFF},
};

/* PSRAM regions KM4NS may access: its image2 BD PSRAM (if NP uses PSRAM). */
const SOLO_REGION_TypeDef solo_km4ns_psram[SOLO_REGION_NUM] = {
	{(u32)__km4ns_bd_psram_start__, (u32)__km4ns_bd_psram_end__ - 1},
	{0xFFFFFFFF, 0xFFFFFFFF},
	{0xFFFFFFFF, 0xFFFFFFFF},
	{0xFFFFFFFF, 0xFFFFFFFF},
};

/* SRAM regions the KM4TZ app must NOT write. Each entry is {start, end} inclusive;
 * app_start passes each populated entry to the MPU as a read-only region for KM4TZ.
 * Set unused entries to {0xFFFFFFFF, 0xFFFFFFFF}. Addresses must be 32-byte aligned.
 *
 * Entry 0: KM4NS private SRAM (everything above the KM4TZ<->KM4NS shared block).
 *          The 2.5K shared block at the KM4NS SRAM base (IPC mailbox + .solo_share
 *          flash-sync flag) is intentionally NOT protected, so KM4TZ can still
 *          drive IPC and the flash write-lock handshake. */
const SOLO_REGION_TypeDef solo_km4tz_ro_sram[SOLO_MPU_ENTRY_NUM] = {
	{(u32)__solo_share_ram_end__, (u32)__km4ns_sram_end__ - 1},
	{0xFFFFFFFF, 0xFFFFFFFF},
};
