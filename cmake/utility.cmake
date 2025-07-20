include_guard(GLOBAL)
include(${CMAKE_CURRENT_LIST_DIR}/utility_base.cmake)
if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/utility_internal.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/utility_internal.cmake)
endif()

# import_kconfig(<prefix> <kconfig_fragment> [<keys>] [TARGET <target>])
#
# Parse a KConfig fragment (typically with extension .config) and
# introduce all the symbols that are prefixed with 'prefix' into the
# CMake namespace. List all created variable names in the 'keys'
# output variable if present.
#
# <prefix>          : symbol prefix of settings in the Kconfig fragment.
# <kconfig_fragment>: absolute path to the config fragment file.
# <keys>            : output variable which will be populated with variable
#                     names loaded from the kconfig fragment.
# TARGET <target>   : set all symbols on <target> instead of adding them to the
#                     CMake namespace.
function(import_kconfig prefix kconfig_fragment)
  cmake_parse_arguments(IMPORT_KCONFIG "" "TARGET" "" ${ARGN})
  file(
    STRINGS
    ${kconfig_fragment}
    DOT_CONFIG_LIST
    ENCODING "UTF-8"
  )

  foreach (LINE ${DOT_CONFIG_LIST})
    if("${LINE}" MATCHES "^(${prefix}[^=]+)=([ymn]|.+$)")
      # Matched a normal value assignment, like: CONFIG_NET_BUF=y
      # Note: if the value starts with 'y', 'm', or 'n', then we assume it's a
      # bool or tristate (we don't know the type from <kconfig_fragment> alone)
      # and we only match the first character. This is to align with Kconfiglib.
      set(CONF_VARIABLE_NAME "${CMAKE_MATCH_1}")
      set(CONF_VARIABLE_VALUE "${CMAKE_MATCH_2}")
    elseif("${LINE}" MATCHES "^# (${prefix}[^ ]+) is not set")
      # Matched something like: # CONFIG_FOO is not set
      # This is interpreted as: CONFIG_FOO=n
      set(CONF_VARIABLE_NAME "${CMAKE_MATCH_1}")
      set(CONF_VARIABLE_VALUE "n")
    else()
      # Ignore this line.
      # Note: we also ignore assignments which don't have the desired <prefix>.
      continue()
    endif()

    # If the provided value is n, then the corresponding CMake variable or
    # target property will be unset.
    if("${CONF_VARIABLE_VALUE}" STREQUAL "n")
      if(DEFINED IMPORT_KCONFIG_TARGET)
        set_property(TARGET ${IMPORT_KCONFIG_TARGET} PROPERTY "${CONF_VARIABLE_NAME}")
      else()
        unset("${CONF_VARIABLE_NAME}" PARENT_SCOPE)
      endif()
      list(REMOVE_ITEM keys "${CONF_VARIABLE_NAME}")
      continue()
    endif()

    # Otherwise, the variable/property will be set to the provided value.
    # For string values, we also remove the surrounding quotation marks.
    if("${CONF_VARIABLE_VALUE}" MATCHES "^\"(.*)\"$")
      set(CONF_VARIABLE_VALUE ${CMAKE_MATCH_1})
    endif()

    if(DEFINED IMPORT_KCONFIG_TARGET)
      set_property(TARGET ${IMPORT_KCONFIG_TARGET} PROPERTY "${CONF_VARIABLE_NAME}" "${CONF_VARIABLE_VALUE}")
    else()
      set("${CONF_VARIABLE_NAME}" "${CONF_VARIABLE_VALUE}" PARENT_SCOPE)
    endif()
    list(APPEND keys "${CONF_VARIABLE_NAME}")
  endforeach()

  if(DEFINED IMPORT_KCONFIG_TARGET)
    set_property(TARGET ${IMPORT_KCONFIG_TARGET} PROPERTY "kconfigs" "${keys}")
  endif()

  list(LENGTH IMPORT_KCONFIG_UNPARSED_ARGUMENTS unparsed_length)
  if(unparsed_length GREATER 0)
    if(unparsed_length GREATER 1)
    # Two mandatory arguments and one optional, anything after that is an error.
      list(GET IMPORT_KCONFIG_UNPARSED_ARGUMENTS 1 first_invalid)
      message(SEND_ERROR "Unexpected argument after '<keys>': import_kconfig(... ${first_invalid})")
    endif()
    set(${IMPORT_KCONFIG_UNPARSED_ARGUMENTS} "${keys}" PARENT_SCOPE)
  endif()
