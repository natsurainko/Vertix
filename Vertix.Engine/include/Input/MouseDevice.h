//
// Created by Natsurainko on 2026/1/17.
//

#ifndef VERTIX_MOUSEDEVICE_H
#define VERTIX_MOUSEDEVICE_H

#include "InputDevice.h"
#include "Math/Vector2D.h"

namespace Vertix::Engine {
    class MouseDevice : public InputDevice {
    public:
        MouseDevice() = default;
        ~MouseDevice() override = default;

        void InitializeDevice(const Microsoft::WRL::ComPtr<GameInput::v3::IGameInput> &gameInput,
                              const Microsoft::WRL::ComPtr<GameInput::v3::IGameInputDevice> &gameInputDevice) override;

        void Update();

        [[nodiscard]]
        Vector2D<int64_t> GetDeltaOffset() const {
            return delta;
        }

        [[nodiscard]]
        Vector2D<int64_t> GetPosition() const {
            return {currentMouseState.absolutePositionX, currentMouseState.absolutePositionY};
        }

        [[nodiscard]]
        bool IsButtonDown(const GameInput::v3::GameInputMouseButtons button) const {
            return (currentMouseState.buttons & button) != 0;
        }

        [[nodiscard]]
        bool IsButtonUp(const GameInput::v3::GameInputMouseButtons button) const {
            return (currentMouseState.buttons & button) == 0;
        }

        [[nodiscard]]
        bool WasButtonPressed(const GameInput::v3::GameInputMouseButtons button) const {
            return (previousMouseState.buttons & button) == 0 &&
                   (currentMouseState.buttons & button) != 0;
        }

        [[nodiscard]]
        bool WasButtonReleased(const GameInput::v3::GameInputMouseButtons button) const {
            return (previousMouseState.buttons & button) != 0 &&
                   (currentMouseState.buttons & button) == 0;
        }
    protected:
        Vector2D<int64_t> delta;

        GameInput::v3::GameInputMouseState previousMouseState{};
        GameInput::v3::GameInputMouseState currentMouseState{};
    };
}

#endif //VERTIX_MOUSEDEVICE_H