/* 
 * File:   misc.hpp
 * Author: alex
 *
 * Created on September 22, 2017, 7:14 PM
 */

#ifndef WILTON_SUPPORT_MISC_HPP
#define WILTON_SUPPORT_MISC_HPP

#include "staticlib/support.hpp"
#include "staticlib/json.hpp"

#include "wilton/wilton.h"

#include "wilton/support/exception.hpp"

namespace wilton {
namespace support {

const std::string file_proto_prefix = "file://";
const std::string zip_proto_prefix = "zip://";
const std::string binmod_postfix = ".wlib";

inline void check_json_callback_script(const sl::json::field& field) {
    if (sl::json::type::object != field.json_type()) {
        throw support::exception(TRACEMSG("Invalid '" + field.name() + "' field,"
                " type: [" + sl::json::stringify_json_type(field.json_type()) + "]," +
                " value: [" + field.val().dumps() + "]"));
    }
    bool module = false;
    for (const sl::json::field& fi : field.as_object()) {
        auto& name = fi.name();
        if ("module" == name) {
            if (sl::json::type::string != fi.json_type()) {
                throw support::exception(TRACEMSG("Invalid '" + fi.name() + "' field,"
                        " type: [" + sl::json::stringify_json_type(fi.json_type()) + "]," +
                        " value: [" + fi.val().dumps() + "]"));
            }
            module = true;
        } else if ("func" == name) {
            if (sl::json::type::string != fi.json_type()) {
                throw support::exception(TRACEMSG("Invalid '" + fi.name() + "' field,"
                        " type: [" + sl::json::stringify_json_type(fi.json_type()) + "]," +
                        " value: [" + fi.val().dumps() + "]"));
            }
        } else if ("args" == name) {
            if (sl::json::type::array != fi.json_type()) {
                throw support::exception(TRACEMSG("Invalid '" + fi.name() + "' field,"
                        " type: [" + sl::json::stringify_json_type(fi.json_type()) + "]," +
                        " value: [" + fi.val().dumps() + "]"));
            }
        } else if ("engine" == name) {
            if (sl::json::type::string != fi.json_type()) {
                throw support::exception(TRACEMSG("Invalid '" + fi.name() + "' field,"
                        " type: [" + sl::json::stringify_json_type(fi.json_type()) + "]," +
                        " value: [" + fi.val().dumps() + "]"));
            }
        } else {
            throw support::exception(TRACEMSG(
                    "Unknown data field: [" + name + "] in object: [" + field.name() + "]"));
        }
    }
    if (!module) {
        throw support::exception(TRACEMSG(
                "Required field: 'module' is not supplied in object: [" + field.name() + "]"));
    }
}

} // namespace
}


#endif /* WILTON_SUPPORT_MISC_HPP */

