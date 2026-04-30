//
// Created by Natsurainko on 2026/1/17.
//

#ifndef VERTIX_GAMEINPUTDEVICE_H
#define VERTIX_GAMEINPUTDEVICE_H

#include <vector>
#include <wrl/client.h>
#include <GameInput/GameInput.h>

#include "Vertix.Engine/VERTIX_ENGINE_EXPORT.h"

namespace Vertix::Engine {
    class InputDevice;
    class GameInputInterface {
    public:
        VERTIX_ENGINE_API GameInputInterface();

        [[nodiscard]]
        VERTIX_ENGINE_API std::vector<Microsoft::WRL::ComPtr<GameInput::v3::IGameInputDevice>> GetInputDevices(const GameInput::v3::GameInputKind &inputKind) const;

        void InitializeDevice(
            InputDevice &inputDevice,
            const Microsoft::WRL::ComPtr<GameInput::v3::IGameInputDevice> &gameInputDevice) const;

        [[nodiscard]]
        const Microsoft::WRL::ComPtr<GameInput::v3::IGameInput>& GetInterface() const noexcept {
            return gameInput;
        }

    private:
        Microsoft::WRL::ComPtr<GameInput::v3::IGameInput> gameInput;

        static void CALLBACK OnGetInputDevicesEnumerated(
            _In_ GameInput::v3::GameInputCallbackToken callbackToken,
            _In_ void* context,
            _In_ GameInput::v3::IGameInputDevice* device,
            _In_ uint64_t timestamp,
            _In_ GameInput::v3::GameInputDeviceStatus currentStatus,
            _In_ GameInput::v3::GameInputDeviceStatus previousStatus);
    };
}

#endif //VERTIX_GAMEINPUTDEVICE_H
