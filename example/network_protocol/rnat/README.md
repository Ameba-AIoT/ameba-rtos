# Example Description

This example demonstrates how to use the R-NAT (Router NAT) system for multi-interface routing management with automatic network segment conflict detection. It supports multiple network interfaces (WiFi STA, WiFi AP, Ethernet, USB-ETH, 4G USB dongle) and provides flexible IP configuration through a clean config structure pattern.

The example shows:
- Multi-interface registration and management using `rnat_netif_create()` and `rnat_netif_start()`
- Automatic IP assignment or static IP configuration via config structure
- Network segment conflict detection and automatic resolution
- WAN interfaces always have higher priority than LAN interfaces
- Event-driven link-up status monitoring via LWIP extension callbacks
- User-defined callbacks triggered on interface link up/down events
- NAT forwarding between WAN and LAN interfaces for bidirectional communication
- 4G USB dongle as WAN with automatic AT command dialing (Quectel / Fibocom / Simcom)

# HW Configuration

## Ethernet
- Connect an Ethernet cable to the board's RJ45 port
- For WAN mode: Connect to upstream router/switch that provides DHCP service
- For LAN mode: Connect PC or other devices to obtain IP from board

## USB-ETH
- Connect to board's USB port
- For WAN mode: USB device connects to upstream network
- For LAN mode: Connected devices obtain IP from board

## USB 4G Dongle
- Connect a supported 4G USB dongle to the board's USB port
- Dongle must expose both a CDC-ACM (AT command) interface and a CDC-ECM (data) interface
- The driver automatically handles AT command dialing; no manual setup required
- **Supported models:**

  | Vendor | Model | Notes |
  |--------|-------|-------|
  | Quectel | EG91 | DHCP from dongle; MAC obtained via AT+qnetifmac |
  | Quectel | EG915 | Static IP from AT+CGCONTRDP; use `RNAT_IP_METHOD_STATIC` |
  | Fibocom | LE271 (CAT1) | DHCP from dongle; ECM mode verified via AT+GTUSBMODE |
  | SIMCom | SIM767X | DHCP from dongle; AT+NETOPEN session management |

# SW configuration

1. Enable R-NAT Feature:

   Use menuconfig and select `CONFIG LWIP`->`Enable R-NAT` to enable R-NAT support.

2. Enable Optional Interfaces:

   - **Ethernet**: Select `CONFIG ETHERNET`->`PHY SOURCE CLK (external 25M)`
   - **USB-ETH**: Enable one of the following USB ECM configurations:
     - USB Device mode: `CONFIG USB` -> select `Device` mode -> `CDC ECM`
     - USB Host mode: `CONFIG USB` -> select `Host` mode -> `CDC ECM` -> `USB Ethernet`
   - **USB 4G Dongle**: `CONFIG USB` -> select `Host` mode -> `CDC ACM` -> `4G Dongle` and `CDC ECM` -> `USB Ethernet`

3. Disable Fast Reconnect:

   In `component/soc/usrcfg/amebaxxx/ameba_wificfg.c` :
   ```c
   wifi_user_config.fast_reconnect_en = 0;
   ```

