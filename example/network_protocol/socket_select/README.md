# Example Description

This example shows how to use socket `select()` to handle socket read from clients or remote server.

# HW Configuration

None

# SW configuration

1. Modify `SERVER_PORT` definition for listen port of created TCP server.

2. Can enable `CONNECT_REMOTE` to include TCP connection to remote server in example.

3. Modify `REMOTE_HOST` and `REMOTE_PORT` for remote server.

4. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

The socket select example thread will be started automatically when booting.

A local TCP server will be started to wait for connection. Can use a TCP client (which is associated in the same network with local TCP server) connecting to this server to send data.

If `CONNECT_REMOTE` is enabed in example, a local TCP client will connect to the remote server. Can send data at the remote server to the created TCP connection.

# Note

If `CONNECT_REMOTE` is enabed in example, Ameba should connect to the same network with remote server.

Without network connection, will encounter TCP client connect error.

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
