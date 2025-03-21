################### ROM LIB ##################
set(ROM_LIB_DIR ${PROJECTDIR}/vsdk/lib/amebalite_rom_acut)

if(CONFIG_LINK_ROM_SYMB)
    set(LINK_ROM_LIB)
    set(LINK_ROM_SYMBOL ${LDDIR}/ameba_rom_symbol_acut.ld)
else()
    set(LINK_ROM_LIB ${ROM_LIB_DIR}/rom.a)
    set(LINK_ROM_SYMBOL ${LDDIR}/ameba_rom_symbol_empty.ld)
endif()


list(APPEND LINK_ROM_SYMBOL ${LDDIR}/ameba_rom_symbol_stdlib.ld)

################### APP LIB ##################
set(APP_LIB_DIR ${TARGET_LIBAPP_FOLDER})
set(LINK_APP_LIB)
set(LINK_THIRD_APP_LIB)

if(CONFIG_TRUSTZONE)
    list(APPEND LINK_THIRD_APP_LIB ${IMAGE_TARGET_FOLDER}/cmse_implib.lib)
endif()

if(CONFIG_BT)
    if(CONFIG_BT_AUDIO_CODEC_SUPPORT)
        list(
            APPEND LINK_THIRD_APP_LIB
            ${APP_LIB_DIR}/lib_btcodec.a
            ${APP_LIB_DIR}/lib_btaudiosync.a
        )
    endif()

    if(NOT CONFIG_BT_ZEPHYR)
        if(CONFIG_BT_BLE_ONLY)
            list(APPEND LINK_THIRD_APP_LIB ${BASEDIR}/component/bluetooth/rtk_stack/platform/amebalite/lib/kr4/ble_only/btgap.a)
        elseif(CONFIG_BT_DUAL_MODE)
            list(
                APPEND LINK_THIRD_APP_LIB
                ${BASEDIR}/component/bluetooth/rtk_stack/platform/amebalite/lib/kr4/dual_mode/btgap.a
                ${BASEDIR}/component/bluetooth/rtk_stack/platform/amebalite/lib/kr4/bt_mgr.a
            )
        endif()

        list(
            APPEND LINK_THIRD_APP_LIB
            ${BASEDIR}/component/bluetooth/rtk_stack/platform/amebalite/lib/kr4/leaudio.a
            ${BASEDIR}/component/bluetooth/rtk_stack/platform/amebalite/lib/kr4/ble_mgr.a
            ${BASEDIR}/component/bluetooth/rtk_stack/platform/amebalite/lib/kr4/sysm.a
        )

        if(CONFIG_BT_MESH_PROVISIONER_SUPPORT OR CONFIG_BT_MESH_DEVICE_SUPPORT)
            list(APPEND LINK_THIRD_APP_LIB ${BASEDIR}/component/bluetooth/rtk_stack/src/mesh/lib/amebalite/kr4/btmesh.a)
        endif()
    endif()
endif()

set(
    APP_LIB
    ${APP_LIB_DIR}/lib_httpc.a
	${APP_LIB_DIR}/lib_httpd.a
	${APP_LIB_DIR}/lib_websocket.a
	${APP_LIB_DIR}/lib_tftp.a
)

if(CONFIG_WLAN)
    # AP Link Library
    if(CONFIG_AS_INIC_AP)
        list(APPEND LINK_APP_LIB ${APP_LIB})
    # NP Link Library
    elseif(CONFIG_AS_INIC_NP)
        if(NOT CONFIG_MP_SHRINK)
            list(APPEND LINK_APP_LIB ${APP_LIB_DIR}/lib_wifi_fw.a)
        endif()
        if(CONFIG_MP_INCLUDED)
            if(CONFIG_MP_SHRINK)
                list(APPEND LINK_APP_LIB ${APP_LIB_DIR}/lib_coex_mp_shrink.a)
            else()
                list(APPEND LINK_APP_LIB ${APP_LIB_DIR}/lib_coex_mp.a)
            endif()
        else()
            list(APPEND LINK_APP_LIB ${APP_LIB_DIR}/lib_coex.a)
        endif()
    # Single Core Library
    elseif(CONFIG_SINGLE_CORE_WIFI)
        list(APPEND LINK_APP_LIB ${APP_LIB})
        if(NOT CONFIG_MP_SHRINK)
            list(APPEND LINK_APP_LIB ${APP_LIB_DIR}/lib_wifi_fw.a)
        endif()
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