4. Interface Configuration:

   Each netif's configuration is defined in `example_rnat.c`. Users can modify the global configuration variables to customize interface behavior:

   ```c
   /* STA (WAN) - DHCP Client */
   const rnat_netif_config_t g_rnat_sta_netif_config = {
       .role = RNAT_ROLE_WAN,
       .ip_method = RNAT_IP_METHOD_DHCP_CLIENT,
       .priority = RNAT_ROUTE_PRIO_WAN_STA,  /* 102 */
       .ip_info = NULL,
       .if_desc = "STA",
       .status_callback = user_netif_status_cb,
       .callback_user_data = NULL
   };

   /* AP (LAN) - DHCP Server with custom IP */
   const rnat_netif_config_t g_rnat_ap_netif_config = {
       .role = RNAT_ROLE_LAN,
       .ip_method = RNAT_IP_METHOD_DHCP_SERVER,
       .priority = RNAT_ROUTE_PRIO_LAN,  /* 0 */
       .ip_info = &g_ap_custom_ip,  /* Use custom IP */
       .if_desc = "AP",
       .status_callback = user_netif_status_cb,
       .callback_user_data = NULL
   };
   ```

   **Simplified Parameters:**

   Both STA and AP accept WiFi configuration through function arguments:

   - **STA configuration**: Specifies the upstream router to connect to (SSID, password, channel)

   ```c
   /* WiFi configuration for STA - router to connect */
   static const rnat_wifi_config_t g_sta_wifi_config = {
       .ssid = "AmebaWiFi",
       .password = "12345678",
       .channel = 0
   };

   rnat_netif_start(sta_netif, (void *)&g_sta_wifi_config);
   ```

   - **AP configuration**: Specifies the SoftAP to be created (SSID, password, channel)

   ```c
   /* WiFi configuration for AP - SoftAP to create */
   static const rnat_wifi_config_t g_ap_wifi_config = {
       .ssid = "AmebaRouter",
       .password = "12345678",
       .channel = 0
   };

   rnat_netif_start(ap_netif, (void *)&g_ap_wifi_config);
   ```

   **Flexible Role Configuration:**

   ETH and USB-ETH interfaces can be independently configured as WAN or LAN by setting the `role` and `ip_method` fields:

   ```c
   /* Example 1: ETH as WAN port (DHCP Client) */
   const rnat_netif_config_t eth_wan_config = {
       .role = RNAT_ROLE_WAN,
       .ip_method = RNAT_IP_METHOD_DHCP_CLIENT,
       .priority = RNAT_ROUTE_PRIO_WAN_ETH,
       .ip_info = NULL,
       .if_desc = "ETH"
   };

   /* Example 2: ETH as LAN port (DHCP Server) */
   const rnat_netif_config_t eth_lan_config = {
       .role = RNAT_ROLE_LAN,
       .ip_method = RNAT_IP_METHOD_DHCP_SERVER,
       .priority = RNAT_ROUTE_PRIO_LAN,
       .ip_info = NULL,
       .if_desc = "ETH"
   };
   ```

   **4G Dongle (USB WAN):**

   The 4G dongle shares the same netif index (`NETIF_USB_ETH_INDEX`) and the same `rnat_netif_config_t` as the regular USB-ETH ECM adapter. The difference is purely in the Kconfig selection and the `ip_method` field — no change is needed to `rnat_netif_create()` or `rnat_netif_start()`. The driver selected by Kconfig (`CONFIG_USBH_CDC_ACM_4G_DONGLE`) automatically handles AT command dialing before handing the interface to rnat.

   IP method depends on the dongle model:

   - **DHCP-based dongles** (EG91, LE271, SIM767X): use `RNAT_IP_METHOD_DHCP_CLIENT`
   - **Static-IP dongles** (EG915): use `RNAT_IP_METHOD_STATIC` with an `ip_info` placeholder; the driver overwrites the IP via AT command response

   ```c
   /* 4G Dongle (WAN) - DHCP from dongle's built-in DHCP server */
   const rnat_netif_config_t g_rnat_usb_netif_config = {
       .role     = RNAT_ROLE_WAN,
       .ip_method = RNAT_IP_METHOD_DHCP_CLIENT,   /* EG91 / LE271 / SIM767X */
       .priority  = RNAT_ROUTE_PRIO_WAN_USB,      /* 101 */
       .ip_info   = NULL,
       .if_desc   = "USB",
       .status_callback    = user_netif_status_cb,
       .callback_user_data = NULL
   };

   /* 4G Dongle (WAN) - EG915 static IP obtained from AT+CGCONTRDP */
   static const rnat_ip_info_t g_usb_placeholder_ip = {
       .ip      = { .addr = 0 },   /* overwritten by driver */
       .gw      = { .addr = 0 },
       .netmask = { .addr = 0 },
   };
   const rnat_netif_config_t g_rnat_usb_netif_config = {
       .role      = RNAT_ROLE_WAN,
       .ip_method = RNAT_IP_METHOD_STATIC,         /* EG915 */
       .priority  = RNAT_ROUTE_PRIO_WAN_USB,       /* 101 */
       .ip_info   = &g_usb_placeholder_ip,
       .if_desc   = "USB",
       .status_callback    = user_netif_status_cb,
       .callback_user_data = NULL
   };
   ```

   Start the interface (no extra argument needed; the driver manages dialing):
   ```c
   rnat_netif_t *usb_netif = rnat_netif_create(NETIF_USB_ETH_INDEX, &g_rnat_usb_netif_config);
   rnat_netif_start(usb_netif, NULL);
   ```

   Available configurations:
   - **STA (WiFi Client)**: Typically WAN port, priority 102
   - **AP (WiFi Access Point)**: Typically LAN port, priority 0
   - **Ethernet**: Can be WAN (priority 103) or LAN (priority 0)
   - **USB-ETH / 4G Dongle**: Can be WAN (priority 101) or LAN (priority 0); 4G dongle is always WAN

