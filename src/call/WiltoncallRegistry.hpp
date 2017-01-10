/* 
 * File:   WiltoncallRegistry.hpp
 * Author: alex
 *
 * Created on January 8, 2017, 4:48 PM
 */

#ifndef WILTON_CALL_WILTONCALLREGISTRY_HPP
#define	WILTON_CALL_WILTONCALLREGISTRY_HPP

#include <mutex>
#include <string>
#include <unordered_map>

#include "staticlib/serialization.hpp"

#include "common/WiltonInternalException.hpp"

namespace wilton {
namespace call {

namespace { // anonymous

namespace ss = staticlib::serialization;

using fun_type = std::function<std::string(const std::string&)>;
using map_type = std::unordered_map<std::string, fun_type>;

} // namespace

class WiltoncallRegistry {
    std::mutex mutex;
    map_type registry;
    
public:
    void put(const std::string& name, fun_type callback) {
        std::lock_guard<std::mutex> guard{mutex};
        if (name.empty()) {
            throw common::WiltonInternalException(TRACEMSG("Invalid empty wilton_function name specified"));
        }
        auto pa = registry.emplace(name, callback);
        if (!pa.second) {
            throw common::WiltonInternalException(TRACEMSG(
                    "Invalid duplicate wilton_function name specified: [" + name + "]"));
        }
    }
    
    void remove(const std::string& name) {
        std::lock_guard<std::mutex> guard{mutex};
        if (name.empty()) {
            throw common::WiltonInternalException(TRACEMSG("Invalid empty wilton_function name specified"));
        }
        auto res = registry.erase(name);
        if (0 == res) {
            throw common::WiltonInternalException(TRACEMSG(
                    "Invalid unknown wilton_function name specified: [" + name + "]"));
        }
    }
    
    std::string invoke(const std::string& name, const std::string& data) {
        if (name.empty()) {
            throw common::WiltonInternalException(TRACEMSG("Invalid empty wilton_function name specified"));
        }
        try {
            // get function
            fun_type fun = [](const std::string&) {
                return std::string();
            };
            {
                std::lock_guard<std::mutex> guard{mutex};
                auto it = registry.find(name);
                if (registry.end() == it) {
                    throw common::WiltonInternalException(TRACEMSG(
                            "Invalid unknown wilton_function name specified: [" + name + "]"));
                }
                fun = it->second;
            }
            // invoke
            return fun(data);
        } catch (const std::exception& e) {
            throw common::WiltonInternalException(TRACEMSG(e.what() + 
                    "\nwiltoncall error for function: [" + name + "]"));
        }
    }

};

} // namespace
}

#endif	/* WILTON_CALL_WILTONCALLREGISTRY_HPP */

