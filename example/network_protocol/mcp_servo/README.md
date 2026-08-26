# Example Description

Demonstrates continuous-rotation servo control via the MCP tool protocol.
Target hardware: Tower Pro SSG90 (continuous rotation variant).

An HTTP server is started on port 8080 that implements the
[Model Context Protocol](https://modelcontextprotocol.io/) (MCP 2025-03-26 / 2024-11-05).
Two tools are registered:

| Tool | Description |
|---|---|
| `servo_set_speed` | Set speed; `speed` is -100 (full CW) to +100 (full CCW), 0 = stop |
| `servo_get_speed` | Return the current speed value |

PWM parameters:
- Timer: TIM4, channel 0
- Tick clock: 40 MHz / 40 = 1 MHz
- Period: 20 ms (50 Hz)
- Pulse mapping: 1000 µs = full CW / 1500 µs = stop / 2000 µs = full CCW

# HW Configuration

```
Servo signal  ->  PA_6
Servo VCC     ->  5 V
Servo GND     ->  GND
```

# SW configuration

1. Adjust `EXAMPLE_MCP_PORT` / `EXAMPLE_MCP_ENDPOINT` in `example_mcp_servo.h` if needed.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

After Wi-Fi connects the serial log prints:

```
[MCP_SERVO] MCP server ready at http://<ip>:8080/mcp
```

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

# full clockwise (speed=-100)
curl -s -X POST http://<ip>:8080/mcp \
     -H 'Content-Type: application/json' \
     -d '{"jsonrpc":"2.0","id":3,"method":"tools/call",
          "params":{"name":"servo_set_speed",
                    "arguments":{"speed":-100}}}'

# stop (speed=0)
curl -s -X POST http://<ip>:8080/mcp \
     -H 'Content-Type: application/json' \
     -d '{"jsonrpc":"2.0","id":4,"method":"tools/call",
          "params":{"name":"servo_set_speed",
                    "arguments":{"speed":0}}}'

# full counter-clockwise (speed=+100)
curl -s -X POST http://<ip>:8080/mcp \
     -H 'Content-Type: application/json' \
     -d '{"jsonrpc":"2.0","id":5,"method":"tools/call",
          "params":{"name":"servo_set_speed",
                    "arguments":{"speed":100}}}'

# half speed clockwise (speed=-50)
curl -s -X POST http://<ip>:8080/mcp \
     -H 'Content-Type: application/json' \
     -d '{"jsonrpc":"2.0","id":6,"method":"tools/call",
          "params":{"name":"servo_set_speed",
                    "arguments":{"speed":-50}}}'

# get current speed
curl -s -X POST http://<ip>:8080/mcp \
     -H 'Content-Type: application/json' \
     -d '{"jsonrpc":"2.0","id":7,"method":"tools/call",
          "params":{"name":"servo_get_speed",
                    "arguments":{}}}'
```

# Note

- The MCP server occupies the global `httpd` instance; do not call `httpd_start`
  elsewhere in the same firmware image.
- The SSG90 stops at 1500 µs, but the exact neutral point varies by unit. If the servo
  drifts at speed=0, fine-tune `SERVO_PULSE_STOP` in `example_mcp_servo.c`.

# Supported IC

RTL8721F
RTL8720F
