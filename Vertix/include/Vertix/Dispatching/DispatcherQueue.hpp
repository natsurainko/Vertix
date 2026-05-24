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
            ++pendingCount;
        }

        void FlushQueue() {
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
    private:
        std::mutex mutex;
        std::queue<std::function<void()>> callbackQueue;
        size_t pendingCount = 0;
    };
}

#endif //VERTIX_DISPATCHERQUEUE_H