# coex api
if(CONFIG_CORE_AS_AP OR ((NOT CONFIG_CORE_AS_NP) AND CONFIG_FULLMAC_DEV))
    list(
        APPEND LINK_APP_LIB 
        ${APP_LIB_DIR}/lib_coex_api.a
    )
endif()

# soc
list(
    APPEND LINK_APP_LIB
    ${TARGET_LIBSOC_FOLDER}/lib_pmc.a
    ${TARGET_LIBSOC_FOLDER}/lib_chipinfo.a
    ${TARGET_LIBSOC_FOLDER}/lib_crashdump.a
)

#audio
if(CONFIG_MEDIA_PLAYER OR CONFIG_MEDIA_LITE_PLAYER)
    list(APPEND LINK_APP_LIB ${APP_LIB_DIR}/lib_playback.a)
    if(CONFIG_MEDIA_CODEC_AAC OR CONFIG_MEDIA_LITE_CODEC_AAC)
        list(APPEND LINK_THIRD_APP_LIB ${APP_LIB_DIR}/lib_fdkaac.a)
    endif()
    if(CONFIG_MEDIA_CODEC_HAAC OR CONFIG_MEDIA_LITE_CODEC_HAAC)
        list(APPEND LINK_THIRD_APP_LIB ${APP_LIB_DIR}/lib_haac.a)
    endif()
    if(CONFIG_MEDIA_CODEC_OPUS)
        # list(APPEND LINK_THIRD_APP_LIB ${APP_LIB_DIR}/lib_opus.a)
    endif()
    if(CONFIG_MEDIA_DEMUX_FLAC)
        list(APPEND LINK_THIRD_APP_LIB ${APP_LIB_DIR}/lib_flac.a)
    endif()
    if(CONFIG_MEDIA_DEMUX_OGG)
        list(APPEND LINK_THIRD_APP_LIB ${APP_LIB_DIR}/lib_vorbisdec.a)
    endif()
    if(CONFIG_MEDIA_CODEC_GSM)
        list(APPEND LINK_THIRD_APP_LIB ${APP_LIB_DIR}/lib_gsm.a)
    endif()
endif()

if(CONFIG_SPEEX_LIB)
    list(APPEND LINK_THIRD_APP_LIB ${APP_LIB_DIR}/lib_speexdsp.a)
endif()
if(CONFIG_OPUS_LIB OR CONFIG_MEDIA_CODEC_OPUS)
    list(APPEND LINK_THIRD_APP_LIB ${APP_LIB_DIR}/lib_opus.a)
endif()


if(CONFIG_AUDIO_MIXER)
    list(APPEND LINK_APP_LIB ${APP_LIB_DIR}/lib_audio_mixer.a)
elseif(CONFIG_AUDIO_PASSTHROUGH)
    list(APPEND LINK_APP_LIB ${APP_LIB_DIR}/lib_audio_passthrough.a)
endif()

if(CONFIG_RPC_EN)
    list(APPEND LINK_APP_LIB ${APP_LIB_DIR}/lib_rpc.a)
endif()
if(CONFIG_AUDIO_RPC_AFE OR CONFIG_AUDIO_LOCAL_AFE OR CONFIG_AUDIO_FWK)
    list(APPEND LINK_APP_LIB ${APP_LIB_DIR}/lib_base.a)
endif()
if(CONFIG_RPC_EN)
    #list(APPEND LINK_APP_LIB ${APP_LIB_DIR}/lib_rpc_hal.a)
endif()

############# WIFI LIB #############

include(${WIFIMAKEDIR}/wifilib.cmake)
