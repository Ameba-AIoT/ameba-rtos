/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
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

#ifndef ARM_BACKTRACE_H
#define ARM_BACKTRACE_H

void do_stack_backtrace(uint32_t *pc, uint32_t *sp, uint32_t *lr);
void get_call_stack(void **caller, uint32_t max_level);
void set_backtrace_depth(uint32_t backtrace_depth);
#endif
