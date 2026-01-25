//
// Created by Natsurainko on 2026/1/17.
//

#ifndef VERTIX_GAMEINPUTDEVICE_H
#define VERTIX_GAMEINPUTDEVICE_H

#include <vector>
#include <wrl/client.h>
#include <GameInput/GameInput.h>

struct GetInputDevicesEnumeratorContext {
    Microsoft::WRL::ComPtr<GameInput::v3::IGameInput> gameInput;
    std::vector<Microsoft::WRL::ComPtr<GameInput::v3::IGameInputDevice>>* devices;
};

namespace Vertix::Engine {
    class InputDevice;
    class GameInputInterface {
    public:
        GameInputInterface();

        [[nodiscard]]
        std::vector<Microsoft::WRL::ComPtr<GameInput::v3::IGameInputDevice>> GetInputDevices(const GameInput::v3::GameInputKind &inputKind) const;

        void InitializeDevice(InputDevice &inputDevice, const Microsoft::WRL::ComPtr<GameInput::v3::IGameInputDevice> &gameInputDevice) const;

        [[nodiscard]]
        Microsoft::WRL::ComPtr<GameInput::v3::IGameInput> GetInterface() const {
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