set(DIR ${BASEDIR}/component/ui/drivers/ili9341)

set(MODULE_IFLAGS ${DIR}/include)

set(MODULE_C_DEFINES __RTOS__)

set(MODULE_C_OPTIONS -DNDEBUG -UDEBUG -Wno-incompatible-pointer-types)

set(CSRC ${DIR}/src/ili9341.c)

ameba_app_library(ui_drivers)
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_OPTIONS})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
