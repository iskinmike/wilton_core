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
 * File:   payload_handle_registry.hpp
 * Author: alex
 *
 * Created on June 12, 2017, 12:38 PM
 */

#ifndef WILTON_SUPPORT_PAYLOAD_HANDLE_REGISTRY_HPP
#define WILTON_SUPPORT_PAYLOAD_HANDLE_REGISTRY_HPP

#include <cstdint>
#include <functional>
#include <mutex>
#include <unordered_map>
#include <utility>

#include "staticlib/config.hpp"

namespace wilton {
namespace support {

template<typename T, typename P>
class payload_handle_registry {
    std::unordered_map<T*, P> registry;
    std::mutex mtx;
    std::function<void(T*)> destoyer;

public:

    payload_handle_registry() { }

    template<typename DestroyFunc>
    payload_handle_registry(DestroyFunc destroyFunc) :
    destoyer(destroyFunc) {
#ifdef STATICLIB_NOEXCEPT_SUPPORTED
        static_assert(noexcept(destroyFunc(nullptr)),
                "Please check that the destroyer func cannot throw, "
                "and mark the lambda as 'noexcept'.");
#endif
    }

    payload_handle_registry(const payload_handle_registry&) = delete;

    payload_handle_registry& operator=(const payload_handle_registry&) = delete;

    ~payload_handle_registry() STATICLIB_NOEXCEPT {
// msvcr doesn't like that in JNI mode
#ifndef STATICLIB_WINDOWS
        std::lock_guard<std::mutex> lock{mtx};
#endif // STATICLIB_WINDOWS
        if (destoyer) {
            for (auto& pa : registry) {
                destoyer(pa.first);
            }
        }
        registry.clear();
    }

    int64_t put(T* ptr, P&& ctx) {
        std::lock_guard<std::mutex> lock(mtx);
        auto pair = registry.emplace(ptr, std::move(ctx));
        return pair.second ? reinterpret_cast<int64_t> (ptr) : 0;
    }

    std::pair<T*, P> remove(int64_t handle) {
        std::lock_guard<std::mutex> lock(mtx);
        T* ptr = reinterpret_cast<T*> (handle);
        auto it = registry.find(ptr);
        if (registry.end() != it) {
            auto ctx = std::move(it->second);
            registry.erase(ptr);
            return std::make_pair(ptr, std::move(ctx));
        } else {
            return std::make_pair(nullptr, P());
        }
    }

    std::mutex& mutex() {
        return mtx;
    }

    // must be used only with external locking on mutex()
    int64_t put_nolock(T* ptr, P&& ctx) {
        auto pair = registry.emplace(ptr, std::move(ctx));
        return pair.second ? reinterpret_cast<int64_t> (ptr) : 0;
    }
};

} // namespace
}

#endif /* WILTON_SUPPORT_PAYLOAD_HANDLE_REGISTRY_HPP */

