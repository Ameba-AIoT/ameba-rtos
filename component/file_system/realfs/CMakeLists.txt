##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#------------------------------------------------------------------#
# Component public part, user config begin(DO NOT remove this line)

ameba_list_append(public_includes
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Compile the read-only debug dump / consistency checker into the core and make
# the API visible to callers (vfs adapter, examples) via a global define.
if(CONFIG_REALFS_DEBUG)
    ameba_list_append(public_definitions
        REALFS_ENABLE_DEBUG
    )
endif()

# Component public part, user config end(DO NOT remove this line)
#------------------------------------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries}) #default: whole-archived

##########################################################################################
## * This part defines private part of the component

set(private_sources)                 #private source files, NOTE: relative path is OK
set(private_includes)                #private include directories, NOTE: relative path is OK
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------------------------------------------#
# Component private part, user config begin(DO NOT remove this line)

# The REALFS core is platform-independent (depends only on the block-device
# abstraction in realfs_bdev.h).  Storage backends are provided elsewhere and
# wired in via struct realfs_bdev.  The port/ and test/ directories are PC-only
# and intentionally not built into firmware.
ameba_list_append(private_sources
    realfs.c
)

# Component private part, user config end(DO NOT remove this line)
#------------------------------------------------------------------#

ameba_add_internal_library(realfs
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
