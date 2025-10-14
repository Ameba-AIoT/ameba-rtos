set(IMAGESCRIPTDIR  ${CMAKE_CURRENT_LIST_DIR}/../tools/image_scripts)

set(PREPENDTOOL python ${IMAGESCRIPTDIR}/axf2bin.py prepend_header)
set(OTAPREPENDTOOL python ${IMAGESCRIPTDIR}/axf2bin.py ota_prepend_header )
set(PADTOOL python ${IMAGESCRIPTDIR}/axf2bin.py pad)
set(IMAGETOOL python ${IMAGESCRIPTDIR}/axf2bin.py imagetool)
set(COMPRESSTOOL  python ${IMAGESCRIPTDIR}/axf2bin.py compress)
set(BINARY_PADING python ${IMAGESCRIPTDIR}/axf2bin.py binary_pading)


set(CODE_ANALYZE_PYTHON python ${c_BASEDIR}/utils/code_analyze/code_analyze.py)
set(STATIC_ANALYZE_PYTHON python ${IMAGESCRIPTDIR}/internal/static_code_analyze.py)
set(EXTRACT_CODE_SIZE_PYTHON python ${IMAGESCRIPTDIR}/internal/extract_code_size.py)
