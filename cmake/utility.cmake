include_guard(GLOBAL)
include(${CMAKE_CURRENT_LIST_DIR}/utility_private.cmake)

set(c_AMEBA_LOG_LEVEL_DEBUG 0)
set(c_AMEBA_LOG_LEVEL_INFO 1)
set(c_AMEBA_LOG_LEVEL_NOTICE 2)
set(c_AMEBA_LOG_LEVEL_WARNING 3)
set(c_AMEBA_LOG_LEVEL_ERROR 4)
set(c_AMEBA_LOG_LEVEL_FATAL 5)
set(c_AMEBA_LOG_LEVEL_DISABLE 255)

if(NOT DEFINED v_AMEBA_LOG_LEVEL)
    set(v_AMEBA_LOG_LEVEL ${c_AMEBA_LOG_LEVEL_DEBUG})
endif()

macro(ameba_debug msg)
    if(v_AMEBA_LOG_LEVEL LESS_EQUAL c_AMEBA_LOG_LEVEL_DEBUG)
        _ameba_log_prefix_get()
        message("${C_COLOR_CYAN}[DEBUG][${_tmp_log_prefix}]${C_COLOR_RESET} ${msg}")
        unset(_tmp_log_prefix)
    endif()
endmacro()

macro(ameba_info msg)
    if(v_AMEBA_LOG_LEVEL LESS_EQUAL c_AMEBA_LOG_LEVEL_INFO)
        _ameba_log_prefix_get()
        message("${C_COLOR_GREEN}[INFOR][${_tmp_log_prefix}]${C_COLOR_RESET} ${msg}")
        unset(_tmp_log_prefix)
    endif()
endmacro()

macro(ameba_warning msg)
    if(v_AMEBA_LOG_LEVEL LESS_EQUAL c_AMEBA_LOG_LEVEL_WARNING)
        _ameba_log_prefix_get()
        message("${C_COLOR_YELLOW}[WARNG][${_tmp_log_prefix}]${C_COLOR_RESET} ${msg}")
        unset(_tmp_log_prefix)
    endif()
endmacro()

macro(ameba_fatal msg)
    if(v_AMEBA_LOG_LEVEL LESS_EQUAL c_AMEBA_LOG_LEVEL_FATAL)
        _ameba_log_prefix_get()
        message(SEND_ERROR "${C_COLOR_RED}[FATAL][${_tmp_log_prefix}]${C_COLOR_RESET} ${msg}")
        unset(_tmp_log_prefix)
    endif()
endmacro()


macro(ameba_option var description value)
    option(${var} ${description} "${value}")
endmacro()

macro(ameba_option_if condition var description value)
    set(options)
    set(oneValueArgs p_ELSE)
    set(multiValueArgs)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    if(DEFINED ${condition})
        if(${condition})
            # message("hehe: ${condition}, ${${condition}}, ${var}")
            option(${var} ${description} ${value})
        else()
            if(DEFINED ARG_p_ELSE)
                option(${var} ${description} ${ARG_p_ELSE})
            endif()
        endif()
    else()
        if(DEFINED ARG_p_ELSE)
            option(${var} ${description} ${ARG_p_ELSE})
        endif()
    endif()
endmacro()

macro(ameba_unset var)
    set(oneValueArgs p_SCOPE)

    cmake_parse_arguments(ameba_unset_ARG "" "${oneValueArgs}" "" ${ARGN})
    if (ameba_unset_ARG_p_SCOPE)
        if (${ameba_unset_ARG_p_SCOPE} STREQUAL "parent")
            unset(${var} PARENT_SCOPE)
        elseif(${ameba_unset_ARG_p_SCOPE} STREQUAL "current")
            unset(${var})
        elseif(${ameba_unset_ARG_p_SCOPE} STREQUAL "both")
            unset(${var} PARENT_SCOPE)
            unset(${var})
        else()
            ameba_fatal("Unknown scope value: ${ameba_unset_ARG_p_SCOPE}")
        endif()
    else()
        unset(${var})
    endif()
endmacro()

macro(ameba_set var)
    set(ameba_set_oneValueArgs p_SCOPE)

    cmake_parse_arguments(ameba_set_ARG "" "${ameba_set_oneValueArgs}" "" ${ARGN})
    set(tmp_argn "${ARGN}")

    if (ameba_set_ARG_p_SCOPE)
        _ameba_list_remove_key(tmp_argn p_SCOPE)
    else()
        set(ameba_set_ARG_p_SCOPE "current")
    endif()
    if (${ameba_set_ARG_p_SCOPE} STREQUAL "parent")
        set(${var} ${tmp_argn} PARENT_SCOPE)
    elseif(${ameba_set_ARG_p_SCOPE} STREQUAL "current")
        set(${var} ${tmp_argn})
    elseif(${ameba_set_ARG_p_SCOPE} STREQUAL "both")
        set(${var} ${tmp_argn} PARENT_SCOPE)
        set(${var} ${tmp_argn})
    else()
        ameba_fatal("Unknown scope value: ${ameba_set_ARG_p_SCOPE}")
    endif()
    unset(tmp_argn)
    unset(ameba_set_oneValueArgs)
    unset(ameba_set_ARG_p_SCOPE)
endmacro()

macro(ameba_set_upper var value)
    string(TOUPPER "${value}" _tmp_val)
    ameba_set(${var} ${_tmp_val} ${ARGN})
    unset(_tmp_val)
endmacro()

macro(ameba_set_if condition var)
    set(ameba_set_if_oneValueArgs p_SCOPE)
    set(ameba_set_if_multiValueArgs p_ELSE)

    cmake_parse_arguments(ameba_set_if_ARG "" "${ameba_set_if_oneValueArgs}" "${ameba_set_if_multiValueArgs}" ${ARGN})
    set(_tmp_ameba_set_if_ARGN "${ARGN}")
    if(DEFINED ameba_set_if_ARG_p_ELSE)
        _ameba_list_remove_key_and_followings(_tmp_ameba_set_if_ARGN p_ELSE)
    endif()

    if(DEFINED ameba_set_if_ARG_p_SCOPE)
        _ameba_list_remove_key(_tmp_ameba_set_if_ARGN p_SCOPE)
    else()
        set(ameba_set_if_ARG_p_SCOPE "current")
    endif()

    if(DEFINED ${condition})
        if(${condition})
            ameba_set(${var} ${_tmp_ameba_set_if_ARGN} p_SCOPE ${ameba_set_if_ARG_p_SCOPE})
        else()
            if(DEFINED ameba_set_if_ARG_p_ELSE)
                ameba_set(${var} ${ameba_set_if_ARG_p_ELSE} p_SCOPE ${ameba_set_if_ARG_p_SCOPE})
            endif()
        endif()
    else()
        if(DEFINED ameba_set_if_ARG_p_ELSE)
            ameba_set(${var} ${ameba_set_if_ARG_p_ELSE} p_SCOPE ${ameba_set_if_ARG_p_SCOPE})
        endif()
    endif()
    unset(_tmp_ameba_set_if_ARGN)
endmacro()

macro(ameba_set_if_unset var)
    if(NOT DEFINED ${var})
        ameba_set(${var} ${ARGN})
    endif()
endmacro()

macro(ameba_set_move new old)
    ameba_set(${new} "${${old}}" ${ARGN})
    ameba_unset(${old} ${ARGN})
endmacro()

macro(ameba_set_basedir var)
    get_filename_component(_tmp_ameba_set_basedir_val "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
    ameba_set(${var} ${_tmp_ameba_set_basedir_val} ${ARGN})
    unset(_tmp_ameba_set_basedir_val)
endmacro()

function(ameba_list_contains list_name item result)
    set(options)
    set(oneValueArgs p_PREFIX p_SUFFIX)
    set(multiValueArgs)

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    foreach(i IN LISTS ${list_name})
        if(${ARG_p_PREFIX}${i}${ARG_p_SUFFIX} STREQUAL ${item})
            set(${result} TRUE PARENT_SCOPE)
            return()
        endif()
    endforeach()
    set(${result} FALSE PARENT_SCOPE)
endfunction()

macro(ameba_list_remove_key list_name key)
    _ameba_list_remove_key(${list_name} ${key})
endmacro()

macro(ameba_list_append list_name)
    list(APPEND ${list_name} "${ARGN}")
endmacro()

macro(ameba_list_append_if condition list_name)
    set(ameba_list_append_if_options)
    set(ameba_list_append_if_oneValueArgs)
    set(ameba_list_append_if_multiValueArgs p_ELSE)

    cmake_parse_arguments(ARG "${ameba_list_append_if_options}" "${ameba_list_append_if_oneValueArgs}" "${ameba_list_append_if_multiValueArgs}" ${ARGN})

    set(__tmp_ameba_list_append_if_ARGN "${ARGN}")
    if(DEFINED ARG_p_ELSE)
        _ameba_list_remove_key_and_followings(__tmp_ameba_list_append_if_ARGN p_ELSE)
    endif()

    if(DEFINED ${condition})
        if (${condition})
            list(APPEND ${list_name} ${__tmp_ameba_list_append_if_ARGN})
        else()
            if(DEFINED ARG_p_ELSE)
                list(APPEND ${list_name} ${ARG_p_ELSE})
            endif()
        endif()
    else()
        if(DEFINED ARG_p_ELSE)
            list(APPEND ${list_name} ${ARG_p_ELSE})
        endif()
    endif()
endmacro()

macro(ameba_list_append_ifdef condition list_name)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs p_ELSE)

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(__tmp_ARGN "${ARGN}")
    if(DEFINED ARG_p_ELSE)
        _ameba_list_remove_key_and_followings(__tmp_ARGN p_ELSE)
    endif()
    if(DEFINED ${condition})
        list(APPEND ${list_name} ${__tmp_ARGN})
    else()
        if(DEFINED ARG_p_ELSE)
            list(APPEND ${list_name} ${ARG_p_ELSE})
        endif()
    endif()
