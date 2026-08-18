include_guard(GLOBAL)
include(${CMAKE_CURRENT_LIST_DIR}/utility_base.cmake)
if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/utility_internal.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/utility_internal.cmake)
endif()

# _ameba_pop_keyword(<list_var> <keyword> <out_var>)
#
# S-2: Extract <keyword> and its values from <list_var>, modifying it
# in-place.  Value collection stops at the next p_[A-Z_]+ keyword (or end of
# list).  The extracted values are placed in <out_var>.  Unlike
# cmake_parse_arguments, this macro never mistakes a subsequent p_XXX keyword
# for a value of the current keyword.
macro(_ameba_pop_keyword _apk_list _apk_key _apk_out)
    set(${_apk_out})
    set(_apk_kept)
    set(_apk_in_key FALSE)
    foreach(_apk_arg IN LISTS ${_apk_list})
        if("${_apk_arg}" STREQUAL "${_apk_key}")
            set(_apk_in_key TRUE)
        elseif(_apk_in_key AND "${_apk_arg}" MATCHES "^p_[A-Z_]+$")
            set(_apk_in_key FALSE)
            list(APPEND _apk_kept "${_apk_arg}")
        elseif(_apk_in_key)
            list(APPEND ${_apk_out} "${_apk_arg}")
        else()
            list(APPEND _apk_kept "${_apk_arg}")
        endif()
    endforeach()
    set(${_apk_list} ${_apk_kept})
    unset(_apk_kept)
    unset(_apk_in_key)
    unset(_apk_arg)
endmacro()

# Convenience alias kept for readability at call sites.
macro(_ameba_pop_release_includes _apri_list _apri_out)
    _ameba_pop_keyword(${_apri_list} p_RELEASE_INCLUDES ${_apri_out})
endmacro()

