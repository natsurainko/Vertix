//
// Created by Natsurainko on 2026/1/17.
//

#ifndef VERTIX_GAMEINPUTDEVICE_H
#define VERTIX_GAMEINPUTDEVICE_H

#include <vector>
#include <wrl/client.h>

#include "InputDevice.h"
#include "Exceptions/HResultException.h"
#include "GameInput/GameInput.h"

struct GetInputDevicesEnumeratorContext {
    Microsoft::WRL::ComPtr<GameInput::v3::IGameInput> gameInput;
    std::vector<Microsoft::WRL::ComPtr<GameInput::v3::IGameInputDevice>>* devices;
};

struct FindFirstActiveDeviceEnumeratorContext {
    std::unordered_map<GameInput::v3::GameInputKind, Vertix::Engine::InputDevice*> devices;
    Microsoft::WRL::ComPtr<GameInput::v3::IGameInput> gameInput;
    GameInput::v3::GameInputCallbackToken callbackToken{};
};

namespace Vertix::Engine {
    class GameInputInterface {
    public:
        GameInputInterface() {
            ThrowIfFailed(GameInput::v3::GameInputCreate(&gameInput));
        }

        [[nodiscard]] std::vector<Microsoft::WRL::ComPtr<GameInput::v3::IGameInputDevice>> GetInputDevices(const GameInput::v3::GameInputKind &inputKind) const {
            std::vector<Microsoft::WRL::ComPtr<GameInput::v3::IGameInputDevice>> devices;

            GameInput::v3::GameInputCallbackToken token;
            GetInputDevicesEnumeratorContext context{gameInput, &devices};
            if (SUCCEEDED(gameInput->RegisterDeviceCallback(
                nullptr,                                          // Don't filter to events from a specific device
                inputKind,                                        // Enumerate gamepads and keyboards
                GameInput::v3::GameInputDeviceAnyStatus,          // Any device status
                GameInput::v3::GameInputBlockingEnumeration,      // Enumerate synchronously
                &context,
                OnGetInputDevicesEnumerated,
                &token))) {
                gameInput->UnregisterCallback(token);
            }

            return devices;
        }

        void InitializeDevice(InputDevice &inputDevice, const Microsoft::WRL::ComPtr<GameInput::v3::IGameInputDevice> &gameInputDevice) const {
            inputDevice.InitializeDevice(gameInput, gameInputDevice);
        }

        /*void RegisterCallbackToFindDevices(const std::unordered_map<GameInput::v3::GameInputKind, InputDevice*> &devices) const {
            auto* context = new FindFirstActiveDeviceEnumeratorContext();
            context->devices = devices;
            context->gameInput = gameInput;
            context->callbackToken = {};

            GameInput::v3::GameInputKind kinds = GameInput::v3::GameInputKindUnknown;

            for (const auto kind: context->devices | std::views::keys) {
                kinds = kinds | kind;
            }

            ThrowIfFailed(gameInput->RegisterReadingCallback(
               nullptr,
               kinds,
               context,
               OnFindFirstActiveDeviceEnumerated,
               &context->callbackToken));
        }*/

        [[nodiscard]] Microsoft::WRL::ComPtr<GameInput::v3::IGameInput> GetInterface() const {
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
            _In_ GameInput::v3::GameInputDeviceStatus previousStatus) {
            const GetInputDevicesEnumeratorContext* enumeratorContext = static_cast<GetInputDevicesEnumeratorContext*>(context);
            const GameInput::v3::GameInputDeviceInfo* deviceInfo = nullptr;
            ThrowIfFailed(device->GetDeviceInfo(&deviceInfo));

            Microsoft::WRL::ComPtr<GameInput::v3::IGameInputDevice> inputDevice;
            ThrowIfFailed(enumeratorContext->gameInput->FindDeviceFromId(&deviceInfo->deviceId, &inputDevice));
            enumeratorContext->devices->push_back(inputDevice);
        }

        /*static void CALLBACK OnFindFirstActiveDeviceEnumerated(
            _In_ const GameInput::v3::GameInputCallbackToken callbackToken,
            _In_ void* contextPtr,
            _In_ GameInput::v3::IGameInputReading* reading) {
            const auto context = static_cast<FindFirstActiveDeviceEnumeratorContext*>(contextPtr);

            bool leftDevice = false;
            for (const auto [kind, device] : context->devices) {
                if (!device->IsInitialized()) {
                    if (reading->GetInputKind() != kind) {
                        leftDevice = true;
                        continue;
                    }
                    Microsoft::WRL::ComPtr<GameInput::v3::IGameInputDevice> inputDevice;
                    reading->GetDevice(&inputDevice);
                    device->InitializeDevice(context->gameInput, inputDevice);
                }
            }

            if (!leftDevice) {
                context->gameInput->StopCallback(callbackToken);
                delete context;
            }
        }*/
    };
}

#endif //VERTIX_GAMEINPUTDEVICE_H