# Example Description

This example describes how to use CapTouch to detect key press and release.

This example does not support the loguart command response.

# HW Configuration

Required Components: capacitive circuit.

* On AmebaSmart
	- CapTouch provides up to `9 channels` for capacitive sensing.
	- Connect `CTC_CHx_PIN` to `to-be-detected signal`, where x can be `0-8` and `CTC_CHx_PIN` is defined under `CapTouch_Chn_Pad_Selection` in ameba_captouch.h.
	- Connect `GND` to `GND` of capacitive circuit.
* On AmebaLite
	- CapTouch provides up to `9 channels` for capacitive sensing.
	- Connect `CTC_CHx_PIN` to `to-be-detected signal`, where x can be `0-8` and `CTC_CHx_PIN` is defined under `CapTouch_Chn_Pad_Selection` in ameba_captouch.h.
	- Connect `GND` to `GND` of capacitive circuit.
* On AmebaDplus
	- CapTouch provides up to `4 channels` for capacitive sensing.
	- Connect `CTC_CHx_PIN` to `to-be-detected signal`, where x can be `0-3` and `CTC_CHx_PIN` is defined under `CapTouch_Chn_Pad_Selection` in ameba_captouch.h.
	- Connect `GND` to `GND` of capacitive circuit.

# SW Configuration

In this example, CapTouch offers a wide range of capacitance detection.

1. Parameters in `CTC_Ch_Config[]`, including `Diff Threshold`, `Mbias Current`, `ETC N_Nosie Threshold`, `ETC P_Noise Threshold` and `Channle Enable`, can be adjusted independently according to different applicaiton environmnet.
2. `CTC_DATA_DBG` can be modified to configure whether to print out CapTouch sample data `periodically`.
3. `TIM_IDX` and `TIM_PERIOD` can be modified to configure basic timer index which can be `0-7` and timer period in `ms`.
4. How to use:
    * Copy `main.c` and `captouch_ext.h` to path`project\realtek_xx_va0_example\src\src_yy\`, and replace old main.c.
    * Run `make all` and `download` image to board.

# Expected Result

1. When a key press or key release is detected on a specific channel, console will print out the log.
2. CapTouch sample data will be printed out every `500ms` if current channel is enabled and `CTC_DATA_DBG` is set to `a non-zero value`.
3. If N-Noise is detected and no press or release is detected before `TIM_PERIOD` expires, CapTouch `baseline` will be reset.
# Note

None

# Supported IC

* AmebaSmart
* AmebaLite
* AmebaDplus
