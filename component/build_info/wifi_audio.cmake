if (__INCLUDED_WIFI_AUDIO_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_WIFI_AUDIO_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_wifi_audio INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_wifi_audio_aac_helper INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_wifi_audio_base_cutils INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_wifi_audio_base_log INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_wifi_audio_base_utils INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_wifi_audio_common INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_wifi_audio_fec INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_wifi_audio_flac_helper INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_wifi_audio_foundation INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_wifi_audio_libs INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_wifi_audio_plc INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_wifi_audio_tests INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_wifi_audio_xip INTERFACE)

#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_wifi_audio p_SCOPE interface
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
    ${c_CMPT_AUDIO_DIR}/interfaces
    ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/log/include
    ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/cutils/include
    ${c_CMPT_APPLICATION_DIR}/wifi-audio/interfaces
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_wifi_audio p_SCOPE interface __RTOS__)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_wifi_audio p_SCOPE interface -Wno-multichar)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_wifi_audio p_SCOPE interface
    ${c_CMPT_APPLICATION_DIR}/wifi-audio/cmd/audioficast_example.c
    ${c_CMPT_APPLICATION_DIR}/wifi-audio/cmd/audioficast_cmd.cpp
    ${c_CMPT_APPLICATION_DIR}/wifi-audio/cmd/audioficast_sink_cmd.cpp
)
#--------------------------#
ameba_target_set(src_${d_MCU_PROJECT_NAME}_wifi_audio_aac_helper p_SCOPE interface
    p_INCLUDES
        ${c_CMPT_AUDIO_DIR}/interfaces
        ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
        ${c_CMPT_AUDIO_DIR}/base/libs/src/include
        ${c_CMPT_AUDIO_DIR}/base/log/interfaces
        ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
        ${c_CMPT_AUDIO_DIR}/base/osal/osal_cxx/interfaces
        ${c_CMPT_AUDIO_DIR}/base/cutils/interfaces
        ${c_CMPT_AUDIO_DIR}/interfaces
        ${c_CMPT_AUDIO_DIR}/third_party/aac/libAACenc/include
        ${c_CMPT_AUDIO_DIR}/third_party/aac/libAACdec/include
        ${c_CMPT_AUDIO_DIR}/third_party/aac/libAACdec/include
        ${c_CMPT_AUDIO_DIR}/third_party/aac/libPCMutils/include
        ${c_CMPT_AUDIO_DIR}/third_party/aac/libFDK/include
        ${c_CMPT_AUDIO_DIR}/third_party/aac/libSYS/include
        ${c_CMPT_AUDIO_DIR}/third_party/aac/libMpegTPDec/include
        ${c_CMPT_AUDIO_DIR}/third_party/aac/libSBRdec/include
        ${c_CMPT_AUDIO_DIR}/third_party/aac/libArithCoding/include
        ${c_CMPT_AUDIO_DIR}/third_party/aac/libDRCdec/include
        ${c_CMPT_AUDIO_DIR}/third_party/aac/libSACdec/include
        ${c_CMPT_AUDIO_DIR}/base/audio_utils/include
        ${c_CMPT_AUDIO_DIR}/interfaces
    # p_SOURCES
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/aac_helper/aac_helper.c #FIXME: file not exist
)
#--------------------------#
ameba_target_set(src_${d_MCU_PROJECT_NAME}_wifi_audio_base_cutils p_SCOPE interface
    p_COMPILE_DEFINES
        __RTOS__
    # p_SOURCES
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/cutils/wfa_mem.c
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/cutils/wfa_mutex.c
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/cutils/wfa_thread.c
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/cutils/wfa_time.c
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/cutils/wfa_timer.c
)
#--------------------------#
ameba_target_set(src_${d_MCU_PROJECT_NAME}_wifi_audio_base_log p_SCOPE interface
    p_COMPILE_DEFINES
        __RTOS__
    p_INCLUDES
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/log/include
    # p_SOURCES
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/log/log.c
)
#--------------------------#
ameba_target_set(src_${d_MCU_PROJECT_NAME}_wifi_audio_base_utils p_SCOPE interface
    p_COMPILE_DEFINES
        __RTOS__
    p_INCLUDES
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/log/include
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/cutils/include
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/utils/include
    # p_SOURCES
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/utils/refbase.cpp
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/utils/mutex_lock.cpp
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/utils/conditions.cpp
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/utils/thread.cpp
)
#--------------------------#
ameba_target_set(src_${d_MCU_PROJECT_NAME}_wifi_audio_common p_SCOPE interface
    p_COMPILE_DEFINES
        __RTOS__
    p_COMPILE_OPTIONS
        -Wno-multichar
        -Wno-unused-parameter
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-narrowing>
        $<$<COMPILE_LANGUAGE:CXX>:-std=c++14>
        $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++14>
    p_INCLUDES
        ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
        ${c_CMPT_AUDIO_DIR}/interfaces
        ${c_CMPT_WIFI_DIR}/wifi_cast
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/log/include
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/cutils/include
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/utils/include
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/foundation/include
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/fec
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/fec/fec
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/interfaces
    # p_SOURCES
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/common/apacket_source.cpp
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/common/aringbuffer_packet_source.cpp
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/common/network_session.cpp
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/common/udp_session.cpp
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/common/wifi_cast_session.cpp
)

