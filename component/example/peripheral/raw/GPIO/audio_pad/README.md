# Example Description

This example describes how to switch audio pad pin between digital path and analog path by RAW GPIO API.

This example does not support the loguart command response.

# HW Configuration

Logic Analyser or oscilloscope.

# SW configuration

1. How to compile:
   - Copy main.c and gpio_ext.h under folder src to project\realtek_amebaxxx_va0_example\src\src_xxx
2. If `DIGIT_PATH_EN` is defined, `AUDIO_PAD_PIN` works under digital path
3. If `DIGIT_PATH_EN`is NOT defined, `AUDIO_PAD_PIN`works as audio pin
4. By default, `AUDIO_PAD_PIN`works under digital path

# Expect result

1. If `AUDIO_PAD_PIN` works under digital path:
   - `AUDIO_PAD_PIN` is configured as gpio in `digit_app_entry()`.
   - In this example, when LA or oscilloscope is connected to `AUDIO_PAD_PIN` defined in gpio_ext.h, output level varies between high and low every 500ms.
2. If `AUDIO_PAD_PIN`works as audio pin:
   Specific applicaion can be realized in `audio_app_entry().`

# Note

1. Audio share pad: `PA18~PA31`, `PB0~PB6`.
2. If `PA20~PA29` is configured to work under digital path, responding amic channel MUST be MUTED to avoid affecting audio module.
3. `PIN <-----> AUDIO FUNC`
   `PA20		MIC1_P`
   `PA21		MIC1_N`
   `PA22		MIC2_P`
   `PA23		MIC2_N`
   `PA24		MIC3_P`
   `PA25		MIC3_N`
   `PA26		MIC4_P`
   `PA27		MIC4_N`
   `PA28		MIC5_P`
   `PA29		MIC5_N`
4. `PA26` and `PA27` are not applicable in QFN100 board.

# Supported IC

- AmebaSmart
