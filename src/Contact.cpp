
#ifndef GALENGINE_CONTACT_SRC
#define GALENGINE_CONTACT_SRC
#include "Contact.h"
std::atomic<bool> Contact::is_changing = 0;
std::deque<std::pair<Contact::TaskID,std::function<void()>>> Contact::call_pool = std::deque<std::pair<Contact::TaskID,std::function<void()>>>();
Contact::TaskID Contact::counter = 0;
Contact::TaskID Contact::add_call(std::function<void()> func) {
    while(is_changing.load()){
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ms);
    }
    is_changing.store(1);
    auto res = counter;
    ++counter;
    call_pool.emplace_back(res,func);
    is_changing.store(0);
    return res;
}
void Contact::process_call() {
    if(call_pool.empty())
        return;
    while(is_changing.load()){
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ms);
    }
    is_changing.store(1);
    auto internal_call_pool = std::move(call_pool);
    is_changing.store(0);
    while(!internal_call_pool.empty()){
        auto a = internal_call_pool.front().second;
        internal_call_pool.pop_front();
        try{
            a();
        }catch(std::exception& e){
            std::cerr << e.what() << std::endl;
        }
    }
}
bool Contact::find_if(Contact::TaskID id){
    for(auto& a : call_pool){
        if(a.first == id)
            return true;
    }
    return false;
}
#endif //GALENGINE_CONTACT_SRC