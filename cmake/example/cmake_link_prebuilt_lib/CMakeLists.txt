##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

set(lib_dir ${CMAKE_CURRENT_SOURCE_DIR}/lib/${c_SOC_TYPE}_${c_MCU_TYPE})

if(${c_CURRENT_IMAGE_TYPE} STREQUAL "image2")
	ameba_list_append(public_libraries ${lib_dir}/lib_foo_prebuilt.a)
endif()

ameba_list_append(public_includes ${CMAKE_CURRENT_SOURCE_DIR}/foo_build/include)

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
#Following operations are used to demonstrate the generation of lib_foo_prebuilt.a. Delete if your lib files are ready.
if(NOT EXISTS ${public_libraries})
add_custom_target(foo_prebuilt ALL                                     # Replace foo with new name as you wish
    COMMAND ${CMAKE_COMMAND} -E echo "Building foo library"   # Some prompt message while building
	COMMAND make -C ${CMAKE_CURRENT_SOURCE_DIR}/foo_build CROSS_COMPILE=${CROSS_COMPILE} GLOBAL_MCU_COMPILE_C_OPTIONS="${c_GLOBAL_MCU_COMPILE_C_OPTIONS}" clean all  # Set your makefile path
    COMMENT "Building foo library using make"
	BYPRODUCTS ${public_libraries}
	COMMAND ${CMAKE_COMMAND} -E make_directory ${lib_dir}
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/foo_build/lib_foo_prebuilt.a ${lib_dir}
)
endif()

##########################################################################################
