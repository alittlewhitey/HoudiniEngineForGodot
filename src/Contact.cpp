
#ifndef GALENGINE_CONTACT_SRC
#define GALENGINE_CONTACT_SRC
#include "Contact.h"
std::atomic<bool> Contact::is_changing = 0;
std::list<std::pair<Contact::TaskID,std::function<void()>>> Contact::call_pool = std::list<std::pair<Contact::TaskID,std::function<void()>>>();
Contact::TaskID Contact::counter = 0;
Contact::TaskID Contact::add_call(std::function<void()> func) {
    while(is_changing.load()){
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ms);
    }
    auto res = counter;
    ++counter;
    is_changing.store(1);
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
    auto a = call_pool.front().second;
    call_pool.erase(call_pool.begin());
    is_changing.store(0);
    try{
        a();
    }catch(std::exception& e){
        std::cerr << e.what() << std::endl;
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