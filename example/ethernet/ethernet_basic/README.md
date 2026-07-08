# Example Description

This example demonstrates the basic usage of the Ethernet driver. It shows how to initialize the Ethernet interface and register a link status callback function via `eth_register_link_cb()`.

The callback function is invoked when the Ethernet link status changes (UP or DOWN), where IP configuration is performed.

# HW Configuration

- Connect the board to a router or switch that provides DHCP service

# SW configuration

1. Enable Ethernet Feature:

   Use menuconfig and select `CONFIG ETHERNET`->`PHY SOURCE CLK (external 25M)` to enable Ethernet support.

2. IP Configuration:

   The example obtain an IP address automatically from the network via DHCP.

3. Build and Download:
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
