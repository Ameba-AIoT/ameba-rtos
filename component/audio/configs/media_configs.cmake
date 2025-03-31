##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries})

##########################################################################################
## * This part defines private part of the component
## * Private part is used to build target of current component
## * NOTE: The build API guarantees the global build configures(mentioned above)
## *       applied to the target automatically. So if any configure was already added
## *       to public above, it's unnecessary to add again below.

#NOTE: User defined section, add your private build configures here
# You may use if-else condition to set these predefined variable
# They are only for ameba_add_internal_library/ameba_add_external_app_library/ameba_add_external_soc_library
set(private_sources)                 #private source files, NOTE: relative path is OK
set(private_includes)                #private include directories, NOTE: relative path is OK
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------#
# Component private part, user config begin

ameba_list_append(private_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/ameba_media_extractor_usrcfg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/ameba_media_decoder_usrcfg.cpp
)
ameba_list_append(private_includes
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/include
    ${c_CMPT_AUDIO_DIR}/interfaces
)

if(CONFIG_MEDIA_PLAYER)
    ameba_list_append(private_definitions
        MEDIA_PLAYER
    )
endif()

if(CONFIG_MEDIA_CODEC_HAAC)
    ameba_list_append(private_definitions
        MEDIA_DEMUX_AAC_DELIVER_ADTS_HEADER
    )
endif()

if(CONFIG_MEDIA_DEMUX_AAC)
    ameba_list_append(private_definitions
        MEDIA_DEMUX_AAC
    )
endif()

if(CONFIG_MEDIA_DEMUX_WAV)
    ameba_list_append(private_definitions
        MEDIA_DEMUX_WAV
    )
endif()

if(CONFIG_MEDIA_DEMUX_MP3)
    ameba_list_append(private_definitions
        MEDIA_DEMUX_MP3
    )
endif()

if(CONFIG_MEDIA_DEMUX_AMR)
    ameba_list_append(private_definitions
        MEDIA_DEMUX_AMR
    )
endif()

if(CONFIG_MEDIA_DEMUX_OGG)
    ameba_list_append(private_definitions
        MEDIA_DEMUX_OGG
    )
endif()

if(CONFIG_MEDIA_DEMUX_FLAC)
    ameba_list_append(private_definitions
        MEDIA_DEMUX_FLAC
    )
endif()

if(CONFIG_MEDIA_DEMUX_MP4)
    ameba_list_append(private_definitions
        MEDIA_DEMUX_MP4
    )
endif()


if(CONFIG_MEDIA_CODEC_PCM)
    ameba_list_append(private_definitions
        MEDIA_CODEC_PCM
    )
endif()

if(CONFIG_MEDIA_CODEC_AAC)
    ameba_list_append(private_definitions
        MEDIA_CODEC_AAC
    )
endif()

if(CONFIG_MEDIA_CODEC_HAAC)
    ameba_list_append(private_definitions
        MEDIA_CODEC_HAAC
    )
endif()

if(CONFIG_MEDIA_CODEC_MP3)
    ameba_list_append(private_definitions
        MEDIA_CODEC_MP3
    )
endif()

if(CONFIG_MEDIA_CODEC_VORBIS)
    ameba_list_append(private_definitions
        MEDIA_CODEC_VORBIS
    )
endif()

if(CONFIG_MEDIA_CODEC_OPUS)
    ameba_list_append(private_definitions
       MEDIA_CODEC_OPUS
    )
endif()

if(CONFIG_MEDIA_CODEC_AMR)
    ameba_list_append(private_definitions
        MEDIA_CODEC_AMR
    )
endif()

if(CONFIG_MEDIA_CODEC_G711)
    ameba_list_append(private_definitions
        MEDIA_CODEC_G711
    )
endif()

if(CONFIG_MEDIA_CODEC_GSM)
    ameba_list_append(private_definitions
        MEDIA_CODEC_GSM
    )
endif()

ameba_list_append(private_definitions
    __RTOS__
)

ameba_list_append(private_compile_options
    $<$<COMPILE_LANGUAGE:C>:-Wno-multichar>
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-parameter>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-deprecated-copy>
    $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>
)

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For closed-source component, only build before release and libs are packaged into lib/application
ameba_add_external_tmp_library(media_configs
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)
##########################################################################################