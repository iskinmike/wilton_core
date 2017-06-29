/* 
 * File:   cli_options.hpp
 * Author: alex
 *
 * Created on June 29, 2017, 3:09 PM
 */

#ifndef WILTON_CLI_OPTIONS_HPP
#define	WILTON_CLI_OPTIONS_HPP

#include <algorithm>
#include <string>
#include <vector>

#include <popt.h>

namespace wilton {
namespace launcher {

class cli_options {
    std::vector<struct poptOption> table;
    char* modules_dir_ptr = nullptr;
    char* requirejs_dir_ptr = nullptr;
    char* startup_module_name_ptr = nullptr;
    
public:    
    poptContext ctx = nullptr;
    std::string parse_error;
    std::vector<std::string> args;

    // public options list
    std::string modules_dir;
    std::string requirejs_dir;
    std::string startup_module_name;
    int help = 0;
    std::string indexjs;

    cli_options(int argc, char** argv) :
    table({
        { "modules-dir", 'm', POPT_ARG_STRING, std::addressof(modules_dir_ptr), static_cast<int> ('m'), "Path to modules directory", nullptr},
        { "requirejs-dir", 'r', POPT_ARG_STRING, std::addressof(requirejs_dir_ptr), static_cast<int> ('r'), "Path to requirejs directory", nullptr},
        { "startup-module-name", 's', POPT_ARG_STRING, std::addressof(startup_module_name_ptr), static_cast<int> ('i'), "Name of the index module", nullptr},
        { "help", 'h', POPT_ARG_NONE, std::addressof(help), static_cast<int> ('h'), "Show this help message", nullptr},
        { nullptr, 0, 0, nullptr, 0, nullptr, nullptr}
    }) {

        { // create context
            ctx = poptGetContext(nullptr, argc, const_cast<const char**> (argv), table.data(), POPT_CONTEXT_NO_EXEC);
            if (!ctx) {
                parse_error.append("'poptGetContext' error");
                return;
            }
        }

        { // parse options
            int val;
            while ((val = poptGetNextOpt(ctx)) >= 0);
            if (val < -1) {
                parse_error.append(poptStrerror(val));
                parse_error.append(": ");
                parse_error.append(poptBadOption(ctx, POPT_BADOPTION_NOALIAS));
                return;
            }
        }

        { // collect arguments
            const char* ar;
            while (nullptr != (ar = poptGetArg(ctx))) {
                args.emplace_back(std::string(ar));
            }
        }

        // check script specified
        if (0 == help && (1 != args.size() || args.at(0).empty())) {
            parse_error.append("invalid arguments, startup script not specified");
            return;
        }
        
        // set options and fix slashes
        indexjs = 0 == help ? args.at(0) : "";
        std::replace(indexjs.begin(), indexjs.end(), '\\', '/');
        modules_dir = nullptr != modules_dir_ptr ? std::string(modules_dir_ptr) : "";
        fix_dir_slashes(modules_dir);
        requirejs_dir = nullptr != requirejs_dir_ptr ? std::string(requirejs_dir_ptr) : "";
        fix_dir_slashes(requirejs_dir);
        startup_module_name = nullptr != startup_module_name_ptr ? std::string(startup_module_name_ptr) : "";
    }
    
    const std::string& usage() {
        static std::string msg = "OPTIONS: wilton path/to/script.js"
                " [-m|--modules-dir=STRING]"
                " [-r|--requirejs-dir=STRING]"
                " [-s|--startup-module-name=STRING]"
                " [-- <app arguments>]";
        return msg;
    }

    ~cli_options() {
        poptFreeContext(ctx);
    }

    cli_options(const cli_options& other) = delete;

    cli_options& operator=(const cli_options& other) = delete;

private:
    static void fix_dir_slashes(std::string& dir) {
        if (!dir.empty()) {
            std::replace(dir.begin(), dir.end(), '\\', '/');        
            if ('/' != dir.at(dir.length() - 1)) {
                dir.push_back('/');
            }
        }
    }
    
};


} // namespace
}

#endif	/* WILTON_CLI_OPTIONS_HPP */

