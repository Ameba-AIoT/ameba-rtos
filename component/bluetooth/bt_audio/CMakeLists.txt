##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above
ameba_list_append(public_includes
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/bt_codec
    ${CMAKE_CURRENT_SOURCE_DIR}/audio_noise_cancellation
)

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries}) #default: whole-archived

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
	bt_audio_intf.c
	bt_audio_resample.c
	bt_audio_track_api.c
	bt_audio_record_api.c
	bt_audio_codec_wrapper.c
	bt_codec/sbc_codec_entity.c
	bt_codec/lc3_codec_entity.c
	bt_codec/cvsd_codec_entity.c
)

ameba_list_append(private_includes
	${c_CMPT_AUDIO_DIR}/interfaces
	${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
	${c_CMPT_BLUETOOTH_DIR}/rtk_stack/example/ble_audio
	${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/bluetooth/leaudio
	audio_sync
	audio_noise_cancellation
	bt_codec/sbc/decoder/include
	bt_codec/sbc/encoder/include
	bt_codec/lc3/include
	bt_codec/lc3/src
)

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For open-source component, always build from source
ameba_add_internal_library(bt_audio
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)

ameba_add_subdirectory(bt_codec)
ameba_add_subdirectory(audio_sync)
if(CONFIG_AMEBASMART)
    ameba_add_subdirectory_if(CONFIG_ARM_CORE_CA32 audio_noise_cancellation)
endif()
##########################################################################################