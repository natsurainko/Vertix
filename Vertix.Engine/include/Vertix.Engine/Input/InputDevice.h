//
// Created by Natsurainko on 2026/1/17.
//

#pragma once

#include <GameInput/GameInput.h>
#include <wrl/client.h>

namespace Vertix::Engine {
    class InputDevice {
    protected:
        Microsoft::WRL::ComPtr<GameInput::v3::IGameInputDevice> device;

        bool                       initialized = false;
        GameInput::v3::IGameInput* input       = nullptr;

    public:
        virtual ~InputDevice() = default;

        virtual void InitializeDevice(
            GameInput::v3::IGameInput*                                     gameInput,
            const Microsoft::WRL::ComPtr<GameInput::v3::IGameInputDevice> &gameInputDevice) {
            this->input  = gameInput;
            this->device = gameInputDevice;

            initialized = true;
        }

        [[nodiscard]] bool IsInitialized() const noexcept { return initialized; }
    };
}