5. User Callback Registration:

   Each netif supports a user-defined status callback that is triggered when the interface link state changes (link up/down):

   ```c
   void user_netif_status_cb(rnat_netif_t *netif, bool is_active, void *user_data);
   ```

   Register the callback in the config structure:
   ```c
   const rnat_netif_config_t g_rnat_sta_netif_config = {
       // ... other fields ...
       .status_callback = user_netif_status_cb,
       .callback_user_data = NULL
   };
   ```

   You can also dynamically set the callback after creation:
   ```c
   rnat_netif_set_status_callback(netif, user_netif_status_cb, user_data);
   ```

6. IP Configuration:

   There are two ways to configure IP addresses by setting the `ip_info` field in the netif config structure:

   **Option A: User-Specified Static IP**

   Define a custom IP configuration and assign it to `ip_info`:
   ```c
   static const rnat_ip_info_t g_ap_custom_ip = {
       .ip = { .addr = PP_HTONL(LWIP_MAKEU32(192, 168, 39, 1)) },
       .gw = { .addr = PP_HTONL(LWIP_MAKEU32(192, 168, 39, 1)) },
       .netmask = { .addr = PP_HTONL(LWIP_MAKEU32(255, 255, 255, 0)) }
   };

   const rnat_netif_config_t g_rnat_ap_netif_config = {
       .role = RNAT_ROLE_LAN,
       .ip_method = RNAT_IP_METHOD_DHCP_SERVER,
       .priority = RNAT_ROUTE_PRIO_LAN,
       .ip_info = &g_ap_custom_ip,  /* Points to custom IP */
       .if_desc = "AP"
       // ... other fields ...
   };
   ```

   **Option B: Auto-Alloc IP**

   Set `ip_info` to NULL, and the system will automatically alloc a non-conflicting IP address or obtain IP from upstream router:
   ```c
   const rnat_netif_config_t g_rnat_sta_netif_config = {
      .role = RNAT_ROLE_WAN,
      .ip_method = RNAT_IP_METHOD_DHCP_CLIENT,
      .priority = RNAT_ROUTE_PRIO_WAN_STA,
      .ip_info = NULL,  /* Auto-obtain IP via DHCP */
      .if_desc = "STA"
       // ... other fields ...
   };

   const rnat_netif_config_t g_rnat_eth_netif_config = {
       .role = RNAT_ROLE_LAN,
       .ip_method = RNAT_IP_METHOD_DHCP_SERVER,
       .priority = RNAT_ROUTE_PRIO_LAN,
       .ip_info = NULL,  /* NULL = auto-alloc */
       .if_desc = "ETH"
       // ... other fields ...
   };
   ```

   In this example:
   - STA uses DHCP client mode (auto-obtain from router)
   - AP uses user-specified IP (192.168.39.1, but will be auto-adjusted if conflict detected)
   - ETH uses auto-alloced IP (192.168.44.1)

7. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect Result

**Note**: The following results demonstrate a configuration with STA as WAN, and both AP and ETH as LAN interfaces.

## System Initialization

When the system starts, all interfaces are initialized:

```
[R-NAT-APP-A] ========================================
[R-NAT-APP-A]             R-NAT Example
[R-NAT-APP-A] ========================================
```

At this point:
- R-NAT system is initialized
- DNS Relay service is running for LAN clients

## WiFi STA Connection

STA interface is created and connects to the upstream router:

```
[R-NAT-I] [STA] Created (role=WAN, ip_method=0, prio=102)
[R-NAT-I] [STA] Starting netif...
[R-NAT-I] [STA] Connecting to 'AmebaWiFi'...
```
```
[$]wifi connected
[R-NAT-I] [STA] LINK UP - ACTIVE
[R-NAT-APP-A] [USER-CB] STA Link UP
[R-NAT-I] [STA] WAN got new IP, reinitializing NAT
Static portmap table cleared!
NAT reinitialization complete
[R-NAT-I] Default GW changed to: 192.168.39.9 (STA, prio=102)
[$]wifi got ip:"192.168.39.9"
[R-NAT-I] [STA] DHCP got IP: 192.168.39.9
```

Key behaviors:
- STA interface is created with WAN role (priority 102)
- STA starts connecting to the configured router using provided SSID/password
- User callback is invoked: `[USER-CB] STA Link UP`
- DHCP client obtains IP address (192.168.39.9) from upstream router
- STA becomes the default gateway (highest priority 102)

## AP Startup & Conflict Detection

After STA successfully obtains IP, the system starts AP interface:

```
[R-NAT-I] [AP] Custom IP: 192.168.39.1
[R-NAT-W] [AP] Custom IP subnet conflicts, auto-allocating...
Netif 1 alloc IP: 192.168.43.1
[R-NAT-I] [AP] Created (role=LAN, ip_method=1, prio=0)
[R-NAT-I] [AP] Starting netif...
[WLAN-A] Init WIFI AP
[R-NAT-I] [AP] LINK UP - ACTIVE
[R-NAT-APP-A] [USER-CB] AP Link UP
[R-NAT-I] [AP] 'AmebaRouter' started on channel 100
```

**Conflict Detection Process:**
1. Before starting AP, system checks if the configured custom IP (192.168.39.1) conflicts with STA's network (192.168.39.x)
2. Conflict detected: AP's custom IP is in the same subnet as STA's WAN connection
3. System automatically allocates a non-conflicting IP: 192.168.43.1
4. AP interface is created with LAN role (priority 0)
5. AP starts successfully with the auto-adjusted IP address
6. User callback is triggered when AP link goes up: `[USER-CB] AP Link UP`

## Ethernet Initialization

ETH interface is created and initialized, waiting for cable connection:

```
Netif 2 alloc IP: 192.168.44.1
[R-NAT-I] [ETH] Created (role=LAN, ip_method=1, prio=0)
[R-NAT-I] [ETH] Starting netif...
[ETH-I] Initializing Ethernet Driver...
[R-NAT-I] [ETH] Initialized
```

ETH is ready but inactive until cable is plugged in.

## Status Summary After Initialization

Print current status of all interfaces:

```
[R-NAT-A] ========== R-NAT Status ==========
[R-NAT-A] [STA] UP: IP=192.168.39.9 GW=192.168.39.1 Role=WAN Method=DHCP-Client Prio=102 [DEFAULT GW]
[R-NAT-A] [AP] UP: IP=192.168.43.1 GW=192.168.43.1 Role=LAN Method=DHCP-Server Prio=0
[R-NAT-A] [ETH] DOWN: IP=192.168.44.1 GW=192.168.44.1 Role=LAN Method=DHCP-Server Prio=0
[R-NAT-A] ====================================
```

Network topology at this stage:
- **WAN**: STA connected to upstream router (192.168.39.x network, priority 102) ← Default Gateway
- **LAN 1**: AP active and serving WiFi clients (192.168.43.x network, priority 0)
- **LAN 2**: ETH initialized but inactive (192.168.44.x network, priority 0)

## WiFi Client Connection

A WiFi device connects to the AP:

```
[WLAN-A] +OnAuth: c4:e5:b1:13:7a:ce
[WLAN-A] +OnAssocReq
[$]client connected:"c4:e5:b1:13:7a:ce"
[$]assign client ip:"192.168.43.100",hwaddr:"c4:e5:b1:13:7a:ce"
```

The WiFi client receives IP 192.168.43.100 from AP's DHCP server.

## Ethernet Connection

When Ethernet cable is plugged in:

```
[ETH-I] Link Up
[ETH-I] MAC Link Info: 100 Mb/s, Full Duplex
[R-NAT-I] [ETH] LINK UP - ACTIVE
[R-NAT-APP-A] [USER-CB] ETH Link UP
[R-NAT-I] === ETH Link UP ===
[R-NAT-I] [ETH] DHCP Server
[R-NAT-I] [ETH] DHCP Server started: 192.168.44.1
[$]assign client ip:"192.168.44.100",hwaddr:"00:e0:4c:b7:23:66"
```

