include_guard(GLOBAL)

# Top soc project create and init, name value maybe amebaxxx
# Usage:
#   ameba_soc_project_create(amebasmart)

function(ameba_soc_name_to_camel_case input output)
    # Check if input starts with 'ameba'
    string(REGEX MATCH "^ameba" is_valid "${input}")
    if(NOT is_valid)
        message(WARNING "Input does not start with 'ameba': ${input}")
        set(${output} "${input}" PARENT_SCOPE)
        return()
    endif()

    # Split the string into 'ameba' and the suffix (remaining part)
    string(REGEX REPLACE "^ameba(.*)" "\\1" suffix "${input}")

    # Process the first part: 'ameba' -> 'Ameba'
    string(SUBSTRING "ameba" 0 1 first_char)
    string(TOUPPER "${first_char}" first_char)
    string(SUBSTRING "ameba" 1 -1 rest)
    string(TOLOWER "${rest}" rest)
    set(prefix "${first_char}${rest}")

    # Process the suffix (capitalize first letter, keep the rest as-is)
    if(suffix)
        string(SUBSTRING "${suffix}" 0 1 suffix_first)
        string(TOUPPER "${suffix_first}" suffix_first)
        string(SUBSTRING "${suffix}" 1 -1 suffix_rest)
        set(suffix_camel "${suffix_first}${suffix_rest}")
    else()
        set(suffix_camel "")
    endif()

    # Combine results
    set(${output} "${prefix}${suffix_camel}" PARENT_SCOPE)
endfunction()

