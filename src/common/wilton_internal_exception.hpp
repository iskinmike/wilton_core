/* 
 * File:   wilton_internal_exception.hpp
 * Author: alex
 *
 * Created on May 5, 2016, 7:35 PM
 */

#ifndef WILTON_COMMON_WILTON_INTERNAL_EXCEPTION_HPP
#define	WILTON_COMMON_WILTON_INTERNAL_EXCEPTION_HPP

#include <string>

#include "staticlib/support/exception.hpp"

namespace wilton {
namespace common {

/**
 * Module specific exception
 */
class wilton_internal_exception : public sl::support::exception {
public:
    /**
     * Default constructor
     */
    wilton_internal_exception() = default;

    /**
     * Constructor with message
     * 
     * @param msg error message
     */
    wilton_internal_exception(const std::string& msg) :
    sl::support::exception(msg) { }

};

} //namespace
}

#endif	/* WILTON_COMMON_WILTON_INTERNAL_EXCEPTION_HPP */

