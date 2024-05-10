#-------------------------------------------------------------------------------
# Copyright (c) 2020-2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0076 NEW)
set(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})

#========================= Platform region defs ===============================#

target_include_directories(platform_region_defs
    INTERFACE
        partition
)

#========================= Platform common defs ===============================#

# Specify the location of platform specific build dependencies.
target_sources(tfm_s
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/startup_musca.c
)
target_add_scatter_file(tfm_s
    $<$<C_COMPILER_ID:ARMClang>:${PLATFORM_DIR}/ext/common/armclang/tfm_common_s.sct>
    $<$<C_COMPILER_ID:GNU>:${PLATFORM_DIR}/ext/common/gcc/tfm_common_s.ld>
    $<$<C_COMPILER_ID:IAR>:${PLATFORM_DIR}/ext/common/iar/tfm_common_s.icf>
)

if(NS)
    target_sources(tfm_ns
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/startup_musca.c
    )
    target_add_scatter_file(tfm_ns
        $<$<C_COMPILER_ID:ARMClang>:${PLATFORM_DIR}/ext/common/armclang/tfm_common_ns.sct>
        $<$<C_COMPILER_ID:GNU>:${PLATFORM_DIR}/ext/common/gcc/tfm_common_ns.ld>
        $<$<C_COMPILER_ID:IAR>:${PLATFORM_DIR}/ext/common/iar/tfm_common_ns.icf>
    )
    target_link_libraries(CMSIS_5_tfm_ns
        INTERFACE
            $<$<OR:$<BOOL:${CONFIG_TFM_ENABLE_FP}>,$<BOOL:${CONFIG_TFM_ENABLE_MVE_FP}>>:CMSIS_5_RTX_V8MMFN>
            $<$<NOT:$<OR:$<BOOL:${CONFIG_TFM_ENABLE_FP}>,$<BOOL:${CONFIG_TFM_ENABLE_MVE_FP}>>>:CMSIS_5_RTX_V8MMN>
    )
    target_compile_options(tfm_ns
        PUBLIC
            ${COMPILER_CP_FLAG}
    )
    target_link_options(tfm_ns
        PUBLIC
            ${LINKER_CP_OPTION}
    )
endif()

if(BL2)
    target_sources(bl2
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/startup_musca.c
    )
    target_add_scatter_file(bl2
            $<$<C_COMPILER_ID:ARMClang>:${PLATFORM_DIR}/ext/common/armclang/tfm_common_bl2.sct>
            $<$<C_COMPILER_ID:GNU>:${PLATFORM_DIR}/ext/common/gcc/tfm_common_bl2.ld>
            $<$<C_COMPILER_ID:IAR>:${PLATFORM_DIR}/ext/common/iar/tfm_common_bl2.icf>
    )
endif()

#========================= Platform Secure ====================================#

target_include_directories(platform_s
    PUBLIC
        .
        CMSIS_Driver
        CMSIS_Driver/Config
        Device/Config
        Device/Include
        Native_Driver
        partition
        services/include
        Libraries
    INTERFACE
        cc312
)

target_sources(platform_s
    PRIVATE
        CMSIS_Driver/Driver_QSPI_Flash.c
        CMSIS_Driver/Driver_Flash_MRAM.c
        CMSIS_Driver/Driver_MPC.c
        CMSIS_Driver/Driver_PPC.c
        CMSIS_Driver/Driver_USART.c
        Device/Source/device_definition.c
        Device/Source/system_core_init.c
        Native_Driver/mpc_sie200_drv.c
        Native_Driver/mpu_armv8m_drv.c
        Native_Driver/ppc_sse200_drv.c
        Native_Driver/gpio_cmsdk_drv.c
        Native_Driver/uart_pl011_drv.c
        Native_Driver/musca_s1_scc_drv.c
        Native_Driver/qspi_ip6514e_drv.c
        Native_Driver/cache_drv.c
        ${CMAKE_CURRENT_SOURCE_DIR}/Native_Driver/timer_cmsdk_drv.c
        Libraries/mt25ql_flash_lib.c
        ${CMAKE_CURRENT_SOURCE_DIR}/plat_test.c
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/services/src/tfm_platform_system.c>
)

target_compile_options(platform_s
    PUBLIC
        ${COMPILER_CMSE_FLAG}
)

# To configure S and NS timer in S side for FP interrupt test
target_compile_definitions(platform_s
    PUBLIC
        $<$<BOOL:${TEST_NS_FPU}>:TEST_NS_FPU>
        $<$<BOOL:${TEST_S_FPU}>:TEST_S_FPU>
)

#========================= Platform Non-Secure ================================#

target_sources(platform_ns
    PRIVATE
        Native_Driver/uart_pl011_drv.c
        Native_Driver/timer_cmsdk_drv.c
        CMSIS_Driver/Driver_USART.c
        Device/Source/system_core_init.c
        Device/Source/device_definition.c
        $<$<BOOL:${PLATFORM_GPLED_ENABLED}>:${CMAKE_CURRENT_SOURCE_DIR}/services/src/tfm_ioctl_ns_api.c>
        $<$<BOOL:${PLATFORM_GPLED_ENABLED}>:${CMAKE_CURRENT_SOURCE_DIR}/services/src/tfm_gpled_api.c>
)

target_include_directories(platform_ns
    PUBLIC
        .
        CMSIS_Driver/Config
        Device/Config
        Device/Include
        Native_Driver
        services/include
        Libraries
        ${PLATFORM_DIR}/ext/cmsis
)

#========================= Platform BL2 =======================================#

if(BL2)
    target_sources(platform_bl2
        PRIVATE
            Device/Source/system_core_init.c
            Device/Source/device_definition.c
            CMSIS_Driver/Driver_Flash_MRAM.c
            Native_Driver/uart_pl011_drv.c
            Native_Driver/cache_drv.c
            Native_Driver/musca_s1_scc_drv.c
            CMSIS_Driver/Driver_USART.c
    )

    target_include_directories(platform_bl2
        PUBLIC
            partition
            Device/Include
            Libraries
        INTERFACE
            cc312
        PRIVATE
            .
            CMSIS_Driver/Config
            Device/Config
            Native_Driver
            ${CMAKE_SOURCE_DIR}/bl2/ext/mcuboot/include # for fih.h interop only
            ${MCUBOOT_PATH}/boot/bootutil/include # for fault_injection_hardening.h only
            ${CMAKE_BINARY_DIR}/bl2/ext/mcuboot # for mcuboot_config.h only
    )

    target_compile_options(platform_bl2
        PRIVATE
            ${BL2_COMPILER_CP_FLAG}
    )
endif()

#========================= tfm_spm ============================================#

target_sources(tfm_spm
    PRIVATE
        target_cfg.c
        tfm_hal_isolation.c
        tfm_hal_platform.c
        faults.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tfm_interrupts.c
)
