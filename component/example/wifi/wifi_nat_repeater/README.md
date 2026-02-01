# Example Description

A simple usage of nat repeater.

# HW Configuration

None

# SW configuration

1. Define `CONFIG_IP_NAT` by selecting configuration in menuconfig:
	1. use menuconfig to show configurations;
	2. choose `CONFIG LWIP`;
	3. choose `NAT REPEATER Enable`.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. A softap will be started automatically after booting.

2. Use atcmd to connect to the public WiFi under sta mode, then use cellphone to connect to softap.  
If sta has linked to public WiFi before, it will connect to it automatically by fast connect after booting which means you don't have to use atcmd.

3. If sta has successfully connected to public WiFi which can access wan, then cellphone can also access wan.

# Note

How to use atcmd to connect to public WiFi under sta mode:
   - Use CMD `AT+WLCONN=ssid,xxx,pw,xxx`; if public WiFi is set with password otherwise don't need `pw,xxx`.

# Supported IC

RTL8721Dx
RTL8721F