# _ameba_install_public_headers(<argn_var>)
#
# Install the public/release headers for an external/internal library target.
# Pops p_RELEASE_INCLUDES from <argn_var> and installs those directories;
# falls back to ${public_includes} when p_RELEASE_INCLUDES is absent.
# Only "*.h" files directly in each listed directory are installed —
# subdirectories are not traversed. Additionally, any directory whose path
# contains an "internal" component is skipped entirely, so callers need not
# filter these out from public_includes / p_RELEASE_INCLUDES.
macro(_ameba_install_public_headers _aiph_argn)
    _ameba_pop_release_includes(${_aiph_argn} _aiph_includes)
    if(NOT _aiph_includes)
        set(_aiph_includes ${public_includes})
    endif()
    foreach(_aiph_inc IN LISTS _aiph_includes)
        get_filename_component(_aiph_inc "${_aiph_inc}" ABSOLUTE
                               BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        if("${_aiph_inc}" MATCHES "(^|/)internal(/|$)")
            continue()
        endif()
        ameba_install_directory_flat("${_aiph_inc}" p_GLOB "*.h")
    endforeach()
    unset(_aiph_includes)
    unset(_aiph_inc)
endmacro()


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

function(ameba_submodule_register name)
    set(multiValueArgs
        p_EXAMPLE_DIRS
    )
    cmake_parse_arguments(ARG "" "" "${multiValueArgs}" ${ARGN})

    set(SUBMODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
    set(INFO_STRING "submodule_name=${name} submodule_path=${SUBMODULE_PATH} ")
    set_property(TARGET g_PROJECT_CONFIG APPEND PROPERTY submodule_info ${INFO_STRING})

    if(NOT ARG_p_EXAMPLE_DIRS)
        message(FATAL_ERROR "None p_EXAMPLE_DIRS. Submodule example directories must be given")
    endif()

    list(LENGTH ARG_p_EXAMPLE_DIRS example_num)
    math(EXPR last_index "${example_num} - 1")
    foreach(index RANGE ${last_index})
        list(GET ARG_p_EXAMPLE_DIRS ${index} example_dir)
        ameba_example_register(${example_dir})
    endforeach()
endfunction()

function(ameba_example_register example_dir)
    set(options p_NOT_SUBMODULE)
    cmake_parse_arguments(ARG "${options}" "" "" ${ARGN})

    if(NOT IS_ABSOLUTE "${example_dir}")
        file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${example_dir}" example_dir)
    endif()
    get_filename_component(example_name "${example_dir}" NAME)

    set(INFO_STRING "example_name=${example_name} example_path=${example_dir} ")

    if(NOT ARG_p_NOT_SUBMODULE)
        set_property(TARGET g_PROJECT_CONFIG APPEND PROPERTY submodule_info ${INFO_STRING})
    # else() # TODO: using for non-submodule
    endif()

endfunction()

# Register a directory whose contents are packed into the read-only "rolfs:"
# littlefs image (requires CONFIG_LITTLEFS_WITHIN_APP_IMG). Call from an
# application's CMakeLists.txt; the application owns the path.
#
# Optional second argument: a CMake target name that produces the staging dir
# (e.g. a custom_target driven by add_custom_command with a stamp file).
# When provided it is stored in g_ROLFS_STAGING_TARGET so that
# ameba_firmware_package can call add_dependencies(firmware_package <target>)
# at the SOC-project level where both targets are visible to CMake.
function(ameba_add_rolfs_content content_dir)
    if(NOT IS_ABSOLUTE "${content_dir}")
        file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${content_dir}" content_dir)
    endif()
    set_property(GLOBAL PROPERTY g_ROLFS_CONTENT_DIR "${content_dir}")
    if(ARGC GREATER 1)
        set_property(GLOBAL PROPERTY g_ROLFS_STAGING_TARGET "${ARGV1}")
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
    # SoC hook (only addition to the original function): a SoC may define
    # ameba_soc_internal_library_hook to intercept internal-library creation
    if(COMMAND ameba_soc_internal_library_hook)
        set(_soc_hook_handled FALSE)
        ameba_soc_internal_library_hook("${name}" "${ARGN}" _soc_hook_handled)
        if(_soc_hook_handled)
            set(c_CURRENT_TARGET_NAME ${c_CURRENT_TARGET_NAME} PARENT_SCOPE)
            set(c_CURRENT_TARGET_FILE ${c_CURRENT_TARGET_FILE} PARENT_SCOPE)
            return()
        endif()
    endif()

    set(options
        p_NO_WHOLE_ARCHIVE          # If set, the target will be linked without whole_archive options
    )

    cmake_parse_arguments(ARG "${options}" "" "" ${ARGN})

    if(ARG_p_NO_WHOLE_ARCHIVE)
        list(REMOVE_ITEM ARGN p_NO_WHOLE_ARCHIVE)
    endif()

    # Install headers and open-source files declared via p_SOURCES.
    # - p_RELEASE_INCLUDES given: install only those dirs (explicit override).
    # - p_RELEASE_INCLUDES absent: install both public_includes and p_INCLUDES.
    # In both cases, directories whose path contains "internal/" are skipped.
    set(_ail_argn ${ARGN})
    # Detect p_RELEASE_INCLUDES before _ameba_install_public_headers consumes it.
    if("p_RELEASE_INCLUDES" IN_LIST _ail_argn)
        set(_ail_has_rls_incs TRUE)
    else()
        set(_ail_has_rls_incs FALSE)
    endif()
    _ameba_install_public_headers(_ail_argn)
    # Use a tmp copy for p_INCLUDES / p_SOURCES so _ail_argn retains both
    # keywords for the subsequent ameba_add_library call.
    set(_ail_argn_tmp ${_ail_argn})
    # Install p_INCLUDES only when p_RELEASE_INCLUDES was not given.
    if(NOT _ail_has_rls_incs)
        _ameba_pop_keyword(_ail_argn_tmp p_INCLUDES _ail_private_incs)
        foreach(_ail_priv_inc IN LISTS _ail_private_incs)
            get_filename_component(_ail_priv_inc "${_ail_priv_inc}" ABSOLUTE
                                   BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
            if("${_ail_priv_inc}" MATCHES "(^|/)internal(/|$)")
                continue()
            endif()
            ameba_install_directory_flat("${_ail_priv_inc}" p_GLOB "*.h")
        endforeach()
    endif()
    # Install p_SOURCES files (skip generated files under the build tree).
    _ameba_pop_keyword(_ail_argn_tmp p_SOURCES _ail_sources)
    if(_ail_sources)
        foreach(_ail_src IN LISTS _ail_sources)
            get_filename_component(_ail_src "${_ail_src}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
            file(RELATIVE_PATH _ail_src_rel "${CMAKE_BINARY_DIR}" "${_ail_src}")
            if(NOT _ail_src_rel MATCHES "^\\.\\.")
                continue()
            endif()
            ameba_install_files("${_ail_src}")
        endforeach()
    endif()
    set(ARGN ${_ail_argn})

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

    if(BUILD_FOR_RLS)
        list(APPEND ARGN p_STRIP_DEBUG p_ENABLE_DETERMINISTIC_ARCHIVES)
    endif()
    ameba_add_library(${name}
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
    # Install public headers (p_RELEASE_INCLUDES if given, else public_includes).
    set(_aetl_argn ${ARGN})
    _ameba_install_public_headers(_aetl_argn)
    set(ARGN ${_aetl_argn})

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

    # Install public headers (p_RELEASE_INCLUDES if given, else public_includes).
    set(_aeail_argn ${ARGN})
    _ameba_install_public_headers(_aeail_argn)
    set(ARGN ${_aeail_argn})

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

    # Install public headers (p_RELEASE_INCLUDES if given, else public_includes).
    set(_aesl_argn ${ARGN})
    _ameba_install_public_headers(_aesl_argn)
    set(ARGN ${_aesl_argn})

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

function(ameba_add_external_module_library name output_path)
    set(oneValueArgs
        p_OUTPUT_NAME               # Set target output name, default: ${name}
    )
    cmake_parse_arguments(ARG "" "${oneValueArgs}" "" ${ARGN})

    # Install public headers (p_RELEASE_INCLUDES if given, else public_includes).
    set(_aeml_argn ${ARGN})
    _ameba_install_public_headers(_aeml_argn)
    set(ARGN ${_aeml_argn})

    #NOTE: Only work before release
    if(CONFIG_AMEBA_RLS)
        return()
    endif()

    if (ARG_p_OUTPUT_NAME)
        ameba_list_remove_key_value(ARGN p_OUTPUT_NAME)
    else()
        set(ARG_p_OUTPUT_NAME ${name})
    endif()

    ameba_add_external_library(${name} ${output_path} ${ARG_p_OUTPUT_NAME} ${ARGN})
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
    set(_objcopy_flags)
    ameba_list_append_if(BUILD_FOR_RLS _objcopy_flags -g -D)
    add_custom_command(
        OUTPUT ${full_output}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${temp_dir}
        ${unpack_commands}
        COMMAND ${CMAKE_COMMAND} -E touch ${temp_dir}/o_files.list
        ${list_cmd}
        COMMAND ${CMAKE_COMMAND} -E rm -f ${full_output}
        COMMAND ${CMAKE_COMMAND} -E chdir ${temp_dir} ${CMAKE_AR} crs ${full_output} "@o_files.list"
        COMMAND ${CMAKE_OBJCOPY} ${_objcopy_flags} ${full_output}
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${temp_dir}
        DEPENDS ${libs}
        COMMENT "Merging libraries lib_${output_name}.a using ar"
    )
    unset(_objcopy_flags)

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

function(ameba_add_merge_module_library output_name output_path)
    #NOTE: Only work before release
    if(CONFIG_AMEBA_RLS)
        return()
    endif()
    ameba_add_merge_library(${output_name} ${output_path} ${ARGN})
    set(c_CURRENT_TARGET_NAME ${c_CURRENT_TARGET_NAME} PARENT_SCOPE)
    set(c_CURRENT_TARGET_FILE ${c_CURRENT_TARGET_FILE} PARENT_SCOPE)
endfunction()

function(ameba_add_image name)
    set(options
        p_EXCLUDE_FROM_ALL
        p_HIDE_FROM_ALL_PATH   # the image will not append to image_list
    )
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
        if(NOT ARG_p_HIDE_FROM_ALL_PATH)
            set_property(TARGET ${c_MCU_PROJ_CONFIG} APPEND PROPERTY ${c_CURRENT_IMAGE_TYPE}_all "${c_SDK_IMAGE_TARGET_DIR}/${ARG_p_IMAGE_ALL}")
        endif()
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
    # Install only the cmake/Kconfig files directly in `dir` (non-recursive).
    # Each subdirectory registers its own files when ameba_add_subdirectory is
    # called for it, so the install mirrors the exact set of directories the
    # current IC visits during configure — no over-installation of sibling dirs.
    if(IS_ABSOLUTE "${dir}")
        set(_aas_abs "${dir}")
    else()
        set(_aas_abs "${CMAKE_CURRENT_SOURCE_DIR}/${dir}")
    endif()
    file(TO_CMAKE_PATH "${_aas_abs}" _aas_abs)
    file(RELATIVE_PATH _aas_rel "${c_BASEDIR}" "${_aas_abs}")
    file(GLOB _aas_files
        "${_aas_abs}/CMakeLists.txt"
        "${_aas_abs}/*.cmake"
        "${_aas_abs}/Kconfig"
        "${_aas_abs}/Kconfig.*"
        "${_aas_abs}/README.md"
    )
    if(_aas_files)
        ameba_install_files(${_aas_files})
    endif()
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
    if(NOT IS_ABSOLUTE "${dir}")
        file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${dir}" dir)
    endif()
    if(NOT EXISTS "${dir}/CMakeLists.txt")
        return()
    endif()
    ameba_add_subdirectory(${dir} ${ARGN})
endfunction()

# ameba_install_directory_flat(<src_dir> [p_GLOB <pat>...])
#
# Install files directly under <src_dir> (non-recursive) to the mirrored path
# relative to c_BASEDIR.  Subdirectories are never traversed.
# <src_dir> may be absolute or relative to CMAKE_CURRENT_SOURCE_DIR.
# p_GLOB restricts which files are collected (e.g. "*.h"); when omitted all
# files directly under <src_dir> are installed.
function(ameba_install_directory_flat src_dir)
    cmake_parse_arguments(ARG "" "" "p_GLOB" ${ARGN})

    if(NOT IS_ABSOLUTE "${src_dir}")
        set(src_dir "${CMAKE_CURRENT_SOURCE_DIR}/${src_dir}")
    endif()
    file(TO_CMAKE_PATH "${src_dir}" src_dir)
    file(RELATIVE_PATH _aidf_rel "${c_BASEDIR}" "${src_dir}")

    if(ARG_p_GLOB)
        set(_aidf_files)
        foreach(_aidf_pat IN LISTS ARG_p_GLOB)
            file(GLOB _aidf_matched "${src_dir}/${_aidf_pat}")
            list(APPEND _aidf_files ${_aidf_matched})
        endforeach()
    else()
        file(GLOB _aidf_files "${src_dir}/*")
        # Remove subdirectories — GLOB returns both files and dirs
        list(FILTER _aidf_files EXCLUDE REGEX "/$")
        foreach(_aidf_f IN LISTS _aidf_files)
            if(IS_DIRECTORY "${_aidf_f}")
                list(REMOVE_ITEM _aidf_files "${_aidf_f}")
            endif()
        endforeach()
    endif()

    if(_aidf_files)
        install(FILES ${_aidf_files} DESTINATION "${_aidf_rel}")
    endif()

endfunction()

# ameba_install_directory(<src_dir>
#   [p_GLOB <pat>...]             # Positive file glob (e.g. "*.h"); implies FILES_MATCHING.
#                                 # When omitted every file is installed.
#   [p_EXCLUDE_PATTERN <pat>...]  # Subdirectory / file name patterns to exclude.
#   [p_EXCLUDE_REGEX   <re>...]   # Regex patterns to exclude.
# )
#
# Install all files under <src_dir> to the mirrored path relative to c_BASEDIR.
# <src_dir> may be absolute or relative to CMAKE_CURRENT_SOURCE_DIR.
# .git is always excluded automatically; callers need not repeat it.
#
# Examples:
#   # Install everything (e.g. example or third-party dir)
#   ameba_install_directory("${example_dir}")
#
#   # Install everything except lib/ and ld_ns/ at any depth (e.g. SoC project dir)
#   ameba_install_directory("${CMAKE_CURRENT_SOURCE_DIR}"
#       p_EXCLUDE_REGEX "(^|/)lib($|/)" "(^|/)ld_ns($|/)"
#   )
#
#   # Install only public headers, hide internal/ and secure/
#   ameba_install_directory("${inc_dir}"
#       p_GLOB "*.h"
#       p_EXCLUDE_PATTERN "internal" "secure"
#   )
function(ameba_install_directory src_dir)
    cmake_parse_arguments(ARG "" "" "p_GLOB;p_EXCLUDE_PATTERN;p_EXCLUDE_REGEX" ${ARGN})

    if(NOT IS_ABSOLUTE "${src_dir}")
        set(src_dir "${CMAKE_CURRENT_SOURCE_DIR}/${src_dir}")
    endif()
    file(TO_CMAKE_PATH "${src_dir}" src_dir)
    file(RELATIVE_PATH _aid_rel "${c_BASEDIR}" "${src_dir}")

    set(_aid_args DIRECTORY "${src_dir}/" DESTINATION "${_aid_rel}")

    # Positive glob — implies FILES_MATCHING
    if(ARG_p_GLOB)
        list(APPEND _aid_args FILES_MATCHING)
        foreach(_aid_pat IN LISTS ARG_p_GLOB)
            list(APPEND _aid_args PATTERN "${_aid_pat}")
        endforeach()
    endif()

    # Pattern-based excludes
    foreach(_aid_pat IN LISTS ARG_p_EXCLUDE_PATTERN)
        list(APPEND _aid_args PATTERN "${_aid_pat}" EXCLUDE)
    endforeach()

    # .git is always excluded
    list(APPEND _aid_args REGEX "/\\.git($|/)" EXCLUDE)

    # Additional regex-based excludes
    foreach(_aid_rex IN LISTS ARG_p_EXCLUDE_REGEX)
        list(APPEND _aid_args REGEX "${_aid_rex}" EXCLUDE)
    endforeach()

    install(${_aid_args})

endfunction()

# ameba_install_files(<file> [<file>...])
#
# Install one or more files to their mirrored paths relative to c_BASEDIR.
# Each file's destination directory is derived from its own parent directory,
# so files from different directories can be passed in a single call.
# Paths may be absolute or relative to CMAKE_CURRENT_SOURCE_DIR.
function(ameba_install_files)
    foreach(_aif_file IN LISTS ARGN)
        if(NOT IS_ABSOLUTE "${_aif_file}")
            set(_aif_file "${CMAKE_CURRENT_SOURCE_DIR}/${_aif_file}")
        endif()
        file(TO_CMAKE_PATH "${_aif_file}" _aif_file)
        get_filename_component(_aif_dir "${_aif_file}" DIRECTORY)
        file(RELATIVE_PATH _aif_rel "${c_BASEDIR}" "${_aif_dir}")
        if("${_aif_rel}" STREQUAL "")
            set(_aif_rel ".")
        endif()
        install(FILES "${_aif_file}" DESTINATION "${_aif_rel}")
    endforeach()
endfunction()

# Install the entire third-party directory (all files, no extension filter) and
# enter it as a subdirectory.  Unlike ameba_add_subdirectory which only installs
# cmake/Kconfig files, third-party sources are installed wholesale because their
# directory layout is opaque and all content is needed in the release SDK.
function(ameba_add_subdirectory_third_party dir)
    # Install the entire third-party directory (all files, no extension filter) and
    # enter it as a subdirectory.  Unlike ameba_add_subdirectory which only installs
    # cmake/Kconfig files, third-party sources are installed wholesale because their
    # directory layout is opaque and all content is needed in the release SDK.
    ameba_install_directory("${dir}")

    if(IS_ABSOLUTE "${dir}")
        file(TO_CMAKE_PATH "${dir}" dir)
        get_filename_component(dir_name ${dir} NAME)
        add_subdirectory(${dir} ${CMAKE_CURRENT_BINARY_DIR}/${dir_name})
    else()
        add_subdirectory(${dir})
    endif()
endfunction()

function(ameba_add_subdirectory_third_party_if condition dir)
    if(DEFINED ${condition})
        if(${condition})
            ameba_add_subdirectory_third_party(${dir} ${ARGN})
        endif()
    endif()
endfunction()

# Enter a closed-source subdirectory without installing any of its
# files into the release SDK.  The directory participates in the build normally
# but its entire contents are excluded from the release package — the opposite of
# ameba_add_subdirectory_third_party which installs the whole directory tree.
#
# CMAKE_SKIP_INSTALL_RULES is set to TRUE inside this function's scope and
# inherited by add_subdirectory, suppressing all install() calls (including those
# triggered by ameba_add_internal_library / ameba_install_files etc.) throughout
# the entire subtree.  The variable is local to this function and does not affect
# any other directories.
function(ameba_add_subdirectory_internal dir)
    if(NOT IS_ABSOLUTE "${dir}")
        file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${dir}" dir)
    endif()
    if(NOT EXISTS "${dir}/CMakeLists.txt")
        return()
    endif()
    set(CMAKE_SKIP_INSTALL_RULES TRUE)
    get_filename_component(dir_name ${dir} NAME)
    add_subdirectory(${dir} ${CMAKE_CURRENT_BINARY_DIR}/${dir_name})
endfunction()

function(ameba_add_subdirectory_internal_if condition dir)
    if(DEFINED ${condition})
        if(${condition})
            ameba_add_subdirectory_internal(${dir} ${ARGN})
        endif()
    endif()
endfunction()

function(ameba_global_include)
    target_include_directories(${c_MCU_PROJ_CONFIG} INTERFACE ${ARGN})
    foreach(_agi_inc IN LISTS ARGN)
        get_filename_component(_agi_inc "${_agi_inc}" ABSOLUTE
                               BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        if("${_agi_inc}" MATCHES "(^|/)internal(/|$)")
            continue()
        endif()
        ameba_install_directory_flat("${_agi_inc}" p_GLOB "*.h")
    endforeach()
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

    # Install all .a files listed in public_libraries.
    # install(FILES ...) registers the rule at configure time; the file only needs
    # to exist at install time (i.e. after the build step), so both prebuilt .a
    # files and compiled-output .a files are handled uniformly here.
    foreach(_agl_lib IN LISTS ARGN)
        if(EXISTS ${_agl_lib})
            ameba_install_files("${_agl_lib}")
        endif()
    endforeach()
endfunction()

function(ameba_layout_extract name ldfile origin end length)
    execute_process(
        COMMAND ${Python3_EXECUTABLE} ${c_SDK_EXTRACT_LD_SCRIPT} ${ldfile} ${name} ORIGIN
        RESULT_VARIABLE ret
        OUTPUT_VARIABLE ${origin}
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(
        COMMAND ${Python3_EXECUTABLE} ${c_SDK_EXTRACT_LD_SCRIPT} ${ldfile} ${name} END
        RESULT_VARIABLE ret
        OUTPUT_VARIABLE ${end}
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    math(EXPR ${length} "${${end}} - ${${origin}}")
    set(${origin} ${${origin}} PARENT_SCOPE)
    set(${end} ${${end}} PARENT_SCOPE)
    set(${length} ${${length}} PARENT_SCOPE)
endfunction()

########################################################################################################
#TODO: deprecated functions

#NOTE: For compatibility with version CMAKE_V0.1
function(ameba_internal_library name)
    ameba_add_internal_library(${name})
    set(CURRENT_LIB_NAME ${c_CURRENT_TARGET_NAME} PARENT_SCOPE)
endfunction()
