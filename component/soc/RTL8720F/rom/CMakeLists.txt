##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above
ameba_list_append(public_includes
    include
)

if(${c_MCU_PROJECT_NAME} STREQUAL "km4tz" AND CONFIG_TRUSTZONE)
    ameba_list_append(public_libraries
        ${c_SDK_LIB_SOC_DIR}/lib_rom_tz.a
    )
else()
    ameba_list_append(public_libraries
        ${c_SDK_LIB_SOC_DIR}/lib_rom.a
    )
endif()


# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries}) #default: whole-archived

##########################################################################################
## * This part defines private part of the component
## * Private part is used to build target of current component
## * NOTE: The build API guarantees the global build configures(mentioned above)
## *       applied to the target automatically. So if any configure was already added
## *       to public above, it's unnecessary to add again below.

#NOTE: User defined section, add your private build configures here
# You may use if-else condition to set these predefined variable
# They are only for ameba_add_internal_library/ameba_add_external_app_library/ameba_add_external_soc_library
set(private_sources)                 #private source files, NOTE: relative path is OK
set(private_includes)                #private include directories, NOTE: relative path is OK
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------#
# Component private part, user config begin
if(${c_CURRENT_IMAGE_TYPE} STREQUAL "image2")
    if(${c_MCU_PROJECT_NAME} STREQUAL "km4tz")
        if(CONFIG_TRUSTZONE)
            ameba_list_append(private_sources
                rom_${c_MCU_PROJECT_NAME}/ameba_vector_rom.c

                rom_km4tz/ameba_crypto_rsip.c
                rom_km4tz/ameba_rsip_rom.c

                rom_common/ameba_pinmux_rom.c
                rom_common/ameba_rcc_rom.c
                rom_common/ameba_assert_rom.c
                rom_common/ameba_otp_rom.c
                rom_common/ameba_shell_rom.c
                rom_common/ameba_tim_rom.c
                rom_common/ameba_gpio_rom.c
                rom_common/ameba_trng.c
                rom_common/ameba_chipinfo_rom.c
                rom_common/ameba_loguart_rom.c
                rom_common/ameba_uart_rom.c
                rom_common/ameba_clk_rom.c
                rom_common/ameba_sema_rom.c
                rom_common/ameba_systimer_rom.c
                rom_common/ameba_wdg.c

                rom_common/ameba_crypto_aes.c
                rom_common/ameba_crypto_sha2.c
                rom_common/ameba_crypto_key_management.c
                rom_common/ameba_crypto.c
                rom_common/ameba_crypto_sm3.c
                rom_common/ameba_crypto_sm4.c
                rom_common/ameba_crypto_hash.c
                rom_common/ameba_crypto_symmetric_cipher.c

                rom_common/ameba_pke_common.c
                rom_common/ameba_pke_ecc.c
                rom_common/ameba_pke_rsa.c

                crashdump/ameba_dump_stack.c
                crashdump/ameba_v8m_crashdump.c

                stdlib/diag.c
                stdlib/log.c
                stdlib/sscanf_minimal.c
            )
        else()
            ameba_list_append(private_sources
                rom_common/ameba_systimer_rom.c
                stdlib/sscanf_minimal.c
            )
        endif()
    elseif(${c_MCU_PROJECT_NAME} STREQUAL "km4ns")
        ameba_list_append(private_sources
            rom_common/ameba_otp_rom.c
            rom_common/ameba_shell_rom.c
            rom_common/ameba_tim_rom.c
            rom_common/ameba_gpio_rom.c
            rom_common/ameba_trng.c
            rom_common/ameba_chipinfo_rom.c
            rom_common/ameba_loguart_rom.c
            rom_common/ameba_uart_rom.c
            rom_common/ameba_clk_rom.c
            rom_common/ameba_sema_rom.c
            rom_common/ameba_systimer_rom.c
            rom_common/ameba_wdg.c
            rom_common/ameba_sdio_rom.c

            rom_common/ameba_crypto_aes.c
            rom_common/ameba_crypto_sha2.c
            rom_common/ameba_crypto_key_management.c
            rom_common/ameba_crypto.c
            rom_common/ameba_crypto_sm3.c
            rom_common/ameba_crypto_sm4.c
            rom_common/ameba_crypto_hash.c
            rom_common/ameba_crypto_symmetric_cipher.c

            rom_common/ameba_pke_common.c
            rom_common/ameba_pke_ecc.c
            rom_common/ameba_pke_rsa.c

            rom_km4tz/ameba_crypto_rsip.c
            rom_km4tz/ameba_rsip_rom.c
            rom_km4tz/ameba_secure_boot_s.c

            fault_injection_hardening/fault_injection_hardening_delay_rng.c
            fault_injection_hardening/fault_injection_hardening.c
            fault_injection_hardening/bootutil_misc.c

            crashdump/ameba_dump_stack.c
            crashdump/ameba_v8m_crashdump.c

            stdlib/sscanf_minimal.c
        )
    endif()
endif()
# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For closed-source component, only build before release and libs are packaged into lib/soc
if((${c_MCU_PROJECT_NAME} STREQUAL "km4tz") AND CONFIG_TRUSTZONE)
    ameba_add_external_soc_library(rom_tz
        p_SOURCES
            ${private_sources}
        p_INCLUDES
            ${private_includes}
        p_DEFINITIONS
            ${private_definitions}
        p_COMPILE_OPTIONS
            ${private_compile_options}
    )
else()
    ameba_add_external_soc_library(rom
        p_SOURCES
            ${private_sources}
        p_INCLUDES
            ${private_includes}
        p_DEFINITIONS
            ${private_definitions}
        p_COMPILE_OPTIONS
            ${private_compile_options}
    )
endif()
##########################################################################################