endfunction()

function(ameba_add_empty_object)
    # Empty object is added to avoid cmake error: NO SOURCE given to target...
    # However, default empty file generates .data, .text, .rodata, .bss section, and debug related sections,
    # which will be linked into the EMPTY SECTION (please refer to ameba_rom_bin.ld).
    # Therefore, these sections need to be renamed ot removed.

    if(${c_ISA_TYPE} STREQUAL "ARM")
        set(att_name ".ARM.attributes")
    elseif(${c_ISA_TYPE} STREQUAL "RISC-V")
        set(att_name ".riscv.attributes")
    else()
        message(FATAL_ERROR "unknown ISA type: ${c_ISA_TYPE}")
    endif()

    ameba_gen_wrap_name(empty_obj c_EMPTY_C_OBJECT)
    add_library(${c_EMPTY_C_OBJECT}_origin OBJECT ${c_EMPTY_C_FILE})
    set(c_EMPTY_C_OBJECT_FILE "$<TARGET_OBJECTS:${c_EMPTY_C_OBJECT}_origin>")
    add_custom_target(
        ${c_EMPTY_C_OBJECT}
        COMMAND ${CMAKE_STRIP} ${c_EMPTY_C_OBJECT_FILE}
        COMMAND ${CMAKE_OBJCOPY} --remove-section .comment --remove-section ${att_name} ${c_EMPTY_C_OBJECT_FILE} ${c_EMPTY_C_OBJECT_FILE}
        COMMAND ${CMAKE_OBJCOPY} --prefix-alloc-sections .rom ${c_EMPTY_C_OBJECT_FILE} ${c_EMPTY_C_OBJECT_FILE}
        DEPENDS ${c_EMPTY_C_OBJECT}_origin
    )

    set(c_EMPTY_C_OBJECT ${c_EMPTY_C_OBJECT} PARENT_SCOPE)
    set(c_EMPTY_C_OBJECT_FILE ${c_EMPTY_C_OBJECT_FILE} PARENT_SCOPE)
endfunction()

function(ameba_add_library name)
    # Add linking ${c_MCU_PROJ_CONFIG}
    list(FIND ARGN "p_LINK_LIBRARIES" _INDEX)
    if(_INDEX GREATER -1)
        list(INSERT ARGN ${_INDEX} ${c_MCU_PROJ_CONFIG})
    else()
        list(APPEND ARGN p_LINK_LIBRARIES ${c_MCU_PROJ_CONFIG})
    endif()

    ameba_target_add(${name}
        p_WRAP_TARGET_NAME
        p_PREFIX lib_
        ${ARGN}
    )
    set(c_CURRENT_TARGET_NAME ${c_CURRENT_TARGET_NAME} PARENT_SCOPE)
    set(c_CURRENT_TARGET_FILE ${c_CURRENT_TARGET_FILE} PARENT_SCOPE)
endfunction()

function(ameba_add_internal_library name)
    set(options
        p_NO_WHOLE_ARCHIVE          # If set, the target will be linked without whole_archive options
    )

    cmake_parse_arguments(ARG "${options}" "" "" ${ARGN})

    if(ARG_p_NO_WHOLE_ARCHIVE)
        list(REMOVE_ITEM ARGN p_NO_WHOLE_ARCHIVE)
    endif()

    ameba_add_library(${name} ${ARGN})

    if(c_CURRENT_IMAGE)
        if(ARG_p_NO_WHOLE_ARCHIVE)
            ameba_target_get_output_info(${c_CURRENT_TARGET_NAME} o_path o_name)
            set_property(TARGET ${c_MCU_PROJ_CONFIG} APPEND PROPERTY ${c_CURRENT_IMAGE}_no_whole_archive_libs "${o_path}/${o_name}")
        else()
            set_property(TARGET ${c_MCU_PROJ_CONFIG} APPEND PROPERTY ${c_CURRENT_IMAGE}_libraries "${c_CURRENT_TARGET_NAME}")
        endif()
    endif()

    set(c_CURRENT_TARGET_NAME ${c_CURRENT_TARGET_NAME} PARENT_SCOPE)
    set(c_CURRENT_TARGET_FILE ${c_CURRENT_TARGET_FILE} PARENT_SCOPE)
