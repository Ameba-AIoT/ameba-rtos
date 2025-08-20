##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above

ameba_list_append_if(CONFIG_AUDIO_MIXER public_libraries
    ${c_SDK_LIB_APPLICATION_DIR}/lib_audio_mixer.a
)

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries} p_NO_WHOLE_ARCHIVE)

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

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

ameba_add_merge_app_library(audio_mixer
    audio_algorithm_plc
    audio_algorithm_sonic
    audio_control_local
    audio_effect_core
    audio_effect_local
    audio_effect_sw_algorithm_bundle
    audio_effect_sw_algorithm_common
    audio_effect_sw_algorithm_eq
    audio_effect_sw_algorithm_wrapper
    audio_stream_core
    audio_stream_local
    audio_stream_services
    audio_policy_common
    audio_policy_impl
    audio_policy_coodinator
    audio_policy_strategy
)
##########################################################################################
