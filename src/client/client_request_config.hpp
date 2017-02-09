/* 
 * File:   client_request_config.hpp
 * Author: alex
 *
 * Created on June 13, 2016, 4:24 PM
 */

#ifndef WILTON_CLIENT_CLIENT_REQUEST_CONFIG_HPP
#define	WILTON_CLIENT_CLIENT_REQUEST_CONFIG_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "staticlib/httpclient.hpp"
#include "staticlib/ranges.hpp"
#include "staticlib/serialization.hpp"

#include "common/wilton_internal_exception.hpp"
#include "common/utils.hpp"

namespace wilton {
namespace client {

class client_request_config {
public:
    staticlib::httpclient::http_request_options options;

    client_request_config(const client_request_config&) = delete;

    client_request_config& operator=(const client_request_config&) = delete;

    client_request_config(client_request_config&& other) :
    options(std::move(other.options)) { }

    client_request_config& operator=(client_request_config&& other) {
        this->options = std::move(other.options);
        return *this;
    }

    client_request_config() { }

    client_request_config(const staticlib::serialization::json_value& json) {
        namespace ss = staticlib::serialization;
        for (const ss::json_field& fi : json.as_object()) {
            auto& name = fi.name();
            if ("headers" == name) {
                for (const ss::json_field& hf : common::get_json_object(fi)) {
                    std::string val = common::get_json_string(hf);
                    options.headers.emplace_back(hf.name(), std::move(val));
                }
            } else if ("method" == name) {
                options.method = common::get_json_string(fi);
            } else if ("abortOnConnectError" == name) {
                options.abort_on_connect_error = common::get_json_bool(fi);
            } else if ("abortOnResponseError" == name) {
                options.abort_on_response_error = common::get_json_bool(fi);
            } else if ("readTimeoutMillis" == name) {
                options.read_timeout_millis = common::get_json_uint32(fi);
            } else if ("fdsetTimeoutMillis" == name) {
                options.fdset_timeout_millis = common::get_json_uint32(fi);
            } else if ("forceHttp10" == name) {
                options.force_http_10 = common::get_json_bool(fi);
            } else if ("noprogress" == name) {
                options.noprogress = common::get_json_bool(fi);
            } else if ("nosignal" == name) {
                options.nosignal = common::get_json_bool(fi);
            } else if ("failonerror" == name) {
                options.failonerror = common::get_json_bool(fi);
            } else if ("pathAsIs" == name) {
                options.path_as_is = common::get_json_bool(fi);
            } else if ("tcpNodelay" == name) {
                options.tcp_nodelay = common::get_json_bool(fi);
            } else if ("tcpKeepalive" == name) {
                options.tcp_keepalive = common::get_json_bool(fi);
            } else if ("tcpKeepidleSecs" == name) {
                options.tcp_keepidle_secs = common::get_json_uint32(fi);
            } else if ("tcpKeepintvlSecs" == name) {
                options.tcp_keepintvl_secs = common::get_json_uint32(fi);
            } else if ("connecttimeoutMillis" == name) {
                options.connecttimeout_millis = common::get_json_uint32(fi);
            } else if ("buffersizeBytes" == name) {
                options.buffersize_bytes = common::get_json_uint32(fi);
            } else if ("acceptEncoding" == name) {
                options.accept_encoding = common::get_json_string(fi);
            } else if ("followlocation" == name) {
                options.followlocation = common::get_json_bool(fi);
            } else if ("maxredirs" == name) {
                options.maxredirs = common::get_json_uint32(fi);
            } else if ("useragent" == name) {
                options.useragent = common::get_json_string(fi);
            } else if ("maxSentSpeedLargeBytesPerSecond" == name) {
                options.max_sent_speed_large_bytes_per_second = common::get_json_uint32(fi);
            } else if ("maxRecvSpeedLargeBytesPerSecond" == name) {
                options.max_recv_speed_large_bytes_per_second = common::get_json_uint32(fi);
            } else if ("sslcertFilename" == name) {
                options.sslcert_filename = common::get_json_string(fi);
            } else if ("sslcertype" == name) {
                options.sslcertype = common::get_json_string(fi);
            } else if ("sslkeyFilename" == name) {
                options.sslkey_filename = common::get_json_string(fi);
            } else if ("sslKeyType" == name) {
                options.ssl_key_type = common::get_json_string(fi);
            } else if ("sslKeypasswd" == name) {
                options.ssl_keypasswd = common::get_json_string(fi);
            } else if ("requireTls" == name) {
                options.require_tls = common::get_json_bool(fi);
            } else if ("sslVerifyhost" == name) {
                options.ssl_verifyhost = common::get_json_bool(fi);
            } else if ("sslVerifypeer" == name) {
                options.ssl_verifypeer = common::get_json_bool(fi);
            } else if ("sslVerifystatus" == name) {
                options.ssl_verifystatus = common::get_json_bool(fi);
            } else if ("cainfoFilename" == name) {
                options.cainfo_filename = common::get_json_string(fi);
            } else if ("crlfileFilename" == name) {
                options.crlfile_filename = common::get_json_string(fi);
            } else if ("sslCipherList" == name) {
                options.ssl_cipher_list = common::get_json_string(fi);
            } else {
                throw common::wilton_internal_exception(TRACEMSG("Unknown 'ClientRequest' field: [" + name + "]"));
            }
        }
    }

