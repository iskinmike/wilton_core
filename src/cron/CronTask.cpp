/* 
 * File:   CronTask.cpp
 * Author: alex
 * 
 * Created on September 7, 2016, 12:51 PM
 */

#include "cron/CronTask.hpp"

#include <atomic>
#include <chrono>
#include <thread>

#include "staticlib/cron.hpp"
#include "staticlib/pimpl/pimpl_forward_macros.hpp"

namespace wilton {
namespace cron {

namespace { // anonymous

namespace cr = staticlib::cron;

using task_fun_type = std::function<void()>;

} //namespace

class CronTask::Impl : public staticlib::pimpl::PimplObject::Impl {
    std::atomic_bool running;
    std::thread worker;
    
public:
    ~Impl() STATICLIB_NOEXCEPT { }; 
    
    Impl(const std::string& cronexpr, std::function<void()> task) :
    running(true),
    worker([this, cronexpr, task]{
        cr::CronExpression cron{cronexpr};
        while (this->running.load()) {
            std::chrono::seconds secs = cron.next();
            std::this_thread::sleep_for(secs);
            task();
        }
    }) {
        worker.detach();
    }
        
    void stop(CronTask&) {
        this->running.store(false);
    }
};
PIMPL_FORWARD_CONSTRUCTOR(CronTask, (const std::string&)(task_fun_type), (), common::WiltonInternalException)
PIMPL_FORWARD_METHOD(CronTask, void, stop, (), (), common::WiltonInternalException)

} // namespace
}