endmacro()

macro(ameba_list_append_ifnot condition list_name)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs p_ELSE)

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(__tmp_ARGN "${ARGN}")
    if(DEFINED ARG_p_ELSE)
        _ameba_list_remove_key_and_followings(__tmp_ARGN p_ELSE)
    endif()

    if(DEFINED ${condition})
        if (NOT ${${condition}})
            list(APPEND ${list_name} ${__tmp_ARGN})
        else()
            if(DEFINED ARG_p_ELSE)
                list(APPEND ${list_name} ${ARG_p_ELSE})
            endif()
        endif()
    else()
        list(APPEND ${list_name} ${__tmp_ARGN})
    endif()
endmacro()

macro(ameba_normalize_path output_path input_path)
    if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.20")
        set(tmp_var ${input_path})
        cmake_path(NORMAL_PATH tmp_var OUTPUT_VARIABLE ${output_path})
        unset(tmp_var)
    else()
        set(${output_path} ${input_path})
        get_filename_component(${output_path} ${${output_path}} ABSOLUTE)
    endif()
endmacro()

########################################################################################################
function(ameba_build_info_gen output)
    set(options)
    set(oneValueArgs p_LIB_NAME)
    set(multiValueArgs)

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    get_filename_component(output_path ${output} DIRECTORY)
    if(ARG_p_LIB_NAME)
        set(_lib_name ${ARG_p_LIB_NAME})
    else()
        get_filename_component(_lib_name ${output} NAME_WE)
    endif()

    execute_process(
      COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
      WORKING_DIRECTORY ${output_path}
      OUTPUT_VARIABLE _git_ver
      RESULT_VARIABLE ERROR_CODE
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(ERROR_CODE)
        message(FATAL_ERROR "git version generate error! " ${ERROR_CODE})
    endif()

    string(TIMESTAMP _configuration_time "%Y/%m/%d-%H:%M:%S")

    configure_file(${CMAKE_FILES_DIR}/git_version.c.in ${output} @ONLY)
endfunction()
########################################################################################################

function(ameba_target_sources target)
    if(NOT TARGET ${target})
        if (d_AMEBA_REALSE)
            return()
        else()
            ameba_warning("call ameba_target_set for Non-target: ${target} ")
        endif()
    endif()

    set(options)
    set(oneValueArgs p_SCOPE)
    set(multiValueArgs)

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(__tmp_ARGN "${ARGN}")
    if (ARG_p_SCOPE)
        _ameba_list_remove_key(__tmp_ARGN p_SCOPE)
        if (${ARG_p_SCOPE} STREQUAL "interface")
            target_sources(${target} INTERFACE ${__tmp_ARGN})
        elseif(${ARG_p_SCOPE} STREQUAL "private")
            target_sources(${target} PRIVATE ${__tmp_ARGN})
        elseif(${ARG_p_SCOPE} STREQUAL "public")
            target_sources(${target} PUBLIC ${__tmp_ARGN})
        elseif(${ARG_p_SCOPE} STREQUAL "default")
            target_sources(${target} ${__tmp_ARGN})
        else()
            ameba_fatal("Unknown scope value: ${ARG_p_SCOPE}")
        endif()
    else()
        ameba_fatal("missa parameter: p_SCOPE")
    endif()
endfunction()

function(ameba_target_sources_if condition target)
    if(NOT TARGET ${target})
        if (d_AMEBA_REALSE)
            return()
        else()
            ameba_warning("call ameba_target_set for Non-target: ${target} ")
        endif()
    endif()

    set(options)
    set(oneValueArgs p_SCOPE)
    set(multiValueArgs p_ELSE)

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    set(__tmp_ARGN "${ARGN}")
    if(DEFINED ARG_p_ELSE)
        _ameba_list_remove_key_and_followings(__tmp_ARGN p_ELSE)
    endif()

    if(DEFINED ARG_p_SCOPE)
        _ameba_list_remove_key(__tmp_ARGN p_SCOPE)
    else()
        set(ARG_p_SCOPE "default")
    endif()

    if(DEFINED ${condition})
        if(${condition})
            ameba_target_sources(${target} ${__tmp_ARGN} p_SCOPE ${ARG_p_SCOPE})
        else()
            if(DEFINED ARG_p_ELSE)
                ameba_target_sources(${target} ${ARG_p_ELSE} p_SCOPE ${ARG_p_SCOPE})
            endif()
        endif()
    else()
        if(DEFINED ARG_p_ELSE)
            ameba_target_sources(${target} ${ARG_p_ELSE} p_SCOPE ${ARG_p_SCOPE})
        endif()
    endif()
    unset(__tmp_ARGN)
endfunction()

function(ameba_target_include target)
    if(NOT TARGET ${target})
        if (d_AMEBA_REALSE)
            return()
        else()
            ameba_warning("call ameba_target_set for Non-target: ${target} ")
        endif()
    endif()

    set(options)
    set(oneValueArgs p_SCOPE)
    set(multiValueArgs)

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (ARG_p_SCOPE)
        set(FIN_ARGS ${ARGN})
        list(REMOVE_ITEM FIN_ARGS "p_SCOPE" "${ARG_p_SCOPE}")
        if (${ARG_p_SCOPE} STREQUAL "interface")
            target_include_directories(${target} INTERFACE ${FIN_ARGS})
        elseif(${ARG_p_SCOPE} STREQUAL "private")
            target_include_directories(${target} PRIVATE ${FIN_ARGS})
        elseif(${ARG_p_SCOPE} STREQUAL "public")
            target_include_directories(${target} PUBLIC ${FIN_ARGS})
        elseif(${ARG_p_SCOPE} STREQUAL "default")
            target_include_directories(${target} PUBLIC ${FIN_ARGS})
        else()
            ameba_fatal("Unknown scope value: ${ARG_p_SCOPE}")
        endif()
    else()
        target_include_directories(${target} ${FIN_ARGS})
    endif()
endfunction()

function(ameba_target_include_if condition target)
    if(NOT TARGET ${target})
        if (d_AMEBA_REALSE)
            return()
        else()
            ameba_warning("call ameba_target_set for Non-target: ${target} ")
        endif()
    endif()

    if(DEFINED ${condition})
        if(${condition})
            ameba_target_include(${target} ${ARGN})
        endif()
    endif()
endfunction()

function(ameba_target_include_ifnot condition target)
    if(NOT TARGET ${target})
        if (d_AMEBA_REALSE)
            return()
        else()
            ameba_warning("call ameba_target_set for Non-target: ${target} ")
        endif()
    endif()

    if(DEFINED ${condition})
        if(NOT ${${condition}})
            ameba_target_include(${target} ${ARGN})
        endif()
    else()
        ameba_target_include(${target} ${ARGN})
    endif()
endfunction()

function(ameba_target_definitions target)
    if(NOT TARGET ${target})
        if (d_AMEBA_REALSE)
            return()
        else()
            ameba_warning("call ameba_target_set for Non-target: ${target} ")
        endif()
    endif()

    set(options)
    set(oneValueArgs p_SCOPE)
    set(multiValueArgs)

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(__tmp_ARGN "${ARGN}")
    if (ARG_p_SCOPE)
        _ameba_list_remove_key(__tmp_ARGN p_SCOPE)
        if (${ARG_p_SCOPE} STREQUAL "interface")
            target_compile_definitions(${target} INTERFACE ${__tmp_ARGN})
        elseif(${ARG_p_SCOPE} STREQUAL "private")
            target_compile_definitions(${target} PRIVATE ${__tmp_ARGN})
        elseif(${ARG_p_SCOPE} STREQUAL "public")
            target_compile_definitions(${target} PUBLIC ${__tmp_ARGN})
        elseif(${ARG_p_SCOPE} STREQUAL "default")
            target_compile_definitions(${target} ${__tmp_ARGN})
        else()
            ameba_fatal("Unknown scope value: ${ARG_p_SCOPE}")
        endif()
    else()
        target_compile_definitions(${target} ${__tmp_ARGN})
    endif()
endfunction()

function(ameba_target_definitions_if condition target)
    if(NOT TARGET ${target})
        if (d_AMEBA_REALSE)
            return()
        else()
            ameba_warning("call ameba_target_set for Non-target: ${target} ")
        endif()
    endif()

    set(options)
    set(oneValueArgs p_SCOPE)
    set(multiValueArgs p_ELSE)

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    set(__tmp_ARGN "${ARGN}")
    if(DEFINED ARG_p_ELSE)
        _ameba_list_remove_key_and_followings(__tmp_ARGN p_ELSE)
    endif()

    if(DEFINED ARG_p_SCOPE)
        _ameba_list_remove_key(__tmp_ARGN p_SCOPE)
    else()
        set(ARG_p_SCOPE "default")
    endif()

    if(DEFINED ${condition})
        if(${condition})
            ameba_target_definitions(${target} p_SCOPE ${ARG_p_SCOPE} ${__tmp_ARGN})
        else()
            if(DEFINED ARG_p_ELSE)
                ameba_target_definitions(${target} ${ARG_p_ELSE} p_SCOPE ${ARG_p_SCOPE})
            endif()
        endif()
    else()
        if(DEFINED ARG_p_ELSE)
            ameba_target_definitions(${target} ${ARG_p_ELSE} p_SCOPE ${ARG_p_SCOPE})
        endif()
    endif()
    unset(__tmp_ARGN)
endfunction()

function(ameba_target_link target)
    if(NOT TARGET ${target})
        if (d_AMEBA_REALSE)
            return()
        else()
            ameba_warning("call ameba_target_set for Non-target: ${target} ")
        endif()
    endif()

    set(options p_WHOLE_ARCHIVE)
    set(oneValueArgs p_SCOPE)
    set(multiValueArgs)

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(__tmp_ARGN ${ARGN})
    if(ARG_p_SCOPE)
        list(REMOVE_ITEM __tmp_ARGN p_SCOPE ${ARG_p_SCOPE})
    else()
        set(ARG_p_SCOPE private)
    endif()

    if(ARG_p_WHOLE_ARCHIVE)
        list(REMOVE_ITEM __tmp_ARGN p_WHOLE_ARCHIVE)
        set(__tmp_ARGN -Wl,--whole-archive ${__tmp_ARGN} -Wl,--no-whole-archive)
    endif()


    if (${ARG_p_SCOPE} STREQUAL "interface")
        target_link_libraries(${target} INTERFACE ${__tmp_ARGN})
    elseif(${ARG_p_SCOPE} STREQUAL "private")
        target_link_libraries(${target} PRIVATE ${__tmp_ARGN})
    elseif(${ARG_p_SCOPE} STREQUAL "public")
        target_link_libraries(${target} PUBLIC ${__tmp_ARGN})
    elseif(${ARG_p_SCOPE} STREQUAL "default")
        target_link_libraries(${target} ${__tmp_ARGN})
    else()
        ameba_fatal("Unknown scope value: ${ARG_p_SCOPE}")
    endif()
endfunction()

function(ameba_target_link_if condition target)
    if(NOT TARGET ${target})
        if (d_AMEBA_REALSE)
            return()
        else()
            ameba_warning("call ameba_target_set for Non-target: ${target} ")
        endif()
    endif()

    set(options p_WHOLE_ARCHIVE)
    set(oneValueArgs p_SCOPE)
    set(multiValueArgs p_ELSE)

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    set(__tmp_ARGN "${ARGN}")
    if(DEFINED ARG_p_ELSE)
        _ameba_list_remove_key_and_followings(__tmp_ARGN p_ELSE)
    endif()

    if(DEFINED ARG_p_SCOPE)
        _ameba_list_remove_key(__tmp_ARGN p_SCOPE)
    else()
        set(ARG_p_SCOPE "default")
    endif()

    if(DEFINED ${condition})
        if(NOT ${${condition}})
            if(DEFINED ARG_p_ELSE)
                set(__tmp_ARGN ${ARG_p_ELSE})
            else()
                set(__tmp_ARGN)
            endif()
        endif()
    else()
        if(DEFINED ARG_p_ELSE)
            set(__tmp_ARGN ${ARG_p_ELSE})
        else()
            set(__tmp_ARGN)
        endif()
    endif()

    if(__tmp_ARGN)
        if(ARG_p_WHOLE_ARCHIVE)
            list(REMOVE_ITEM __tmp_ARGN p_WHOLE_ARCHIVE)
            ameba_target_link(${target} ${__tmp_ARGN} p_SCOPE ${ARG_p_SCOPE} p_WHOLE_ARCHIVE)
        else()
            ameba_target_link(${target} ${__tmp_ARGN} p_SCOPE ${ARG_p_SCOPE})
        endif()
    endif()
endfunction()

function(ameba_target_link_ifnot condition target)
    if(NOT TARGET ${target})
        if (d_AMEBA_REALSE)
            return()
        else()
            ameba_warning("call ameba_target_set for Non-target: ${target} ")
        endif()
    endif()

    if(DEFINED ${condition})
        if(NOT ${${condition}})
            ameba_target_link(${target} ${ARGN})
        endif()
    else()
        ameba_target_link(${target} ${ARGN})
    endif()
endfunction()

function(ameba_target_link_options target)
    if(NOT TARGET ${target})
        if (d_AMEBA_REALSE)
            return()
        else()
            ameba_warning("call ameba_target_set for Non-target: ${target} ")
        endif()
    endif()

    set(options)
    set(oneValueArgs p_SCOPE)
    set(multiValueArgs)

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (ARG_p_SCOPE)
        set(FIN_ARGS ${ARGN})
        list(REMOVE_ITEM FIN_ARGS "p_SCOPE" "${ARG_p_SCOPE}")
        if (${ARG_p_SCOPE} STREQUAL "interface")
            target_link_options(${target} INTERFACE ${FIN_ARGS})
        elseif(${ARG_p_SCOPE} STREQUAL "private")
            target_link_options(${target} PRIVATE ${FIN_ARGS})
        elseif(${ARG_p_SCOPE} STREQUAL "public")
            target_link_options(${target} PUBLIC ${FIN_ARGS})
        elseif(${ARG_p_SCOPE} STREQUAL "default")
            target_link_options(${target} ${FIN_ARGS})
        else()
            ameba_fatal("Unknown scope value: ${ARG_p_SCOPE}")
        endif()
    else()
        target_link_options(${target} ${FIN_ARGS})
    endif()
endfunction()

function(ameba_target_link_options_if condition target)
    if(NOT TARGET ${target})
        if (d_AMEBA_REALSE)
            return()
        else()
            ameba_warning("call ameba_target_set for Non-target: ${target} ")
        endif()
    endif()

    set(options)
    set(oneValueArgs p_SCOPE)
    set(multiValueArgs p_ELSE)

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    set(__tmp_ARGN "${ARGN}")
    if(DEFINED ARG_p_ELSE)
        _ameba_list_remove_key_and_followings(__tmp_ARGN p_ELSE)
    endif()

    if(DEFINED ARG_p_SCOPE)
        _ameba_list_remove_key(__tmp_ARGN p_SCOPE)
    else()
        set(ARG_p_SCOPE "default")
    endif()

    if(DEFINED ${condition})
        if(${condition})
            ameba_target_link_options(${target} ${__tmp_ARGN} p_SCOPE ${ARG_p_SCOPE})
        else()
            if(DEFINED ARG_p_ELSE)
                ameba_target_link_options(${target} ${ARG_p_ELSE} p_SCOPE ${ARG_p_SCOPE})
            endif()
        endif()
    else()
        if(DEFINED ARG_p_ELSE)
            ameba_target_link_options(${target} ${ARG_p_ELSE} p_SCOPE ${ARG_p_SCOPE})
        endif()
    endif()
    unset(__tmp_ARGN)
endfunction()

function(ameba_target_compile_options target)
    if(NOT TARGET ${target})
        if (d_AMEBA_REALSE)
            return()
        else()
            ameba_warning("call ameba_target_set for Non-target: ${target} ")
        endif()
    endif()

    set(options)
    set(oneValueArgs p_SCOPE)
    set(multiValueArgs)

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (ARG_p_SCOPE)
        set(FIN_ARGS ${ARGN})
        list(REMOVE_ITEM FIN_ARGS "p_SCOPE" "${ARG_p_SCOPE}")
        if (${ARG_p_SCOPE} STREQUAL "interface")
            target_compile_options(${target} INTERFACE ${FIN_ARGS})
        elseif(${ARG_p_SCOPE} STREQUAL "private")
            target_compile_options(${target} PRIVATE ${FIN_ARGS})
        elseif(${ARG_p_SCOPE} STREQUAL "public")
            target_compile_options(${target} PUBLIC ${FIN_ARGS})
        elseif(${ARG_p_SCOPE} STREQUAL "default")
            target_compile_options(${target} ${FIN_ARGS})
        else()
            ameba_fatal("Unknown scope value: ${ARG_p_SCOPE}")
        endif()
    else()
        target_compile_options(${target} ${FIN_ARGS})
    endif()
endfunction()

function(ameba_target_compile_options_if condition target)
    if(NOT TARGET ${target})
        if (d_AMEBA_REALSE)
            return()
        else()
            ameba_warning("call ameba_target_set for Non-target: ${target} ")
        endif()
    endif()

    set(options)
    set(oneValueArgs p_SCOPE)
    set(multiValueArgs p_ELSE)

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    set(__tmp_ARGN "${ARGN}")
    if(DEFINED ARG_p_ELSE)
        _ameba_list_remove_key_and_followings(__tmp_ARGN p_ELSE)
    endif()

    if(DEFINED ARG_p_SCOPE)
        _ameba_list_remove_key(__tmp_ARGN p_SCOPE)
    else()
        set(ARG_p_SCOPE "default")
    endif()

    if(DEFINED ${condition})
        if(${condition})
            ameba_target_compile_options(${target} ${__tmp_ARGN} p_SCOPE ${ARG_p_SCOPE})
        else()
            if(DEFINED ARG_p_ELSE)
                ameba_target_compile_options(${target} ${ARG_p_ELSE} p_SCOPE ${ARG_p_SCOPE})
            endif()
        endif()
    else()
        if(DEFINED ARG_p_ELSE)
            ameba_target_compile_options(${target} ${ARG_p_ELSE} p_SCOPE ${ARG_p_SCOPE})
        endif()
    endif()
    unset(__tmp_ARGN)
endfunction()

function(ameba_target_remove_compile_options target)
    if(NOT TARGET ${target})
        if (d_AMEBA_REALSE)
            return()
        else()
            ameba_warning("call ameba_target_set for Non-target: ${target} ")
        endif()
    endif()

    set(options)
    set(oneValueArgs)
    set(multiValueArgs)

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    get_target_property(_tmp_compile_options ${target} COMPILE_OPTIONS)
    if (_tmp_compile_options)
        foreach(c IN LISTS ARGN)
            list(REMOVE_ITEM _tmp_compile_options "${c}")
        endforeach()

        if (_tmp_compile_options)
            set_target_properties(${target} PROPERTIES COMPILE_OPTIONS "${_tmp_compile_options}")
        endif()
    endif()
endfunction()

function(ameba_target_remove_compile_options_if condition target)
    if(NOT TARGET ${target})
        if (d_AMEBA_REALSE)
            return()
        else()
            ameba_warning("call ameba_target_set for Non-target: ${target} ")
        endif()
    endif()

    set(options)
    set(oneValueArgs)
    set(multiValueArgs p_ELSE)

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    set(__tmp_ARGN "${ARGN}")
    if(DEFINED ARG_p_ELSE)
        _ameba_list_remove_key_and_followings(__tmp_ARGN p_ELSE)
    endif()

    if(DEFINED ${condition})
        if(${condition})
            ameba_target_remove_compile_options(${target} ${__tmp_ARGN})
        else()
            if(DEFINED ARG_p_ELSE)
                ameba_target_remove_compile_options(${target} ${ARG_p_ELSE})
            endif()
        endif()
    else()
        if(DEFINED ARG_p_ELSE)
            ameba_target_remove_compile_options(${target} ${ARG_p_ELSE})
        endif()
    endif()
    unset(__tmp_ARGN)
endfunction()


function(ameba_target_depend target)
    if(NOT TARGET ${target})
        if (d_AMEBA_REALSE)
            return()
        else()
            ameba_warning("call ameba_target_set for Non-target: ${target} ")
        endif()
    endif()

    if(TARGET ${target})
        add_dependencies(${target} ${ARGN})
    else()
        ameba_warning("Try add dependencies for non-target: ${target}, dependencies: ${ARGN}")
    endif()
endfunction()

function(ameba_target_depend_if condition target)
    if(NOT TARGET ${target})
        if (d_AMEBA_REALSE)
            return()
        else()
            ameba_warning("call ameba_target_set for Non-target: ${target} ")
        endif()
    endif()

    set(options)
    set(oneValueArgs)
    set(multiValueArgs p_ELSE)

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    set(__tmp_ARGN "${ARGN}")
    if(DEFINED ARG_p_ELSE)
        _ameba_list_remove_key_and_followings(__tmp_ARGN p_ELSE)
    endif()

    if(DEFINED ${condition})
        if(${condition})
            add_dependencies(${target} ${__tmp_ARGN})
        else()
            if(DEFINED ARG_p_ELSE)
                add_dependencies(${target} ${ARG_p_ELSE})
            endif()
        endif()
    else()
        if(DEFINED ARG_p_ELSE)
            add_dependencies(${target} ${ARG_p_ELSE})
        endif()
    endif()
endfunction()

function(ameba_add_subdirectories)
    foreach(dir IN LISTS ARGN)
        add_subdirectory(${dir})
    endforeach()
endfunction()

function(ameba_add_subdirectories_if condition)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs p_ELSE)

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    set(__tmp_ARGN "${ARGN}")
    if(DEFINED ARG_p_ELSE)
        _ameba_list_remove_key_and_followings(__tmp_ARGN p_ELSE)
    endif()

    if(DEFINED ${condition})
        if(${condition})
            foreach(dir IN LISTS __tmp_ARGN)
                add_subdirectory(${dir})
            endforeach()
        else()
            if(DEFINED ARG_p_ELSE)
                foreach(dir IN LISTS ARG_p_ELSE)
                    add_subdirectory(${dir})
                endforeach()
            endif()
        endif()
    else()
        if(DEFINED ARG_p_ELSE)
            foreach(dir IN LISTS ARG_p_ELSE)
                add_subdirectory(${dir})
            endforeach()
        endif()
    endif()
