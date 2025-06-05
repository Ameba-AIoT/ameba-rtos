# AIVoice

This folder contains AIVoice offline flow example.

It shows how to use AIVoice full flow (afe+kws+asr) with pre-recorded audio and will run only once after EVB reset.

**Audio functions such as recording and playback are not integrated.**

The pre-recorded audio used in this example is a 3 channel audio, channel 1 and channel 2 are microphone signals with 2mic50mm array and channel 3 is AEC reference signal. Content in example audio is “你好小强” “打开空调” “你好小强” “播放音乐”.

**Note: AFE res, kws lib, fst lib should match the content of audio, otherwise AIVoice can not detect.**

# SW Configuration
### AmebaSmart PSRAM

1. make menuconfig
```< IMG2(Application) running on PSRAM or FLASH? (FLASH)  ---> ```
```< CONFIG APPLICATION  ---> AI Config  ---> [*] Enable TFLITE MICRO```
```< CONFIG APPLICATION  ---> AI Config  ---> [*] Enable AIVoice -> Select AFE Resource (afe_res_2mic50mm))```

2. cd amebasmart_gcc_project
```./build.py -a full_flow_offline```


### AmebaDplus KM4

> **Note: This chip only supports single microphone input and does not support ASR functionality.**

#### 1. Replace Test Audio
- Use `nhxq_dkkt_gbkt_noecho_1c.wav` as the test audio file.
- Place the audio file in the flash region using `__attribute__((section(".audio.data")))`.
- Modify the linker script `amebadplus_gcc_project/project_km4/asdk/ld/ameba_img2_all.ld` by adding the following to the `.xip_image2.text` section:
    ```ld
    .xip_image2.text :
    {
        __flash_text_start__ = .;
        *(.audio.data*);
        *(.non.dram.text*)
        __flash_text_end__ = .;
    } > KM4_IMG2_XIP
    ```

#### 2. Modify Code to Run afe_kws_vad Flow
- Set the interface and parameters:
    ```c
    #define AIVOICE_ENABLE_AFE_SSL (0)

    const struct rtk_aivoice_iface *aivoice = &aivoice_iface_afe_kws_vad_v1;
    rtk_aivoice_set_single_kws_mode();
    struct afe_config afe_param = AFE_CONFIG_ASR_DEFAULT();
    afe_param.mic_array = AFE_1MIC;
    afe_param.ref_num = 0;
    config.afe = &afe_param;

    int mics_num = 1;
    ```

#### 3. Configure menuconfig
- Enable the following options:
    ```< CONFIG APPLICATION  ---> AI Config  ---> [*] Enable TFLITE MICRO```
    ```< CONFIG APPLICATION  ---> AI Config  ---> [*] Enable AIVoice -> Select AFE Resource (afe_res_1mic)```

#### 4. Build the Project
- Enter the project directory and build:
    ```
    cd amebadplus_gcc_project
    ./build.py -a full_flow_offline
    ```


# Expected Result
* full_flow_offline result
```
[AIVOICE] set multi kws mode
...
[AIVOICE] rtk_aivoice version: v1.4.1#Sb993ddf#N014ae4b#Abf7ba50
[AIVOICE] rtk_aivoice_model afe version: afe_2mic_asr_v1.2_AfePara_2mic50_v1.0_bf_v0.0_20241210
[AIVOICE] rtk_aivoice_model vad version: vad_v7_opt
[AIVOICE] rtk_aivoice_model kws version: kws_xqxq_v4.1_opt
[AIVOICE] rtk_aivoice_model asr version: asr_cn_v8_opt
[AIVOICE] rtk_aivoice_log_format version: v2
[user] afe output 1 channels raw audio, others: {"abnormal_flag":0,"ssl_angle":-10}
[AIVOICE] [KWS] result: {"id":2,"keyword":"ni-hao-xiao-qiang","score":0.775240}
[user] wakeup. {"id":2,"keyword":"ni-hao-xiao-qiang","score":0.775240}
[user] voice angle 90.0
[AIVOICE] [ASR] result: {"type":0,"commands":[{"rec":"打开空调","id":1}]}
[user] asr. {"type":0,"commands":[{"rec":"打开空调","id":1}]}
[user] voice angle 90.0
[AIVOICE] [KWS] result: {"id":2,"keyword":"ni-hao-xiao-qiang","score":0.750704}
[user] wakeup. {"id":2,"keyword":"ni-hao-xiao-qiang","score":0.750704}
[user] voice angle 90.0
[AIVOICE] [ASR] result: {"type":0,"commands":[{"rec":"播放音乐","id":37}]}
[user] asr. {"type":0,"commands":[{"rec":"播放音乐","id":37}]}
[user] voice angle 90.0
```
