/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_CRASHDUMP_ENV_H_
#define _AMEBA_CRASHDUMP_ENV_H_

/* Build-time paths used to print the addr2line hint of a crashdump.
 * Defined in ameba_crashdump_env.c, which is always built from source so that
 * the paths follow the current build instead of the one that released the lib.
 * Weak: images that do not build that file(e.g. flashloader) link with the
 * symbols unresolved, and the hint is skipped instead of printing a wrong path.
 */

extern const char crashdump_toolchain_dir[] __attribute__((weak));
extern const char crashdump_image_dir[] __attribute__((weak));

#endif
