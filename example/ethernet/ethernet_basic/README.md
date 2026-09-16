# Example Description

This example demonstrates the basic usage of the Ethernet driver. It shows how to initialize the Ethernet interface and register a link status callback function via `eth_register_link_cb()`.

The callback function is invoked when the Ethernet link status changes (UP or DOWN), where IP configuration is performed.

# HW Configuration

- Connect the board to a router or switch that provides DHCP service

# SW configuration

1. Enable Ethernet Feature:

   Use menuconfig and enable `CONFIG ETHERNET`, then select `PHY SOURCE CLK` according to how the PHY on your board is clocked:
   * `50M from Ameba` - Ameba outputs a 50MHz clock to the PHY
   * `25M from Ameba` - Ameba outputs a 25MHz clock to the PHY
   * `PHY xtal` - PHY is clocked by its own crystal

   See [ETH PHY CLK](https://aiot.realmcu.com/zh/latest/rtos/peripherals/ethernet/index.html#eth-phy-clk) for more details.

2. Select the RMII Pin Group:

   Set `ETHERNET_Pin_Grp` in `component/soc/usrcfg/amebagreen2/ameba_intfcfg.c` to the group (`0x0`-`0x3`) that matches the RMII pins actually wired on your board. The pin mapping of each group is listed in `ETHERNET_PAD[]` in the same file.

3. IP Configuration:

   The example obtain an IP address automatically from the network via DHCP.

4. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect Result

When the Ethernet cable is plugged in:
```
=== Ethernet Link UP ===
Ethernet DHCP got IP: 192.168.1.100
```

When the Ethernet cable is unplugged:
```
=== Ethernet Link DOWN ===
Ethernet IP released
```

# Note

The callback function is called in `eth_link_monitor_thread`

# Supported IC

RTL8721F
