if(CONFIG_MEDIA_PLAYER)
    ameba_add_subdirectory_if_exist(media_utils)
    ameba_add_subdirectory_if_exist(media_common)
    ameba_add_subdirectory_if_exist(media_source)
    ameba_add_subdirectory_if_exist(media_demux)

    ameba_add_subdirectory_if_exist(media_codec)
    ameba_add_subdirectory_if_exist(media_core)
    ameba_add_subdirectory_if_exist(media_libmedia)

    include(configs/media_configs.cmake)
    include(play_back.cmake)
endif()

if(CONFIG_AUDIO_FWK)
    ameba_add_subdirectory_if_exist(audio_algorithm)
    ameba_add_subdirectory_if_exist(audio_effect)
    ameba_add_subdirectory_if_exist(audio_control)
    ameba_add_subdirectory_if_exist(audio_stream)
    include(configs/audio_configs.cmake)
    include(audio_mixer.cmake)
    include(audio_passthrough.cmake)
endif()

ameba_add_subdirectory_if(CONFIG_AUDIO_FWK audio_hal)


ameba_add_subdirectory(third_party)

if(CONFIG_AUDIO_FWK)
    ameba_add_subdirectory_if_exist(base)
    include(base.cmake)
endif()


