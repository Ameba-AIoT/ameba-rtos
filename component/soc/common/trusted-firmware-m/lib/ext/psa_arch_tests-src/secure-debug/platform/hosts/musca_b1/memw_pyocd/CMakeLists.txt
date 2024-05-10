#/** @file
# * Copyright (c) 2022 Arm Limited or its affiliates. All rights reserved.
# * SPDX-License-Identifier : Apache-2.0
# *
# * Licensed under the Apache License, Version 2.0 (the "License");
# * you may not use this file except in compliance with the License.
# * You may obtain a copy of the License at
# *
# *  http://www.apache.org/licenses/LICENSE-2.0
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an "AS IS" BASIS,
# * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# * See the License for the specific language governing permissions and
# * limitations under the License.

# Set the minimum required version of CMake for the project
cmake_minimum_required(VERSION 3.10)
PROJECT (platform_host)

include_directories(
    ${CMAKE_SOURCE_DIR}/platform/common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

add_library(platform_host STATIC)
target_sources(platform_host PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pal_interfaces.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    )

target_link_libraries(platform_host)
