#/** @file
# * Copyright (c) 2019, Arm Limited or its affiliates. All rights reserved.
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

list(APPEND CC_SOURCE
	test_entry_i071.c
	test_i071.c
)
list(APPEND CC_OPTIONS )
list(APPEND AS_SOURCE  )
list(APPEND AS_OPTIONS )

list(APPEND CC_SOURCE_SPE
	test_i071.c
	test_supp_i071.c
)
list(APPEND CC_OPTIONS_SPE )
list(APPEND AS_SOURCE_SPE  )
list(APPEND AS_OPTIONS_SPE )
