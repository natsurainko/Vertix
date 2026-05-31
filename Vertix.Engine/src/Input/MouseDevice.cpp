//
// Created by Natsurainko on 2026/1/25.
//

#include "Vertix.Engine/Input/MouseDevice.h"

using namespace GameInput::v3;
using Microsoft::WRL::ComPtr;

void Vertix::Engine::MouseDevice::InitializeDevice(
    IGameInput*                     gameInput,
    const ComPtr<IGameInputDevice> &gameInputDevice) {
    InputDevice::InitializeDevice(gameInput, gameInputDevice);

    ComPtr<IGameInputReading> reading;
    if (const HRESULT hr = input->GetCurrentReading(GameInputKindMouse, device.Get(), &reading);
        !(SUCCEEDED(hr)) || !reading)
        return;

    reading->GetMouseState(&currentMouseState);
}

void Vertix::Engine::MouseDevice::Update() {
    ComPtr<IGameInputReading> reading;
    if (const HRESULT hr = input->GetCurrentReading(GameInputKindMouse, device.Get(), &reading);
        !(SUCCEEDED(hr)) || !reading)
        return;

    previousMouseState = currentMouseState;
    if (!reading->GetMouseState(&currentMouseState)) return;

    delta.X = static_cast<int>(currentMouseState.positionX - previousMouseState.positionX);
    delta.Y = static_cast<int>(currentMouseState.positionY - previousMouseState.positionY);
}

Vertix::Engine::GeneralMouseDevice::GeneralMouseDevice(const GameInputInterface &inputInterface) {
    GeneralMouseDevice::InitializeDevice(inputInterface.GetInterface(), nullptr);
}

void Vertix::Engine::GeneralMouseDevice::InitializeDevice(
    IGameInput*                     gameInput,
    const ComPtr<IGameInputDevice> &comPtr) {
    MouseDevice::InitializeDevice(gameInput, nullptr);
}
