ameba_add_subdirectories_if(CONFIG_AUDIO_FWK
    audio_hal           # make -C audio_hal all
    audio_config        # make -C audio_config config

    # audio_test        # make -C audio_test all
    # audio_hal/test    # make -C audio_hal/test test
)

if(CONFIG_MEDIA_PLAYER OR CONFIG_MEDIA_LITE_PLAYER)
    ameba_add_subdirectories(
        media_config    # make -C media_config config
        # media_test    # make -C media_test test
    )
endif()