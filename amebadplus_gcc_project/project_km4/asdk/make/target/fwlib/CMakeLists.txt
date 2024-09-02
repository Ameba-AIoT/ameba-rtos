set(DIR  ${TARGETDIR}/fwlib/ram_km4)
set(DIR_COM  ${TARGETDIR}/fwlib/ram_common)
set(CFGDIR  ${TARGETDIR}/usrcfg)
set(LOGDIR  ${TARGETDIR}/swlib)

set(
    MODULE_IFLAGS
    ${FREERTOSDIR}/include/
    ${FREERTOSDIR}/portable/GCC/AmebaDplus_KM4/non_secure
    ${FREERTOSDIR}/portable/GCC/AmebaDplus_KM4/secure

    ${DIR}/../include/
    ${LOGDIR}
    ${BASEDIR}/component/soc/common/crashdump/include
)

set(
    CSRC 
    ${DIR}/ameba_app_start.c
	${DIR}/ameba_data_flash.c
	${DIR}/ameba_data_flashclk.c

    ${DIR_COM}/ameba_i2c.c
	${DIR_COM}/ameba_spi.c
	${DIR_COM}/ameba_rtc.c
	${DIR_COM}/ameba_rtc_io.c
	${DIR_COM}/ameba_uart.c
	${DIR_COM}/ameba_tim.c
	${DIR_COM}/ameba_ups.c
	${DIR_COM}/ameba_adc.c
	${DIR_COM}/ameba_comparator.c
	${DIR_COM}/ameba_ipc_api.c
	${DIR_COM}/ameba_captouch.c
	${DIR_COM}/ameba_gdma_memcpy.c
	${DIR_COM}/ameba_bor.c
	${DIR_COM}/ameba_gdma_ram.c 
	${DIR_COM}/ameba_ipc_ram.c 
	${DIR_COM}/ameba_mpu_ram.c
	${DIR_COM}/ameba_ledc.c 
	${DIR_COM}/ameba_reset.c 
	${DIR_COM}/ameba_codec.c 
	${DIR_COM}/ameba_sport.c
	${DIR_COM}/ameba_audio_clock.c 
	${DIR_COM}/ameba_flash_ram.c 
	${DIR_COM}/ameba_pll.c 
	${DIR_COM}/ameba_clk.c 
	${DIR_COM}/ameba_psram.c 
	${DIR_COM}/ameba_arch.c 
	${DIR_COM}/ameba_keyscan.c 
	${DIR_COM}/ameba_ir.c 
	${DIR_COM}/ameba_ppe.c 
	${DIR_COM}/ameba_qspi.c 
	${DIR_COM}/ameba_sdio.c 
	${DIR_COM}/ameba_fault_handle.c 

    ${LOGDIR}/log.c
    ${LOGDIR}/sscanf_minimal.c

    ${CFGDIR}/ameba_sleepcfg.c 
	${CFGDIR}/ameba_flashcfg.c
)

if(CONFIG_INTERNAL_LIB)
    ameba_internal_library(target_fwlib)
    target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
else()
    set_source_files_properties(${CSRC} DIRECTORY ../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
    target_sources(target_img2_${PROJECT_NAME} PRIVATE ${CSRC})
endif()
