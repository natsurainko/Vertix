//
// Created by Natsurainko on 2026/1/17.
//

#pragma once

#include "GameInputInterface.h"
#include "InputDevice.h"

namespace Vertix::Engine {
    class KeyboardDevice : public InputDevice {
    protected:
        uint32_t                              maxSimultaneousKeys = 4;
        uint32_t                              activeKeyCount      = 0;
        GameInput::v3::GameInputKeyState*     activeKeysArray     = nullptr;
        GameInput::v3::GameInputCallbackToken callbackToken {};

    public:
        VERTIX_ENGINE_API KeyboardDevice();
        VERTIX_ENGINE_API ~KeyboardDevice() override;

        VERTIX_ENGINE_API void InitializeDevice(
            GameInput::v3::IGameInput*                                     gameInput,
            const Microsoft::WRL::ComPtr<GameInput::v3::IGameInputDevice> &gameInputDevice) override;

        [[nodiscard]] VERTIX_ENGINE_API bool IsKeyPressed(const UINT &virtualKey) const noexcept;

        void GetActiveKeys(
            const GameInput::v3::GameInputKeyState* &keys,
            UINT &                                   keyCount) const noexcept {
            keys     = activeKeysArray;
            keyCount = activeKeyCount;
        }

    protected:
        VERTIX_ENGINE_API static void CALLBACK OnKeyboardReadingCallback(
            _In_ GameInput::v3::GameInputCallbackToken callbackToken,
            _In_ void*                                 contextPtr,
            _In_ GameInput::v3::IGameInputReading*     reading);
    };

    class GeneralKeyboardDevice : public KeyboardDevice {
    public:
        VERTIX_ENGINE_API explicit GeneralKeyboardDevice(const GameInputInterface &inputInterface);

        VERTIX_ENGINE_API void InitializeDevice(
            GameInput::v3::IGameInput* gameInput,
            const Microsoft::WRL::ComPtr<GameInput::v3::IGameInputDevice> &) override;
    };
}
