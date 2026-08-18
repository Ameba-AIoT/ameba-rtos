# Example Description

Demonstrates real GPIO control via the MCP tool protocol.

An HTTP server is started on port 8080 that implements the
[Model Context Protocol](https://modelcontextprotocol.io/) (MCP 2025-03-26 / 2024-11-05).
One tool is registered:

| Tool | Description |
|---|---|
| `gpio_control` | Set any GPIO pin high or low; `pin` selects the pin, `state` is `"high"` or `"low"` |

# HW Configuration

Connect an LED (or logic analyser) to any available GPIO pin (e.g. `PA_25`).

Supported pin names: `PA_0`–`PA_31`, `PB_0`–`PB_31`, `PC_0`–`PC_8`.

# SW configuration

1. Adjust `EXAMPLE_MCP_PORT` / `EXAMPLE_MCP_ENDPOINT` in `example_mcp_gpio.h` if needed.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

After Wi-Fi connects, the serial log shows the server address and example curl commands.

Test with curl (replace `<ip>` with the device's IP address):

```bash
# initialize
curl -s -X POST http://<ip>:8080/mcp \
     -H 'Content-Type: application/json' \
     -d '{"jsonrpc":"2.0","id":1,"method":"initialize",
          "params":{"protocolVersion":"2025-03-26",
                    "clientInfo":{"name":"curl","version":"0"}}}'

# list tools
curl -s -X POST http://<ip>:8080/mcp \
     -H 'Content-Type: application/json' \
     -d '{"jsonrpc":"2.0","id":2,"method":"tools/list"}'

# set PA_0 high
curl -s -X POST http://<ip>:8080/mcp \
     -H 'Content-Type: application/json' \
     -d '{"jsonrpc":"2.0","id":3,"method":"tools/call",
          "params":{"name":"gpio_control",
                    "arguments":{"pin":"PA_0","state":"high"}}}'

# set PA_0 low
curl -s -X POST http://<ip>:8080/mcp \
     -H 'Content-Type: application/json' \
     -d '{"jsonrpc":"2.0","id":4,"method":"tools/call",
          "params":{"name":"gpio_control",
                    "arguments":{"pin":"PA_0","state":"low"}}}'

# invalid state → isError=true
curl -s -X POST http://<ip>:8080/mcp \
     -H 'Content-Type: application/json' \
     -d '{"jsonrpc":"2.0","id":5,"method":"tools/call",
          "params":{"name":"gpio_control",
                    "arguments":{"pin":"PA_0","state":"blink"}}}'
```

# Note

- The MCP server occupies the global `httpd` instance; do not call `httpd_start`
  elsewhere in the same firmware image.
- The GPIO pin is re-initialised as output on every call; no persistent state is kept
  between calls.

# Supported IC

RTL8721F
