# AIVoice

This folder contains AIVoice offline flow example.

It shows how to use AIVoice full flow (afe+kws+asr) with pre-recorded audio and will run only once after EVB reset.

**Audio functions such as recording and playback are not integrated.**

The pre-recorded audio used in this example is a 3 channel audio, channel 1 and channel 2 are microphone signals with 2mic50mm array and channel 3 is AEC reference signal. Content in example audio is “你好小强” “打开空调” “你好小强” “播放音乐”.

**Note: AFE res, kws lib, fst lib should match the content of audio, otherwise AIVoice can not detect.**

# SW Configuration
### AmebaSmart PSRAM

1. make menuconfig  
```< CONFIG BOOT OPTION  ---> [*] XIP_FLASH ```  
```< MENUCONFIG FOR CA32 CONFIG ---> AI Config ---> [*] Enable TFLITE MICRO```  
```< MENUCONFIG FOR CA32 CONFIG ---> AI Config ---> [*] Enable AIVoice  ---> Select AFE Resource (afe_res_2mic50mm)```

2. cd amebasmart_gcc_project  
```./build.py -a full_flow_offline```

# Expected Result
* full_flow_offline result
```
[AIVOICE] rtk_aivoice version: v1.2.0#63ac510#5b3c283
[AIVOICE] rtk_aivoice_model afe version: afe_2mic_asr_v0.0_AfePara_2mic50_v0.0_bf_v0.0_20240716
[AIVOICE] rtk_aivoice_model vad version: vad_v7_opt
[AIVOICE] rtk_aivoice_model kws version: kws_xqxq_v4.1_opt
[AIVOICE] rtk_aivoice_model asr version: asr_cn_v8_opt
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
```