endfunction()

function(ameba_add_external_library name output_path output_name)
    # NOTE: A component may be compiled multiple times in an
    #       MCU project (using a wrapper based on the image name).
    #       In this case, it's okay if the output library files are
    #       placed in different directories, but errors will occur
    #       if they are in the same directory. Therefore, it's necessary
    #       to compile the component only once within the scope of the MCU project.
    #       Here, a flag target can be used for checking.
    set(flag_target ${name}_multi_build_check_${c_MCU_PROJECT_NAME})
    if(TARGET ${flag_target})
        return()
    else()
        add_library(${flag_target} INTERFACE)
    endif()

    ameba_add_library(${name}
        p_STRIP_DEBUG
        p_ENABLE_DETERMINISTIC_ARCHIVES
        p_OUTPUT_PATH ${output_path}
        p_OUTPUT_NAME ${output_name}
        ${ARGN}
    )

    if(TARGET ${c_CURRENT_IMAGE})
        #NOTE: target maybe dropped by p_DROP_IF_NO_SOURCES
        if(c_CURRENT_TARGET_NAME)
            add_dependencies(${c_CURRENT_IMAGE} ${c_CURRENT_TARGET_NAME})
        endif()
    else()
        ameba_warning("no current image")
    endif()
    set(c_CURRENT_TARGET_NAME ${c_CURRENT_TARGET_NAME} PARENT_SCOPE)
    set(c_CURRENT_TARGET_FILE ${c_CURRENT_TARGET_FILE} PARENT_SCOPE)
endfunction()


function(ameba_add_external_tmp_library name)
    #NOTE: Only work before release
    if(CONFIG_AMEBA_RLS)
        return()
    endif()

    ameba_add_library(${name} ${ARGN})

    set(c_CURRENT_TARGET_NAME ${c_CURRENT_TARGET_NAME} PARENT_SCOPE)
    set(c_CURRENT_TARGET_FILE ${c_CURRENT_TARGET_FILE} PARENT_SCOPE)
endfunction()

function(ameba_add_external_app_library name)
    set(oneValueArgs
        p_OUTPUT_NAME               # Set target output name, default: ${name}
    )
    cmake_parse_arguments(ARG "" "${oneValueArgs}" "" ${ARGN})

    #NOTE: Only work before release
    if(CONFIG_AMEBA_RLS)
        return()
    endif()

    if (ARG_p_OUTPUT_NAME)
        ameba_list_remove_key_value(ARGN p_OUTPUT_NAME)
    else()
        set(ARG_p_OUTPUT_NAME ${name})
    endif()

    ameba_add_external_library(${name} ${c_SDK_LIB_APPLICATION_DIR} ${ARG_p_OUTPUT_NAME} ${ARGN})
    set(c_CURRENT_TARGET_NAME ${c_CURRENT_TARGET_NAME} PARENT_SCOPE)
    set(c_CURRENT_TARGET_FILE ${c_CURRENT_TARGET_FILE} PARENT_SCOPE)
endfunction()

function(ameba_add_external_soc_library name)
    set(oneValueArgs
        p_OUTPUT_NAME               # Set target output name, default: ${name}
    )
    cmake_parse_arguments(ARG "" "${oneValueArgs}" "" ${ARGN})

    #NOTE: Only work before release
    if(CONFIG_AMEBA_RLS)
        return()
    endif()

    if (ARG_p_OUTPUT_NAME)
        ameba_list_remove_key_value(ARGN p_OUTPUT_NAME)
    else()
        set(ARG_p_OUTPUT_NAME ${name})
    endif()

    ameba_add_external_library(${name} ${c_SDK_LIB_SOC_DIR} ${ARG_p_OUTPUT_NAME} ${ARGN})
    set(c_CURRENT_TARGET_NAME ${c_CURRENT_TARGET_NAME} PARENT_SCOPE)
    set(c_CURRENT_TARGET_FILE ${c_CURRENT_TARGET_FILE} PARENT_SCOPE)
endfunction()

