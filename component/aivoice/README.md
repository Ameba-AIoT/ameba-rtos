# AIVoice

## Overview

AIVoice provides offline AI solution to build voice related applications on Realtek SoCs.

### Supported SoCs

| Chip       | OS    |     AIVoice            | aivoice_lib_dir   | aivoice_example_dir     |
| :--------- | ----- | :--------------------: | ----------------- | ----------------------- |
| AmebaSmart | Linux | ![alt text][supported] | apps/aivoice      | apps/aivoice/example    |
| AmebaSmart | RTOS  | ![alt text][supported] | component/aivoice | example/aivoice         |
| AmebaLite  | RTOS  | ![alt text][supported] | lib/aivoice       | example/example_aivoice |

[supported]: https://img.shields.io/badge/-supported-green "supported"

### Modules

- AFE(Audio Front End)

  AFE is audio signal processing module for enhancing speech signals. It can improve robustness of speech recognition system or improve signal quality of communication system. 

  In AIVoice, Realtek provides AFE including submodules: AEC (Acoustic Echo Cancelling), BF(Beamforming), NS(Noise Suppression) and AGC (Automatic gain control).

  Currently SDK provides libraries for four microphone array: 1mic, 2mic_30mm, 2mic_50mm, and 2mic_70mm. If you have other requirements, please contact us.

- KWS(Keyword Spotting)

  KWS is the module to detect specific wakeup words from audio.  It is usually the first step in a voice interaction system. The device will enter the state of waiting voice commands after detecting the keyword.

  In AIVoice, Realtek provides two solutions of KWS:

  | Solution           | Training Data     | Available Keywords                                       | Feature                           |
  | ------------------ | ----------------- | -------------------------------------------------------- | --------------------------------- |
  | Fixed Keyword      | specific keywords | keywords same as training data                           | better performance; smaller model |
  | Customized Keyword | common data       | customized keyword of the same language as training data | more flexible                     |

  Currently SDK provides a fixed keyword model library of Chinese keyword "xiao-qiang-xiao-qiang" or "ni-hao-xiao-qiang". If you have other requirements, please contact us.

- VAD (Voice Activity Detection)

  VAD is the module to detect the presence of human speech in audio. 

  In AIVoice, Realtek provides a neural network based VAD and can be used in speech enhancement, ASR system etc.

- ASR (Automatic Speech Recognition)

  ASR is the module to recognize speech to text.
  
  In AIVoice, Realtek provides ASR supporting recognition of Chinese speech command words offline.

  Currently SDK provides libraries for 40 air-conditioning related command words, including "打开空调" and "关闭空调" etc. If you have other requirements, please contact us.

### Flows

Some algorithm flows have been implemented to facilitate user development.

- full flow: AFE+KWS+ASR
- AFE+KWS

## Configurations

### AIVoice

AIVoice configuration includes asr timeout in full flow.

Please refer to *${aivoice_lib_dir}/include/aivoice_sdk_config.h* for details.

### AFE

AFE configuration includes microphone array, working mode, submodule switches, etc. 

If you need to change mic_array, both config and afe resource lib should change accordingly.

Please refer to *${aivoice_lib_dir}/include/aivoice_afe_config.h* for details.

**Note: Please make sure the mic_array and ref_num match AFE input audio.**

### KWS

KWS configuration includes keywords, thresholds, and sensitivity.

Three levels of sensitivity are provided with predefined thresholds. If you need to use sensitivity, please set thresholds to 0. Otherwise will use thresholds in config file.

Please refer to *${aivoice_lib_dir}/include/aivoice_kws_config.h* for details.

**Note: Available keywords depend on kws model.**

### VAD

VAD configuration includes sensitivity and margin.

Please refer to *${aivoice_lib_dir}/include/aivoice_vad_config.h* for details.

**Note: left_margin only affects offset_ms returned by vad, it won't affect the vad status change time. If you need get the audio during left_margin, please implement a buffer to keep audio.**

### ASR

ASR configuration includes sensitivity.

Please refer to *${aivoice_lib_dir}/include/aivoice_asr_config.h* for details.

