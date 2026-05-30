//
// Created by Natsurainko on 2026/3/17.
//

#pragma once

#include <functional>
#include <mutex>
#include <queue>

namespace Vertix {
    class DispatcherQueue {
    public:
        VERTIX_API void Enqueue(std::function<void()> fn) noexcept;
        VERTIX_API void FlushQueue();

    private:
        std::mutex                        mutex;
        std::queue<std::function<void()>> callbackQueue;
        size_t                            pendingCount = 0;
    };
}
