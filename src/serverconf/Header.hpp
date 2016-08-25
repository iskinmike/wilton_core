/* 
 * File:   Header.hpp
 * Author: alex
 *
 * Created on May 5, 2016, 9:00 PM
 */

#ifndef WILTON_SERVERCONF_HEADER_HPP
#define	WILTON_SERVERCONF_HEADER_HPP

#include <string>

#include "staticlib/config.hpp"
#include "staticlib/serialization.hpp"

#include "common/WiltonInternalException.hpp"
#include "common/utils.hpp"

namespace wilton {
namespace serverconf {

class Header {
public:
    std::string name = "";
    std::string value = "";

    Header(const Header&) = delete;

    Header& operator=(const Header&) = delete;

    Header(Header&& other) :
    name(std::move(other.name)),
    value(std::move(other.value)) { }

    Header& operator=(Header&& other) {
        this->name = std::move(other.name);
        this->value = std::move(other.value);
        return *this;
    }

    Header() { }
    
    Header(std::string name, std::string value) :
    name(std::move(name)),
    value(std::move(value)) { }

    Header(const staticlib::serialization::JsonValue& json) {
        namespace ss = staticlib::serialization;
        for (const ss::JsonField& fi : json.as_object()) {
            auto& fname = fi.name();
            if ("name" == fname) {
                this->name = common::get_json_string(fi, "header.name");
            } else if ("value" == fname) {
                this->value = common::get_json_string(fi, "header.value");
            } else {
                throw common::WiltonInternalException(TRACEMSG("Unknown 'header' field: [" + fname + "]"));
            }
        }
        if (0 == name.length()) throw common::WiltonInternalException(TRACEMSG(
                "Invalid 'header.name' field: []"));
        if (0 == value.length()) throw common::WiltonInternalException(TRACEMSG(
                "Invalid 'header.value' field: []"));
    }

    staticlib::serialization::JsonField to_json() const {
        namespace ss = staticlib::serialization;
        return ss::JsonField{name, ss::JsonValue{value}};
    }
};


} // namepspace
}

#endif	/* WILTON_SERVERCONF_HEADER_HPP */

