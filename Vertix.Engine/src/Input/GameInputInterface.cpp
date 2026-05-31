//
// Created by Natsurainko on 2026/1/25.
//

#include "Vertix.Engine/Input/GameInputInterface.h"

#include <GameInput/GameInput.h>

using namespace GameInput::v3;
using Microsoft::WRL::ComPtr;

struct GetInputDevicesEnumeratorContext {
    ComPtr<IGameInput>                     gameInput;
    std::vector<ComPtr<IGameInputDevice>>* devices;
};

Vertix::Engine::GameInputInterface::GameInputInterface() {
    ThrowIfFailed(GameInputCreate(&gameInput));
}

std::vector<ComPtr<IGameInputDevice>> Vertix::Engine::GameInputInterface::GetInputDevices(const GameInputKind &inputKind) const {
    std::vector<ComPtr<IGameInputDevice>> devices;

    GameInputCallbackToken           token;
    GetInputDevicesEnumeratorContext context { gameInput, &devices };
    if (SUCCEEDED(
        gameInput->RegisterDeviceCallback(
            nullptr,                      // Don't filter to events from a specific device
            inputKind,                    // Enumerate gamepads and keyboards
            GameInputDeviceAnyStatus,     // Any device status
            GameInputBlockingEnumeration, // Enumerate synchronously
            &context,
            OnGetInputDevicesEnumerated,
            &token)
    )) {
        gameInput->UnregisterCallback(token);
    }

    return devices;
}

void Vertix::Engine::GameInputInterface::OnGetInputDevicesEnumerated(
    GameInputCallbackToken callbackToken,
    void*                  context,
    IGameInputDevice*      device,
    uint64_t               timestamp,
    GameInputDeviceStatus  currentStatus,
    GameInputDeviceStatus  previousStatus) {
    const GetInputDevicesEnumeratorContext* enumeratorContext = static_cast<GetInputDevicesEnumeratorContext*>(context);
    const GameInputDeviceInfo*              deviceInfo        = nullptr;
    ThrowIfFailed(device->GetDeviceInfo(&deviceInfo));

    ComPtr<IGameInputDevice> inputDevice;
    ThrowIfFailed(enumeratorContext->gameInput->FindDeviceFromId(&deviceInfo->deviceId, &inputDevice));
    enumeratorContext->devices->push_back(inputDevice);
}
