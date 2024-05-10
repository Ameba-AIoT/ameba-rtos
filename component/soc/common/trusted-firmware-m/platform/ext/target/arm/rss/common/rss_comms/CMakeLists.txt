#-------------------------------------------------------------------------------
# Copyright (c) 2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

target_include_directories(platform_s
    PUBLIC
        .
)

target_sources(platform_s
    PRIVATE
        rss_comms.c
        rss_comms_hal.c
        rss_comms_queue.c
        rss_comms_protocol.c
        rss_comms_protocol_embed.c
        rss_comms_protocol_pointer_access.c
        rss_comms_atu.c
)

target_compile_definitions(platform_s
    PRIVATE
        RSS_COMMS_MAX_CONCURRENT_REQ=2
        RSS_COMMS_PROTOCOL_EMBED_ENABLED
        RSS_COMMS_PROTOCOL_POINTER_ACCESS_ENABLED
        $<$<BOOL:${CONFIG_TFM_HALT_ON_CORE_PANIC}>:CONFIG_TFM_HALT_ON_CORE_PANIC>
)

# For spm_log_msgval
target_link_libraries(platform_s
    PRIVATE
        tfm_spm
        tfm_sprt
)