#--------------------------#
ameba_target_set(src_${d_MCU_PROJECT_NAME}_wifi_audio_fec p_SCOPE interface
    p_COMPILE_DEFINES
        __RTOS__
    p_COMPILE_OPTIONS
        -Wno-multichar
        -Wno-unused-parameter
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-narrowing>
        $<$<COMPILE_LANGUAGE:CXX>:-std=c++14>
        $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++14>
    p_INCLUDES
        ${c_CMPT_AUDIO_DIR}/interfaces
        ${c_CMPT_AUDIO_DIR}/audio_algorithm
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/cutils/include
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/utils/include
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/fec
    # p_SOURCES
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/fec/fec/fec_private_tables_bursty.cc
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/fec/fec/fec_private_tables_random.cc
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/fec/fec/flexfec_header_reader_writer.cc
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/fec/fec/forward_error_correction.cc
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/fec/fec/forward_error_correction_internal.cc
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/fec/fec/ulpfec_header_reader_writer.cc
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/fec/fec/copy_on_write_buffer.cc
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/fec/fec/rtp_header_extension_map.cc
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/fec/fec/flexfec_sender.cc
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/fec/fec/flexfec_receiver.cc
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/fec/fec/rtp_packet.cc
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/fec/fec/rtp_packet_received.cc
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/fec/fec/rtp_packet_to_send.cc
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/fec/fec/ulpfec_receiver.cc
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/fec/fec/ulpfec_generator.cc
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/fec/fec/rtp_headers.cc
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/fec/fec/zero_memory.cc
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/fec/fec/random.cc
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/fec/fec/checks.cc
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/fec/fec/logging.cc
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/fec
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/fec/absl/base/internal/raw_logging.cc
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/fec/absl/strings/internal/string_view.cc
)
#--------------------------#
ameba_target_set(src_${d_MCU_PROJECT_NAME}_wifi_audio_flac_helper p_SCOPE interface
    p_INCLUDES
        ${c_CMPT_AUDIO_DIR}/third_party/flac/include/FLAC
    # p_SOURCES
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/flac_helper/flac_helper.c
)
#--------------------------#
ameba_target_set(src_${d_MCU_PROJECT_NAME}_wifi_audio_foundation p_SCOPE interface
    p_COMPILE_DEFINES
        __RTOS__
    p_COMPILE_OPTIONS
        -Wno-multichar
        -Wno-unused-parameter
    p_INCLUDES
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/log/include
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/cutils/include
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/utils/include
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/foundation/include
    p_SOURCES
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/foundation/rstring.cpp #FIXME: file not exist
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/foundation/hexdump.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/foundation/rbuffer.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/foundation/rhandler.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/foundation/rlooper.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/foundation/rlooper_manager.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/foundation/rmessage.cpp
)
#--------------------------#
ameba_target_set(src_${d_MCU_PROJECT_NAME}_wifi_audio_libs p_SCOPE interface
    p_COMPILE_DEFINES
        __RTOS__
    p_COMPILE_OPTIONS
        -Wno-multichar
        -Wno-unused-parameter
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-narrowing>
        $<$<COMPILE_LANGUAGE:CXX>:-std=c++14>
        $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++14>
    p_INCLUDES
        ${c_CMPT_AUDIO_DIR}/base/libs/src/include
        ${c_CMPT_AUDIO_DIR}/base/cutils/interfaces
        ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
        ${c_CMPT_AUDIO_DIR}/base/audio_utils/include
        ${c_CMPT_AUDIO_DIR}/interfaces
        ${c_CMPT_AUDIO_DIR}/audio_hal/base
        ${c_CMPT_WIFI_DIR}/wifi_cast
        ${c_CMPT_AUDIO_DIR}/third_party/flac/include/FLAC
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/log/include
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/cutils/include
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/utils/include
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/foundation/include
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/plc
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/fec
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/fec/fec
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/aac_helper
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/flac_helper
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/common
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/source
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/sink
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/interfaces
    p_SOURCES
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/source/wave_table.cpp  #FIXME: file not exist
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/source/wave_source_base.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/source/sine_source.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/source/pulse_source.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/source/triangle_source.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/source/wave_source.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/source/audio_source.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/source/capture_source.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/source/wav_file_source.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/source/wave_file_source.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/source/audioficast.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/source/wifi_audio_source.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/source/app_server.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/source/channel_monitor.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/source/rtcp_sender.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/source/network_sender.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/source/source_assembler.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/source/playback_session.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/sink/audioficast_sink.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/sink/wifi_audio_sink.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/sink/clock_anchor.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/sink/rtcp_receiver.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/sink/network_atp_sink.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/sink/sink_assembler.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/sink/tunnel_renderer.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/sink/fiplayer_pll_sync.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/sink/fiplayer_renderer.cpp
        # ${c_CMPT_APPLICATION_DIR}/wifi-audio/sink/fiplayer_audio_output.cpp
)
#--------------------------#
ameba_target_set(src_${d_MCU_PROJECT_NAME}_wifi_audio_plc p_SCOPE interface
    p_COMPILE_DEFINES
        __RTOS__
    p_COMPILE_OPTIONS
        -Wno-multichar
        -Wno-unused-parameter
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-narrowing>
    p_INCLUDES
        ${c_CMPT_AUDIO_DIR}/interfaces
        ${c_CMPT_AUDIO_DIR}/base/audio_utils/include
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/log/include
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/cutils/include
    # p_SOURCES
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/plc/rlist.c
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/plc/vox_mem.c
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/plc/vox_math.c
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/plc/rt_plc_engine.c
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/plc/rt_plc.cpp
)
#--------------------------#
ameba_target_set(src_${d_MCU_PROJECT_NAME}_wifi_audio_tests p_SCOPE interface
    p_COMPILE_DEFINES
        __RTOS__
    p_COMPILE_OPTIONS
        -Wno-multichar
        -Wno-incompatible-pointer-types
        -Wno-int-conversion -Wno-pointer-sign
    p_INCLUDES
        ${c_CMPT_AUDIO_DIR}/interfaces
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/log/include
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/cutils/include
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/base/utils/include
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/foundation/include
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/common
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/include
    # p_SOURCES
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/tests/app_example.c
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/tests/wfa_timer_test.c

    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/tests/thread_test.cpp
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/tests/rlooper_test.cpp
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/tests/rmessage_test.cpp
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/tests/playback_source_test.cpp
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/tests/wifi_audio_imitate_test.cpp
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/tests/tunnel_renderer_test.cpp
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/tests/playback_session_test.cpp
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/tests/rtcodec_test.cpp
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/tests/flac_enc_test.cpp
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/tests/flac_dec_test.cpp
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/tests/apacket_source_test.cpp
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/tests/apacket_ringbuffer_test.cpp
    #     ${c_CMPT_APPLICATION_DIR}/wifi-audio/tests/fiplayer_render_test.cpp
)
#--------------------------#
