//
// Created by Natsurainko on 2026/1/17.
//

#pragma once

#include <vector>
#include <GameInput/GameInput.h>
#include <wrl/client.h>

namespace Vertix::Engine {
    class InputDevice;

    class GameInputInterface {
        Microsoft::WRL::ComPtr<GameInput::v3::IGameInput> gameInput;

    public:
        VERTIX_ENGINE_API GameInputInterface();

        [[nodiscard]] VERTIX_ENGINE_API std::vector<Microsoft::WRL::ComPtr<GameInput::v3::IGameInputDevice>> GetInputDevices(const GameInput::v3::GameInputKind &inputKind) const;

        [[nodiscard]] GameInput::v3::IGameInput* GetInterface() const noexcept { return gameInput.Get(); }

    private:
        static void CALLBACK OnGetInputDevicesEnumerated(
            _In_ GameInput::v3::GameInputCallbackToken callbackToken,
            _In_ void*                                 context,
            _In_ GameInput::v3::IGameInputDevice*      device,
            _In_ uint64_t                              timestamp,
            _In_ GameInput::v3::GameInputDeviceStatus  currentStatus,
            _In_ GameInput::v3::GameInputDeviceStatus  previousStatus);
    };
}
