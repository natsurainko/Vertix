//
// Created by Natsurainko on 2026/1/27.
//

#pragma once

namespace Vertix::Engine {
    template <typename TValue>
    struct IControllerInputListener {
        virtual      ~IControllerInputListener() = default;
        virtual void OnInputUpdated(const TValue &value, double deltaTime) = 0;
    };

    template <typename TValue, size_t MaxListeners = 1>
    class IControllerInputer {
        IControllerInputListener<TValue>* listeners[MaxListeners] = {};
        size_t                            listenerCount           = 0;

    protected:
        void NotifyUpdate(double deltaTime) {
            for (size_t i = 0; i < listenerCount; ++i)
                listeners[i]->OnInputUpdated(value, deltaTime);
        }

        TValue value;

    public:
        virtual ~IControllerInputer() = default;

        bool AddListener(IControllerInputListener<TValue>* listener) {
            if (listenerCount >= MaxListeners) return false;
            listeners[listenerCount++] = listener;
            return true;
        }
    };
}
