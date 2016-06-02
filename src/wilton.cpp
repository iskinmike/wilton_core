/* 
 * File:   wilton.cpp
 * Author: alex
 *
 * Created on April 30, 2016, 11:50 PM
 */

#include "wilton/wilton.h"

#include "staticlib/config.hpp"
#include "staticlib/serialization.hpp"
#include "staticlib/utils.hpp"

#include "logging/WiltonLogger.hpp"
#include "server/Request.hpp"
#include "server/Server.hpp"

#include "json/ResponseMetadata.hpp"

namespace { // anonymous

namespace sc = staticlib::config;
namespace su = staticlib::utils;
namespace ss = staticlib::serialization;

}

struct wilton_Server {
private:
    wilton::server::Server delegate;

public:
    wilton_Server(wilton::server::Server&& delegate) :
    delegate(std::move(delegate)) { }

    wilton::server::Server& impl() {
        return delegate;
    }
};

struct wilton_Request {
private:
    wilton::server::Request& delegate;

public:
    wilton_Request(wilton::server::Request& delegate) :
    delegate(delegate) { }

    wilton::server::Request& impl() {
        return delegate;
    }
};

void wilton_free(char* errmsg) {
    std::free(errmsg);
}

// todo: fixme message copy
char* wilton_log(
        const char* level_name,
        int level_name_len,
        const char* logger_name,
        int logger_name_len,
        const char* message,
        int message_len) {
    if (nullptr == level_name) return su::alloc_copy(TRACEMSG(std::string() +
            "Null 'level_name' parameter specified"));
    if (level_name_len <= 0 ||
            static_cast<int64_t> (level_name_len) > std::numeric_limits<uint32_t>::max()) return su::alloc_copy(TRACEMSG(std::string() +
            "Invalid 'level_name_len' parameter specified: [" + sc::to_string(level_name_len) + "]"));
    if (nullptr == logger_name) return su::alloc_copy(TRACEMSG(std::string() +
            "Null 'logger_name' parameter specified"));
    if (logger_name_len <= 0 ||
            static_cast<int64_t> (logger_name_len) > std::numeric_limits<uint32_t>::max()) return su::alloc_copy(TRACEMSG(std::string() +
            "Invalid 'logger_name_len' parameter specified: [" + sc::to_string(logger_name_len) + "]"));
    if (nullptr == message) return su::alloc_copy(TRACEMSG(std::string() +
            "Null 'message' parameter specified"));
    if (message_len <= 0 ||
            static_cast<int64_t> (message_len) > std::numeric_limits<uint32_t>::max()) return su::alloc_copy(TRACEMSG(std::string() +
            "Invalid 'message_len' parameter specified: [" + sc::to_string(message_len) + "]"));
    try {
        uint32_t level_name_len_u32 = static_cast<uint32_t> (level_name_len);
        std::string level_name_str{level_name, level_name_len_u32};
        uint32_t logger_name_len_u32 = static_cast<uint32_t> (logger_name_len);
        std::string logger_name_str{logger_name, logger_name_len_u32};
        uint32_t message_len_u32 = static_cast<uint32_t> (message_len);
        std::string message_str{message, message_len_u32};
        wilton::logging::WiltonLogger::log(level_name_str, logger_name_str, message_str);
        return nullptr;
    } catch (const std::exception& e) {
        return su::alloc_copy(TRACEMSG(std::string() + e.what() + "\nException raised"));
    }
}

// TODO: fixme json copy
char* wilton_Server_create(
        wilton_Server** server_out,
        void* gateway_ctx,
        void (*gateway_cb)(
                void* gateway_ctx,
                wilton_Request* request),
        const char* conf_json,
        int conf_json_len) /* noexcept */ {
    if (nullptr == server_out) return su::alloc_copy(TRACEMSG(std::string() +
            "Null 'server_out' parameter specified"));
    if (nullptr == gateway_cb) return su::alloc_copy(TRACEMSG(std::string() +
            "Null 'gateway_cb' parameter specified"));
    if (nullptr == conf_json) return su::alloc_copy(TRACEMSG(std::string() +
            "Null 'conf_json' parameter specified"));
    if (conf_json_len <= 0 ||
            static_cast<int64_t>(conf_json_len) > std::numeric_limits<uint32_t>::max()) return su::alloc_copy(TRACEMSG(std::string() +
            "Invalid 'conf_json_len' parameter specified: [" + sc::to_string(conf_json_len) + "]"));
    try {
        uint32_t conf_json_len_u32 = static_cast<uint32_t> (conf_json_len);
        std::string metadata{conf_json, conf_json_len_u32};
        ss::JsonValue json = ss::load_json_from_string(metadata);
        wilton::server::Server server{
            [gateway_ctx, gateway_cb](wilton::server::Request& req) {
                wilton_Request* req_ptr = new wilton_Request(req);
                gateway_cb(gateway_ctx, req_ptr);
                // todo: special handling for chunked send
                delete req_ptr;
            },
            std::move(json)
        };
        wilton_Server* server_ptr = new wilton_Server(std::move(server));
        *server_out = server_ptr;
        return nullptr;
    } catch (const std::exception& e) {
        return su::alloc_copy(TRACEMSG(std::string() + e.what() + "\nException raised"));
    }    
}

