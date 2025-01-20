if (__INCLUDED_AUDIO_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_AUDIO_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_audio_algorithm_plc INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_algorithm_sonic INTERFACE)
if(NOT d_AMEBA_RELEASE)
    add_library(src_${d_MCU_PROJECT_NAME}_audio_base_audio_utils INTERFACE)
    add_library(src_${d_MCU_PROJECT_NAME}_audio_base_cutils_cmd INTERFACE)
    add_library(src_${d_MCU_PROJECT_NAME}_audio_base_cutils_src INTERFACE)
    add_library(src_${d_MCU_PROJECT_NAME}_audio_base_libs INTERFACE)
    add_library(src_${d_MCU_PROJECT_NAME}_audio_base_log INTERFACE)
    add_library(src_${d_MCU_PROJECT_NAME}_audio_base_osal INTERFACE)
    add_library(src_${d_MCU_PROJECT_NAME}_audio_base_xip INTERFACE)
endif()
add_library(src_${d_MCU_PROJECT_NAME}_audio_config INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_control_local INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_control_passthrough INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_effect_core INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_effect_local INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_effect_sw_algorithm_bundle INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_effect_sw_algorithm_common INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_effect_sw_algorithm_eq INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_effect_sw_algorithm_wrapper INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_hal INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_hal_test INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_media_codec_common_pvmp3 INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_media_codec_common_amrnb INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_media_codec_common_amrwb INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_media_codec_simple INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_media_common INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_media_config INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_media_core_lite INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_media_core_standard INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_media_demux INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_media_libmedia_lite INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_media_libmedia_standard INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_media_source INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_media_test INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_media_utils INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_media_xip INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_stream_core INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_stream_local INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_stream_passthrough INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_stream_services INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_test INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_third_fdk_acc INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_third_flac INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_third_haac INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_third_opus INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_third_speexdsp INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_third_tremolo INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_third_gsm INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_xip_local INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_audio_xip_passthrough INTERFACE)


#--------------------------#
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_algorithm_plc p_SCOPE interface __RTOS__)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_algorithm_plc p_SCOPE interface
    $<$<COMPILE_LANGUAGE:C>:-Wno-multichar>
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-parameter>
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_algorithm_plc p_SCOPE interface
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-multichar>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-parameter>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-narrowing>
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_algorithm_plc p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/audio_algorithm/plc/vox_mem.c
    ${c_CMPT_AUDIO_DIR}/audio_algorithm/plc/vox_math.c
    ${c_CMPT_AUDIO_DIR}/audio_algorithm/plc/rt_plc.cpp
)
#--------------------------#
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_algorithm_sonic p_SCOPE interface __RTOS__)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_algorithm_sonic p_SCOPE interface
    $<$<COMPILE_LANGUAGE:C>:-Wno-multichar>
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-parameter>
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_algorithm_sonic p_SCOPE interface
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-multichar>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-parameter>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-narrowing>
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_algorithm_sonic p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/audio_algorithm/sonic/sonic.c
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_base_audio_utils p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/base/audio_utils/src/format.c
    ${c_CMPT_AUDIO_DIR}/base/audio_utils/src/primitives.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_base_audio_utils p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/base/log/interfaces
    ${c_CMPT_AUDIO_DIR}/base/audio_utils/include
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_base_audio_utils p_SCOPE interface __RTOS__)
#--------------------------#
ameba_target_set(src_${d_MCU_PROJECT_NAME}_audio_base_cutils_cmd p_SCOPE interface
    p_COMPILE_DEFINES
        __RTOS__
    p_COMPILE_OPTIONS
        $<$<COMPILE_LANGUAGE:C>:-Wno-multichar>
        $<$<COMPILE_LANGUAGE:C>:-Wno-pointer-sign>
    p_INCLUDES
        ${c_CMPT_AUDIO_DIR}/base/cutils/interfaces
        ${c_CMPT_AUDIO_DIR}/base/cutils/src/include
        ${c_CMPT_AUDIO_DIR}/base/log/interfaces
        ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
    p_SOURCES
        ${c_CMPT_AUDIO_DIR}/base/cutils/cmd/cmd_main.c
)
#--------------------------#
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_base_cutils_src p_SCOPE interface __RTOS__)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_base_cutils_src p_SCOPE interface
    $<$<COMPILE_LANGUAGE:C>:-Wno-multichar>
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-parameter>
    $<$<COMPILE_LANGUAGE:C>:-Wno-int-to-pointer-cast>
    $<$<COMPILE_LANGUAGE:C>:-Wno-pointer-to-int-cast>
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_base_cutils_src p_SCOPE interface
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-multichar>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-parameter>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-narrowing>
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_base_cutils_src p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/base/cutils/interfaces
    ${c_CMPT_AUDIO_DIR}/base/cutils/src/include
    ${c_CMPT_AUDIO_DIR}/base/log/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_base_cutils_src p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/base/cutils/src/rtrbtree.c
    ${c_CMPT_AUDIO_DIR}/base/cutils/src/rtintmap.c
    ${c_CMPT_AUDIO_DIR}/base/cutils/src/property.c
)
if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    if("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_base_cutils_src p_SCOPE interface
            ${c_CMPT_AUDIO_DIR}/base/cutils/src/parcel.c
            ${c_CMPT_AUDIO_DIR}/base/cutils/src/rtrbmap.c
            ${c_CMPT_AUDIO_DIR}/base/cutils/src/rtlist.c
            ${c_CMPT_AUDIO_DIR}/base/cutils/src/system_metrics.c
            ${c_CMPT_AUDIO_DIR}/base/cutils/src/system_properties.c
        )
    endif()
endif()
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_base_libs p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/base/libs/src/string.c
    ${c_CMPT_AUDIO_DIR}/base/libs/src/strcasecmp.c
    ${c_CMPT_AUDIO_DIR}/base/libs/src/strnlen.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_base_libs p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/base/libs/src/include
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_base_libs p_SCOPE interface __RTOS__)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_base_libs p_SCOPE interface
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-multichar>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-parameter>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-int-to-pointer-cast>
)
#--------------------------#
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_base_log p_SCOPE interface __RTOS__)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_base_log p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/base/log/log.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_base_log p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/base/log/interfaces
)
#--------------------------#
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_base_osal p_SCOPE interface __RTOS__)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_base_osal p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/adapter/freertos/osal_mem.c
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/adapter/freertos/osal_mutex.c
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/adapter/freertos/osal_sem.c
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/adapter/freertos/osal_condition.c
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/adapter/freertos/osal_thread.c
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/adapter/freertos/osal_time.c
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/adapter/freertos/osal_timer.c
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/adapter/freertos/osal_strdup.c
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/adapter/freertos/osal_vasprintf.c

    ${c_CMPT_AUDIO_DIR}/base/osal/osal_cxx/src/refptr.cpp
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_cxx/adapter/freertos/mutex.cpp
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_cxx/adapter/freertos/condition.cpp
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_cxx/adapter/freertos/threads.cpp
)

if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    if("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_base_osal p_SCOPE interface
            ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/adapter/freertos/osal_timer.c
        )
    endif()
endif()

ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_base_osal p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/base/log/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
    ${c_CMPT_AUDIO_DIR}/base/libs/src/include
    ${c_FREERTOS_DIR}/include
    ${c_FREERTOS_DIR}/portable/GCC/ARM_CA7
    ${c_CMPT_AUDIO_DIR}/base/log/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_cxx/interfaces
    ${c_CMPT_AUDIO_DIR}/base/libs/src/include
)
if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    if("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
        ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_base_osal p_SCOPE interface
            ${c_FREERTOS_DIR}/portable/GCC/ARM_CA7
        )
    elseif("${d_MCU_PROJECT_NAME}" STREQUAL "hp")
        ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_base_osal p_SCOPE interface
            ${c_FREERTOS_DIR}/portable/GCC/AmebaSmart_HP/non_secure
            ${c_FREERTOS_DIR}/portable/GCC/AmebaSmart_HP/secure
        )
    endif()
