# Example Description

This example demonstrates how to use mbed-CoAP C library to build and parse a CoAP message.

# HW Configuration

None

# SW configuration

1. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. In the example, a confirmable GET request is send to test server `californium.eclipse.org` to retrieve the resource under path `/obs`. The expected return is an ACK message with payload containing the Greenwich Mean Time (GMT).

2. In the example, you can also set a GET request sent to test server `coap.me` to retrieve the resource under path `/hello`. The expected return is an ACK message with payload `world`.


# Note

1. Company Firewall may block CoAP message. You can use copper (https://addons.mozilla.org/en-US/firefox/addon/copper-270430/) to test the server's reachability.

2. Make sure `SERVER_PORT` has been set.

3. For more details, please refer to UM0150 Realtek Ameba CoAP User Guide.pdf

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
