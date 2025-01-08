set(CFU ${BASEDIR}/component/wpan/platform/rtk_rcp_cfu)

list(
    APPEND MODULE_IFLAGS
    ${CFU}/
)

set(CSRC)

list(
	APPEND CSRC 
    ${CFU}/rtk_cfu.c
    ${CFU}/rtk_cfu_file_array.c
    ${CFU}/rtk_cfu_file_fs.c
    ${CFU}/rtk_cfu_fw_array.c
    ${CFU}/rtk_cfu_fw_update_process.c
    ${CFU}/rtk_cfu_fw_update_sub_process.c
    ${CFU}/rtk_cfu_hdlc.c
    ${CFU}/rtk_cfu_rcp_config.c
    ${CFU}/rtk_cfu_uart.c
)

ameba_internal_library(rtk_rcp_cfu)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})