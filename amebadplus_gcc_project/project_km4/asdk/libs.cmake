################### ROM LIB ##################
if(CONFIG_AMEBADPLUS_A_CUT)
    set(ROM_LIB_DIR  ${PROJECTDIR}/asdk/lib/amebadplus_rom_acut)
elseif(CONFIG_AMEBADPLUS_B_CUT)
    set(ROM_LIB_DIR ${PROJECTDIR}/asdk/lib/amebadplus_rom_bcut)
else()

endif()

if(CONFIG_LINK_ROM_SYMB)
    set(LINK_ROM_LIB)
    set(LINK_ROM_LIB_NS)
    set(LINK_ROM_SYMBOL_NS ${LDDIR}/ameba_rom_symbol_acut.ld)
    set(LINK_ROM_SYMBOL_S  ${LDDIR}/ameba_rom_symbol_acut_s.ld)
else()
    set(LINK_ROM_LIB ${ROM_LIB_DIR}/rom.a)
    set(LINK_ROM_LIB_NS ${ROM_LIB_DIR}/rom_ns.a)
    set(LINK_ROM_SYMBOL_NS ${LDDIR}/ameba_rom_symbol_empty.ld)
    set(LINK_ROM_SYMBOL_S ${LDDIR}/ameba_rom_symbol_empty.ld)
endif()

list(
    APPEND LINK_ROM_LIB
    ${TARGET_LIBSOC_FOLDER}/lib_chipinfo.a
)

list(
    APPEND LINK_ROM_LIB_NS
    ${TARGET_LIBSOC_FOLDER}/lib_chipinfo.a
)

set(LINK_ROM_SYMBOL_STDLIB ${LDDIR}/ameba_rom_symbol_stdlib.ld)

################### LOADER LIB ##################
set(
    LINK_LOADER_LIB
    ${TARGET_LIBSOC_FOLDER}/lib_bootloader.a
    ${TARGET_LIBSOC_FOLDER}/lib_crashdump.a
)

################### APP LIB ##################
set(APP_LIB_DIR ${TARGET_LIBAPP_FOLDER})
set(LINK_APP_LIB)
set(LINK_THIRD_APP_LIB)

if(CONFIG_AUDIO_EN)
    list(
        APPEND LINK_APP_LIB
        ${APP_LIB_DIR}/lib_ac3.a
        ${APP_LIB_DIR}/lib_amr.a
        ${APP_LIB_DIR}/lib_haac.a
        #${APP_LIB_DIR}/lib_hls.a
        ${APP_LIB_DIR}/lib_hmp3.a
        #${APP_LIB_DIR}/lib_m4a.a
        ${APP_LIB_DIR}/lib_m4a_self.a
        ${APP_LIB_DIR}/lib_mp3.a
        ${APP_LIB_DIR}/lib_flac.a
        ${APP_LIB_DIR}/lib_ekho.a
        ${APP_LIB_DIR}/lib_gsm610.a
        #for cyberon tts
        #${APP_LIB_DIR}/LIBtts2.a
        ${APP_LIB_DIR}/lib_opusfile.a
        ${APP_LIB_DIR}/lib_opusenc.a
        ${APP_LIB_DIR}/lib_libogg.a
        ${APP_LIB_DIR}/lib_opus.a
        
        # MMF LIBRARY
        # ${APP_LIB_DIR}/lib_rtsp.a
        # ${APP_LIB_DIR}/lib_g711.a
        # ${APP_LIB_DIR}/lib_haac.a
        # ${APP_LIB_DIR}/lib_faac.a
        ${APP_LIB_DIR}/lib_cmsis_dsp.a
    )
endif()


if(CONFIG_RDP_BASIC)
    list(APPEND LINK_THIRD_APP_LIB ${IMAGE_TARGET_FOLDER}/cmse_implib.lib)
endif()

if(CONFIG_RDP_TFM)
    list(
        APPEND LINK_THIRD_APP_LIB 
        ${IMAGE_TARGET_FOLDER}/s_veneers.o
        ${IMAGE_TARGET_FOLDER}/lib_tfm_ns.a
    )
endif()

if(CONFIG_BT)
    if(NOT CONFIG_BT_INIC)
        if (NOT CONFIG_BT_ZEPHYR)
            list(
                APPEND LINK_THIRD_APP_LIB
                ${BASEDIR}/component/bluetooth/rtk_stack/platform/amebadplus/lib/btgap.a
            )
            if(CONFIG_BT_MESH_PROVISIONER_SUPPORT OR CONFIG_BT_MESH_DEVICE_SUPPORT)
                if(CONFIG_BT_MESH_BASED_ON_CODED_PHY)
                    list(
                        APPEND LINK_THIRD_APP_LIB
                        ${BASEDIR}/component/bluetooth/rtk_stack/src/mesh/lib/amebadplus/btmesh_coded_phy.a
                    )
                else()
                    list(
                        APPEND LINK_THIRD_APP_LIB
                        ${BASEDIR}/component/bluetooth/rtk_stack/src/mesh/lib/amebadplus/btmesh.a
                    )
                endif()
            endif()
        endif()
    endif()
endif()

