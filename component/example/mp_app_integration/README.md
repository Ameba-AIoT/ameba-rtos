# Example Description

This example demonstrates how to integrate a customer's application into the Expand MP image, enabling calibration testing and running the customer application using only the Expand MP image.

The status of bit 0 in BKUP_REG1 is used to determine whether to run the customer application. This approach prevents the customer application from affecting the calibration results during calibration testing.

# HW Configuration

None

# SW configuration

1. Add AT command defined in the example into SDK as follows:
   a. Add `{c_CMPT_EXAMPLE_DIR}/mp_app_integration` into private include path in `/component/at_cmd/CMakeLists.txt`.
         ameba_list_append(private_includes
               ${c_CMPT_WIFI_DIR}/api
               ${c_CMPT_WIFI_DIR}/common
      +        ${c_CMPT_EXAMPLE_DIR}/mp_app_integration
         )
   b. Modify the file `component/at_cmd/atcmd_service.c` by adding `#include "example_mp_app_integration.h"` and including `at_custom_init` in the `log_init_table`.
      +#if (defined CONFIG_WHC_HOST || defined CONFIG_WHC_NONE)
      +#include "example_mp_app_integration.h"
      +#endif

      log_init_t log_init_table[] = {
         at_sys_init_common,
      +#if (defined CONFIG_WHC_HOST || defined CONFIG_WHC_NONE)
      +  at_custom_init,
      +#endif
2. Run `./menuconfig.py` in the project directory and select the following configuration:
   - Navigate to `CONFIG Mass Production` ---> Check `Enable MP` ---> Enter `MP Mode` ---> Select `expand`.
3. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.
4. Here is the usage method:
   a. After downloading the image, reset the chip. At this point, `execute_user_app()` (i.e., the customer application) will run.
   b. Enter `AT+MARKMP` to indicate that calibration and testing are required. This will set bit 0 of BKUP_REG1.
   c. Enter `AT+RST` to perform a software reset of the chip. The data in the backup register will not be lost, so `execute_user_app()` will not run at this time.
   d. Perform calibration and testing.
   e. Reset the chip either by power-cycling or by driving the `CHIP_EN` pin low and then high. The data in the backup register will be lost, and `execute_user_app()` will run again.

# Expect result

The Expand MP image can complete calibration testing and run the customer application without interference between the two.

# Note

None

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx