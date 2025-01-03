set(DIR ${BASEDIR}/component/audio/configs)
set(MODULE_IFLAGS ${DIR}/include)

set(MODULE_CPP_DEFINES __RTOS__)
set(MODULE_CPP_OPTIONS -Wno-multichar -Wno-unused-parameter -std=gnu++17 -std=c++17)

set(CPPSRC ${DIR}/ameba_media_extractor_usrcfg.cpp ${DIR}/ameba_media_decoder_usrcfg.cpp)

########## PLAYER CONFIGS ##########
if(CONFIG_MEDIA_PLAYER)
    list(APPEND MODULE_CPP_DEFINES MEDIA_PLAYER)

    #HELIC AAC
    #if(CONFIG_MEDIA_CODEC_HAAC)
    #list(APPEND MODULE_CPP_DEFINES MEDIA_DEMUX_HELIC)
    #endif()

    # AAC demux
    if(CONFIG_MEDIA_DEMUX_AAC)
        list(APPEND MODULE_CPP_DEFINES MEDIA_DEMUX_AAC)
    endif()

    # WAV demux
    if(CONFIG_MEDIA_DEMUX_WAV)
        list(APPEND MODULE_CPP_DEFINES MEDIA_DEMUX_WAV)
    endif()

    # MP3 demux
    if(CONFIG_MEDIA_DEMUX_MP3)
        list(APPEND MODULE_CPP_DEFINES MEDIA_DEMUX_MP3)
    endif()

    # AMR demux
    if(CONFIG_MEDIA_DEMUX_AMR)
        list(APPEND MODULE_CPP_DEFINES MEDIA_DEMUX_AMR)
    endif()

    # OGG demux
    if(CONFIG_MEDIA_DEMUX_OGG)
        list(APPEND MODULE_CPP_DEFINES MEDIA_DEMUX_OGG)
    endif()

    # FLAC demux
    if(CONFIG_MEDIA_DEMUX_FLAC)
        list(APPEND MODULE_CPP_DEFINES MEDIA_DEMUX_FLAC)
    endif()

    # MP4 demux
    if(CONFIG_MEDIA_DEMUX_MP4)
        list(APPEND MODULE_CPP_DEFINES MEDIA_DEMUX_MP4)
    endif()

    # rawpcm codec
    if(CONFIG_MEDIA_CODEC_PCM)
        list(APPEND MODULE_CPP_DEFINES MEDIA_CODEC_PCM)
    endif()

    # mp3 codec
    if(CONFIG_MEDIA_CODEC_MP3)
        list(APPEND MODULE_CPP_DEFINES MEDIA_CODEC_MP3)
    endif()

    # aac codec
    if(CONFIG_MEDIA_CODEC_AAC)
        list(APPEND MODULE_CPP_DEFINES MEDIA_CODEC_AAC)
    endif()

    # haac codec
    if(CONFIG_MEDIA_CODEC_HAAC)
        list(APPEND MODULE_CPP_DEFINES MEDIA_CODEC_HAAC)
    endif()

    # vorbis codec
    if(CONFIG_MEDIA_CODEC_VORBIS)
        list(APPEND MODULE_CPP_DEFINES MEDIA_CODEC_VORBIS)
    endif()

    # opus codec
    if(CONFIG_MEDIA_CODEC_OPUS)
        list(APPEND MODULE_CPP_DEFINES MEDIA_CODEC_OPUS)
    endif()

    # amr codec
    if(CONFIG_MEDIA_CODEC_AMR)
        list(APPEND MODULE_CPP_DEFINES MEDIA_CODEC_AMR)
    endif()

    #g711 codec
    if(CONFIG_MEDIA_CODEC_G711)
        list(APPEND MODULE_CPP_DEFINES MEDIA_CODEC_G711)
    endif()

    #gsm codec
    if(CONFIG_MEDIA_CODEC_GSM)
        list(APPEND MODULE_CPP_DEFINES MEDIA_CODEC_GSM)
    endif()
endif()


########## LITE PLAYER CONFIGS ##########
if(CONFIG_MEDIA_LITE_PLAYER)
    list(APPEND MODULE_CPP_DEFINES MEDIA_LITE_PLAYER)

    # AAC demux
    if(CONFIG_MEDIA_LITE_DEMUX_AAC)
        list(APPEND MODULE_CPP_DEFINES MEDIA_LITE_DEMUX_AAC)
    endif()

    # WAV demux
    if(CONFIG_MEDIA_LITE_DEMUX_WAV)
        list(APPEND MODULE_CPP_DEFINES MEDIA_LITE_DEMUX_WAV)
    endif()

    # MP3 demux
    if(CONFIG_MEDIA_LITE_DEMUX_MP3)
        list(APPEND MODULE_CPP_DEFINES MEDIA_LITE_DEMUX_MP3)
    endif()

    # MP4 demux
    if(CONFIG_MEDIA_LITE_DEMUX_MP4)
        list(APPEND MODULE_CPP_DEFINES MEDIA_LITE_DEMUX_MP4)
    endif()

    # rawpcm codec
    if(CONFIG_MEDIA_LITE_CODEC_PCM)
        list(APPEND MODULE_CPP_DEFINES MEDIA_LITE_CODEC_PCM)
    endif()

    # mp3 codec
    if(CONFIG_MEDIA_LITE_CODEC_MP3)
        list(APPEND MODULE_CPP_DEFINES MEDIA_LITE_CODEC_MP3)
    endif()

    # haac codec
    if(CONFIG_MEDIA_LITE_CODEC_HAAC)
        list(APPEND MODULE_CPP_DEFINES MEDIA_LITE_CODEC_HAAC)
    endif()
endif()

ameba_internal_library(media_config)
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${MODULE_CPP_DEFINES}>)
target_compile_options(${CURRENT_LIB_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${MODULE_CPP_OPTIONS}>)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CPPSRC})
