# Example Description

This example describes how to use CapTouch to detect key press and release.

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
* On AmebaGreen2
	- CapTouch provides up to `9 channels` for capacitive sensing.
	- Connect `CTC_CHx_PIN` to `to-be-detected signal`, where x can be `0-8` and `CTC_CHx_PIN` is defined under `CapTouch_Chn_Pad_Selection` in ameba_captouch.h.
	- Connect `GND` to `GND` of capacitive circuit.

# SW Configuration

In this example, CapTouch offers a wide range of capacitance detection.

1. Parameters in `CTC_Ch_Config[]`, including `Diff Threshold`, `Mbias Current`, `ETC N_Nosie Threshold`, `ETC P_Noise Threshold` and `Channle Enable`, can be adjusted independently according to different applicaiton environmnet.
2. `CTC_DATA_DBG` can be modified to configure whether to print out CapTouch sample data `periodically`.
3. `TIM_IDX` and `TIM_PERIOD` can be modified to configure basic timer index and timer period in `ms`.
4. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

1. When a key press or key release is detected on a specific channel, console will print out the log.
2. CapTouch sample data will be printed out every `500ms` if current channel is enabled and `CTC_DATA_DBG` is set to `a non-zero value`.
3. If N-Noise is detected and no press or release is detected before `TIM_PERIOD` expires, CapTouch `baseline` will be reset.
# Note

None

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F2
