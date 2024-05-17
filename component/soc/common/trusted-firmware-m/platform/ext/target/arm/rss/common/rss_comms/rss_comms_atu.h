/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __RSS_COMMS_ATU_H__
#define __RSS_COMMS_ATU_H__

#include "tfm_plat_defs.h"
#include "rss_comms_atu_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t ref_counts[RSS_COMMS_ATU_REGION_AM];
} comms_atu_region_set_t;

/* Add an ATU region to a set of ATU regions */
enum tfm_plat_err_t comms_atu_add_region_to_set(comms_atu_region_set_t *set,
                                                uint8_t region);

/* Convert a host pointer into a pointer in the rss address space (inside the
 * given region), or return an error if that host pointer cannot be accessed in
 * the given region
 */
enum tfm_plat_err_t comms_atu_get_rss_ptr_from_host_addr(uint8_t region,
                                                         uint64_t host_addr,
                                                         void **rss_ptr);

/* Allocate an ATU region to contain the given host buffer, and return the index
 * of it. If there is already a region allocated that contains that host buffer,
 * increment the reference counter for it and return the index of that region.
 */
enum tfm_plat_err_t comms_atu_alloc_region(uint64_t host_addr, uint32_t size,
                                           uint8_t *region);

/* Decrease the regerence count to the particular region. If this is the last
 * reference to that region, uninitialise it and return it to the pool that can
 * be allocated.
 */
enum tfm_plat_err_t comms_atu_free_region(uint8_t region);

/* For each region in the set, decrease the reference count to the region by the
 * reference count in the set. If this causes any regions to have no references,
 * uninitialise the regions and return them to the pool that can be allocated.
 */
enum tfm_plat_err_t comms_atu_free_regions(comms_atu_region_set_t regions);

#ifdef __cplusplus
}
#endif

#endif /* __RSS_COMMS_ATU_H__ */