endif()
#--------------------------#

ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_config p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/configs/include
    ${c_CMPT_AUDIO_DIR}/interfaces
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_config p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/configs/ameba_audio_mixer_usrcfg.cpp
)

ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_config p_SCOPE interface
    -Wno-multichar
    -Wno-unused-parameter
    -std=gnu++17
    -std=c++17
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_config p_SCOPE interface __RTOS__)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_control_local p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/audio_control/local/audio_control.cpp
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_control_local p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/base/audio_utils/include
    ${c_CMPT_AUDIO_DIR}/base/cutils/interfaces
    ${c_CMPT_AUDIO_DIR}/base/log/interfaces
    ${c_CMPT_AUDIO_DIR}/base/libs/src/include
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_cxx/interfaces
    ${c_CMPT_AUDIO_DIR}/base/utils/interfaces
    ${c_CMPT_AUDIO_DIR}/interfaces
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_control_local p_SCOPE interface
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-multichar>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-parameter>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_control_local p_SCOPE interface __RTOS__)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_control_passthrough p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/audio_control/passthrough/audio_control.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_control_passthrough p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/base/log/interfaces
    ${c_CMPT_AUDIO_DIR}/base/libs/src/include
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
    ${c_CMPT_AUDIO_DIR}/interfaces
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_control_passthrough p_SCOPE interface -UDEBUG)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_control_passthrough p_SCOPE interface __RTOS__ NDEBUG)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_effect_core p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/audio_effect/core/effect_base.cpp
    ${c_CMPT_AUDIO_DIR}/audio_effect/core/equalizer_client.cpp
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_effect_core p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/base/audio_utils/include
    ${c_CMPT_AUDIO_DIR}/base/cutils/interfaces
    ${c_CMPT_AUDIO_DIR}/base/log/interfaces
    ${c_CMPT_AUDIO_DIR}/base/libs/src/include
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_cxx/interfaces
    ${c_CMPT_AUDIO_DIR}/audio_stream/services
    ${c_CMPT_AUDIO_DIR}/audio_algorithm/sonic
    ${c_CMPT_AUDIO_DIR}/interfaces
    ${c_CMPT_AUDIO_DIR}/audio_effect/include
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/wrapper/Bundle
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_effect_core p_SCOPE interface
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-multichar>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-parameter>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_effect_core p_SCOPE interface __RTOS__)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_effect_local p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/audio_effect/local/audio_equalizer.cpp
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_effect_local p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/base/audio_utils/include
    ${c_CMPT_AUDIO_DIR}/base/cutils/interfaces
    ${c_CMPT_AUDIO_DIR}/base/log/interfaces
    ${c_CMPT_AUDIO_DIR}/base/libs/src/include
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_cxx/interfaces
    ${c_CMPT_AUDIO_DIR}/audio_algorithm/sonic
    ${c_CMPT_AUDIO_DIR}/audio_stream/core
    ${c_CMPT_AUDIO_DIR}/audio_stream/services
    ${c_CMPT_AUDIO_DIR}/interfaces
    ${c_CMPT_AUDIO_DIR}/audio_effect/core
    ${c_CMPT_AUDIO_DIR}/audio_effect/include
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/wrapper/Bundle
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_effect_local p_SCOPE interface
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-multichar>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-parameter>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_effect_local p_SCOPE interface __RTOS__)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_effect_sw_algorithm_bundle p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Bundle/src/LVM_Buffers.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Bundle/src/LVM_Control.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Bundle/src/LVM_Init.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Bundle/src/LVM_Process.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Bundle/src/LVM_Tables.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Bundle/src/LVM_API_Specials.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_effect_sw_algorithm_bundle p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Bundle/include
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/include
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/StereoWidening/src
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/StereoWidening/include
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Eq/include
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Eq/src
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_effect_sw_algorithm_bundle p_SCOPE interface
    -UDEBUG
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_effect_sw_algorithm_bundle p_SCOPE interface __RTOS__ NDEBUG)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_effect_sw_algorithm_common p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/InstAlloc.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/DC_2I_D16_TRC_WRA_01.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/DC_2I_D16_TRC_WRA_01_Init.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/FO_2I_D16F32C15_LShx_TRC_WRA_01.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/FO_2I_D16F32Css_LShx_TRC_WRA_01_Init.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/FO_1I_D16F16C15_TRC_WRA_01.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/FO_1I_D16F16Css_TRC_WRA_01_Init.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/BP_1I_D16F32C30_TRC_WRA_01.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/BP_1I_D16F16C14_TRC_WRA_01.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/BP_1I_D32F32C30_TRC_WRA_02.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/BP_1I_D16F16Css_TRC_WRA_01_Init.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/BP_1I_D16F32Cll_TRC_WRA_01_Init.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/BP_1I_D32F32Cll_TRC_WRA_02_Init.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/BQ_2I_D32F32Cll_TRC_WRA_01_Init.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/BQ_2I_D32F32C30_TRC_WRA_01.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/BQ_2I_D16F32C15_TRC_WRA_01.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/BQ_2I_D16F32C14_TRC_WRA_01.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/BQ_2I_D16F32C13_TRC_WRA_01.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/BQ_2I_D16F32Css_TRC_WRA_01_init.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/BQ_2I_D16F16C15_TRC_WRA_01.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/BQ_2I_D16F16C14_TRC_WRA_01.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/BQ_2I_D16F16Css_TRC_WRA_01_Init.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/BQ_1I_D16F16C15_TRC_WRA_01.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/BQ_1I_D16F16Css_TRC_WRA_01_Init.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/BQ_1I_D16F32C14_TRC_WRA_01.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/BQ_1I_D16F32Css_TRC_WRA_01_init.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/PK_2I_D32F32C30G11_TRC_WRA_01.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/PK_2I_D32F32C14G11_TRC_WRA_01.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/PK_2I_D32F32CssGss_TRC_WRA_01_Init.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/PK_2I_D32F32CllGss_TRC_WRA_01_Init.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/Int16LShiftToInt32_16x32.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/From2iToMono_16.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/Copy_16.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/MonoTo2I_16.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/LoadConst_16.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/dB_to_Lin32.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/Shift_Sat_v16xv16.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/Abs_32.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/Int32RShiftToInt16_Sat_32x16.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/From2iToMono_32.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/mult3s_16x16.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/NonLinComp_D16.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/DelayMix_16x16.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/MSTo2i_Sat_16x16.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/From2iToMS_16x16.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/Mac3s_Sat_16x16.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/Add2_Sat_16x16.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/LVC_MixSoft_1St_2i_D16C31_SAT.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/LVC_MixSoft_1St_D16C31_SAT.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/LVC_Mixer_VarSlope_SetTimeConstant.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/LVC_Mixer_SetTimeConstant.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/LVC_Mixer_SetTarget.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/LVC_Mixer_GetTarget.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/LVC_Mixer_Init.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/LVC_Core_MixHard_1St_2i_D16C31_SAT.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/LVC_Core_MixSoft_1St_2i_D16C31_WRA.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/LVC_Core_MixInSoft_D16C31_SAT.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/LVC_Mixer_GetCurrent.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/LVC_MixSoft_2St_D16C31_SAT.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/LVC_Core_MixSoft_1St_D16C31_WRA.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/LVC_Core_MixHard_2St_D16C31_SAT.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/LVC_MixInSoft_D16C31_SAT.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/AGC_MIX_VOL_2St1Mon_D32_WRA.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/LVM_Timer.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src/LVM_Timer_Init.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_effect_sw_algorithm_common p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/include
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_effect_sw_algorithm_common p_SCOPE interface
    -UDEBUG
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_effect_sw_algorithm_common p_SCOPE interface NDEBUG)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_effect_sw_algorithm_eq p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Eq/src/LVEQNB_CalcCoef.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Eq/src/LVEQNB_Control.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Eq/src/LVEQNB_Tables.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Eq/src/LVEQNB_Init.c
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Eq/src/LVEQNB_Process.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_effect_sw_algorithm_eq p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Eq/include
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/include
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/src
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_effect_sw_algorithm_eq p_SCOPE interface
    -UDEBUG
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_effect_sw_algorithm_eq p_SCOPE interface __RTOS__ NDEBUG)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_effect_sw_algorithm_wrapper p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/wrapper/Bundle/EffectBundle.cpp
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_effect_sw_algorithm_wrapper p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/audio_effect/include
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Eq/include
    ${c_CMPT_AUDIO_DIR}/base/log/interfaces
    ${c_CMPT_AUDIO_DIR}/base/cutils/interfaces
    ${c_CMPT_AUDIO_DIR}/base/audio_utils/include
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Bundle/include
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/Common/include
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_effect_sw_algorithm_wrapper p_SCOPE interface
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-multichar>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-parameter>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_effect_sw_algorithm_wrapper p_SCOPE interface __RTOS__)
#--------------------------#

ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_hal p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/interfaces
    ${c_CMPT_AUDIO_DIR}/base/cutils/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
)
if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    if("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
        ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_hal p_SCOPE interface
            ${c_FREERTOS_DIR}/include
            ${c_FREERTOS_DIR}/portable/GCC/ARM_CA7
        )
    elseif("${d_MCU_PROJECT_NAME}" STREQUAL "hp")
        ameba_target_include_if(CONFIG_CLINTWOOD src_${d_MCU_PROJECT_NAME}_audio_hal p_SCOPE interface
            ${c_CMPT_APPLICATION_DIR}/clintwood/soc
        )
    endif()
endif()
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_hal p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/audio_hal/${d_PLATFORM_TYPE}/ameba_audio_stream_buffer.c
    ${c_CMPT_AUDIO_DIR}/audio_hal/${d_PLATFORM_TYPE}/ameba_audio_stream.c
    ${c_CMPT_AUDIO_DIR}/audio_hal/${d_PLATFORM_TYPE}/ameba_audio_stream_control.c
    ${c_CMPT_AUDIO_DIR}/audio_hal/${d_PLATFORM_TYPE}/ameba_audio_stream_utils.c
    ${c_CMPT_AUDIO_DIR}/audio_hal/${d_PLATFORM_TYPE}/ameba_audio_stream_render.c
    ${c_CMPT_AUDIO_DIR}/audio_hal/${d_PLATFORM_TYPE}/ameba_audio_stream_capture.c
    ${c_CMPT_AUDIO_DIR}/audio_hal/${d_PLATFORM_TYPE}/primary_audio_hw_stream_out.c
    ${c_CMPT_AUDIO_DIR}/audio_hal/${d_PLATFORM_TYPE}/primary_audio_hw_card.c
    ${c_CMPT_AUDIO_DIR}/audio_hal/${d_PLATFORM_TYPE}/primary_audio_hw_stream_in.c
    ${c_CMPT_AUDIO_DIR}/audio_hal/${d_PLATFORM_TYPE}/audio_hw_params_handle.c
    ${c_CMPT_AUDIO_DIR}/audio_hal/${d_PLATFORM_TYPE}/audio_hw_manager.c
    ${c_CMPT_AUDIO_DIR}/audio_hal/${d_PLATFORM_TYPE}/audio_hw_control.c
)

ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_hal p_SCOPE interface -UDEBUG -Wno-multichar)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_hal p_SCOPE interface __RTOS__ NDEBUG)

#--------------------------#

ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_hal_test p_SCOPE interface ${c_CMPT_AUDIO_DIR}/audio_hal/interfaces)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_hal_test p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/audio_hal/test/render/render_test.c
    ${c_CMPT_AUDIO_DIR}/audio_hal/test/capture/capture_test.c
)

ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_hal_test p_SCOPE interface -Wno-unused-parameter)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_hal_test p_SCOPE interface __RTOS__)

