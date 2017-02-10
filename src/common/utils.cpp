/* 
 * File:   utils.cpp
 * Author: alex
 * 
 * Created on June 14, 2016, 11:45 AM
 */

#include "common/utils.hpp"

#include "staticlib/io.hpp"
#include "staticlib/tinydir.hpp"
#include "staticlib/utils.hpp"

#include "wilton/wilton.h"

#include "common/wilton_internal_exception.hpp"

namespace wilton {
namespace common {

namespace { // anonymous

namespace sc = staticlib::config;
namespace si = staticlib::io;
namespace ss = staticlib::serialization;
namespace st = staticlib::tinydir;
namespace su = staticlib::utils;

} //namespace

void throw_wilton_error(char* err, const std::string& msg) {
    wilton_free(err);
    throw wilton_internal_exception(msg);
}

std::string wrap_wilton_output(char* out, int out_len) {
    std::string res{out, static_cast<std::string::size_type> (out_len)};
    wilton_free(out);
    return res;
}

void check_json_callback_script(const staticlib::serialization::json_field& field) {
    if (ss::json_type::object != field.type()) {
        throw common::wilton_internal_exception(TRACEMSG("Invalid '" + field.name() + "' field,"
                " type: [" + ss::stringify_json_type(field.type()) + "]," +
                " value: [" + ss::dump_json_to_string(field.value()) + "]"));
    }
    bool module = false;
    bool func = false;
    bool args = false;
    for (const ss::json_field& fi : field.as_object()) {
        auto& name = fi.name();
        if ("module" == name) {
            if (ss::json_type::string != fi.type()) {
                throw common::wilton_internal_exception(TRACEMSG("Invalid '" + fi.name() + "' field,"
                        " type: [" + ss::stringify_json_type(fi.type()) + "]," +
                        " value: [" + ss::dump_json_to_string(fi.value()) + "]"));
            }
            module = true;
        } else if ("func" == name) {
            if (ss::json_type::string != fi.type()) {
                throw common::wilton_internal_exception(TRACEMSG("Invalid '" + fi.name() + "' field,"
                        " type: [" + ss::stringify_json_type(fi.type()) + "]," +
                        " value: [" + ss::dump_json_to_string(fi.value()) + "]"));
            }
            func = true;
        } else if ("args" == name) {
            if (ss::json_type::array != fi.type()) {
                throw common::wilton_internal_exception(TRACEMSG("Invalid '" + fi.name() + "' field,"
                        " type: [" + ss::stringify_json_type(fi.type()) + "]," +
                        " value: [" + ss::dump_json_to_string(fi.value()) + "]"));
            }
            args = true;
        } else {
            throw common::wilton_internal_exception(TRACEMSG(
                    "Unknown data field: [" + name + "] in object: [" + field.name() + "]"));
        }
    }
    if (!module) {
        throw common::wilton_internal_exception(TRACEMSG(
                "Required field: 'module' is not supplied in object: [" + field.name() + "]"));
    }
    if (!func) {
        throw common::wilton_internal_exception(TRACEMSG(
                "Required field: 'func' is not supplied in object: [" + field.name() + "]"));
    }
    if (!args) {
        throw common::wilton_internal_exception(TRACEMSG(
                "Required field: 'func' is not supplied in object: [" + field.name() + "]"));
    }
}

void dump_error(const std::string& directory, const std::string& msg) {
    try {
        // random postfix
        std::string id = su::random_string_generator().generate(12);
        auto errfile = directory + "wilton_ERROR_" + id + ".txt";
        auto fd = st::file_sink(errfile);
        si::write_all(fd, msg);
    } catch (...) {
        // give up
    }
}

} //namespace
}  

