//
// Created by Natsurainko on 2026/1/17.
//

#ifndef VERTIX_MOUSEDEVICE_H
#define VERTIX_MOUSEDEVICE_H

#include <Vertix/Math/Vector2D.hpp>

#include "Vertix.Engine/VERTIX_ENGINE_EXPORT.h"
#include "Vertix.Engine/Input/InputDevice.hpp"

namespace Vertix::Engine {
    class MouseDevice : public InputDevice {
    public:
        VERTIX_ENGINE_API MouseDevice() = default;
        VERTIX_ENGINE_API ~MouseDevice() override = default;

        VERTIX_ENGINE_API void InitializeDevice(
            const Microsoft::WRL::ComPtr<GameInput::v3::IGameInput> &gameInput,
            const Microsoft::WRL::ComPtr<GameInput::v3::IGameInputDevice> &gameInputDevice) override;

        VERTIX_ENGINE_API void Update();

        [[nodiscard]]
        Vector2D<int> GetDeltaOffset() const noexcept {
            return delta;
        }

        [[nodiscard]]
        Vector2D<int> GetPosition() const noexcept {
            return {
                static_cast<int>(currentMouseState.absolutePositionX),
                static_cast<int>(currentMouseState.absolutePositionY)
            };
        }

        [[nodiscard]]
        bool IsButtonDown(const GameInput::v3::GameInputMouseButtons button) const noexcept {
            return (currentMouseState.buttons & button) != 0;
        }

        [[nodiscard]]
        bool IsButtonUp(const GameInput::v3::GameInputMouseButtons button) const noexcept {
            return (currentMouseState.buttons & button) == 0;
        }

        [[nodiscard]]
        bool WasButtonPressed(const GameInput::v3::GameInputMouseButtons button) const noexcept {
            return (previousMouseState.buttons & button) == 0 &&
                   (currentMouseState.buttons & button) != 0;
        }

        [[nodiscard]]
        bool WasButtonReleased(const GameInput::v3::GameInputMouseButtons button) const noexcept {
            return (previousMouseState.buttons & button) != 0 &&
                   (currentMouseState.buttons & button) == 0;
        }
    protected:
        Vector2D<int> delta;

        GameInput::v3::GameInputMouseState previousMouseState{};
        GameInput::v3::GameInputMouseState currentMouseState{};
    };
}

#endif //VERTIX_MOUSEDEVICE_H
