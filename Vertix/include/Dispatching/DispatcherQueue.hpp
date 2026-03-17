//
// Created by Natsurainko on 2026/3/17.
//

#ifndef VERTIX_DISPATCHERQUEUE_H
#define VERTIX_DISPATCHERQUEUE_H

#include <functional>
#include <mutex>
#include <queue>

namespace Vertix {
    class DispatcherQueue {
    public:
        void Enqueue(std::function<void()> fn) {
            std::lock_guard lock(mutex);
            callbackQueue.push(std::move(fn));
            pendingCount.fetch_add(1, std::memory_order_release);
        }

        void FlushQueue() {
            if (pendingCount.load(std::memory_order_acquire) == 0)
                return;

            std::queue<std::function<void()>> local;
            {
                std::lock_guard lock(mutex);
                std::swap(local, callbackQueue);
                pendingCount.store(0, std::memory_order_relaxed);
            }
            while (!local.empty()) {
                local.front()();
                local.pop();
            }
        }
    private:
        std::mutex mutex;
        std::queue<std::function<void()>> callbackQueue;
        std::atomic<size_t> pendingCount{0};
    };
}

#endif //VERTIX_DISPATCHERQUEUE_H