    staticlib::serialization::json_value to_json() const {
        namespace sr = staticlib::ranges;
        namespace ss = staticlib::serialization;
        auto ha = sr::transform(sr::refwrap(options.headers), [](const std::pair<std::string, std::string>& el) {
            return ss::json_field{el.first, el.second};
        });
        std::vector<ss::json_field> hfields = sr::emplace_to_vector(std::move(ha));
        return {
            {"headers", std::move(hfields)},
            {"method", options.method},
            {"abortOnConnectError", options.abort_on_connect_error},
            {"abortOnResponseError", options.abort_on_response_error},
            {"readTimeoutMillis", options.read_timeout_millis},
            {"fdsetTimeoutMillis", options.fdset_timeout_millis},
            {"forceHttp10", options.force_http_10},
            {"noprogress", options.noprogress},
            {"nosignal", options.nosignal},
            {"failonerror", options.failonerror},
            {"pathAsIs", options.path_as_is},
            {"tcpNodelay", options.tcp_nodelay},
            {"tcpKeepalive", options.tcp_keepalive},
            {"tcpKeepidleSecs", options.tcp_keepidle_secs},
            {"tcpKeepintvlSecs", options.tcp_keepintvl_secs},
            {"connecttimeoutMillis", options.connecttimeout_millis},
            {"buffersizeBytes", options.buffersize_bytes},
            {"acceptEncoding", options.accept_encoding},
            {"followlocation", options.followlocation},
            {"maxredirs", options.maxredirs},
            {"useragent", options.useragent},
            {"maxSentSpeedLargeBytesPerSecond", options.max_sent_speed_large_bytes_per_second},
            {"maxRecvSpeedLargeBytesPerSecond", options.max_recv_speed_large_bytes_per_second},
            {"sslcertFilename", options.sslcert_filename},
            {"sslcertype", options.sslcertype},
            {"sslkeyFilename", options.sslkey_filename},
            {"sslKeyType", options.ssl_key_type},
            {"sslKeypasswd", options.ssl_keypasswd},
            {"requireTls", options.require_tls},
            {"sslVerifyhost", options.ssl_verifyhost},
            {"sslVerifypeer", options.ssl_verifypeer},
            {"sslVerifystatus", options.ssl_verifystatus},
            {"cainfoFilename", options.cainfo_filename},
            {"crlfileFilename", options.crlfile_filename},
            {"sslCipherList", options.ssl_cipher_list}
        };
    }

};

} // namespace
} 

#endif	/* WILTON_CLIENT_CLIENT_REQUEST_CONFIG_HPP */

