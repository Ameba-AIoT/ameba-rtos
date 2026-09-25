/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* SDK_TOOLCHAIN and IMAGE_DIR are paths of the machine doing the build, and
 * IMAGE_DIR carries the selected part number (e.g. build_RTL8726E/...). They
 * must not be baked into lib_crashdump.a: an IC with several part numbers is
 * released with the lib built for one of them, so any other part number would
 * print the released one. Keeping them in this file, which is open source and
 * rebuilt together with the application, makes the printed path always match
 * the image that crashed.
 */

#include "ameba_crashdump_env.h"

const char crashdump_toolchain_dir[] = SDK_TOOLCHAIN;
const char crashdump_image_dir[] = IMAGE_DIR;