#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_media_codec_common_pvmp3 p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/include
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_codec_common_pvmp3 p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_normalize.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_alias_reduction.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_crc.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_decode_header.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_decode_huff_cw.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_getbits.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_dequantize_sample.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_framedecoder.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_get_main_data_size.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_get_side_info.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_get_scale_factors.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_mpeg2_get_scale_data.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_mpeg2_get_scale_factors.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_mpeg2_stereo_proc.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_huffman_decoding.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_huffman_parsing.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_tables.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_imdct_synth.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_mdct_6.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_dct_6.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_poly_phase_synthesis.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_equalizer.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_seek_synch.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_stereo_proc.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_reorder.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_polyphase_filter_window.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_mdct_18.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_dct_9.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src/pvmp3_dct_16.cpp
)

ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_media_codec_common_pvmp3 p_SCOPE interface
    -Wno-unused-parameter
    -Wno-multichar
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_codec_common_pvmp3 p_SCOPE interface __RTOS__)

#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_media_codec_common_amrnb p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/include
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src
    ${c_CMPT_AUDIO_DIR}/base/log/interfaces
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_codec_common_amrnb p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/add.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/az_lsp.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/bitno_tab.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/bitreorder_tab.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/bits2prm.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/c2_9pf_tab.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/copy.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/div_32.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/div_s.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/extract_h.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/extract_l.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/gains_tbl.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/gc_pred.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/gmed_n.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/grid_tbl.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/gray_tbl.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/int_lpc.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/inv_sqrt.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/inv_sqrt_tbl.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/l_abs.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/l_deposit_h.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/l_deposit_l.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/l_shr_r.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/log2.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/log2_norm.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/log2_tbl.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/lsfwt.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/lsp.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/lsp_az.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/lsp_lsf.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/lsp_lsf_tbl.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/lsp_tab.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/mult_r.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/norm_l.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/norm_s.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/ph_disp_tab.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/pow2.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/pow2_tbl.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/pred_lt.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/q_plsf.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/q_plsf_3.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/q_plsf_3_tbl.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/q_plsf_5.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/q_plsf_5_tbl.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/qua_gain_tbl.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/reorder.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/residu.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/round.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/set_zero.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/shr.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/shr_r.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/sqrt_l.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/sqrt_l_tbl.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/sub.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/syn_filt.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/vad1.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/weight_a.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/src/window_tab.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/a_refl.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/agc.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/amrdecode.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/b_cn_cod.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/bgnscd.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/c_g_aver.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/d1035pf.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/d2_11pf.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/d2_9pf.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/d3_14pf.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/d4_17pf.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/d8_31pf.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/d_gain_c.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/d_gain_p.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/d_plsf.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/d_plsf_3.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/d_plsf_5.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/dec_amr.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/dec_gain.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/dec_input_format_tab.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/dec_lag3.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/dec_lag6.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/dtx_dec.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/ec_gains.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/ex_ctrl.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/if2_to_ets.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/int_lsf.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/lsp_avg.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/ph_disp.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/post_pro.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/preemph.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/pstfilt.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/qgain475_tab.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/sp_dec.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src/wmf_to_ets.cpp
)

ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_media_codec_common_amrnb p_SCOPE interface
    -Wno-error
    -Wno-multichar
    -Wno-unused-parameter
    "-DOSCL_UNUSED_ARG(x)=(void)(x)"
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_codec_common_amrnb p_SCOPE interface __RTOS__ OSCL_IMPORT_REF= OSCL_EXPORT_REF=)

#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_media_codec_common_amrwb p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/include
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src
)

ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_codec_common_amrwb p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/agc2_amr_wb.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/band_pass_6k_7k.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/dec_acelp_2p_in_64.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/dec_acelp_4p_in_64.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/dec_alg_codebook.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/dec_gain2_amr_wb.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/deemphasis_32.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/dtx_decoder_amr_wb.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/get_amr_wb_bits.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/highpass_400hz_at_12k8.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/highpass_50hz_at_12k8.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/homing_amr_wb_dec.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/interpolate_isp.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/isf_extrapolation.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/isp_az.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/isp_isf.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/lagconceal.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/low_pass_filt_7k.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/median5.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/mime_io.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/noise_gen_amrwb.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/normalize_amr_wb.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/oversamp_12k8_to_16k.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/phase_dispersion.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/pit_shrp.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/pred_lt4.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/preemph_amrwb_dec.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/pvamrwb_math_op.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/pvamrwbdecoder.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/q_gain2_tab.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/qisf_ns.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/qisf_ns_tab.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/qpisf_2s.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/qpisf_2s_tab.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/scale_signal.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/synthesis_amr_wb.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/voice_factor.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/wb_syn_filt.cpp
    ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/src/weight_amrwb_lpc.cpp
)

ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_media_codec_common_amrwb p_SCOPE interface
    -Wno-error
    -Wno-multichar
    -Wno-unused-parameter
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_codec_common_amrwb p_SCOPE interface __RTOS__)

#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/media_codec/codec-omx/include
    ${c_CMPT_AUDIO_DIR}/media_codec/codec-omx/include/media/codec/omxil
)
if(CONFIG_MEDIA_PLAYER)
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface
        ${c_CMPT_AUDIO_DIR}/media_codec/codec-omx/omx/omx_core.cpp
        ${c_CMPT_AUDIO_DIR}/media_codec/codec-omx/omx/sw_omx_component.cpp
        ${c_CMPT_AUDIO_DIR}/media_codec/codec-omx/media_codec.cpp
        ${c_CMPT_AUDIO_DIR}/media_codec/codec-omx/codec.cpp
        ${c_CMPT_AUDIO_DIR}/media_codec/codec-omx/codec_buffer_tunnel.cpp
        ${c_CMPT_AUDIO_DIR}/media_codec/codec-omx/data_converter.cpp
        ${c_CMPT_AUDIO_DIR}/media_codec/codec-omx/omx/omx_buffer.cpp
        ${c_CMPT_AUDIO_DIR}/media_codec/codec-omx/omx/omx_utils.cpp
        ${c_CMPT_AUDIO_DIR}/media_codec/codec-omx/omx/omx_node.cpp
        ${c_CMPT_AUDIO_DIR}/media_codec/codec-omx/omx/omx.cpp
    )
    ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface MEDIA_PLAYER)
    if(CONFIG_MEDIA_CODEC_PCM)
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface
            ${c_CMPT_AUDIO_DIR}/media_codec/codec-omx/codecs/rawdec/sw_raw_dec.cpp
        )
        ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface MEDIA_CODEC_PCM)
    endif()

    if(CONFIG_MEDIA_CODEC_AAC)
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface
            ${c_CMPT_AUDIO_DIR}/media_codec/codec-omx/codecs/aacdec/drc_pres_mode_wrap.cpp
            ${c_CMPT_AUDIO_DIR}/media_codec/codec-omx/codecs/aacdec/sw_aac2_dec.cpp
        )
        ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface
            ${c_CMPT_AUDIO_DIR}/third_party/aac/libAACdec/include
            ${c_CMPT_AUDIO_DIR}/third_party/aac/libPCMutils/include
            ${c_CMPT_AUDIO_DIR}/third_party/aac/libFDK/include
            ${c_CMPT_AUDIO_DIR}/third_party/aac/libSYS/include
            ${c_CMPT_AUDIO_DIR}/third_party/aac/libMpegTPDec/include
            ${c_CMPT_AUDIO_DIR}/third_party/aac/libSBRdec/include
            ${c_CMPT_AUDIO_DIR}/third_party/aac/libArithCoding/include
            ${c_CMPT_AUDIO_DIR}/third_party/aac/libDRCdec/include
            ${c_CMPT_AUDIO_DIR}/third_party/aac/libSACdec/include
        )
        ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface MEDIA_CODEC_AAC)
    endif()

    if(CONFIG_MEDIA_CODEC_HAAC)
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface
            ${c_CMPT_AUDIO_DIR}/media_codec/codec-omx/codecs/helixaacdec/sw_helix_aac_dec.cpp
        )
        ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface
            ${c_CMPT_AUDIO_DIR}/third_party/haac
        )
        ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface MEDIA_CODEC_HAAC)
    endif()

    if(CONFIG_MEDIA_CODEC_MP3)
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface
            ${c_CMPT_AUDIO_DIR}/media_codec/codec-omx/codecs/mp3dec/sw_mp3_dec.cpp
        )
        ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface
            ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/include
            ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src
        )
        ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface MEDIA_CODEC_MP3)
    endif()

    if(CONFIG_MEDIA_CODEC_VORBIS)
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface
            ${c_CMPT_AUDIO_DIR}/media_codec/codec-omx/codecs/vorbisdec/sw_vorbis_dec.cpp
        )
        ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface
            ${c_CMPT_AUDIO_DIR}/third_party/tremolo
        )
        ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface MEDIA_CODEC_VORBIS)
    endif()

    if(CONFIG_MEDIA_CODEC_OPUS)
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface
            ${c_CMPT_AUDIO_DIR}/media_codec/codec-omx/codecs/opusdec/sw_opus_dec.cpp
        )
        ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface
            ${c_CMPT_AUDIO_DIR}/third_party/libopus/include
        )
        ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface MEDIA_CODEC_OPUS)
    endif()
    if(CONFIG_MEDIA_CODEC_AMR)
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface
            ${c_CMPT_AUDIO_DIR}/media_codec/codec-omx/codecs/amrdec/sw_amr_dec.cpp
        )
        ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface
            ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/common/include
            ${c_CMPT_AUDIO_DIR}/media_codec/common/amrnb/dec/src
            ${c_CMPT_AUDIO_DIR}/media_codec/common/amrwb/include
            ${c_CMPT_AUDIO_DIR}/audio/media_codec/common/amrwb/src
        )
        ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface MEDIA_CODEC_AMR)
    endif()
    if(CONFIG_MEDIA_CODEC_G711)
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface
            ${c_CMPT_AUDIO_DIR}/media_codec/codec-omx/codecs/g711dec/sw_g711_dec.cpp
        )
        ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface MEDIA_CODEC_G711)
    endif()
    if(CONFIG_MEDIA_CODEC_GSM)
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface
            ${c_CMPT_AUDIO_DIR}/media_codec/codec-omx/codecs/gsmdec/sw_gsm_dec.cpp
        )
        ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface
            ${c_CMPT_AUDIO_DIR}/third_party/libgsm/inc
        )
        ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface MEDIA_CODEC_GSM)
    endif()
