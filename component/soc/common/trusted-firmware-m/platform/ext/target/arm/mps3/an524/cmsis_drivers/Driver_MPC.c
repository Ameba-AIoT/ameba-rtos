/*
 * Copyright (c) 2016-2023 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cmsis_driver_config.h"
#include "RTE_Device.h"
#include "Driver_MPC_Sie.h"

#if (defined (RTE_ISRAM0_MPC) && (RTE_ISRAM0_MPC == 1)) || \
    (defined (RTE_ISRAM1_MPC) && (RTE_ISRAM1_MPC == 1)) || \
    (defined (RTE_ISRAM2_MPC) && (RTE_ISRAM2_MPC == 1)) || \
    (defined (RTE_ISRAM3_MPC) && (RTE_ISRAM3_MPC == 1)) || \
    (defined (RTE_CODE_SRAM_MPC) && (RTE_CODE_SRAM_MPC == 1)) || \
    (defined (RTE_SSRAM1_MPC) && (RTE_SSRAM1_MPC == 1)) || \
    (defined (RTE_SSRAM2_MPC) && (RTE_SSRAM2_MPC == 1)) || \
    (defined (RTE_SSRAM3_MPC) && (RTE_SSRAM3_MPC == 1)) || \
    (defined (RTE_QSPI_MPC) && (RTE_QSPI_MPC == 1)) || \
    (defined (RTE_EFLASH0_MPC) && (RTE_EFLASH0_MPC == 1)) || \
    (defined (RTE_EFLASH1_MPC) && (RTE_EFLASH1_MPC == 1)) || \
    (defined (RTE_BRAM_MPC) && (RTE_BRAM_MPC == 1))

#if (defined (RTE_ISRAM0_MPC) && (RTE_ISRAM0_MPC == 1))
ARM_DRIVER_MPC(MPC_ISRAM0_DEV, Driver_ISRAM0_MPC);
#endif /* RTE_ISRAM0_MPC */

#if (defined (RTE_ISRAM1_MPC) && (RTE_ISRAM1_MPC == 1))
ARM_DRIVER_MPC(MPC_ISRAM1_DEV, Driver_ISRAM1_MPC);
#endif /* RTE_ISRAM1_MPC */

#if (defined (RTE_ISRAM2_MPC) && (RTE_ISRAM2_MPC == 1))
ARM_DRIVER_MPC(MPC_ISRAM2_DEV, Driver_ISRAM2_MPC);
#endif /* RTE_ISRAM2_MPC */

#if (defined (RTE_ISRAM3_MPC) && (RTE_ISRAM3_MPC == 1))
ARM_DRIVER_MPC(MPC_ISRAM3_DEV, Driver_ISRAM3_MPC);
#endif /* RTE_ISRAM3_MPC */

#if (defined (RTE_CODE_SRAM_MPC) && (RTE_CODE_SRAM_MPC == 1))
ARM_DRIVER_MPC(MPC_CODE_SRAM_DEV, Driver_CODE_SRAM_MPC);
#endif /* RTE_CODE_SRAM_MPC */

#if (defined (RTE_SSRAM1_MPC) && (RTE_SSRAM1_MPC == 1))
ARM_DRIVER_MPC(MPC_SSRAM1_DEV, Driver_SSRAM1_MPC);
#endif /* RTE_SSRAM1_MPC */

#if (defined (RTE_SSRAM2_MPC) && (RTE_SSRAM2_MPC == 1))
ARM_DRIVER_MPC(MPC_SSRAM2_DEV, Driver_SSRAM2_MPC);
#endif /* RTE_SSRAM2_MPC */

#if (defined (RTE_SSRAM3_MPC) && (RTE_SSRAM3_MPC == 1))
ARM_DRIVER_MPC(MPC_SSRAM3_DEV, Driver_SSRAM3_MPC);
#endif /* RTE_SSRAM3_MPC */

#if (defined (RTE_QSPI_MPC) && (RTE_QSPI_MPC == 1))
ARM_DRIVER_MPC(MPC_QSPI_DEV, Driver_QSPI_MPC);
#endif /* RTE_QSPI_MPC */

#if (defined (RTE_EFLASH0_MPC) && (RTE_EFLASH0_MPC == 1))
ARM_DRIVER_MPC(MPC_EFLASH0_DEV, Driver_EFLASH0_MPC);
#endif /* RTE_EFLASH0_MPC */

#if (defined (RTE_EFLASH1_MPC) && (RTE_EFLASH1_MPC == 1))
ARM_DRIVER_MPC(MPC_EFLASH1_DEV, Driver_EFLASH1_MPC);
#endif /* RTE_EFLASH1_MPC */

#if (defined (RTE_BRAM_MPC) && (RTE_BRAM_MPC == 1))
ARM_DRIVER_MPC(MPC_BRAM_DEV, Driver_BRAM_MPC);
#endif /* RTE_BRAM_MPC */

#endif
