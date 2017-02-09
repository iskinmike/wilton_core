/* 
 * File:   wilton_client.cpp
 * Author: alex
 * 
 * Created on June 13, 2016, 4:23 PM
 */

#include "wilton/wilton.h"

#include <array>
#include <string>

#include "staticlib/config.hpp"
#include "staticlib/httpclient.hpp"
#include "staticlib/io.hpp"
#include "staticlib/serialization.hpp"
#include "staticlib/utils.hpp"
#include "staticlib/tinydir.hpp"

#include "client/ClientResponse.hpp"
#include "client/ClientRequestConfig.hpp"
#include "client/ClientSessionConfig.hpp"

namespace { // anonymous

namespace sc = staticlib::config;
namespace sh = staticlib::httpclient;
namespace si = staticlib::io;
namespace ss = staticlib::serialization;
namespace st = staticlib::tinydir;
namespace su = staticlib::utils;
namespace wc = wilton::client;
    
} //namespace

struct wilton_HttpClient {
private:
    sh::http_session delegate;

public:
    wilton_HttpClient(sh::http_session&& delegate) :
    delegate(std::move(delegate)) { }

    sh::http_session& impl() {
        return delegate;
    }
};

char* wilton_HttpClient_create(
        wilton_HttpClient** http_out,
        const char* conf_json,
        int conf_json_len) {
    if (nullptr == http_out) return su::alloc_copy(TRACEMSG("Null 'http_out' parameter specified"));
    if (nullptr == conf_json) return su::alloc_copy(TRACEMSG("Null 'conf_json' parameter specified"));
    if (!sc::is_uint32_positive(conf_json_len)) return su::alloc_copy(TRACEMSG(
            "Invalid 'conf_json_len' parameter specified: [" + sc::to_string(conf_json_len) + "]"));
    try {
        uint32_t conf_json_len_u32 = static_cast<uint32_t> (conf_json_len);
        std::string json_str{conf_json, conf_json_len_u32};
        ss::json_value json = ss::load_json_from_string(json_str);
        wc::ClientSessionConfig conf{std::move(json)};
        sh::http_session session{std::move(conf.options)};
        wilton_HttpClient* http_ptr = new wilton_HttpClient(std::move(session));
        *http_out = http_ptr;
        return nullptr;
    } catch (const std::exception& e) {
        return su::alloc_copy(TRACEMSG(e.what() + "\nException raised"));
    }
}

char* wilton_HttpClient_close(
        wilton_HttpClient* http) {
    if (nullptr == http) return su::alloc_copy(TRACEMSG("Null 'http' parameter specified"));
    try {
        delete http;
        return nullptr;
    } catch (const std::exception& e) {
        return su::alloc_copy(TRACEMSG(e.what() + "\nException raised"));
    }
}

