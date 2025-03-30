##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin
# You may use if-else condition to set or update predefined variable above

ameba_list_append(public_includes include)
ameba_list_append(public_libraries ${c_SDK_LIB_SOC_DIR}/lib_crashdump.a)

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries}) #default: whole-achived

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

if(${c_ISA_TYPE} STREQUAL "ARM")
    ameba_list_append(private_sources
        ameba_freertos_backtrace.c
        arm/ameba_v8m_backtrace.c
        arm/ameba_v8m_crashdump.c
        arm/ameba_v8m_fault_diagnosis.c
    )
elseif(${c_ISA_TYPE} STREQUAL "RISC-V")
    ameba_list_append(private_sources
        ameba_freertos_backtrace.c
        riscv/ameba_rv32_backtrace.c
        riscv/ameba_rv32_crashdump.c
    )
else()
    ameba_fatal("crashdump's CMakeLists.txt is not ready for current isa type: ${c_ISA_TYPE}")
endif()

ameba_list_append(private_definitions
    SDK_TOOLCHAIN="${SDK_TOOLCHAIN}"
    IMAGE_DIR="${c_SDK_IMAGE_TARGET_DIR}"
)
# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For closed-source component, only build before release and libs are packaged into lib/soc
ameba_add_external_soc_library(crashdump
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
