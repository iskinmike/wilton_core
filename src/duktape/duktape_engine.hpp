/* 
 * File:   duktape_engine.hpp
 * Author: alex
 *
 * Created on May 20, 2017, 2:09 PM
 */

#ifndef WILTON_DUKTAPE_ENGINE_HPP
#define	WILTON_DUKTAPE_ENGINE_HPP

#include <string>

#include "staticlib/pimpl.hpp"

#include "common/wilton_internal_exception.hpp"

namespace wilton {
namespace duktape {

class duktape_engine : public sl::pimpl::object {
protected:
    /**
     * implementation class
     */
    class impl;
public:
    /**
     * PIMPL-specific constructor
     * 
     * @param pimpl impl object
     */
    PIMPL_CONSTRUCTOR(duktape_engine)
            
    duktape_engine();

    std::string run_script(const std::string& script_body, const std::string& filename);
};

} // namespace
}

#endif	/* WILTON_DUKTAPE_ENGINE_HPP */

