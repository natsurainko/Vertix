//
// Created by Natsurainko on 2026/1/17.
//

#ifndef VERTIX_KEYBOARDDEVICE_H
#define VERTIX_KEYBOARDDEVICE_H

#include "Input/InputDevice.hpp"
#include "VERTIX_ENGINE_EXPORT.h"

namespace Vertix::Engine {
    class VERTIX_ENGINE_API KeyboardDevice : public InputDevice {
    public:
        KeyboardDevice();
        ~KeyboardDevice() override;

        void InitializeDevice(
            const Microsoft::WRL::ComPtr<GameInput::v3::IGameInput> &gameInput,
            const Microsoft::WRL::ComPtr<GameInput::v3::IGameInputDevice> &gameInputDevice) override;

        [[nodiscard]]
        bool IsKeyPressed(const UINT &virtualKey) const noexcept;

        void GetActiveKeys(
            const GameInput::v3::GameInputKeyState* &keys,
            UINT &keyCount) const noexcept
        {
            keys = activeKeysArray;
            keyCount = activeKeyCount;
        }

    protected:
        UINT maxSimultaneousKeys = 4;
        UINT activeKeyCount = 0;
        GameInput::v3::GameInputKeyState* activeKeysArray = nullptr;
        GameInput::v3::GameInputCallbackToken callbackToken{};

        static void CALLBACK OnKeyboardReadingCallback(
            _In_ GameInput::v3::GameInputCallbackToken callbackToken,
            _In_ void* contextPtr,
            _In_ GameInput::v3::IGameInputReading* reading);
    };
}
#endif //VERTIX_KEYBOARDDEVICE_H