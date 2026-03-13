//
// Created by Natsurainko on 2026/1/27.
//

#ifndef VERTIX_ICONTROLLERINPUT_H
#define VERTIX_ICONTROLLERINPUT_H

#include <functional>

namespace Vertix::Engine {
    template <typename TValue>
    class IControllerInput {
    public:
        virtual ~IControllerInput() = default;

        [[nodiscard]]
        virtual const TValue& GetValue() const = 0;

        void OnUpdate(const std::function<void(double)> &handler) {
            onUpdate = handler;
        }

    protected:
        TValue value;
        std::function<void(double)> onUpdate;
    };
}

#endif //VERTIX_ICONTROLLERINPUT_H