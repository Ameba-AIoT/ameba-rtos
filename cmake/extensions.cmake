
#define soc library named lib_${name}_${PROJECT_NAME}.a, and it will be move to lib/soc
macro(ameba_soc_library name)

    ameba_library_common(${name})

    add_custom_command(
        OUTPUT ${TARGET_LIBSOC_FOLDER}/lib_${name}.a
        COMMAND ${CMAKE_OBJCOPY} -g -D lib_${CURRENT_LIB_NAME}.a
        COMMAND ${CMAKE_COMMAND} -E copy lib_${CURRENT_LIB_NAME}.a ${TARGET_LIBSOC_FOLDER}/lib_${name}.a
        ${ARGN}     #append extra command
        DEPENDS ${CURRENT_LIB_NAME}
    )

    add_custom_target(
      ${CURRENT_LIB_NAME}_postbuild
      DEPENDS ${TARGET_LIBSOC_FOLDER}/lib_${name}.a
    )

    get_property(ALL_EXE_TARGETS_PROPERTY GLOBAL PROPERTY ALL_EXE_TARGETS_${PROJECT_NAME})

    foreach(exe_target ${ALL_EXE_TARGETS_PROPERTY})
        add_dependencies(${exe_target} ${CURRENT_LIB_NAME}_postbuild)
    endforeach()

endmacro()

#define application library named lib_${name}_${PROJECT_NAME}.a, and it will be move to lib/application
macro(ameba_app_library name)

    ameba_library_common(${name})

    add_custom_command(
        OUTPUT ${TARGET_LIBAPP_FOLDER}/lib_${name}.a
        COMMAND ${CMAKE_OBJCOPY} -g -D lib_${CURRENT_LIB_NAME}.a
        COMMAND ${CMAKE_COMMAND} -E copy lib_${CURRENT_LIB_NAME}.a ${TARGET_LIBAPP_FOLDER}/lib_${name}.a
        ${ARGN}     #append extra command
        DEPENDS ${CURRENT_LIB_NAME}
    )

    add_custom_target(
      ${CURRENT_LIB_NAME}_postbuild
      DEPENDS ${TARGET_LIBAPP_FOLDER}/lib_${name}.a
    )

    add_dependencies(target_img2_${PROJECT_NAME} ${CURRENT_LIB_NAME}_postbuild)

endmacro()

#define application library named lib_${name}_${PROJECT_NAME}.a, and it will be move to lib/application
#additionally, a git version source file will be added in this lib
macro(ameba_app_library_with_gitver name)

    ameba_library_common(${name})

    ameba_git_version_gen(${name})

    add_custom_command(
        OUTPUT ${TARGET_LIBAPP_FOLDER}/lib_${name}.a
        COMMAND ${CMAKE_OBJCOPY} -g -D lib_${CURRENT_LIB_NAME}.a
        COMMAND ${CMAKE_COMMAND} -E copy lib_${CURRENT_LIB_NAME}.a ${TARGET_LIBAPP_FOLDER}/lib_${name}.a
        ${ARGN}     #append extra command
        DEPENDS ${CURRENT_LIB_NAME}
    )

    add_custom_target(
      ${CURRENT_LIB_NAME}_postbuild
      DEPENDS ${TARGET_LIBAPP_FOLDER}/lib_${name}.a
    )

    add_dependencies(target_img2_${PROJECT_NAME} ${CURRENT_LIB_NAME}_postbuild)


endmacro()

#internal library of img2, do not export
#this macro is for components which have extra compile or include flags than target_img2
macro(ameba_internal_library name)

  ameba_library_common(${name})
# record lib name in GLOBAL PROPERTY IMG2_LIBS_${PROJECT_NAME}, then target_img2 can get them when linking
  set_property(GLOBAL APPEND PROPERTY IMG2_LIBS_${PROJECT_NAME} ${CURRENT_LIB_NAME})
  set_property(GLOBAL APPEND PROPERTY g_${d_MCU_PROJECT_NAME}_IMAGE2_LIBS ${CURRENT_LIB_NAME})

endmacro()

#common configuration for both soc libs and application libs
macro(ameba_library_common name)
# add suffix ${PROJECT_NAME} to avoid target name conflict
    set(CURRENT_LIB_NAME ${name}_${PROJECT_NAME})
    add_library(${CURRENT_LIB_NAME} STATIC "")
    set_target_properties(${CURRENT_LIB_NAME} PROPERTIES PREFIX "lib_"  SUFFIX ".a")

    target_link_libraries(${CURRENT_LIB_NAME} PUBLIC ameba_interface_${PROJECT_NAME})
    add_dependencies(${CURRENT_LIB_NAME} build_info_${PROJECT_NAME})
endmacro()

#internal library of rom ns, do not export
#this macro is for components which have extra compile or include flags than target_rom_ns
macro(ameba_rom_ns_library name)

  set(CURRENT_LIB_NAME ${name}_${PROJECT_NAME})
  add_library(${CURRENT_LIB_NAME} STATIC EXCLUDE_FROM_ALL "")
  set_target_properties(${CURRENT_LIB_NAME} PROPERTIES PREFIX "lib_"  SUFFIX ".a")

  target_link_libraries(${CURRENT_LIB_NAME} PUBLIC ameba_interface_${PROJECT_NAME})
  add_dependencies(${CURRENT_LIB_NAME} build_info_${PROJECT_NAME})
  # record lib name in GLOBAL PROPERTY ROM_NS_LIBS_${PROJECT_NAME}, then target_rom_ns can get them when linking
  set_property(GLOBAL APPEND PROPERTY ROM_NS_LIBS_${PROJECT_NAME} ${CURRENT_LIB_NAME})
