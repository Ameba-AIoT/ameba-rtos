if(NOT CONFIG_TFLITE_MICRO_EN)
    return() # DO nothing if CONFIG_TFLITE_MICRO_EN is not set
endif()
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
    ${c_CMPT_TFLITE_DIR}
    ${c_CMPT_TFLITE_DIR}/third_party/gemmlowp
    ${c_CMPT_TFLITE_DIR}/third_party/flatbuffers/include
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

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

#Some custom variables need to be transferred into tflite dir
set(BUILD_IN_MCU_SDK TRUE)
if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    set(CMAKE_CHIP_TARGET amebasmart_ca32_freertos)
else()
    set(CMAKE_CHIP_TARGET ${c_SOC_TYPE}_${c_MCU_TYPE})
endif()

#Add origin tflite dir to build
ameba_add_subdirectory(${c_CMPT_TFLITE_DIR})

###NOTE: For closed-source and with standalone build scripts component, process and copy library to lib/application
ameba_port_standalone_internal_library(
    tensorflow-microlite    # The name of target in origin tflite who generates the library directly
)
if(CONFIG_AMEBALITE OR CONFIG_AMEBADPLUS)
    ameba_port_standalone_internal_library(
        cmsis-nn            # The name of target in origin tflite who generates the library directly
    )
endif()
