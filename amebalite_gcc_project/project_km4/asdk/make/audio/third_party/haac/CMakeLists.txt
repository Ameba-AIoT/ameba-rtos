set(DIR ${BASEDIR}/component/audio/third_party/haac)
set(MODULE_IFLAGS ${DIR})

set(MODULE_C_DEFINES __RTOS__)
set(MODULE_C_OPTIONS -Wno-error)
set(
    CSRC
    ${DIR}/trigtabs_fltgen.c
    ${DIR}/trigtabs.c
    ${DIR}/tns.c
    ${DIR}/stproc.c
    ${DIR}/sbrtabs.c
    ${DIR}/sbrside.c
    ${DIR}/sbrqmf.c
    ${DIR}/sbrmath.c
    ${DIR}/sbrimdct.c
    ${DIR}/sbrhuff.c
    ${DIR}/sbrhfgen.c
    ${DIR}/sbrhfadj.c
    ${DIR}/sbrfreq.c
    ${DIR}/sbrfft.c
    ${DIR}/sbr.c
    ${DIR}/pns.c
    ${DIR}/noiseless.c
    ${DIR}/imdct.c
    ${DIR}/hufftabs.c
    ${DIR}/huffman.c
    ${DIR}/filefmt.c
    ${DIR}/fft.c
    ${DIR}/dequant.c
    ${DIR}/decelmnt.c
    ${DIR}/dct4.c
    ${DIR}/buffers.c
    ${DIR}/bitstream.c
    ${DIR}/aactabs.c
    ${DIR}/aacdec.c
)

ameba_app_library(haac)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_OPTIONS})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})