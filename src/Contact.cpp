
#ifndef GALENGINE_CONTACT_SRC
#define GALENGINE_CONTACT_SRC
#include "Contact.h"
std::atomic<bool> Contact::is_changing = 0;
std::list<std::function<void()>> Contact::call_pool = std::list<std::function<void()>>();

void Contact::add_call(std::function<void()> func) {
    while(is_changing.load()){
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ms);
    }
    is_changing.store(1);
    call_pool.emplace_back(func);
    is_changing.store(0);
}

void Contact::process_call() {
    if(call_pool.empty())
        return;
    while(is_changing.load()){
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ms);
    }
    is_changing.store(1);
    auto a = call_pool.front();
    call_pool.erase(call_pool.begin());
    is_changing.store(0);
    a();
}
#endif //GALENGINE_CONTACT_SRC