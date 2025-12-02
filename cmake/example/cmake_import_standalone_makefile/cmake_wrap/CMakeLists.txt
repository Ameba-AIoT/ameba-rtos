get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
set(third_party_dir ${PARENT_DIR}/third_party_project)
##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

ameba_list_append(public_includes
    ${third_party_dir}/include
) # for the header files needed by other component
ameba_list_append(public_libraries
    ${third_party_dir}/lib_foo_makefile.a
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
# They are only for ameba_add_internal_library
set(private_sources)                 #private source files, NOTE: relative path is OK
set(private_includes)                #private include directories, NOTE: relative path is OK
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------#
# Component private part, user config begin

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's build system

add_custom_target(foo_makefile ALL                                     # Replace foo with new name as you wish
    COMMAND ${CMAKE_COMMAND} -E echo "Building foo project"   # Some prompt message while building
    COMMAND make CROSS_COMPILE=${CROSS_COMPILE} GLOBAL_MCU_COMPILE_C_OPTIONS="${c_GLOBAL_MCU_COMPILE_C_OPTIONS}" -C ${third_party_dir} clean all  # Set your makefile path and compile toolchain
    COMMENT "Building foo project using make"
	BYPRODUCTS ${public_libraries}
)

ameba_target_depend(${c_CURRENT_IMAGE} foo_makefile)                   # Add dependency, foo MUST be same as in add_custom_target above

##########################################################################################
