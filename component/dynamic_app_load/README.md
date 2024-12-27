# Dynamic App Load
Here provide examples to simplify the use of Position Independent Code(PIC).

## About The Application
This is a minimal example of having dynamic apps with syscall functionality. It is divided into 2 parts, the kernel
and the app. The kernel is responsible for loading the app and handling syscall functionality. The syscall functionality 
is implemented using jumptables. The app can then call APIs present in the kernel through this jump table. The kernel also 
can call APIs present in the app through the jump table.

Mainly to learn about the GOT, PIC, memory layout of mcu, and a lot more.

#### Kernel
The syscall functionality is implemented in the **sys_module** while the app loading is implemented in the 
**app_loader**.  

The jumptable struct is ``sys_struct sys``. It should contain all the APIs that the app can invoke in the kernel.

The kernel contains the actual code of the APIs in ``sys_struct sys``, This code does not get compiled into the app again. 
So these APIs can be changed without needing to rebuild the app.

#### App
The app is compiled as a Position independent Code. 
The following flags are used:``-fpic -msingle-pic-base -mpic-register=r9 -mno-pic-data-is-text-relative -mlong-calls``. 
This creates an executable which can be run from any memory location given that the GOT address is in the register **R9**.
This value is given as a parameter while calling the app, so this gets placed in r0. 
The ``set_got_base`` function defined in **apps/common/app_start.c** is used to copy the value from r0 to r9. This is the first 
instruction that gets executed when kernel need to call APIs present in app. Followed by the ``app_main`` function defined in app. 
This sequence is defined using the app linker script **ld/app_base.ld**.  
gcc generates the executable in elf which contains a lot of information and is quite large for a mcu. So this elf file is 
converted into [TINF](elf2tinf), which can be easily loaded onto a mcu.  

The app invokes syscalls using the pointer ``sys_struct sys``. The actual address of the ``sys_struct sys`` is added when the app
is loaded. When the app invokes a syscall as follows ``sys->print_test(led_num, led_status);`` the execution flow jumps into the actual
location of the ``print_test`` function which was defined in the kernel.

The jumptable in app is ``const apps_1_struct_t __attribute__ ((__section__(".apps_struct"))) apps_info``. The ``apps_1_struct_t`` should
contain all the APIs that kernel can invoke in the app. So it should be defined for each different app. For example, The struct ``apps_1_struct_t`` 
is defined for **apps/app_test_1**, and the struct ``apps_2_struct_t`` is defined for **apps/app_test_2**.

The ``apps_common_magic_t`` in ``apps_x_struct_t`` is necessary. It includes the magic, type of app, GOT address of app, and the ``set_got_base`` function.
They are all needed when the app is loaded and the APIs present in app are called.
```
    {
        .magic = {'a', 'p', 'p', 'i', 'n', 'f', 'o', '\0'},
        .type = AMOD_TYPE_MOD_TEST_2,   // user define
        .got_base_addr = (uint32_t)app_sgot,
        .set_got_base = &set_got_base,
    },
``` 

The address of the APIs in ``apps_info`` is updated with actual address when the app is loaded.

#### Ioctrl
The ioctrl module is provided for the kernel to call the APIs implemented in apps. Each API present in apps has corresponding command.
In this application, the correspondence between APIs and commands are shown below:

 App                   | API                        | Command
 ----------------------|----------------------------|---------------------------------
  AMOD_TYPE_MOD_TEST_1 | test_pthread               | MOD_APP_CMD_TEST_PTHREAD
                       | test_socket                | MOD_APP_CMD_TEST_SOCKET
                       | app_main                   | MOD_APP_CMD_APP_MAIN
                       | test_pass_params           | MOD_APP_CMD_PASS_PARAMS
  AMOD_TYPE_MOD_TEST_2 | test_pthread               | MOD_APP_CMD_TEST_PTHREAD

Add App type, API and command as your need.

## Quick Start
### Hardware Preparation
Support for AmebaSmart for now.

