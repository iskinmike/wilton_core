/* 
 * File:   Logger.hpp
 * Author: alex
 *
 * Created on May 10, 2016, 8:01 PM
 */

#ifndef WILTON_C_JSON_LOGGER_HPP
#define	WILTON_C_JSON_LOGGER_HPP

#include <string>

#include "staticlib/config.hpp"
#include "staticlib/serialization.hpp"

#include "WiltonInternalException.hpp"

namespace wilton {
namespace c {
namespace json {

namespace { // anonymous

namespace ss = staticlib::serialization;

} // namespace

class Logger {
public:
    std::string name;
    std::string level;

    Logger(const Logger&) = delete;

    Logger& operator=(const Logger&) = delete;

    Logger(Logger&& other) :
    name(std::move(other.name)),
    level(std::move(other.level)) { }

    Logger& operator=(Logger&& other) {
        this->name = std::move(other.name);
        this->level = std::move(other.level);
        return *this;
    }

    Logger() { }

    Logger(const ss::JsonValue& json) {
        for (const ss::JsonField& fi : json.get_object()) {
            auto& fname = fi.get_name();
            if ("name" == fname) {
                if (0 == fi.get_string().length()) throw WiltonInternalException(TRACEMSG(std::string() +
                        "Invalid 'logging.loggers.name' field: [" + ss::dump_json_to_string(fi.get_value()) + "]"));
                this->name = fi.get_string();
            } else if ("level" == fname) {
                if (0 == fi.get_string().length()) throw WiltonInternalException(TRACEMSG(std::string() +
                        "Invalid 'logging.loggers.level' field: [" + ss::dump_json_to_string(fi.get_value()) + "]"));
                this->level = fi.get_string();
            } else {
                throw WiltonInternalException(TRACEMSG(std::string() +
                        "Unknown 'logging.loggers' field: [" + ss::dump_json_to_string(fi.get_value()) + "]"));
            }
        }
        if (0 == name.length()) throw WiltonInternalException(TRACEMSG(std::string() +
                "Invalid 'logging.loggers.name' field: []"));
        if (0 == level.length()) throw WiltonInternalException(TRACEMSG(std::string() +
                "Invalid 'logging.loggers.level' field: []"));
    }

    ss::JsonValue to_json() {
        return {
            {"name", name},
            {"level", level}
        };
    }
};

} // namepspace
}
}

#endif	/* WILTON_C_JSON_LOGGER_HPP */

