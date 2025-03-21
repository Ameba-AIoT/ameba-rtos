include_guard(GLOBAL)
include(${CMAKE_CURRENT_LIST_DIR}/global_define.cmake)

# Get the current directory name to get platform and compile type, and Try matching again for the parent directory when failed.
function(ameba_parse_platform)
    get_filename_component(CURRENT_DIR "${CMAKE_CURRENT_LIST_DIR}" NAME)
    string(REGEX MATCH "^([a-zA-Z0-9]+)_([a-zA-Z0-9_]+)$" _ "${CURRENT_DIR}")

    if (DEFINED CMAKE_MATCH_1 AND DEFINED CMAKE_MATCH_2)
        ameba_set(_tmp_PLATFORM_TYPE "${CMAKE_MATCH_1}" p_SCOPE parent) #amebaxxx
    else()
        get_filename_component(CURRENT_DIR "${CMAKE_CURRENT_LIST_DIR}" DIRECTORY) # in project_xx subdirectory
        get_filename_component(DIR_NAME "${CURRENT_DIR}" NAME)
        string(REGEX MATCH "^([a-zA-Z0-9]+)_([a-zA-Z0-9_]+)$" _ "${CURRENT_DIR}")
        if (DEFINED CMAKE_MATCH_1 AND DEFINED CMAKE_MATCH_2)
            ameba_set(_tmp_PLATFORM_TYPE "${CMAKE_MATCH_1}" p_SCOPE parent)
        else()
            message(FATAL_ERROR "Directory name does not match expected pattern 'chip_compiler'")
        endif()
    endif()
endfunction()

# ARG_p_TYPE value maybe amebaxxx
# Usage:
#   ameba_platform_project_create([p_TYPE <typevalue>])
function(ameba_platform_project_create)
    set(options)
    set(oneValueArgs p_TYPE)
    set(multiValueArgs)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (ARG_p_TYPE)
        ameba_set(_tmp_PLATFORM_TYPE ${ARG_p_TYPE} p_SCOPE both)
    else()
        ameba_parse_platform()
        ameba_set(_tmp_PLATFORM_TYPE ${_tmp_PLATFORM_TYPE} p_SCOPE both)
    endif()

    ameba_set(CMAKE_BUILD_TYPE "" p_SCOPE parent)
    ameba_set(d_PLATFORM_PROJECT_CREATED TRUE p_SCOPE parent)
endfunction()

# Usage:
#   ameba_platform_project_init()
function(ameba_platform_project_init)
    if (NOT d_PLATFORM_PROJECT_CREATED)
        ameba_fatal("Call ameba_platform_project_create before ameba_platform_project_init")
    endif()

    #NOTE: used in CMAKE_V0.2

    ameba_set(d_PLATFORM_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR} p_SCOPE both)
    ameba_set_move(d_PLATFORM_TYPE _tmp_PLATFORM_TYPE p_SCOPE both)
    ameba_set_upper(d_PLATFORM_TYPE_UPPER ${d_PLATFORM_TYPE} p_SCOPE both)
    ameba_set(d_SOC_PLATFORM_DIR ${c_BASEDIR}/component/soc/${d_PLATFORM_TYPE} p_SCOPE both)

     #NOTE: For compatibility with version CMAKE_V0.1, use variable starts with d_ above
    ameba_set(TARGETDIR ${d_SOC_PLATFORM_DIR} p_SCOPE both)
    ameba_set(BOOTLOADER ${TARGETDIR}/bootloader p_SCOPE both)

    ameba_set(d_PLATFORM_PROJECT_INITIALIZED TRUE p_SCOPE parent)
endfunction()

