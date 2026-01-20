# ameba-rtos

* [English Version](./README.md)

ameba-rtos是瑞昱官方推出的物联网开发框架，同时支持Linux和Windows操作系统。

# 支持的芯片

| 芯片      |         master       |     release/v1.0          |     release/v1.1         |
|:--------- |:---------------------:| :-----------------------:| :-----------------------:|
| RTL8730E  |![alt text][supported] | ![alt text][supported]   | ![alt text][supported]   |
| RTL8726E  |![alt text][supported] | ![alt text][supported]   | ![alt text][supported]   |
| RTL8721Dx |![alt text][supported] | ![alt text][supported]   | ![alt text][supported]   |
| RTL8710E  |![alt text][supported] | ![alt text][supported]   | ![alt text][supported]   |
| RTL8721F  |![alt text][supported] | ![alt text][not-support] | ![alt text][not-support] |
| RTL8720E  |![alt text][supported] | ![alt text][supported]   | ![alt text][supported]   |
| RTL8713E  |![alt text][supported] | ![alt text][supported]   | ![alt text][supported]   |

[supported]: https://img.shields.io/badge/-%E6%94%AF%E6%8C%81-green "supported"
[not-support]: https://img.shields.io/badge/-%E4%B8%8D%E6%94%AF%E6%8C%81-red "not support"

# 相关文档

最新版的文档: [RealMCU](https://aiot.realmcu.com/cn/latest/rtos/index.html) ，在菜单栏中选择产品来查看具体芯片的文档和手册。

**注意:** 不同系列的芯片都有其相对应的数据手册，请根据特定的芯片名称查找手册。

# 使用Gitee加速

对于可以访问[Gitee](https://gitee.com) 的用户，当发现从GitHub下载仓库过慢时，我们建议直接下载Gitee仓库[ameba-rtos](https://gitee.com/ameba-aiot/ameba-rtos)来提升下载速度。

# 快速参考

我们支持以下两种方式配置编译环境：

## 使用 VS Code 扩展插件

Ameba 扩展插件是在 Cline 基础上开发的 VS Code 扩展， 兼容原生 Cline 的全部功能，并针对 Realtek Ameba 系列芯片，特别增强了如下能力：

* 开发环境自动检查与安装
* SDK 配置自动化
* 项目编译一键完成
* 固件烧录便捷工具
* 串口监控集成支持

参考[VS Code 使用指南](https://aiot.realmcu.com/cn/latest/tools/vscode/index.html)进行插件安装。安装插件后，即可高效完成环境配置、项目编译、固件烧录及串口监控等开发任务。

## 手动配置编译环境

如果您希望手动完成环境配置，请参考[FreeRTOS SDK 使用指南](https://aiot.realmcu.com/cn/latest/rtos/sdk/index.html)。根据指南内容，逐步进行以下操作：

* SDK 环境配置: Linux 平台执行 `source env.sh`，Windows 平台执行 `env.bat`。
* 工程配置：
  `ameba.py soc <soc_name>` 选择目标芯片 `<soc_name>`。
  `ameba.py menuconfig` 配置工程。
* 工程编译： `ameba.py build`
* 固件烧录： `ameba.py flash -p <PORT> -b <BAUDRATE> -i <BIN_FILE> <START_ADDR> <END_ADDR>`
* 串口监控： `ameba.py monitor -p <PORT> -b 1500000`

# 反馈

* 如果你在项目开发过程中有任何问题或者建议，请登录[Real-AIOT](https://forum.real-aiot.com/) 给我们反馈。

* 如果你在使用中发现了错误或者需要新的功能，请先[查看 GitHub Issues](https://github.com/Ameba-AIoT/ameba-rtos/issues)，确保该问题没有重复提交。