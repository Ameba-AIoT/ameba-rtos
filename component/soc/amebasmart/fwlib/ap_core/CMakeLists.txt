##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

ameba_list_append(public_includes
    .
)

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries}) #default: whole-archived

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

ameba_list_append(private_sources
    arm_gic.c
    cstub.c
    system_sheipa.c
    amebaap_vector.c
    ameba_app_start.c
    traps.c
    psci.c
    smp.c
    ameba_pmc.c
)

ameba_list_append(private_sources
    startup.S
    vectors_table.S
    smccc_call.S
    cache-v7.S
    # translation_table.S

    ameba_xlat_table.c
    xlat_table/xlat_tables_context.c
    xlat_table/xlat_tables_core.c
    xlat_table/xlat_tables_utils.c
    xlat_table/xlat_tables_arch.c
    xlat_table/enable_mmu.S
    xlat_table/cache_helpers.S
)

#FIXME: Here cannot add xlat_table to private_includes for some header file conflict with toolchain
set_source_files_properties(ameba_xlat_table.c PROPERTIES INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/xlat_table
)

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For closed-source component, only build before release and libs are packaged into lib/soc
ameba_add_internal_library(fwlib_ap_core
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)
##########################################################################################
