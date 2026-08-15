#ifndef HE_COOK_SERVICE_H
#define HE_COOK_SERVICE_H

#include <cstdint>
#include <functional>

#include "HECookManager.h"

// Host-independent cook scheduling facade.
//
// This service currently owns the cook task manager. Later it will also own
// cook options, progress reporting, and dependency ordering.
class HECookService {
public:
    using TaskId = HECookManager::TaskId;
    using Worker = HECookManager::Worker;

    TaskId start(int nodeId, uint64_t sessionEpoch, Worker worker){
        return manager_.start(nodeId, sessionEpoch, std::move(worker));
    }

    bool cancel(int nodeId){
        return manager_.cancel(nodeId);
    }

    bool cancelAndWait(int nodeId){
        return manager_.cancelAndWait(nodeId);
    }

    void cancelAll(){
        manager_.cancelAll();
    }

    void cancelAllAndWait(){
        manager_.cancelAllAndWait();
    }

    void waitAll(){
        manager_.waitAll();
    }

    bool isCooking(int nodeId) const{
        return manager_.isCooking(nodeId);
    }

    std::size_t activeCount() const{
        return manager_.activeCount();
    }

private:
    HECookManager manager_;
};

#endif // HE_COOK_SERVICE_H
