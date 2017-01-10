/* 
 * File:   wiltoncall_logging.cpp
 * Author: alex
 *
 * Created on January 10, 2017, 6:01 PM
 */

#include "call/wiltoncall_internal.hpp"

#include "wilton/wilton.h"

namespace wilton {
namespace logging {

namespace { //anonymous

namespace ss = staticlib::serialization;

} // namespace

std::string logging_initialize(const std::string& data) {
    char* err = wilton_logger_initialize(data.c_str(), data.length());
    if (nullptr != err) common::throw_wilton_error(err, TRACEMSG(std::string(err)));
    return "{}";
}

std::string logging_log(const std::string& data) {
    // json parse
    ss::JsonValue json = ss::load_json_from_string(data);
    auto rlevel = std::ref(common::empty_string());
    auto rlogger = std::ref(common::empty_string());
    auto rmessage = std::ref(common::empty_string());
    for (const ss::JsonField& fi : json.as_object()) {
        auto& name = fi.name();
        if ("level" == name) {
            rlevel = common::get_json_string(fi);
        } else if ("logger" == name) {
            rlogger = common::get_json_string(fi);
        } else if ("message" == name) {
            rmessage = fi.as_string();
        } else {
            throw common::WiltonInternalException(TRACEMSG("Unknown data field: [" + name + "]"));
        }
    }
    if (rlevel.get().empty()) throw common::WiltonInternalException(TRACEMSG(
            "Required parameter 'level' not specified"));
    if (rlogger.get().empty()) throw common::WiltonInternalException(TRACEMSG(
            "Required parameter 'logger' not specified"));
    const std::string& level = rlevel.get();
    const std::string& logger = rlogger.get();
    const std::string& message = rmessage.get();
    // call wilton
    char* err = wilton_logger_log(level.c_str(), level.length(), logger.c_str(), logger.length(),
            message.c_str(), message.length());
    if (nullptr != err) common::throw_wilton_error(err, TRACEMSG(std::string(err)));
    return "{}";
}

std::string logging_is_level_enabled(const std::string& data) {
    // parse json
    ss::JsonValue json = ss::load_json_from_string(data);
    auto rlevel = std::ref(common::empty_string());
    auto rlogger = std::ref(common::empty_string());
    for (const ss::JsonField& fi : json.as_object()) {
        auto& name = fi.name();
        if ("level" == name) {
            rlevel = common::get_json_string(fi);
        } else if ("logger" == name) {
            rlogger = common::get_json_string(fi);
        } else {
            throw common::WiltonInternalException(TRACEMSG("Unknown data field: [" + name + "]"));
        }
    }
    if (rlevel.get().empty()) throw common::WiltonInternalException(TRACEMSG(
            "Required parameter 'level' not specified"));
    if (rlogger.get().empty()) throw common::WiltonInternalException(TRACEMSG(
            "Required parameter 'logger' not specified"));
    const std::string& level = rlevel.get();
    const std::string& logger = rlogger.get();
    // call wilton
    int out;
    char* err = wilton_logger_is_level_enabled(logger.c_str(), logger.length(),
            level.c_str(), level.length(), std::addressof(out));
    if (nullptr != err) common::throw_wilton_error(err, TRACEMSG(std::string(err)));
    return ss::dump_json_to_string({
        { "enabled", out != 0}
    });
}

std::string logging_shutdown(const std::string&) {
    // call wilton
    int out;
    char* err = wilton_logger_shutdown();
    if (nullptr != err) common::throw_wilton_error(err, TRACEMSG(std::string(err)));
    return "{}";
}

} // namespace
}
