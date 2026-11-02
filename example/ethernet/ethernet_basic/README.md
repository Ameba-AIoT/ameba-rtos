# Example Description

This example demonstrates the basic usage of the Ethernet driver. It shows how to initialize the Ethernet interface and handle link status changes in the example's own thread: a binary semaphore is assigned to `eth_link_sema` before `eth_init()`, and the driver gives it on every link change.

The link thread reads the link state and invokes the callback function when it changes (UP or DOWN), where IP configuration is performed.

# HW Configuration

- Connect the board to a router or switch that provides DHCP service

# SW configuration

1. Enable Ethernet Feature:

   Use menuconfig and enable `CONFIG ETHERNET`, then select `MAC CLOCK SOURCE`:
   * `Internal clock divided from PLL` - the MAC clock comes from the SoC. Then select `PHY CLOCK SOURCE` according to how the PHY on your board is clocked:
     * `50M from Ameba` - Ameba outputs a 50MHz clock to the PHY
     * `25M from Ameba` - Ameba outputs a 25MHz clock to the PHY
     * `PHY xtal` - PHY is clocked by its own crystal
   * `External 50M from the EXTCLK pin` - the MAC clock comes from outside. The EXTCLK pin is an input then, so Ameba cannot clock the PHY and there is no `PHY CLOCK SOURCE` option: the PHY runs from its own crystal

   See [ETH PHY CLK](https://aiot.realmcu.com/zh/latest/rtos/peripherals/ethernet/index.html#eth-phy-clk) for more details.

2. Select the RMII Pin Group:

   Use menuconfig `CONFIG User Config` -> `Pinmux Group Configuration` -> `Ethernet(RMII) pinmux group` to select the group (`0`-`3`, default `2`) that matches the RMII pins actually wired on your board. The pin mapping of each group is listed in `ETHERNET_PAD[]` in `component/soc/usrcfg/amebagreen2/ameba_intfcfg.c`. Do not edit `ETHERNET_Pin_Grp` in that file directly, it is assigned from the menuconfig value.

3. IP Configuration:

   The example obtains an IP address automatically from the network via DHCP.

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

The callback function is called in the example's `ETH_LINK` thread, so it may block and call `netifapi_*`.

# Supported IC

RTL8721F
