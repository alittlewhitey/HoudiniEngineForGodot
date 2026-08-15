
#ifndef GALENGINE_CONTACT_SRC
#define GALENGINE_CONTACT_SRC
#include "HEGContact.h"
std::mutex HEGContact::mutex_;
std::deque<std::pair<HEGContact::TaskID,std::function<void()>>> HEGContact::call_pool = std::deque<std::pair<HEGContact::TaskID,std::function<void()>>>();
std::unordered_set<HEGContact::TaskID> HEGContact::cancelled_;
HEGContact::TaskID HEGContact::counter = 0;
HEGContact::TaskID HEGContact::add_call(std::function<void()> func) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto res = counter;
    ++counter;
    call_pool.emplace_back(res,func);
    return res;
}
void HEGContact::process_call() {
    decltype(call_pool) internal_call_pool;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if(call_pool.empty())
            return;
        internal_call_pool = std::move(call_pool);
    }
    while(!internal_call_pool.empty()){
        auto a = internal_call_pool.front();
        internal_call_pool.pop_front();
        {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = cancelled_.find(a.first);
            if(it != cancelled_.end()){
                cancelled_.erase(it);
                continue;
            }
        }
        try{
            a.second();
        }catch(std::exception& e){
            std::cerr << e.what() << std::endl;
        }catch(...){
            std::cerr << "Unknown exception in HEGContact callback" << std::endl;
        }
    }
}
bool HEGContact::cancel(HEGContact::TaskID id){
    std::lock_guard<std::mutex> lock(mutex_);
    for(auto& a : call_pool){
        if(a.first == id){
            cancelled_.insert(id);
            return true;
        }
    }
    return false;
}
bool HEGContact::find_if(HEGContact::TaskID id){
    std::lock_guard<std::mutex> lock(mutex_);
    for(auto& a : call_pool){
        if(a.first == id)
            return true;
    }
    return false;
}
#endif //GALENGINE_CONTACT_SRC