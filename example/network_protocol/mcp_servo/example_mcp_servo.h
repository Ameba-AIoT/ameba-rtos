#ifndef EXAMPLE_MCP_SERVO_H
#define EXAMPLE_MCP_SERVO_H

/* HTTP port the MCP server listens on. */
#define EXAMPLE_MCP_PORT      8080

/* URL path for the MCP endpoint. */
#define EXAMPLE_MCP_ENDPOINT  "/mcp"

/* Name reported in the MCP initialize response. */
#define EXAMPLE_MCP_NAME      "ameba-mcp-servo"

void example_mcp_servo(void);

#endif /* EXAMPLE_MCP_SERVO_H */