# Usage:
#   ameba_platform_project_config()
macro(ameba_platform_project_config)
    ameba_platform_project_custom_config()
    ameba_set(d_WORKING_PROJECT_DIR ${CMAKE_BINARY_DIR}/..)
    ameba_set(c_MENUCONFIG_DIR ${d_WORKING_PROJECT_DIR}/menuconfig)
    #EXAMPLE can be a absolute path, relative path where contain a CMakeLists and source files, or can be a specific example name
    if (EXAMPLE)
        if(IS_ABSOLUTE EXAMPLE)
            set(EXAMPLEDIR ${EXAMPLE})
        else()
            cmake_path(ABSOLUTE_PATH EXAMPLE BASE_DIRECTORY ${d_WORKING_PROJECT_DIR} NORMALIZE OUTPUT_VARIABLE OUTPUT_EXAMPLE)
            message(${OUTPUT_EXAMPLE})
            if (EXISTS ${OUTPUT_EXAMPLE})
                set(EXAMPLEDIR ${OUTPUT_EXAMPLE})
            else()
                file(GLOB_RECURSE EXAMPLEDIR
                    ${c_CMPT_EXAMPLE_DIR}/example_${EXAMPLE}.c
                    ${c_CMPT_EXAMPLE_DIR}/example_${EXAMPLE}.cc)
                cmake_path(REMOVE_FILENAME EXAMPLEDIR)
                if(EXAMPLEDIR)
                    message("THE PATH of example_${EXAMPLE}.c is " "${EXAMPLEDIR}")
                else()
                    message(SEND_ERROR "example_${EXAMPLE}.c not found. Please check example name!")
                endif()
            endif()
        endif()
    endif()

    if (EXAMPLEDIR AND EXISTS ${EXAMPLEDIR}/prj.conf)
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy ${EXAMPLEDIR}/prj.conf ${c_MENUCONFIG_DIR}/prj.conf
        )
    endif()

    execute_process(
        COMMAND python menuconfig.py --check -d ${d_WORKING_PROJECT_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMAND_ERROR_IS_FATAL ANY
    )

    if(NOT FINAL_IMAGE_DIR)
        set(FINAL_IMAGE_DIR ${d_PLATFORM_PROJECT_DIR})
    else()
        if(NOT EXISTS FINAL_IMAGE_DIR)
            execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${FINAL_IMAGE_DIR})
        endif()
    endif()
endmacro()

# must use macro to define, otherwise project() will not work if use function.
# Usage:
#   ameba_mcu_project_create(<name> <mcu_type>)
macro(ameba_mcu_project_create name mcu_type)
    ameba_set(CMAKE_BUILD_TYPE "")
    ameba_set(_tmp_MCU_TYPE ${mcu_type})

    # set(MCU_PROJECT_INITIALIZED TRUE)

    if (_tmp_MCU_TYPE STREQUAL "km0" OR _tmp_MCU_TYPE STREQUAL "km4" OR _tmp_MCU_TYPE STREQUAL "ca32")
        ameba_set(_tmp_ISA_TYPE "ARM")
    elseif(_tmp_MCU_TYPE STREQUAL "kr4")
        ameba_set(_tmp_ISA_TYPE "RISC-V")
    else()
        message(FATAL_ERROR "unknown mcu type: ${_tmp_MCU_TYPE}")
    endif()

    if (_tmp_ISA_TYPE STREQUAL "ARM")
        ameba_set(_tmp_SDK_NAME "asdk")
    elseif(_tmp_ISA_TYPE STREQUAL "RISC-V")
        ameba_set(_tmp_SDK_NAME "vsdk")
    else()
        message(FATAL_ERROR "unknown isa type: ${_tmp_ISA_TYPE}")
    endif()
    ameba_set_upper(_tmp_SDK_NAME_UPPER ${_tmp_SDK_NAME})

    ameba_set(ASDK_VER ${v_ASDK_VER})
    ameba_set(VSDK_VER ${v_VSDK_VER})

    include (${c_CMAKE_FILES_DIR}/toolchain/ameba-toolchain-${_tmp_SDK_NAME}-${v_${_tmp_SDK_NAME_UPPER}_VER}.cmake)

    project(${name} LANGUAGES C CXX ASM)

    ameba_info("import config: ${d_PLATFORM_PROJECT_DIR}/menuconfig/.config_${mcu_type}")


    configure_file(${c_MENUCONFIG_DIR}/.config_${mcu_type} ${CMAKE_CURRENT_BINARY_DIR}/.config_${mcu_type} COPYONLY)
    import_kconfig("CONFIG" ${CMAKE_CURRENT_BINARY_DIR}/.config_${mcu_type})

    include(${CMAKE_CURRENT_SOURCE_DIR}/mcu_project_config.cmake)

    if(EXAMPLE)
        if(CONFIG_CORE_AS_AP)
            set(d_ENABLE_EXAMPLE TRUE)
        else()
            set(d_ENABLE_EXAMPLE FALSE)
        endif()
    else()
        set(d_ENABLE_EXAMPLE FALSE)
    endif()

    ameba_reset_global_define()

    ameba_set(d_MCU_PROJECT_CREATED TRUE)
