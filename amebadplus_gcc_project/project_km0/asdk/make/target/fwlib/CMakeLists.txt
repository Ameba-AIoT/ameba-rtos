set(DIR ${TARGETDIR}/fwlib/ram_km0)
set(CFGDIR ${BASEDIR}/component/soc/usrcfg/${c_SOC_TYPE})
set(DIR_COM ${TARGETDIR}/fwlib/ram_common)
set(LOGDIR ${TARGETDIR}/swlib)

set(
    MODULE_IFLAGS 
    ${FREERTOSDIR}/include/
    ${FREERTOSDIR}/portable/GCC/AmebaDplus_KM0/

    ${DIR}/../include/
    ${LOGDIR}
    ${BASEDIR}/component/soc/common/crashdump/include
)

set(
    CSRC 
    ${DIR}/ameba_app_start.c 
	${DIR}/ameba_pinmap.c 
	${DIR_COM}/ameba_pll.c 
	${DIR_COM}/ameba_flash_ram.c 
	${DIR}/ameba_npcap.c 

    ${CFGDIR}/ameba_pinmapcfg.c 
	${CFGDIR}/ameba_sleepcfg.c 
	${CFGDIR}/ameba_flashcfg.c 

    ${DIR_COM}/ameba_ipc_ram.c 
	${DIR_COM}/ameba_ipc_api.c 
	${DIR_COM}/ameba_tim.c 
	${DIR_COM}/ameba_rtc.c 
	${DIR_COM}/ameba_rtc_io.c 
	${DIR_COM}/ameba_reset.c 
	${DIR_COM}/ameba_mpu_ram.c 
	${DIR_COM}/ameba_i2c.c 
	${DIR_COM}/ameba_spi.c 
	${DIR_COM}/ameba_arch.c 
	${DIR_COM}/ameba_keyscan.c 
	${DIR_COM}/ameba_gdma_ram.c 
	${DIR_COM}/ameba_adc.c 
	${DIR_COM}/ameba_comparator.c 
	${DIR_COM}/ameba_captouch.c 
	${DIR_COM}/ameba_uart.c 
	${DIR_COM}/ameba_psram.c 
	${DIR_COM}/ameba_ppe.c 
	${DIR_COM}/ameba_qspi.c 
	${DIR_COM}/ameba_sdio.c 
	${DIR_COM}/ameba_fault_handle.c 

    ${LOGDIR}/log.c
    ${LOGDIR}/sscanf_minimal.c
    ${LOGDIR}/locks.c
	${BASEDIR}/component/soc/common/rtk_assert/rtk_assert.c
)

ameba_internal_library(fwlib)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