endfunction()

function(ameba_add_subdirectories_ifnot condition)
    if(DEFINED ${condition})
        if(NOT ${${condition}})
            foreach(dir IN LISTS ARGN)
                add_subdirectory(${dir})
            endforeach()
        endif()
    else()
        foreach(dir IN LISTS ARGN)
            add_subdirectory(${dir})
        endforeach()
    endif()
endfunction()

function(ameba_execute)
    foreach(cmd IN LISTS ARGN)
        execute_process(COMMAND "${cmd}"
            OUTPUT_VARIABLE output
            ERROR_VARIABLE error
        )
        message("execute: ${cmd}, ${output}, ${error}")
    endforeach()
endfunction()

function(ameba_get_link_libraries TARGET OUTPUT_LIST)
    get_target_property(IMPORTED ${TARGET} IMPORTED)
    list(APPEND VISITED_TARGETS ${TARGET})
    if (IMPORTED)
        get_target_property(LIBS ${TARGET} INTERFACE_LINK_LIBRARIES)
    else()
        get_target_property(LIBS ${TARGET} LINK_LIBRARIES)
    endif()
    set(LIB_FILES "")
    foreach(LIB ${LIBS})
        if (TARGET ${LIB})
            list(FIND VISITED_TARGETS ${LIB} VISITED)
            if (${VISITED} EQUAL -1)
                get_target_property(LIB_FILE ${LIB} LOCATION)
                ameba_get_link_libraries(${LIB} LINK_LIB_FILES)
                list(APPEND LIB_FILES ${LIB_FILE} ${LINK_LIB_FILES})
            endif()
        endif()
    endforeach()
    set(VISITED_TARGETS ${VISITED_TARGETS} PARENT_SCOPE)
    set(${OUTPUT_LIST} ${LIB_FILES} PARENT_SCOPE)
