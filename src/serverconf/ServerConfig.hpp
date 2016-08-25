/* 
 * File:   ServerConfigConfig.hpp
 * Author: alex
 *
 * Created on May 5, 2016, 7:20 PM
 */

#ifndef WILTON_SERVERCONF_SERVERCONFIG_HPP
#define	WILTON_SERVERCONF_SERVERCONFIG_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "staticlib/ranges.hpp"
#include "staticlib/serialization.hpp"

#include "common/WiltonInternalException.hpp"
#include "common/utils.hpp"
#include "logging/AppenderConfig.hpp"
#include "logging/LoggingConfig.hpp"
#include "serverconf/DocumentRoot.hpp"
#include "serverconf/RequestPayloadConfig.hpp"
#include "serverconf/SslConfig.hpp"

namespace wilton {
namespace serverconf {

class ServerConfig {
public:    
    uint32_t numberOfThreads = 2;
    uint16_t tcpPort = 8080;
    std::string ipAddress = "0.0.0.0";
    SslConfig ssl;
    std::vector<DocumentRoot> documentRoots;
    RequestPayloadConfig requestPayload;
    logging::LoggingConfig logging;

    ServerConfig(const ServerConfig&) = delete;

    ServerConfig& operator=(const ServerConfig&) = delete;

    ServerConfig(ServerConfig&& other) :
    numberOfThreads(other.numberOfThreads),
    tcpPort(other.tcpPort),
    ipAddress(std::move(other.ipAddress)),
    ssl(std::move(other.ssl)),
    documentRoots(std::move(other.documentRoots)),
    requestPayload(std::move(other.requestPayload)),
    logging(std::move(other.logging)) { }

    ServerConfig& operator=(ServerConfig&& other) {
        this->numberOfThreads = other.numberOfThreads;
        this->tcpPort = other.tcpPort;
        this->ipAddress = std::move(other.ipAddress);
        this->ssl = std::move(other.ssl);
        this->documentRoots = std::move(other.documentRoots);
        this->logging = std::move(other.logging);
        this->requestPayload = std::move(other.requestPayload);
        return *this;
    }

    ServerConfig(const staticlib::serialization::JsonValue& json) {
        namespace ss = staticlib::serialization;
        for (const ss::JsonField& fi : json.as_object()) {
            auto& name = fi.name();
            if ("numberOfThreads" == name) {
                this->numberOfThreads = common::get_json_uint16(fi, "numberOfThreads");
            } else if ("tcpPort" == name) {
                this->tcpPort = common::get_json_uint16(fi, "tcpPort");
            } else if ("ipAddress" == name) {
                this->ipAddress = common::get_json_string(fi, "ipAddress");
            } else if ("ssl" == name) {
                this->ssl = SslConfig(fi.value());
            } else if ("documentRoots" == name) {
                for (const ss::JsonValue& lo : common::get_json_array(fi, "documentRoots")) {
                    auto jd = serverconf::DocumentRoot(lo);
                    this->documentRoots.emplace_back(std::move(jd));
                }
            } else if ("requestPayload" == name) {
                this->requestPayload = serverconf::RequestPayloadConfig(fi.value());
            } else if ("logging" == name) {
                this->logging = logging::LoggingConfig(fi.value());
            } else {
                throw common::WiltonInternalException(TRACEMSG("Unknown field: [" + name + "]"));
            }
        }
        if (0 == logging.appenders.size()) {
            logging.appenders.emplace_back(logging::AppenderConfig());
        }
    }
    
    staticlib::serialization::JsonValue to_json() const {
        namespace sr = staticlib::ranges;
        auto drs = sr::transform(sr::refwrap(documentRoots), [](const serverconf::DocumentRoot& el) {
            return el.to_json();
        });
        return {
            {"numberOfThreads", numberOfThreads},
            {"tcpPort", tcpPort},
            {"ipAddress", ipAddress},
            {"ssl", ssl.to_json()},
            {"documentRoots", drs},
            {"requestPayload", logging.to_json()},
            {"logging", logging.to_json()}
        };
    }
};

} // namespace
}

#endif	/* WILTON_SERVERCONF_SERVERCONFIG_HPP */

