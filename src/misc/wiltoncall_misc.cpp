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
#include "wilton/wilton_service_api.h"
#include "staticlib/config/os.hpp"

#ifdef STATICLIB_WINDOWS
#include <windows.h>
#include <psapi.h>
#elif defined(STATICLIB_ANDROID) || defined (STATICLIB_LINUX) || defined (STATICLIB_MAC) 
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
    int64_t pid; // beacause getpid is int32_t, but GetCurrentProcessId returns DWORD which uint32_t
#ifdef STATICLIB_WINDOWS
    pid = GetCurrentProcessId();
#elif defined(STATICLIB_ANDROID) || defined (STATICLIB_LINUX) || defined (STATICLIB_MAC) 
    pid = ::getpid();
#endif
    return support::make_string_buffer(sl::support::to_string(pid));
}

support::buffer service_get_process_memory_size_bytes(sl::io::span<const char>) {
#ifdef STATICLIB_WINDOWS
    PROCESS_MEMORY_COUNTERS pmc;
    // THere may be different PSAPI_VERSION, but it always use GetProcessMemoryInfo function name
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    // https://docs.microsoft.com/en-us/windows/desktop/api/psapi/ns-psapi-_process_memory_counters
    // PagefileUsage - The Commit Charge value in bytes for this process. 
    //                 Commit Charge is the total amount of memory that the memory manager has committed for a running process.
    return support::make_string_buffer(sl::support::to_string(pmc.PagefileUsage));
#elif defined(STATICLIB_ANDROID) || defined (STATICLIB_LINUX) || defined (STATICLIB_MAC) 
#endif
    return support::make_null_buffer();
}

support::buffer service_get_thread_count(sl::io::span<const char> ) {
    int count = 0;
    wilton_service_get_thread_count(&count);
    return support::make_string_buffer(sl::support::to_string(count));
}

} // namespace
}
