/* 
 * File:   wiltoncall.cpp
 * Author: alex
 *
 * Created on January 8, 2017, 1:44 PM
 */

#include "wilton/wiltoncall.h"

#include "staticlib/config.hpp"
#include "staticlib/utils.hpp"

#include "common/WiltonInternalException.hpp"
#include "call/WiltoncallRegistry.hpp"
#include "call/wiltoncall_internal.hpp"

namespace { // anonymous

namespace sc = staticlib::config;
namespace su = staticlib::utils;
namespace wc = wilton::common;

wilton::call::WiltoncallRegistry& static_registry() {
    static wilton::call::WiltoncallRegistry registry;
    return registry;
}

} // namespace

char* wiltoncall_init() {
    try {
        auto& reg = static_registry();
        
        reg.put("tcp_wait_for_connection", wilton::misc::tcp_wait_for_connection);
        
        return nullptr;
    } catch (const std::exception& e) {
        return su::alloc_copy(TRACEMSG(e.what() +
                "\n'wiltoncall' initialization error"));
    }
}

char* wiltoncall(char* call_name, int call_name_len, char* json_in, int json_in_len,
        char** json_out, int* json_out_len) /* noexcept */ {
    if (nullptr == call_name) return su::alloc_copy(TRACEMSG("Null 'call_name' parameter specified"));
    if (!su::is_positive_uint16(call_name_len)) return su::alloc_copy(TRACEMSG(
            "Invalid 'call_name_len' parameter specified: [" + sc::to_string(call_name_len) + "]"));
    if (nullptr == json_in) return su::alloc_copy(TRACEMSG("Null 'json_in' parameter specified"));
    if (!su::is_positive_uint32(json_in_len)) return su::alloc_copy(TRACEMSG(
            "Invalid 'json_in_len' parameter specified: [" + sc::to_string(json_in_len) + "]"));
    if (nullptr == json_out) return su::alloc_copy(TRACEMSG("Null 'json_out' parameter specified"));
    if (nullptr == json_out_len) return su::alloc_copy(TRACEMSG("Null 'json_out_len' parameter specified"));
    std::string call_name_str = "";
    std::string json_in_str = "";
    try {
        uint16_t call_name_len_u16 = static_cast<uint16_t> (call_name_len);
        call_name_str = std::string(call_name, call_name_len_u16);
        uint32_t json_in_len_u32 = static_cast<uint32_t> (json_in_len);
        json_in_str = std::string(json_in, json_in_len_u32);
        std::string out = static_registry().invoke(call_name_str, json_in_str);
        *json_out = su::alloc_copy(out);
        *json_out_len = out.length();
        return nullptr;
    } catch (const std::exception& e) {
        return su::alloc_copy(TRACEMSG(e.what() + 
                "\n'wiltoncall' error for name: [" + call_name_str + "], data: [" + json_in_str + "]"));
    }
}

char* wiltoncall_register(char* call_name, int call_name_len, void* call_ctx,
        char* (*call_cb)
        (void* call_ctx, const char* json_in, int json_in_len, char** json_out, int* json_out_len)) /* noexcept */ {
    if (nullptr == call_name) return su::alloc_copy(TRACEMSG("Null 'call_name' parameter specified"));
    if (!su::is_positive_uint16(call_name_len)) return su::alloc_copy(TRACEMSG(
            "Invalid 'call_name_len' parameter specified: [" + sc::to_string(call_name_len) + "]"));
    if (nullptr == call_cb) return su::alloc_copy(TRACEMSG("Null 'call_cb' parameter specified"));
    try {
        uint16_t call_name_len_u16 = static_cast<uint16_t> (call_name_len);
        std::string call_name_str{call_name, call_name_len_u16};
        // todo: consider different signature to prevent data coping
        auto fun = [call_ctx, call_cb](const std::string& data) {
            char* out = nullptr;
            int out_len = 0;
            auto err = call_cb(call_ctx, data.c_str(), static_cast<int>(data.length()), 
                    std::addressof(out), std::addressof(out_len));
            if (nullptr != err) {
                std::string msg = TRACEMSG(std::string(err));
                wilton_free(err);
                throw wc::WiltonInternalException(msg);
            }
            if (nullptr == out) {
                throw wc::WiltonInternalException(TRACEMSG("Invalid 'null' result returned"));
            }
            if (!su::is_uint32(out_len)){
                throw wc::WiltonInternalException(TRACEMSG(
                    "Invalid result length value returned: [" + sc::to_string(out_len) + "]"));
            }
            return std::string(out, static_cast<uint32_t>(out_len));
        };
        static_registry().put(call_name_str, fun);
        return nullptr;
    } catch (const std::exception& e) {
        return su::alloc_copy(TRACEMSG(e.what() + "\nException raised"));
    }
}

char* wiltoncall_remove(char* call_name, int call_name_len) {
    if (nullptr == call_name) return su::alloc_copy(TRACEMSG("Null 'call_name' parameter specified"));
    if (!su::is_positive_uint16(call_name_len)) return su::alloc_copy(TRACEMSG(
            "Invalid 'call_name_len' parameter specified: [" + sc::to_string(call_name_len) + "]"));
    try {
        uint16_t call_name_len_u16 = static_cast<uint16_t> (call_name_len);
        std::string call_name_str{call_name, call_name_len_u16};
        static_registry().remove(call_name_str);
        return nullptr;
    } catch (const std::exception& e) {
        return su::alloc_copy(TRACEMSG(e.what() + "\nException raised"));
    }
}
