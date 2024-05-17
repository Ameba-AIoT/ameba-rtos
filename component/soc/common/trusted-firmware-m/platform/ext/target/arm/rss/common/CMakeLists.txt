#-------------------------------------------------------------------------------
# Copyright (c) 2020-2023, Arm Limited. All rights reserved.
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
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
        cc312
)

target_compile_definitions(platform_region_defs
    INTERFACE
        $<$<BOOL:${RSS_XIP}>:RSS_XIP>
        $<$<BOOL:${RSS_DEBUG_UART}>:RSS_DEBUG_UART>
        $<$<BOOL:${RSS_GPT_SUPPORT}>:RSS_GPT_SUPPORT>
)

#========================= Platform common defs ===============================#

# Specify the location of platform specific build dependencies.
target_sources(tfm_s
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_rss.c
)
target_add_scatter_file(tfm_s
        $<$<C_COMPILER_ID:ARMClang>:${PLATFORM_DIR}/ext/common/armclang/tfm_common_s.sct>
        $<$<C_COMPILER_ID:GNU>:${PLATFORM_DIR}/ext/common/gcc/tfm_common_s.ld>
)

if(NS)
    target_sources(tfm_ns
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_rss.c
    )
    target_add_scatter_file(tfm_ns
            $<$<C_COMPILER_ID:ARMClang>:${PLATFORM_DIR}/ext/common/armclang/tfm_common_ns.sct>
            $<$<C_COMPILER_ID:GNU>:${PLATFORM_DIR}/ext/common/gcc/tfm_common_ns.ld>
    )
    target_link_libraries(CMSIS_5_tfm_ns
        INTERFACE
            CMSIS_5_RTX_V8MMN
    )
endif()

target_sources(bl2
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_rss.c
        $<$<BOOL:${PLATFORM_HAS_BOOT_DMA}>:${CMAKE_CURRENT_SOURCE_DIR}/bl2/boot_dma.c>
)

target_add_scatter_file(bl2
        $<$<C_COMPILER_ID:ARMClang>:${PLATFORM_DIR}/ext/common/armclang/tfm_common_bl2.sct>
        $<$<C_COMPILER_ID:GNU>:${PLATFORM_DIR}/ext/common/gcc/tfm_common_bl2.ld>
)

target_compile_options(bl2_scatter
    PRIVATE
        $<$<BOOL:${PLATFORM_HAS_BOOT_DMA}>:${COMPILER_CMSE_FLAG}>
)

target_compile_definitions(bootutil
    PRIVATE
        MULTIPLE_EXECUTABLE_RAM_REGIONS
)

target_sources(bl1_1
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_rss.c
)
target_add_scatter_file(bl1_1
        $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/armclang/rss_bl1_1.sct>
        $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/gcc/rss_bl1_1.ld>
)

target_sources(bl1_2
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_rss.c
)
target_add_scatter_file(bl1_2
        $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/armclang/rss_bl1_2.sct>
        $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/gcc/rss_bl1_2.ld>
)


install(FILES       tfm_builtin_key_ids.h
        DESTINATION ${TFM_INSTALL_PATH}/interface/include)

#========================= Platform Secure ====================================#

if (TFM_MULTI_CORE_TOPOLOGY)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/rss_comms rss_comms)
endif()

target_include_directories(platform_s
    PUBLIC
        .
        ../common
        cmsis_drivers
        cmsis_drivers/config
        device
        device/config
        device/include
        device/source/armclang
        native_drivers
        libraries
        partition
        services/src
        ${PLATFORM_DIR}/..
        ${PLATFORM_DIR}/ext/target/arm/drivers/counter/armv8m
        ${PLATFORM_DIR}/ext/target/arm/drivers/mpc_sie
        ${PLATFORM_DIR}/ext/target/arm/drivers/mpu/armv8m
        ${PLATFORM_DIR}/ext/target/arm/drivers/timer/armv8m
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/cmsdk
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/common
)

