<div align="center">

# ameba-rtos

**瑞昱 Ameba 系列芯片官方物联网开发框架，同时支持 Linux 和 Windows 操作系统。**

[![SDK](https://img.shields.io/badge/SDK-FreeRTOS-blue.svg)](https://aiot.realmcu.com/zh/latest/rtos/index.html)
[![License](https://img.shields.io/badge/License-Apache%202.0-lightgrey.svg)](LICENSE)
[![Issues](https://img.shields.io/github/issues/Ameba-AIoT/ameba-rtos)](https://github.com/Ameba-AIoT/ameba-rtos/issues)

[English](README.md) · [中文版](README_CN.md) · [文档](https://aiot.realmcu.com/zh/latest/rtos/index.html) · [产品介绍](https://aiot.realmcu.com/zh/product/index.html)

</div>

ameba-rtos 是瑞昱官方推出的物联网开发框架，基于 FreeRTOS，同时支持 Linux 和 Windows 操作系统。

## 🔌 支持的芯片

| 芯片      |         master           |     release/v1.2         |     release/v1.1         |     release/v1.0         |
|:--------- |:------------------------:|:------------------------:|:------------------------:|:------------------------:|
| RTL8730E  | ![alt text][supported]   | ![alt text][supported]   | ![alt text][supported]   | ![alt text][supported]   |
| RTL8726E  | ![alt text][supported]   | ![alt text][supported]   | ![alt text][supported]   | ![alt text][supported]   |
| RTL8721Dx | ![alt text][supported]   | ![alt text][supported]   | ![alt text][supported]   | ![alt text][supported]   |
| RTL8710E  | ![alt text][supported]   | ![alt text][supported]   | ![alt text][supported]   | ![alt text][supported]   |
| RTL8721F  | ![alt text][supported]   | ![alt text][supported]   | ![alt text][not-support] | ![alt text][not-support] |
| RTL8720E  | ![alt text][supported]   | ![alt text][supported]   | ![alt text][supported]   | ![alt text][supported]   |
| RTL8713E  | ![alt text][supported]   | ![alt text][supported]   | ![alt text][supported]   | ![alt text][supported]   |
| RTL8720F  | ![alt text][supported]   | ![alt text][not-support] | ![alt text][not-support] | ![alt text][not-support] |

[supported]: https://img.shields.io/badge/-%E6%94%AF%E6%8C%81-green "supported"
[not-support]: https://img.shields.io/badge/-%E4%B8%8D%E6%94%AF%E6%8C%81-red "not support"

## 🏗️ 仓库结构

```text
ameba-rtos/
├── component/                  # SDK 中间件与驱动
│   ├── soc/                    #   各芯片 HAL 及外设驱动
│   ├── wifi/                   #   Wi-Fi 驱动与 API（STA / AP / 监听模式）
│   ├── bluetooth/              #   BLE / 经典蓝牙协议栈及示例
│   ├── lwip/                   #   lwIP TCP/IP 协议栈
│   ├── network/                #   应用层协议（MQTT、HTTP、CoAP 等）
│   ├── ssl/                    #   TLS/SSL（mbedTLS、GmSSL）
│   ├── audio/                  #   音频框架                            [submodule]
│   ├── aivoice/                #   AI 语音处理                         [submodule]
│   ├── file_system/            #   VFS、FatFS、LittleFS、FTL、KV 存储
│   ├── usb/                    #   USB Device 与 Host 驱动
│   ├── ethernet/               #   以太网驱动及示例
│   ├── at_cmd/                 #   AT 指令框架（UART / SPI / SDIO / USB）
│   ├── dynamic_app/            #   动态应用加载框架
│   ├── application/speechmind/ #   SpeechMind 应用                    [submodule]
│   ├── os/                     #   FreeRTOS 内核与 OS 封装层
│   ├── ota/                    #   OTA 空中升级框架
│   ├── ui/                     #   LVGL 图形库                         [submodule]
│   └── tflite_micro/           #   TensorFlow Lite Micro 端侧推理      [submodule]
├── example/                    # 开箱即用的应用示例
│   ├── wifi/                   #   Wi-Fi STA、AP、漫游、CSI 等
│   ├── wificast/               #   WifiCast（控制、OTA、安全等）
│   ├── network_protocol/       #   TCP/UDP、MQTT、HTTP、WebSocket、CoAP 等
│   ├── ssl/                    #   TLS 客户端 / 服务端示例
│   ├── peripheral/             #   GPIO、SPI、I2C、UART、PWM、ADC 等
│   ├── storage/                #   VFS、KV、SD 卡、USB 大容量存储等
│   ├── usb/                    #   USB Device 与 Host 各类示例
│   ├── ethernet/               #   以太网示例
│   ├── atcmd_host/             #   AT 指令 Host 示例（UART/SPI/SDIO/USB）
│   ├── ota/                    #   OTA 升级示例
│   ├── mp_app_integration/     #   MP 应用集成示例
│   ├── cJSON/                  #   cJSON 使用示例
│   └── xml/                    #   XML 使用示例
├── tools/                      # 主机端工具
│   ├── ameba/                  #   ImageTool、烧录工具、Monitor、MCP Server 等
│   └── scripts/                #   构建脚本、镜像处理脚本
├── cmake/                      # CMake 构建系统（工具链、编译标志、Kconfig）
├── ameba.py                    # 统一命令行工具：build / flash / monitor / menuconfig
└── env.sh / env.bat            # 工具链环境初始化（Linux / Windows）
```

## ✨ 主要特性

- **多核支持** — 异构 CPU 组合（KM4 + KR4 / KM4 + CA32），支持核间通信（IPC）
- **Wi-Fi** — STA、AP、Wi-Fi Direct、Enterprise（802.1X）、CSI、快速连接、漫游、NAT Repeater、WifiCast、R-Mesh、网卡模式（WHC：支持 Linux / RTOS / Zephyr / STM32 主机）
- **蓝牙** — BLE 5.0 GAP/GATT 及经典蓝牙，支持 Wi-Fi 共存
- **安全** — mbedTLS 3.x、GmSSL、安全启动、TrustZone
- **文件系统** — FatFS、LittleFS、FTL、VFS 抽象层，支持 SD 卡与 USB MSC
- **USB** — Device（CDC-ACM、HID、MSC、UAC、Vendor）与 Host 类
- **OTA** — 基于 HTTP/HTTPS 的双分区空中升级
- **AT 指令** — 支持 UART / SPI / SDIO / USB 多接口
- **音频** — 音频框架及 Codec 驱动 *(XDK)*
- **AI 语音** — AI 语音处理框架 *(XDK)*
- **TFLite Micro** — TensorFlow Lite Micro 端侧推理 *(XDK)*
- **SpeechMind** — SpeechMind 应用集成 *(XDK)*
- **UI** — LVGL 图形库集成 *(XDK)*
- **动态应用加载** — 支持运行时加载应用模块

## 📚 相关文档

最新版文档请访问：[FreeRTOS SDK 及使用指南](https://aiot.realmcu.com/zh/latest/rtos/index.html)。

更多关于 Ameba 系列芯片的信息，请访问[官方产品页面](https://aiot.realmcu.com/zh/product/index.html)。

## 📥 SDK 下载

我们提供两种下载方式，按需选择：

**基础 SDK** — 不含 submodule 的核心 SDK，适用于 Wi-Fi、蓝牙、网络协议及外设开发：

```bash
git clone https://github.com/Ameba-AIoT/ameba-rtos.git
```

**XDK（扩展版）** — 包含全部 submodule 的完整 SDK，音频、AI 语音、UI、TFLite Micro 开发必需：

```bash
git clone --recurse-submodules https://github.com/Ameba-AIoT/ameba-rtos.git
```

若已克隆基础 SDK，后续需要 submodule 时可执行：

```bash
git submodule update --init --recursive
```

> submodule 组件（`audio`、`aivoice`、`ui`、`tflite_micro`、`application/speechmind`）托管于独立仓库，仅在使用对应功能时才需要初始化。

## 🚀 快速开始

我们支持以下两种方式配置编译环境：

### 使用 VS Code 扩展插件

Ameba 扩展插件是在 Cline 基础上开发的 VS Code 扩展，兼容原生 Cline 的全部功能，并针对 Realtek Ameba 系列芯片特别增强了如下能力：

* 开发环境自动检查与安装
* SDK 配置自动化
* 项目编译一键完成
* 固件烧录便捷工具
* 串口监控集成支持

参考 [VS Code 使用指南](https://aiot.realmcu.com/zh/latest/tools/vscode/index.html) 进行插件安装。

### 手动配置编译环境

如果您希望手动完成环境配置，请参考 [FreeRTOS SDK 使用指南](https://aiot.realmcu.com/zh/latest/rtos/sdk/index.html)，按照以下步骤操作。

**1. 配置 SDK 环境**

```bash
source env.sh   # Linux
env.bat         # Windows
```

> `env.sh` 首次运行时会从网络下载交叉工具链预编译包，后续运行直接复用。

**2. 选择目标芯片并配置工程**

```bash
ameba.py soc <soc_name>   # 例如：RTL8721Dx、RTL8730E
ameba.py menuconfig       # 打开 Kconfig 菜单
```

**3. 编译**

```bash
ameba.py build
```

**4. 固件烧录**

```bash
ameba.py flash -p <PORT> -b <BAUDRATE> -i <BIN_FILE> <START_ADDR> <END_ADDR>
```

**5. 串口监控**

```bash
ameba.py monitor -p <PORT> -b 1500000
```

## 🌐 使用 Gitee 加速

对于可以访问 [Gitee](https://gitee.com) 的用户，当发现从 GitHub 下载仓库过慢时，建议直接使用 Gitee 镜像仓库 [ameba-rtos](https://gitee.com/ameba-aiot/ameba-rtos) 以提升下载速度。

## 💬 反馈

* 如果你在项目开发过程中有任何问题或建议，请登录 [Real-AIOT 论坛](https://forum.real-aiot.com/) 给我们反馈。
* 如果你发现了错误或需要新功能，请先[查看 GitHub Issues](https://github.com/Ameba-AIoT/ameba-rtos/issues)，确保该问题尚未被提交。
