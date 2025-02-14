#include "scheduler.hpp"

#include <mbgl/util/util.hpp>

#include <cassert>

namespace QMapLibre {

Scheduler::Scheduler() = default;

Scheduler::~Scheduler() {
    MBGL_VERIFY_THREAD(tid);
}

void Scheduler::schedule(std::function<void()> function) {
    const std::lock_guard<std::mutex> lock(m_taskQueueMutex);
    m_taskQueue.push(std::move(function));

    // Need to force the main thread to wake
    // up this thread and process the events.
    emit needsProcessing();
}

void Scheduler::processEvents() {
    std::queue<std::function<void()>> taskQueue;
    {
        const std::unique_lock<std::mutex> lock(m_taskQueueMutex);
        std::swap(taskQueue, m_taskQueue);
    }

    while (!taskQueue.empty()) {
        auto& function = taskQueue.front();
        if (function) {
            function();
        }
        taskQueue.pop();
    }
}

} // namespace QMapLibre
