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

target_link_libraries(platform_region_defs
    INTERFACE
        tfm_fih_headers
)

#========================= Platform common defs ===============================#

# Specify the location of platform specific build dependencies.
target_sources(tfm_s
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/cmsis_core/startup_an521.c
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
         ${CMAKE_CURRENT_SOURCE_DIR}/cmsis_core/startup_an521.c
    )
    target_add_scatter_file(tfm_ns
        $<$<C_COMPILER_ID:ARMClang>:${PLATFORM_DIR}/ext/common/armclang/tfm_common_ns.sct>
        $<$<C_COMPILER_ID:GNU>:${PLATFORM_DIR}/ext/common/gcc/tfm_common_ns.ld>
        $<$<C_COMPILER_ID:IAR>:${PLATFORM_DIR}/ext/common/iar/tfm_common_ns.icf>
    )
    target_link_libraries(CMSIS_5_tfm_ns
        INTERFACE
        $<$<AND:$<C_COMPILER_ID:GNU>,$<OR:$<BOOL:${CONFIG_TFM_ENABLE_FP}>,$<BOOL:${CONFIG_TFM_ENABLE_MVE_FP}>>>:CMSIS_5_RTX_V8MMFN>
        $<$<AND:$<C_COMPILER_ID:GNU>,$<NOT:$<OR:$<BOOL:${CONFIG_TFM_ENABLE_FP}>,$<BOOL:${CONFIG_TFM_ENABLE_MVE_FP}>>>>:CMSIS_5_RTX_V8MMN>
            $<$<C_COMPILER_ID:ARMClang>:CMSIS_5_RTX_V8MMN>
            $<$<C_COMPILER_ID:IAR>:CMSIS_5_RTX_V8MMN>
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
            ${CMAKE_CURRENT_SOURCE_DIR}/cmsis_core/startup_an521.c
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
        ../common
        cmsis_drivers
        native_drivers
        partition
        retarget
        cmsis_core
        native_drivers/timer_cmsdk
        ${PLATFORM_DIR}/..
)

target_sources(platform_s
    INTERFACE
        $<$<STREQUAL:"${TFM_FIH_PROFILE}","HIGH">:${PLATFORM_DIR}/ext/common/template/tfm_fih_rng.c>
    PRIVATE
        cmsis_drivers/Driver_Flash.c
        cmsis_drivers/Driver_MPC.c
        cmsis_drivers/Driver_PPC.c
        cmsis_drivers/Driver_USART.c
        retarget/platform_retarget_dev.c
        cmsis_core/system_core_init.c
        native_drivers/mpc_sie200_drv.c
        native_drivers/mpu_armv8m_drv.c
        native_drivers/ppc_sse200_drv.c
        native_drivers/ppc_sse200_drv.c
        native_drivers/arm_uart_drv.c
        $<$<AND:$<NOT:$<BOOL:${TEST_NS_SLIH_IRQ}>>,$<NOT:$<BOOL:${TEST_NS_FLIH_IRQ}>>>:${CMAKE_CURRENT_SOURCE_DIR}/native_drivers/timer_cmsdk/timer_cmsdk.c>
        $<$<OR:$<BOOL:${TFM_S_REG_TEST}>,$<BOOL:${TFM_NS_REG_TEST}>>:${CMAKE_CURRENT_SOURCE_DIR}/plat_test.c>
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/services/src/tfm_platform_system.c>
)

target_sources(tfm_sprt
    PRIVATE
        # SLIH test Partition and FLIH test Partition access the timer as ARoT Partitions.
        # Put the driver to SPRT so that both SLIH and FLIH tests can access it.
        $<$<OR:$<BOOL:${TEST_NS_SLIH_IRQ}>,$<BOOL:${TEST_NS_FLIH_IRQ}>>:${CMAKE_CURRENT_SOURCE_DIR}/native_drivers/timer_cmsdk/timer_cmsdk.c>
)

target_compile_options(platform_s
    PUBLIC
        ${COMPILER_CMSE_FLAG}
)

target_compile_definitions(platform_s
    PUBLIC
        $<$<BOOL:${TEST_NS_FPU}>:TEST_NS_FPU>
        $<$<BOOL:${TEST_S_FPU}>:TEST_S_FPU>
)

#========================= Platform Non-Secure ================================#

target_sources(platform_ns
    PRIVATE
        native_drivers/arm_uart_drv.c
        native_drivers/timer_cmsdk/timer_cmsdk.c
        cmsis_drivers/Driver_USART.c
        retarget/platform_retarget_dev.c
        cmsis_core/system_core_init.c
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/plat_test.c>
    INTERFACE
        $<$<BOOL:${TEST_NS_FPU}>:${CMAKE_CURRENT_SOURCE_DIR}/cmsis_core/an521_ns_init.c>
)

target_include_directories(platform_ns
    PUBLIC
        .
        ../common
        ${PLATFORM_DIR}/..
        retarget
        cmsis_core
        native_drivers
        native_drivers/timer_cmsdk
)

#========================= Platform BL2 =======================================#

if(BL2)
    target_sources(platform_bl2
        PRIVATE
            retarget/platform_retarget_dev.c
            cmsis_core/system_core_init.c
            cmsis_drivers/Driver_Flash.c
            native_drivers/arm_uart_drv.c
            cmsis_drivers/Driver_USART.c
    )

    target_include_directories(platform_bl2
        PUBLIC
            partition
            cmsis_core
            retarget
        PRIVATE
            .
            ${PLATFORM_DIR}/..
            native_drivers
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
