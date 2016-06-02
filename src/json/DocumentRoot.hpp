/* 
 * File:   DocumentRoot.hpp
 * Author: alex
 *
 * Created on May 5, 2016, 7:27 PM
 */

#ifndef WILTON_C_JSON_DOCUMENTROOT_HPP
#define	WILTON_C_JSON_DOCUMENTROOT_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "asio.hpp"

#include "staticlib/config.hpp"
#include "staticlib/ranges.hpp"
#include "staticlib/serialization.hpp"

#include "WiltonInternalException.hpp"
#include "MimeType.hpp"

namespace wilton {
namespace json {

namespace { // anonymous

namespace sr = staticlib::ranges;
namespace ss = staticlib::serialization;

std::vector<MimeType> mimes_copy(const std::vector<MimeType>& vec) {
    auto copied = sr::transform(sr::refwrap(vec), [](const MimeType& el) {
        return el.clone();
    });
    return sr::emplace_to_vector(std::move(copied));
}

std::vector<MimeType> default_mimes() {
    std::vector<MimeType> res{};
    res.emplace_back("txt",  "text/plain");
    res.emplace_back("js",   "text/javascript");
    res.emplace_back("css",  "text/css");
    res.emplace_back("html", "text/html");
    res.emplace_back("png",  "image/png");
    res.emplace_back("jpg",  "image/jpeg");
    res.emplace_back("svg",  "image/svg+xml");
    return res;
}

} // namepspace

class DocumentRoot {
public:
    std::string resource = "";
    std::string dirPath = "";
    std::string zipPath = "";
    std::string zipInnerPrefix = "";
    uint32_t cacheMaxAgeSeconds = 604800;
    std::vector<MimeType> mimeTypes = default_mimes();
    
    DocumentRoot(const DocumentRoot&) = delete;
    
    DocumentRoot& operator=(const DocumentRoot&) = delete;

    DocumentRoot(DocumentRoot&& other) :
    resource(std::move(other.resource)),
    dirPath(std::move(other.dirPath)),
    zipPath(std::move(other.zipPath)),
    zipInnerPrefix(std::move(other.zipInnerPrefix)),
    cacheMaxAgeSeconds(other.cacheMaxAgeSeconds),
    mimeTypes(std::move(other.mimeTypes)) { }

    DocumentRoot& operator=(DocumentRoot&& other) {
        this->resource = std::move(other.resource);
        this->dirPath = std::move(other.dirPath);
        this->zipPath = std::move(other.zipPath);
        this->zipInnerPrefix = std::move(other.zipInnerPrefix);
        this->cacheMaxAgeSeconds = other.cacheMaxAgeSeconds;
        this->mimeTypes = std::move(other.mimeTypes);
        return *this;
    }
    
    DocumentRoot() { }
    
    DocumentRoot(const std::string& resource, const std::string& dirPath, 
            const std::string& zipPath, const std::string& zipInnerPrefix,
            uint32_t cacheMaxAgeSeconds, const std::vector<MimeType>& mimeTypes) :
    resource(resource.data(), resource.length()), 
    dirPath(dirPath.data(), dirPath.length()), 
    zipPath(zipPath.data(), zipPath.length()), 
    zipInnerPrefix(zipInnerPrefix.data(), zipInnerPrefix.length()), 
    cacheMaxAgeSeconds(cacheMaxAgeSeconds), 
    mimeTypes(mimes_copy(mimeTypes)) { }
    
    DocumentRoot(const ss::JsonValue& json) {
        for (const ss::JsonField& fi : json.get_object()) {
            auto& name = fi.get_name();
            if ("resource" == name) {
                if (0 == fi.get_string().length()) throw WiltonInternalException(TRACEMSG(std::string() +
                        "Invalid 'documentRoot.resource' field: [" + ss::dump_json_to_string(fi.get_value()) + "]"));
                this->resource = fi.get_string();
            } else if ("dirPath" == name) {
                if (0 == fi.get_string().length()) throw WiltonInternalException(TRACEMSG(std::string() +
                        "Invalid 'documentRoot.dirPath' field: [" + ss::dump_json_to_string(fi.get_value()) + "]"));
                this->dirPath = fi.get_string();
            } else if ("zipPath" == name) {
                if (0 == fi.get_string().length()) throw WiltonInternalException(TRACEMSG(std::string() +
                        "Invalid 'documentRoot.zipPath' field: [" + ss::dump_json_to_string(fi.get_value()) + "]"));
                this->zipPath = fi.get_string();
            } else if ("zipInnerPrefix" == name) {
                if (0 == fi.get_string().length()) throw WiltonInternalException(TRACEMSG(std::string() +
                        "Invalid 'documentRoot.zipInnerPrefix' field: [" + ss::dump_json_to_string(fi.get_value()) + "]"));
                this->zipInnerPrefix = fi.get_string();                
            } else if ("cacheMaxAgeSeconds" == name) {
                if (ss::JsonType::INTEGER != fi.get_type() ||
                        fi.get_int32() < 0 ||
                        fi.get_integer() > std::numeric_limits<uint32_t>::max()) {
                    throw WiltonInternalException(TRACEMSG(std::string() +
                            "Invalid 'documentRoot.cacheMaxAgeSeconds' field: [" + ss::dump_json_to_string(fi.get_value()) + "]"));
                }
                this->cacheMaxAgeSeconds = fi.get_uint32();
            } else if ("mimeTypes" == name) {
                if (ss::JsonType::ARRAY != fi.get_type() || 0 == fi.get_array().size()) throw WiltonInternalException(TRACEMSG(std::string() +
                        "Invalid 'documentRoot.mimeTypes' field: [" + ss::dump_json_to_string(fi.get_value()) + "]"));
                for (const ss::JsonValue& ap : fi.get_array()) {
                    auto ja = json::MimeType(ap);
                    mimeTypes.emplace_back(std::move(ja));
                }
            } else {
                throw WiltonInternalException(TRACEMSG(std::string() +
                        "Unknown 'documentRoot' field: [" + name + "]"));
            }
        }
        if (0 == resource.length()) throw WiltonInternalException(TRACEMSG(std::string() +
                    "Invalid 'documentRoot.resource' field: []"));
        if (0 == dirPath.length() && 0 == zipPath.length()) throw WiltonInternalException(TRACEMSG(std::string() +
                    "Invalid 'documentRoot.dirPath' and 'documentRoot.zipPath' fields: [], []"));
    }
       
    ss::JsonValue to_json() const {
        auto mimes = sr::transform(sr::refwrap(mimeTypes), [](const MimeType& el) {
            return el.to_json();
        });
        return {
            {"resource", resource},
            {"dirPath", dirPath},
            {"zipPath", zipPath},
            {"zipInnerPrefix", zipInnerPrefix},
            {"cacheMaxAgeSeconds", cacheMaxAgeSeconds},
            {"mimeTypes", mimes}
        };
    }
    
    bool is_empty() {
        return 0 == resource.length();
    }
    
    DocumentRoot clone() const {
        return DocumentRoot(resource, dirPath, zipPath, zipInnerPrefix, cacheMaxAgeSeconds, mimeTypes);
    }

};

} // namespace
}

#endif	/* WILTON_C_JSON_DOCUMENTROOT_HPP */

