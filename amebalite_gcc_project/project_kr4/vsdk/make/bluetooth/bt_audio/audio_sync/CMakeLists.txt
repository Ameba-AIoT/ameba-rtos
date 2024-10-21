set(BT_DIR ${BASEDIR}/component/bluetooth)
set(BT_AUDIO_DIR ${BT_DIR}/bt_audio)
set(AUDIO_HAL_DIR ${BASEDIR}/component/audio)

set(
    MODULE_IFLAGS
	${BT_AUDIO_DIR}/include
	${BT_AUDIO_DIR}/audio_sync
	${AUDIO_HAL_DIR}/interfaces
	${AUDIO_HAL_DIR}/base/osal/osal_c/interfaces
)

set(CSRC ${BT_AUDIO_DIR}/audio_sync/bt_audio_sync.c)

ameba_app_library(btaudiosync)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