target_sources(platform_s
    PRIVATE
        cmsis_drivers/Driver_MPC.c
        cmsis_drivers/Driver_PPC.c
        device/source/device_definition.c
        device/source/host_device_definition.c
        device/source/system_core_init.c
        libraries/dma350_lib.c
        native_drivers/atu_rss_drv.c
        native_drivers/dma350_drv.c
        native_drivers/dma350_ch_drv.c
        native_drivers/kmu_drv.c
        native_drivers/sic_drv.c
        native_drivers/mhu_v2_x.c
        native_drivers/mhu_wrapper_v2_x.c
        native_drivers/ppc_rss_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/counter/armv8m/syscounter_armv8-m_cntrl_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/mpc_sie/mpc_sie_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/mpu/armv8m/mpu_armv8m_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/timer/armv8m/systimer_armv8-m_drv.c
        $<$<BOOL:${RSS_DEBUG_UART}>:${CMAKE_CURRENT_SOURCE_DIR}/cmsis_drivers/Driver_USART_cmsdk.c>
        $<$<BOOL:${RSS_DEBUG_UART}>:${PLATFORM_DIR}/ext/target/arm/drivers/usart/cmsdk/uart_cmsdk_drv.c>
        tfm_peripherals_def.c
        $<$<OR:$<BOOL:${TEST_NS_SLIH_IRQ}>,$<BOOL:${TEST_NS_FLIH_IRQ}>>:${CMAKE_CURRENT_SOURCE_DIR}/plat_test.c>
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/services/src/tfm_platform_system.c>
        native_drivers/lcm_drv.c
        otp_lcm.c
        nv_counters.c
        attest_hal.c
        tfm_hal_platform_reset.c
        provisioning.c
        $<$<BOOL:${PLATFORM_SVC_HANDLERS}>:${CMAKE_CURRENT_SOURCE_DIR}/platform_svc_handler.c>
        $<$<BOOL:${PLATFORM_SVC_HANDLERS}>:${CMAKE_CURRENT_SOURCE_DIR}/libraries/dma350_checker_layer.c>
        $<$<BOOL:${PLATFORM_SVC_HANDLERS}>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/dma350_checker_device_defs.c>
        spm_dma_copy.c
)

target_link_libraries(platform_s
    PRIVATE
        tfm_sprt # For tfm_strnlen in attest HAL
)

target_sources(tfm_sprt
    PRIVATE
        # SLIH test Partition and FLIH test Partition access the timer as ARoT Partitions.
        # Put the driver to SPRT so that both SLIH and FLIH tests can access it.
        $<$<OR:$<BOOL:${TEST_NS_SLIH_IRQ}>,$<BOOL:${TEST_NS_FLIH_IRQ}>>:${PLATFORM_DIR}/ext/target/arm/drivers/timer/armv8m/systimer_armv8-m_drv.c>
)

target_compile_definitions(platform_s
    INTERFACE
        ATTEST_KEY_BITS=${ATTEST_KEY_BITS}
        spm_memcpy=spm_dma_memcpy
    PRIVATE
        # Needed for DMA-350 library
        CMSIS_device_header="rss.h"
        $<$<BOOL:${PLATFORM_SVC_HANDLERS}>:PLATFORM_SVC_HANDLERS>
)

target_compile_options(platform_s
    PUBLIC
        ${COMPILER_CMSE_FLAG}
)

#========================= Platform Crypto Keys ===============================#

if (TFM_PARTITION_CRYPTO)
    target_sources(platform_crypto_keys
        PRIVATE
            crypto_keys.c
    )

    target_include_directories(platform_crypto_keys
        PUBLIC
            .
            ./native_drivers
            ./device/include
            ./device/config
    )
endif()

#========================= Platform Non-Secure ================================#

target_sources(platform_ns
    PRIVATE
        device/source/device_definition.c
        device/source/host_device_definition.c
        device/source/system_core_init.c
        $<$<BOOL:${RSS_DEBUG_UART}>:${CMAKE_CURRENT_SOURCE_DIR}/cmsis_drivers/Driver_USART_cmsdk.c>
        $<$<BOOL:${RSS_DEBUG_UART}>:${PLATFORM_DIR}/ext/target/arm/drivers/usart/cmsdk/uart_cmsdk_drv.c>
        $<$<BOOL:${TFM_MULTI_CORE_TOPOLOGY}>:${CMAKE_CURRENT_SOURCE_DIR}/platform_ns_mailbox.c>
)