endfunction()

function(ameba_target_get_include_directories TARGET result)
    set(tmp_result)
    get_target_property(dirs ${TARGET} INTERFACE_INCLUDE_DIRECTORIES)
    if(dirs)
        list(APPEND tmp_result ${dirs})
    endif()

    get_target_property(libs ${TARGET} INTERFACE_LINK_LIBRARIES)
    foreach(lib ${libs})
        if (TARGET ${lib})
            get_target_property(TARGET_TYPE ${lib} TYPE)
            if(${TARGET_TYPE} STREQUAL "INTERFACE_LIBRARY")
                ameba_target_get_include_directories(${lib} dirs)
                if (dirs)
                    list(APPEND tmp_result ${dirs})
                endif()
            endif()
        endif()
    endforeach()

    set(${result} ${tmp_result} PARENT_SCOPE)
endfunction()

function(ameba_target_get_sources target result)
    set(tmp_result)
    get_target_property(srcs ${target} INTERFACE_SOURCES)
    if(srcs)
        list(APPEND tmp_result ${srcs})
    endif()

    get_target_property(libs ${target} INTERFACE_LINK_LIBRARIES)
    foreach(lib ${libs})
        if (TARGET ${lib})
            get_target_property(TARGET_TYPE ${lib} TYPE)
            if(${TARGET_TYPE} STREQUAL "INTERFACE_LIBRARY")
                ameba_target_get_sources(${lib} srcs)
                if (srcs)
                    list(APPEND tmp_result ${srcs})
                endif()
            endif()
        endif()
    endforeach()

    set(${result} ${tmp_result} PARENT_SCOPE)
