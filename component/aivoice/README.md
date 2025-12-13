# Ameba AIVoice

## Overview

AIVoice is an offline AI solution developed by Realtek, including local algorithm modules like Audio Front End (Signal Processing), Keyword Spotting, Voice Activity Detection, Speech Recognition etc. It can be used to build smart voice related applications on Realtek Ameba SoCs.

Note that this repository is not recommended for standalone use. Please use it with SDK ([ameba-rtos](https://github.com/Ameba-AIoT/ameba-rtos) or [ameba-dsp](https://github.com/Ameba-AIoT/ameba-dsp) or [ameba-linux](https://github.com/Ameba-AIoT/ameba-linux-manifest) ) based on the chosen SoC and OS.

Documentation: [AIVoice Documentation](https://aiot.realmcu.com/en/latest/rtos/ai/aivoice/aivoice_overview/index.html)

### Supported SoCs

| Chip              | OS    | Processor |         master         | SDK link                                                     |
| :---------------- | ----- | --------- | :--------------------: | ------------------------------------------------------------ |
| RTL8730E          | Linux | CA32      | ![alt text][supported] | [ameba-linux](https://github.com/Ameba-AIoT/ameba-linux-manifest) |
| RTL8730E          | RTOS  | CA32      | ![alt text][supported] | [ameba-rtos](https://github.com/Ameba-AIoT/ameba-rtos)       |
| RTL8713E/RTL8726E | RTOS  | HiFi5 DSP | ![alt text][supported] | [ameba-dsp](https://github.com/Ameba-AIoT/ameba-dsp)         |
| RTL8721Dx         | RTOS  | KM4       | ![alt text][supported] | [ameba-rtos](https://github.com/Ameba-AIoT/ameba-rtos)       |

[supported]: https://img.shields.io/badge/-supported-green "supported"

## Modules

### AFE (Audio Front End)

AFE is audio signal processing module for enhancing speech signals. It can improve robustness of speech recognition system or improve signal quality of communication system.

In AIVoice, AFE includes **submodules**:

- AEC (Acoustic Echo Cancelling)
- BF(Beamforming)
- NS(Noise Suppression)
- AGC (Automatic gain control)
- SSL (Sound Source Localization)

Currently SDK provides libraries for five **microphone arrays**:

- 1mic
- 2mic_30mm
- 2mic_50mm
- 2mic_70mm
- 3mic_50mm

Other microphone arrays or performance optimizations can be provided through customized services.

Support two **modes**: 

- Speech recognition 
- Voice communication

### KWS (Keyword Spotting)

KWS is the module to detect specific wakeup words from audio. It is usually the first step in a voice interaction system. The device will enter the state of waiting voice commands after detecting the keyword.

AIVoice provides two **solutions**: 

- Fixed keyword 
- User-defined keyword

### VAD (Voice Activity Detection)

VAD is the module to detect the presence of human speech in audio.

In AIVoice, a neural network based VAD is provided and can be used in speech enhancement, ASR system etc.

### ASR (Automatic Speech Recognition)

ASR is the module to recognize speech to text.

In AIVoice, ASR supports recognition of Chinese speech command words offline.

## Flows

Some algorithm flows have been implemented to facilitate user development.

- full flow: AFE+KWS+ASR
- AFE+KWS
- AFE+KWS+VAD

## Examples

### AIVoice Offline: Full flow with pre-recorded audio

This example shows how to use AIVoice full flow with a pre-recorded 3 channel audio and will run only once after EVB reset. **Audio functions such as recording and playback are not integrated.** 

Please refer to *examples/full_flow_offline/README.md* for details.

### SpeechMind Realtime: Microphone audio stream input

[SpeechMind](https://github.com/Ameba-AIoT/speechmind) is an intelligent voice assistant framework that **integrates AIVoice with audio functions such as recording and playback**. Its demo implementation varies by chip:

- RTL8726E and RTL8713E: Refer to *examples/speechmind_demo/README.md* for DSP part, and refer to SpeechMind for MCU part.
- RTL8730E: RTOS is supported. Refer to SpeechMind for details.
- Other chip: Coming soon...
