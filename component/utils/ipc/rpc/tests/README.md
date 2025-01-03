## HOW TO use RPC test :

####  (1) <Enable RPC test config>

For amebalite:
        MENUCONFIG FOR KM4 CONFIG  --->
        IPC Message Queue Config  --->
        Enable CONFIG IPC Message Queue
        Enable CONFIG RPC

        MENUCONFIG FOR KR4 CONFIG  --->
        IPC Message Queue Config  --->
        Enable CONFIG IPC Message Queue
        Enable CONFIG RPC
For amebasmart:
        MENUCONFIG FOR HP CONFIG  --->
        IPC Message Queue Config  --->
        Enable CONFIG IPC Message Queue
        Enable CONFIG RPC

        MENUCONFIG FOR AP CONFIG  --->
        IPC Message Queue Config  --->
        Enable CONFIG IPC Message Queue
        Enable CONFIG RPC


For amebalite:
project\realtek_amebaLite_va0_example\GCC-RELEASE\auto_build\config\.config_km4
project\realtek_amebaLite_va0_example\GCC-RELEASE\auto_build\config\.config_kr4

For amebasmart:
project\realtek_amebaLite_va0_example\GCC-RELEASE\auto_build\config\.config_ap
project\realtek_amebaLite_va0_example\GCC-RELEASE\auto_build\config\.config_hp

set CONFIG_RPC_TEST=y

```shell
CONFIG_RPC_TEST=y
```
For amebalite:
In project\realtek_amebaLite_va0_example\inc\inc_km4\platform_autoconf.h define CONFIG_RPC_TEST
In project\realtek_amebaLite_va0_example\inc\inc_kr4\platform_autoconf.h define CONFIG_RPC_TEST
For amebasmart:
In project\realtek_amebaLite_va0_example\inc\inc_ap\platform_autoconf.h define CONFIG_RPC_TEST
In project\realtek_amebaLite_va0_example\inc\inc_hp\platform_autoconf.h define CONFIG_RPC_TEST
```c
#define CONFIG_RPC_TEST 1
```

####  (2) <Test case>

run rpc_test:
@rpc_test
rpc_test