### Build Dynamic Apps
This app provide mainly test for file system, pthread and socket. There is some extra code in the example to purposely populate the data 
and bss sections. And also to verify if the GOT is copied properly. ie global variable access.

#### 1 Build app_test_1. A example app called app_test_1 is provided.
This app provide mainly test for file system. 
``cd apps/app_test_1``
``make clean``  
``make all``  
A binary file called **target_apps_1_tinf.bin** will be generated in **image**, and then copy to **tinfbin**.

#### 2 Build app_test_2. A example app called app_test_2 is provided.  
This app provide manily test for pthread and socket.
``cd apps/app_test_2``
``make clean``  
``make all``  
A binary file called **target_apps_2_tinf.bin** will be generated in **image**, and then copy to **tinfbin**.

### Build the kernel
Navigate to the base directory of the SDK.

#### 1 Configure the menuconfig to support dynamic app loading.
``cd amebasmart_gcc_project``
``./menuconfig.py``
Choose ``MENUCONFIG FOR CA32 CONFIG`` -> ``Dynamic App Load Config`` -> [*] Dynamic App Load Enable
Save and Exit.

#### 2 Configure the memory layout
Modify **CA32_DYNAMIC_APP_LOAD_DRAM** in ``amebasmart_gcc_project/amebasmart_layout.ld`` to set memory region for dynamic app loading.
For example:
```
	CA32_BL3_DRAM_NS (rwx) :                ORIGIN = 0x60300000, LENGTH = 0x60600000 - 0x60300000	/* CA32 BL3 DRAM NS: 3MB */
	CA32_DYNAMIC_APP_LOAD_DRAM (rwx) :      ORIGIN = 0x60600020, LENGTH = 0x60700000 - 0x60600020	/* CA32 CA32 DYNAMIC APP LOAD DRAM: 1MB */
```
Depends on the size of your applications to adjust the origin and length.
**Note:** The ``ORIGIN`` should be added 0x20 for the image header, and both the ``ORIGIN - 0x20`` and ``LENGTH + 0x20`` should be 4K aligned.

#### 3 Build the kernel
Build the kernel, this is the code that will actually load the app and run it  
``cd amebasmart_gcc_project``
``./build.py``
This will generate the **dynamic_app.bin** in **tinfbin**, then the **dynamic_app.bin** is attached behind **km0_km4_ca32_app.bin**. Flash the **km0_km4_ca32_app.bin** into your board.

