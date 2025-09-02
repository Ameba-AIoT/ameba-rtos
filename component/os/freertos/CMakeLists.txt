##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#------------------------------------------------------------------#
# Component public part, user config begin(DO NOT remove this line)
# You may use if-else condition to set or update predefined variable above

if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    if("${c_MCU_PROJECT_NAME}" STREQUAL "ap")
        set(portable_soc_mcu_dir ${c_FREERTOS_DIR}/portable/GCC/ARM_CA32)
        if(CONFIG_MP_SHRINK)
            if("${CONFIG_CORE_NUM}" EQUAL 1)
                set(output_name freertos_smp_sc_mp)
            else()
                set(output_name freertos_smp_mp)
            endif()
        else()
            if("${CONFIG_CORE_NUM}" EQUAL 1)
                set(output_name freertos_smp_sc)
            else()
                set(output_name freertos_smp)
            endif()
        endif()

        ameba_list_append(public_libraries
            ${c_SDK_LIB_SOC_DIR}/lib_${output_name}.a
        )
    else()
        #OPTIMIZE: Change the dir name from AmebaSmart_HP/AmebaSmart_LP to AmebaSmart_KM4/AmebaSmart_KM0 will be same as other soc.\
        #OPTIMIZE: Amebasmart and amebasmartplus share same port dir
        set(portable_soc_mcu_dir ${c_FREERTOS_DIR}/portable/GCC/AmebaSmart_${c_MCU_PROJECT_NAME_UPPER})
        set(output_name freertos)
    endif()
else()
    set(portable_soc_mcu_dir ${c_FREERTOS_DIR}/portable/GCC/${c_SOC_TYPE_CAMEL}_${c_MCU_TYPE_UPPER})
    set(output_name freertos)
endif()

ameba_list_append(public_includes
    ${c_FREERTOS_DIR}/include #Path to freertos/freertos_vxxx/Source/include
    ${c_CMPT_OS_DIR}/freertos/os_wrapper/include
    ${portable_soc_mcu_dir}
)

ameba_list_append(public_includes
    heap_trace
)

if(EXISTS ${portable_soc_mcu_dir}/secure)
    ameba_list_append(public_includes
        ${portable_soc_mcu_dir}/secure
    )
endif()

if(EXISTS ${portable_soc_mcu_dir}/non_secure)
    ameba_list_append(public_includes
        ${portable_soc_mcu_dir}/non_secure
    )
endif()

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
        ${c_FREERTOS_DIR}/tasks.c
        ${c_FREERTOS_DIR}/list.c
        ${c_FREERTOS_DIR}/croutine.c
        ${c_FREERTOS_DIR}/queue.c
        ${c_FREERTOS_DIR}/timers.c
        ${c_FREERTOS_DIR}/event_groups.c
        ${c_FREERTOS_DIR}/stream_buffer.c
        ${c_FREERTOS_DIR}/portable/MemMang/heap_5.c
    )

    if(CONFIG_AMEBAD OR CONFIG_AMEBADPLUS)
        if("${c_MCU_PROJECT_NAME}" STREQUAL "km0")
            ameba_list_append(private_sources
                ${portable_soc_mcu_dir}/port.c
            )
        elseif("${c_MCU_PROJECT_NAME}" STREQUAL "km4")
            ameba_list_append(private_sources
                ${portable_soc_mcu_dir}/non_secure/port.c
                ${portable_soc_mcu_dir}/non_secure/portasm.c
            )
        endif()
    elseif(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
        if("${c_MCU_PROJECT_NAME}" STREQUAL "hp")
            ameba_list_append(private_sources
                ${portable_soc_mcu_dir}/non_secure/port.c
            )
            ameba_list_append_if(CONFIG_TRUSTZONE_EN private_sources
                ${portable_soc_mcu_dir}/non_secure/portasm.c
                p_ELSE
                ${portable_soc_mcu_dir}/non_secure/portasm_ntz.c
            )
        elseif("${c_MCU_PROJECT_NAME}" STREQUAL "lp")
            ameba_list_append(private_sources
                ${portable_soc_mcu_dir}/port.c
                ${portable_soc_mcu_dir}/portasm.c
            )
        elseif("${c_MCU_PROJECT_NAME}" STREQUAL "ap")
            ameba_list_append(private_sources
                ${portable_soc_mcu_dir}/port.c
                ${portable_soc_mcu_dir}/portASM.S #OPTIMIZE: change name to portasm.c?
            )
        endif()
    elseif(CONFIG_AMEBAGREEN2 OR CONFIG_AMEBAL2 OR CONFIG_RTL8720F)
        ameba_list_append(private_sources
            ${portable_soc_mcu_dir}/non_secure/port.c
            ${portable_soc_mcu_dir}/non_secure/portasm.c
        )
    elseif(CONFIG_AMEBALITE)
        if("${c_MCU_PROJECT_NAME}" STREQUAL "km4")
            ameba_list_append(private_sources
                ${portable_soc_mcu_dir}/non_secure/port.c
                ${portable_soc_mcu_dir}/non_secure/portasm.c
            )
        elseif("${c_MCU_PROJECT_NAME}" STREQUAL "kr4")
            ameba_list_append(private_sources
                ${portable_soc_mcu_dir}/port.c
                ${portable_soc_mcu_dir}/rtk-port.c
                ${portable_soc_mcu_dir}/portASM.S #OPTIMIZE: change name to portasm.c?
            )
        endif()
    endif()

if(CONFIG_HEAP_PROTECTOR)
    ameba_list_append(private_sources
        heap_trace/heap_trace.c
    )

    file(RELATIVE_PATH IMAGE_DIR ${FINAL_IMAGE_DIR} ${c_SDK_IMAGE_TARGET_DIR})
    ameba_list_append(private_definitions
        SDK_TOOLCHAIN="${SDK_TOOLCHAIN}"
        IMAGE_DIR="${IMAGE_DIR}"
    )
endif()

elseif(${c_CURRENT_IMAGE_TYPE} STREQUAL "image3")
    ameba_list_append(private_sources
        ${portable_soc_mcu_dir}/secure/secure_context.c
        ${portable_soc_mcu_dir}/secure/secure_context_port.c
        ${portable_soc_mcu_dir}/secure/secure_heap.c
        ${portable_soc_mcu_dir}/secure/secure_init.c
        os_wrapper/os_wrapper_memory.c
    )
    ameba_list_append(private_compile_options
        -O0
    )
endif()

# Component private part, user config end(DO NOT remove this line)
#------------------------------------------------------------------#

if("${c_MCU_TYPE}" STREQUAL "ca32")
    ###NOTE: For closed-source component, only build before release and libs are packaged into lib/soc
    ameba_add_external_soc_library(${output_name}
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
else()
    ###NOTE: For open-source component, always build from source
    ameba_add_internal_library(${output_name}
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
endif()
##########################################################################################