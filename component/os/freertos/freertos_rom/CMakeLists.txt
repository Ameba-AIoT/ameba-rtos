##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#------------------------------------------------------------------#
# Component public part, user config begin(DO NOT remove this line)
# You may use if-else condition to set or update predefined variable above

set(portable_soc_mcu_dir ${c_FREERTOS_DIR}/portable/GCC/${c_SOC_TYPE_CAMEL}_${c_MCU_TYPE_UPPER})
ameba_list_append(public_includes
    ${c_CMPT_OS_DIR}/freertos/os_wrapper/include
    ${c_SOC_TYPE}/freertos_${v_FREERTOS_VER}/Source/include
    ${c_SOC_TYPE}/freertos_${v_FREERTOS_VER}/os_cfg
    ${portable_soc_mcu_dir}/secure
    ${portable_soc_mcu_dir}/non_secure
)

# Component public part, user config end(DO NOT remove this line)
#------------------------------------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries}) #default: whole-achived

##########################################################################################
## * This part defines private part of the component
## * Private part is used to build target of current component
## * NOTE: The build API guarantees the global build configures(mentioned above)
## *       applied to the target automatically. So if any configure was already added
## *       to public, it's unnecessary to add again below.


# They are only for ameba_add_internal_library/ameba_add_external_app_library/ameba_add_external_soc_library
set(private_sources)                 #private source files, NOTE: relative path is OK
set(private_includes)                #private include directories, NOTE: relative path is OK
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------------------------------------------#
# Component private part, user config begin(DO NOT remove this line)

#NOTE: User defined section, add your private build configures here
# You may use if-else condition to set these predefined variable

if(${c_CURRENT_IMAGE_TYPE} STREQUAL "image2")
    ameba_list_append(private_sources
        ${c_SOC_TYPE}/freertos_${v_FREERTOS_VER}/os_cfg/os_cfg_ram.c
        ${c_SOC_TYPE}/freertos_${v_FREERTOS_VER}/Source/portable/MemMang/heap_5_patch.c
    )
elseif(${c_CURRENT_IMAGE_TYPE} STREQUAL "image3")
    ameba_list_append(private_sources
        ${portable_soc_mcu_dir}/secure/secure_context.c
        ${portable_soc_mcu_dir}/secure/secure_context_port.c
        ${portable_soc_mcu_dir}/secure/secure_heap.c
        ${portable_soc_mcu_dir}/secure/secure_init.c
    )
    ameba_list_append(private_compile_options
        -O0
    )
endif()

# Component private part, user config end(DO NOT remove this line)
#------------------------------------------------------------------#
ameba_add_internal_library(freertos
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
    p_DEPENDENCIES
        ${c_BUILD_INFO}
)
##########################################################################################