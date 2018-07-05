/*
 * Copyright 2017, alex at staticlibs.net
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

/* 
 * File:   wiltoncall_misc.cpp
 * Author: alex
 *
 * Created on January 10, 2017, 12:34 PM
 */

#include <iostream>
#include <string>

#include "call/wiltoncall_internal.hpp"

#include "wilton/wilton.h"

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#elif defined(__linux__) || defined (__ANDROID_API__)
#include <sys/types.h>
#include <unistd.h>
#endif

namespace wilton {
namespace misc {

support::buffer get_wiltoncall_config(sl::io::span<const char>) {
    return support::make_json_buffer(*internal::shared_wiltoncall_config());
}

support::buffer stdin_readline(sl::io::span<const char>) {
    std::string res;
    std::getline(std::cin,res);
    return support::make_string_buffer(res);
}

support::buffer service_get_pid(sl::io::span<const char>) {
    uint64_t pid;
#ifdef _WIN32
    pid = GetCurrentProcessId();
#elif defined(__linux__) || defined (__ANDROID_API__)
    pid = getpid();
#elif TARGET_OS_X

#endif
    return support::make_string_buffer(sl::support::to_string(pid));
}

support::buffer service_get_process_memory_size_bytes(sl::io::span<const char>) {

#ifdef _WIN32
    uint64_t vm_process_usage;
    PROCESS_MEMORY_COUNTERS pmc;
    // THere may be different PSAPI_VERSION, but it always use GetProcessMemoryInfo function name
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    // https://docs.microsoft.com/en-us/windows/desktop/api/psapi/ns-psapi-_process_memory_counters
    // PagefileUsage - The Commit Charge value in bytes for this process. 
    //                 Commit Charge is the total amount of memory that the memory manager has committed for a running process.
    vm_process_usage = pmc.PagefileUsage;
    return support::make_string_buffer(sl::support::to_string(vm_process_usage));
#elif defined(__linux__) || defined (__ANDROID_API__)
#elif TARGET_OS_X
#endif
    return support::make_null_buffer();
}

} // namespace
}
