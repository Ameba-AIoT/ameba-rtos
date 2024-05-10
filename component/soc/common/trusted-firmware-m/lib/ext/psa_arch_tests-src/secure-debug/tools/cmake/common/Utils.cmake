#/** @file
# * Copyright (c) 2021, Arm Limited or its affiliates. All rights reserved.
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
#**/

# Function to CMake arguments
function(_check_arguments)
        foreach(_ARG IN LISTS ARGV)
                if(NOT DEFINED ${_ARG})
                        message(FATAL_ERROR "[PSA] : ${_ARG} is not passed! Please specify -D${_ARG}=<...> to CMake.")
                endif()
        endforeach()
endfunction(_check_arguments)

# Function to get all the folders inside given parent directory
function(_get_sub_dir_list result parent_dir)
        file(GLOB parent_dir_items RELATIVE ${parent_dir} ${parent_dir}/*)
        set(dir_list "")
        foreach(item ${parent_dir_items})
                if(IS_DIRECTORY ${parent_dir}/${item})
                        list(APPEND dir_list ${item})
                endif()
        endforeach()
        set(${result} ${dir_list} PARENT_SCOPE)
endfunction(_get_sub_dir_list)
