set(DIR ${BASEDIR}/component/audio/media_test)
set(
    MODULE_IFLAGS
    ${BASEDIR}/component/file_system/fatfs
	${BASEDIR}/component/file_system/fatfs/${FATFS_VER}/include
	${BASEDIR}/component/audio/base/libs/src/include
	${BASEDIR}/component/audio/base/log/interfaces
	${BASEDIR}/component/audio/base/osal/osal_c/interfaces
	${BASEDIR}/component/audio/base/osal/osal_cxx/interfaces
	${BASEDIR}/component/audio/base/cutils/interfaces
	${BASEDIR}/component/audio/interfaces
)

set(MODULE_C_DEFINES __RTOS__)
set(MODULE_C_OPTIONS -Wno-multichar -Wincompatible-pointer-types -Wno-conversion -Wno-pointer-sign)

if(CONFIG_MEDIA_PLAYER)
    list(APPEND MODULE_C_DEFINES MEDIA_PLAYER)
endif()

if(CONFIG_MEDIA_LITE_PLAYER)
    list(APPEND MODULE_C_DEFINES MEDIA_LITE_PLAYER)
endif()

set(CSRC ${DIR}/app_example.c ${DIR}/socket_test.c)

if(CONFIG_MEDIA_PLAYER)
    list(
        APPEND CSRC
        ${DIR}/stardard/player_test.c
        ${DIR}/stardard/player_looper_test.c
        ${DIR}/stardard/mydata_source/mydata_source.c
        ${DIR}/stardard/mydata_source/mydata_source_test.c
        ${DIR}/stardard/meidi/mcrc_player_adapter.c
        ${DIR}/stardard/meidi/mcrc_player_looper_test.c
    )
endif()

if(CONFIG_MEDIA_LITE_PLAYER)
    list(
        APPEND CSRC
        ${DIR}/lite/player_test_lite.c
    )
endif()

ameba_internal_library(media_test)
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_OPTIONS})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