**Note: Available command words depends on libfst.a.**

## Interfaces

| Interface                  | flow/module |
| -------------------------- | ----------- |
| aivoice_iface_full_flow_v1 | afe+kws+asr |
| aivoice_iface_afe_kws_v1   | afe+kws     |
| aivoice_iface_afe_v1       | afe         |
| aivoice_iface_vad_v1       | vad         |
| aivoice_iface_kws_v1       | kws         |
| aivoice_iface_asr_v1       | asr         |

all interfaces support below functions:

create()

destroy()

reset()

feed()

Please refer to *${aivoice_lib_dir}/include/aivoice_interface.h* for details.

### Event and Callback Message

| aivoice_out_event_type        | callback message                                             |
| ----------------------------- | ------------------------------------------------------------ |
| AIVOICE_EVOUT_VAD             | struct includes VAD status, offset.                          |
| AIVOICE_EVOUT_WAKEUP          | json string includes id, keyword, and score. <br />Example: {"id":2,"keyword":"ni-hao-xiao-qiang","score":0.9} |
| AIVOICE_EVOUT_ASR_RESULT      | json string includes fst type, commands and id. <br />Example: {"type":0,"commands":[{"rec":"play music","id":14}]} |
| AIVOICE_EVOUT_AFE             | struct includes AFE output data, channel number, etc.        |
| AIVOICE_EVOUT_ASR_REC_TIMEOUT | NULL                                                         |

## Examples

### aivoice offline example

This example shows how to use AIVoice full flow with a pre-recorded 3 channel audio and will run only once after EVB reset. **Audio functions such as recording and playback are not integrated.**

Steps：

1. Select aivoice flow or modules needed.
2. Build configuration.
3. Use **create()** to create and initialize aivoice instance with given configuration.
4. Register callback function.
5. Use **feed()** to input audio data to aivoice.
6. If need reset status, use **reset()**.
7. If aivoice no longer needed, use **destroy()** to destroy the instance.

Please refer to *${aivoice_example_dir}/full_flow_offline* for details.

## Glossary

AEC(Acoustic Echo Cancellation), or echo cancellation, refers to removing the echo signal from the input signal. The echo signal is generated by a sound played through the speaker of the device then captured by the microphone.

AFE(Audio Front End), refers to a combination of modules for preprocessing raw audio signals. It's usually performed to improve the quality of speech signal before the voice interaction, including several speech enhancement algorithms. 

AGC(Automatic Gain Control), an algorithm that dynamically controls the gain of a signal and automatically adjust the amplitude to maintain an optimal signal strength.

ASR(Automatic Speech Recognition), or Speech-to-Text, refers to recognition of spoken language from audio into text. It can be used to build voice-user interface to enable spoken human interaction with AI devices.

BF(BeamForming), refers to a spatial filter designed for a microphone array to enhance the signal from a specific direction and attenuate signals from other directions.

KWS(Keyword Spotting), or wakeup word detection, refers to identifying specific keywords from audio. It is usually the first step in a voice interaction system. The device will enter the state of waiting voice commands after detecting the keyword.

NN(Neural Network), is a machine learning model used for various task in artificial intelligence. Neural networks rely on training data to learn and improve their accuracy.

NS(Noise Suppression), or noise reduction, refers to suppressing ambient noises in the signal to enhance the speech signal, especially stationary noises.

RES(Residual Echo Suppression), refers to suppressing the remained echo signal after AEC processing. It is a postfilter for AEC.

SSL(Sound Source Localization), or direction of arrival(DOA), refers to estimating the spatial location of a sound source using a microphone array.

TTS(Text-To-Speech),or speech synthesis, is a technology that converts text into spoken audio. It can be used in any speech-enabled application that requires converting text to speech imitating human voice.

VAD (Voice Activity Detection), or speech activity detection, is a binary classifier to detect the presence or absence of human speech. It is widely used in speech enhancement, ASR system etc, and can also be used to deactivate some processes during non-speech section of an audio session, saving on computation or bandwidth.