// todo: json copying
// todo: data copying
char* wilton_HttpClient_execute(
        wilton_HttpClient* http,
        const char* url,
        int url_len,
        const char* request_data,
        int request_data_len,
        const char* request_metadata_json,
        int request_metadata_len,
        char** response_data_out,
        int* response_data_len_out) {
    if (nullptr == http) return su::alloc_copy(TRACEMSG("Null 'http' parameter specified"));    
    if (nullptr == url) return su::alloc_copy(TRACEMSG("Null 'url' parameter specified"));
    if (!sc::is_uint32_positive(url_len)) return su::alloc_copy(TRACEMSG(
            "Invalid 'url_len' parameter specified: [" + sc::to_string(url_len) + "]"));
    if (!sc::is_uint32(request_data_len)) return su::alloc_copy(TRACEMSG(
            "Invalid 'request_data_len' parameter specified: [" + sc::to_string(request_data_len) + "]"));
    if (!sc::is_uint32(request_metadata_len)) return su::alloc_copy(TRACEMSG(
            "Invalid 'request_metadata_len' parameter specified: [" + sc::to_string(request_metadata_len) + "]"));
    try {
        std::string url_str{url, static_cast<uint32_t> (url_len)};
        ss::json_value opts_json{};
        if (request_metadata_len > 0) {
            std::string meta_str{request_metadata_json, static_cast<uint32_t> (request_metadata_len)};
            opts_json = ss::load_json_from_string(meta_str);
        }
        wc::ClientRequestConfig opts{std::move(opts_json)};
        std::array<char, 4096> buf;
        si::string_sink sink{};
        ss::json_value resp_json{};
        if (request_data_len > 0) {
            std::string data_str{request_data, static_cast<uint32_t> (request_data_len)};
            si::string_source data_src{std::move(data_str)};
            // POST will be used by default for this API call
            sh::http_resource resp = http->impl().open_url(url_str, std::move(data_src), opts.options);            
            si::copy_all(resp, sink, buf);
            resp_json = wc::ClientResponse::to_json(std::move(sink.get_string()), resp.get_info());
        } else {
            // GET will be used by default for this API call
            sh::http_resource resp = http->impl().open_url(url_str, opts.options);
            si::copy_all(resp, sink, buf);
            resp_json = wc::ClientResponse::to_json(std::move(sink.get_string()), resp.get_info());
        }
        std::string resp_complete = ss::dump_json_to_string(resp_json);
        *response_data_out = su::alloc_copy(resp_complete);
        *response_data_len_out = resp_complete.length();
        return nullptr;
    } catch (const std::exception& e) {
        return su::alloc_copy(TRACEMSG(e.what() + "\nException raised"));
    }    
}

char* wilton_HttpClient_send_file(
        wilton_HttpClient* http,
        const char* url,
        int url_len,
        const char* file_path,
        int file_path_len,
        const char* request_metadata_json,
        int request_metadata_len,
        char** response_data_out,
        int* response_data_len_out,
        void* finalizer_ctx,
        void (*finalizer_cb)(
                void* finalizer_ctx,
                int sent_successfully)) {
    if (nullptr == http) return su::alloc_copy(TRACEMSG("Null 'http' parameter specified"));
    if (nullptr == url) return su::alloc_copy(TRACEMSG("Null 'url' parameter specified"));
    if (!sc::is_uint32_positive(url_len)) return su::alloc_copy(TRACEMSG(
            "Invalid 'url_len' parameter specified: [" + sc::to_string(url_len) + "]"));
    if (nullptr == file_path) return su::alloc_copy(TRACEMSG("Null 'file_path' parameter specified"));
    if (!sc::is_uint16_positive(file_path_len)) return su::alloc_copy(TRACEMSG(
            "Invalid 'file_path_len' parameter specified: [" + sc::to_string(file_path_len) + "]"));
    if (!sc::is_uint32(request_metadata_len)) return su::alloc_copy(TRACEMSG(
            "Invalid 'request_metadata_len' parameter specified: [" + sc::to_string(request_metadata_len) + "]"));
    try {
        std::string url_str{url, static_cast<uint32_t> (url_len)};
        ss::json_value opts_json{};
        if (request_metadata_len > 0) {
            std::string meta_str{request_metadata_json, static_cast<uint32_t> (request_metadata_len)};
            opts_json = ss::load_json_from_string(meta_str);
        }
        wc::ClientRequestConfig opts{std::move(opts_json)};
        std::array<char, 4096> buf;
        std::string file_path_str{file_path, static_cast<uint32_t> (file_path_len)};
        auto fd = st::file_source(file_path_str);
        sh::http_resource resp = http->impl().open_url(url_str, std::move(fd), opts.options);
        si::string_sink sink{};
        si::copy_all(resp, sink, buf);
        ss::json_value resp_json = wc::ClientResponse::to_json(std::move(sink.get_string()), resp.get_info());
        std::string resp_complete = ss::dump_json_to_string(resp_json);
        if (nullptr != finalizer_cb) {
            finalizer_cb(finalizer_ctx, 1);
        }
        *response_data_out = su::alloc_copy(resp_complete);
        *response_data_len_out = resp_complete.length();
        return nullptr;
    } catch (const std::exception& e) {
        if (nullptr != finalizer_cb) {
            finalizer_cb(finalizer_ctx, 0);
        }
        return su::alloc_copy(TRACEMSG(e.what() + "\nException raised"));
    }   
}
