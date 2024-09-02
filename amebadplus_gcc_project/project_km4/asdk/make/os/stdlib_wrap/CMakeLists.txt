set(DIR ${BASEDIR}/component/soc/amebadplus/swlib)

set(CSRC ${DIR}/gcc_wrap.c)

target_sources(
    target_img2_${PROJECT_NAME} 
    PRIVATE 
    ${CSRC}
)
