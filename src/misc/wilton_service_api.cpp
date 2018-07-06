/*
 * Copyright 2018, mike at myasnikov.mike@gmail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "wilton/wilton_service_api.h"

#include "wilton/support/alloc.hpp"
#include "wilton_service.hpp"


char* wilton_service_increase_thread_count() {
    try {
        wilton::service::wilton_service::increase_thread_count();
        return nullptr;
    } catch (const std::exception& e) {
        return wilton::support::alloc_copy(TRACEMSG(e.what() + "\nException raised"));
    }
}

char* wilton_service_decrease_thread_count() {
    try {
        wilton::service::wilton_service::decrease_thread_count();
        return nullptr;
    } catch (const std::exception& e) {
        return wilton::support::alloc_copy(TRACEMSG(e.what() + "\nException raised"));
    }
}

char* wilton_service_get_thread_count(int* count_out) {
    if (nullptr == count_out) return wilton::support::alloc_copy(TRACEMSG("Null 'count_out' parameter specified"));
    try {
        *count_out = wilton::service::wilton_service::get_thread_count();
        return nullptr;
    } catch (const std::exception& e) {
        return wilton::support::alloc_copy(TRACEMSG(e.what() + "\nException raised"));
    }
}
