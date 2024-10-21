set(DIR ${TARGETDIR}/fwlib/ram_km4)
set(DIR_COM ${TARGETDIR}/fwlib/ram_common)
set(LOGDIR ${TARGETDIR}/swlib)

set(
    CSRC
    ${DIR}/ameba_trustzone.c
    ${DIR_COM}/ameba_gdma_ram.c
    ${DIR_COM}/ameba_codec.c
    ${DIR_COM}/ameba_sport.c
    ${DIR_COM}/ameba_vad.c
    ${DIR_COM}/ameba_ledc.c
    ${DIR_COM}/ameba_uart.c
    ${DIR_COM}/ameba_spi.c
    ${DIR_COM}/ameba_i2c.c
    ${DIR_COM}/ameba_tim.c
    ${DIR_COM}/ameba_rtc.c
    ${DIR_COM}/ameba_adc.c
    ${DIR_COM}/ameba_comparator.c
    ${DIR_COM}/ameba_thermal.c
    ${DIR_COM}/ameba_otp.c
    ${DIR_COM}/ameba_swr_calib.c
    ${LOGDIR}/log.c
)

target_sources(target_img3 PRIVATE ${CSRC})