function(ameba_port_standalone_app_library target output_name)
    #NOTE: Only work before release
    if(CONFIG_AMEBA_RLS)
        return()
    endif()
    ameba_target_get_output_info(${target} o_path o_name) #Get target's library output path and output file name
    ameba_port_library_file(${o_path}/${o_name} ${c_SDK_LIB_APPLICATION_DIR} ${output_name})
    set(c_CURRENT_TARGET_NAME ${c_CURRENT_TARGET_NAME} PARENT_SCOPE)
    set(c_CURRENT_TARGET_FILE ${o_path}/${o_name} PARENT_SCOPE)
endfunction()

function(ameba_port_standalone_internal_library name)
    set(options
        p_NO_WHOLE_ARCHIVE          # If set, the target will be linked without whole_archive options
    )

    cmake_parse_arguments(ARG "${options}" "" "" ${ARGN})

    if(ARG_p_NO_WHOLE_ARCHIVE)
        list(REMOVE_ITEM ARGN p_NO_WHOLE_ARCHIVE)
    endif()

    if(c_CURRENT_IMAGE)
        if(ARG_p_NO_WHOLE_ARCHIVE)
            ameba_target_get_output_info(${name} o_path o_name)
            set_property(TARGET ${c_MCU_PROJ_CONFIG} APPEND PROPERTY ${c_CURRENT_IMAGE}_no_whole_archive_libs "${o_path}/${o_name}")
        else()
            set_property(TARGET ${c_MCU_PROJ_CONFIG} APPEND PROPERTY ${c_CURRENT_IMAGE}_libraries "${name}")
        endif()
    else()
        ameba_warning("c_CURRENT_IMAGE is unset when call ameba_port_standalone_internal_library")
    endif()
endfunction()

