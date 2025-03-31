# AIVoice

## 概述

AIVoice提供用于在Realtek SoC上构建语音相关应用的离线AI解决方案。

### 支持平台

| Chip       | OS    |     AIVoice            | aivoice_lib_dir   | aivoice_example_dir     |
| :--------- | ----- | :--------------------: | ----------------- | ----------------------- |
| AmebaSmart | Linux | ![alt text][supported] | apps/aivoice      | apps/aivoice/example    |
| AmebaSmart | RTOS  | ![alt text][supported] | component/aivoice | example/aivoice         |
| AmebaLite  | RTOS  | ![alt text][supported] | lib/aivoice       | example/example_aivoice |

[supported]: https://img.shields.io/badge/-supported-green "supported"

### 模块

- AFE（信号处理）

  AFE 是用于增强语音信号的音频信号处理模块。它可以提高语音识别系统的鲁棒性或改善通信系统的信号质量。

  在AIVoice中，Realtek提供了AFE算法，包括子模块：AEC（声学回声消除）、BF（波束成形）、NS（噪声抑制）和 AGC（自动增益控制）。

  当前SDK中提供了1mic，2mic_30mm，2mic_50mm，2mic_70mm四种阵型的算法库，如果您有其他需求，请联系我们。

- KWS（唤醒词检测）

  KWS是用于检测音频中特定唤醒词的模块。它通常是语音交互的第一步，设备检测到唤醒词后，会进入等待语音指令状态。

  在AIVoice中，Realtek提供了两种KWS算法方案：

  | 方案         | 训练数据   | 可选唤醒词                           | 特点               |
  | ------------ | ---------- | ------------------------------------ | ------------------ |
  | 固定唤醒词   | 特定唤醒词 | 训练数据所用唤醒词                   | 性能更好；模型更小 |
  | 自定义唤醒词 | 通用数据   | 与训练数据相同语种的任意自定义唤醒词 | 更灵活             |

  当前SDK中提供了一个“小强小强”或“你好小强”的固定唤醒词模型，如果您需要自定义唤醒词或定制其他固定唤醒词，请联系我们。

- VAD （语音端点检测）

  VAD是用于检测音频中有无语音信号的模块。

  在AIVoice中，Realtek提供了一个基于神经网络的VAD算法，可以用于语音增强、识别等语音系统中。

- ASR （语音识别）

  ASR是用于把语音信号识别为文本的模块。

  在AIVoice中，Realtek提供了支持中文离线语音命令词检测的ASR算法。

  当前SDK中提供了一套包含“打开空调” “关闭空调”等空调相关的40条命令词的识别算法，如果您需要更换命令词，请联系我们。

### 流程

为了方便用户开发，部分算法流程已在AIVoice中实现：

- full flow（全流程）: AFE+KWS+ASR
- AFE+KWS

## 配置

### AIVoice

AIVoice配置参数包括了全流程中的asr超时时间。

详情请参考 *${aivoice_lib_dir}/include/aivoice_sdk_config.h*.

### AFE

AFE配置包括麦克风阵列，模式，子模块开关等。

如果需要修改麦克风阵列，请确保config文件和afe资源库相应修改。

详情请参考 *${aivoice_lib_dir}/include/aivoice_afe_config.h*.

**注意：请确保 mic_array 、ref_num 两个参数与 AFE 输入音频匹配。**

### KWS

KWS配置包括唤醒词，阈值，灵敏度。

KWS提供了预先调好阈值的三档灵敏度，如果需要使用灵敏度参数，请确保config阈值设为0，否则将使用config中的阈值。

详情请参考 *${aivoice_lib_dir}/include/aivoice_kws_config.h*.

**注意：可配置的唤醒词取决于使用的KWS模型。**

### VAD

VAD配置包括灵敏度和左右边距。

详情请参考 *${aivoice_lib_dir}/include/aivoice_vad_config.h*.

**注意： left_margin参数仅影响vad返回的offset_ms数值, 无法影响vad状态的改变时间. 如果需要保留left_margin区间内的音频，请在外部通过音频缓存实现。**

### ASR

ASR配置包括了灵敏度。

详情请参考 *${aivoice_lib_dir}/include/aivoice_asr_config.h*.

