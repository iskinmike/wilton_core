/* 
 * File:   wilton_c.h
 * Author: alex
 *
 * Created on April 30, 2016, 11:49 PM
 */

#ifndef WILTON_C_H
#define	WILTON_C_H

#ifdef _WIN32
#ifdef WILTON_DLL_IMPORT
#define WILTON_EXPORT __declspec(dllimport)
#else
#define WILTON_EXPORT __declspec(dllexport)
#endif
#else
#define WILTON_EXPORT __attribute__ ((visibility ("default")))
#endif

#ifdef	__cplusplus
extern "C" {
#endif

struct wilton_Server;
typedef struct wilton_Server wilton_Server;

struct wilton_Request;
typedef struct wilton_Request wilton_Request;

WILTON_EXPORT void wilton_free(
        char* errmsg);
/*
    {
        "numberOfThreads": uint32_t, 
        "tcpPort": uint16_t,
        "ipAddress": "x.x.x.x",
        "ssl": {
            "keyFile": "path/to/file",
            "keyPassword": "pwd",
            "verifyFile": "path/to/file",
            "verifyOrganization_unit": "ou_name",
        },
        "documentRoot": {
            "urlPath": "/path/to/hanldler",
            "dirPath": "path/to/directory".
            "cacheMaxAge": uint32_t
        }
    }
 */
WILTON_EXPORT char* wilton_Server_create(
        wilton_Server** server_out,
        void* handler_ctx,
        void (*handler)(
                void* handler_ctx,
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
// set-cookie is always an array. Duplicates are added to the array.
// For all other headers, the values are joined together with ', '.
{
    "httpVersion": "1.1",
    "method": "GET|POST|PUT|DELETE",
    "resource": "/path/to/hanldler",
    "queryString": "/path/to/hanldler?param1=val1...",
    "headers": {
        "Header-Name": "header_value",
        ...
    }
}
 */
WILTON_EXPORT char* wilton_Request_get_request_metadata(
        wilton_Request* request,
        const char** metadata_json_out,
        int* metadata_json_len_out);

WILTON_EXPORT const char* wilton_Request_get_request_data(
        wilton_Request* request,
        const char** data_out,
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

WILTON_EXPORT char* wilton_Request_send_response_chunked(
        wilton_Request* request,
        void* read_ctx,
        int (*read)(
                void* read_ctx,
                char* buf,
                int len));

#ifdef	__cplusplus
}
#endif

#endif	/* WILTON_C_H */