target_include_directories(platform_ns
    PUBLIC
        .
        ../common
        ${PLATFORM_DIR}/..
        cmsis_drivers
        cmsis_drivers/config
        device
        device/config
        device/include
        device/source/armclang
        native_drivers
        partition
        ${PLATFORM_DIR}/ext/target/arm/drivers/counter/armv8m
        ${PLATFORM_DIR}/ext/target/arm/drivers/mpc_sie
        ${PLATFORM_DIR}/ext/target/arm/drivers/timer/armv8m
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/cmsdk
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/common
)

#========================= Platform BL2 =======================================#

target_sources(platform_bl2
    PRIVATE
        tfm_hal_platform_reset.c
        bl2/provisioning.c
        device/source/device_definition.c
        device/source/host_device_definition.c
        device/source/system_core_init.c
        $<$<BOOL:${PLATFORM_HAS_BOOT_DMA}>:${CMAKE_CURRENT_SOURCE_DIR}/libraries/dma350_lib.c>
        native_drivers/atu_rss_drv.c
        $<$<BOOL:${PLATFORM_HAS_BOOT_DMA}>:${CMAKE_CURRENT_SOURCE_DIR}/native_drivers/dma350_ch_drv.c>
        $<$<BOOL:${PLATFORM_HAS_BOOT_DMA}>:${CMAKE_CURRENT_SOURCE_DIR}/native_drivers/dma350_drv.c>
        native_drivers/kmu_drv.c
        native_drivers/sic_drv.c
        native_drivers/mhu_v2_x.c
        $<$<BOOL:${RSS_DEBUG_UART}>:${CMAKE_CURRENT_SOURCE_DIR}/cmsis_drivers/Driver_USART_cmsdk.c>
        $<$<BOOL:${RSS_DEBUG_UART}>:${PLATFORM_DIR}/ext/target/arm/drivers/usart/cmsdk/uart_cmsdk_drv.c>
        native_drivers/lcm_drv.c
        otp_lcm.c
        nv_counters.c
        fip_parser/fip_parser.c
        fip_parser/host_flash_atu.c
        $<$<BOOL:${RSS_GPT_SUPPORT}>:${CMAKE_CURRENT_SOURCE_DIR}/fip_parser/gpt.c>
        $<$<BOOL:${RSS_XIP}>:${CMAKE_CURRENT_SOURCE_DIR}/bl2/sic_boot.c>
)

target_include_directories(platform_bl2
    PUBLIC
        .
        bl2
        cmsis_drivers
        cmsis_drivers/config
        device
        device/config
        device/include
        device/source/armclang
        native_drivers
        partition
        services/src
        libraries
        ${PLATFORM_DIR}/ext/target/arm/drivers/counter/armv8m
        ${PLATFORM_DIR}/ext/target/arm/drivers/mpc_sie
        ${PLATFORM_DIR}/ext/target/arm/drivers/timer/armv8m
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/cmsdk
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/common

    PRIVATE
        ${PLATFORM_DIR}/..
        ./fip_parser
)

target_compile_definitions(platform_bl2
    PRIVATE
        # Needed for DMA-350 library
        $<$<BOOL:${PLATFORM_HAS_BOOT_DMA}>:PLATFORM_HAS_BOOT_DMA>
        $<$<BOOL:${PLATFORM_BOOT_DMA_MIN_SIZE_REQ}>:BOOT_DMA_MIN_SIZE_REQ=${PLATFORM_BOOT_DMA_MIN_SIZE_REQ}>
        $<$<BOOL:${PLATFORM_HAS_BOOT_DMA}>:CMSIS_device_header="rss.h">
)

target_compile_options(platform_bl2
    PUBLIC
        $<$<BOOL:${PLATFORM_HAS_BOOT_DMA}>:${COMPILER_CMSE_FLAG}>
)

target_link_libraries(platform_bl2
    PRIVATE
        $<$<AND:$<BOOL:${CONFIG_TFM_BOOT_STORE_MEASUREMENTS}>,$<BOOL:${TFM_PARTITION_MEASURED_BOOT}>>:tfm_boot_status>
)

target_compile_definitions(platform_bl2
    PUBLIC
        $<$<AND:$<BOOL:${CONFIG_TFM_BOOT_STORE_MEASUREMENTS}>,$<BOOL:${TFM_PARTITION_MEASURED_BOOT}>>:MEASURED_BOOT_API>
)

#========================= Platform BL1 =======================================#

