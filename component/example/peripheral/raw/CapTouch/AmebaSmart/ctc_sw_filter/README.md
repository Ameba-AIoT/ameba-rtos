# Example Description

This example describes how to filter CapTouch sample data and detect press and release by software.

This example does not support the loguart command response.

# HW Configuration

Required Components: capacitive circuit.

* On AmebaSmart
	- CapTouch provides up to `9 channels` for capacitive sensing.
	- Connect `CTC_CHx_PIN` to `to-be-detected signal`, where x can be `0-8` and `CTC_CHx_PIN` is defined under `CapTouch_Chn_Pad_Selection` in ameba_captouch.h.
	- Connect `GND` to `GND` of capacitive circuit.

# SW Configuration

In this example, press and release is detected by software based on filtered CapTouch sample data, which improves accuracy.

1. Macro `ACFREQ_IS_50` can be defined as 1 when AC frequency is 50Hz or as 0 when AC frequency is 60Hz.
2. Parameters in `ctc_ch_config[]`, including `Diff Threshold`, `Mbias Current`, `ETC N_Nosie Threshold`, `ETC P_Noise Threshold` and `Channle Enable`, can be adjusted independently according to different applicaiton environmnet.
3. Macro `CTC_EN_CH` should be consistent with enabled channel number in `ctc_ch_config[]`.
4. Filtered data will be checked every `BASE_UPD_CYCLE` cycles and each cycle consists of `PBUF_NUM` scan intervals.
5. When diff data or accumulated diff data is over `BASE_UPD_THRE`, baseline will be updated immediately and step value is `BASE_UPD_STEP`.
6. Key press or key release will be decided only when it takes place over `CTC_DBC_CNT` consecutive times.
7. `CTC_DATA_DBG` can be configured to control data debug. When `CTC_DATA_DBG` is set to 1, raw data/median data/iir data/average data/baseline data can all be dumped from assigned address. `CTC_DBG_SZ` can be modified to set debug data size. If `CTC_DBG_SZ` is too big, compiling errors will occur.
8. How to use:
    * Copy `main.c` to path`amebasmart_gcc_project\project_xx\src\`, and replace old main.c.
    * Run `make all` and `download` image to board.

# Expected Result

1. When a key press/key release/n_noise/p_noise event is detected, console will print out the log.
2. More features can be realized in function ctc_press_entry() and funciton ctc_release_entry().
3. If N-Noise is detected and no press, release or pnoise is detected over `NNOISE_RST_LMT` times, CapTouch `baseline` will be reset.

# Note

None

# Supported IC

* AmebaSmart