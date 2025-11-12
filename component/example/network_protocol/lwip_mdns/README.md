# Example Description

An example to use the mdns APIs in `example_lwip_mdns.c`.

# HW Configuration

None

# SW configuration

1. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

2. In example, operations on mDNS responder and service are presented.

3. If device Wi-Fi connection is setup by fast reconnect (Please refer to readme of fast reconnect example),
the mDNS responder could be started after IP address is available.

4. An example service is registered, updated. Finally, the mDNS responder is stopped.

# Expect result

1. To discover service in network, the `dns-sd` command provided in Bonjour SDK can be used.

2. Bonjour SDK is available on Apple developer support site, and can be downloaded by Apple developer.

3. In the following, `dns-sd -B _service1` command is used to browse for service instance of type `_service1`, and service of `ameba._service1._tcp.local.` is found.

4. `dns-sd -L ameba _service1 local` command is used to look up this service instance, and the content of service text record is shown.

# Note

None

# Supported IC

- AmebaSmart
  * RTL8730E
- AmebaLite
  * RTL8726E
  * RTL8720E
  * RTL8713E
  * RTL8710E
- AmebaDplus
  * RTL8721Dx
- AmebaGreen2
  * RTL8721F