target_sources(platform_bl1
    PRIVATE
        ./bl1/boot_hal_bl1.c
        ./device/source/device_definition.c
        ./device/source/host_device_definition.c
        ./device/source/system_core_init.c
        ./native_drivers/atu_rss_drv.c
        ./native_drivers/kmu_drv.c
        $<$<AND:$<BOOL:${TFM_BL1_LOGGING}>,$<BOOL:${RSS_DEBUG_UART}>>:${CMAKE_CURRENT_SOURCE_DIR}/cmsis_drivers/Driver_USART_cmsdk.c>
        $<$<AND:$<BOOL:${TFM_BL1_LOGGING}>,$<BOOL:${RSS_DEBUG_UART}>>:${PLATFORM_DIR}/ext/target/arm/drivers/usart/cmsdk/uart_cmsdk_drv.c>
        ./native_drivers/lcm_drv.c
        ./otp_lcm.c
        ./nv_counters.c
        ./tfm_hal_platform_reset.c
        ./fip_parser/fip_parser.c
        ./fip_parser/host_flash_atu.c
        $<$<BOOL:${RSS_GPT_SUPPORT}>:${CMAKE_CURRENT_SOURCE_DIR}/fip_parser/gpt.c>
)

target_include_directories(platform_bl1_interface
    INTERFACE
        .
        ./cmsis_drivers
        ./cmsis_drivers/config
        ./device/config
        ./device/include
        ./native_drivers
        ./fip_parser
        ${PLATFORM_DIR}/ext/target/arm/drivers/counter/armv8m
        ${PLATFORM_DIR}/ext/target/arm/drivers/mpc_sie
        ${PLATFORM_DIR}/ext/target/arm/drivers/timer/armv8m
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/cmsdk
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/common
)

target_compile_definitions(platform_bl1
    PUBLIC
        $<$<AND:$<BOOL:${CONFIG_TFM_BOOT_STORE_MEASUREMENTS}>,$<BOOL:${TFM_PARTITION_MEASURED_BOOT}>>:MEASURED_BOOT_API>
)

target_sources(bl1_1_lib
    INTERFACE
        ./bl1/provisioning.c
)

#========================= ns_agent_mailbox ===================================#

if(TFM_MULTI_CORE_TOPOLOGY)
    target_sources(tfm_psa_rot_partition_ns_agent_mailbox
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/tfm_hal_multi_core.c
    )
endif()

#========================= tfm_spm ============================================#

target_sources(tfm_spm
    PRIVATE
        target_cfg.c
        tfm_hal_isolation.c
        tfm_hal_platform.c
        tfm_interrupts.c
        faults.c
)

#=========================== CPAK generator tool ===============================

find_package(Python3)

add_custom_target(cpak_generator
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cpak_generator/cpak_generator.c
)

add_dependencies(cpak_generator
    bl1_2_padded_bin
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator/cpak_generator
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/cpak_generator/cpak_generator.c
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator
    COMMAND ${CMAKE_COMMAND} -S ${CMAKE_CURRENT_SOURCE_DIR}/cpak_generator
                             -B ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator
                             -G ${CMAKE_GENERATOR}
                             -DMBEDCRYPTO_PATH=${MBEDCRYPTO_PATH}
                             -DBL1_2_HASH_PATH=\\\"$<TARGET_FILE_DIR:bl1_2>/bl1_2_padded_hash.bin\\\"
                             -DGUK_PATH=\\\"${TFM_GUK_PATH}\\\"
                             -DCPAK_OUTPUT_PATH=\\\"${CMAKE_CURRENT_BINARY_DIR}/cpak_generator/cpak_public.bin\\\"
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator
)

add_custom_target(cpak_public ALL
    DEPENDS ${TFM_GUK_PATH}
    DEPENDS cpak_generator
    DEPENDS ${CMAKE_BINARY_DIR}/bin/bl1_2_padded_hash.bin
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator/cpak_generator
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/cpak_generator/bin_to_pem.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator/
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator/cpak_generator
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/cpak_generator/bin_to_pem.py
                    --input_key_file ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator/cpak_public.bin
                    --output_key_file ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator/cpak_public.pem
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator/cpak_public.bin $<TARGET_FILE_DIR:bl1_1>
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator/cpak_public.pem $<TARGET_FILE_DIR:bl1_1>
)

