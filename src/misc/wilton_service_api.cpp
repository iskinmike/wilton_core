

#include "wilton/wilton_service_api.h"

#include "wilton/support/alloc.hpp"
#include "wilton_service.hpp"


char* wilton_service_increase_thread_count() {
    try {
        wilton::service::wilton_service::increase_thread_count();
        return nullptr;
    } catch (const std::exception& e) {
        return wilton::support::alloc_copy(TRACEMSG(e.what() + "\nException raised"));
    }
}

char* wilton_service_decrease_thread_count() {
    try {
        wilton::service::wilton_service::decrease_thread_count();
        return nullptr;
    } catch (const std::exception& e) {
        return wilton::support::alloc_copy(TRACEMSG(e.what() + "\nException raised"));
    }
}

char* wilton_service_get_thread_count(int* count_out) {
    try {
        *count_out = wilton::service::wilton_service::get_thread_count();
        return nullptr;
    } catch (const std::exception& e) {
        return wilton::support::alloc_copy(TRACEMSG(e.what() + "\nException raised"));
    }
}
