# AIVoice

This folder contains AIVoice offline flow example.

It shows how to use AIVoice full flow (afe+kws+asr) with pre-recorded audio and will run only once after EVB reset.

**Audio functions such as recording and playback are not integrated.**

The pre-recorded audio used in this example is a 3 channel audio, channel 1 and channel 2 are microphone signals with 2mic50mm array and channel 3 is AEC reference signal. Content in example audio is “你好小强” “打开空调” “你好小强” “播放音乐”.

**Note: AFE res, kws lib, fst lib should match the content of audio, otherwise AIVoice can not detect.**

# SW Configuration
### AmebaSmart PSRAM

#### 0. `cd amebasmart_gcc_project`

#### 1. Configure menuconfig
    `./menuconfig.py`
    CONFIG Link Option  --->
        IMG2(Application) running on PSRAM or FLASH? (FLASH)  --->
    CONFIG APPLICATION  --->
        AI Config  --->
            [*] Enable TFLITE MICRO
            [*] Enable AIVoice
                Select AFE Resource (afe_res_2mic50mm)  --->
                Select KWS Resource (kws_res_xqxq)  --->

#### 2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

#### Expected Result
* full_flow_offline result
```
[AIVOICE] set multi kws mode
---------------------SPEECH COMMANDS---------------------
Command ID1, 打开空调
Command ID2, 关闭空调
Command ID3, 制冷模式
Command ID4, 制热模式
Command ID5, 加热模式
Command ID6, 送风模式
Command ID7, 除湿模式
Command ID8, 调到十六度
Command ID9, 调到十七度
Command ID10, 调到十八度
Command ID11, 调到十九度
Command ID12, 调到二十度
Command ID13, 调到二十一度
Command ID14, 调到二十二度
Command ID15, 调到二十三度
Command ID16, 调到二十四度
Command ID17, 调到二十五度
Command ID18, 调到二十六度
Command ID19, 调到二十七度
Command ID20, 调到二十八度
Command ID21, 调到二十九度
Command ID22, 调到三十度
Command ID23, 开高一度
Command ID24, 开低一度
Command ID25, 高速风
Command ID26, 中速风
Command ID27, 低速风
Command ID28, 增大风速
Command ID29, 减小风速
Command ID30, 自动风
Command ID31, 最大风量
Command ID32, 中等风量
Command ID33, 最小风量
Command ID34, 自动风量
Command ID35, 左右摆风
Command ID36, 上下摆风
Command ID37, 播放音乐
Command ID38, 暂停播放
Command ID39, 接听电话
Command ID40, 挂断电话
---------------------------------------------------------

[AIVOICE] rtk_aivoice version: v1.5.0#S0825120#N1ed33d6#A6c25e38
[AIVOICE] rtk_aivoice_model afe version: afe_2mic_asr_v1.3.1_AfePara_2mic50_v2.0_bf_v0.0_20250401
[AIVOICE] rtk_aivoice_model vad version: vad_v7_opt
[AIVOICE] rtk_aivoice_model kws version: kws_xqxq_v4.1_opt
[AIVOICE] rtk_aivoice_model asr version: asr_cn_v8_opt
[AIVOICE] rtk_aivoice_log_format version: v2
[user] afe output 1 channels raw audio, others: {"abnormal_flag":0,"ssl_angle":-10}
[AIVOICE] [KWS] result: {"id":2,"keyword":"ni-hao-xiao-qiang","score":0.7746397852897644}
[user] wakeup. {"id":2,"keyword":"ni-hao-xiao-qiang","score":0.7746397852897644}
[user] voice angle 90.0
[user] vad. status = 1, offset = 385
[user] vad. status = 0, offset = 1865
[AIVOICE] [ASR] result: {"type":0,"commands":[{"rec":"打开空调","id":1}]}
[user] asr. {"type":0,"commands":[{"rec":"打开空调","id":1}]}
[user] voice angle 90.0
[user] vad. status = 1, offset = 525
[AIVOICE] [KWS] result: {"id":2,"keyword":"ni-hao-xiao-qiang","score":0.750707507133484}
[user] wakeup. {"id":2,"keyword":"ni-hao-xiao-qiang","score":0.750707507133484}
[user] voice angle 90.0
[user] vad. status = 1, offset = 445
[user] vad. status = 0, offset = 1765
[AIVOICE] [ASR] result: {"type":0,"commands":[{"rec":"播放音乐","id":37}]}
[user] asr. {"type":0,"commands":[{"rec":"播放音乐","id":37}]}
[user] voice angle 90.0
```

### AmebaDplus KM4

> **Note: This chip only supports single microphone input and does not support ASR functionality.**

#### 1. Replace Test Audio
- Modify CMakeLists.txt to use `nhxq_dkkt_gbkt_noecho_1c.wav` as the test audio file
- Place test audio file in the flash region using `__attribute__((section(".audio.data")))` and modify the linker script `amebadplus_gcc_project/project_km4/asdk/ld/ameba_img2_all.ld` by adding the following to `.xip_image2.text` section:
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
    #define AIVOICE_TARGET_AMEBADPLUS   (1)
    ```

#### 3. Configure menuconfig
- Enable the following options:
    ```< CONFIG APPLICATION  ---> AI Config  ---> [*] Enable TFLITE MICRO```
    ```< CONFIG APPLICATION  ---> AI Config  ---> [*] Enable AIVoice -> Select AFE Resource (afe_res_1mic)```

#### 4. Build and Download
* Refer to the SDK Examples section of the online documentation to generate images.
* `Download` images to board by Ameba Image Tool.


#### Expected Result
* full_flow_offline result
```
[AIVOICE] rtk_aivoice version: v1.5.1#S32da26d#Nf0eb5ca#A6f74d23
[AIVOICE] rtk_aivoice_model afe version: afe_1mic_asr_v1.1_AfePara_1mic_ASR_v0.0_20240626_COM_v0.3_20250528
[AIVOICE] rtk_aivoice_model vad version: vad_v8_opt
[AIVOICE] rtk_aivoice_model kws version: kws_xqxq_v5_opt
[AIVOICE] rtk_aivoice_log_format version: v2
[user] afe output 1 channels raw audio, others: 
[AIVOICE] [KWS] result: {"id":2,"keyword":"ni-hao-xiao-qiang","score":0.63414132595062256}
[user] wakeup. {"id":2,"keyword":"ni-hao-xiao-qiang","score":0.63414132595062256}
[user] vad. status = 1, offset = 1185
[user] vad. status = 0, offset = 2625
[user] vad. status = 1, offset = 3885
[user] vad. status = 0, offset = 5325
```
