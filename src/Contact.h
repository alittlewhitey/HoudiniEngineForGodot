//
// Created by littlewhite on 2025/2/26
//

#ifndef GALENGINE_CONTACT_H
#define GALENGINE_CONTACT_H
#include <iostream>
#include <functional>
#include <atomic>
#include <deque>
#include <thread>
#include <chrono>
class Contact{
public:
    typedef uint32_t TaskID;
private:
    static std::atomic<bool> is_changing;
    static std::deque<std::pair<TaskID,std::function<void()>>> call_pool;
    static TaskID counter;
public:
    static TaskID add_call(std::function<void()> func);
    static void process_call();
    static bool find_if(TaskID id);
};


#endif //GALENGINE_CONTACT_H
