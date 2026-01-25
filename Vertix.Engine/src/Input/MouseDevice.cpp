//
// Created by Natsurainko on 2026/1/25.
//

#include "Input/MouseDevice.h"

using namespace GameInput::v3;
using Microsoft::WRL::ComPtr;

void Vertix::Engine::MouseDevice::InitializeDevice(const ComPtr<IGameInput> &gameInput,
    const ComPtr<IGameInputDevice> &gameInputDevice) {
    InputDevice::InitializeDevice(gameInput, gameInputDevice);

    ComPtr<IGameInputReading> reading;
    if (const HRESULT hr = input->GetCurrentReading(GameInputKindMouse, device.Get(), &reading);
        !(SUCCEEDED(hr)) || !reading) return;

    reading->GetMouseState(&currentMouseState);
    previousMouseState = currentMouseState;
}

void Vertix::Engine::MouseDevice::Update() {
    ComPtr<IGameInputReading> reading;
    if (const HRESULT hr = input->GetCurrentReading(GameInputKindMouse, device.Get(), &reading);
        !(SUCCEEDED(hr)) || !reading) return;

    previousMouseState = currentMouseState;
    if (!reading->GetMouseState(&currentMouseState)) return;

    delta = Vector2D(
        currentMouseState.positionX - previousMouseState.positionX,
        currentMouseState.positionY - previousMouseState.positionY
    );
}
