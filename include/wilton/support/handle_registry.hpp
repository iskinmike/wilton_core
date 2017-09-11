/* 
 * File:   handle_registry.hpp
 * Author: alex
 *
 * Created on June 12, 2017, 12:32 PM
 */

#ifndef WILTON_SUPPORT_HANDLE_REGISTRY_HPP
#define WILTON_SUPPORT_HANDLE_REGISTRY_HPP

#include <cstdint>
#include <functional>
#include <mutex>
#include <unordered_set>

#include "staticlib/config.hpp"

namespace wilton {
namespace support {

template<typename T>
class handle_registry {
    std::unordered_set<T*> registry;
    std::mutex mutex;
    std::function<void(T*)> destoyer;

public:

    handle_registry() { }
    
    template<typename DestroyFunc>
    handle_registry(DestroyFunc destroyFunc):
    destoyer(destroyFunc) {
#ifdef STATICLIB_NOEXCEPT_SUPPORTED
        static_assert(noexcept(destroyFunc(nullptr)),
                "Please check that the destroyer func cannot throw, "
                "and mark the lambda as 'noexcept'.");
#endif
    }
    
    handle_registry(const handle_registry&) = delete;
    
    handle_registry& operator=(const handle_registry&) = delete;
    
    ~handle_registry() STATICLIB_NOEXCEPT {
        std::lock_guard<std::mutex> lock{mutex};
        if (destoyer) {
            for (T* ptr : registry) {
                destoyer(ptr);
            }
        }
        registry.clear();
    }
    
    int64_t put(T* ptr) {
        std::lock_guard<std::mutex> lock{mutex};
        auto pair = registry.insert(ptr);
        return pair.second ? reinterpret_cast<int64_t> (ptr) : 0;
    }

    T* remove(int64_t handle) {
        std::lock_guard<std::mutex> lock{mutex};
        T* ptr = reinterpret_cast<T*> (handle);
        auto erased = registry.erase(ptr);
        return 1 == erased ? ptr : nullptr;
    }

    T* peek(int64_t handle) {
        std::lock_guard<std::mutex> lock{mutex};
        T* ptr = reinterpret_cast<T*> (handle);
        auto exists = registry.count(ptr);
        return 1 == exists ? ptr : nullptr;
    }
};

} // namespace
}

#endif /* WILTON_SUPPORT_HANDLE_REGISTRY_HPP */

