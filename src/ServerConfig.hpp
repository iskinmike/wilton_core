/* 
 * File:   ServerConfig.hpp
 * Author: alex
 *
 * Created on May 5, 2016, 7:20 PM
 */

#ifndef WILTON_C_SERVERCONFIG_HPP
#define	WILTON_C_SERVERCONFIG_HPP

#include <cstdint>
#include <string>

#include "asio.h"

#include "staticlib/serialization.hpp"

#include "DocumentRoot.hpp"

namespace wilton {
namespace c {

namespace { // anonymous

namespace ss = staticlib::serialization;

}

class ServerConfig {
public:    
    uint32_t numberOfThreads = 2;
    uint16_t tcpPort = 8080;
    asio::ip::address_v4 ipAddress = asio::ip::address_v4::any();
    DocumentRoot documentRoot;

    ServerConfig(const ServerConfig&) = delete;

    ServerConfig& operator=(const ServerConfig&) = delete;

    ServerConfig(ServerConfig&& other) :
    numberOfThreads(other.numberOfThreads),
    tcpPort(other.tcpPort),
    ipAddress(std::move(other.ipAddress)),
    documentRoot(std::move(other.documentRoot)) { }

    ServerConfig& operator=(const ServerConfig& other) {
        this->numberOfThreads = other.numberOfThreads;
        this->tcpPort = other.tcpPort;
        this->ipAddress = std::move(other.ipAddress);
        this->documentRoot = std::move(other.documentRoot);
        return *this;
    }

    ServerConfig(const ss::JsonValue& json) {
        for (const auto& fi : val.get_object()) {
            auto name = fi.get_name();
            if ("numberOfThreads" == name) {
                if (ss::JsonType::INTEGER != fi.get_type() ||
                        fi.get_int32() < 0 ||
                        fi.get_uint32() > std::numeric_limits<uint16_t>::max()) {
                    throw WiltonInternalException(TRACEMSG(std::string() +
                            "Invalid 'numberOfThreads' field: [" + ss::dump_json_to_string(fi) + "]"));
                }
                this->numberOfThreads = fi.get_uint16();
            } else if ("tcpPort" == name) {
                if (ss::JsonType::INTEGER != fi.get_type() ||
                        fi.get_int32() < 0 ||
                        fi.get_uint64() > std::numeric_limits<uint32_t>::max()) {
                    throw WiltonInternalException(TRACEMSG(std::string() +
                            "Invalid 'tcpPort' field: [" + ss::dump_json_to_string(fi) + "]"));
                }
                this->tcpPort = fi.get_uint32();
            } else if ("ipAddress" == name) {
                this->ipAddress = asio::ip::address_v4::from_string(fi.get_string());
            } else if ("documentRoot" == name) {
                this->documentRoot = DocumentRoot(fi.get_value());
            } else {
                throw WiltonInternalException(TRACEMSG(std::string() +
                        "Unknown field: [" + ss::dump_json_to_string(fi) + "]"));
            }
        }
    }
    
    ss::JsonValue to_json() {
        return {
            {"numberOfThreads", numberOfThreads},
            {"tcpPort", tcpPort},
            {"ipAddress", ipAddress.to_string()},
            {"documentRoot", documentRoot.to_json()}
        }
    }
};

} // namespace
}

#endif	/* WILTON_C_SERVERCONFIG_HPP */