### Run the app
Reset the board. Type command ``pic load`` and observe the result. The log print out should be as below if loading works fun.
```
pic load
[APP_LOADER-I] ------------->start, sizeof(target_apps_1): 4348
[APP_LOADER-I] ------------->start, sizeof(target_apps_2): 1760
[APP_LOADER-I] __dram_dynamic_app_text_size__: 0xFFFE0
[APP_LOADER-I] img_start_addr: 0x60600020
[APP_LOADER-I] img_end_addr: 0x60700000
[APP_LOADER-I] find target app: target_apps_1, addr: 0x60600740, size: 4352
[APP_LOADER-I] App bin: 0x6060076C
[APP_LOADER-I] App text size: 994 32 bit word
[APP_LOADER-I] App rel_data_base: 0x606016F4
[APP_LOADER-I] App data_base: 0x60601708
[APP_LOADER-I] Loading app: target_apps_1 1.0
[APP_LOADER-I] App text size: 994 32 bit word
[APP_LOADER-I] App rel data size: 5 32 bit word
[APP_LOADER-I] App data size: 11 32 bit word
[APP_LOADER-I] App bss size: 8 32 bit word
[APP_LOADER-I] App GOT entries: 66
[APP_LOADER-I] Allocating app memory of 360 bytes
[APP_LOADER-I] app_data_base: 0x603B7F54
[APP_LOADER-I] app_stack_base: 0x603B80BC
[APP_LOADER-I] app_got_base: 0x603B7F80
[APP_LOADER-I] Data at data section (flash): 0xF0000E64
[APP_LOADER-I] Data at data section (RAM): 0xA5A5A5A5
[APP_LOADER-I] App sys_struct: 0x60394728
[APP_LOADER-I] App apps_info->magic: appinfo
[APP_LOADER-I] App api_entries: 5
[APP_LOADER-I] App apps_info->got_base_addr: 0x603B7F80
[APP_LOADER-I] GOT in app stack: 603B7F80
[APP_LOADER-I] got_entries_base: 60601734
[APP_LOADER-I] Data offset: 0x10000000
[APP_LOADER-I] Data at bss section (RAM): 0x6060160C
[APP_LOADER-I] __dram_dynamic_app_text_size__: 0xFFFE0
[APP_LOADER-I] img_start_addr: 0x60600020
[APP_LOADER-I] img_end_addr: 0x60700000
[APP_LOADER-I] find target app: target_apps_2, addr: 0x60600040, size: 1760
[APP_LOADER-I] App bin: 0x6060006C
[APP_LOADER-I] App text size: 389 32 bit word
[APP_LOADER-I] App rel_data_base: 0x60600680
[APP_LOADER-I] App data_base: 0x60600680
[APP_LOADER-I] Loading app: target_apps_2 1.0
[APP_LOADER-I] App text size: 389 32 bit word
[APP_LOADER-I] App rel data size: 0 32 bit word
[APP_LOADER-I] App data size: 7 32 bit word
[APP_LOADER-I] App bss size: 0 32 bit word
[APP_LOADER-I] App GOT entries: 33
[APP_LOADER-I] Allocating app memory of 160 bytes
[APP_LOADER-I] app_data_base: 0x603B8140
[APP_LOADER-I] app_stack_base: 0x603B81E0
[APP_LOADER-I] app_got_base: 0x603B815C
[APP_LOADER-I] Data at data section (flash): 0xA5A5A5A5
[APP_LOADER-I] Data at data section (RAM): 0xA5A5A5A5
[APP_LOADER-I] App sys_struct: 0x60394728
[APP_LOADER-I] App apps_info->magic: appinfo
[APP_LOADER-I] App api_entries: 2
[APP_LOADER-I] App apps_info->got_base_addr: 0x603B815C
[APP_LOADER-I] GOT in app stack: 603B815C
[APP_LOADER-I] got_entries_base: 6060069C
[APP_LOADER-I] Data offset: 0x10000000
[APP_LOADER-I] ------------->end
[APP_LOADER-I] ------------->first call
[APP_IOCTRL-I] module magic: appinfo
[APP_IOCTRL-I] module type: 0
[APP_LOADER-I] ------------->second call
[APP_IOCTRL-I] module magic: appinfo
[APP_IOCTRL-I] module type: 0
[APP_IOCTRL-I] module magic: appinfo
[APP_IOCTRL-I] module type: 0
[APP_MAIN_1-I] test_socket
[APP_MAIN_1-I] test_socket task stack 4096
[APP_IOCTRL-I] module magic: appinfo
[APP_IOCTRL-I] module type: 0
[APP_MAIN_1-I] app_main, 10
[APP_MAIN_1-I] i = 0
[APP_MAIN_1-I] APP_MAIN_1 testTab = AAAA
[APP_MAIN_1-I] i = 1
[APP_MAIN_1-I] APP_MAIN_1 testTab = BBBB
[APP_MAIN_1-I] i = 2
[APP_MAIN_1-I] APP_MAIN_1 testTab = CCCC
[APP_MAIN_1-I] i = 3
[APP_MAIN_1-I] APP_MAIN_1 testTab = DDDD
[APP_MAIN_1-I] i = 4
prefix: vfs
dir: test
path: vfs:test
KV dir already existdir make success!, 1
[APP_IOCTRL-I] module magic: appinfo
[APP_IOCTRL-I] module type: 1
[APP_MAIN_2-I] test_pthread
[APP_MAIN_2-I] test_pthread task stack 4096
[APP_MAIN_2-I] This is a test task
[APP_MAIN_2-I] This is a test task: -155
```