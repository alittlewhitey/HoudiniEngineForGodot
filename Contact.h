//
// Created by littlewhite on 2025/2/26
//

#ifndef GALENGINE_CONTACT_H
#define GALENGINE_CONTACT_H
#include <functional>
#include <atomic>
#include <list>
#include <thread>
#include <chrono>
class Contact{
    static std::atomic<bool> is_changing;
    static std::list<std::function<void()>> call_pool;
public:
    static void add_call(std::function<void()> func);
    static void process_call();
};


#endif //GALENGINE_CONTACT_H
