##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#------------------------------------------------------------------#
# Component public part, user config begin(DO NOT remove this line)

#FIXME: These include dirs should be move there own cmakelist
ameba_list_append(public_includes
    ${c_CMPT_NETWORK_DIR}/mqtt/MQTTClient #Repeat by disable wlan
    ${c_CMPT_NETWORK_DIR}/mqtt/MQTTPacket #Repeat by disable wlan
    ${c_MBEDTLS_DIR}/include
    ${c_MBEDTLS_DIR}/hw_alt
    rom/include
    # ${c_BASEDIR}/component/soc/common/include
    # ${c_BASEDIR}/component/soc/common/lzma
)
# Component public part, user config end(DO NOT remove this line)
#------------------------------------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries}) #default: whole-achived

