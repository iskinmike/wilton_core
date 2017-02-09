/* 
 * File:   wiltoncall_cron.cpp
 * Author: alex
 *
 * Created on January 11, 2017, 9:19 AM
 */

#include "call/wiltoncall_internal.hpp"

#include <atomic>
#include <memory>

#include "wilton/wilton.h"
#include "wilton/wiltoncall.h"

#include "logging/logging_internal.hpp"

namespace wilton {
namespace cron {

namespace { //anonymous

namespace ss = staticlib::serialization;

common::payload_handle_registry<wilton_CronTask, std::unique_ptr<std::string>>& static_registry() {
    static common::payload_handle_registry<wilton_CronTask, std::unique_ptr<std::string>> registry;
    return registry;
}

} // namespace

std::string cron_start(const std::string& data) {
    // json parse
    ss::json_value json = ss::load_json_from_string(data);
    auto rcallback = std::ref(common::empty_json());
    auto rexpr = std::ref(common::empty_string());
    for (const ss::json_field& fi : json.as_object()) {
        auto& name = fi.name();
        if ("callbackScript" == name) {
            common::check_json_callback_script(fi);
            rcallback = fi.value();
        } else if ("expression" == name) {
            rexpr = common::get_json_string(fi);
        } else {
            throw common::WiltonInternalException(TRACEMSG("Unknown data field: [" + name + "]"));
        }
    }
    if (ss::json_type::nullt == rcallback.get().type()) throw common::WiltonInternalException(TRACEMSG(
            "Required parameter 'callbackScript' not specified"));
    if (rexpr.get().empty()) throw common::WiltonInternalException(TRACEMSG(
            "Required parameter 'url' not specified"));
    const ss::json_value& callback = rcallback.get();
    const std::string& expr = rexpr.get();
    std::string* str_to_pass = new std::string(ss::dump_json_to_string(callback));
    // call wilton
    wilton_CronTask* cron;
    char* err = wilton_CronTask_start(std::addressof(cron), expr.c_str(), expr.length(),
            static_cast<void*> (str_to_pass),
            [](void* passed) {
                std::string* str = static_cast<std::string*> (passed);
                // output will be ignored
                char* out;
                int out_len;
                auto err = wiltoncall_runscript(str->c_str(), static_cast<int> (str->length()),
                std::addressof(out), std::addressof(out_len));
                if (nullptr != err) {
                    log_error("wilton.cron", TRACEMSG(err));
                    wilton_free(err);
                }
            });
    if (nullptr != err) common::throw_wilton_error(err, TRACEMSG(err));
    int64_t handle = static_registry().put(cron, std::unique_ptr<std::string>(str_to_pass));
    return ss::dump_json_to_string({
        { "cronHandle", handle}
    });
}

std::string cron_stop(const std::string& data) {
    // json parse
    ss::json_value json = ss::load_json_from_string(data);
    int64_t handle = -1;
    for (const ss::json_field& fi : json.as_object()) {
        auto& name = fi.name();
        if ("cronHandle" == name) {
            handle = common::get_json_int64(fi);
        } else {
            throw common::WiltonInternalException(TRACEMSG("Unknown data field: [" + name + "]"));
        }
    }
    if (-1 == handle) throw common::WiltonInternalException(TRACEMSG(
            "Required parameter 'cronHandle' not specified"));
    // get handle
    auto pa = static_registry().remove(handle);
    if (nullptr == pa.first) throw common::WiltonInternalException(TRACEMSG(
            "Invalid 'cronHandle' parameter specified"));
    // call wilton
    char* err = wilton_CronTask_stop(pa.first);
    if (nullptr != err) {
        static_registry().put(pa.first, std::move(pa.second));
        common::throw_wilton_error(err, TRACEMSG(err));
    }
    return "{}";
}

} // namespace
}
