# Example Description

In this application, Ameba is designed as an USB CDC NCM device (USB Ethernet dongle) which can communicate with a USB CDC NCM host and provide Ethernet-over-USB connectivity with DHCP service.

The topology:
```
[Ameba (NCM Device)] ---USB Cable--- [USB CDC NCM Host]
```

# HW Configuration

None

# SW configuration

1. Menuconfig

   Use menuconfig and choose `CONFIG USB`:
   ```
   [*] Enable USB
           USB Mode (Device)  --->
   [*]     CDC NCM
   ```
   Save and exit.

   **Note**: When `CDC NCM` is enabled, `NCM TX datagram aggregation` can be selected under `USB -> Device -> CDC NCM`. When enabled, multiple Ethernet frames are coalesced into a single NTB before transmission on BULK OUT; when disabled, each frame is sent immediately.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
   ```
   [NCM-I] USBD NCM demo start
   ```

2. Connect the USB port of Ameba board to USB host (Another Ameba board as USB CDC NCM host) with USB cable.

3. If the test runs successfully, the console will show:
   ```
   Device IP: 192.168.45.1
   DHCP Server started
   ```

4. After the host obtains an IP address via DHCP (e.g., 192.168.45.2), type command `AT+PING=192.168.45.2` on the device console to verify connectivity. Make sure the ping succeeds with no packet loss.

5. The example also supports iperf test. Type `AT+IPERF=-s` to act as a TCP iperf server on the device; from the host side run the iperf client. Similarly, `AT+IPERF=-c,<host_ip>,-i,1,-t,10` acts as a TCP client. UDP test is also supported; type `AT+IPERF` for more iperf options.

   Note: For detailed information about iperf and ping, please refer to https://aiot.realmcu.com/zh/latest/rtos/atcmd/at_command_network.html

6. A shell command `usbd_ncm_link <0|1>` is provided to report the upper-layer network link state to the host. Calling it is edge-triggered; repeating the same value is a no-op inside the class driver.

# Note

For other chips, refer to the AN for setup guide.

# Supported IC

RTL8730E
RTL8721Dx
RTL8721F