############################### IMAGE SIGNING ##################################

if (RSS_XIP)
    find_package(Python3)

    add_custom_target(tfm_s_sic_tables
        SOURCES bin/tfm_s_sic_tables.bin
    )

    add_custom_command(OUTPUT bin/tfm_s_sic_tables.bin
        DEPENDS $<TARGET_FILE_DIR:tfm_s>/tfm_s.bin
        DEPENDS tfm_s_bin
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bl2/create_xip_tables.py
            --input_image $<TARGET_FILE_DIR:tfm_s>/tfm_s.bin
            --table_output_file tfm_s_sic_tables.bin
            --encrypted_image_output_file tfm_s_encrypted.bin
            --image_version ${MCUBOOT_SECURITY_COUNTER_S}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/tfm_s_sic_tables.bin $<TARGET_FILE_DIR:tfm_s>
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/tfm_s_encrypted.bin $<TARGET_FILE_DIR:tfm_s>
    )

    add_custom_target(tfm_ns_sic_tables
        SOURCES bin/tfm_ns_sic_tables.bin
    )

    add_custom_command(OUTPUT bin/tfm_ns_sic_tables.bin
        DEPENDS $<TARGET_FILE_DIR:tfm_ns>/tfm_ns.bin
        DEPENDS tfm_ns_bin
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bl2/create_xip_tables.py
            --input_image $<TARGET_FILE_DIR:tfm_ns>/tfm_ns.bin
            --table_output_file tfm_ns_sic_tables.bin
            --encrypted_image_output_file tfm_ns_encrypted.bin
            --image_version ${MCUBOOT_SECURITY_COUNTER_NS}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/tfm_ns_sic_tables.bin $<TARGET_FILE_DIR:tfm_ns>
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/tfm_ns_encrypted.bin $<TARGET_FILE_DIR:tfm_s>
    )

    set(IMAGE_TYPE "S_IMAGE")
    set(FLASH_AREA_NUM ${MCUBOOT_S_IMAGE_FLASH_AREA_NUM})
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bl2/signing_layout_sic_tables.c.in signing_layout_s.c @ONLY)
    add_library(signing_layout_s OBJECT ${CMAKE_CURRENT_BINARY_DIR}/signing_layout_s.c)

    target_compile_options(signing_layout_s
        PRIVATE
            $<$<C_COMPILER_ID:GNU>:-E\;-xc>
            $<$<C_COMPILER_ID:ARMClang>:-E\;-xc>
            $<$<C_COMPILER_ID:IAR>:--preprocess=ns\;$<TARGET_OBJECTS:signing_layout_s>>
    )
    target_compile_definitions(signing_layout_s
        PRIVATE
            $<$<BOOL:${BL2}>:BL2>
            $<$<BOOL:${MCUBOOT_IMAGE_NUMBER}>:MCUBOOT_IMAGE_NUMBER=${MCUBOOT_IMAGE_NUMBER}>
            $<$<STREQUAL:${MCUBOOT_UPGRADE_STRATEGY},DIRECT_XIP>:IMAGE_ROM_FIXED>
    )
    target_link_libraries(signing_layout_s
        PRIVATE
            platform_bl2
    )

    add_custom_target(tfm_s_sic_tables_signed_bin
        SOURCES bin/tfm_s_sic_tables_signed.bin
    )
    add_custom_command(OUTPUT bin/tfm_s_sic_tables_signed.bin
        DEPENDS tfm_s_sic_tables
        DEPENDS tfm_s_bin signing_layout_s
        WORKING_DIRECTORY ${MCUBOOT_PATH}/scripts

        #Sign secure binary image with provided secret key
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/bl2/ext/mcuboot/scripts/wrapper/wrapper.py
            -v ${MCUBOOT_IMAGE_VERSION_S}
            --layout $<TARGET_OBJECTS:signing_layout_s>
            -k ${MCUBOOT_KEY_S}
            --public-key-format $<IF:$<BOOL:${MCUBOOT_HW_KEY}>,full,hash>
            --align ${MCUBOOT_ALIGN_VAL}
            --pad
            --pad-header
            -H ${BL2_HEADER_SIZE}
            -s ${MCUBOOT_SECURITY_COUNTER_S}
            -L ${MCUBOOT_ENC_KEY_LEN}
            -d \"\(1,${MCUBOOT_NS_IMAGE_MIN_VER}\)\"
            $<$<STREQUAL:${MCUBOOT_UPGRADE_STRATEGY},OVERWRITE_ONLY>:--overwrite-only>
            $<$<BOOL:${MCUBOOT_ENC_IMAGES}>:-E${MCUBOOT_KEY_ENC}>
            $<$<BOOL:${MCUBOOT_MEASURED_BOOT}>:--measured-boot-record>
            $<TARGET_FILE_DIR:tfm_s>/tfm_s_sic_tables.bin
            ${CMAKE_CURRENT_BINARY_DIR}/tfm_s_sic_tables_signed.bin
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/tfm_s_sic_tables_signed.bin $<TARGET_FILE_DIR:bl2>
    )

    set(IMAGE_TYPE "NS_IMAGE")
    set(FLASH_AREA_NUM ${MCUBOOT_NS_IMAGE_FLASH_AREA_NUM})
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bl2/signing_layout_sic_tables.c.in signing_layout_ns.c @ONLY)
    add_library(signing_layout_ns OBJECT ${CMAKE_CURRENT_BINARY_DIR}/signing_layout_ns.c)

    target_compile_options(signing_layout_ns
        PRIVATE
            $<$<C_COMPILER_ID:GNU>:-E\;-xc>
            $<$<C_COMPILER_ID:ARMClang>:-E\;-xc>
            $<$<C_COMPILER_ID:IAR>:--preprocess=ns\;$<TARGET_OBJECTS:signing_layout_ns>>
    )
    target_compile_definitions(signing_layout_ns
        PRIVATE
            $<$<BOOL:${BL2}>:BL2>
            $<$<BOOL:${MCUBOOT_IMAGE_NUMBER}>:MCUBOOT_IMAGE_NUMBER=${MCUBOOT_IMAGE_NUMBER}>
            $<$<STREQUAL:${MCUBOOT_UPGRADE_STRATEGY},DIRECT_XIP>:IMAGE_ROM_FIXED>
    )
    target_link_libraries(signing_layout_ns
        PRIVATE
            platform_bl2
    )

    add_custom_target(tfm_ns_sic_tables_signed_bin
        SOURCES bin/tfm_ns_sic_tables_signed.bin
    )
    add_custom_command(OUTPUT bin/tfm_ns_sic_tables_signed.bin
        DEPENDS tfm_ns_sic_tables
        DEPENDS tfm_ns_bin signing_layout_ns
        WORKING_DIRECTORY ${MCUBOOT_PATH}/scripts

        #Sign non-secure binary image with provided secret key
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/bl2/ext/mcuboot/scripts/wrapper/wrapper.py
            -v ${MCUBOOT_IMAGE_VERSION_NS}
            --layout $<TARGET_OBJECTS:signing_layout_ns>
            -k ${MCUBOOT_KEY_NS}
            --public-key-format $<IF:$<BOOL:${MCUBOOT_HW_KEY}>,full,hash>
            --align ${MCUBOOT_ALIGN_VAL}
            --pad
            --pad-header
            -H ${BL2_HEADER_SIZE}
            -s ${MCUBOOT_SECURITY_COUNTER_NS}
            -L ${MCUBOOT_ENC_KEY_LEN}
            -d \"\(0, ${MCUBOOT_S_IMAGE_MIN_VER}\)\"
            $<TARGET_FILE_DIR:tfm_ns>/tfm_ns_sic_tables.bin
            $<$<STREQUAL:${MCUBOOT_UPGRADE_STRATEGY},OVERWRITE_ONLY>:--overwrite-only>
            $<$<BOOL:${MCUBOOT_ENC_IMAGES}>:-E${MCUBOOT_KEY_ENC}>
            $<$<BOOL:${MCUBOOT_MEASURED_BOOT}>:--measured-boot-record>
            ${CMAKE_CURRENT_BINARY_DIR}/tfm_ns_sic_tables_signed.bin
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/tfm_ns_sic_tables_signed.bin $<TARGET_FILE_DIR:bl2>
    )

    add_custom_target(signed_images
        ALL
        DEPENDS bin/tfm_s_sic_tables_signed.bin
        DEPENDS bin/tfm_ns_sic_tables_signed.bin
    )
endif()
