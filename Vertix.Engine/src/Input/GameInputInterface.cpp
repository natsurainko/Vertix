//
// Created by Natsurainko on 2026/1/25.
//

#include "Input/GameInputInterface.h"

#include <GameInput/GameInput.h>

#include "Exceptions/HResultException.h"
#include "Input/InputDevice.h"

using namespace GameInput::v3;

Vertix::Engine::GameInputInterface::GameInputInterface() {
    ThrowIfFailed(GameInputCreate(&gameInput));
}

std::vector<Microsoft::WRL::ComPtr<IGameInputDevice>> Vertix::Engine::GameInputInterface::
GetInputDevices(const GameInputKind &inputKind) const {
    std::vector<Microsoft::WRL::ComPtr<IGameInputDevice>> devices;

    GameInputCallbackToken token;
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

void Vertix::Engine::GameInputInterface::InitializeDevice(InputDevice &inputDevice,
    const Microsoft::WRL::ComPtr<IGameInputDevice> &gameInputDevice) const {
    inputDevice.InitializeDevice(gameInput, gameInputDevice);
}

void Vertix::Engine::GameInputInterface::OnGetInputDevicesEnumerated(
    GameInputCallbackToken callbackToken, void *context, IGameInputDevice *device,
    uint64_t timestamp, GameInputDeviceStatus currentStatus,
    GameInputDeviceStatus previousStatus) {
    const GetInputDevicesEnumeratorContext* enumeratorContext = static_cast<GetInputDevicesEnumeratorContext*>(context);
    const GameInputDeviceInfo* deviceInfo = nullptr;
    ThrowIfFailed(device->GetDeviceInfo(&deviceInfo));

    Microsoft::WRL::ComPtr<IGameInputDevice> inputDevice;
    ThrowIfFailed(enumeratorContext->gameInput->FindDeviceFromId(&deviceInfo->deviceId, &inputDevice));
    enumeratorContext->devices->push_back(inputDevice);
}