endif()
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface
    -Wno-multichar
    -Wno-unused-parameter
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_codec_omx p_SCOPE interface __RTOS__)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_codec_simple p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/media_codec/codec-simple/sw_media_codec_factory.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_media_codec_simple p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/media_codec/codec-simple/include
    ${c_CMPT_AUDIO_DIR}/base/libs/src/include
    ${c_CMPT_AUDIO_DIR}/base/log/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_cxx/interfaces
    ${c_CMPT_AUDIO_DIR}/base/cutils/interfaces
    ${c_CMPT_AUDIO_DIR}/base/audio_utils/include
    ${c_CMPT_AUDIO_DIR}/configs/include
)

if(CONFIG_MEDIA_LITE_CODEC_PCM)
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_codec_simple p_SCOPE interface
        ${c_CMPT_AUDIO_DIR}/media_codec/codec-simple/codecs/rawdec/sw_raw_dec_lite.c
    )
    ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_codec_simple p_SCOPE interface MEDIA_LITE_CODEC_PCM)
endif()

if(CONFIG_MEDIA_LITE_CODEC_HAAC)
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_codec_simple p_SCOPE interface
        ${c_CMPT_AUDIO_DIR}/media_codec/codec-simple/codecs/helixaacdec/sw_helix_aac_dec_lite.c
    )
    ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_media_codec_simple p_SCOPE interface
        ${c_CMPT_AUDIO_DIR}/third_party/haac
    )
    ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_codec_simple p_SCOPE interface MEDIA_LITE_CODEC_HAAC)
endif()

if(CONFIG_MEDIA_LITE_CODEC_MP3)
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_codec_simple p_SCOPE interface
        ${c_CMPT_AUDIO_DIR}/media_codec/codec-simple/codecs/mp3dec/sw_mp3_dec_lite.c
    )
    ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_media_codec_simple p_SCOPE interface
        ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/include
        ${c_CMPT_AUDIO_DIR}/media_codec/common/pvmp3/src
    )
    ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_codec_simple p_SCOPE interface MEDIA_LITE_CODEC_MP3)
endif()

ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_media_codec_simple p_SCOPE interface
    -Wno-multichar
    -Wno-unused-parameter
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_codec_simple p_SCOPE interface
    __RTOS__
    MEDIA_LITE_PLAYER
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_common p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/media_common/media_track.cpp
    ${c_CMPT_AUDIO_DIR}/media_common/media_buffer_base.cpp
    ${c_CMPT_AUDIO_DIR}/media_common/media_buffer.cpp
    ${c_CMPT_AUDIO_DIR}/media_common/media_buffer_group.cpp
    ${c_CMPT_AUDIO_DIR}/media_common/codec_buffer_info.cpp
    ${c_CMPT_AUDIO_DIR}/media_common/media_codec_buffer.cpp
    ${c_CMPT_AUDIO_DIR}/media_common/media_defs.cpp
    ${c_CMPT_AUDIO_DIR}/media_common/meta_data_base.cpp
    ${c_CMPT_AUDIO_DIR}/media_common/meta_data.cpp
    ${c_CMPT_AUDIO_DIR}/media_common/packet_pool.cpp
    ${c_CMPT_AUDIO_DIR}/media_common/esds.cpp
    ${c_CMPT_AUDIO_DIR}/media_common/utils.cpp
    ${c_CMPT_AUDIO_DIR}/media_common/media_base64.cpp
    ${c_CMPT_AUDIO_DIR}/media_common/media_bit_reader.cpp
    ${c_CMPT_AUDIO_DIR}/media_common/clock_anchor.cpp
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_media_common p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/base/libs/src/include
    ${c_CMPT_AUDIO_DIR}/base/log/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_cxx/interfaces
    ${c_CMPT_AUDIO_DIR}/base/cutils/interfaces
    ${c_CMPT_AUDIO_DIR}/media_utils/include
    ${c_CMPT_AUDIO_DIR}/interfaces
    ${c_CMPT_AUDIO_DIR}/media_common/include
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_media_common p_SCOPE interface
    -Wno-multichar
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_media_common p_SCOPE interface
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-deprecated-copy>
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_common p_SCOPE interface
    __RTOS__
)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_config p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/configs/ameba_media_extractor_usrcfg.cpp
    ${c_CMPT_AUDIO_DIR}/configs/ameba_media_decoder_usrcfg.cpp
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_media_config p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/configs/include
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_media_config p_SCOPE interface
    -Wno-multichar
    -Wno-unused-parameter
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_media_common p_SCOPE interface
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-deprecated-copy>
    $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_config p_SCOPE interface
    __RTOS__
)
if(CONFIG_MEDIA_PLAYER)
    ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_config p_SCOPE interface MEDIA_PLAYER)
    # ameba_target_definitions_if(CONFIG_MEDIA_CODEC_HAAC src_${d_MCU_PROJECT_NAME}_audio_media_config p_SCOPE interface
    #     MEDIA_DEMUX_HELIC
    # )
    set(_tmp_media_demux_type_ AAC WAV MP3 AMR OGG FLAC MP4)
    set(_tmp_media_codec_type_ PCM MP3 AAC HAAC VORBIS OPUS AMR G711 GSM)
    set(_tmp_media_lite_demux_type_ AAC WAV MP3 MP4)
    set(_tmp_media_lite_codec_type_ PCM MP3 HAAC)
    foreach(t IN LISTS _tmp_media_demux_type_)
        ameba_target_definitions_if(CONFIG_MEDIA_DEMUX_${t} src_${d_MCU_PROJECT_NAME}_audio_media_config p_SCOPE interface
            MEDIA_DEMUX_${t}
        )
    endforeach()
    foreach(t IN LISTS _tmp_media_codec_type_)
        ameba_target_definitions_if(CONFIG_MEDIA_CODEC_${t} src_${d_MCU_PROJECT_NAME}_audio_media_config p_SCOPE interface
            MEDIA_CODEC_${t}
        )
    endforeach()
    ameba_target_definitions_if(CONFIG_MEDIA_LITE_PLAYER src_${d_MCU_PROJECT_NAME}_audio_media_config p_SCOPE interface
        MEDIA_LITE_PLAYER
    )
    foreach(t IN LISTS _tmp_media_lite_demux_type_)
        ameba_target_definitions_if(CONFIG_MEDIA_LITE_DEMUX_${t} src_${d_MCU_PROJECT_NAME}_audio_media_config p_SCOPE interface
            MEDIA_LITE_DEMUX_${t}
        )
    endforeach()
    foreach(t IN LISTS _tmp_media_lite_codec_type_)
        ameba_target_definitions_if(CONFIG_MEDIA_LITE_CODEC_${t} src_${d_MCU_PROJECT_NAME}_audio_media_config p_SCOPE interface
            MEDIA_LITE_CODEC_${t}
        )
    endforeach()
    unset(_tmp_media_demux_type_)
    unset(_tmp_media_codec_type_)
    unset(_tmp_media_lite_demux_type_)
    unset(_tmp_media_lite_codec_type_)
endif()
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_core_lite p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/media_core/lite/player_driver_lite.cpp
    ${c_CMPT_AUDIO_DIR}/media_core/lite/player_core_lite.cpp
    ${c_CMPT_AUDIO_DIR}/media_core/lite/player_core_decoder_lite.cpp
    ${c_CMPT_AUDIO_DIR}/media_core/lite/player_core_renderer_lite.cpp
    ${c_CMPT_AUDIO_DIR}/media_core/lite/generic_source_lite.cpp
    ${c_CMPT_AUDIO_DIR}/media_core/lite/audio_output_lite.cpp
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_media_core_lite p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/base/libs/src/include
    ${c_CMPT_AUDIO_DIR}/base/log/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_cxx/interfaces
    ${c_CMPT_AUDIO_DIR}/base/cutils/interfaces
    ${c_CMPT_AUDIO_DIR}/base/audio_utils/include
    ${c_CMPT_AUDIO_DIR}/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces/osal_c
    ${c_CMPT_AUDIO_DIR}/media_utils/include
    ${c_CMPT_AUDIO_DIR}/media_common/include
    ${c_CMPT_AUDIO_DIR}/media_source/include
    ${c_CMPT_AUDIO_DIR}/media_demux/include
    ${c_CMPT_AUDIO_DIR}/media_codec/codec-simple/include
    ${c_CMPT_AUDIO_DIR}/media_core/lite/include
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_media_core_lite p_SCOPE interface
    -Wno-multichar
    -Wno-unused-parameter
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_core_lite p_SCOPE interface
    __RTOS__
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_core_standard p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/media_core/standard/player_driver.cpp
    ${c_CMPT_AUDIO_DIR}/media_core/standard/player_core.cpp
    ${c_CMPT_AUDIO_DIR}/media_core/standard/player_core_decoder_base.cpp
    ${c_CMPT_AUDIO_DIR}/media_core/standard/player_core_decoder.cpp
    ${c_CMPT_AUDIO_DIR}/media_core/standard/player_core_renderer.cpp
    ${c_CMPT_AUDIO_DIR}/media_core/standard/generic_source.cpp
    ${c_CMPT_AUDIO_DIR}/media_core/standard/audio_output.cpp
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_media_core_standard p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/base/libs/src/include
    ${c_CMPT_AUDIO_DIR}/base/log/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_cxx/interfaces
    ${c_CMPT_AUDIO_DIR}/base/cutils/interfaces
    ${c_CMPT_AUDIO_DIR}/base/audio_utils/include
    ${c_CMPT_AUDIO_DIR}/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces/osal_c
    ${c_CMPT_AUDIO_DIR}/media_utils/include
    ${c_CMPT_AUDIO_DIR}/media_common/include
    ${c_CMPT_AUDIO_DIR}/media_source/include
    ${c_CMPT_AUDIO_DIR}/media_demux/include
    ${c_CMPT_AUDIO_DIR}/media_codec/codec-omx/include
    ${c_CMPT_AUDIO_DIR}/media_core/standard/include
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_media_core_standard p_SCOPE interface
    -Wno-multichar
    -Wno-unused-parameter
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_core_standard p_SCOPE interface
    __RTOS__
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_demux p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/media_demux/media_extractor.cpp
    ${c_CMPT_AUDIO_DIR}/media_demux/media_extractor_factory.cpp
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_media_demux p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/base/libs/src/include
    ${c_CMPT_AUDIO_DIR}/base/log/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_cxx/interfaces
    ${c_CMPT_AUDIO_DIR}/base/cutils/interfaces
    ${c_CMPT_AUDIO_DIR}/base/audio_utils/include
    ${c_CMPT_AUDIO_DIR}/media_utils/include
    ${c_CMPT_AUDIO_DIR}/media_common/include
    ${c_CMPT_AUDIO_DIR}/media_source/include
    ${c_CMPT_AUDIO_DIR}/configs/include
    ${c_CMPT_AUDIO_DIR}/media_demux/include
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_media_demux p_SCOPE interface
    -Wno-multichar
    -Wno-unused-parameter
    -Wno-multichar
    -Wno-unused-value
    -Wno-unused-variable
    -Wno-unused-but-set-variable
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_demux p_SCOPE interface
    __RTOS__
)

