#ifndef HE_COOK_MANAGER_H
#define HE_COOK_MANAGER_H

#include <cstdint>
#include <cstddef>
#include <functional>
#include <thread>
#include <stop_token>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

// Pure C++ cook task manager.
//
// This type is intentionally independent from godot-cpp and from the frontend.
// The frontend supplies a worker function which performs the actual HAPI
// polling/fetching on the manager's thread and must honor the stop_token.
class HECookManager {
public:
    using TaskId = uint64_t;
    using Worker = std::function<void(std::stop_token)>;

    HECookManager() = default;
    ~HECookManager();

    HECookManager(const HECookManager&) = delete;
    HECookManager& operator=(const HECookManager&) = delete;

    // Starts one task per node. Returns 0 when a task is already running for
    // this node. The manager owns and joins the worker thread.
    TaskId start(int nodeId, uint64_t sessionEpoch, Worker worker);

    // Requests cancellation without waiting.
    bool cancel(int nodeId);

    // Requests cancellation and blocks until the worker has exited.
    bool cancelAndWait(int nodeId);

    void cancelAll();
    void cancelAllAndWait();
    void waitAll();

    bool isCooking(int nodeId) const;
    std::size_t activeCount() const;

private:
    struct Task {
        TaskId taskId = 0;
        int nodeId = -1;
        uint64_t sessionEpoch = 0;
        std::stop_source stop;
        std::jthread thread;
    };

    void finish(int nodeId);

    mutable std::mutex mutex_;
    std::unordered_map<int, std::shared_ptr<Task>> tasks_;
    // Finished but not yet joined from another thread. Keeping the Task and
    // its jthread here prevents destroying a joinable jthread on its own
    // worker thread.
    std::vector<std::shared_ptr<Task>> finished_;
    TaskId nextTaskId_ = 1;
};

#endif // HE_COOK_MANAGER_H