endfunction()

function(ameba_target_get_sources target result)
    ameba_target_get_property_recursive(${target} sources tmp_result)
    set(${result} ${tmp_result} PARENT_SCOPE)
endfunction()

function(ameba_target_get_compile_options target result)
    ameba_target_get_property_recursive(${target} compile_options tmp_result)
    set(${result} ${tmp_result} PARENT_SCOPE)
endfunction()

function(ameba_target_get_compile_defines target result)
    ameba_target_get_property_recursive(${target} compile_defines tmp_result)
    set(${result} ${tmp_result} PARENT_SCOPE)
endfunction()

function(ameba_collect_interface_library_properties)
    set(oneValueArgs
        p_IGNORED_TARGETS
        p_SOURCES
        p_INCLUDES
        p_COMPILE_OPTIONS
        p_COMPILE_DEFINES
        p_LINK_LIBRARY
    )

    cmake_parse_arguments(ARG "" "${oneValueArgs}" "" ${ARGN})
    set(tmp_ARGN "${ARGN}")
    list(REMOVE_ITEM tmp_ARGN ${oneValueArgs} ${ARG_p_IGNORED_TARGETS})
    foreach(a IN LISTS oneValueArgs)
        if (ARG_${a})
            list(REMOVE_ITEM tmp_ARGN ${a} ${ARG_${a}})
        endif()
    endforeach()
    list(REMOVE_ITEM oneValueArgs p_IGNORED_TARGETS)

    set(ignored_targets)
    foreach(p IN LISTS oneValueArgs) # traverse properties
        if(NOT ARG_${p})
            continue()
        endif()
        string(REGEX REPLACE "^p_" "" u_property ${p})
        string(TOLOWER ${u_property} property)
        set(prop_results)
        foreach(l IN LISTS tmp_ARGN) # traverse targets
            if(TARGET ${l})
                get_target_property(_type ${l} TYPE)
                if(${_type} STREQUAL "INTERFACE_LIBRARY")
                    ameba_target_get_property_recursive(${l} ${property} tmp_result)
                    ameba_list_append_if(tmp_result prop_results ${tmp_result})
                else()
                    ameba_list_append(ignored_targets ${ARG_p_IGNORED_TARGETS} ${l})
                endif()
            elseif(${l} MATCHES "^\\-Wl")
                ameba_list_append(ignored_targets ${ARG_p_IGNORED_TARGETS} ${l})
            else()
                ameba_fatal("not a target: ${l}")
            endif()
        endforeach()
        ameba_set_if(prop_results ${ARG_${p}} ${prop_results} p_SCOPE parent)
    endforeach()
    if(ARG_p_IGNORED_TARGETS)
        list(REMOVE_DUPLICATES ignored_targets)
        set(${ARG_p_IGNORED_TARGETS} ${ignored_targets} PARENT_SCOPE)
    endif()
endfunction()

function(ameba_target_get_link_libraries TARGET result)
    set(tmp_result)
    get_target_property(libs ${TARGET} LINK_LIBRARIES)
    if(libs)
        list(APPEND tmp_result ${libs})
    endif()

    get_target_property(libs ${TARGET} INTERFACE_LINK_LIBRARIES)
    if(libs)
        foreach(lib ${libs})
            if (TARGET ${lib})
                get_target_property(TARGET_TYPE ${lib} TYPE)
                if(${TARGET_TYPE} STREQUAL "INTERFACE_LIBRARY")
                    ameba_target_get_link_libraries(${lib} libs)
                    if (libs)
                        list(APPEND tmp_result ${libs})
                    endif()
                else()
                    list(APPEND tmp_result ${lib})
                endif()
            else()
                list(APPEND tmp_result ${lib})
            endif()
        endforeach()
    endif()

    set(${result} ${tmp_result} PARENT_SCOPE)
endfunction()


