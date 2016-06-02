/* 
 * File:   WiltonLogger.hpp
 * Author: alex
 *
 * Created on May 10, 2016, 4:49 PM
 */

#ifndef WILTON_LOGGING_WILTONLOGGER_HPP
#define	WILTON_LOGGING_WILTONLOGGER_HPP

#include <string>

#include "staticlib/pimpl.hpp"

#include "WiltonInternalException.hpp"
#include "json/Appender.hpp"
#include "json/Logger.hpp"
#include "json/Logging.hpp"

namespace wilton {
namespace logging {

class WiltonLogger : public staticlib::pimpl::PimplObject {
protected:
    /**
     * Implementation class
     */
    class Impl;
public:
    /**
     * PIMPL-specific constructor
     * 
     * @param pimpl impl object
     */
    PIMPL_CONSTRUCTOR(WiltonLogger)    

    static void log(const std::string& level_name, const std::string& logger_name, const std::string& message);
    
    static void apply_config(const json::Logging& config);
    
};

} // namespace
}

#endif	/* WILTON_LOGGING_WILTONLOGGER_HPP */
