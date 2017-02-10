/* 
 * File:   wiltoncall_mustache.cpp
 * Author: alex
 *
 * Created on January 10, 2017, 5:53 PM
 */

#include "call/wiltoncall_internal.hpp"

#include "wilton/wilton.h"

namespace wilton {
namespace mustache {

namespace { //anonymous

namespace su = staticlib::utils;
namespace ss = staticlib::serialization;

} // namespace

std::string mustache_render(const std::string& data) {
    // parse json
    ss::json_value json = ss::load_json_from_string(data);
    auto rtemplate = std::ref(su::empty_string());
    std::string values = su::empty_string();
    for (const ss::json_field& fi : json.as_object()) {
        auto& name = fi.name();
        if ("template" == name) {
            rtemplate = fi.as_string_nonempty_or_throw(name);
        } else if ("values" == name) {
            values = ss::dump_json_to_string(fi.value());
        } else {
            throw common::wilton_internal_exception(TRACEMSG("Unknown data field: [" + name + "]"));
        }
    }
    if (rtemplate.get().empty()) throw common::wilton_internal_exception(TRACEMSG(
            "Required parameter 'template' not specified"));
    const std::string& templade = rtemplate.get();
    if (values.empty()) {
        values = "{}";
    }
    // call wilton
    char* out;
    int out_len;
    char* err = wilton_render_mustache(templade.c_str(), templade.length(),
            values.c_str(), values.length(), std::addressof(out), std::addressof(out_len));
    if (nullptr != err) common::throw_wilton_error(err, TRACEMSG(err));
    return common::wrap_wilton_output(out, out_len);
}

std::string mustache_render_file(const std::string& data) {
    // parse json
    ss::json_value json = ss::load_json_from_string(data);
    auto rfile = std::ref(su::empty_string());
    std::string values = su::empty_string();
    for (const ss::json_field& fi : json.as_object()) {
        auto& name = fi.name();
        if ("file" == name) {
            rfile = fi.as_string_nonempty_or_throw(name);
        } else if ("values" == name) {
            values = ss::dump_json_to_string(fi.value());
        } else {
            throw common::wilton_internal_exception(TRACEMSG("Unknown data field: [" + name + "]"));
        }
    }
    if (rfile.get().empty()) throw common::wilton_internal_exception(TRACEMSG(
            "Required parameter 'file' not specified"));
    const std::string& file = rfile.get();
    if (values.empty()) {
        values = "{}";
    }
    // call wilton
    char* out;
    int out_len;
    char* err = wilton_render_mustache_file(file.c_str(), file.length(),
            values.c_str(), values.length(), std::addressof(out), std::addressof(out_len));
    if (nullptr != err) common::throw_wilton_error(err, TRACEMSG(err));
    return common::wrap_wilton_output(out, out_len);
}

} // namespace
}
