/* 
 * File:   winservice.cpp
 * Author: alex
 *
 * Created on June 6, 2017, 6:31 PM
 */

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <popt.h>

#include "staticlib/json.hpp"
#include "staticlib/ranges.hpp"
#include "staticlib/utils.hpp"

#include "common/wilton_internal_exception.hpp"

#include "launcher/winservice_config.hpp"
#include "launcher/winservice_options.hpp"

namespace { //anonymous

std::string current_exedir() {
    auto exepath = sl::utils::current_executable_path();
    auto exedir = sl::utils::strip_filename(exepath);
    std::replace(exedir.begin(), exedir.end(), '\\', '/');
}

wilton::launcher::winservice_config load_config(const std::string& exedir, char* cpath) {
    auto path = [cpath] {
        if (nullptr != cpath) {
            return std::string(cpath);
        }
        return exedir + "config.json";
    }();
    auto fi = sl::tinydir::file_source(path);
    std::map<std::string, std::string> values = {{"appdir", exedir}};
    auto onerr = [](const std::string & err) {
        throw common::wilton_internal_exception(TRACEMSG(err));
    };
    auto src = sl::io::replacer_source<sl::io::string_source>(fi, values, onerr, "${", "}");
    return sl::json::load(src);
}

void init_wilton(const std::string& exedir) {
    auto config = sl::json::dumps({
        {"defaultScriptEngine", "duktape"},
        {"requireJsDirPath", exedir + "requirejs"},
        {"requireJsConfig", {
                {"waitSeconds", 0},
                {"enforceDefine", true},
                {"nodeIdCompat", true},
                {"baseUrl", exedir + "modules"}
            }}
    });
    auto err = wiltoncall_init(config.c_str(), static_cast<int> (config.length()));
    if (nullptr != err) {
        auto msg = TRACEMSG(err);
        wilton_free(err);
        throw common::wilton_internal_exception(msg);
    }
}

void run_script(const std::string& func, const std::vector<std::string>& args) {
    std::string in = sl::json::dumps({
        { "module", "index"},
        { "func", func},
        { "args", [&opts] {
                auto ra = sl::ranges::transform(args, [](const std::string& ar) {
                    return sl::json::value(ar);
                });
                return ra.to_vector();
            } ()},
    });
    char* out = nullptr;
    int out_len;
    auto err = wiltoncall_runscript_duktape(in.c_str(), in.length(), std::addressof(out), std::addressof(out_len));
    if (nullptr != err) {
        auto msg = TRACEMSG(err);
        wilton_free(err);
        throw common::wilton_internal_exception(msg);
    }
    if (nullptr != out) {
        auto res = std::string(out, static_cast<uint32_t> (out_len));
        wilton_free(out);
        std::cout << res << std::endl;
    }
}

void install(const wilton::launcher::winservice_config& conf) {
    std::cout << "Installing service with config: [" + conf.to_json().dumps() + "]" << std::endl;
    if (!sl::utils::current_process_elevated()) {
        throw common::wilton_internal_exception(TRACEMSG(
            "Service install error, must be run under elevated (Administrator) account"));
    }
    sl::utils::ensure_has_logon_as_service(conf.user);
    sl::winservice::install_service(conf.service_name, conf.display_name,
            ".\\" + conf.account, conf.password);
    sl::winservice::start_service(conf.service_name);
}

void uninstall(const wilton::launcher::winservice_config& conf) {
    std::cout << "Uninstalling service with config: [" + conf.to_json().dumps() + "]" << std::endl;
    if (!sl::utils::current_process_elevated()) {
        throw common::wilton_internal_exception(TRACEMSG(
            "Service uninstall error, must be run under elevated (Administrator) account"));
    }
    sl::winservice::uninstall_service(conf.service_name);
}

void stop(const wilton::launcher::winservice_config& conf) {
    std::cout << "Stopping service, name: [" + conf.service_name + "]" << std::endl;
    sl::winservice::stop_service(conf.service_name);
}

void start_service_and_wait(const wilton::launcher::winservice_config& conf,
        const std::vector<std::string>& arguments) {
    std::cout << "Starting service with config: [" + conf.to_json().dumps() + "]" << std::endl;
    auto args = std::make_shared<std::vector<std::string>>(args);
    sl::winservice::start_service_and_wait(conf.service_name,
    [args] {
        run_script("start", *args);
    },
    [args] {
        run_script("stop", *args);
        std::cout << "Stopping service ..." << std::endl;
    },
    [](const std::string& msg) {
        std::err << msg << std::endl;
    });
    std::cout << "Service stopped" << std::endl;
}

} // namespace

int main(int argc, char** argv) {

    // parse command line
    wilton::launcher::winservice_options opts(argc, argv);

    // check invalid options
    if (!opts.parse_error.empty()) {
        std::cerr << "ERROR: " << opts.parse_error << std::endl;
        poptPrintUsage(opts.ctx, stderr, 0);
        return 1;
    }

    // show help
    if (opts.help) {
        poptPrintHelp(opts.ctx, stdout, 0);
        return 0;
    }
    
    try {
        std::string exedir = current_exedir();
        init_wilton(exedir);
        wilton::launcher::winservice_config conf = load_config(exedir, opts.config);

        if (opts.install) {
            install(conf);
        } else if (opts.uninstall) {
            uninstall(conf);
        } else if (opts.stop) {
            stop(conf);
        } else if (opts.direct) {
            run_script("start", opts.args);
        } else { // SCM call            
            start_service_and_wait(conf, opts.args);
        }

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}