endmacro()

# Usage:
#   ameba_mcu_project_init()
function(ameba_mcu_project_init)
    if (NOT d_PLATFORM_PROJECT_CREATED)
        ameba_fatal("Call ameba_mcu_project_create before ameba_mcu_project_init")
    endif()
    ameba_set(d_MCU_PROJECT_INITIALIZED TRUE p_SCOPE both)

    #NOTE: used in CMAKE_V0.2
    ameba_set(d_MCU_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR} p_SCOPE both)
    ameba_set(d_MCU_PROJECT_NAME ${PROJECT_NAME} p_SCOPE both)
    ameba_set_upper(d_MCU_PROJECT_NAME_UPPER ${PROJECT_NAME} p_SCOPE both)
    ameba_set_move(d_MCU_TYPE _tmp_MCU_TYPE p_SCOPE both)
    ameba_set_move(d_ISA_TYPE _tmp_ISA_TYPE p_SCOPE both)
    ameba_set_move(d_SDK_NAME _tmp_SDK_NAME p_SCOPE both)
    ameba_set_upper(d_SDK_NAME_UPPER ${d_SDK_NAME} p_SCOPE both)
    ameba_set_upper(d_MCU_TYPE_UPPER ${d_MCU_TYPE} p_SCOPE both)
    ameba_set(d_SDK_VERSION ${v_${d_SDK_NAME_UPPER}_VER} p_SCOPE both)
    ameba_set(d_TOOLCHAIN_DIR ${SDK_TOOLCHAIN} p_SCOPE both)


    # sub dirs in ${d_MCU_PROJECT_DIR}/
    ameba_set(d_MCU_SDK_DIR ${d_MCU_PROJECT_DIR}/${d_SDK_NAME} p_SCOPE both)
    ameba_set(d_MCU_SRC_DIR ${d_MCU_PROJECT_DIR}/src p_SCOPE both)
    ameba_set(d_MCU_INC_DIR ${d_MCU_PROJECT_DIR}/inc p_SCOPE both)

    # sub dirs in *sdk/
    ameba_set(d_SDK_BUILD_DIR           ${d_MCU_SDK_DIR}/build p_SCOPE both)
    ameba_set(d_SDK_FLASHLOADER_DIR     ${d_MCU_SDK_DIR}/flashloader p_SCOPE both)
    ameba_set(d_SDK_GNU_UTILITY_DIR     ${d_MCU_SDK_DIR}/gnu_utility p_SCOPE both)
    ameba_set(d_SDK_GNU_SCRIPT_DIR      ${d_SDK_GNU_UTILITY_DIR}/gnu_script p_SCOPE both)
    ameba_set(d_SDK_IMGTOOL_FLOADER_DIR ${d_SDK_GNU_UTILITY_DIR}/image_tool_flashloader p_SCOPE both)
    ameba_set(d_SDK_IMAGE_DIR           ${d_MCU_SDK_DIR}/image p_SCOPE both)
    ameba_set(d_SDK_IMAGE_MP_DIR        ${d_MCU_SDK_DIR}/image_mp p_SCOPE both)
    ameba_set(d_SDK_IMAGE_UTILITY_DIR   ${d_MCU_SDK_DIR}/image_utility p_SCOPE both)
    ameba_set(d_SDK_LD_DIR              ${d_MCU_SDK_DIR}/ld p_SCOPE both)
    ameba_set(d_SDK_LD_NS_DIR           ${d_MCU_SDK_DIR}/ld_ns p_SCOPE both)
    ameba_set(d_SDK_LIB_DIR             ${d_MCU_SDK_DIR}/lib p_SCOPE both)
    ameba_set(d_SDK_LIB_APPLICATION_DIR ${d_MCU_SDK_DIR}/lib/application p_SCOPE both)
    ameba_set(d_SDK_LIB_SOC_DIR         ${d_MCU_SDK_DIR}/lib/soc p_SCOPE both)

    # more define
    ameba_set_if(CONFIG_AMEBA_RLS d_AMEBA_RELEASE ON p_ELSE OFF p_SCOPE both)
    if(NOT d_TOOLCHAIN_DIR)
          message(FATAL_ERROR "unknown toolchian dir: ${d_TOOLCHAIN_DIR}")
    endif()

    # dirs define on specific conditions
    ameba_set_if(CONFIG_MP_INCLUDED d_SDK_IMAGE_FOLDER_NAME image_mp p_ELSE image p_SCOPE both)
    ameba_set(d_SDK_IMAGE_TARGET_FOLDER ${d_MCU_SDK_DIR}/${d_SDK_IMAGE_FOLDER_NAME} p_SCOPE both)

    #compatible with CMAKE_V0.1
    ameba_set(IMAGE_TARGET_FOLDER ${d_SDK_IMAGE_TARGET_FOLDER} p_SCOPE both)
    ameba_set_if(CONFIG_MP_INCLUDED BUILD_TYPE MFG p_ELSE NONE p_SCOPE both)
    ameba_set_if(CONFIG_MP_INCLUDED ANALYZE_MP_IMG 1 p_ELSE 0 p_SCOPE both)

    #NOTE: For compatibility with version CMAKE_V0.1, use variable starts with d_ above
    ameba_set(PROJECTDIR ${d_MCU_PROJECT_DIR} p_SCOPE both)
    ameba_set(SRCDIR ${d_MCU_SRC_DIR} p_SCOPE both)
    ameba_set(INCDIR ${d_MCU_INC_DIR} p_SCOPE both)
    ameba_set(LDDIR ${d_SDK_LD_DIR} p_SCOPE both)
    ameba_set(TARGET_LIBSOC_FOLDER ${d_MCU_SDK_DIR}/lib/soc p_SCOPE both)
    ameba_set(TARGET_LIBAPP_FOLDER ${d_MCU_SDK_DIR}/lib/application p_SCOPE both)
    ameba_set(UTILITYDIR ${d_SDK_GNU_UTILITY_DIR} p_SCOPE both)

    #WARNING: For compatibility with version CMAKE_V0.1, use variable starts with v_
    ameba_set(FATFS_VER ${v_FATFS_VER} p_SCOPE both)
    ameba_set(FREERTOS_VER ${v_FREERTOS_VER} p_SCOPE both)
    ameba_set(LITTLEFS_VER ${v_LITTLEFS_VER} p_SCOPE both)
    ameba_set(LWIP_VER ${v_LWIP_VER} p_SCOPE both)
    ameba_set(MBEDTLS_VER ${v_MBEDTLS_VER} p_SCOPE both)