ameba_target_definitions_if(CONFIG_MEDIA_PLAYER src_${d_MCU_PROJECT_NAME}_audio_media_demux p_SCOPE interface
    MEDIA_PLAYER
)

ameba_target_definitions_if(CONFIG_MEDIA_LITE_PLAYER src_${d_MCU_PROJECT_NAME}_audio_media_demux p_SCOPE interface
    MEDIA_LITE_PLAYER
)
if(CONFIG_MEDIA_CODEC_HAAC OR CONFIG_MEDIA_LITE_CODEC_HAAC)
    ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_demux p_SCOPE interface
        MEDIA_DEMUX_AAC_DELIVER_ADTS_HEADER
    )
endif()
if(CONFIG_MEDIA_DEMUX_AAC OR CONFIG_MEDIA_LITE_DEMUX_AAC)
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_demux p_SCOPE interface
        ${c_CMPT_AUDIO_DIR}/media_demux/extractors/aac/aac_extractor.cpp
    )
    ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_demux p_SCOPE interface
        MEDIA_DEMUX_AAC
    )
endif()
if(CONFIG_MEDIA_DEMUX_WAV OR CONFIG_MEDIA_LITE_DEMUX_WAV)
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_demux p_SCOPE interface
        ${c_CMPT_AUDIO_DIR}/media_demux/extractors/wav/wav_extractor.cpp
    )
    ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_demux p_SCOPE interface
        MEDIA_DEMUX_WAV
    )
endif()
if(CONFIG_MEDIA_DEMUX_MP3 OR CONFIG_MEDIA_LITE_DEMUX_MP3)
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_demux p_SCOPE interface
        ${c_CMPT_AUDIO_DIR}/media_demux/extractors/mp3/mp3_extractor.cpp
        ${c_CMPT_AUDIO_DIR}/media_demux/extractors/mp3/mp3_vbri_seeker.cpp
        ${c_CMPT_AUDIO_DIR}/media_demux/extractors/mp3/mpr_xing_seeker.cpp
        ${c_CMPT_AUDIO_DIR}/media_demux/extractors/mp3/mp3_id3.cpp
    )
    ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_demux p_SCOPE interface
        MEDIA_DEMUX_MP3
    )
endif()
if(CONFIG_MEDIA_DEMUX_AMR OR CONFIG_MEDIA_LITE_DEMUX_AMR)
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_demux p_SCOPE interface
        ${c_CMPT_AUDIO_DIR}/media_demux/extractors/amr/amr_extractor.cpp
    )
    ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_demux p_SCOPE interface
        MEDIA_DEMUX_AMR
    )
endif()
if(CONFIG_MEDIA_DEMUX_OGG OR CONFIG_MEDIA_LITE_DEMUX_OGG)
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_demux p_SCOPE interface
        ${c_CMPT_AUDIO_DIR}/media_demux/extractors/ogg/ogg_extractor.cpp
    )
    ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_media_demux p_SCOPE interface
        ${c_CMPT_AUDIO_DIR}/third_party/tremolo
    )
    ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_demux p_SCOPE interface
        MEDIA_DEMUX_OGG
    )
endif()
if(CONFIG_MEDIA_DEMUX_FLAC OR CONFIG_MEDIA_LITE_DEMUX_FLAC)
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_demux p_SCOPE interface
        ${c_CMPT_AUDIO_DIR}/media_demux/extractors/flac/flac_extractor.cpp
    )
    ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_media_demux p_SCOPE interface
        ${c_CMPT_AUDIO_DIR}/third_party/flac/include
    )
    ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_demux p_SCOPE interface
        MEDIA_DEMUX_FLAC
    )
endif()
if(CONFIG_MEDIA_DEMUX_MP4 OR CONFIG_MEDIA_LITE_DEMUX_MP4)
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_demux p_SCOPE interface
        ${c_CMPT_AUDIO_DIR}/media_demux/extractors/mp4/mpeg4_extractor.cpp
        ${c_CMPT_AUDIO_DIR}/media_demux/extractors/mp4/sample_table.cpp
        ${c_CMPT_AUDIO_DIR}/media_demux/extractors/mp4/sample_iterator.cpp
    )
    ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_demux p_SCOPE interface
        MEDIA_DEMUX_MP4
    )
