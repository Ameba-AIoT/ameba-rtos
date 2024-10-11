# AIVoice

This folder contains AIVoice offline flow example.

It shows how to use AIVoice full flow (afe+kws+asr) with pre-recorded audio and will run only once after EVB reset.

**Audio functions such as recording and playback are not integrated.**

The pre-recorded audio used in this example is a 3 channel audio, channel 1 and channel 2 are microphone signals with 2mic50mm array and channel 3 is AEC reference signal. Content in example audio is “你好小强” “打开空调” “你好小强” “播放音乐”.

**Note: AFE res, kws lib, fst lib should match the content of audio, otherwise AIVoice can not detect.**

# SW Configuration
### AmebaSmart PSRAM

1. Add aivoice library link in amebasmart_gcc_project/project_ap/asdk/Makefile
LINK_THIRD_APP_LIB += $(BASEDIR)/component/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libaivoice.a
LINK_THIRD_APP_LIB += $(BASEDIR)/component/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libafe_kernel.a
LINK_THIRD_APP_LIB += $(BASEDIR)/component/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libafe_res_2mic50mm.a
LINK_THIRD_APP_LIB += $(BASEDIR)/component/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libkernel.a
LINK_THIRD_APP_LIB += $(BASEDIR)/component/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libasr.a
LINK_THIRD_APP_LIB += $(BASEDIR)/component/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libfst.a
LINK_THIRD_APP_LIB += $(BASEDIR)/component/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libkws.a
LINK_THIRD_APP_LIB += $(BASEDIR)/component/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libvad.a
LINK_THIRD_APP_LIB += $(BASEDIR)/component/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libtensorflow-microlite.a
LINK_THIRD_APP_LIB += $(BASEDIR)/component/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libtomlc99.a
LINK_THIRD_APP_LIB += $(BASEDIR)/component/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libNE10.a

2. Modify amebasmart_gcc_project/project_ap/asdk/make/utilities_example/Makefile
add include path as below
    MODULE_IFLAGS += -I$(BASEDIR)/component/aivoice/include

3. make menuconfig
< CONFIG BOOT OPTION  --->
        [*] XIP_FLASH 

4. cd amebasmart_gcc_project , make EXAMPLE=full_flow_offline

# Expected Result
* full_flow_offline
[AIVOICE] rtk_aivoice version: v1.2.0#63ac510#5b3c283
[AIVOICE] rtk_aivoice_model afe version: afe_2mic_asr_v0.0_AfePara_2mic50_v0.0_bf_v0.0_20240716
[AIVOICE] rtk_aivoice_model vad version: vad_v7
[AIVOICE] rtk_aivoice_model kws version: kws_xqxq_v4.1
[AIVOICE] rtk_aivoice_model asr version: asr_cn_v8
[AIVOICE] rtk_aivoice_log_format version: v1
[user] afe output 1 channels raw audio, others: {"abnormal_flag":0}
[AIVOICE] [KWS] result: {"id":2,"keyword":"ni-hao-xiao-qiang","score":0.775240}
[user] wakeup. {"id":2,"keyword":"ni-hao-xiao-qiang","score":0.775240}
[AIVOICE] [ASR] result: {"type":0,"commands":[{"rec":"打开空调","id":1}]}
[user] asr. {"type":0,"commands":[{"rec":"打开空调","id":1}]}
[AIVOICE] [KWS] result: {"id":2,"keyword":"ni-hao-xiao-qiang","score":0.750704}
[user] wakeup. {"id":2,"keyword":"ni-hao-xiao-qiang","score":0.750704}
[AIVOICE] [ASR] result: {"type":0,"commands":[{"rec":"播放音乐","id":37}]}
[user] asr. {"type":0,"commands":[{"rec":"播放音乐","id":37}]}
