##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above
ameba_list_append(public_includes
    interfaces/include
)

ameba_list_append(public_libraries
    ${c_SDK_LIB_APPLICATION_DIR}/lib_jpeg.a
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
    libjpeg-turbo/jcapimin.c
    libjpeg-turbo/jcapistd.c
    libjpeg-turbo/jaricom.c
    libjpeg-turbo/jcarith.c
    libjpeg-turbo/jccoefct.c
    libjpeg-turbo/jccolor.c
    libjpeg-turbo/jcdctmgr.c
    libjpeg-turbo/jchuff.c
    libjpeg-turbo/jcinit.c
    libjpeg-turbo/jcmainct.c
    libjpeg-turbo/jcmarker.c
    libjpeg-turbo/jcmaster.c
    libjpeg-turbo/jcomapi.c
    libjpeg-turbo/jcparam.c
    libjpeg-turbo/jcphuff.c
    libjpeg-turbo/jcprepct.c
    libjpeg-turbo/jcsample.c
    libjpeg-turbo/jctrans.c
    libjpeg-turbo/jdapimin.c
    libjpeg-turbo/jdapistd.c
    libjpeg-turbo/jdarith.c
    libjpeg-turbo/jdatadst.c
    libjpeg-turbo/jdatasrc.c
    libjpeg-turbo/jdcoefct.c
    libjpeg-turbo/jdcolor.c
    libjpeg-turbo/jddctmgr.c
    libjpeg-turbo/jdhuff.c
    libjpeg-turbo/jdinput.c
    libjpeg-turbo/jdmainct.c
    libjpeg-turbo/jdmarker.c
    libjpeg-turbo/jdmaster.c
    libjpeg-turbo/jdmerge.c
    libjpeg-turbo/jdphuff.c
    libjpeg-turbo/jdpostct.c
    libjpeg-turbo/jdsample.c
    libjpeg-turbo/jdtrans.c
    libjpeg-turbo/jerror.c
    libjpeg-turbo/jfdctflt.c
    libjpeg-turbo/jfdctfst.c
    libjpeg-turbo/jfdctint.c
    libjpeg-turbo/jidctflt.c
    libjpeg-turbo/jidctfst.c
    libjpeg-turbo/jidctint.c
    libjpeg-turbo/jidctred.c
    libjpeg-turbo/jmemmgr.c
    libjpeg-turbo/jmemnobs.c
    libjpeg-turbo/jquant1.c
    libjpeg-turbo/jquant2.c
    libjpeg-turbo/jutils.c
    libjpeg-turbo/simd/jsimd_arm.c
    libjpeg-turbo/simd/jsimd_arm_neon.S
)
ameba_list_append(private_compile_options
    -O3
    -fstrict-aliasing
    -Wno-sign-compare
    -Wno-unused-parameter
    -Wno-error
)

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For closed-source component, only build before release and libs are packaged into lib/application
ameba_add_external_app_library(jpeg
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