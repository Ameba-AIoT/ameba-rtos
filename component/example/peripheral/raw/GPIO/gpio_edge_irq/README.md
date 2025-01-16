# Example Description

This example describes how to implement falling edge trigger on a gpio pin by RAW GPIO API.

This example does not support the loguart command response.

# HW Configuration

Connect wires:

- Connect `GPIO_IRQ_EDGE_PIN` to `GPIO_SIGNAL_SOURCE`

For example:

- On AmebaSmart, then connect as below
  - Connect `GPIO_SIGNAL_SOURCE (_PB_10)` to `GPIO_IRQ_EDGE_PIN (_PA_10)`
- On AmebaLite, then connect as below
  - Connect `GPIO_SIGNAL_SOURCE (_PB_5)` to `GPIO_IRQ_EDGE_PIN (_PA_29)`
- On AmebaDplus, then connect as below
  - Connect `GPIO_SIGNAL_SOURCE (_PB_10)` to `GPIO_IRQ_EDGE_PIN (_PA_12)`

# SW configuration

1. How to compile :

   - Copy main.c and gpio_ext.h under folder src to project\realtek_amebaxxx_va0_example\src\src_xxx
2. `PA_12` map to `GPIOA_12 `, `PB_8`  map to `GPIOB_8` and etc.
3. `GPIO_IRQ_EDGE_PIN` is configured to listen to rising edge events, and can handle both rising/falling edge events:

   - When `GPIO_IRQ_EDGE_PIN` catches falling edge events, it is configured to listen to rising edge events
   - When `GPIO_IRQ_EDGE_PIN` catches rising edge events, it is changed back to listen to falling edge events
4. `GPIO_SIGNAL_SOURCE` is signal source that changes level to high and low periodically.

# Expect result

In this example, you will see log that prints rising/falling edge event periodically.

# Note

None.

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
- AmebaD
