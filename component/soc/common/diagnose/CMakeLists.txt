##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin
# You may use if-else condition to set or update predefined variable above

ameba_list_append(public_includes .)

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

set(fmt_head ${CMAKE_CURRENT_SOURCE_DIR}/ameba_diagnose_format.h)
set(fmt_full ${CMAKE_CURRENT_SOURCE_DIR}/ameba_diagnose_format_full.json)
set(fmt_json ${CMAKE_CURRENT_BINARY_DIR}/tmp_ameba_diagnose_format.json)
set(fmt_src ${CMAKE_BINARY_DIR}/tmp_ameba_diagnose_hash.c)

if(NOT TARGET diag_fmt)
  add_custom_target(diag_fmt
      COMMENT "generate diagnose hash"
      COMMAND ${CMAKE_COMMAND}
        -Dc_DIAG_FMT_SCRIPT="${c_DIAG_FMT_SCRIPT}"
        -Dc_CMAKE_FILES_DIR=${c_CMAKE_FILES_DIR}
        -Dfmt_head=${fmt_head}
        -Dfmt_json=${fmt_json}
        -Dfmt_src=${fmt_src}
        -Dfmt_full=${fmt_full}
        -DGIT_EXECUTABLE=${GIT_EXECUTABLE}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/generate_hash.cmake
      BYPRODUCTS ${fmt_src}
      DEPENDS ${fmt_head} ${CMAKE_CURRENT_SOURCE_DIR}/generate_hash.cmake ${CMAKE_CURRENT_SOURCE_DIR}/ameba_diagnose_hash.c.in
  )
  set_property(TARGET diag_fmt PROPERTY ADDITIONAL_CLEAN_FILES ${fmt_json} ${fmt_src})
endif()

if(CONFIG_WHC_DEV OR CONFIG_WHC_NONE)
    ameba_list_append(private_sources
        ameba_diagnose_np.c
        ameba_diagnose_transform.c
        ${fmt_src}

        # only for np
        ameba_diagnose_protocol.c
        ameba_diagnose_ring_array.c
        ameba_diagnose_queue.c
        ameba_diagnose_heap.c
    )
elseif(CONFIG_WHC_HOST)
    ameba_list_append(private_sources
        ameba_diagnose_ap.c
        ameba_diagnose_transform.c
        ${fmt_src}
    )
endif()

set(DIAG_DEBUG_TEST 0)
if(DIAG_DEBUG_TEST)
    ameba_list_append(private_sources ameba_diagnose_debug.c)
    ameba_list_append(private_definitions DIAG_DEBUG_TEST)
endif()


# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For closed-source component, only build before release and libs are packaged into lib/soc

ameba_add_internal_library(diagnose
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
    p_DEPENDENCIES
        diag_fmt
    p_DROP_IF_NO_SOURCES
)
##########################################################################################