endmacro()

#internal library of rom, do not export
#this macro is for components which have extra compile or include flags than target_rom
macro(ameba_rom_library name)

  set(CURRENT_LIB_NAME ${name}_${PROJECT_NAME})
  add_library(${CURRENT_LIB_NAME} STATIC EXCLUDE_FROM_ALL "")
  set_target_properties(${CURRENT_LIB_NAME} PROPERTIES PREFIX "lib_"  SUFFIX ".a")

  target_link_libraries(${CURRENT_LIB_NAME} PUBLIC ameba_interface_${PROJECT_NAME})
  add_dependencies(${CURRENT_LIB_NAME} build_info_${PROJECT_NAME})
  # record lib name in GLOBAL PROPERTY ROM_LIBS_${PROJECT_NAME}, then target_rom can get them when linking
  set_property(GLOBAL APPEND PROPERTY ROM_LIBS_${PROJECT_NAME} ${CURRENT_LIB_NAME})
endmacro()

#genereate git version of lib
macro(ameba_git_version_gen name)

    execute_process(
      COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      OUTPUT_VARIABLE _git_ver
      RESULT_VARIABLE ERROR_CODE
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(ERROR_CODE)
        message(SEND_ERROR "git version generate error! " ${ERROR_CODE})
    endif()

    set(_lib_name lib_${name})
    string(TIMESTAMP _configuration_time "%Y/%m/%d-%H:%M:%S")

    configure_file(${CMAKE_FILES_DIR}/git_version.c.in ${CMAKE_CURRENT_BINARY_DIR}/lib_${name}_git_version.c  @ONLY)

    target_sources(${CURRENT_LIB_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/lib_${name}_git_version.c)

endmacro()

#add executable
macro(ameba_executable name)

  add_executable(${name} ${ARGN})
  # record executable target name in GLOBAL PROPERTY ALL_EXE_TARGETS_${PROJECT_NAME}
  set_property(GLOBAL APPEND PROPERTY ALL_EXE_TARGETS_${PROJECT_NAME} ${name})
  if(TARGET rom)
      add_dependencies(${name} rom)
  endif()

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

macro(RENAME_ROM_OBJS)
    add_custom_command(
        TARGET ${CURRENT_LIB_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} --rename-section .rodata.str1.4=.rom.rodata.str1.4 ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a
        COMMAND ${CMAKE_OBJCOPY} --rename-section .rodata.str1.1=.rom.rodata.str1.1 ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a
        COMMAND ${CMAKE_OBJCOPY} --rename-section .rodata=.rom.rodata ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a
        COMMENT "RENAME_ROM_OBJS"
    )
endmacro()

# use MERGE_ROM_RODATA_COMMON to merge drom common rodata
macro(MERGE_ROM_RODATA_COMMON)
    add_custom_command(
      TARGET ${CURRENT_LIB_NAME} POST_BUILD
      COMMAND ${CMAKE_OBJCOPY} --rename-section .hal.rom.rodata=.com.rom.rodata.hal ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a
      COMMAND ${CMAKE_OBJCOPY} --rename-section .rodata.str1.4=.com.rom.rodata.str1.4 ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a
      COMMAND ${CMAKE_OBJCOPY} --rename-section .rodata.str1.1=.com.rom.rodata.str1.1 ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a
      COMMAND ${CMAKE_OBJCOPY} --rename-section .rodata=.com.rom.rodata ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a
      COMMENT "MERGE_ROM_RODATA_COMMON"
    )
endmacro()

macro(ROM_COMMON_SHARE)
    add_custom_command(
      TARGET ${CURRENT_LIB_NAME} POST_BUILD
      COMMAND ${CMAKE_OBJCOPY} --rename-section .hal.rom.text=.share.rom.text ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a
      COMMAND ${CMAKE_OBJCOPY} --rename-section .hal.rom.rodata=.share.rom.rodata.hal ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a
      COMMAND ${CMAKE_OBJCOPY} --rename-section .rodata.str1.4=.share.rom.rodata.str1.4 ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a
      COMMAND ${CMAKE_OBJCOPY} --rename-section .rodata.str1.1=.share.rom.rodata.str1.1 ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a
      COMMAND ${CMAKE_OBJCOPY} --rename-section .rodata=.share.rom.rodata ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a
      COMMENT "ROM_COMMON_SHARE"
    )
endmacro()

macro(RENAME_SSL2ROM)
    add_custom_command(
      TARGET ${CURRENT_LIB_NAME} POST_BUILD
      COMMAND ${CMAKE_OBJCOPY} --rename-section .text=.ssl.rom.text ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a
      COMMAND ${CMAKE_OBJCOPY} --rename-section .rodata=.ssl.rom.rodata ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a
      COMMAND ${CMAKE_OBJCOPY} --rename-section .rodata.str1.4=.ssl.rom.rodata.str1.4 ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a
      COMMAND ${CMAKE_OBJCOPY} --rename-section .rodata.str1.1=.ssl.rom.rodata.str1.1 ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a
      COMMAND ${CMAKE_OBJCOPY} --rename-section .hal.rom.bss=.hal.rom.bank.bss ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a
      COMMENT "RENAME_SSL2ROM"
    )
endmacro()

