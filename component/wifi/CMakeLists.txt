
set(APP_LIB_DIR ${c_SDK_LIB_APPLICATION_DIR}) #Used in wifilib.cmake
set(BASEDIR ${c_BASEDIR}) #Used in wifi_include.cmake
set(TARGETDIR ${c_CMPT_SOC_DIR})
set(c_SDK_IMAGE_TARGET_FOLDER ${c_SDK_IMAGE_TARGET_DIR})
set(PROJECTDIR ${c_MCU_PROJECT_DIR})
set(IMAGE_TARGET_FOLDER ${c_SDK_IMAGE_DIR})
include(wifi_include.cmake)
include(wifilib.cmake)
ameba_list_append(WIFI_IFLAGS
    ${c_CMPT_WIFI_DIR}/wifi_fw/common
    ${c_CMPT_WIFI_DIR}/wifi_fw/${c_SOC_TYPE}/include
)
ameba_global_include(${WIFI_IFLAGS})
ameba_global_library(${LINK_APP_LIB})
ameba_global_library(p_NO_WHOLE_ARCHIVE ${LINK_THIRD_APP_LIB})
if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    if(${c_MCU_PROJECT_NAME} STREQUAL "lp")
        #NOTE: For lp, just only add head dirs
        return()
    endif()
endif()

ameba_add_subdirectory(api)

if(NOT CONFIG_AMEBALITE)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E echo
        OUTPUT_FILE wifi_feature_disable/wifi_intf_drv_to_app_ext_noused.c
    )
endif()
ameba_add_subdirectory(whc)
add_subdirectory(./wifi_make/wifi_feature_disable)

if (CONFIG_WLAN)
    if(CONFIG_AS_INIC_AP)
        if(NOT CONFIG_MP_SHRINK)
            ameba_add_subdirectory(wifi_tunnel_app)
            ameba_add_subdirectory(wifi_cast)
            ameba_add_subdirectory(wpa_supplicant/wpa_lite)
            ameba_add_subdirectory(wpa_supplicant/src/wps)
        endif()
    elseif(CONFIG_AS_INIC_NP)
        ameba_add_subdirectory(driver)
        if(CONFIG_ZEPHYR_SDK OR CONFIG_WPA_LOCATION_DEV)
            ameba_add_subdirectory(wpa_supplicant/wpa_lite)
            ameba_add_subdirectory(wpa_supplicant/src/wps)
        endif()
    elseif(CONFIG_SINGLE_CORE_WIFI)
        ameba_add_subdirectory(driver)
        if(NOT CONFIG_MP_SHRINK)
            ameba_add_subdirectory(wpa_supplicant/wpa_lite)
            ameba_add_subdirectory(wpa_supplicant/src/wps)
        endif()
    endif()
    ameba_add_subdirectory(common)
endif()

# make wifi lib
if(NOT CONFIG_AMEBA_RLS)
    if(CONFIG_WLAN)
        #build AP libs
        if(CONFIG_AS_INIC_AP)
            
        #build NP libs
        elseif(CONFIG_AS_INIC_NP)
            if(CONFIG_WPA_STD)
                add_subdirectory(./wifi_make/wpa_supplicant_std)
            endif()
            
        # build single core wifi libs
        elseif(CONFIG_SINGLE_CORE_WIFI)
            
        endif()
    endif()
endif()