macro(ameba_soc_project_create name)
    ###############################################################################
    ## vscode extension support
    #OPTIMIZE: move these to ameba_soc_project_create?
    #NOTICE: Cache variable EXAMPLE will be deleted at the end.
    set(EXAMPLE ${EXAMPLE} CACHE STRING "Example Name. If EXAMPLE is specified by user,\
    the floder named <EXAMPLE> will be searched from components/example.\
    e.g., EXAPMLE=ssl_client. Default example name is NULL")

    #NOTICE: Cache variable USER_SRC_DIR will be deleted at the end.
    set(USER_SRC_DIR ${USER_SRC_DIR} CACHE PATH "User source code floader containing main.c\
    and CMakeLists.txt. If USER_SRC_DIR is specified by user, the project will add this floader\
    as a subdirectory. USER_SRC_DIR can be a absolute path or a path relative to 'BASEDIR', \
    which is parent floder of component. e.g., USER_SRC_DIR=/home/user/src, USER_SRC_DIR=component/example/xxx/src.\
    Default source code floader is amebadeplus_gcc_project/project/project_km4/src")

    #NOTICE: Cache variable TOOLCHAIN_DIR will be deleted at the end.
    set(TOOLCHAIN_DIR ${TOOLCHAIN_DIR} CACHE PATH "Toolchain installation path. If TOOLCHAIN_DIR is specified\
    by user, cross-compile toolchain will be searched in <TOOLCHAIN_DIR>. Default toolchain installation path is\
    '/opt/rtk-toolchain' in linux and 'C:/rtk-toolchain' in windows.")

    #NOTICE: Cache variable FINAL_IMAGE_DIR will be deleted at the end.
    set(FINAL_IMAGE_DIR ${FINAL_IMAGE_DIR} CACHE PATH "Final images path, i.e., xxx_boot_all.bin, ota_all.bin \
    and xxx_app.bin final path. If FINAL_IMAGE_DIR is specified by user, final images will be copied to \
    <FINAL_IMAGE_DIR> after building. Default final image directory is project root path, \
    where the current CMakeLists.txt is located.")
    ###############################################################################
    set(oneValueArgs
        p_OUTPUT_MCU_PROJECT  # Specific which mcu project you want output files
    )
    cmake_parse_arguments(ARG "" "${oneValueArgs}" "" ${ARGN})
    if(ARG_p_OUTPUT_MCU_PROJECT)
        ameba_set(c_OUTPUT_MCU_PROJECT ${ARG_p_OUTPUT_MCU_PROJECT})
    else()
        ameba_fatal("Output mcu project MUST be set when call ameba_soc_project_create")
    endif()

    ameba_set(c_CMAKE_BUILD_DIR ${CMAKE_BINARY_DIR})
    ameba_set(c_SOC_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    ameba_set(c_SOC_TYPE ${name})
    ameba_set_upper(c_SOC_TYPE_UPPER ${c_SOC_TYPE})
    ameba_soc_name_to_camel_case(${c_SOC_TYPE} c_SOC_TYPE_CAMEL)
    ameba_set(c_CMPT_SOC_DIR ${c_BASEDIR}/component/soc/${c_SOC_TYPE})

    ameba_set(c_WORKING_PROJECT_DIR ${CMAKE_BINARY_DIR}/..)
    ameba_set(c_MENUCONFIG_DIR ${c_WORKING_PROJECT_DIR}/menuconfig)

    ameba_set(c_OUTPUT_MCU_PROJECT_DIR ${c_SOC_PROJECT_DIR}/project_${ARG_p_OUTPUT_MCU_PROJECT})
    if (NOT EXISTS ${c_OUTPUT_MCU_PROJECT_DIR})
        ameba_fatal("Output mcu project not exist: ${c_OUTPUT_MCU_PROJECT_DIR}")
    endif()

    ameba_set(CMAKE_BUILD_TYPE "")

    #NOTE: hook macro defined by user to add custom config
    # ameba_soc_project_custom_config()

    #EXAMPLE can be a absolute path, relative path where contain a CMakeLists and source files, or can be a specific example name
    if (EXAMPLE)
        if(IS_ABSOLUTE EXAMPLE)
            set(EXAMPLEDIR ${EXAMPLE})
        else()
            cmake_path(ABSOLUTE_PATH EXAMPLE BASE_DIRECTORY ${c_WORKING_PROJECT_DIR} NORMALIZE OUTPUT_VARIABLE OUTPUT_EXAMPLE)
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
        COMMAND python menuconfig.py --check -d ${c_WORKING_PROJECT_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMAND_ERROR_IS_FATAL ANY
    )

    if(NOT FINAL_IMAGE_DIR)
        set(FINAL_IMAGE_DIR ${c_WORKING_PROJECT_DIR})
    else()
        if(NOT EXISTS FINAL_IMAGE_DIR)
            execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${FINAL_IMAGE_DIR})
        endif()
    endif()

    ameba_set(ASDK_VER ${v_ASDK_VER}) #NOTE: Only used in ameba-toolchain-asdk-${v_ASDK_VER}.cmake
    # must include toolchain before project(xx LANGUAGES C CXX ASM)
    if(NOT USE_SYSTEM_DEFAULT_TOOLCHAIN)
        include (${c_CMAKE_FILES_DIR}/toolchain/ameba-toolchain-asdk-${v_ASDK_VER}.cmake)
    endif()

    ameba_set_if(c_VERBOSE_MAKEFILE CMAKE_VERBOSE_MAKEFILE ON)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    add_library(g_PROJECT_CONFIG INTERFACE)

    configure_file(${c_MENUCONFIG_DIR}/.config ${CMAKE_CURRENT_BINARY_DIR}/.config COPYONLY)
    import_kconfig("CONFIG" ${CMAKE_CURRENT_BINARY_DIR}/.config)
    ameba_set_if(CONFIG_MP_INCLUDED c_SDK_IMAGE_FOLDER_NAME image_mp p_ELSE image)
    ameba_set(c_SOC_PROJECT_CREATED TRUE)
endmacro()

# Top soc project create and init, name value maybe amebaxxx
macro(ameba_soc_project_exit)
    set(DAILY_BUILD 0 CACHE STRING "code analysis argument" FORCE)
    unset(EXAMPLE CACHE)
    unset(USER_SRC_DIR CACHE)
    unset(TOOLCHAIN_DIR CACHE)
    unset(FINAL_IMAGE_DIR CACHE)
endmacro()

