# Example Description

This example describes how to implement high/low level trigger on a gpio pin by MBED GPIO API.

# HW Configuration

Connect wires:

- Connect `GPIO_IRQ_LEVEL_PIN` to `GPIO_SIGNAL_SOURCE` defined in `gpio_ext.h`

For example:

- On AmebaSmart, connect as below

  - Connect `GPIO_SIGNAL_SOURCE (_PB_10)` to `GPIO_IRQ_LEVEL_PIN (_PA_10)`
- On AmebaLite, connect as below

  - Connect `GPIO_SIGNAL_SOURCE (_PB_5)` to `GPIO_IRQ_LEVEL_PIN (_PA_29)`
- On AmebaDplus, connect as below

  - `Connect GPIO_SIGNAL_SOURCE (_PB_10)` to `GPIO_IRQ_LEVEL_PIN (_PA_12)`

# SW configuration

1. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.
2. `PA_12` map to `GPIOA_12`, `PB_8`  map to `GPIOB_8` and etc.
3. `GPIO_IRQ_LEVEL_PIN` is configured to listen to level events and irq is enabled, and can handle both high/low level events.
4. GPIO Interrupt Polarity will inverse in ISR.
5. `GPIO_SIGNAL_SOURCE` is a signal source that changes level between high and low periodically.

# Expect result

In this example, you will see log that prints high/low level event periodically.

# Note

None.

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
- AmebaD
