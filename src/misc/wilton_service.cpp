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

#include "wilton_service.hpp"

#include <algorithm>
#include <atomic>

#include "staticlib/pimpl/forward_macros.hpp"
#include "wilton/support/exception.hpp"

namespace wilton {
namespace service {

class wilton_service::impl : public staticlib::pimpl::object::impl {
private:
    static std::atomic_int thread_count;
   
public:
	static void increase_thread_count() {
        ++thread_count;
	}
    static void decrease_thread_count() {
        --thread_count;
    }
    static int get_thread_count() {
        return thread_count;
    }
};

std::atomic_int wilton_service::impl::thread_count{0};

PIMPL_FORWARD_METHOD_STATIC(wilton_service, void, increase_thread_count, (), (), support::exception)
PIMPL_FORWARD_METHOD_STATIC(wilton_service, void, decrease_thread_count, (), (), support::exception)
PIMPL_FORWARD_METHOD_STATIC(wilton_service, int, get_thread_count, (), (), support::exception)

} // namespace
}
