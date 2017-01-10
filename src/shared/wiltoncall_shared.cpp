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

namespace { //anonymous

namespace ss = staticlib::serialization;

} // namespace

std::string shared_put(const std::string& data) {
    // json parse
    ss::JsonValue json = ss::load_json_from_string(data);
    auto rkey = std::ref(common::empty_string());
    auto rvalue = std::ref(common::empty_string());
    for (const ss::JsonField& fi : json.as_object()) {
        auto& name = fi.name();
        if ("key" == name) {
            rkey = common::get_json_string(fi);
        } else if ("value" == name) {
            rvalue = common::get_json_string(fi);
        } else {
            throw common::WiltonInternalException(TRACEMSG("Unknown data field: [" + name + "]"));
        }
    }
    if (rkey.get().empty()) throw common::WiltonInternalException(TRACEMSG(
            "Required parameter 'key' not specified"));
    const std::string& key = rkey.get();
    if (rvalue.get().empty()) throw common::WiltonInternalException(TRACEMSG(
            "Required parameter 'value' not specified"));
    const std::string& value = rvalue.get();
    // call wilton
    char* err = wilton_shared_put(key.c_str(), key.length(), value.c_str(), value.length());
    if (nullptr != err) {
        common::throw_wilton_error(err, TRACEMSG(err));
    }
    return "{}";

}

std::string shared_get(const std::string& data) {
    // json parse
    ss::JsonValue json = ss::load_json_from_string(data);
    auto rkey = std::ref(common::empty_string());
    for (const ss::JsonField& fi : json.as_object()) {
        auto& name = fi.name();
        if ("key" == name) {
            rkey = common::get_json_string(fi);
        } else {
            throw common::WiltonInternalException(TRACEMSG("Unknown data field: [" + name + "]"));
        }
    }
    if (rkey.get().empty()) throw common::WiltonInternalException(TRACEMSG(
            "Required parameter 'key' not specified"));
    const std::string& key = rkey.get();
    // call wilton
    char* out;
    int out_len;
    char* err = wilton_shared_get(key.c_str(), key.length(),
            std::addressof(out), std::addressof(out_len));
    if (nullptr != err) {
        common::throw_wilton_error(err, TRACEMSG(err));
    }
    if (nullptr == out) {
        return ""; // invalid json, should be checked by caller
    }
    return common::wrap_wilton_output(out, out_len);
}

std::string shared_wait_change(const std::string& data) {
    // json parse
    ss::JsonValue json = ss::load_json_from_string(data);
    int64_t timeout_millis = -1;
    auto rkey = std::ref(common::empty_string());
    auto rcvalue = std::ref(common::empty_string());
    for (const ss::JsonField& fi : json.as_object()) {
        auto& name = fi.name();
        if ("timeoutMillis" == name) {
            timeout_millis = common::get_json_int64(fi);
        } else if ("key" == name) {
            rkey = common::get_json_string(fi);
        } else if ("currentValue" == name) {
            rcvalue = common::get_json_string(fi);
        } else {
            throw common::WiltonInternalException(TRACEMSG("Unknown data field: [" + name + "]"));
        }
    }
    if (-1 == timeout_millis) throw common::WiltonInternalException(TRACEMSG(
            "Required parameter 'timeoutMillis' not specified"));
    if (rkey.get().empty()) throw common::WiltonInternalException(TRACEMSG(
            "Required parameter 'key' not specified"));
    const std::string& key = rkey.get();
    if (rcvalue.get().empty()) throw common::WiltonInternalException(TRACEMSG(
            "Required parameter 'currentValue' not specified"));
    const std::string& cvalue = rcvalue.get();
    // call wilton
    char* out;
    int out_len;
    char* err = wilton_shared_wait_change(static_cast<int> (timeout_millis),
            key.c_str(), key.length(), cvalue.c_str(), cvalue.length(),
            std::addressof(out), std::addressof(out_len));
    if (nullptr != err) {
        common::throw_wilton_error(err, TRACEMSG(err));
    }
    if (nullptr == out) {
        return ""; // invalid json, should be checked by caller
    }
    return common::wrap_wilton_output(out, out_len);
}

std::string shared_remove(const std::string& data) {
    // json parse
    ss::JsonValue json = ss::load_json_from_string(data);
    auto rkey = std::ref(common::empty_string());
    for (const ss::JsonField& fi : json.as_object()) {
        auto& name = fi.name();
        if ("key" == name) {
            rkey = common::get_json_string(fi);
        } else {
            throw common::WiltonInternalException(TRACEMSG("Unknown data field: [" + name + "]"));
        }
    }
    if (rkey.get().empty()) throw common::WiltonInternalException(TRACEMSG(
            "Required parameter 'key' not specified"));
    const std::string& key = rkey.get();
    // call wilton
    char* err = wilton_shared_remove(key.c_str(), key.length());
    if (nullptr != err) {
        common::throw_wilton_error(err, TRACEMSG(err));
    }
    return "{}";
}

} // namespace
}