function(ameba_add_merge_library output_name output_path)
    ameba_gen_wrap_name(${output_name} c_CURRENT_TARGET_NAME)
    set(libs)
    set(deps)
    foreach(lib ${ARGN})
        get_filename_component(file_ext "${lib}" EXT)
        if("${file_ext}_" STREQUAL ".a_" OR "${file_ext}_" STREQUAL ".lib_")
            #FIXME: For lib files, dependency may miss when target of lib file compiled after MERGE
            ameba_fatal("NOT support merge lib file")
            # if(NOT EXISTS ${lib})
            #     ameba_fatal("${lib} not exist")
            # endif()
            # ameba_list_append(libs ${lib})
        else()
            if(TARGET ${lib})
                set(target_name ${lib})
            else()
                ameba_gen_wrap_name(${lib} target_name)
                if(NOT TARGET ${target_name})
                    ameba_fatal("${lib} is not a file or target(nor ${target_name} is a target)")
                endif()
            endif()
            ameba_target_get_output_info(${target_name} o_path o_name)
            ameba_list_append(libs ${o_path}/${o_name})
            ameba_list_append(deps ${target_name})
        endif()
    endforeach()

    set(unpack_commands)
    set(temp_dir "${CMAKE_CURRENT_BINARY_DIR}/${c_CURRENT_TARGET_NAME}")
    foreach(lib IN LISTS libs)
        list(APPEND unpack_commands
            COMMAND ${CMAKE_COMMAND} -E chdir ${temp_dir} ${CMAKE_AR} x ${lib}
        )
    endforeach()

   if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL Windows)
        set(list_cmd COMMAND ${CMAKE_COMMAND} -E chdir ${temp_dir} cmd /C "dir /b *.o > o_files.list")
   else()
      set(list_cmd COMMAND ls ${temp_dir}/*.o > ${temp_dir}/o_files.list)
   endif()

    set(full_output ${output_path}/lib_${output_name}.a)
    add_custom_command(
        OUTPUT ${full_output}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${temp_dir}
        ${unpack_commands}
        COMMAND ${CMAKE_COMMAND} -E touch ${temp_dir}/o_files.list
        ${list_cmd}
        COMMAND ${CMAKE_COMMAND} -E rm -f ${full_output}
        COMMAND ${CMAKE_COMMAND} -E chdir ${temp_dir} ${CMAKE_AR} crs ${full_output} "@o_files.list"
        COMMAND ${CMAKE_OBJCOPY} -g -D ${full_output}
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${temp_dir}
        DEPENDS ${libs}
        COMMENT "Merging libraries lib_${output_name}.a using ar"
    )

    add_custom_target(
        ${c_CURRENT_TARGET_NAME}_merge ALL
        DEPENDS ${full_output}
    )
    add_dependencies(${c_CURRENT_TARGET_NAME}_merge ${deps})

    add_library(${c_CURRENT_TARGET_NAME} STATIC IMPORTED)
    set_target_properties(${c_CURRENT_TARGET_NAME} PROPERTIES
        IMPORTED_LOCATION "${full_output}"
    )
    set_source_files_properties(${c_CURRENT_TARGET_NAME} PROPERTIES GENERATED TRUE)

    if(TARGET ${c_CURRENT_IMAGE})
        add_dependencies(${c_CURRENT_IMAGE} ${c_CURRENT_TARGET_NAME}_merge)
    else()
        ameba_warning("no current image")
    endif()
    set(c_CURRENT_TARGET_NAME ${c_CURRENT_TARGET_NAME} PARENT_SCOPE)
    set(c_CURRENT_TARGET_FILE ${full_output} PARENT_SCOPE)
endfunction()

function(ameba_add_merge_app_library output_name)
    #NOTE: Only work before release
    if(CONFIG_AMEBA_RLS)
        return()
    endif()

    ameba_add_merge_library(${output_name} ${c_SDK_LIB_APPLICATION_DIR} ${ARGN})
    set(c_CURRENT_TARGET_NAME ${c_CURRENT_TARGET_NAME} PARENT_SCOPE)
    set(c_CURRENT_TARGET_FILE ${c_CURRENT_TARGET_FILE} PARENT_SCOPE)
endfunction()

function(ameba_add_merge_soc_library output_name)
    #NOTE: Only work before release
    if(CONFIG_AMEBA_RLS)
        return()
    endif()
    ameba_add_merge_library(${output_name} ${c_SDK_LIB_SOC_DIR} ${ARGN})
    set(c_CURRENT_TARGET_NAME ${c_CURRENT_TARGET_NAME} PARENT_SCOPE)
    set(c_CURRENT_TARGET_FILE ${c_CURRENT_TARGET_FILE} PARENT_SCOPE)
endfunction()

function(ameba_add_image name)
    set(options p_EXCLUDE_FROM_ALL)
    set(oneValueArgs p_TYPE p_IMAGE_ALL)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "" ${ARGN})
    ameba_gen_wrap_name(${name} c_CURRENT_IMAGE)
    if(ARG_p_EXCLUDE_FROM_ALL)
        add_executable(${c_CURRENT_IMAGE} EXCLUDE_FROM_ALL ${c_EMPTY_C_OBJECT_FILE})
        add_dependencies(${c_CURRENT_IMAGE} ${c_EMPTY_C_OBJECT})
    else()
        add_executable(${c_CURRENT_IMAGE} ${c_EMPTY_C_OBJECT_FILE})
        add_dependencies(${c_CURRENT_IMAGE} ${c_EMPTY_C_OBJECT})
    endif()
    set_property(TARGET ${c_MCU_PROJ_CONFIG} APPEND PROPERTY image_list "${c_CURRENT_IMAGE}")

    if(ARG_p_TYPE)
        list(FIND c_VALID_IMAGE_TYPES ${ARG_p_TYPE} _INDEX)
        if(_INDEX EQUAL -1)
            ameba_fatal("Invalid image type: ${ARG_p_TYPE}, valid list: ${c_VALID_IMAGE_TYPES}")
        endif()
        set(c_CURRENT_IMAGE_TYPE ${ARG_p_TYPE})
    else()
        get_filename_component(c_CURRENT_IMAGE_TYPE ${CMAKE_CURRENT_LIST_DIR} NAME)

        if(${c_CURRENT_IMAGE_TYPE} STREQUAL "image_floader" OR ${c_CURRENT_IMAGE_TYPE} STREQUAL "image_gdb_floader")
            set(c_CURRENT_IMAGE_TYPE gdb_floader)
        elseif(${c_CURRENT_IMAGE_TYPE} STREQUAL "image_imgtool_floader")
            set(c_CURRENT_IMAGE_TYPE imgtool_floader)
        elseif(${c_CURRENT_IMAGE_TYPE} STREQUAL "image_rom")
            set(c_CURRENT_IMAGE_TYPE rom_ns)
        elseif(${c_CURRENT_IMAGE_TYPE} STREQUAL "ns")
            set(c_CURRENT_IMAGE_TYPE rom_ns)
        elseif(${c_CURRENT_IMAGE_TYPE} STREQUAL "tz")
            set(c_CURRENT_IMAGE_TYPE rom_tz)
        elseif(${c_CURRENT_IMAGE_TYPE} STREQUAL "image1" OR
               ${c_CURRENT_IMAGE_TYPE} STREQUAL "image2" OR
               ${c_CURRENT_IMAGE_TYPE} STREQUAL "image3"
              )
        else()
            ameba_warning("Call ameba_add_image under an unexpected path: ${CMAKE_CURRENT_LIST_DIR}")
        endif()
    endif()

    if(${c_CURRENT_IMAGE_TYPE} STREQUAL "rom_ns" OR ${c_CURRENT_IMAGE_TYPE} STREQUAL "rom_tz")
        ameba_set(c_CURRENT_IMAGE_IS_ROM TRUE p_SCOPE both)
    else()
        ameba_set(c_CURRENT_IMAGE_IS_ROM FALSE p_SCOPE both)
    endif()

    if(TARGET rom)
        if(NOT ${c_CURRENT_IMAGE_IS_ROM})
            add_dependencies(${c_CURRENT_IMAGE} rom)
        endif()
    endif()

    #Can be used to get target name of image1/image2/image2 somewhere else
    set_property(TARGET ${c_MCU_PROJ_CONFIG} APPEND PROPERTY ${c_CURRENT_IMAGE_TYPE} "${c_CURRENT_IMAGE}")
    if (ARG_p_IMAGE_ALL)
        set_property(TARGET ${c_MCU_PROJ_CONFIG} APPEND PROPERTY ${c_CURRENT_IMAGE_TYPE}_all "${c_SDK_IMAGE_TARGET_DIR}/${ARG_p_IMAGE_ALL}")
    endif()
    set(c_CURRENT_IMAGE ${c_CURRENT_IMAGE} PARENT_SCOPE)
    set(c_CURRENT_IMAGE_TYPE ${c_CURRENT_IMAGE_TYPE} PARENT_SCOPE)
endfunction()

function(ameba_get_image_target_name image_type result)
    set(oneValueArgs
        p_MCU_PROJECT_NAME  # Specific which mcu project you want get image from, default is current mcu project
    )
    cmake_parse_arguments(ARG "" "${oneValueArgs}" "" ${ARGN})
    if(ARG_p_MCU_PROJECT_NAME)
        get_property(mcu_config TARGET g_PROJECT_CONFIG PROPERTY ${ARG_p_MCU_PROJECT_NAME}_config)
    else()
        set(mcu_config ${c_MCU_PROJ_CONFIG})
    endif()
    if(TARGET ${mcu_config})
        get_property(tmp_result TARGET ${mcu_config} PROPERTY ${image_type})
        set(${result} ${tmp_result} PARENT_SCOPE)
    else()
        ameba_warning("mcu config: ${mcu_config} not exist")
        unset(${result} PARENT_SCOPE)
    endif()
endfunction()

function(ameba_get_image_all_path image_type result)
    set(oneValueArgs
        p_MCU_PROJECT_NAME  # Specific which mcu project you want get image from, default is current mcu project
    )
    cmake_parse_arguments(ARG "" "${oneValueArgs}" "" ${ARGN})
    if(ARG_p_MCU_PROJECT_NAME)
        get_property(mcu_config TARGET g_PROJECT_CONFIG PROPERTY ${ARG_p_MCU_PROJECT_NAME}_config)
    else()
        set(mcu_config ${c_MCU_PROJ_CONFIG})
    endif()
    if(TARGET ${mcu_config})
        get_property(tmp_result TARGET ${mcu_config} PROPERTY ${image_type}_all)
        set(${result} ${tmp_result} PARENT_SCOPE)
    else()
        ameba_warning("mcu config: ${mcu_config} not exist")
        unset(${result} PARENT_SCOPE)
    endif()
endfunction()

# get the main output image dir under special mcu project set when call ameba_soc_project_create
function(ameba_get_image_output_dir result)
    get_property(tmp_result TARGET g_PROJECT_CONFIG PROPERTY image_output_dir)
    set(${result} ${tmp_result} PARENT_SCOPE)
endfunction()

function(ameba_add_rom name)
    add_executable(${name} ${c_EMPTY_C_FILE})
    set_property(TARGET ${c_MCU_PROJ_CONFIG} APPEND PROPERTY image_list "${name}")
    set(c_CURRENT_IMAGE ${name} PARENT_SCOPE)
endfunction()

function(ameba_add_subdirectory dir)
    if(IS_ABSOLUTE "${dir}")
        if(ARGN)
            add_subdirectory(${dir} ${ARGN})
        else()
            file(TO_CMAKE_PATH "${dir}" dir)
            get_filename_component(dir_name ${dir} NAME)
            add_subdirectory(${dir} ${CMAKE_CURRENT_BINARY_DIR}/${dir_name})
        endif()
    else()
        add_subdirectory(${dir})
    endif()
endfunction()

function(ameba_add_subdirectory_if condition dir)
    if(DEFINED ${condition})
        if(${condition})
            ameba_add_subdirectory(${dir} ${ARGN})
        endif()
    endif()
endfunction()

function(ameba_add_subdirectory_ifnot condition dir)
    if(DEFINED ${condition})
        if(NOT ${condition})
            ameba_add_subdirectory(${dir} ${ARGN})
        endif()
    else()
        ameba_add_subdirectory(${dir} ${ARGN})
    endif()
endfunction()

function(ameba_add_subdirectory_if_exist dir)
    if(CONFIG_AMEBA_RLS)
        if(NOT IS_ABSOLUTE "${dir}")
            file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${dir}" dir)
        endif()
        if(NOT EXISTS ${dir})
            return()
        endif()
    endif()
    ameba_add_subdirectory(${dir} ${ARGN})
endfunction()

function(ameba_global_include)
    target_include_directories(${c_MCU_PROJ_CONFIG} INTERFACE ${ARGN})
endfunction()

function(ameba_global_define)
    target_compile_definitions(${c_MCU_PROJ_CONFIG} INTERFACE ${ARGN})
endfunction()

function(ameba_global_library)
    set(options
        p_NO_WHOLE_ARCHIVE   # If set, libraries will be marked as no-whole-archive when linked to current image
    )
    cmake_parse_arguments(ARG "${options}" "" "" ${ARGN})

    if(c_CURRENT_IMAGE)
        if(ARG_p_NO_WHOLE_ARCHIVE)
            list(REMOVE_ITEM ARGN p_NO_WHOLE_ARCHIVE)
            set_property(TARGET ${c_MCU_PROJ_CONFIG} APPEND PROPERTY ${c_CURRENT_IMAGE}_no_whole_archive_libs "${ARGN}")
        else()
            set_property(TARGET ${c_MCU_PROJ_CONFIG} APPEND PROPERTY ${c_CURRENT_IMAGE}_whole_archive_libs "${ARGN}")
        endif()
    endif()
endfunction()


########################################################################################################
#TODO: deprecated functions

function(ameba_add_exist_library name)
    ameba_warning("This function is deprecated and will be removed in future versions, use ameba_port_standalone_internal_library instead")
    ameba_port_standalone_internal_library(${name})
endfunction()

#NOTE: For compatibility with version CMAKE_V0.1

function(ameba_internal_library name)
    ameba_add_internal_library(${name})
    set(CURRENT_LIB_NAME ${c_CURRENT_TARGET_NAME} PARENT_SCOPE)
endfunction()

#define soc library named lib_${name}_${PROJECT_NAME}.a, and it will be move to lib/soc
function(ameba_soc_library name)
    ameba_add_external_soc_library(${name})
    set(CURRENT_LIB_NAME ${c_CURRENT_TARGET_NAME} PARENT_SCOPE)
endfunction()

#define application library named lib_${name}_${PROJECT_NAME}.a, and it will be move to lib/application
function(ameba_app_library name)
    ameba_add_external_app_library(${name})
    set(CURRENT_LIB_NAME ${c_CURRENT_TARGET_NAME} PARENT_SCOPE)
endfunction()

function(ameba_app_library_with_gitver name)
    ameba_add_external_app_library(${name})
    set(CURRENT_LIB_NAME ${c_CURRENT_TARGET_NAME} PARENT_SCOPE)
endfunction()
