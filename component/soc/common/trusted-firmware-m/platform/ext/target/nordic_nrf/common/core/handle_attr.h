/*
 * Copyright (c) 2021-2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef __HANDLE_ATTR_H__
#define __HANDLE_ATTR_H__

/* Boundary handle binding macros. */
#define HANDLE_ATTR_PRIV_POS            1U
#define HANDLE_ATTR_PRIV_MASK           (0x1UL << HANDLE_ATTR_PRIV_POS)
#define HANDLE_ATTR_NS_POS              0U
#define HANDLE_ATTR_NS_MASK             (0x1UL << HANDLE_ATTR_NS_POS)

#endif /* __HANDLE_ATTR_H__ */
