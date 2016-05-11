/* 
 * File:   FileHandler.hpp
 * Author: alex
 *
 * Created on May 11, 2016, 12:50 PM
 */

#ifndef WILTON_C_FILEHANDLER_HPP
#define	WILTON_C_FILEHANDLER_HPP

#include <cstdint>
#include <string>

#include "staticlib/config.hpp"
#include "staticlib/io.hpp"
#include "staticlib/httpserver.hpp"
#include "staticlib/utils.hpp"

#include "ResponseStreamSender.hpp"
#include "WiltonInternalException.hpp"

#include "json/DocumentRoot.hpp"

namespace wilton {
namespace c {

namespace { // anonymous

namespace sc = staticlib::config;
namespace si = staticlib::io;
namespace sh = staticlib::httpserver;
namespace su = staticlib::utils;

} //namespace

class FileHandler {
    json::DocumentRoot conf;
    
public:
    // todo: path leading slash check
    FileHandler(const json::DocumentRoot& conf) :
    conf(conf.clone()) {
        if (0 == conf.dirPath.length()) throw WiltonInternalException(TRACEMSG(std::string() + 
                "Invalid empty 'dirPath' specified"));
    }
    
    // todo: error messages format
    void operator()(sh::http_request_ptr& req, sh::tcp_connection_ptr& conn) {
        auto finfun = std::bind(&sh::tcp_connection::finish, conn);
        auto resp = sh::http_response_writer::create(conn, *req, finfun);
        std::string url_path = std::string{req->get_resource(), conf.resource.length()};
        std::string file_path = std::string{conf.dirPath} + "/" + url_path;
        if (file_path.find("..") != std::string::npos) {
            resp->get_response().set_status_code(sh::http_request::RESPONSE_CODE_BAD_REQUEST);
            resp->get_response().set_status_message(sh::http_request::RESPONSE_MESSAGE_BAD_REQUEST);
            resp << sh::http_request::RESPONSE_CODE_BAD_REQUEST << " "
                    << sh::http_request::RESPONSE_MESSAGE_BAD_REQUEST << ":"
                    << " [" << url_path << "]\n";
            resp->send();
        } else {
            try {
                su::FileDescriptor fd{file_path, 'r'};
                auto fd_ptr = si::make_source_istream_ptr(std::move(fd));
                auto sender = std::make_shared<ResponseStreamSender>(resp, std::move(fd_ptr));
                set_resp_headers(resp->get_response());
                sender->send();
            } catch (const std::exception& e) {
                resp->get_response().set_status_code(sh::http_request::RESPONSE_CODE_NOT_FOUND);
                resp->get_response().set_status_message(sh::http_request::RESPONSE_MESSAGE_NOT_FOUND);
                resp << sh::http_request::RESPONSE_CODE_NOT_FOUND << " "
                        << sh::http_request::RESPONSE_MESSAGE_NOT_FOUND << ":"
                        << " [" << url_path << "]\n";
                resp->send();
            }
        }
    }
    
private:
    void set_resp_headers(const std::string& url_path, sh::http_response& resp) {
        std::string ct{"application/octet-stream"};
        for(const auto& mi : conf.mimeTypes) {
            if (su::ends_with(url_path, mi.extension)) {
                ct = mi.mime;
                break;
            }
        }
        resp.change_header("Content-Type", ct);
        // set caching
        resp.change_header("Cache-Control", "max-age=" + sc::to_string(conf.cacheMaxAgeSeconds) + ", public");
    }
    
};


} // namespace
}

#endif	/* WILTON_C_FILEHANDLER_HPP */

