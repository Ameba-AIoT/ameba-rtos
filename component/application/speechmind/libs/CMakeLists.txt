##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#

ameba_list_append(public_libraries
    ${c_CMPT_APP_DIR}/speechmind/libs/${c_SOC_TYPE}/${c_MCU_TYPE}/lib_speechmind_base.a
    ${c_CMPT_APP_DIR}/speechmind/libs/${c_SOC_TYPE}/${c_MCU_TYPE}/lib_speechmind_foundation.a
)
#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_defines})
ameba_global_library(${public_libraries}) #default: whole-archived
