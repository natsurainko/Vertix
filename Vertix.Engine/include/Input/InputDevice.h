//
// Created by Natsurainko on 2026/1/17.
//

#ifndef VERTIX_INPUTDEVICE_H
#define VERTIX_INPUTDEVICE_H

#include "GameInput/GameInput.h"

#include <wrl/client.h>

namespace Vertix::Engine {
    class InputDevice {
    public:
        virtual ~InputDevice() = default;

        virtual void InitializeDevice(const Microsoft::WRL::ComPtr<GameInput::v3::IGameInput> &gameInput,
                              const Microsoft::WRL::ComPtr<GameInput::v3::IGameInputDevice> &gameInputDevice) {
            this->input = gameInput;
            this->device = gameInputDevice;

            initialized = true;
        }

        [[nodiscard]] bool IsInitialized() const {
            return initialized;
        }
    protected:
        bool initialized = false;
        Microsoft::WRL::ComPtr<GameInput::v3::IGameInput> input;
        Microsoft::WRL::ComPtr<GameInput::v3::IGameInputDevice> device;
    };
}
#endif //VERTIX_INPUTDEVICE_H