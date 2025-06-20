# Example Description

This example shows how to use cJSON lib to generate JSON data and how to parse JSON data.

# HW Configuration

None

# SW configuration

1. You can review the EXAMPLE file `example_cJSON.c` located in `component/example/cJSON/`. Try to modify the function of `generate_json_data()` according to specific data structure, and then parse the generated data structure in `handle_json_data()`.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

The data structure you defined display on the screen after booting.
For example:

```
{
	"Motion_Sensor":	5,
	"Light":	{
		"Red":	253,
		"Green":	123,
		"Blue":	43
	}
}
```

# Note

None

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
