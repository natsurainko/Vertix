//
// Created by Natsurainko on 2026/5/26.
//

#include "Vertix/Dispatching/DispatcherQueue.h"

void Vertix::DispatcherQueue::Enqueue(std::function<void()> fn) noexcept {
    std::lock_guard lock(mutex);
    callbackQueue.push(std::move(fn));
    ++pendingCount;
}

void Vertix::DispatcherQueue::FlushQueue() {
    std::queue<std::function<void()>> local;
    {
        std::lock_guard lock(mutex);
        if (!pendingCount) return;
        std::swap(local, callbackQueue);
        pendingCount = 0;
    }

    while (!local.empty()) {
        local.front()();
        local.pop();
    }
}