# must use macro to define, otherwise project() will not work if use function.
# Usage:
#   ameba_mcu_project_create(<name> <mcu_type>)
macro(ameba_mcu_project_create name mcu_type)
    if (NOT c_SOC_PROJECT_CREATED)
        ameba_fatal("Call ameba_soc_project_create before ameba_mcu_project_create")
    endif()

    if(${mcu_type} STREQUAL "kr4")
        ameba_set(c_ISA_TYPE "RISC-V")
        ameba_set(c_SDK_NAME "vsdk")
    else()
        ameba_set(c_ISA_TYPE "ARM") #mcu_type is one of km0/km4/ca32
        ameba_set(c_SDK_NAME "asdk")
    endif()

    ameba_set_upper(c_SDK_NAME_UPPER ${c_SDK_NAME})
    ameba_set_upper(c_MCU_TYPE_UPPER ${mcu_type})

    ameba_set(ASDK_VER ${v_ASDK_VER})
    ameba_set(VSDK_VER ${v_VSDK_VER})
    include(${c_CMAKE_FILES_DIR}/toolchain/ameba-toolchain-${c_SDK_NAME}-${v_${c_SDK_NAME_UPPER}_VER}.cmake)

    project(${name} LANGUAGES C CXX ASM)
    ameba_set_if(c_VERBOSE_MAKEFILE CMAKE_VERBOSE_MAKEFILE ON)


    if(EXISTS ${c_MENUCONFIG_DIR}/.config_${mcu_type})
        configure_file(${c_MENUCONFIG_DIR}/.config_${mcu_type} ${CMAKE_CURRENT_BINARY_DIR}/.config_${mcu_type} COPYONLY)
        ameba_set(c_MCU_KCONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/.config_${mcu_type})
    elseif(EXISTS ${c_MENUCONFIG_DIR}/.config_${name})
        configure_file(${c_MENUCONFIG_DIR}/.config_${name} ${CMAKE_CURRENT_BINARY_DIR}/.config_${name} COPYONLY)
        ameba_set(c_MCU_KCONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/.config_${name})
    else()
        ameba_fatal("Could not find kconfig file")
    endif()
    ameba_info("import config: ${c_MCU_KCONFIG_FILE}")
    import_kconfig("CONFIG" ${c_MCU_KCONFIG_FILE})

    #NOTE: Determine whether build example for this mcu project
    if(EXAMPLE) #By default, MCU as ap run example
        if(CONFIG_CORE_AS_AP)
            set(c_ENABLE_EXAMPLE TRUE)
        else()
            set(c_ENABLE_EXAMPLE FALSE)
        endif()
    else()
        set(c_ENABLE_EXAMPLE FALSE)
    endif()

    if (c_ENABLE_EXAMPLE)
        message("build example for mcu ${c_MCU_TYPE_UPPER}: ${EXAMPLE}")
    endif()


    #NOTE:
    ameba_set(c_MCU_TYPE ${mcu_type})
    ameba_set_upper(c_MCU_TYPE_UPPER ${c_MCU_TYPE})
    ameba_set(c_SDK_NAME ${c_SDK_NAME})
    ameba_set_upper(c_SDK_NAME_UPPER ${c_SDK_NAME})

    ameba_set(c_MCU_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

    ameba_set(c_MCU_PROJECT_NAME ${PROJECT_NAME})
    ameba_set_upper(c_MCU_PROJECT_NAME_UPPER ${PROJECT_NAME})
    ameba_set(c_MCU_PROJECT_CREATED TRUE)

    ameba_set(c_SDK_VERSION ${v_${c_SDK_NAME_UPPER}_VER})

    # sub dirs in ${c_MCU_PROJECT_DIR}/
    ameba_set(c_MCU_SDK_DIR ${c_MCU_PROJECT_DIR}/${c_SDK_NAME})
    ameba_set(c_MCU_SRC_DIR ${c_MCU_PROJECT_DIR}/src)
    ameba_set(c_MCU_INC_DIR ${c_MCU_PROJECT_DIR}/inc)

    # sub dirs in *sdk/
    ameba_set(c_SDK_BUILD_DIR           ${c_MCU_SDK_DIR}/build)
    ameba_set(c_SDK_FLASHLOADER_DIR     ${c_MCU_SDK_DIR}/flashloader)
    ameba_set(c_SDK_GNU_UTILITY_DIR     ${c_MCU_SDK_DIR}/gnu_utility)
    ameba_set(c_SDK_GNU_SCRIPT_DIR      ${c_SDK_GNU_UTILITY_DIR}/gnu_script)
    ameba_set(c_SDK_IMGTOOL_FLOADER_DIR ${c_SDK_GNU_UTILITY_DIR}/image_tool_flashloader)
    ameba_set(c_SDK_FLOADER_BIN_DIR     ${c_SDK_IMGTOOL_FLOADER_DIR}/${c_SOC_TYPE}_acut)
    ameba_set(c_SDK_IMAGE_DIR           ${c_MCU_SDK_DIR}/image)
    ameba_set(c_SDK_IMAGE_MP_DIR        ${c_MCU_SDK_DIR}/image_mp)
    ameba_set(c_SDK_IMAGE_UTILITY_DIR   ${c_MCU_SDK_DIR}/img_utility)
    ameba_set(c_SDK_LD_DIR              ${c_MCU_SDK_DIR}/ld)
    ameba_set(c_SDK_LD_NS_DIR           ${c_MCU_SDK_DIR}/ld_ns)
    ameba_set(c_SDK_LIB_DIR             ${c_MCU_SDK_DIR}/lib)
    ameba_set(c_SDK_LIB_APPLICATION_DIR ${c_MCU_SDK_DIR}/lib/application)
    ameba_set(c_SDK_LIB_APP_DIR         ${c_MCU_SDK_DIR}/lib/application)
    ameba_set(c_SDK_LIB_SOC_DIR         ${c_MCU_SDK_DIR}/lib/soc)

    ameba_set(c_SDK_ROM_SYMBOL_GEN_SCRIPT       ${c_SDK_IMAGE_UTILITY_DIR}/export_rom_symbol.py)
    ameba_set(c_SDK_ROM_SYMBOL_S_GEN_SCRIPT     ${c_SDK_IMAGE_UTILITY_DIR}/export_rom_symbol_s.py)
    ameba_set(c_SDK_ROM_TOTAL_SIZE_SCRIPT       ${c_SDK_IMAGE_UTILITY_DIR}/total_rom_size.py)
    ameba_set(c_SDK_ROM_CODE_ANALYZE_SCRIPT     ${c_SDK_IMAGE_UTILITY_DIR}/code_analyze.py)
    ameba_set(c_SDK_EXTRACT_LD_SCRIPT           ${c_SDK_IMAGE_UTILITY_DIR}/extract_ld_vars.sh)

    if (CONFIG_CA32_FREERTOS_V10_2_1_SMP)
        set(v_FREERTOS_VER v10.2.1)
        set(c_FREERTOS_DIR_SUFFIX _smp)
    endif()
    if (CONFIG_CA32_FREERTOS_V11_1_0)
        set(v_FREERTOS_VER v11.1.0)
    endif()

    ameba_reset_global_define()
    ameba_set_if(CONFIG_MP_INCLUDED BUILD_TYPE MFG p_ELSE NONE p_SCOPE both)
    ameba_set_if(CONFIG_MP_INCLUDED ANALYZE_MP_IMG 1 p_ELSE 0 p_SCOPE both)

    # dirs define on specific conditions
    ameba_set(c_SDK_IMAGE_TARGET_DIR ${c_MCU_SDK_DIR}/${c_SDK_IMAGE_FOLDER_NAME})

    if(EXISTS ${c_SDK_IMAGE_TARGET_DIR})
        file(GLOB FILES_TO_REMOVE "${c_SDK_IMAGE_TARGET_DIR}/*")
        if(FILES_TO_REMOVE)
            ameba_info("clear old files in ${c_SDK_IMAGE_TARGET_DIR}")
            file(REMOVE_RECURSE ${FILES_TO_REMOVE})
        endif()
    endif()

    # Handle compile configs
    include(${c_CMAKE_FILES_DIR}/flags/common/compile_options.cmake)
    include(${c_CMAKE_FILES_DIR}/flags/common/link_options.cmake)
    include(${c_CMAKE_FILES_DIR}/flags/${c_MCU_TYPE}/compile_options.cmake)
    include(${c_CMAKE_FILES_DIR}/flags/${c_MCU_TYPE}/link_options.cmake)

    add_compile_options(
        "$<$<COMPILE_LANGUAGE:C>:${c_GLOBAL_COMMON_COMPILE_C_OPTIONS}>"         #defined in cmake/flags/common/compile_options.cmake
        "$<$<COMPILE_LANGUAGE:C>:${c_GLOBAL_MCU_COMPILE_C_OPTIONS}>"            #defined in cmake/flags/${c_MCU_TYPE}/compile_options.cmake

        "$<$<COMPILE_LANGUAGE:CXX>:${c_GLOBAL_COMMON_COMPILE_CPP_OPTIONS}>"     #defined in cmake/flags/common/compile_options.cmake
        "$<$<COMPILE_LANGUAGE:CXX>:${c_GLOBAL_MCU_COMPILE_CPP_OPTIONS}>"        #defined in cmake/flags/${c_MCU_TYPE}/compile_options.cmake

        "$<$<COMPILE_LANGUAGE:ASM>:${c_GLOBAL_COMMON_COMPILE_ASM_OPTIONS}>"     #defined in cmake/flags/common/compile_options.cmake
        "$<$<COMPILE_LANGUAGE:ASM>:${c_GLOBAL_MCU_COMPILE_ASM_OPTIONS}>"        #defined in cmake/flags/${c_MCU_TYPE}/compile_options.cmake
    )

    add_compile_definitions(
        ${c_GLOBAL_COMMON_COMPILE_DEFINES}
        ${c_GLOBAL_MCU_COMPILE_DEFINES}
    )

    set(c_MCU_PROJ_CONFIG g_PUBLIC_BUILD_INTERFACE_${c_MCU_PROJECT_NAME_UPPER})
    add_library(${c_MCU_PROJ_CONFIG} INTERFACE)
    set_property(TARGET ${c_MCU_PROJ_CONFIG} APPEND PROPERTY project_dir "${c_MCU_PROJECT_DIR}")

    target_link_options(${c_MCU_PROJ_CONFIG} INTERFACE
        ${c_GLOBAL_COMMON_LINK_OPTIONS}     #defined in cmake/flags/common/link_options.cmake
        ${c_GLOBAL_MCU_LINK_OPTIONS}        #defined in cmake/flags/${d_MCU_TYPE}/link_options.cmake
    )

    target_include_directories(${c_MCU_PROJ_CONFIG} INTERFACE
        ${c_GLOBAL_MCU_INCLUDE_DIRECTORIES}
    )

    set_property(TARGET g_PROJECT_CONFIG APPEND PROPERTY mcu_project_list ${c_MCU_PROJECT_NAME})
    set_property(TARGET g_PROJECT_CONFIG APPEND PROPERTY ${c_MCU_PROJECT_NAME}_config "${c_MCU_PROJ_CONFIG}")
    if (${c_MCU_PROJECT_NAME} STREQUAL ${c_OUTPUT_MCU_PROJECT})
        set_property(TARGET g_PROJECT_CONFIG APPEND PROPERTY image_output_dir "${c_SDK_IMAGE_TARGET_DIR}")
    endif()

    set(c_BUILD_INFO build_info_${PROJECT_NAME})
    add_custom_target(
        ${c_BUILD_INFO}
        ALL
        COMMENT "generate build_info.h"
        COMMAND ${CMAKE_COMMAND} -DPROJECTDIR=${c_MCU_PROJECT_DIR} -DCMAKE_FILES_DIR=${c_CMAKE_FILES_DIR} -DCONFIG_TOOLCHAIN_ARM_GCC=${CONFIG_TOOLCHAIN_ARM_GCC} -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -P ${c_CMAKE_FILES_DIR}/buildinfo.cmake
        BYPRODUCTS ${c_MCU_INC_DIR}/build_info.h
    )
    set_property(TARGET ${c_BUILD_INFO} PROPERTY ADDITIONAL_CLEAN_FILES ${c_MCU_PROJECT_DIR}/inc/build_info.h)
    set(c_CURRENT_IMAGE_IS_ROM FALSE) #Flag to identify whether current image is rom, updated in ameba_add_image
    ameba_add_empty_object()
endmacro()

function(ameba_firmware_package output_app_name)
    if (CONFIG_FULLMAC_MODE)
        ameba_info("Skip firmware package when CONFIG_FULLMAC_MODE enabled")
        return()
    endif()
    set(multiValueArgs
        p_CUSTOM_VARIABLES #Variables to be transformed to postbuild.cmake directly
        p_MCU_PROJECTS
    )

    cmake_parse_arguments(ARG "" "" "${multiValueArgs}" ${ARGN})
    set(custom_variables)
    foreach(var ${ARG_p_CUSTOM_VARIABLES})
        ameba_list_append(custom_variables -D${var}=${${var}})
    endforeach()

    # Merge image2/image3 to app.bin
    set(c_APP_BINARY_NAME ${output_app_name})
    ameba_get_image_output_dir(c_IMAGE_OUTPUT_DIR)

    set(postbuild_targets)
    set(image1_all_files)
    set(image2_all_files)
    set(image3_all_files)
    foreach(mcu_proj ${ARG_p_MCU_PROJECTS})
        ameba_get_image_target_name(image2 target_name p_MCU_PROJECT_NAME ${mcu_proj})
        ameba_list_append_if(target_name postbuild_targets ${target_name}_postbuild)

        ameba_get_image_target_name(image3 target_name p_MCU_PROJECT_NAME ${mcu_proj})
        ameba_list_append_if(target_name postbuild_targets ${target_name}_postbuild)

        ameba_get_image_all_path(image1 all_name p_MCU_PROJECT_NAME ${mcu_proj})
        ameba_list_append_if(all_name image1_all_files ${all_name})

        ameba_get_image_all_path(image2 all_name p_MCU_PROJECT_NAME ${mcu_proj})
        ameba_list_append_if(all_name image2_all_files ${all_name})

        ameba_get_image_all_path(image3 all_name p_MCU_PROJECT_NAME ${mcu_proj})
        ameba_list_append_if(all_name image3_all_files ${all_name})
    endforeach()

    string(REPLACE ";" "\\;" image1_all_files "${image1_all_files}")
    string(REPLACE ";" "\\;" image2_all_files "${image2_all_files}")
    string(REPLACE ";" "\\;" image3_all_files "${image3_all_files}")
    ameba_info("image all list: image1: ${image1_all_files}, image2: ${image2_all_files}, image3: ${image3_all_files}")
    add_custom_target(firmware_package ALL
        COMMAND ${CMAKE_COMMAND}
            # common variables
            -Dc_BASEDIR=${c_BASEDIR}  # dir of sdk's root
            -Dc_CMAKE_FILES_DIR=${c_CMAKE_FILES_DIR} # dir of cmake scripts
            -Dc_MCU_KCONFIG_FILE=${CMAKE_CURRENT_BINARY_DIR}/.config
            -Dc_SOC_PROJECT_DIR=${c_SOC_PROJECT_DIR} # dir of soc project
            -Dc_SDK_IMAGE_TARGET_DIR=${c_SDK_IMAGE_TARGET_DIR} # dir of image output
            -Dc_IMAGE_OUTPUT_DIR=${c_IMAGE_OUTPUT_DIR}
            -Dc_APP_BINARY_NAME=${c_APP_BINARY_NAME}
            -Dc_SDK_IMAGE_FOLDER_NAME=${c_SDK_IMAGE_FOLDER_NAME}
            -Dc_IMAGE1_ALL_FILES="${image1_all_files}" #NOTE: transfer as list
            -Dc_IMAGE2_ALL_FILES="${image2_all_files}" #NOTE: transfer as list
            -Dc_IMAGE3_ALL_FILES="${image3_all_files}" #NOTE: transfer as list

            # user's variables
            -DFINAL_IMAGE_DIR=${FINAL_IMAGE_DIR}
            -DANALYZE_MP_IMG=${ANALYZE_MP_IMG}

            # special variables for current image
            ${custom_variables}

            -P ${c_SOC_PROJECT_DIR}/postbuild.cmake
        DEPENDS
            ${postbuild_targets}
    )
endfunction()

function(ameba_soc_project_check)
    ameba_info("soc project : ${PROJECT_NAME}, soc type: ${c_SOC_TYPE}|${c_SOC_TYPE_CAMEL}")
    if(NOT EXISTS ${c_AXF2BIN_SCRIPT})
        message(FATAL_ERROR "Please update $SDK/tools repo to latest")
    endif()

    ameba_execute_process(COMMAND ${op_USAGE})
endfunction()

function(ameba_mcu_project_check)
    ameba_info("mcu project: ${PROJECT_NAME}, mcu: ${c_MCU_TYPE}|${c_MCU_TYPE_UPPER}, isa: ${c_ISA_TYPE}, sdk: ${c_SDK_NAME}, dir: ${c_MCU_PROJECT_DIR}")
    ameba_info("freertos: ${c_FREERTOS_DIR}")
endfunction()
