//
// Created by Natsurainko on 2026/1/25.
//

#include "Input/KeyboardDevice.h"

#include "Exceptions/HResultException.h"

using namespace GameInput::v3;
using Microsoft::WRL::ComPtr;

Vertix::Engine::KeyboardDevice::KeyboardDevice() = default;

Vertix::Engine::KeyboardDevice::~KeyboardDevice() {
    if (initialized) {
        input->UnregisterCallback(callbackToken);
        delete[] activeKeysArray;
    }
}

void Vertix::Engine::KeyboardDevice::InitializeDevice(
    const ComPtr<IGameInput> &gameInput,
    const ComPtr<IGameInputDevice> &gameInputDevice)
{
    const GameInputDeviceInfo* deviceInfo = nullptr;
    ThrowIfFailed(gameInputDevice->GetDeviceInfo(&deviceInfo));
    maxSimultaneousKeys = deviceInfo->keyboardInfo->maxSimultaneousKeys;
    activeKeysArray = new GameInputKeyState[maxSimultaneousKeys];

    ThrowIfFailed(gameInput->RegisterReadingCallback(
        gameInputDevice.Get(),
        GameInputKindKeyboard,
        this,
        OnKeyboardReadingCallback,
        &callbackToken));

    InputDevice::InitializeDevice(gameInput, gameInputDevice);
}

bool Vertix::Engine::KeyboardDevice::IsKeyPressed(const UINT &virtualKey) const noexcept {
    for (UINT i = 0; i < activeKeyCount; i++)
        if (activeKeysArray[i].virtualKey == virtualKey)
            return true;

    return false;
}

void Vertix::Engine::KeyboardDevice::OnKeyboardReadingCallback(
    const GameInputCallbackToken,
    void *contextPtr,
    IGameInputReading *reading)
{
    const auto keyboard = static_cast<KeyboardDevice*>(contextPtr);
    keyboard->activeKeyCount = reading->GetKeyState(keyboard->maxSimultaneousKeys, keyboard->activeKeysArray);
}
