#ifndef HOUDINI_ENGINE_CONTACT_H
#define HOUDINI_ENGINE_CONTACT_H
#include "DebugSymbol.h"
#include <iostream>
#include <functional>
#include <mutex>
#include <deque>
class Contact{
public:
    typedef uint32_t TaskID;
private:
    static std::mutex mutex_;
    static std::deque<std::pair<TaskID,std::function<void()>>> call_pool;
    static TaskID counter;
public:
    static TaskID add_call(std::function<void()> func);
    static void process_call();
    static bool find_if(TaskID id);
};


#endif