char* wilton_Server_stop_server(wilton_Server* server) /* noexcept */ {
    if (nullptr == server) return su::alloc_copy(TRACEMSG(std::string() +
            "Null 'server' parameter specified"));
    try {
        server->impl().stop();
        delete server;
        return nullptr;
    } catch (const std::exception& e) {
        return su::alloc_copy(TRACEMSG(std::string() + e.what() + "\nException raised"));
    }
}

// TODO: fixme json copy
char* wilton_Request_get_request_metadata(wilton_Request* request, char** metadata_json_out,
        int* metadata_json_len_out) /* noexcept */ {
    if (nullptr == request) return su::alloc_copy(TRACEMSG(std::string() +
            "Null 'server' parameter specified"));
    if (nullptr == metadata_json_out) return su::alloc_copy(TRACEMSG(std::string() +
            "Null 'metadata_json_out' parameter specified"));
    if (nullptr == metadata_json_len_out) return su::alloc_copy(TRACEMSG(std::string() +
            "Null 'metadata_json_len_out' parameter specified"));
    try {
        auto meta = request->impl().get_request_metadata();
        ss::JsonValue json = meta.to_json();
        std::string res = ss::dump_json_to_string(json);
        *metadata_json_out = su::alloc_copy(res);
        *metadata_json_len_out = res.size();
        return nullptr;
    } catch (const std::exception& e) {
        return su::alloc_copy(TRACEMSG(std::string() + e.what() + "\nException raised"));
    }    
}

// TODO: think about copy
char* wilton_Request_get_request_data(wilton_Request* request, char** data_out,
        int* data_len_out) /* noexcept */ {
    if (nullptr == request) return su::alloc_copy(TRACEMSG(std::string() +
            "Null 'request' parameter specified"));
    if (nullptr == data_out) return su::alloc_copy(TRACEMSG(std::string() +
            "Null 'data_out' parameter specified"));
    if (nullptr == data_len_out) return su::alloc_copy(TRACEMSG(std::string() +
            "Null 'data_len_out' parameter specified"));
    try {
        const std::string& res = request->impl().get_request_data();
        *data_out = su::alloc_copy(res);
        *data_len_out = res.size();
        return nullptr;
    } catch (const std::exception& e) {
        return su::alloc_copy(TRACEMSG(std::string() + e.what() + "\nException raised"));
    }
}

// TODO: fixme json copy
char* wilton_Request_set_response_metadata(wilton_Request* request,
        const char* metadata_json, int metadata_json_len) /* noexcept */ {
    if (nullptr == request) return su::alloc_copy(TRACEMSG(std::string() +
            "Null 'request' parameter specified"));
    if (nullptr == metadata_json) return su::alloc_copy(TRACEMSG(std::string() +
            "Null 'metadata_json' parameter specified"));
    if (metadata_json_len <= 0 ||
            static_cast<uint64_t>(metadata_json_len) > std::numeric_limits<uint32_t>::max()) return su::alloc_copy(TRACEMSG(std::string() +
            "Invalid 'metadata_json_len' parameter specified: [" + sc::to_string(metadata_json_len) + "]"));
    try {
        uint32_t metadata_json_len_u32 = static_cast<uint32_t> (metadata_json_len);
        std::string metadata{metadata_json, metadata_json_len_u32};
        ss::JsonValue json = ss::load_json_from_string(metadata);
        wilton::json::ResponseMetadata rm{json};
        request->impl().set_response_metadata(std::move(rm));
        return nullptr;
    } catch (const std::exception& e) {
        return su::alloc_copy(TRACEMSG(std::string() + e.what() + "\nException raised"));
    }
}