**注意：可识别的命令词取决于libfst.a**

## 接口

| 接口                       | 对应流程/模块 |
| -------------------------- | ------------- |
| aivoice_iface_full_flow_v1 | afe+kws+asr   |
| aivoice_iface_afe_kws_v1   | afe+kws       |
| aivoice_iface_afe_v1       | afe           |
| aivoice_iface_vad_v1       | vad           |
| aivoice_iface_kws_v1       | kws           |
| aivoice_iface_asr_v1       | asr           |

所有接口均支持以下函数:

create()

destroy()

reset()

feed()

详情请参考 *${aivoice_lib_dir}/include/aivoice_interface.h*.

### 事件及回调信息

| 事件                          | 回调信息                                                     |
| ----------------------------- | ------------------------------------------------------------ |
| AIVOICE_EVOUT_VAD             | 包含VAD状态，偏移的结构体                                    |
| AIVOICE_EVOUT_WAKEUP          | 包含id，唤醒词，唤醒得分的json字符串。<br />示例: {"id":2,"keyword":"ni-hao-xiao-qiang","score":0.9} |
| AIVOICE_EVOUT_ASR_RESULT      | 包含fst类型，命令词，id的json字符串。 <br />示例: {"type":0,"commands":[{"rec":"打开空调","id":14}]} |
| AIVOICE_EVOUT_AFE             | 包含AFE输出数据，通道数等的结构体                            |
| AIVOICE_EVOUT_ASR_REC_TIMEOUT | NULL                                                         |

## 例子

### aivoice offline example

该例子通过一条提前录制的三通道音频演示如何使用AIVoice的全流程，在开发板启动后仅运行一次。**未整合录音、播放等音频功能。**

流程：

1. 选择需要的aivoice流程或模块
2. 准备配置参数
3. 使用 **create()** 和指定配置来创建并初始化aivoice实例
4. 注册回调函数
5. 使用 **feed()** 给aivoice输入音频数据
6. 如果需要重置状态, 使用 **reset()**
7. 如果不再需要aivoice，使用 **destroy()** 销毁实例

详情请参考 *${aivoice_example_dir}/full_flow_offline*.

## 词汇表

AEC(Acoustic Echo Cancellation)，声学回声消除，或回声消除，是指消除输入信号中的回声信号。回声信号是指麦克风采集到的设备自身扬声器播放的音频。

AFE(Audio Front End)，音频前端，或信号处理，是指用于原始音频信号预处理的一些模块的组合。通常在进行语音交互前执行以增强信号质量，包含多种语音增强算法。

AGC(Automatic Gain Control)，自动增益控制，用于动态调节信号的增益，自动调整信号幅值，以保持最佳信号强度。

ASR(Automatic Speech Recognition)，语音识别，是指把音频中的语音识别为文本。它可用于构建语音用户界面，实现人类与人工智能设备的语音交互。

BF(BeamForming)，波束形成，是指一种为麦克风阵列设计的空间滤波器，用于增强来自特定方向的信号同时衰减其他方向的信号。

KWS(Keyword Spotting)，关键词检测，或唤醒词检测、语音唤醒，是指从音频中识别特定的唤醒词。通常是语音交互的第一步，设备检测到唤醒词后，会进入等待语音指令状态。

NN(Neural Network)，神经网络，是一种用于人工智能各类任务的机器学习模型。神经网络依靠训练数据学习并提升准确性。

NS(Noise Suppression)，噪声抑制，或降噪，是指抑制信号中的环境噪声以增强语音信号, 尤其是稳态噪声。

RES(Residual Echo Suppression)，残余回声抑制，是指抑制AEC处理后的残余回声信号。是AEC的后置滤波器。

SSL(Sound Source Localization)，声源定位，是指利用麦克风阵列估计声源的空间方位。

TTS(Text-To-Speech)，语音合成，是一种将文本转换为语音的技术。它可用于各种需要将文本转换为人声的语音应用中。

VAD (Voice Activity Detection)，语音活性检测，或语音端点检测，是指从音频中检测有无语音信号。它被广泛用于语音增强、语音识别等系统中，也可用于去除音频会话中的非语音片段以减少计算、带宽等。
