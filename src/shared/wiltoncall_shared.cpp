/* 
 * File:   wiltoncall_shared.cpp
 * Author: alex
 *
 * Created on January 10, 2017, 5:04 PM
 */

#include "call/wiltoncall_internal.hpp"

#include "wilton/wilton.h"

namespace wilton {
namespace shared {

std::string shared_put(const std::string& data) {
    // json parse
    sl::json::value json = sl::json::loads(data);
    auto rkey = std::ref(sl::utils::empty_string());
    auto rvalue = std::ref(sl::utils::empty_string());
    for (const sl::json::field& fi : json.as_object()) {
        auto& name = fi.name();
        if ("key" == name) {
            rkey = fi.as_string_nonempty_or_throw(name);
        } else if ("value" == name) {
            rvalue = fi.as_string_nonempty_or_throw(name);
        } else {
            throw common::wilton_internal_exception(TRACEMSG("Unknown data field: [" + name + "]"));
        }
    }
    if (rkey.get().empty()) throw common::wilton_internal_exception(TRACEMSG(
            "Required parameter 'key' not specified"));
    const std::string& key = rkey.get();
    if (rvalue.get().empty()) throw common::wilton_internal_exception(TRACEMSG(
            "Required parameter 'value' not specified"));
    const std::string& value = rvalue.get();
    // call wilton
    char* out;
    int out_len;
    char* err = wilton_shared_put(key.c_str(), static_cast<int>(key.length()),
            value.c_str(), static_cast<int>(value.length()),
            std::addressof(out), std::addressof(out_len));
    if (nullptr != err) {
        common::throw_wilton_error(err, TRACEMSG(err));
    }
    if (nullptr != out) {
        return common::wrap_wilton_output(out, out_len);
    }
    return "";
}

std::string shared_get(const std::string& data) {
    // json parse
    sl::json::value json = sl::json::loads(data);
    auto rkey = std::ref(sl::utils::empty_string());
    for (const sl::json::field& fi : json.as_object()) {
        auto& name = fi.name();
        if ("key" == name) {
            rkey = fi.as_string_nonempty_or_throw(name);
        } else {
            throw common::wilton_internal_exception(TRACEMSG("Unknown data field: [" + name + "]"));
        }
    }
    if (rkey.get().empty()) throw common::wilton_internal_exception(TRACEMSG(
            "Required parameter 'key' not specified"));
    const std::string& key = rkey.get();
    // call wilton
    char* out;
    int out_len;
    char* err = wilton_shared_get(key.c_str(), static_cast<int>(key.length()),
            std::addressof(out), std::addressof(out_len));
    if (nullptr != err) {
        common::throw_wilton_error(err, TRACEMSG(err));
    }
    if (nullptr != out) {
        return common::wrap_wilton_output(out, out_len);
    }
    return "";
}

std::string shared_wait_change(const std::string& data) {
    // json parse
    sl::json::value json = sl::json::loads(data);
    int64_t timeout_millis = -1;
    auto rkey = std::ref(sl::utils::empty_string());
    auto rcvalue = std::ref(sl::utils::empty_string());
    for (const sl::json::field& fi : json.as_object()) {
        auto& name = fi.name();
        if ("timeoutMillis" == name) {
            timeout_millis = fi.as_int64_or_throw(name);
        } else if ("key" == name) {
            rkey = fi.as_string_nonempty_or_throw(name);
        } else if ("currentValue" == name) {
            rcvalue = fi.as_string_nonempty_or_throw(name);
        } else {
            throw common::wilton_internal_exception(TRACEMSG("Unknown data field: [" + name + "]"));
        }
    }
    if (-1 == timeout_millis) throw common::wilton_internal_exception(TRACEMSG(
            "Required parameter 'timeoutMillis' not specified"));
    if (rkey.get().empty()) throw common::wilton_internal_exception(TRACEMSG(
            "Required parameter 'key' not specified"));
    const std::string& key = rkey.get();
    if (rcvalue.get().empty()) throw common::wilton_internal_exception(TRACEMSG(
            "Required parameter 'currentValue' not specified"));
    const std::string& cvalue = rcvalue.get();
    // call wilton
    char* out;
    int out_len;
    char* err = wilton_shared_wait_change(static_cast<int> (timeout_millis),
            key.c_str(), static_cast<int>(key.length()),
            cvalue.c_str(), static_cast<int>(cvalue.length()),
            std::addressof(out), std::addressof(out_len));
    if (nullptr != err) {
        common::throw_wilton_error(err, TRACEMSG(err));
    }
    if (nullptr != out) {
        return common::wrap_wilton_output(out, out_len);
    }
    return "";
}

std::string shared_remove(const std::string& data) {
    // json parse
    sl::json::value json = sl::json::loads(data);
    auto rkey = std::ref(sl::utils::empty_string());
    for (const sl::json::field& fi : json.as_object()) {
        auto& name = fi.name();
        if ("key" == name) {
            rkey = fi.as_string_nonempty_or_throw(name);
        } else {
            throw common::wilton_internal_exception(TRACEMSG("Unknown data field: [" + name + "]"));
        }
    }
    if (rkey.get().empty()) throw common::wilton_internal_exception(TRACEMSG(
            "Required parameter 'key' not specified"));
    const std::string& key = rkey.get();
    // call wilton
    char* out;
    int out_len;
    char* err = wilton_shared_remove(key.c_str(), static_cast<int>(key.length()),
            std::addressof(out), std::addressof(out_len));
    if (nullptr != err) {
        common::throw_wilton_error(err, TRACEMSG(err));
    }
    if (nullptr != out) {
        return common::wrap_wilton_output(out, out_len);
    }
    return "";
}

} // namespace
}