endfunction()

# Usage:
#   ameba_mcu_project_config()
macro(ameba_mcu_project_config)
    ameba_mcu_project_custom_config() # Call user's config function defined in mcu_project_config.cmake

    ameba_info("example status for core: ${d_MCU_TYPE_UPPER}: enable: ${d_ENABLE_EXAMPLE}, example: ${EXAMPLE}, for_core: ${EXAMPLE_FOR_${d_MCU_TYPE_UPPER}}")

    include(${c_COMPONENT_DIR}/build_info/dependency_common.cmake)
    include(${c_COMPONENT_DIR}/build_info/dependency_platform.cmake)
    include(${c_COMPONENT_DIR}/build_info/dependency_mcu.cmake)
    set(d_MCU_DEPS)
    foreach(dep ${p_dependency_list_full})
        string(TOUPPER ${dep} dep_upper)
        if(d_DEP_${dep_upper})
            #TODO: add compile dep after cmake binding with dep's src
            if(TARGET dep_${d_MCU_PROJECT_NAME}_${dep})
                ameba_list_append(d_MCU_DEPS dep_${d_MCU_PROJECT_NAME}_${dep})
            else()
                ameba_fatal("not a TARGET: dep_${d_MCU_PROJECT_NAME}_${dep}")
            endif()
        endif()
    endforeach()

    ####################################

    include(${c_CMAKE_FILES_DIR}/flags/common/compile_options.cmake)
    include(${c_CMAKE_FILES_DIR}/flags/common/link_options.cmake)
    include(${c_CMAKE_FILES_DIR}/flags/${d_PLATFORM_TYPE}/compile_options.cmake) #TODO: remain discuss
    include(${c_CMAKE_FILES_DIR}/flags/${d_PLATFORM_TYPE}/link_options.cmake)
    include(${c_CMAKE_FILES_DIR}/flags/${d_MCU_TYPE}/compile_options.cmake)
    include(${c_CMAKE_FILES_DIR}/flags/${d_MCU_TYPE}/link_options.cmake)
    # COMPILE CONFIG
    add_compile_options(
        "$<$<COMPILE_LANGUAGE:C>:${d_GLOBAL_COMMON_COMPILE_C_OPTIONS}>"         #defined in cmake/flags/common/compile_options.cmake
        "$<$<COMPILE_LANGUAGE:C>:${d_GLOBAL_MCU_COMPILE_C_OPTIONS}>"            #defined in cmake/flags/${d_MCU_TYPE}/compile_options.cmake
        "$<$<COMPILE_LANGUAGE:C>:${d_CUSTOM_PLATFORM_COMPILE_C_OPTIONS}>"       #defined in ${d_PLATFORM_TYPE}/platform_project_config.cmake
        "$<$<COMPILE_LANGUAGE:C>:${d_CUSTOM_MCU_COMPILE_C_OPTIONS}>"            #defined in ${d_PLATFORM_TYPE}/${d_MCU_TYPE}/mcu_project_config.cmake

        "$<$<COMPILE_LANGUAGE:CXX>:${d_GLOBAL_COMMON_COMPILE_CPP_OPTIONS}>"     #defined in cmake/flags/common/compile_options.cmake
        "$<$<COMPILE_LANGUAGE:CXX>:${d_GLOBAL_MCU_COMPILE_CPP_OPTIONS}>"        #defined in cmake/flags/${d_MCU_TYPE}/compile_options.cmake
        "$<$<COMPILE_LANGUAGE:CXX>:${d_CUSTOM_PLATFORM_COMPILE_CPP_OPTIONS}>"   #defined in ${d_PLATFORM_TYPE}/platform_project_config.cmake
        "$<$<COMPILE_LANGUAGE:CXX>:${d_CUSTOM_MCU_COMPILE_CPP_OPTIONS}>"        #defined in ${d_PLATFORM_TYPE}/${d_MCU_TYPE}/mcu_project_config.cmake

        "$<$<COMPILE_LANGUAGE:ASM>:${d_GLOBAL_COMMON_COMPILE_ASM_OPTIONS}>"     #defined in cmake/flags/common/compile_options.cmake
        "$<$<COMPILE_LANGUAGE:ASM>:${d_GLOBAL_MCU_COMPILE_ASM_OPTIONS}>"        #defined in cmake/flags/${d_MCU_TYPE}/compile_options.cmake
        "$<$<COMPILE_LANGUAGE:ASM>:${d_CUSTOM_PLATFORM_COMPILE_ASM_OPTIONS}>"   #defined in ${d_PLATFORM_TYPE}/platform_project_config.cmake
        "$<$<COMPILE_LANGUAGE:ASM>:${d_CUSTOM_MCU_COMPILE_ASM_OPTIONS}>"        #defined in ${d_PLATFORM_TYPE}/${d_MCU_TYPE}/mcu_project_config.cmake
    )

    add_compile_definitions(
        ${d_GLOBAL_COMMON_COMPILE_DEFINES}
        ${d_GLOBAL_MCU_COMPILE_DEFINES}
        ${d_CUSTOM_PLATFORM_COMPILE_DEFINES}
    )

    add_library(d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_ALL_DEPS INTERFACE)
    target_link_options(d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_ALL_DEPS INTERFACE
        ${d_GLOBAL_COMMON_LINK_OPTIONS}     #defined in cmake/flags/common/link_options.cmake
        ${d_GLOBAL_PLATFORM_LINK_OPTIONS}   #defined in cmake/flags/${d_PLATFORM_TYPE}/link_options.cmake
        ${d_GLOBAL_MCU_LINK_OPTIONS}        #defined in cmake/flags/${d_MCU_TYPE}/link_options.cmake
        ${d_CUSTOM_PLATFORM_LINK_OPTIONS}   #defined in ${d_PLATFORM_TYPE}/platform_project_config.cmake
        ${d_CUSTOM_MCU_LINK_OPTIONS}        #defined in ${d_PLATFORM_TYPE}/${d_MCU_TYPE}/mcu_project_config.cmake
    )

    # d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_ALL_DEPS is global build info
    target_link_libraries(d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_ALL_DEPS INTERFACE ${d_MCU_DEPS})
    target_include_directories(d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_ALL_DEPS INTERFACE ${d_GLOBAL_MCU_INCLUDE_DIRECTORIES})

    add_library(d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_LINK_OPTIONS INTERFACE)
    target_link_options(d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_LINK_OPTIONS INTERFACE
        ${d_GLOBAL_COMMON_LINK_OPTIONS}     #defined in cmake/flags/common/link_options.cmake
        ${d_GLOBAL_PLATFORM_LINK_OPTIONS}   #defined in cmake/flags/${d_PLATFORM_TYPE}/link_options.cmake
        ${d_GLOBAL_MCU_LINK_OPTIONS}        #defined in cmake/flags/${d_MCU_TYPE}/link_options.cmake
        ${d_CUSTOM_PLATFORM_LINK_OPTIONS}   #defined in ${d_PLATFORM_TYPE}/platform_project_config.cmake
        ${d_CUSTOM_MCU_LINK_OPTIONS}        #defined in ${d_PLATFORM_TYPE}/${d_MCU_TYPE}/mcu_project_config.cmake
    )

    add_library(d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_INCLUDE_DIRECTORIES INTERFACE)
    ameba_target_get_include_directories(d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_ALL_DEPS tmp_A_INCLUDE_DIRS)
    target_include_directories(d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_INCLUDE_DIRECTORIES INTERFACE ${tmp_A_INCLUDE_DIRS})

    # add_library(d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_LIBRARIES INTERFACE)
    ameba_target_get_link_libraries(d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_ALL_DEPS libs)
    # target_link_libraries(d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_LIBRARIES INTERFACE ${libs})
    set(d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_LIBRARIES ${libs}) #if use interface target, than whole-archive libs may not work.

    set(d_LIST_${d_MCU_PROJECT_NAME_UPPER}_LOADER_LIBRARIES)
    set(d_LIST_${d_MCU_PROJECT_NAME_UPPER}_LOADER_DEPS)
    set(d_LIST_${d_MCU_PROJECT_NAME_UPPER}_NO_LOADER_LIBRARIES)
    set(d_LIST_${d_MCU_PROJECT_NAME_UPPER}_NO_LOADER_DEPS)
    set(d_LIST_${d_MCU_PROJECT_NAME_UPPER}_SECURE_LIBRARIES)
    set(d_LIST_${d_MCU_PROJECT_NAME_UPPER}_SECURE_DEPS)
    foreach(dep IN LISTS d_MCU_DEPS)
        ameba_list_contains(p_dependency_list_non_loader ${dep} flag p_PREFIX dep_${d_MCU_PROJECT_NAME}_)
        if(flag)
            ameba_target_get_link_libraries(${dep} libs)
            list(APPEND d_LIST_${d_MCU_PROJECT_NAME_UPPER}_NO_LOADER_LIBRARIES ${libs})
            list(APPEND d_LIST_${d_MCU_PROJECT_NAME_UPPER}_NO_LOADER_DEPS ${dep})
        endif()

        ameba_list_contains(p_dependency_list_loader ${dep} flag p_PREFIX dep_${d_MCU_PROJECT_NAME}_)
        if(flag)
            ameba_target_get_link_libraries(${dep} libs)
            list(APPEND d_LIST_${d_MCU_PROJECT_NAME_UPPER}_LOADER_LIBRARIES ${libs})
            list(APPEND d_LIST_${d_MCU_PROJECT_NAME_UPPER}_LOADER_DEPS ${dep})
        endif()

        ameba_list_contains(p_dependency_list_secure ${dep} flag p_PREFIX dep_${d_MCU_PROJECT_NAME}_)
        if(flag)
            ameba_target_get_link_libraries(${dep} libs)
            list(APPEND d_LIST_${d_MCU_PROJECT_NAME_UPPER}_SECURE_LIBRARIES ${libs})
            list(APPEND d_LIST_${d_MCU_PROJECT_NAME_UPPER}_SECURE_DEPS ${dep})
        endif()

        # maybe one component has a global macro, add it to the global project.
        get_target_property(defs ${dep} INTERFACE_COMPILE_DEFINITIONS)
        if(defs)
            add_compile_definitions(${defs})
        endif()
    endforeach()

    ameba_debug("non loader libraries: ${d_LIST_${d_MCU_PROJECT_NAME_UPPER}_NO_LOADER_LIBRARIES}")
    ameba_debug("loader libraries: ${d_LIST_${d_MCU_PROJECT_NAME_UPPER}_LOADER_LIBRARIES}")

    #NOTE: For compatibility with version CMAKE_V0.1
    add_library(ameba_interface_${PROJECT_NAME} INTERFACE)
    target_include_directories(ameba_interface_${PROJECT_NAME} INTERFACE ${tmp_A_INCLUDE_DIRS})
    target_link_options(ameba_interface_${PROJECT_NAME} INTERFACE
        ${d_GLOBAL_COMMON_LINK_OPTIONS}     #defined in cmake/flags/common/link_options.cmake
        ${d_GLOBAL_PLATFORM_LINK_OPTIONS}   #defined in cmake/flags/${d_PLATFORM_TYPE}/link_options.cmake
        ${d_GLOBAL_MCU_LINK_OPTIONS}        #defined in cmake/flags/${d_MCU_TYPE}/link_options.cmake
        ${d_CUSTOM_PLATFORM_LINK_OPTIONS}   #defined in ${d_PLATFORM_TYPE}/platform_project_config.cmake
        ${d_CUSTOM_MCU_LINK_OPTIONS}        #defined in ${d_PLATFORM_TYPE}/${d_MCU_TYPE}/mcu_project_config.cmake
    )

    ####################################

    add_custom_target(
        build_info_${PROJECT_NAME}
        ALL
        COMMENT "generate build_info.h"
        COMMAND ${CMAKE_COMMAND} -DPROJECTDIR=${PROJECTDIR} -DCMAKE_FILES_DIR=${c_CMAKE_FILES_DIR} -DCONFIG_TOOLCHAIN_ARM_GCC=${CONFIG_TOOLCHAIN_ARM_GCC} -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -P ${c_CMAKE_FILES_DIR}/buildinfo.cmake
    )
    set_property(TARGET build_info_${PROJECT_NAME}
        PROPERTY ADDITIONAL_CLEAN_FILES
        ${PROJECTDIR}/inc/build_info.h
    )

    ####################################
endmacro()
