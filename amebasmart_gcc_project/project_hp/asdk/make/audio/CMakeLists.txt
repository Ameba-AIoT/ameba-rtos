ameba_add_subdirectories_if(CONFIG_AUDIO_FWK
    audio_hal       # make -C audio_hal all
    audio_config    # make -C audio_config config

    # audio_test          # make -C audio_test all
    # audio_hal/test      # make -C audio_hal/test
)
