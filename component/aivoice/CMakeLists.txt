if(NOT CONFIG_AIVOICE_EN)
    return() # DO nothing if CONFIG_AIVOICE_EN is not set
endif()

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
    ${c_CMPT_AIVOICE_DIR}/include
)

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

set(KWS_RESOURCE kws)
if(CONFIG_KWS_RES_CUSTOM)
    set(KWS_RESOURCE kws_custom)
endif()

if(CONFIG_AMEBADPLUS)
    ameba_list_append(public_libraries
        ${c_CMPT_AIVOICE_DIR}/prebuilts/lib/rtl8721dx_km4/libaivoice.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/lib/rtl8721dx_km4/libafe_kernel.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/lib/rtl8721dx_km4/libafe_res_1mic.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/lib/rtl8721dx_km4/libkernel.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/lib/rtl8721dx_km4/libkws.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/lib/rtl8721dx_km4/libvad.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/lib/rtl8721dx_km4/libtomlc99.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/lib/rtl8721dx_km4/libkissfft.a
    )
elseif(CONFIG_AMEBASMART)
    ameba_list_append(public_libraries
        ${c_CMPT_AIVOICE_DIR}/prebuilts/lib/rtl8730e_ca32_freertos/libaivoice.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/lib/rtl8730e_ca32_freertos/libafe_kernel.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/lib/rtl8730e_ca32_freertos/lib${AFE_RESOURCE}.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/lib/rtl8730e_ca32_freertos/libkernel.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/lib/rtl8730e_ca32_freertos/libfst.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/lib/rtl8730e_ca32_freertos/lib${KWS_RESOURCE}.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/lib/rtl8730e_ca32_freertos/libasr.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/lib/rtl8730e_ca32_freertos/libvad.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/lib/rtl8730e_ca32_freertos/libnnns.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/lib/rtl8730e_ca32_freertos/librnnoise_16k.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/lib/rtl8730e_ca32_freertos/libtomlc99.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/lib/rtl8730e_ca32_freertos/libNE10.a
    )
endif()

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_defines})
ameba_global_library(${public_libraries}) #default: whole-archived
