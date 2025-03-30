##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above

# Component public part, user config end
#----------------------------------------#

set(AFE_RESOURCE afe_res_2mic50mm)
if(CONFIG_AFE_RES_1MIC)
    set(AFE_RESOURCE afe_res_1mic)
elseif(CONFIG_AFE_RES_2MIC30MM)
    set(AFE_RESOURCE afe_res_2mic30mm)
elseif(CONFIG_AFE_RES_2MIC50MM)
    set(AFE_RESOURCE afe_res_2mic50mm)
elseif(CONFIG_AFE_RES_2MIC70MM)
    set(AFE_RESOURCE afe_res_2mic70mm)
endif()


ameba_list_append_if(CONFIG_AIVOICE_EN public_libraries
    ${c_COMPONENT_DIR}/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libaivoice.a
    ${c_COMPONENT_DIR}/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libafe_kernel.a
    ${c_COMPONENT_DIR}/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/lib${AFE_RESOURCE}.a
    ${c_COMPONENT_DIR}/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libkernel.a
    ${c_COMPONENT_DIR}/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libfst.a
    ${c_COMPONENT_DIR}/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libkws.a
    ${c_COMPONENT_DIR}/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libasr.a
    ${c_COMPONENT_DIR}/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libvad.a
    ${c_COMPONENT_DIR}/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libtomlc99.a
    ${c_COMPONENT_DIR}/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libNE10.a
)

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_defines})
ameba_global_library(${public_libraries}) #default: whole-archived
