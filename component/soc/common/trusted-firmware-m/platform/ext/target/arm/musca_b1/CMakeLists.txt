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

target_compile_definitions(platform_region_defs
    INTERFACE
        $<$<BOOL:${PLATFORM_PSA_ADAC_SECURE_DEBUG}>:PLATFORM_PSA_ADAC_SECURE_DEBUG>
)

#========================= Platform common defs ===============================#

# Specify the location of platform specific build dependencies.
target_sources(tfm_s
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/startup_musca.c
)

# Use a common scatter file for Isolation L1 and L2, a dedicated one for Isolation L3
# IAR is not supported for L3
target_add_scatter_file(tfm_s
    $<$<AND:$<VERSION_LESS:${TFM_ISOLATION_LEVEL},3>,$<C_COMPILER_ID:ARMClang>>:${PLATFORM_DIR}/ext/common/armclang/tfm_common_s.sct>
    $<$<AND:$<VERSION_LESS:${TFM_ISOLATION_LEVEL},3>,$<C_COMPILER_ID:GNU>>:${PLATFORM_DIR}/ext/common/gcc/tfm_common_s.ld>
    $<$<AND:$<VERSION_LESS:${TFM_ISOLATION_LEVEL},3>,$<C_COMPILER_ID:IAR>>:${PLATFORM_DIR}/ext/common/iar/tfm_common_s.icf>
    $<$<AND:$<VERSION_EQUAL:${TFM_ISOLATION_LEVEL},3>,$<C_COMPILER_ID:ARMClang>>:${CMAKE_BINARY_DIR}/generated/platform/ext/common/armclang/tfm_isolation_l3.sct>
    $<$<AND:$<VERSION_EQUAL:${TFM_ISOLATION_LEVEL},3>,$<C_COMPILER_ID:GNU>>:${CMAKE_BINARY_DIR}/generated/platform/ext/common/gcc/tfm_isolation_l3.ld>
    $<$<AND:$<VERSION_EQUAL:${TFM_ISOLATION_LEVEL},3>,$<C_COMPILER_ID:IAR>>:${CMAKE_BINARY_DIR}/generated/platform/ext/common/iar/tfm_isolation_l3.icf>
)

if(NS)
    target_sources(tfm_ns
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/startup_musca.c
    )
    target_add_scatter_file(tfm_ns
        $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/armclang/musca_ns.sct>
        $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/gcc/musca_ns.ld>
        $<$<C_COMPILER_ID:IAR>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/iar/musca_ns.icf>
    )
    target_link_libraries(CMSIS_5_tfm_ns
        INTERFACE
            CMSIS_5_RTX_V8MMN
    )
endif()

if(BL2)
    target_sources(bl2
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/startup_musca.c
    )
    target_add_scatter_file(bl2
            $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/armclang/musca_bl2.sct>
            $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/gcc/musca_bl2.ld>
            $<$<C_COMPILER_ID:IAR>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/iar/musca_bl2.icf>
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
        CMSIS_Driver/Driver_MPC.c
        CMSIS_Driver/Driver_PPC.c
        CMSIS_Driver/Driver_USART.c
        Device/Source/device_definition.c
        Device/Source/system_core_init.c
        Native_Driver/mpc_sie200_drv.c
        Native_Driver/mpu_armv8m_drv.c
        Native_Driver/gpio_cmsdk_drv.c
        Libraries/mt25ql_flash_lib.c
        Native_Driver/qspi_ip6514e_drv.c
        Native_Driver/musca_b1_scc_drv.c
        Native_Driver/ppc_sse200_drv.c
        Native_Driver/uart_pl011_drv.c
        $<$<NOT:$<BOOL:${TEST_NS_SLIH_IRQ}>>:${CMAKE_CURRENT_SOURCE_DIR}/Native_Driver/timer_cmsdk_drv.c>
        $<$<OR:$<BOOL:${TFM_S_REG_TEST}>,$<BOOL:${TFM_NS_REG_TEST}>>:${CMAKE_CURRENT_SOURCE_DIR}/plat_test.c>
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/services/src/tfm_platform_system.c>
)

target_compile_options(platform_s
    PUBLIC
        ${COMPILER_CMSE_FLAG}
)

target_sources(tfm_sprt
    PRIVATE
        # The SLIH test Partition accesses the timer as ARoT Partitions.
        # Put the driver to SPRT so that the SLIH tests can access it.
        $<$<BOOL:${TEST_NS_SLIH_IRQ}>:${CMAKE_CURRENT_SOURCE_DIR}/Native_Driver/timer_cmsdk_drv.c>
)

#========================= Platform Non-Secure ================================#

target_sources(platform_ns
    PRIVATE
        Native_Driver/uart_pl011_drv.c
        Native_Driver/timer_cmsdk_drv.c
        CMSIS_Driver/Driver_USART.c
        Device/Source/system_core_init.c
        Device/Source/device_definition.c
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/plat_test.c>
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
)

#========================= Platform BL2 =======================================#

if(BL2)
    target_sources(platform_bl2
        PRIVATE
            Device/Source/system_core_init.c
            Device/Source/device_definition.c
            Native_Driver/uart_pl011_drv.c
            CMSIS_Driver/Driver_USART.c
            Native_Driver/musca_b1_scc_drv.c
    )

    target_include_directories(platform_bl2
        PUBLIC
            partition
            Device/Include
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
            Libraries
    )

endif()

#========================= tfm_adac ============================================#

if (${PLATFORM_PSA_ADAC_SECURE_DEBUG})
    target_sources(platform_bl2
        PRIVATE
            systick_microsecond_timer.c
            bl2/boot_hal_bl2.c
    )

    target_compile_definitions(platform_bl2
        PRIVATE
            PLATFORM_PSA_ADAC_SECURE_DEBUG
    )

    target_link_libraries(platform_bl2
        PRIVATE
            trusted-firmware-m-psa-adac
    )

    target_link_libraries(trusted-firmware-m-psa-adac
        PRIVATE
            psa_adac_cc312
    )

endif()

#========================= tfm_spm ============================================#

target_sources(tfm_spm
    PRIVATE
        target_cfg.c
        tfm_hal_isolation.c
        tfm_hal_platform.c
        faults.c
        $<$<OR:$<BOOL:${CONFIG_TFM_FLIH_API}>,$<BOOL:${CONFIG_TFM_SLIH_API}>>:${CMAKE_CURRENT_SOURCE_DIR}/tfm_interrupts.c>
)

#========================= Flash driver definition ===========================#

add_library(flash_drivers STATIC)

target_include_directories(flash_drivers
    PRIVATE
        ${PLATFORM_DIR}/ext/driver
        ${PLATFORM_DIR}/ext/cmsis
        ${PLATFORM_DIR}/ext/common
        Native_Driver
        CMSIS_Driver/Config
        Device/Config
        Device/Include
        Libraries
        .
)

target_sources(flash_drivers
    PRIVATE
        CMSIS_Driver/Driver_GFC100_EFlash.c
        Native_Driver/gfc100_eflash_drv.c
        Native_Driver/musca_b1_eflash_drv.c
        Device/Source/gfc100_eflash_definition.c
)

if(BL2)
    target_link_libraries(platform_bl2
        PRIVATE
            flash_drivers
    )
endif()

target_link_libraries(platform_s
    PRIVATE
        flash_drivers
)
