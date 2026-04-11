//
// Created by Natsurainko on 2026/1/27.
//

#ifndef D3D12_RENDER_PIPELINE_DEMO_KEYBOARD_CONTROLLER_INPUT_H
#define D3D12_RENDER_PIPELINE_DEMO_KEYBOARD_CONTROLLER_INPUT_H

#include <DirectXTK12/SimpleMath.h>

#include "Controlling/IControllerInput.hpp"

#ifndef USING_GAMEINPUT
// Win32 Implementation

class KeyboardControllerInput : public Vertix::Engine::IControllerInput<DirectX::SimpleMath::Vector3> {
public:
    void Update(const double deltaTime) {
        if (GetAsyncKeyState('W') & 0x8000) movingOffset.x += 1;
        if (GetAsyncKeyState('S') & 0x8000) movingOffset.x -= 1;
        if (GetAsyncKeyState(VK_SPACE) & 0x8000) movingOffset.y += 1;
        if (GetAsyncKeyState(VK_LSHIFT) & 0x8000) movingOffset.y -= 1;
        if (GetAsyncKeyState('D') & 0x8000) movingOffset.z += 1;
        if (GetAsyncKeyState('A') & 0x8000) movingOffset.z -= 1;

        onUpdate(deltaTime);
        movingOffset = DirectX::SimpleMath::Vector3::Zero;
    }

    [[nodiscard]]
    const DirectX::SimpleMath::Vector3& GetValue() const override {
        return movingOffset;
    }

private:
    DirectX::SimpleMath::Vector3 movingOffset = DirectX::SimpleMath::Vector3::Zero;
};

#else
// GameInput Implementation

#include "Input/GameInputInterface.h"
#include "Input/GeneralKeyboardDevice.hpp"

class KeyboardControllerInput : public Vertix::Engine::IControllerInput<DirectX::SimpleMath::Vector3> {
public:
    explicit KeyboardControllerInput(const Vertix::Engine::GameInputInterface &inputInterface)
        : keyboardDevice(inputInterface) {}

    void Update(const double deltaTime) {
        if (keyboardDevice.IsKeyPressed('W')) movingOffset.x += 1;
        if (keyboardDevice.IsKeyPressed('S')) movingOffset.x -= 1;
        if (keyboardDevice.IsKeyPressed(VK_SPACE)) movingOffset.y += 1;
        if (keyboardDevice.IsKeyPressed(VK_LSHIFT)) movingOffset.y -= 1;
        if (keyboardDevice.IsKeyPressed('D')) movingOffset.z += 1;
        if (keyboardDevice.IsKeyPressed('A')) movingOffset.z -= 1;

        onUpdate(deltaTime);
        movingOffset = DirectX::SimpleMath::Vector3::Zero;
    }

    [[nodiscard]]
    const DirectX::SimpleMath::Vector3& GetValue() const override {
        return movingOffset;
    }

private:
    Vertix::Engine::GeneralKeyboardDevice keyboardDevice;
    DirectX::SimpleMath::Vector3 movingOffset = DirectX::SimpleMath::Vector3::Zero;
};

#endif

#endif //D3D12_RENDER_PIPELINE_DEMO_KEYBOARD_CONTROLLER_INPUT_H