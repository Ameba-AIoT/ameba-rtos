/** @file
 * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

Steps to follow to run debug test via DS-5 using debugger_script.py
1. Setup DS-5 "Debug Configuration" to connect to the appropriate model/target
2. For FVP, start M33 model with "-S" option. This will now wait for debugger to be connected before it can proceed.
3. From DS-5, use the "Connect to Target" option. This will detect and connect to the running model if the Debug Configurations were correct.
4. Now run the script with appropriate arguments in the command line of DS-5. Use the absolute path of the script, for ex
   /home/<user_name>/syscomp_tbsa_m/tools/debug/debugger_script.py <arg1> <arg2>
