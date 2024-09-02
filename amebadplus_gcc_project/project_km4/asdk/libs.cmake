set(APP_LIB_DIR ${TARGET_LIBAPP_FOLDER})
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
                list(
                    APPEND LINK_THIRD_APP_LIB
                    ${BASEDIR}/component/bluetooth/rtk_stack/src/mesh/lib/amebadplus/btmesh.a
                )
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
	#${APP_LIB_DIR}/lib_mdns.a 
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
    elseif(CONFIG_AS_INIC_AP)
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

list(
    APPEND LINK_APP_LIB
    ${APP_LIB_DIR}/lib_ui_drivers.a
)

#make libs
if(CONFIG_WLAN)
    if(CONFIG_AS_INIC_AP)
        add_subdirectory(network/httpc)
        add_subdirectory(network/httpd)
        add_subdirectory(network/websocket)
        add_subdirectory(network/ipnat)
        add_subdirectory(network/ipdnrd)
        add_subdirectory(network/tftp)
    elseif(CONFIG_AS_INIC_NP)
        add_subdirectory(rtk_coex)
    elseif(CONFIG_SINGLE_CORE_WIFI)
        add_subdirectory(network/httpc)
        add_subdirectory(network/httpd)
        add_subdirectory(network/websocket)
        add_subdirectory(network/ipnat)
        add_subdirectory(network/ipdnrd)
        add_subdirectory(network/tftp)
        add_subdirectory(rtk_coex)
    endif()
endif()

if(CONFIG_AUDIO_EN)
    add_subdirectory(cmsis-dsp)
    add_subdirectory(audio/ac3)
    add_subdirectory(audio/amr)
    add_subdirectory(audio/flac)
    add_subdirectory(audio/haac)
    add_subdirectory(audio/hmp3)
    add_subdirectory(audio/mp3)
    add_subdirectory(audio/gsm610)
    add_subdirectory(audio/ekho)
    add_subdirectory(audio/libogg-1.3.3)
    add_subdirectory(audio/opus-1.3.1)
    add_subdirectory(audio/opusfile-0.11)
    add_subdirectory(audio/libopusenc-0.2.1)
    add_subdirectory(audio/m4a_selfparse)
endif()

add_subdirectory(usb_otg/device/lib)

if(CONFIG_WFA_SRC)
    add_subdirectory(application/wifi-audio-src)
endif()

if(CONFIG_WLAN)
    if(CONFIG_WIFI_FW_EN)
        add_subdirectory(target/wifi_fw)
    endif()
endif()

add_subdirectory(ui/drivers)