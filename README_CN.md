# ameba-rtos

* [English Version](./README.md)

ameba-rtos是瑞昱官方推出的物联网开发框架，目前仅支持Linux操作系统。

# 支持的芯片

|芯片         |          master       |     release/v1.0       |
|:----------- |:---------------------:| :---------------------:|
|AmebaSmart   |![alt text][supported] | ![alt text][supported] |
|AmebaLite    |![alt text][supported] | ![alt text][supported] |
|AmebaDplus   |![alt text][supported] | ![alt text][supported] |

[supported]: https://img.shields.io/badge/-supported-green "supported"

# 相关文档

最新版的文档: https://www.realmcu.com/ ，根据所使用的芯片类型获取相应的应用指南和数据手册。

**注意:** 不同系列的芯片都有其相对应的文档，请根据特定的芯片名称查找文档。

# 使用Gitee加速

对于可以访问[Gitee](https://gitee.com) 的用户，当发现从GitHub下载仓库过慢时，我们建议将仓库导入到Gitee中来提升下载速度。

具体导入步骤请参考[如何导入 GitHub 等第三方 Git 仓库 ](https://gitee.com/help/articles/4261)。

# 快速参考

详细的配置步骤请参考上面链接中的应用指南的**Build Environment**章节，这里仅仅提供项目开发过程中部分通用命令和工程配置的参考：

## 配置编译环境

* 工具链默认安装在路径`/opt/rtk-toolchain`中，如果编译过程中出现"Error: No Toolchain in `/opt/rtk-toolchain/vsdk-10.3.1/linux/newlib`"的错误提示，请参考应用指南中的**Installing Toolchain**章节，获取更多详细的信息。
* 目前仅支持Linux系统，要求Ubuntu 16.04 64位或更高版本。
* 未来将支持Windows系统，使用VSCode插件进行编译。
* 要求Python版本3.7或更高版本。执行命令`python --version`检查当前系统中的Python版本，如果出现"Command `python` not found"的错误提示，请参考应用指南中的**Preparing GCC Environment**章节安装Python3。如果仍出现错误，请执行命令`sudo ln -s /usr/bin/python3 /usr/bin/python`创建`/usr/bin/python3`映射到`/usr/bin/python`的符号链接。

**注意:** 首次编译工程时，工具链默认从GitHub下载并安装。如果发现下载速度过慢或者下载失败，请执行命令`make toolchan URL=github`或者`make toolchain URL=aliyun`先下载好工具链。为提升下载速度，我们建议执行`make toolchain URL=aliyun`从阿里云服务器下载。

## 寻找项目

ameba-rtos所支持的芯片的项目命名规则为amebaxxx_gcc_project。

一旦找到了需要的项目，便可以进入该目录，执行配置和编译的操作。

## 配置项目

执行命令`make menuconfig`可打开一个基于文本的配置菜单，在这里可以对芯片的各个核进行配置。

* General Config: 所有核共享的配置选项。
* Network Config: 各个核之间互斥的配置。
* Core Config: 每个核单独的配置选项。

## 编译项目

* 对于编译默认的固件，直接执行命令`make all`。
* 对于编译带有例程的固件，执行命令`make all EXAMPLE=xxx`，每个例程的详细步骤请参考[example](component/example)。
* 对于编译MP固件，请参考应用指南中的**How to Build MP Image**章节。

编译结束之后，将会生成引导程序固件和应用程序固件。

**注意:** 如果编译失败，请执行命令`make clean`清理工程之后重新编译。

## 固件烧录

当编译结束之后，使用工具[AmebaImageTool](tools/ameba/ImageTool/AmebaImageTool.exe)将固件烧写到芯片中：

对于烧写工具的详细使用请参考应用指南的**Image Tool**章节，以下仅列出通用配置。

* 仅支持Windows系统，包括WinXP，Win7或以上版本，需要安装微软插件.NET Framework 4.0。
* 通过USB线将芯片和电脑连接。
* 根据所使用的芯片类型选择配置文件。
* 选择实际使用的串口和波特率，默认波特率为1500000bps。
* 选择需要烧写的固件，根据芯片的Flash配置，设置烧写的起始地址和结束地址，Flash配置参考`ameba_flashcfg.c/Flash_layout`。
* 点击Download按钮开始烧写，每个固件的下载进度都会显示在进度条上，日志窗口提供当前操作状态信息。

**注意:** 对于空的芯片，引导程序固件和应用程序固件都需要烧写。

## 擦除Flash

固件烧写时仅会擦除当前固件大小的区域，如果想要参考整片Flash或者是指定的区域，可以使用工具[AmebaImageTool](tools/ameba/ImageTool/AmebaImageTool.exe)的**Flash Erase**的功能。

* 根据所使用的芯片类型选择配置文件。
* 选择实际使用的串口和波特率，默认波特率为1500000bps。
* 设置想要擦除区域的起始地址和大小。
* 点击Erase按钮开始擦除，从日志窗口获取当前操作状态信息。

# 反馈

* 如果你在项目开发过程中有任何问题或者建议，请登录[RealMCU](https://www.realmcu.com/en/Account/Login?ReturnUrl=%2FCommunity%2Fcima%2F3187aedd-cb0d-444b-aa0c-284fd82cc501) 给我们反馈。

* 如果你在使用中发现了错误或者需要新的功能，请先[查看 GitHub Issues](https://github.com/Ameba-AIoT/ameba-rtos/issues)，确保该问题没有重复提交。