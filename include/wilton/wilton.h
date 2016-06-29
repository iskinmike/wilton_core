/* 
 * File:   wilton.h
 * Author: alex
 *
 * Created on April 30, 2016, 11:49 PM
 */

#ifndef WILTON_H
#define	WILTON_H

#ifdef WILTON_SHARED
#ifdef _WIN32
#ifdef WILTON_SHARED_IMPORT
#define WILTON_EXPORT __declspec(dllimport)
#else
#define WILTON_EXPORT __declspec(dllexport)
#endif // WILTON_SHARED_IMPORT
#else
#ifdef WILTON_SHARED_IMPORT
#define WILTON_EXPORT
#else
#define WILTON_EXPORT __attribute__ ((visibility ("default")))
#endif // WILTON_SHARED_IMPORT
#endif // _WIN32
#else
#define WILTON_EXPORT
#endif // WILTON_SHARED

#ifdef	__cplusplus
extern "C" {
#endif
    
// common

WILTON_EXPORT void wilton_free(
        char* errmsg);


// server

struct wilton_Server;
typedef struct wilton_Server wilton_Server;

struct wilton_Request;
typedef struct wilton_Request wilton_Request;

struct wilton_ResponseWriter;
typedef struct wilton_ResponseWriter wilton_ResponseWriter;

/*
    {
        "numberOfThreads": uint32_t, 
        "tcpPort": uint16_t,
        "ipAddress": "x.x.x.x",
        "ssl": {
            "keyFile": "path/to/file",
            "keyPassword": "pwd",
            "verifyFile": "path/to/file",
            "verifySubjectSubstr": "CN=some_name",
        },
        "documentRoots": [{
            "resource": "/path/to/hanldler",
            "dirPath": "path/to/directory",
            "zipPath": "path/to/directory.zip",
            "zipInnerPrefix": "some/dir",
            "cacheMaxAgeSeconds": uint32_t,
            "mimeTypes": [{
                "extension": ".css",
                "mime": "text/css"
            }, ...]
        }, ...],
        "logging": {
            "appenders": [{
                "appenderType": "NULL | CONSOLE | FILE | DAILY_ROLLING_FILE",
                "filePath": "path/to/log/file",
                "layout": "%d{%Y-%m-%d %H:%M:%S,%q} [%-5p %-5.5t %-20.20c] %m%n",
                "thresholdLevel": "TRACE | DEBUG | INFO | WARN | ERROR | FATAL"
            }, ... ],
            "loggers": [{
                "name": "my.logger.name",
                "level": "TRACE | DEBUG | INFO | WARN | ERROR | FATAL"
            }, ...]
        },
        "natProxyPaths": ["/path/to/proxy1", "/path/to/proxy2"]
    }
 */
WILTON_EXPORT char* wilton_Server_create(
        wilton_Server** server_out,
        void* gateway_ctx,
        void (*gateway_cb)(
                void* gateway_ctx,
                wilton_Request* request),
        const char* conf_json,
        int conf_json_len);

WILTON_EXPORT char* wilton_Server_stop_server(
        wilton_Server* server);

/*
// Duplicates in raw headers are handled in the following ways, depending on the header name:
// Duplicates of age, authorization, content-length, content-type, etag, expires, 
// from, host, if-modified-since, if-unmodified-since, last-modified, location, 
// max-forwards, proxy-authorization, referer, retry-after, or user-agent are discarded.
// For all other headers, the values are joined together with ', '.
{
    "httpVersion": "1.1",
    "protocol": "http|https",
    "method": "GET|POST|PUT|DELETE",
    "pathname": "/path/to/hanldler",
    "query": "param1=val1...",
    "url": "/path/to/hanldler?param1=val1...",
    "headers": {
        "Header-Name": "header_value",
        ...
    }
}
 */
WILTON_EXPORT char* wilton_Request_get_request_metadata(
        wilton_Request* request,
        char** metadata_json_out,
        int* metadata_json_len_out);

WILTON_EXPORT char* wilton_Request_get_request_data(
        wilton_Request* request,
        char** data_out,
        int* data_len_out);

/*
{
    "statusCode": uint16_t,
    "statusMessage": "Status message",
    "headers": {
        "Header-Name": "header_value",
        ...
    }
}
 */
WILTON_EXPORT char* wilton_Request_set_response_metadata(
        wilton_Request* request,
        const char* metadata_json,
        int metadata_json_len);

WILTON_EXPORT char* wilton_Request_send_response(
        wilton_Request* request,
        const char* data,
        int data_len);

WILTON_EXPORT char* wilton_Request_send_file(
        wilton_Request* request,
        const char* file_path,
        int file_path_len,
        void* finalizer_ctx,
        void (*finalizer_cb)(
                void* finalizer_ctx,
                int sent_successfully));

WILTON_EXPORT char* wilton_Request_send_mustache(
        wilton_Request* request,
        const char* mustache_file_path,
        int mustache_file_path_len,
        const char* values_json,
        int values_json_len);

WILTON_EXPORT char* wilton_Request_send_later(
        wilton_Request* request,
        wilton_ResponseWriter** writer_out);

WILTON_EXPORT char* wilton_ResponseWriter_send(
        wilton_ResponseWriter* writer,
        const char* data,
        int data_len);


// logging

WILTON_EXPORT char* wilton_log(
        const char* level_name,
        int level_name_len,
        const char* logger_name,
        int logger_name_len,
        const char* message,
        int message_len);


// mustache

WILTON_EXPORT char* wilton_render_mustache(
        const char* template_text,
        int template_text_len,
        const char* values_json,
        int values_json_len,
        char** output_text_out,
        int* output_text_len_out);


// DB

struct wilton_DBConnection;
typedef struct wilton_DBConnection wilton_DBConnection;

struct wilton_DBTransaction;
typedef struct wilton_DBTransaction wilton_DBTransaction;

WILTON_EXPORT char* wilton_DBConnection_open(
        wilton_DBConnection** conn_out,
        const char* conn_url,
        int conn_url_len);

WILTON_EXPORT char* wilton_DBConnection_query(
        wilton_DBConnection* conn,
        const char* sql_text,
        int sql_text_len,
        const char* params_json,
        int params_json_len,
        char** result_set_out,
        int* result_set_len_out);

WILTON_EXPORT char* wilton_DBConnection_execute(
        wilton_DBConnection* conn,
        const char* sql_text,
        int sql_text_len,
        const char* params_json,
        int params_json_len);

WILTON_EXPORT char* wilton_DBConnection_close(
        wilton_DBConnection* conn);

WILTON_EXPORT char* wilton_DBTransaction_start(
        wilton_DBConnection* conn,
        wilton_DBTransaction** tran_out);

WILTON_EXPORT char* wilton_DBTransaction_commit(
        wilton_DBTransaction* tran);

WILTON_EXPORT char* wilton_DBTransaction_rollback(
        wilton_DBTransaction* tran);

// HttpClient

struct wilton_HttpClient;
typedef struct wilton_HttpClient wilton_HttpClient;

/*
 {
    // https://curl.haxx.se/libcurl/c/CURLMOPT_MAX_HOST_CONNECTIONS.html
    "maxHostConnections": uint32_t,
    // https://curl.haxx.se/libcurl/c/CURLMOPT_MAX_TOTAL_CONNECTIONS.html
    "maxTotalConnections": uint32_t,
    // https://curl.haxx.se/libcurl/c/CURLMOPT_MAXCONNECTS.html
    "maxconnects": uint32_t
 }
 */
WILTON_EXPORT char* wilton_HttpClient_create(
        wilton_HttpClient** http_out,
        const char* conf_json,
        int conf_json_len);

WILTON_EXPORT char* wilton_HttpClient_close(
        wilton_HttpClient* http);

/*

// options implemented manually
 
    "headers": {
        "Header-Name": "header_value",
        ...
    },
    "method": "GET|POST|PUT|DELETE",
    "abortOnResponseError": true,
    "readTimeoutMillis": uint32_t,
    "fdsetTimeoutMillis": uint32_t,

// general behavior options

    // https://curl.haxx.se/libcurl/c/CURLOPT_HTTP_VERSION.html
    "forceHttp10": false,
    // https://curl.haxx.se/libcurl/c/CURLOPT_NOPROGRESS.html
    "noprogress": true,
    // https://curl.haxx.se/libcurl/c/CURLOPT_NOSIGNAL.html
    "nosignal": true,
    // https://curl.haxx.se/libcurl/c/CURLOPT_FAILONERROR.html
    "failonerror": false,
    // https://curl.haxx.se/libcurl/c/CURLOPT_PATH_AS_IS.html
    "pathAsIs": true,

// TCP options

    // https://curl.haxx.se/libcurl/c/CURLOPT_TCP_NODELAY.html
    "tcpNodelay": false,
    // https://curl.haxx.se/libcurl/c/CURLOPT_TCP_KEEPALIVE.html
    "tcpKeepalive": false,
    // https://curl.haxx.se/libcurl/c/CURLOPT_TCP_KEEPIDLE.html
    "tcpKeepidleSecs": uint32_t,
    // https://curl.haxx.se/libcurl/c/CURLOPT_TCP_KEEPINTVL.html
    "tcpKeepintvlSecs": uint32_t,
    // https://curl.haxx.se/libcurl/c/CURLOPT_CONNECTTIMEOUT_MS.html
    "connecttimeoutMillis": uint32_t,

// HTTP options

    // https://curl.haxx.se/libcurl/c/CURLOPT_BUFFERSIZE.html 
    "buffersizeBytes": uint32_t,
    // https://curl.haxx.se/libcurl/c/CURLOPT_ACCEPT_ENCODING.html
    "acceptEncoding": "gzip",
    // https://curl.haxx.se/libcurl/c/CURLOPT_FOLLOWLOCATION.html
    "followlocation": true,
    // https://curl.haxx.se/libcurl/c/CURLOPT_MAXREDIRS.html
    "maxredirs": uint32_t,
    // https://curl.haxx.se/libcurl/c/CURLOPT_USERAGENT.html
    // "Mozilla/5.0 (Linux; U; Android 4.2.2; en-us; GT-I9505 Build/JDQ39) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30"
    useragent = "";

// throttling options

    // https://curl.haxx.se/libcurl/c/CURLOPT_MAX_SEND_SPEED_LARGE.html
    "maxSentSpeedLargeBytesPerSecond": uint32_t,
    // https://curl.haxx.se/libcurl/c/CURLOPT_MAX_RECV_SPEED_LARGE.html
    "maxRecvSpeedLargeBytesPerSecond": uint32_t,

// SSL options

    // https://curl.haxx.se/libcurl/c/CURLOPT_SSLCERT.html
    "sslcertFilename": "",
    // https://curl.haxx.se/libcurl/c/CURLOPT_SSLCERTTYPE.html
    "sslcertype": "",
    // https://curl.haxx.se/libcurl/c/CURLOPT_SSLKEY.html
    "sslkeyFilename": "",
    // https://curl.haxx.se/libcurl/c/CURLOPT_SSLKEYTYPE.html
    "sslKeyType": "",
    // https://curl.haxx.se/libcurl/c/CURLOPT_KEYPASSWD.html
    "sslKeypasswd": "".
    // https://curl.haxx.se/libcurl/c/CURLOPT_SSLVERSION.html
    "requireTls": false.
    // https://curl.haxx.se/libcurl/c/CURLOPT_SSL_VERIFYHOST.html
    "sslVerifyhost": false,
    // https://curl.haxx.se/libcurl/c/CURLOPT_SSL_VERIFYPEER.html
    "sslVerifypeer": false,
    // https://curl.haxx.se/libcurl/c/CURLOPT_SSL_VERIFYSTATUS.html
    "sslVerifystatus": false,
    // https://curl.haxx.se/libcurl/c/CURLOPT_CAINFO.html
    "cainfoFilename": "",
    // https://curl.haxx.se/libcurl/c/CURLOPT_CRLFILE.html
    "crlfileFilename": "",
    // https://curl.haxx.se/libcurl/c/CURLOPT_SSL_CIPHER_LIST.html
    "sslCipherList": ""
 }
 {
    "headers": {
        "Header-Name": "header_value",
        ...
    },
    // https://curl.haxx.se/libcurl/c/CURLINFO_EFFECTIVE_URL.html
    "effectiveUrl": "",
    // https://curl.haxx.se/libcurl/c/CURLINFO_RESPONSE_CODE.html
    "responseCode": long,
    // https://curl.haxx.se/libcurl/c/CURLINFO_TOTAL_TIME.html
    "totalTimeSecs": double,
    // https://curl.haxx.se/libcurl/c/CURLINFO_NAMELOOKUP_TIME.html
    "namelookupTimeSecs": double,
    // https://curl.haxx.se/libcurl/c/CURLINFO_CONNECT_TIME.html
    "connectTimeSecs": double,
    // https://curl.haxx.se/libcurl/c/CURLINFO_APPCONNECT_TIME.html
    "appconnectTimeSecs": double,
    // https://curl.haxx.se/libcurl/c/CURLINFO_PRETRANSFER_TIME.html
    "pretransferTimeSecs": double,
    // https://curl.haxx.se/libcurl/c/CURLINFO_STARTTRANSFER_TIME.html
    "starttransferTimeSecs": double,
    // https://curl.haxx.se/libcurl/c/CURLINFO_REDIRECT_TIME.html
    "redirectTimeSecs": double,
    // https://curl.haxx.se/libcurl/c/CURLINFO_REDIRECT_COUNT.html
    "redirectCount": long,
    // https://curl.haxx.se/libcurl/c/CURLINFO_SPEED_DOWNLOAD.html
    "speedDownloadBytesSecs": double,
    // https://curl.haxx.se/libcurl/c/CURLINFO_SPEED_UPLOAD.html
    "speedUploadBytesSecs": double,
    // https://curl.haxx.se/libcurl/c/CURLINFO_HEADER_SIZE.html
    "headerSizeBytes": long,
    // https://curl.haxx.se/libcurl/c/CURLINFO_REQUEST_SIZE.html
    "requestSizeBytes": long,
    // https://curl.haxx.se/libcurl/c/CURLINFO_SSL_VERIFYRESULT.html
    "sslVerifyresult": long,
    // https://curl.haxx.se/libcurl/c/CURLINFO_OS_ERRNO.html
    "osErrno": long,
    // https://curl.haxx.se/libcurl/c/CURLINFO_NUM_CONNECTS.html
    "numConnects": long,
    // https://curl.haxx.se/libcurl/c/CURLINFO_PRIMARY_IP.html
    "primaryIp": "",
    // https://curl.haxx.se/libcurl/c/CURLINFO_PRIMARY_PORT.html
    "primaryPort": long,

 }
 */

WILTON_EXPORT char* wilton_HttpClient_execute(
        wilton_HttpClient* http,
        const char* url,
        int url_len,
        const char* request_data,
        int request_data_len,
        const char* request_metadata_json,
        int request_metadata_len,
        char** response_data_out,
        int* response_data_len_out);

WILTON_EXPORT char* wilton_HttpClient_send_file(
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
                int sent_successfully));


// NAT proxy

struct wilton_NatProxy;
typedef struct wilton_NatProxy wilton_NatProxy;

WILTON_EXPORT char* wilton_NatProxy_create(
        wilton_NatProxy** proxy_out,
        const char* conf_json,
        int conf_json_len);

WILTON_EXPORT char* wilton_NatProxy_stop(
        wilton_NatProxy* proxy);


#ifdef	__cplusplus
}
#endif

#endif	/* WILTON_H */
