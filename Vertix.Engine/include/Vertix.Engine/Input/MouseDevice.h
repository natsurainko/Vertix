//
// Created by Natsurainko on 2026/1/17.
//

#pragma once

#include <Vertix/Math/Vector2D.hpp>

#include "GameInputInterface.h"
#include "InputDevice.h"

namespace Vertix::Engine {
    class MouseDevice : public InputDevice {
    protected:
        Vector2D<int> delta;

        GameInput::v3::GameInputMouseState previousMouseState {};
        GameInput::v3::GameInputMouseState currentMouseState {};

    public:
        VERTIX_ENGINE_API void InitializeDevice(
            GameInput::v3::IGameInput*                                     gameInput,
            const Microsoft::WRL::ComPtr<GameInput::v3::IGameInputDevice> &gameInputDevice) override;

        VERTIX_ENGINE_API void Update();

        [[nodiscard]] Vector2D<int> GetDeltaOffset() const noexcept { return delta; }
        [[nodiscard]] Vector2D<int> GetPosition() const noexcept { return { static_cast<int>(currentMouseState.absolutePositionX), static_cast<int>(currentMouseState.absolutePositionY) }; }

        [[nodiscard]] bool IsButtonDown(const GameInput::v3::GameInputMouseButtons button) const noexcept { return (currentMouseState.buttons & button) != 0; }
        [[nodiscard]] bool IsButtonUp(const GameInput::v3::GameInputMouseButtons button) const noexcept { return (currentMouseState.buttons & button) == 0; }
        [[nodiscard]] bool WasButtonPressed(const GameInput::v3::GameInputMouseButtons button) const noexcept { return (previousMouseState.buttons & button) == 0 && (currentMouseState.buttons & button) != 0; }
        [[nodiscard]] bool WasButtonReleased(const GameInput::v3::GameInputMouseButtons button) const noexcept { return (previousMouseState.buttons & button) != 0 && (currentMouseState.buttons & button) == 0; }
    };

    class GeneralMouseDevice : public MouseDevice {
    public:
        VERTIX_ENGINE_API explicit GeneralMouseDevice(const GameInputInterface &inputInterface);

        VERTIX_ENGINE_API void InitializeDevice(
            GameInput::v3::IGameInput* gameInput,
            const Microsoft::WRL::ComPtr<GameInput::v3::IGameInputDevice> &) override;
    };
}
