set(APP_LIB_DIR ${TARGET_LIBAPP_FOLDER})

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

#make app libs
if(CONFIG_WIFI_FW_EN)
    add_subdirectory(target/wifi_fw)
endif()

if(CONFIG_WLAN)
    if(CONFIG_AS_INIC_NP)
        add_subdirectory(rtk_coex)
    endif()
endif()
