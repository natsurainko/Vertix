//
// Created by Natsurainko on 2026/1/20.
//

#ifndef VERTIX_GENERALKEYBOARDDEVICE_H
#define VERTIX_GENERALKEYBOARDDEVICE_H
#include "KeyboardDevice.h"
#include "Exceptions/HResultException.h"

namespace Vertix::Engine {
    class GeneralKeyboardDevice : public KeyboardDevice {
    public:
        explicit GeneralKeyboardDevice(const GameInputInterface &inputInterface) {
            GeneralKeyboardDevice::InitializeDevice(inputInterface.GetInterface(), nullptr);
        }

        void InitializeDevice(const Microsoft::WRL::ComPtr<GameInput::v3::IGameInput> &gameInput,
                              const Microsoft::WRL::ComPtr<GameInput::v3::IGameInputDevice> &) override {
            maxSimultaneousKeys = 32;
            activeKeysArray = new GameInput::v3::GameInputKeyState[maxSimultaneousKeys];

            ThrowIfFailed(gameInput->RegisterReadingCallback(
               nullptr,
               GameInput::v3::GameInputKindKeyboard,
               this,
               OnKeyboardReadingCallback,
               &callbackToken));

            this->input = gameInput;
            this->device = nullptr;

            initialized = true;
        }
    };
}

#endif //VERTIX_GENERALKEYBOARDDEVICE_H