Key behaviors:
- User callback is invoked: `[USER-CB] ETH Link UP`
- ETH DHCP server starts automatically with pre-allocated IP (192.168.44.1)
- Wired client connects and receives IP (192.168.44.100) from DHCP server

## Final Multi-Interface Status

After all connections are established:

```
========== R-NAT Status ==========
[STA] UP: IP=192.168.39.9 GW=192.168.39.1 Role=WAN Method=DHCP-Client Prio=102 [DEFAULT GW]
[AP] UP: IP=192.168.43.1 GW=192.168.43.1 Role=LAN Method=DHCP-Server Prio=0
[ETH] UP: IP=192.168.44.1 GW=192.168.44.1 Role=LAN Method=DHCP-Server Prio=0
====================================
```

Complete network topology:
- **WAN**: STA connected to upstream router (192.168.39.x network, priority 102) ← Default Gateway
- **LAN 1**: AP serving WiFi clients (192.168.43.x network, priority 0)
- **LAN 2**: ETH serving wired clients (192.168.44.x network, priority 0)

## NAT Forwarding

With the above configuration, NAT (Network Address Port Translation) forwarding is now enabled between WAN and LAN interfaces:

- **LAN → Internet**: Devices connected to AP (192.168.43.x) or ETH (192.168.44.x) can access the internet through STA's connection to the upstream router
- **Automatic NAT**: The system automatically translates private LAN IP addresses to the WAN interface's public IP
- **Bidirectional Communication**: Return traffic from the internet is correctly routed back to the originating LAN device

Example scenario:
```
WiFi Client (192.168.43.100) → AP (192.168.43.1) → NAT → STA (192.168.39.9) → Internet
Ethernet Client (192.168.44.100) → ETH (192.168.44.1) → NAT → STA (192.168.39.9) → Internet
```

This enables NAT forwarding between WAN and LAN interfaces, allowing LAN devices to access the internet through the WAN connection.

## 4G Dongle Connection

When a supported 4G USB dongle is plugged in, the driver automatically runs the AT command dialing sequence and brings up the ECM data interface:

```
[USBH-A] Device connect
...
[USB-4G-I] VID 0x2c7c PID 0x0191
[USB-4G-I] LE271 state(0)
...
[USB-4G-I] LE271 dial ok
[USB-4G-I] TRX start
[USB-4G-I] MAC:ea:24:64:58:ca:83 Vid 0x2c7c
[R-NAT-I] === USB Link UP ===
[R-NAT-I] [USB] LINK UP - ACTIVE
[R-NAT-APP-A] [USER-CB] USB Link UP
[R-NAT-I] [USB] DHCP Client
[R-NAT-I] Default GW changed to: 10.72.154.211 (USB, prio=101)
[R-NAT-I] [USB] WAN GW switched, reinitializing NAT
[DNS_PROXY-A] Upstream DNS server[0]: 112.4.1.36
[DNS_PROXY-A] Upstream DNS server[1]: 112.4.12.200
[DNS_PROXY-A] Upstream servers changed, clearing cache and transactions
[DNS_PROXY-A] Upstream DNS servers updated: 2 servers
[$]wifi got ip:"10.72.154.211"
[R-NAT-I] [USB] DHCP got IP: 10.72.154.211
```

When the dongle is unplugged, the driver detects the detach and tears down the connection:
```
[USBH-A] Device disconnected
[USBH-A] Force to detach 12
[USB-4G-I] DETACH
[USB-4G-I] Hotplug reinit heap:0x1051468
[USB-W] UPHY OTP flag 0xff, use default
[USB-4G-I] Link down Vid 0x0
[R-NAT-I] === USB Link DOWN ===
[R-NAT-I] [USB] LINK DOWN - INACTIVE
[R-NAT-APP-A] [USER-CB] USB Link DOWN
[R-NAT-I] Default GW changed to: 192.168.43.1 (AP, prio=0)
[R-NAT-I] [USB] WAN GW switched, reinitializing NAT
[R-NAT-I] [USB] IP released
```

# Note

NONE

# Supported IC

RTL8721F