endif()
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_libmedia_lite p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/media_libmedia/lite/rtplayer_lite.cpp
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_media_libmedia_lite p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/base/cutils/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_cxx/interfaces
    ${c_CMPT_AUDIO_DIR}/base/log/interfaces
    ${c_CMPT_AUDIO_DIR}/interfaces
    ${c_CMPT_AUDIO_DIR}/media_utils/include
    ${c_CMPT_AUDIO_DIR}/media_common/include
    ${c_CMPT_AUDIO_DIR}/media_core/lite/include
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_media_libmedia_lite p_SCOPE interface
    -Wno-multichar
    -Wno-unused-parameter
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_libmedia_lite p_SCOPE interface
    __RTOS__
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_libmedia_standard p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/media_libmedia/standard/rtplayer.cpp
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_media_libmedia_standard p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/base/cutils/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_cxx/interfaces
    ${c_CMPT_AUDIO_DIR}/base/log/interfaces
    ${c_CMPT_AUDIO_DIR}/interfaces
    ${c_CMPT_AUDIO_DIR}/media_utils/include
    ${c_CMPT_AUDIO_DIR}/media_common/include
    ${c_CMPT_AUDIO_DIR}/media_core/standard/include
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_media_libmedia_standard p_SCOPE interface
    -Wno-multichar
    -Wno-unused-parameter
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_libmedia_standard p_SCOPE interface
    __RTOS__
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_source p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/media_source/src/data_source_base.cpp
    ${c_CMPT_AUDIO_DIR}/media_source/src/data_source_factory.cpp
    ${c_CMPT_AUDIO_DIR}/media_source/src/file_source.cpp
    ${c_CMPT_AUDIO_DIR}/media_source/src/buffer_source.cpp
    ${c_CMPT_AUDIO_DIR}/media_source/src/cdata_source.cpp
    ${c_CMPT_AUDIO_DIR}/media_source/src/http/http_source.cpp
    ${c_CMPT_AUDIO_DIR}/media_source/src/http/http_tls.cpp
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_media_source p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/base/libs/src/include
    ${c_CMPT_AUDIO_DIR}/base/log/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_cxx/interfaces
    ${c_CMPT_AUDIO_DIR}/interfaces
    ${c_CMPT_AUDIO_DIR}/media_utils/include
    ${c_CMPT_AUDIO_DIR}/media_common/include
    ${c_CMPT_AUDIO_DIR}/configs/include
    ${c_CMPT_AUDIO_DIR}/media_source/include
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_media_source p_SCOPE interface
    -Wno-multichar
    -Wno-unused-parameter
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_source p_SCOPE interface
    __RTOS__
    CONFIG_MEDIA_DATASOURCE_BUFFER
    CONFIG_MEDIA_DATASOURCE_HTTP
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_test p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/media_test/app_example.c
    # ${c_CMPT_AUDIO_DIR}/media_test/threads_test.c
    ${c_CMPT_AUDIO_DIR}/media_test/socket_test.c
    # ${c_CMPT_AUDIO_DIR}/media_test/test_thread.cpp
    ${c_CMPT_AUDIO_DIR}/media_test/handler_looper_test.cpp
    ${c_CMPT_AUDIO_DIR}/media_test/message_test.cpp

    ${c_CMPT_AUDIO_DIR}/media_test/system_metrics_test.c
    ${c_CMPT_AUDIO_DIR}/media_test/system_properties_test.c
)
ameba_target_sources_if(CONFIG_MEDIA_PLAYER src_${d_MCU_PROJECT_NAME}_audio_media_test p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/media_test/stardard/player_test.c
    ${c_CMPT_AUDIO_DIR}/media_test/stardard/player_looper_test.c
    ${c_CMPT_AUDIO_DIR}/media_test/stardard/mydata_source/mydata_source.c
    ${c_CMPT_AUDIO_DIR}/media_test/stardard/mydata_source/mydata_source_test.c
    ${c_CMPT_AUDIO_DIR}/media_test/stardard/meidi/mcrc_player_adapter.c
    ${c_CMPT_AUDIO_DIR}/media_test/stardard/meidi/mcrc_player_looper_test.c
)
ameba_target_sources_if(CONFIG_MEDIA_PLAYER src_${d_MCU_PROJECT_NAME}_audio_media_test p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/media_test/lite/player_test_lite.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_media_test p_SCOPE interface
    ${c_CMPT_FILE_SYSTEM_DIR}/fatfs
    ${c_CMPT_FILE_SYSTEM_DIR}/fatfs/r0.14b/include
    ${c_CMPT_AUDIO_DIR}/base/libs/src/include
    ${c_CMPT_AUDIO_DIR}/base/log/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_cxx/interfaces
    ${c_CMPT_AUDIO_DIR}/base/cutils/interfaces
    ${c_CMPT_AUDIO_DIR}/base/cutils/src/include
    ${c_CMPT_AUDIO_DIR}/media_utils/include
    ${c_CMPT_AUDIO_DIR}/interfaces
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_media_test p_SCOPE interface
    -Wno-multichar
    -Wno-incompatible-pointer-types
    -Wno-int-conversion
    -Wno-pointer-sign
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_test p_SCOPE interface
    __RTOS__
)
ameba_target_definitions_if(CONFIG_MEDIA_PLAYER src_${d_MCU_PROJECT_NAME}_audio_media_test p_SCOPE interface
    MEDIA_PLAYER
)
ameba_target_definitions_if(CONFIG_MEDIA_LITE_PLAYER src_${d_MCU_PROJECT_NAME}_audio_media_test p_SCOPE interface
    MEDIA_PLAYER
)
#--------------------------#]
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_media_utils p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/media_utils/rstring.cpp
    ${c_CMPT_AUDIO_DIR}/media_utils/byte_utils.cpp
    ${c_CMPT_AUDIO_DIR}/media_utils/hexdump.cpp
    ${c_CMPT_AUDIO_DIR}/media_utils/rbuffer.cpp
    ${c_CMPT_AUDIO_DIR}/media_utils/message_handler.cpp
    ${c_CMPT_AUDIO_DIR}/media_utils/message_looper.cpp
    ${c_CMPT_AUDIO_DIR}/media_utils/message_looper_manager.cpp
    ${c_CMPT_AUDIO_DIR}/media_utils/message.cpp
    ${c_CMPT_AUDIO_DIR}/media_utils/rstate_machine.cpp
    ${c_CMPT_AUDIO_DIR}/media_utils/unicode.cpp
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_media_utils p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/base/libs/src/include
    ${c_CMPT_AUDIO_DIR}/base/log/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_cxx/interfaces
    ${c_CMPT_AUDIO_DIR}/media_utils/include
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_media_utils p_SCOPE interface
    -Wno-multichar
    -Wno-unused-parameter
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_media_utils p_SCOPE interface
    __RTOS__
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_stream_core p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/audio_stream/core/track_client.cpp
    ${c_CMPT_AUDIO_DIR}/audio_stream/core/record_client.cpp
    ${c_CMPT_AUDIO_DIR}/audio_stream/core/audio_system.cpp
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_stream_core p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/base/audio_utils/include
    ${c_CMPT_AUDIO_DIR}/base/cutils/interfaces
    ${c_CMPT_AUDIO_DIR}/base/log/interfaces
    ${c_CMPT_AUDIO_DIR}/base/libs/src/include
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_cxx/interfaces
    ${c_CMPT_AUDIO_DIR}/audio_stream/services
    ${c_CMPT_AUDIO_DIR}/audio_algorithm/sonic
    ${c_CMPT_AUDIO_DIR}/interfaces
    ${c_CMPT_AUDIO_DIR}/audio_effect/include
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/wrapper/Bundle
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_stream_core p_SCOPE interface
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-multichar>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-parameter>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_stream_core p_SCOPE interface __RTOS__)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_stream_local p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/audio_stream/local/audio_track.cpp
    ${c_CMPT_AUDIO_DIR}/audio_stream/local/audio_record.cpp
    ${c_CMPT_AUDIO_DIR}/audio_stream/local/audio_service.cpp
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_stream_local p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/base/audio_utils/include
    ${c_CMPT_AUDIO_DIR}/base/cutils/interfaces
    ${c_CMPT_AUDIO_DIR}/base/log/interfaces
    ${c_CMPT_AUDIO_DIR}/base/libs/src/include
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_cxx/interfaces
    ${c_CMPT_AUDIO_DIR}/audio_algorithm/sonic
    ${c_CMPT_AUDIO_DIR}/audio_stream/core
    ${c_CMPT_AUDIO_DIR}/audio_stream/services
    ${c_CMPT_AUDIO_DIR}/interfaces
    ${c_CMPT_AUDIO_DIR}/audio_effect/include
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/wrapper/Bundle
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_stream_local p_SCOPE interface
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-multichar>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-parameter>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_stream_local p_SCOPE interface __RTOS__)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_stream_passthrough p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/audio_stream/passthrough/audio_track.c
    ${c_CMPT_AUDIO_DIR}/audio_stream/passthrough/audio_record.c
    ${c_CMPT_AUDIO_DIR}/audio_stream/passthrough/audio_service.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_stream_passthrough p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/base/log/interfaces
    ${c_CMPT_AUDIO_DIR}/base/audio_utils/include
    ${c_CMPT_AUDIO_DIR}/base/libs/src/include
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
    ${c_CMPT_AUDIO_DIR}/interfaces
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_stream_passthrough p_SCOPE interface
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-multichar>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-parameter>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_stream_passthrough p_SCOPE interface -UDEBUG)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_stream_passthrough p_SCOPE interface __RTOS__ NDEBUG)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_stream_services p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/audio_stream/services/track_shared.cpp
    ${c_CMPT_AUDIO_DIR}/audio_stream/services/buffer_agents.cpp
    ${c_CMPT_AUDIO_DIR}/audio_stream/services/raw_resample_coef.cpp
    ${c_CMPT_AUDIO_DIR}/audio_stream/services/linear_raw_resampler.cpp
    ${c_CMPT_AUDIO_DIR}/audio_stream/services/sinc_raw_resampler.cpp
    ${c_CMPT_AUDIO_DIR}/audio_stream/services/raw_resampler.cpp
    ${c_CMPT_AUDIO_DIR}/audio_stream/services/raw_mixer.cpp
    ${c_CMPT_AUDIO_DIR}/audio_stream/services/general_tracks.cpp
    ${c_CMPT_AUDIO_DIR}/audio_stream/services/effects.cpp
    ${c_CMPT_AUDIO_DIR}/audio_stream/services/streaming_threads.cpp
    ${c_CMPT_AUDIO_DIR}/audio_stream/services/general_audio.cpp
    ${c_CMPT_AUDIO_DIR}/audio_stream/services/audio_stream_render.cpp
    # ${c_CMPT_AUDIO_DIR}/audio_stream/services/audio_stream_capture.cpp
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_stream_services p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/base/audio_utils/include
    ${c_CMPT_AUDIO_DIR}/base/cutils/interfaces
    ${c_CMPT_AUDIO_DIR}/base/log/interfaces
    ${c_CMPT_AUDIO_DIR}/base/libs/src/include
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_cxx/interfaces
    ${c_CMPT_AUDIO_DIR}/audio_algorithm/sonic
    ${c_CMPT_AUDIO_DIR}/interfaces
    ${c_CMPT_AUDIO_DIR}/audio_effect/include
    ${c_CMPT_AUDIO_DIR}/audio_effect/sw_algorithm/wrapper/Bundle
    ${c_CMPT_AUDIO_DIR}/configs/include
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_stream_services p_SCOPE interface
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-multichar>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-parameter>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-value>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-aligned-new>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_stream_services p_SCOPE interface
    __RTOS__
    # CONFIG_AUDIO_MIXER_RECORD
)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_test p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/audio_test/audio_test.c
    ${c_CMPT_AUDIO_DIR}/audio_test/audio_buffer.c
    ${c_CMPT_AUDIO_DIR}/audio_test/audio_plc_test.c
    ${c_CMPT_AUDIO_DIR}/audio_test/audio_track_test.c
    ${c_CMPT_AUDIO_DIR}/audio_test/audio_record_test.c
    ${c_CMPT_AUDIO_DIR}/audio_test/audio_control_test.c
    ${c_CMPT_AUDIO_DIR}/audio_test/audio_test_utils.c
    ${c_CMPT_AUDIO_DIR}/audio_test/audio_wav_test.cpp
    ${c_CMPT_AUDIO_DIR}/audio_test/audio_wav_test_thread.c
)
if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    if("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_test p_SCOPE interface
            ${c_CMPT_AUDIO_DIR}/audio_test/audio_buffer.c
            ${c_CMPT_AUDIO_DIR}/audio_test/audio_plc_test.c
        )
    endif()
endif()

ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_test p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/base/audio_utils/include
    ${c_CMPT_AUDIO_DIR}/base/cutils/interfaces
    ${c_CMPT_AUDIO_DIR}/base/log/interfaces
    ${c_CMPT_AUDIO_DIR}/base/libs/src/include
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_cxx/interfaces
    ${c_CMPT_AUDIO_DIR}/interfaces
)

if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    if("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
        ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_test p_SCOPE interface
            ${c_CMPT_AUDIO_DIR}/audio_algorithm/plc
        )
    elseif("${d_MCU_PROJECT_NAME}" STREQUAL "hp")
        ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_test p_SCOPE interface
            ${c_CMPT_AUDIO_DIR}/audio_algorithm/sonic
        )
    endif()
