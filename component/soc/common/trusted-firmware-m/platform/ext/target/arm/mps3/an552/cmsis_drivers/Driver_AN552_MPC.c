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

#if (defined (RTE_SRAM_MPC) && (RTE_SRAM_MPC == 1)) || \
    (defined (RTE_ISRAM0_MPC) && (RTE_ISRAM0_MPC == 1)) || \
    (defined (RTE_ISRAM1_MPC) && (RTE_ISRAM1_MPC == 1)) || \
    (defined (RTE_QSPI_MPC) && (RTE_QSPI_MPC == 1)) || \
    (defined (RTE_DDR4_MPC) && (RTE_DDR4_MPC == 1))

#if (defined (RTE_SRAM_MPC) && (RTE_SRAM_MPC == 1))
ARM_DRIVER_MPC(MPC_SRAM_DEV, Driver_SRAM_MPC);
#endif /* RTE_SRAM_MPC */

#if (defined (RTE_ISRAM0_MPC) && (RTE_ISRAM0_MPC == 1))
ARM_DRIVER_MPC(MPC_ISRAM0_DEV, Driver_ISRAM0_MPC);
#endif /* RTE_ISRAM0_MPC */

#if (defined (RTE_ISRAM1_MPC) && (RTE_ISRAM1_MPC == 1))
ARM_DRIVER_MPC(MPC_ISRAM1_DEV, Driver_ISRAM1_MPC);
#endif /* RTE_ISRAM1_MPC */

#if (defined (RTE_QSPI_MPC) && (RTE_QSPI_MPC == 1))
ARM_DRIVER_MPC(MPC_QSPI_DEV, Driver_QSPI_MPC);
#endif /* RTE_QSPI_MPC */

#if (defined (RTE_DDR4_MPC) && (RTE_DDR4_MPC == 1))
ARM_DRIVER_MPC(MPC_DDR4_DEV, Driver_DDR4_MPC);
#endif /* RTE_DDR4_MPC */

#endif
