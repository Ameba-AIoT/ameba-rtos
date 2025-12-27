# Example Description

This example shows how to use chipen interrupt mode to wakeup system.

# HW Configuration

After the system boots, press and hold the `CHIP_EN` button for a duration longer than:  
`DEBOUNCE_TIME + SHORTPRESS_TIME + LONGPRESS_TIME`.

# SW Configuration

You can modify the macro `ACK_WND_KEEP_LP_INTR` in the example source file to change the interrupt behavior:

- **Uncomment** `ACK_WND_KEEP_LP_INTR`: To test the system reset behavior when the Long Press (LP) interrupt is **not cleared** within the ACK window.
- **Comment out** `ACK_WND_KEEP_LP_INTR` (Default): To test the standard behavior where the interrupt is cleared.

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

**1. Log Output:**
Regardless of the configuration, when the `CHIP_EN` interrupt occurs (in Active or Sleep mode), the serial log will display:
> `SP INT CPUID`
> `LP INT CPUID`

**2. System Behavior (Reset or Not):**
The post-interrupt behavior depends on the `ACK_WND_KEEP_LP_INTR` macro configuration:

*   **If `ACK_WND_KEEP_LP_INTR` is DEFINED:**
    The system will perform a **System Reset** immediately after printing the logs (due to the uncleared LP interrupt).

*   **If `ACK_WND_KEEP_LP_INTR` is UNDEFINED:**
    It will **NOT** reset.

**3. Wakeup system**
*   If in sleep mode, the system will **Wake Up**.

# Note

In DSLP mode, the system wakes up without the `SP INT CPUID` and `LP INT CPUID` logs.

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
