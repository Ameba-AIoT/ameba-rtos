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
        ${CMAKE_CURRENT_SOURCE_DIR}/cmsis_core/startup_amebadplus.c
        ${CMAKE_CURRENT_SOURCE_DIR}/cmsis_core/wakeup_amebadplus.c
)

# Use a common scatter file for Isolation L1 and L2, a dedicated one for Isolation L3
# IAR is not supported for L3
target_add_scatter_file(tfm_s
    $<$<AND:$<VERSION_LESS:${TFM_ISOLATION_LEVEL},3>,$<C_COMPILER_ID:GNU>>:${CMAKE_CURRENT_SOURCE_DIR}/ld/tfm_amebadplus_s.ld>
)

if(NS)
    target_sources(tfm_ns
        PRIVATE
         ${CMAKE_CURRENT_SOURCE_DIR}/cmsis_core/amebadplus_ns_init.c
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
		# ${CMAKE_BINARY_DIR}/../../../../inc/inc_km4
		# ${CMAKE_SOURCE_DIR}/../../amebadplus/cmsis
		${CMAKE_SOURCE_DIR}/../../../os/os_wrapper/include
		${CMAKE_SOURCE_DIR}/../../amebadplus/fwlib/include
        ${CMAKE_SOURCE_DIR}/../../amebadplus/lib/include
		${CMAKE_SOURCE_DIR}/../../amebadplus/swlib
)

target_sources(platform_s
    INTERFACE
        $<$<STREQUAL:"${TFM_FIH_PROFILE}","HIGH">:${PLATFORM_DIR}/ext/common/template/tfm_fih_rng.c>
    PRIVATE
        cmsis_drivers/Driver_Flash.c
        native_drivers/mpu_armv8m_drv.c
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
		CONFIG_ARM_CORE_CM4
)

#========================= Platform Non-Secure ================================#

target_sources(platform_ns
    PRIVATE
        native_drivers/arm_uart_drv.c
        native_drivers/timer_cmsdk/timer_cmsdk.c
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
		${CMAKE_SOURCE_DIR}/../../../os/os_wrapper/include
		${CMAKE_SOURCE_DIR}/../../amebadplus/fwlib/include
		${CMAKE_SOURCE_DIR}/../../amebadplus/swlib
)

target_compile_definitions(platform_ns
    PUBLIC
		CONFIG_ARM_CORE_CM4
)

#========================= tfm_spm ============================================#

target_sources(tfm_spm
    PRIVATE
        target_cfg.c
        tfm_hal_isolation.c
        tfm_hal_platform.c
        $<$<OR:$<BOOL:${CONFIG_TFM_FLIH_API}>,$<BOOL:${CONFIG_TFM_SLIH_API}>>:${CMAKE_CURRENT_SOURCE_DIR}/tfm_interrupts.c>
)
