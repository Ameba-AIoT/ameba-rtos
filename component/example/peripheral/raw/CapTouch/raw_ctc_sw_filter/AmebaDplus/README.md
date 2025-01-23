# Example Description

This example describes how to filter CapTouch sample data and detect press and release by digital circuit.

This example does not support the loguart command response.

# HW Configuration

Required Components: capacitive circuit.

* On AmebaDplus
	- CapTouch provides up to `4 channels` for capacitive sensing.
	- Connect `CTC_CHx_PIN` to `to-be-detected signal`, where x can be `0-3` and `CTC_CHx_PIN` is defined under `CapTouch_Chn_Pad_Selection` in ameba_captouch.h.
	- Connect `GND` to `GND` of capacitive circuit.

# SW Configuration

In this example, press and release is detected based on filtered CapTouch sample data by digital circuit, which improves accuracy.

1. Macro `ACFREQ_IS_50` can be defined as 1 when AC frequency is 50Hz or as 0 when AC frequency is 60Hz.
2. Parameters in `ctc_ch_config[]`, including `Diff Threshold`, `Mbias Current`, `ETC N_Nosie Threshold`, `ETC P_Noise Threshold` and `Channle Enable`, can be adjusted independently according to different applicaiton environmnet.
4. How to use:
    * Run `./build.py -a raw_ctc_sw_filter` under path `amebadplus_gcc_project/` to generate images.
    * `Download` images to board by Ameba Image Tool.

# Expected Result

When a key press or key release is detected, console will print out the log.

# Note

None

# Supported IC

* AmebaDplus