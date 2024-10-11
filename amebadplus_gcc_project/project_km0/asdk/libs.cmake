set(APP_LIB_DIR ${TARGET_LIBAPP_FOLDER})

if(CONFIG_AMEBADPLUS_A_CUT)
    set(ROM_LIB_DIR  ${PROJECTDIR}/asdk/lib/amebadplus_rom_acut)
endif()

################### ROM LIB ##################
if(CONFIG_LINK_ROM_SYMB) 
    set(LINK_ROM_LIB)
    set(LINK_ROM_SYMBOL ${LDDIR}/ameba_rom_symbol_acut.ld)
else()
    set(LINK_ROM_LIB ${ROM_LIB_DIR}/rom.a)
    set(LINK_ROM_SYMBOL ${LDDIR}/ameba_rom_symbol_empty.ld)
endif()


list(APPEND LINK_ROM_SYMBOL ${LDDIR}/ameba_rom_symbol_stdlib.ld)

################### APP LIB ##################

set(LINK_APP_LIB ${ASDK_TOOLCHAIN}/arm-none-eabi/lib/thumb/v8-m.base/libm.a)

if(CONFIG_WIFI_FW_EN)
    if(CONFIG_FW_DRIVER_COEXIST)
        list(APPEND LINK_APP_LIB ${APP_LIB_DIR}/lib_wifi_fw.a)
    else()
        list(APPEND LINK_APP_LIB ${APP_LIB_DIR}/lib_wifi_fw_singlecore.a)
    endif()
endif()

if(CONFIG_WLAN)
    if(CONFIG_AS_INIC_NP)
        if(CONFIG_MP_INCLUDED)
            if(CONFIG_MP_SHRINK)
                list(APPEND LINK_APP_LIB ${APP_LIB_DIR}/lib_coex_mp_shrink.a)
            else()
                list(APPEND LINK_APP_LIB ${APP_LIB_DIR}/lib_coex_mp.a)
            endif()
        else()
            list(APPEND LINK_APP_LIB ${APP_LIB_DIR}/lib_coex.a)
        endif()
    endif()
endif()

list(
    APPEND LINK_APP_LIB
    ${TARGET_LIBSOC_FOLDER}/lib_pmc.a 
    ${TARGET_LIBSOC_FOLDER}/lib_chipinfo.a 
    ${TARGET_LIBSOC_FOLDER}/lib_crashdump.a 
)

############# WIFI LIB #############

include(${WIFIMAKEDIR}/wifilib.cmake)