endif()

ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_test p_SCOPE interface
    -Wno-multichar
    -Wno-unused-parameter
    -Wno-narrowing
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_test p_SCOPE interface __RTOS__)
#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_third_fdk_acc p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/include
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libPCMutils/include
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libFDK/include
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSYS/include
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libMpegTPDec/include
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSBRdec/include
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libArithCoding/include
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libDRCdec/include
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSACdec/include
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_third_fdk_acc p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/src/aacdec_drc.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/src/aacdec_hcr_bit.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/src/aacdec_hcr.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/src/aacdec_hcrs.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/src/aacdecoder.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/src/aacdecoder_lib.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/src/aacdec_pns.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/src/aacdec_tns.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/src/aac_ram.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/src/aac_rom.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/src/block.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/src/channel.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/src/channelinfo.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/src/conceal.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/src/FDK_delay.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/src/ldfiltbank.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/src/pulsedata.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/src/rvlcbit.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/src/rvlcconceal.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/src/rvlc.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/src/stereo.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/src/usacdec_ace_d4t64.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/src/usacdec_acelp.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/src/usacdec_ace_ltp.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/src/usacdec_fac.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/src/usacdec_lpc.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/src/usacdec_lpd.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libAACdec/src/usacdec_rom.cpp

    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libPCMutils/src/limiter.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libPCMutils/src/pcm_utils.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libPCMutils/src/pcmdmx_lib.cpp

    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libFDK/src/FDK_lpc.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libFDK/src/fixpoint_math.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libFDK/src/FDK_matrixCalloc.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libFDK/src/qmf.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libFDK/src/FDK_tools_rom.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libFDK/src/FDK_qmf_domain.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libFDK/src/FDK_decorrelate.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libFDK/src/mdct.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libFDK/src/autocorr2nd.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libFDK/src/FDK_bitbuffer.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libFDK/src/fft_rad2.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libFDK/src/nlc_dec.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libFDK/src/FDK_crc.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libFDK/src/fft.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libFDK/src/FDK_trigFcts.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libFDK/src/scale.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libFDK/src/FDK_hybrid.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libFDK/src/huff_nodes.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libFDK/src/FDK_core.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libFDK/src/dct.cpp

    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSYS/src/syslib_channelMapDescr.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSYS/src/genericStds.cpp

    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libMpegTPDec/src/tpdec_adts.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libMpegTPDec/src/tpdec_lib.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libMpegTPDec/src/tpdec_adif.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libMpegTPDec/src/tpdec_drm.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libMpegTPDec/src/tpdec_latm.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libMpegTPDec/src/tpdec_asc.cpp

    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libArithCoding/src/ac_arith_coder.cpp

    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libDRCdec/src/drcDec_tools.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libDRCdec/src/drcGainDec_init.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libDRCdec/src/drcDec_reader.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libDRCdec/src/drcDec_rom.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libDRCdec/src/drcDec_selectionProcess.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libDRCdec/src/drcGainDec_process.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libDRCdec/src/FDK_drcDecLib.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libDRCdec/src/drcDec_gainDecoder.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libDRCdec/src/drcGainDec_preprocess.cpp


    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSBRdec/src/pvc_dec.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSBRdec/src/psbitdec.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSBRdec/src/sbr_ram.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSBRdec/src/env_extr.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSBRdec/src/sbrdec_drc.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSBRdec/src/huff_dec.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSBRdec/src/sbrdecoder.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSBRdec/src/env_calc.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSBRdec/src/env_dec.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSBRdec/src/lpp_tran.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSBRdec/src/psdec_drm.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSBRdec/src/sbr_dec.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSBRdec/src/sbr_crc.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSBRdec/src/HFgen_preFlat.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSBRdec/src/psdecrom_drm.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSBRdec/src/sbr_deb.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSBRdec/src/hbe.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSBRdec/src/psdec.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSBRdec/src/sbrdec_freq_sca.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSBRdec/src/sbr_rom.cpp

    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSACdec/src/sac_tsd.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSACdec/src/sac_process.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSACdec/src/sac_calcM1andM2.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSACdec/src/sac_dec_conceal.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSACdec/src/sac_stp.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSACdec/src/sac_dec.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSACdec/src/sac_qmf.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSACdec/src/sac_rom.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSACdec/src/sac_bitdec.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSACdec/src/sac_reshapeBBEnv.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSACdec/src/sac_smoothing.cpp
    ${c_CMPT_AUDIO_DIR}/third_party/fdk-aac/libSACdec/src/sac_dec_lib.cpp
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_third_fdk_acc p_SCOPE interface
    __RTOS__
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_third_fdk_acc p_SCOPE interface
    -Wno-error
)
# #--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_third_flac p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/third_party/flac
    ${c_CMPT_AUDIO_DIR}/third_party/flac/include
    ${c_CMPT_AUDIO_DIR}/third_party/flac/src/libFLAC/include
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_third_flac p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/third_party/flac/src/libFLAC/bitmath.c
    ${c_CMPT_AUDIO_DIR}/third_party/flac/src/libFLAC/bitreader.c
    ${c_CMPT_AUDIO_DIR}/third_party/flac/src/libFLAC/bitwriter.c
    ${c_CMPT_AUDIO_DIR}/third_party/flac/src/libFLAC/cpu.c
    ${c_CMPT_AUDIO_DIR}/third_party/flac/src/libFLAC/crc.c
    ${c_CMPT_AUDIO_DIR}/third_party/flac/src/libFLAC/fixed.c
    ${c_CMPT_AUDIO_DIR}/third_party/flac/src/libFLAC/float.c
    ${c_CMPT_AUDIO_DIR}/third_party/flac/src/libFLAC/format.c
    ${c_CMPT_AUDIO_DIR}/third_party/flac/src/libFLAC/lpc.c
    ${c_CMPT_AUDIO_DIR}/third_party/flac/src/libFLAC/memory.c
    ${c_CMPT_AUDIO_DIR}/third_party/flac/src/libFLAC/md5.c
    ${c_CMPT_AUDIO_DIR}/third_party/flac/src/libFLAC/stream_decoder.c
    ${c_CMPT_AUDIO_DIR}/third_party/flac/src/libFLAC/stream_encoder.c
    ${c_CMPT_AUDIO_DIR}/third_party/flac/src/libFLAC/stream_encoder_framing.c
    ${c_CMPT_AUDIO_DIR}/third_party/flac/src/libFLAC/window.c
    ${c_CMPT_AUDIO_DIR}/third_party/flac/src/libFLAC/metadata_object.c
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_third_flac p_SCOPE interface
    __RTOS__
    HAVE_CONFIG_H
    FLAC__INTEGER_ONLY_LIBRARY
    FLAC__HAS_OGG=0
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_third_flac p_SCOPE interface
    -Wno-undef
    -Wno-format
    -Wno-implicit-fallthrough
    -Wno-discarded-qualifiers
    -Wno-incompatible-pointer-types
)
#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_third_haac p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/third_party/haac
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_third_haac p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/third_party/haac
    ${c_CMPT_AUDIO_DIR}/third_party/haac/trigtabs_fltgen.c
    ${c_CMPT_AUDIO_DIR}/third_party/haac/trigtabs.c
    ${c_CMPT_AUDIO_DIR}/third_party/haac/tns.c
    ${c_CMPT_AUDIO_DIR}/third_party/haac/stproc.c
    ${c_CMPT_AUDIO_DIR}/third_party/haac/sbrtabs.c
    ${c_CMPT_AUDIO_DIR}/third_party/haac/sbrside.c
    ${c_CMPT_AUDIO_DIR}/third_party/haac/sbrqmf.c
    ${c_CMPT_AUDIO_DIR}/third_party/haac/sbrmath.c
    ${c_CMPT_AUDIO_DIR}/third_party/haac/sbrimdct.c
    ${c_CMPT_AUDIO_DIR}/third_party/haac/sbrhuff.c
    ${c_CMPT_AUDIO_DIR}/third_party/haac/sbrhfgen.c
    ${c_CMPT_AUDIO_DIR}/third_party/haac/sbrhfadj.c
    ${c_CMPT_AUDIO_DIR}/third_party/haac/sbrfreq.c
    ${c_CMPT_AUDIO_DIR}/third_party/haac/sbrfft.c
    ${c_CMPT_AUDIO_DIR}/third_party/haac/sbr.c
    ${c_CMPT_AUDIO_DIR}/third_party/haac/pns.c
    ${c_CMPT_AUDIO_DIR}/third_party/haac/noiseless.c
    ${c_CMPT_AUDIO_DIR}/third_party/haac/imdct.c
    ${c_CMPT_AUDIO_DIR}/third_party/haac/hufftabs.c
    ${c_CMPT_AUDIO_DIR}/third_party/haac/huffman.c
    ${c_CMPT_AUDIO_DIR}/third_party/haac/filefmt.c
    ${c_CMPT_AUDIO_DIR}/third_party/haac/fft.c
    ${c_CMPT_AUDIO_DIR}/third_party/haac/dequant.c
    ${c_CMPT_AUDIO_DIR}/third_party/haac/decelmnt.c
    ${c_CMPT_AUDIO_DIR}/third_party/haac/dct4.c
    ${c_CMPT_AUDIO_DIR}/third_party/haac/buffers.c
    ${c_CMPT_AUDIO_DIR}/third_party/haac/bitstream.c
    ${c_CMPT_AUDIO_DIR}/third_party/haac/aactabs.c
    ${c_CMPT_AUDIO_DIR}/third_party/haac/aacdec.c
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_third_haac p_SCOPE interface
    __RTOS__
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_third_haac p_SCOPE interface
    -Wno-error
)
#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_third_opus p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/third_party/libopus
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/celt
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/fixed
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/src
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/include
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/win32
)

ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_third_opus p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/celt/bands.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/celt/celt.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/celt/celt_encoder.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/celt/celt_decoder.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/celt/cwrs.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/celt/entcode.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/celt/entdec.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/celt/entenc.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/celt/kiss_fft.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/celt/laplace.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/celt/mathops.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/celt/mdct.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/celt/modes.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/celt/pitch.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/celt/celt_lpc.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/celt/quant_bands.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/celt/rate.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/celt/vq.c

    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/CNG.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/code_signs.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/init_decoder.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/decode_core.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/decode_frame.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/decode_parameters.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/decode_indices.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/decode_pulses.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/decoder_set_fs.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/dec_API.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/enc_API.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/encode_indices.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/encode_pulses.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/gain_quant.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/interpolate.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/LP_variable_cutoff.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/NLSF_decode.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/NSQ.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/NSQ_del_dec.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/PLC.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/shell_coder.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/tables_gain.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/tables_LTP.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/tables_NLSF_CB_NB_MB.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/tables_NLSF_CB_WB.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/tables_other.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/tables_pitch_lag.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/tables_pulses_per_block.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/VAD.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/control_audio_bandwidth.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/quant_LTP_gains.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/VQ_WMat_EC.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/HP_variable_cutoff.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/NLSF_encode.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/NLSF_VQ.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/NLSF_unpack.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/NLSF_del_dec_quant.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/process_NLSFs.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/stereo_LR_to_MS.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/stereo_MS_to_LR.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/check_control_input.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/control_SNR.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/init_encoder.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/control_codec.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/A2NLSF.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/ana_filt_bank_1.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/biquad_alt.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/bwexpander_32.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/bwexpander.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/debug.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/decode_pitch.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/inner_prod_aligned.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/lin2log.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/log2lin.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/LPC_analysis_filter.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/LPC_fit.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/LPC_inv_pred_gain.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/table_LSF_cos.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/NLSF2A.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/NLSF_stabilize.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/NLSF_VQ_weights_laroia.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/pitch_est_tables.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/resampler.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/resampler_down2_3.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/resampler_down2.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/resampler_private_AR2.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/resampler_private_down_FIR.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/resampler_private_IIR_FIR.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/resampler_private_up2_HQ.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/resampler_rom.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/sigm_Q15.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/sort.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/sum_sqr_shift.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/stereo_decode_pred.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/stereo_encode_pred.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/stereo_find_predictor.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/stereo_quant_pred.c

    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/fixed/LTP_analysis_filter_FIX.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/fixed/LTP_scale_ctrl_FIX.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/fixed/corrMatrix_FIX.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/fixed/encode_frame_FIX.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/fixed/find_LPC_FIX.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/fixed/find_LTP_FIX.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/fixed/find_pitch_lags_FIX.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/fixed/find_pred_coefs_FIX.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/fixed/noise_shape_analysis_FIX.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/fixed/process_gains_FIX.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/fixed/regularize_correlations_FIX.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/fixed/residual_energy16_FIX.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/fixed/residual_energy_FIX.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/fixed/warped_autocorrelation_FIX.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/fixed/apply_sine_window_FIX.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/fixed/autocorr_FIX.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/fixed/burg_modified_FIX.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/fixed/k2a_FIX.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/fixed/k2a_Q16_FIX.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/fixed/pitch_analysis_core_FIX.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/fixed/vector_ops_FIX.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/fixed/schur64_FIX.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/fixed/schur_FIX.c

    ${c_CMPT_AUDIO_DIR}/third_party/libopus/src/mapping_matrix.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/src/opus.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/src/opus_decoder.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/src/opus_encoder.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/src/opus_multistream.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/src/opus_multistream_encoder.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/src/opus_multistream_decoder.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/src/opus_projection_encoder.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/src/opus_projection_decoder.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/src/repacketizer.c

    ${c_CMPT_AUDIO_DIR}/third_party/libopus/src/analysis.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/src/mlp.c
    ${c_CMPT_AUDIO_DIR}/third_party/libopus/src/mlp_data.c

    # ${c_CMPT_AUDIO_DIR}/third_party/libopus/celt/arm/armcpu.c
    # ${c_CMPT_AUDIO_DIR}/third_party/libopus/celt/arm/arm_celt_map.c

    # ${c_CMPT_AUDIO_DIR}/third_party/libopus/celt/arm/celt_neon_intr.c
    # ${c_CMPT_AUDIO_DIR}/third_party/libopus/celt/arm/pitch_neon_intr.c
    # ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/arm/arm_silk_map.c
    # ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/arm/biquad_alt_neon_intr.c
    # ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/arm/LPC_inv_pred_gain_neon_intr.c
    # ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/arm/NSQ_del_dec_neon_intr.c
    # ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/arm/NSQ_neon.c
    # ${c_CMPT_AUDIO_DIR}/third_party/libopus/silk/fixed/arm/warped_autocorrelation_FIX_neon_intr.c

    # ${c_CMPT_AUDIO_DIR}/third_party/libopus/celt/arm/celt_pitch_xcorr_arm_gnu.s
)

ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_third_opus p_SCOPE interface
    NULL=0
    SOCKLEN_T=socklen_t
    LOCALE_NOT_USED
    _LARGEFILE_SOURCE=1
    _FILE_OFFSET_BITS=64
    restrict=
    __EMX__
    OPUS_BUILD
    FIXED_POINT
    USE_ALLOCA
    SIMD_EXTRA_ALLOC_BYTES=16
    HAVE_LRINT
    HAVE_LRINTF
    ENABLE_HARDENING
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_third_opus p_SCOPE interface
    -fno-math-errno
    -Wno-error
)
#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_third_speexdsp p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/third_party/speexdsp/libspeexdsp
    ${c_CMPT_AUDIO_DIR}/third_party/speexdsp/include
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_third_speexdsp p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/third_party/speexdsp/libspeexdsp/buffer.c
    ${c_CMPT_AUDIO_DIR}/third_party/speexdsp/libspeexdsp/fftwrap.c
    ${c_CMPT_AUDIO_DIR}/third_party/speexdsp/libspeexdsp/filterbank.c
    ${c_CMPT_AUDIO_DIR}/third_party/speexdsp/libspeexdsp/jitter.c
    ${c_CMPT_AUDIO_DIR}/third_party/speexdsp/libspeexdsp/kiss_fft.c
    ${c_CMPT_AUDIO_DIR}/third_party/speexdsp/libspeexdsp/kiss_fftr.c
    ${c_CMPT_AUDIO_DIR}/third_party/speexdsp/libspeexdsp/mdf.c
    ${c_CMPT_AUDIO_DIR}/third_party/speexdsp/libspeexdsp/preprocess.c
    ${c_CMPT_AUDIO_DIR}/third_party/speexdsp/libspeexdsp/resample.c
    ${c_CMPT_AUDIO_DIR}/third_party/speexdsp/libspeexdsp/scal.c
    ${c_CMPT_AUDIO_DIR}/third_party/speexdsp/libspeexdsp/smallft.c
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_third_speexdsp p_SCOPE interface
    __RTOS__
    HAVE_CONFIG_H
    FIXED_POINT
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_third_speexdsp p_SCOPE interface
    -Wno-error
)
# #--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_third_tremolo p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/third_party/tremolo
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_third_tremolo p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/third_party/tremolo/bitwise.c
    ${c_CMPT_AUDIO_DIR}/third_party/tremolo/codebook.c
    ${c_CMPT_AUDIO_DIR}/third_party/tremolo/dsp.c
    ${c_CMPT_AUDIO_DIR}/third_party/tremolo/floor0.c
    ${c_CMPT_AUDIO_DIR}/third_party/tremolo/floor1.c
    ${c_CMPT_AUDIO_DIR}/third_party/tremolo/floor_lookup.c
    ${c_CMPT_AUDIO_DIR}/third_party/tremolo/framing.c
    ${c_CMPT_AUDIO_DIR}/third_party/tremolo/mapping0.c
    ${c_CMPT_AUDIO_DIR}/third_party/tremolo/mdct.c
    ${c_CMPT_AUDIO_DIR}/third_party/tremolo/misc.c
    ${c_CMPT_AUDIO_DIR}/third_party/tremolo/res012.c
    ${c_CMPT_AUDIO_DIR}/third_party/tremolo/treminfo.c
    ${c_CMPT_AUDIO_DIR}/third_party/tremolo/vorbisfile.c
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_third_tremolo p_SCOPE interface
    __RTOS__
    ONLY_C
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_third_tremolo p_SCOPE interface
    -Wno-error
)
#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_audio_third_gsm p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/third_party/libgsm/inc
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_audio_third_gsm p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/third_party/libgsm/src/add.c
    ${c_CMPT_AUDIO_DIR}/third_party/libgsm/src/code.c
    ${c_CMPT_AUDIO_DIR}/third_party/libgsm/src/decode.c
    ${c_CMPT_AUDIO_DIR}/third_party/libgsm/src/gsm_create.c
    ${c_CMPT_AUDIO_DIR}/third_party/libgsm/src/gsm_decode.c
    ${c_CMPT_AUDIO_DIR}/third_party/libgsm/src/gsm_destroy.c
    ${c_CMPT_AUDIO_DIR}/third_party/libgsm/src/gsm_encode.c
    ${c_CMPT_AUDIO_DIR}/third_party/libgsm/src/gsm_option.c
    ${c_CMPT_AUDIO_DIR}/third_party/libgsm/src/long_term.c
    ${c_CMPT_AUDIO_DIR}/third_party/libgsm/src/lpc.c
    ${c_CMPT_AUDIO_DIR}/third_party/libgsm/src/preprocess.c
    ${c_CMPT_AUDIO_DIR}/third_party/libgsm/src/rpe.c
    ${c_CMPT_AUDIO_DIR}/third_party/libgsm/src/short_term.c
    ${c_CMPT_AUDIO_DIR}/third_party/libgsm/src/table.c
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_audio_third_gsm p_SCOPE interface
    __RTOS__
    __ SASR
    WAV49
    WMOPS=1
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_audio_third_gsm p_SCOPE interface
    -Wno-error
)
#--------------------------#