list(
    APPEND APP_LIB
    ${APP_LIB_DIR}/lib_httpc.a
	${APP_LIB_DIR}/lib_httpd.a
	${APP_LIB_DIR}/lib_websocket.a
	${APP_LIB_DIR}/lib_ipnat.a
	${APP_LIB_DIR}/lib_ipdnrd.a
	${APP_LIB_DIR}/lib_tftp.a
	#${APP_LIB_DIR}/lib_coap.a
	#${APP_LIB_DIR}/lib_dct.a
	#${APP_LIB_DIR}/lib_eap.a
	#${APP_LIB_DIR}/lib_rt_airkiss_algo.a
	#$(BASEDIR)/component/application/airkiss/2.0.0-26515/lib/libairkiss.a
)

if(CONFIG_WLAN)
    if(CONFIG_WIFI_FW_EN)
        list(
            APPEND LINK_APP_LIB
            ${APP_LIB_DIR}/lib_wifi_fw.a
        )
    endif()
    if(CONFIG_AS_INIC_AP)
        list(
            APPEND LINK_APP_LIB
            ${APP_LIB}
        )        
    elseif(CONFIG_AS_INIC_NP)
        if(CONFIG_MP_INCLUDED)
            list(
                APPEND LINK_APP_LIB
                ${APP_LIB_DIR}/lib_coex_mp.a
            )
        else()
            list(
                APPEND LINK_APP_LIB
                ${APP_LIB_DIR}/lib_coex.a
            )
        endif()
    elseif(CONFIG_SINGLE_CORE_WIFI)
        list(
            APPEND LINK_APP_LIB
            ${APP_LIB}
        )
        if(CONFIG_MP_INCLUDED)
            list(
                APPEND LINK_APP_LIB
                ${APP_LIB_DIR}/lib_coex_mp.a
            )
        else()
            list(
                APPEND LINK_APP_LIB
                ${APP_LIB_DIR}/lib_coex.a
            )
        endif()        
    endif()
endif()

# coex api
if(CONFIG_CORE_AS_AP OR ((NOT CONFIG_CORE_AS_NP) AND CONFIG_FULLMAC_MENU))
    list(
        APPEND LINK_APP_LIB 
        ${APP_LIB_DIR}/lib_coex_api.a
    )
endif()

if(CONFIG_USB_DEVICE_EN)
    list(
        APPEND LINK_APP_LIB
        ${APP_LIB_DIR}/lib_usbd.a
    )
endif()

list(
    APPEND LINK_APP_LIB
    ${TARGET_LIBSOC_FOLDER}/lib_pmc.a
    ${TARGET_LIBSOC_FOLDER}/lib_crashdump.a
)

if(CONFIG_WIFI_AUDIO)
    list(
        APPEND LINK_THIRD_APP_LIB
        -Wl,--whole-archive ${BASEDIR}/component/application/wifi-audio/lib/lib_wfa_pal.a -Wl,--no-whole-archive
        ${BASEDIR}/component/application/wifi-audio/lib/lib_wifi_audio.a
    )
    list(
        APPEND LINK_APP_LIB
        ${APP_LIB_DIR}/lib_flac.a
    )
endif()

if(CONFIG_WFA_SRC)
    list(
        APPEND LINK_THIRD_APP_LIB
        -Wl,--whole-archive ${APP_LIB_DIR}/lib_wfa_pal.a -Wl,--no-whole-archive
        ${APP_LIB_DIR}/lib_wifi_audio.a
    )
    list(
        APPEND LINK_APP_LIB
        ${APP_LIB_DIR}/lib_flac.a
    )
endif()

if(CONFIG_MEDIA_PLAYER)
    list(
        APPEND LINK_APP_LIB
        ${APP_LIB_DIR}/lib_playback.a
    )
    if(CONFIG_MEDIA_CODEC_AAC)
        list(
            APPEND LINK_APP_LIB
            ${APP_LIB_DIR}/lib_fdkaac.a
        )
    endif()
    if(CONFIG_MEDIA_CODEC_HAAC)
        list(
            APPEND LINK_APP_LIB
            ${APP_LIB_DIR}/lib_haac.a
        )
    endif()
endif()

if(CONFIG_SPEEX_LIB)
    list(
        APPEND LINK_APP_LIB 
        ${APP_LIB_DIR}/lib_speexdsp.a
    )
endif()

if(CONFIG_OPUS_LIB)
    list(
        APPEND LINK_APP_LIB 
        ${APP_LIB_DIR}/lib_opus.a
    )
endif()

if(CONFIG_AUDIO_MIXER)
    list(
        APPEND LINK_APP_LIB
        ${APP_LIB_DIR}/lib_audio_mixer.a
    )
elseif(CONFIG_AUDIO_PASSTHROUGH)
    list(
        APPEND LINK_APP_LIB
        ${APP_LIB_DIR}/lib_audio_passthrough.a
    )
endif()

if(CONFIG_AUDIO_FWK)
    list(
        APPEND LINK_APP_LIB
        ${APP_LIB_DIR}/lib_base.a
    )
endif()

if(CONFIG_TFLITE_MICRO_EN)
    list(
        APPEND LINK_APP_LIB
        ${APP_LIB_DIR}/lib_tflite_micro.a
        ${APP_LIB_DIR}/lib_cmsis_nn.a
    )
endif()

list(
    APPEND LINK_APP_LIB
    ${APP_LIB_DIR}/lib_ui_drivers.a
)

############# WIFI LIB #############

include(${WIFIMAKEDIR}/wifilib.cmake)

############# WPAN LIB #############

if(CONFIG_802154_EN)
	include(make/wpan/wpanlib.cmake)
endif()