char* wilton_Request_send_response(wilton_Request* request, const char* data,
        int data_len) /* noexcept */ {
    if (nullptr == request) return su::alloc_copy(TRACEMSG(std::string() +
            "Null 'request' parameter specified"));
    if (nullptr == data) return su::alloc_copy(TRACEMSG(std::string() +
            "Null 'data' parameter specified"));
    if (data_len < 0 ||
            static_cast<uint64_t>(data_len) > std::numeric_limits<uint32_t>::max()) return su::alloc_copy(TRACEMSG(std::string() +
            "Invalid 'data_len' parameter specified: [" + sc::to_string(data_len) + "]"));
    try {
        uint32_t data_len_u32 = static_cast<uint32_t> (data_len);
        request->impl().send_response(data, data_len_u32);
        return nullptr;
    } catch (const std::exception& e) {
        return su::alloc_copy(TRACEMSG(std::string() + e.what() + "\nException raised"));
    }
}

char* wilton_Request_send_file(
        wilton_Request* request,
        const char* file_path,
        int file_path_len,
        void* finalizer_ctx,
        void (*finalizer_cb)(
                void* finalizer_ctx,
                int sent_successfully)) /* noexcept */ {
    if (nullptr == request) return su::alloc_copy(TRACEMSG(std::string() +
            "Null 'request' parameter specified"));
    if (nullptr == file_path) return su::alloc_copy(TRACEMSG(std::string() +
            "Null 'file_path' parameter specified"));
    if (file_path_len <= 0 ||
            static_cast<uint32_t> (file_path_len) > std::numeric_limits<uint16_t>::max()) return su::alloc_copy(TRACEMSG(std::string() +
            "Invalid 'file_path_len' parameter specified: [" + sc::to_string(file_path_len) + "]"));
    if (nullptr == finalizer_cb) return su::alloc_copy(TRACEMSG(std::string() +
            "Null 'finalizer_cb' parameter specified"));
    try {
        uint16_t file_path_len_u16 = static_cast<uint16_t> (file_path_len);
        std::string file_path_str{file_path, file_path_len_u16};
        request->impl().send_file(std::move(file_path_str), 
                [finalizer_ctx, finalizer_cb](bool success) {
                    int success_int = success ? 1 : 0;
                    finalizer_cb(finalizer_ctx, success_int);
                });
        return nullptr;
    } catch (const std::exception& e) {
        return su::alloc_copy(TRACEMSG(std::string() + e.what() + "\nException raised"));
    }
}

char* wilton_Request_send_mustache(
        wilton_Request* request,
        const char* mustache_file_path,
        int mustache_file_path_len,
        const char* values_json,
        int values_json_len) /* noexcept */ {
    if (nullptr == request) return su::alloc_copy(TRACEMSG(std::string() +
            "Null 'request' parameter specified"));
    if (nullptr == mustache_file_path) return su::alloc_copy(TRACEMSG(std::string() +
            "Null 'mustache_file_path' parameter specified"));
    if (mustache_file_path_len <= 0 ||
            static_cast<uint32_t> (mustache_file_path_len) > std::numeric_limits<uint16_t>::max()) return su::alloc_copy(TRACEMSG(std::string() +
            "Invalid 'mustache_file_path_len' parameter specified: [" + sc::to_string(mustache_file_path_len) + "]"));
    if (nullptr == values_json) return su::alloc_copy(TRACEMSG(std::string() +
            "Null 'values_json' parameter specified"));
    if (values_json_len <= 0 ||
            static_cast<uint64_t> (values_json_len) > std::numeric_limits<uint32_t>::max()) return su::alloc_copy(TRACEMSG(std::string() +
            "Invalid 'values_json_len' parameter specified: [" + sc::to_string(values_json_len) + "]"));
    try {
        uint16_t mustache_file_path_len_u16 = static_cast<uint16_t> (mustache_file_path_len);
        std::string mustache_file_path_str{mustache_file_path, mustache_file_path_len_u16};
        uint32_t values_json_len_u32 = static_cast<uint32_t> (values_json_len);
        std::string values_json_str{values_json, values_json_len_u32};
        ss::JsonValue json = ss::load_json_from_string(values_json_str);
        request->impl().send_mustache(std::move(mustache_file_path_str), std::move(json));
        return nullptr;
    } catch (const std::exception& e) {
        return su::alloc_copy(TRACEMSG(std::string() + e.what() + "\nException raised"));
    }    
}
