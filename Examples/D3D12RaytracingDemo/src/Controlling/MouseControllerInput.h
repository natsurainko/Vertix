//
// Created by Natsurainko on 2026/2/3.
//

#ifndef VERTIX_MOUSECONTROLLERINPUT_H
#define VERTIX_MOUSECONTROLLERINPUT_H

#include <DirectXTK12/SimpleMath.h>

#include "Vertix.Engine/Controlling/IControllerInput.hpp"
#include "Vertix/Windowing/GameWindow.h"

#ifndef USING_GAMEINPUT
// Win32 Implementation

class MouseControllerInput : public Vertix::Engine::IControllerInput<DirectX::SimpleMath::Vector3> {
public:
    explicit MouseControllerInput(Vertix::GameWindow* window) : window(window), hwnd(window->GetWindowHandle()) {}

    void Update(const double deltaTime) {
        if (window->GetDraggingState()) return;

        POINT currentPos;
        GetCursorPos(&currentPos);

        if (window->HitTest(currentPos) == HTCLIENT) {
            if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
                rightButtonPressed = true;
            } else if (rightButtonPressed && EnableRotating) {
                rightButtonPressed = false;
                EnableRotating = false;
                ShowCursor(true);
                ClipCursor(nullptr);
            }

            if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
                leftButtonPressed = true;
            } else if (leftButtonPressed && !EnableRotating) {
                leftButtonPressed = false;
                EnableRotating = true;

                ShowCursor(false);

                RECT rect;
                GetClientRect(hwnd, &rect);

                // Set cursor center on window
                {
                    POINT center;
                    center.x = rect.right / 2;
                    center.y = rect.bottom / 2;

                    ClientToScreen(hwnd, &center);
                    SetCursorPos(center.x, center.y);
                }
                // Clip cursor in window rect
                {
                    MapWindowPoints(hwnd, nullptr, reinterpret_cast<POINT *>(&rect), 2);
                    ClipCursor(&rect);
                }

                return;
            }
        }

        if (EnableRotating) {
            RECT rect;
            GetClientRect(hwnd, &rect);

            POINT center{ rect.right / 2, rect.bottom / 2 };
            ClientToScreen(hwnd, &center);

            const auto dx = static_cast<float>(currentPos.x - center.x);
            const auto dy = static_cast<float>(currentPos.y - center.y);

            rotatingOffset = DirectX::SimpleMath::Vector3{ -dy, -dx, 0.0f };
            onUpdate(deltaTime);
            SetCursorPos(center.x, center.y);
        }

        rotatingOffset = DirectX::SimpleMath::Vector3::Zero;
    }

    [[nodiscard]]
    const DirectX::SimpleMath::Vector3& GetValue() const override {
        return rotatingOffset;
    }

    bool EnableRotating = false;

private:
    Vertix::GameWindow* window;
    const HWND &hwnd;
    DirectX::SimpleMath::Vector3 rotatingOffset = DirectX::SimpleMath::Vector3::Zero;

    bool leftButtonPressed = false;
    bool rightButtonPressed = false;
};

#else
// GameInput Implementation

#include "Input/GameInputInterface.h"
#include "Input/GeneralMouseDevice.hpp"

class MouseControllerInput : public Vertix::Engine::IControllerInput<DirectX::SimpleMath::Vector3> {
public:
    explicit MouseControllerInput(
        const Vertix::Engine::GameInputInterface &inputInterface,
        Vertix::GameWindow* window) : window(window), mouseDevice(inputInterface) {}

    void Update(const double deltaTime) {
        if (mouseDevice.IsInitialized()) {
            mouseDevice.Update();

            if (mouseDevice.WasButtonReleased(GameInput::v3::GameInputMouseRightButton)
                && window->HitTest(mouseDevice.GetPosition().Cast<UINT>()) == HTCLIENT
                && EnableRotating) {
                ShowCursor(true);
                EnableRotating = false;
            }
            if (mouseDevice.WasButtonReleased(GameInput::v3::GameInputMouseLeftButton)
                && window->HitTest(mouseDevice.GetPosition().Cast<UINT>()) == HTCLIENT
                && !EnableRotating) {
                ShowCursor(false);
                window->SetCursorCenterWindow();
                EnableRotating = true;
            }

            if (EnableRotating) {
                const Vertix::Vector2D<float> mouseDeltaOffset = -mouseDevice.GetDeltaOffset().Cast<float>();
                rotatingOffset = DirectX::SimpleMath::Vector3{ mouseDeltaOffset.Y, mouseDeltaOffset.X, 0.0f };
                onUpdate(deltaTime);
                window->SetCursorCenterWindow();
            }
        }

        rotatingOffset = DirectX::SimpleMath::Vector3::Zero;
    }

    [[nodiscard]]
    const DirectX::SimpleMath::Vector3& GetValue() const override {
        return rotatingOffset;
    }

    bool EnableRotating = false;

private:
    Vertix::GameWindow* window;
    Vertix::Engine::GeneralMouseDevice mouseDevice;
    DirectX::SimpleMath::Vector3 rotatingOffset = DirectX::SimpleMath::Vector3::Zero;
};

#endif

#endif //VERTIX_MOUSECONTROLLERINPUT_H
