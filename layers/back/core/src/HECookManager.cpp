#include "HECookManager.h"

#include <utility>

HECookManager::~HECookManager(){
    cancelAll();
    waitAll();
}

HECookManager::TaskId HECookManager::start(int nodeId, uint64_t sessionEpoch, Worker worker){
    if(nodeId < 0 || !worker)
        return 0;

    std::shared_ptr<Task> task;
    {
        std::lock_guard lock(mutex_);
        if(tasks_.find(nodeId) != tasks_.end())
            return 0;

        task = std::make_shared<Task>();
        task->taskId = nextTaskId_++;
        task->nodeId = nodeId;
        task->sessionEpoch = sessionEpoch;
        task->stop = std::stop_source();

        const std::stop_token token = task->stop.get_token();
        auto workerWrapper = [this, nodeId, worker = std::move(worker), token](std::stop_token){
            try{
                worker(token);
            }catch(...){
            }

            finish(nodeId);
        };

        task->thread = std::jthread(std::move(workerWrapper));

        tasks_.emplace(nodeId, task);
    }

    return task->taskId;
}

bool HECookManager::cancel(int nodeId){
    std::shared_ptr<Task> task;
    {
        std::lock_guard lock(mutex_);
        auto it = tasks_.find(nodeId);
        if(it == tasks_.end())
            return false;
        task = it->second;
    }

    task->stop.request_stop();
    return true;
}

bool HECookManager::cancelAndWait(int nodeId){
    std::shared_ptr<Task> task;
    {
        std::lock_guard lock(mutex_);
        auto it = tasks_.find(nodeId);
        if(it == tasks_.end())
            return false;
        task = it->second;
    }

    task->stop.request_stop();
    if(task->thread.joinable())
        task->thread.join();
    return true;
}

void HECookManager::cancelAll(){
    std::vector<std::shared_ptr<Task>> tasks;
    {
        std::lock_guard lock(mutex_);
        tasks.reserve(tasks_.size());
        for(auto& [nodeId, task] : tasks_)
            tasks.push_back(task);
    }

    for(auto& task : tasks)
        task->stop.request_stop();
}

void HECookManager::cancelAllAndWait(){
    cancelAll();
    waitAll();
}

void HECookManager::waitAll(){
    for(;;){
        std::vector<std::shared_ptr<Task>> tasks;
        {
            std::lock_guard lock(mutex_);
            tasks.reserve(tasks_.size() + finished_.size());
            for(auto& [nodeId, task] : tasks_)
                tasks.push_back(task);
            for(auto& task : finished_)
                tasks.push_back(task);
            finished_.clear();
        }

        if(tasks.empty())
            break;

        for(auto& task : tasks){
            if(task->thread.joinable())
                task->thread.join();
        }
    }
}

bool HECookManager::isCooking(int nodeId) const{
    std::lock_guard lock(mutex_);
    return tasks_.find(nodeId) != tasks_.end();
}

std::size_t HECookManager::activeCount() const{
    std::lock_guard lock(mutex_);
    return tasks_.size();
}

void HECookManager::finish(int nodeId){
    std::shared_ptr<Task> task;
    {
        std::lock_guard lock(mutex_);
        auto it = tasks_.find(nodeId);
        if(it == tasks_.end())
            return;
        task = std::move(it->second);
        tasks_.erase(it);
        finished_.push_back(std::move(task));
    }
}
