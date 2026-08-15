#ifndef HOUDINI_ENGINE_CONTACT_H
#define HOUDINI_ENGINE_CONTACT_H
#include "DebugSymbol.h"
#include <iostream>
#include <functional>
#include <mutex>
#include <deque>
#include <unordered_set>
// The single "background thread -> main thread" task queue.
// add_call is thread-safe and may be called from any thread; process_call must
// only run on the main thread (once per frame). Tasks posted here must be
// main-thread-only work (Godot object construction / callbacks); background
// threads must never touch Godot objects directly.
class HEGContact{
public:
    typedef uint32_t TaskID;
private:
    static std::mutex mutex_;
    static std::deque<std::pair<TaskID,std::function<void()>>> call_pool;
    static std::unordered_set<TaskID> cancelled_;
    static TaskID counter;
public:
    // Thread-safe. Post a task from any thread, returns its TaskID.
    static TaskID add_call(std::function<void()> func);
    // Main thread only. Drain and run every posted task.
    static void process_call();
    // Thread-safe. Best-effort cancel: mark a still-pending task as cancelled.
    // Returns true if it was found (and marked), false if it already ran or
    // does not exist. Tasks already drained by process_call cannot be undone.
    static bool cancel(TaskID id);
    // Thread-safe. Query whether a TaskID is still pending in the queue.
    static bool find_if(TaskID id);
};


#endif