function(ameba_target_get_output_info target output_path ouput_name)
    get_target_property(TARGET_OUTPUT_DIRECTORY ${target} ARCHIVE_OUTPUT_DIRECTORY)
    get_target_property(TARGET_PREFIX ${target} PREFIX)
    get_target_property(TARGET_OUTPUT_NAME ${target} OUTPUT_NAME)
    get_target_property(TARGET_SUFFIX ${target} SUFFIX)
    get_target_property(TARGET_TYPE ${target} TYPE)

    if(NOT TARGET_OUTPUT_DIRECTORY)
        set(TARGET_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
    endif()

    if(NOT TARGET_OUTPUT_NAME)
        set(TARGET_OUTPUT_NAME "${target}")
    endif()

    if(NOT TARGET_PREFIX)
        if (TARGET_TYPE)
            set(TARGET_PREFIX ${CMAKE_${TARGET_TYPE}_PREFIX})
        else()
            unset(TARGET_PREFIX)
        endif()
    endif()

    if(NOT TARGET_SUFFIX)
        if (TARGET_TYPE)
            set(TARGET_SUFFIX ${CMAKE_${TARGET_TYPE}_SUFFIX})
        else()
            unset(TARGET_SUFFIX)
        endif()
    endif()

    set(${output_path} ${TARGET_OUTPUT_DIRECTORY} PARENT_SCOPE)
    set(${ouput_name} "${TARGET_PREFIX}${TARGET_OUTPUT_NAME}${TARGET_SUFFIX}" PARENT_SCOPE)
endfunction()

function(ameba_source_set_property source)
    set(options
        p_APPEND                    # If set, property will be appened to the given source
    )

    set(multiValueArgs
        p_INCLUDES                  # Set source include directories
        p_COMPILE_OPTIONS           # Set source compile options
        p_COMPILE_DEFINES           # Set source compile definitions

        # p_TARGET                    # If set, property will only bind to target which contains the source
        p_SCOPE_DIRS                # Set property scope, refer to set_property on cmake.org for more detail
    )
    cmake_parse_arguments(ARG "${options}" "" "${multiValueArgs}" ${ARGN})

    if (ARG_p_APPEND)
        set(ARG_p_APPEND APPEND)
    else()
        unset(ARG_p_APPEND)
    endif()

    if (ARG_p_SCOPE_DIRS)
        set(ARG_p_SCOPE_DIRS DIRECTORY ${ARG_p_SCOPE_DIRS})
    else()
        set(ARG_p_SCOPE_DIRS DIRECTORY ${d_MCU_PROJECT_DIR})
    endif()

    if(ARG_p_INCLUDES)
        set_property(
            SOURCE ${source}
            ${ARG_p_SCOPE_DIRS}
            ${ARG_p_APPEND} PROPERTY
            includes ${ARG_p_INCLUDES}
        )
        set_source_files_properties(${source} PROPERTIES INCLUDE_DIRECTORIES "${ARG_p_INCLUDES}")
    endif()
    if(ARG_p_COMPILE_OPTIONS)
        set_property(
            SOURCE ${source}
            ${ARG_p_SCOPE_DIRS}
            ${ARG_p_APPEND} PROPERTY
            compile_options ${ARG_p_COMPILE_OPTIONS}
        )
        set_source_files_properties(${source} PROPERTIES COMPILE_OPTIONS "${ARG_p_COMPILE_OPTIONS}")
    endif()
    if(ARG_p_COMPILE_DEFINES)
        set_property(
            SOURCE ${source}
            ${ARG_p_SCOPE_DIRS}
            ${ARG_p_APPEND} PROPERTY
            compile_defines ${ARG_p_COMPILE_DEFINES}
        )
        set_source_files_properties(${source} PROPERTIES COMPILE_DEFINITIONS "${ARG_p_COMPILE_DEFINES}")
    endif()
endfunction()

function(ameba_source_get_property source)
    set(oneValueArgs
        p_INCLUDES                  # Set source include directories
        p_COMPILE_OPTIONS           # Set source compile options
        p_COMPILE_DEFINES           # Set source compile definitions
    )

    set(multiValueArgs
        # p_TARGET                    # If set, property will only bind to target which contains the source
        p_SCOPE_DIRS                # Get property scope, refer to set_property on cmake.org for more detail
    )
    cmake_parse_arguments(ARG "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (ARG_p_SCOPE_DIRS)
        set(ARG_p_SCOPE_DIRS DIRECTORY ${ARG_p_SCOPE_DIRS})
    else()
        set(ARG_p_SCOPE_DIRS DIRECTORY ${d_MCU_PROJECT_DIR})
    endif()

    if(ARG_p_INCLUDES)
        get_property(
            tmp_var
            SOURCE ${source}
            ${ARG_p_SCOPE_DIRS}
            PROPERTY includes
        )
        get_source_file_property(tmp_var2 ${source} ${ARG_p_SCOPE_DIRS} INCLUDE_DIRECTORIES)
        ameba_list_append_if(tmp_var2 tmp_var ${tmp_var2})
        list(REMOVE_DUPLICATES tmp_var)
        set(${ARG_p_INCLUDES} ${tmp_var} PARENT_SCOPE)
    endif()
    if(ARG_p_COMPILE_OPTIONS)
        get_property(
            tmp_var
            SOURCE ${source}
            ${ARG_p_SCOPE_DIRS}
            PROPERTY compile_options
        )
        get_source_file_property(tmp_var2 ${source} ${ARG_p_SCOPE_DIRS} COMPILE_OPTIONS)
        ameba_list_append_if(tmp_var2 tmp_var ${tmp_var2})
        list(REMOVE_DUPLICATES tmp_var)
        set(${ARG_p_COMPILE_OPTIONS} ${tmp_var} PARENT_SCOPE)
    endif()
    if(ARG_p_COMPILE_DEFINES)
        get_property(
            tmp_var
            SOURCE ${source}
            ${ARG_p_SCOPE_DIRS}
            PROPERTY compile_defines
        )
        get_source_file_property(tmp_var2 ${source} ${ARG_p_SCOPE_DIRS} COMPILE_DEFINITIONS)
        ameba_list_append_if(tmp_var2 tmp_var ${tmp_var2})
        list(REMOVE_DUPLICATES tmp_var)
        set(${ARG_p_COMPILE_DEFINES} ${tmp_var} PARENT_SCOPE)
    endif()
endfunction()

function(ameba_source_property_inherite source)
    ameba_source_get_property(${source}
        p_INCLUDES _ins
        p_COMPILE_OPTIONS _opts
        p_COMPILE_DEFINES _defs
    )
    ameba_source_set_property(${source}
        p_INCLUDES ${_ins}
        p_COMPILE_OPTIONS ${_opts}
        p_COMPILE_DEFINES ${_defs}
    )
endfunction()

function(ameba_target_add name)
    set(options
        p_WRAP_TARGET_NAME              # If set, target's name will be wrapped as ${name}_${PROJECT_NAME}
        p_ENABLE_TARGET_INFO_LIST       # If set, target's compile/link/include/sources info are stored to associated lists
                                        # List's name is formated as ${d_CURRENT_TARGET_NAME}_[compile|link|include|source]
                                        # List will be updated automaticlly by other ameba_target_* APIs
                                        # If p_TYPE is interface, this option will be enabled automaticlly
        p_DISABLE_TARGET_INFO_LIST      # If set, p_ENABLE_TARGET_INFO_LIST will be diabled
        p_STRIP_DEBUG                   # If set, target will be proecessed by [objcopy -g]
        p_ENABLE_DETERMINISTIC_ARCHIVES # If set, target will be proecessed by [objcopy -D]
        p_ADD_EMPTY_C_FILE              # If set, an empty c file is added to the target sources
        p_ADD_BUILD_INFO                # Set target compile with build info: git version, time, etc..
        p_DROP_IF_NO_SOURCES            # If set, target will not be added if no sources found
    )
    set(oneValueArgs
        p_TYPE                      # Set target type, one of [static|shared|executable], default: static
        p_OUTPUT_PATH               # Set target output path, default: by cmake rule
        p_OUTPUT_NAME               # Set target output name, default: ${name}
        p_PREFIX                    # Set target output prefix, default: lib(for static|shared)
        p_SUFFIX                    # Set target output suffix, default: [.a|.so|.exe]

        p_LINKED_BY_SCOPE           # Set link scope for p_LINKED_BY_LIBRARIES option
    )
    set(multiValueArgs
        p_SOURCES                   # Set target source files
        p_INCLUDES                  # Set target include directories
        p_COMPILE_OPTIONS           # Set target compile options
        p_COMPILE_DEFINES           # Set target compile definitions
        p_LINK_OPTIONS              # Set target link options
        p_LINK_LIBRARY              # Set target link libraries
        p_DEPENDENCIES              # Set target dependencies
        p_APPEND_TO_LIST            # Append target to list(properties) used in top level
        p_REMOVE_COMPILE_OPTIONS    # Remove compile options(maybe from global config) for the target.

        p_LINKED_BY_LIBRARIES       # Set target being linked by other libraries
        p_DEPENDENTED_BY_TARGETS    # Set target being linked by other libraries
    )
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    ameba_set_if(ARG_p_WRAP_TARGET_NAME d_CURRENT_TARGET_NAME "${name}_${PROJECT_NAME}" p_ELSE ${name} p_SCOPE both)

    #Some validation check
    if (ARG_p_OUTPUT_PATH)
        if(${ARG_p_OUTPUT_PATH} STREQUAL ${d_SDK_LIB_APPLICATION_DIR} OR ${ARG_p_OUTPUT_PATH} STREQUAL ${d_SDK_LIB_SOC_DIR})
            if(NOT ARG_p_STRIP_DEBUG OR NOT ARG_p_ENABLE_DETERMINISTIC_ARCHIVES)
                ameba_warning("No set p_STRIP_DEBUG OR p_ENABLE_DETERMINISTIC_ARCHIVES for app/soc target: ${name}:${ARG_p_OUTPUT_PATH}")
            endif()

            if(ARG_p_APPEND_TO_LIST)
                ameba_warning("Set p_APPEND_TO_LIST for app/soc target: ${name}")
            endif()

            if(NOT ARG_p_OUTPUT_NAME)
                ameba_warning("Not set p_OUTPUT_NAME for app/soc target: ${name}")
            endif()
        else()
            if(ARG_p_STRIP_DEBUG OR ARG_p_ENABLE_DETERMINISTIC_ARCHIVES)
                ameba_warning("Set p_STRIP_DEBUG OR p_ENABLE_DETERMINISTIC_ARCHIVES for non-app/soc target: ${name}:${ARG_p_OUTPUT_PATH}")
            endif()
            if(NOT ARG_p_APPEND_TO_LIST AND NOT ARG_p_LINKED_BY_LIBRARIES)
                ameba_warning("NO set p_APPEND_TO_LIST for non-app/soc target: ${name}:${ARG_p_OUTPUT_PATH}")
            endif()
        endif()
    else()
        if(ARG_p_STRIP_DEBUG OR ARG_p_ENABLE_DETERMINISTIC_ARCHIVES)
            ameba_warning("Set p_STRIP_DEBUG OR p_ENABLE_DETERMINISTIC_ARCHIVES for non-app/soc target: ${name}")
        endif()
        if(NOT ARG_p_APPEND_TO_LIST AND NOT ARG_p_LINKED_BY_LIBRARIES)
            ameba_warning("NO set p_APPEND_TO_LIST for non-app/soc target: ${name}")
        endif()
    endif()

    if (ARG_p_LINK_LIBRARY)
        ameba_collect_interface_library_properties(
            p_IGNORED_TARGETS   non_interface_libs
            p_SOURCES           srcs_from_libs
            p_INCLUDES          incs_from_libs
            p_COMPILE_OPTIONS   ops_from_libs
            p_COMPILE_DEFINES   def_from_libs
            ${ARG_p_LINK_LIBRARY}
        )
    endif()

    if (ARG_p_DROP_IF_NO_SOURCES)
        if(NOT ARG_p_ADD_EMPTY_C_FILE AND NOT ARG_p_SOURCES AND NOT srcs_from_libs)
            ameba_warning("target ${d_CURRENT_TARGET_NAME} has no source, abort")
            return()
        endif()
    endif()

    ameba_set_if_unset(ARG_p_OUTPUT_NAME ${name})
    ameba_set_if_unset(ARG_p_ENABLE_TARGET_INFO_LIST OFF)
    ameba_set_if_unset(ARG_p_DISABLE_TARGET_INFO_LIST OFF)
    ameba_set_if_unset(ARG_p_TYPE static)

    if (${ARG_p_TYPE} STREQUAL "static")
        add_library(${d_CURRENT_TARGET_NAME} STATIC "")
        ameba_set_if_unset(ARG_p_PREFIX ${CMAKE_STATIC_LIBRARY_PREFIX})
        ameba_set_if_unset(ARG_p_SUFFIX ${CMAKE_STATIC_LIBRARY_SUFFIX})
    elseif (${ARG_p_TYPE} STREQUAL "shared")
        add_library(${d_CURRENT_TARGET_NAME} SHARED "")
        ameba_set_if_unset(ARG_p_PREFIX ${CMAKE_SHARED_LIBRARY_PREFIX})
        ameba_set_if_unset(ARG_p_SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX})
    elseif (${ARG_p_TYPE} STREQUAL "executable")
        set(_tmp_exe_sources_)
        ameba_list_append_if(ARG_p_ADD_EMPTY_C_FILE _tmp_exe_sources_ ${c_EMPTY_C_FILE})
        ameba_list_append_if(ARG_p_SOURCES _tmp_exe_sources_ ${ARG_p_SOURCES})
        add_executable(${d_CURRENT_TARGET_NAME} ${_tmp_exe_sources_})
        unset(_tmp_exe_sources_)
        ameba_set_if_unset(ARG_p_SUFFIX ${CMAKE_EXECUTABLE_SUFFIX})
    elseif (${ARG_p_TYPE} STREQUAL "interface")
        add_library(${d_CURRENT_TARGET_NAME} INTERFACE)
        ameba_set(ARG_p_ENABLE_TARGET_INFO_LIST ON)
    else()
        ameba_fatal("Unknown library type: ${ARG_p_TYPE}")
    endif()

    if(NOT ARG_p_DISABLE_TARGET_INFO_LIST AND ARG_p_ENABLE_TARGET_INFO_LIST)
        set(_tmp_enable_target_info_list TRUE)
        set(${d_CURRENT_TARGET_NAME}_sources "")
        set(${d_CURRENT_TARGET_NAME}_includes "")
        set(${d_CURRENT_TARGET_NAME}_compile_options "")
        set(${d_CURRENT_TARGET_NAME}_compile_defines "")
        set(${d_CURRENT_TARGET_NAME}_link_options "")
        set(${d_CURRENT_TARGET_NAME}_link_libraries "")
        set(${d_CURRENT_TARGET_NAME}_dependencies "")
    else()
        set(_tmp_enable_target_info_list FALSE)
    endif()

    if(ARG_p_PREFIX)
        set_target_properties(${d_CURRENT_TARGET_NAME} PROPERTIES PREFIX ${ARG_p_PREFIX})
    endif()
    if(ARG_p_SUFFIX)
        set_target_properties(${d_CURRENT_TARGET_NAME} PROPERTIES SUFFIX ${ARG_p_SUFFIX})
    endif()
    if(ARG_p_OUTPUT_NAME)
        set_target_properties(${d_CURRENT_TARGET_NAME} PROPERTIES OUTPUT_NAME ${ARG_p_OUTPUT_NAME})
    endif()

    if (NOT ${ARG_p_TYPE} STREQUAL "executable")
        if (ARG_p_SOURCES)
            target_sources(${d_CURRENT_TARGET_NAME} PRIVATE ${ARG_p_SOURCES})
            ameba_list_append_ifdef(${d_CURRENT_TARGET_NAME}_sources ${d_CURRENT_TARGET_NAME}_sources ${ARG_p_SOURCES})
        endif()

        if (ARG_p_ADD_EMPTY_C_FILE)
            target_sources(${d_CURRENT_TARGET_NAME} PRIVATE ${c_EMPTY_C_FILE})
        endif()
    endif()

    if (ARG_p_ADD_BUILD_INFO)
        ameba_debug("add build info for ${d_CURRENT_TARGET_NAME}: ${ARG_p_ADD_BUILD_INFO}")
        set(_tmp_build_info_file_ ${CMAKE_CURRENT_BINARY_DIR}/__tmp_${d_CURRENT_TARGET_NAME}_build_info.c)
        ameba_build_info_gen(${_tmp_build_info_file_} p_LIB_NAME ${ARG_p_PREFIX}${ARG_p_OUTPUT_NAME})
        target_sources(${d_CURRENT_TARGET_NAME} PRIVATE ${_tmp_build_info_file_})
        set_property(TARGET ${d_CURRENT_TARGET_NAME} PROPERTY ADDITIONAL_CLEAN_FILES ${_tmp_build_info_file_})
        unset(_tmp_build_info_file_)
    endif()

    if (ARG_p_INCLUDES)
        if (${ARG_p_TYPE} STREQUAL "interface")
            target_include_directories(${d_CURRENT_TARGET_NAME} INTERFACE ${ARG_p_INCLUDES})
        else()
            target_include_directories(${d_CURRENT_TARGET_NAME} PRIVATE ${ARG_p_INCLUDES})
        endif()
        ameba_list_append_ifdef(${d_CURRENT_TARGET_NAME}_includes ${d_CURRENT_TARGET_NAME}_includes ${ARG_p_INCLUDES})
    endif()

    if (ARG_p_COMPILE_OPTIONS)
        if (${ARG_p_TYPE} STREQUAL "interface")
            target_compile_options(${d_CURRENT_TARGET_NAME} INTERFACE ${ARG_p_COMPILE_OPTIONS})
        else()
            target_compile_options(${d_CURRENT_TARGET_NAME} PRIVATE ${ARG_p_COMPILE_OPTIONS})
        endif()
        ameba_list_append_ifdef(${d_CURRENT_TARGET_NAME}_compile_options ${d_CURRENT_TARGET_NAME}_compile_options ${ARG_p_COMPILE_OPTIONS})
    endif()

    if (ARG_p_COMPILE_DEFINES)
        if (${ARG_p_TYPE} STREQUAL "interface")
            target_compile_definitions(${d_CURRENT_TARGET_NAME} INTERFACE ${ARG_p_COMPILE_DEFINES})
        else()
            target_compile_definitions(${d_CURRENT_TARGET_NAME} PRIVATE ${ARG_p_COMPILE_DEFINES})
        endif()
        ameba_list_append_ifdef(${d_CURRENT_TARGET_NAME}_compile_defines ${d_CURRENT_TARGET_NAME}_compile_defines ${ARG_p_COMPILE_DEFINES})
    endif()

    if (ARG_p_LINK_OPTIONS)
        if (${ARG_p_TYPE} STREQUAL "interface")
            target_link_options(${d_CURRENT_TARGET_NAME} INTERFACE ${ARG_p_LINK_OPTIONS})
        else()
            target_link_options(${d_CURRENT_TARGET_NAME} PRIVATE ${ARG_p_LINK_OPTIONS})
        endif()
        ameba_list_append_ifdef(${d_CURRENT_TARGET_NAME}_link_options ${d_CURRENT_TARGET_NAME}_link_options ${ARG_p_LINK_OPTIONS})
    endif()

    if (ARG_p_LINK_LIBRARY)
        if (${ARG_p_TYPE} STREQUAL "interface")
            target_link_libraries(${d_CURRENT_TARGET_NAME} INTERFACE ${ARG_p_LINK_LIBRARY})
        else()
            foreach(s IN LISTS srcs_from_libs)
                ameba_source_property_inherite(${s})
            endforeach()
            ameba_target_link_if(non_interface_libs ${d_CURRENT_TARGET_NAME}  ${non_interface_libs} p_SCOPE private)
            ameba_target_include_if(incs_from_libs ${d_CURRENT_TARGET_NAME}  ${incs_from_libs} p_SCOPE private)
            ameba_target_sources_if(srcs_from_libs ${d_CURRENT_TARGET_NAME}  ${srcs_from_libs} p_SCOPE private)
            ameba_target_compile_options_if(ops_from_libs ${d_CURRENT_TARGET_NAME}  ${ops_from_libs} p_SCOPE private)
            ameba_target_definitions_if(def_from_libs ${d_CURRENT_TARGET_NAME}  ${def_from_libs} p_SCOPE private)
        endif()
        ameba_list_append_ifdef(${d_CURRENT_TARGET_NAME}_link_libraries ${d_CURRENT_TARGET_NAME}_link_libraries ${ARG_p_LINK_LIBRARY})
    endif()

    if (ARG_p_DEPENDENCIES)
        add_dependencies(${d_CURRENT_TARGET_NAME} ${ARG_p_DEPENDENCIES})
        ameba_list_append_ifdef(${d_CURRENT_TARGET_NAME}_dependencies ${d_CURRENT_TARGET_NAME}_dependencies ${ARG_p_DEPENDENCIES})
    endif()

    if (ARG_p_OUTPUT_PATH)
        set_target_properties(${d_CURRENT_TARGET_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${ARG_p_OUTPUT_PATH})
        set_target_properties(${d_CURRENT_TARGET_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${ARG_p_OUTPUT_PATH})
        set_target_properties(${d_CURRENT_TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ARG_p_OUTPUT_PATH})
    endif()

    if(ARG_p_REMOVE_COMPILE_OPTIONS)
        ameba_info("remove compile options for ${d_CURRENT_TARGET_NAME}: ${ARG_p_REMOVE_COMPILE_OPTIONS}")
        ameba_target_remove_compile_options(${d_CURRENT_TARGET_NAME} ${ARG_p_REMOVE_COMPILE_OPTIONS})
    endif()

    if (ARG_p_LINKED_BY_LIBRARIES)
        if(NOT DEFINED ARG_p_LINKED_BY_SCOPE)
            set(ARG_p_LINKED_BY_SCOPE private)
        endif()
        if (${ARG_p_LINKED_BY_SCOPE} STREQUAL "interface")
            if (NOT ${ARG_p_TYPE} STREQUAL "interface")
                ameba_warning("non-interface(${ARG_p_TYPE}) target ${d_CURRENT_TARGET_NAME} is linked by INTERFACE")
            endif()
            foreach(t IN LISTS ARG_p_LINKED_BY_LIBRARIES)
                target_link_libraries(${t} INTERFACE ${d_CURRENT_TARGET_NAME})
            endforeach()
        elseif(${ARG_p_LINKED_BY_SCOPE} STREQUAL "private")
            foreach(t IN LISTS ARG_p_LINKED_BY_LIBRARIES)
                target_link_libraries(${t} PRIVATE ${d_CURRENT_TARGET_NAME})
            endforeach()
        elseif(${ARG_p_LINKED_BY_SCOPE} STREQUAL "public")
            foreach(t IN LISTS ARG_p_LINKED_BY_LIBRARIES)
                target_link_libraries(${t} PUBLIC ${d_CURRENT_TARGET_NAME})
            endforeach()
        elseif(${ARG_p_LINKED_BY_SCOPE} STREQUAL "default")
            foreach(t IN LISTS ARG_p_LINKED_BY_LIBRARIES)
                target_link_libraries(${t} ${d_CURRENT_TARGET_NAME})
            endforeach()
        else()
            ameba_fatal("Unknown p_LINKED_BY_SCOPE value: ${ARG_p_LINKED_BY_SCOPE}")
        endif()
    endif()


    if(ARG_p_DEPENDENTED_BY_TARGETS)
        foreach(t IN LISTS ARG_p_DEPENDENTED_BY_TARGETS)
            ameba_target_depend(${t} ${d_CURRENT_TARGET_NAME})
        endforeach()
    endif()

    ameba_set(_tmp_param)
    ameba_list_append_if(ARG_p_STRIP_DEBUG _tmp_param -g)
    ameba_list_append_if(ARG_p_ENABLE_DETERMINISTIC_ARCHIVES _tmp_param -D)
    if (_tmp_param)
        add_custom_command(
            TARGET ${d_CURRENT_TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} ${_tmp_param} $<TARGET_FILE:${d_CURRENT_TARGET_NAME}>
            COMMENT "strip target ${d_CURRENT_TARGET_NAME}"
        )
        unset(_tmp_param)
    endif()

    if (ARG_p_APPEND_TO_LIST)
        foreach(prop IN LISTS ARG_p_APPEND_TO_LIST)
            set_property(GLOBAL APPEND PROPERTY ${prop} ${d_CURRENT_TARGET_NAME})
        endforeach()
    endif()

    unset(_tmp_enable_target_info_list)
endfunction()

function(ameba_target_set target)
    if(NOT TARGET ${target})
        if (d_AMEBA_REALSE)
            return()
        else()
            ameba_warning("call ameba_target_set for Non-target: ${target} ")
        endif()
    endif()

    set(options
        p_ADD_EMPTY_C_FILE              # If set, an empty c file is added to the target sources
    )
    set(oneValueArgs
        p_SCOPE                     # Set target type, one of [public|private|interface], default: private
        p_OUTPUT_PATH               # Set target output path, default: by cmake rule
        p_OUTPUT_NAME               # Set target output name, default: ${name}
        p_PREFIX                    # Set target output prefix, default: lib(for static|shared)
        p_SUFFIX                    # Set target output suffix, default: [.a|.so|.exe]

        p_LINKED_BY_SCOPE              # Set link scope for p_LINKED_BY_LIBRARIES
    )
    set(multiValueArgs
        p_SOURCES                   # Set target source files
        p_INCLUDES                  # Set target include directories
        p_COMPILE_OPTIONS           # Set target compile options
        p_COMPILE_DEFINES           # Set target compile definitions
        p_LINK_OPTIONS              # Set target link options
        p_LINK_LIBRARY              # Set target link libraries
        p_DEPENDENCIES              # Set target dependencies
        p_APPEND_TO_LIST            # Append target to list(properties) used in top level
        p_REMOVE_COMPILE_OPTIONS    # Remove compile options(maybe from global config) for the target.

        p_LINKED_BY_LIBRARIES       # Set target being linked by other libraries
        p_DEPENDENTED_BY_TARGETS    # Set target being linked by other libraries
    )
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(ARG_p_OUTPUT_PATH)
        set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${ARG_p_OUTPUT_PATH})
        set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${ARG_p_OUTPUT_PATH})
        set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ARG_p_OUTPUT_PATH})
    endif()

    if(ARG_p_OUTPUT_NAME)
        set_target_properties(${target} PROPERTIES OUTPUT_NAME ${ARG_p_OUTPUT_NAME})
    endif()

    if(ARG_p_PREFIX)
        set_target_properties(${target} PROPERTIES PREFIX ${ARG_p_PREFIX})
    endif()

    if(ARG_p_SUFFIX)
        set_target_properties(${target} PROPERTIES SUFFIX ${ARG_p_SUFFIX})
    endif()

    if (ARG_p_SOURCES)
        target_sources(${target} PRIVATE ${ARG_p_SOURCES})
    endif()

    if(NOT DEFINED ARG_p_SCOPE)
        set(ARG_p_SCOPE default)
    endif()


    ameba_target_sources_if(ARG_p_SOURCES ${target} p_SCOPE ${ARG_p_SCOPE} ${ARG_p_SOURCES})
    ameba_target_sources_if(ARG_p_ADD_EMPTY_C_FILE ${target} p_SCOPE ${ARG_p_SCOPE} ${c_EMPTY_C_FILE})
    ameba_target_include_if(ARG_p_INCLUDES ${target} p_SCOPE ${ARG_p_SCOPE} ${ARG_p_INCLUDES})
    ameba_target_compile_options_if(ARG_p_COMPILE_OPTIONS ${target} p_SCOPE ${ARG_p_SCOPE} ${ARG_p_COMPILE_OPTIONS})
    ameba_target_definitions_if(ARG_p_COMPILE_DEFINES ${target} p_SCOPE ${ARG_p_SCOPE} ${ARG_p_COMPILE_DEFINES})
    ameba_target_link_options_if(ARG_p_LINK_OPTIONS ${target} p_SCOPE ${ARG_p_SCOPE} ${ARG_p_LINK_OPTIONS})
    ameba_target_link_if(ARG_p_LINK_LIBRARY ${target} p_SCOPE ${ARG_p_SCOPE} ${ARG_p_LINK_LIBRARY})
    ameba_target_depend_if(ARG_p_DEPENDENCIES ${target} ${ARG_p_DEPENDENCIES})
    ameba_target_remove_compile_options_if(ARG_p_REMOVE_COMPILE_OPTIONS ${target} ${ARG_p_REMOVE_COMPILE_OPTIONS})


    if (ARG_p_LINKED_BY_LIBRARIES)
        if(NOT DEFINED ARG_p_LINKED_BY_SCOPE)
            set(ARG_p_LINKED_BY_SCOPE private)
        endif()
        if (${ARG_p_LINKED_BY_SCOPE} STREQUAL "interface")
            foreach(t IN LISTS ARG_p_LINKED_BY_LIBRARIES)
                target_link_libraries(${t} INTERFACE ${target})
            endforeach()
        elseif(${ARG_p_LINKED_BY_SCOPE} STREQUAL "private")
            foreach(t IN LISTS ARG_p_LINKED_BY_LIBRARIES)
                target_link_libraries(${t} PRIVATE ${target})
            endforeach()
        elseif(${ARG_p_LINKED_BY_SCOPE} STREQUAL "public")
            foreach(t IN LISTS ARG_p_LINKED_BY_LIBRARIES)
                target_link_libraries(${t} PUBLIC ${target})
            endforeach()
        elseif(${ARG_p_LINKED_BY_SCOPE} STREQUAL "default")
            foreach(t IN LISTS ARG_p_LINKED_BY_LIBRARIES)
                target_link_libraries(${t} ${target})
            endforeach()
        else()
            ameba_fatal("Unknown p_LINKED_BY_SCOPE value: ${ARG_p_LINKED_BY_SCOPE}")
        endif()
    endif()

    if(ARG_p_DEPENDENTED_BY_TARGETS)
        foreach(t IN LISTS ARG_p_DEPENDENTED_BY_TARGETS)
            ameba_target_depend(${t} ${target})
        endforeach()
    endif()
endfunction()

function(ameba_target_set_if condition target)
    if(NOT TARGET ${target})
        if (d_AMEBA_REALSE)
            return()
        else()
            ameba_warning("call ameba_target_set for Non-target: ${target} ")
        endif()
    endif()

    if(DEFINED ${condition})
        if(${condition})
            ameba_target_set(${target} ${ARGN})
        endif()
    endif()
endfunction()
