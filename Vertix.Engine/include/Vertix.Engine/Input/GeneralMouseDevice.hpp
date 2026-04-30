//
// Created by Natsurainko on 2026/1/20.
//

#ifndef VERTIX_GENERALMOUSEDEVICE_H
#define VERTIX_GENERALMOUSEDEVICE_H

#include "Vertix.Engine/Input/GameInputInterface.h"
#include "Vertix.Engine/Input/MouseDevice.h"

namespace Vertix::Engine {
    class GeneralMouseDevice : public MouseDevice {
    public:
        explicit GeneralMouseDevice(const GameInputInterface &inputInterface) {
            GeneralMouseDevice::InitializeDevice(inputInterface.GetInterface(), nullptr);
        }

        void InitializeDevice(
            const Microsoft::WRL::ComPtr<GameInput::v3::IGameInput> &gameInput,
            const Microsoft::WRL::ComPtr<GameInput::v3::IGameInputDevice> &) override
        {
            MouseDevice::InitializeDevice(gameInput, nullptr);
        }
    };
}

#endif //VERTIX_GENERALMOUSEDEVICE_H
