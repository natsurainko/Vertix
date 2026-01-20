//
// Created by Natsurainko on 2025/12/23.
//

#ifndef VERTIX_GAMEWINDOW_H
#define VERTIX_GAMEWINDOW_H

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <chrono>
#include <string>
#include <windows.h>

#include "Math/Vector2D.h"

struct CD3DX12_VIEWPORT;
struct CD3DX12_RECT;

namespace Vertix {
    struct KeyboardEventArgs;
    class GraphicsDevice;
    class FrameCommandList;
    class SwapChain;
    class GameWindow {
    public:
        virtual ~GameWindow();

        void NativeInitialize(HINSTANCE hInstance);
        void InitializeDevice(GraphicsDevice* device);

        virtual void RunMessageLoop(WPARAM &result);

        virtual void OnInitialize() {}
        virtual void OnDestroy() {}

        [[nodiscard]] LRESULT HitTest(const Vector2D<UINT> &point) const;
        void Show(int nCmdShow) const;

        [[nodiscard]] Vector2D<UINT> GetWindowSize() const;
        [[nodiscard]] std::wstring GetWindowTitle() const;
        [[nodiscard]] HWND GetWindowHandle() const;

        [[nodiscard]] bool GetDraggingState() const;
        [[nodiscard]] bool GetFocusingState() const;

        void GetD3D12ViewportAndScissorRect(CD3DX12_VIEWPORT &viewport, CD3DX12_RECT &scissorRect) const;

        void SetWindowTitle(const std::wstring& title);
        void SetCursorCenterWindow() const;

    protected:
        GraphicsDevice* graphicsDevice = nullptr;
        FrameCommandList* frameCommandList = nullptr;
        SwapChain* swapChain = nullptr;

        UINT frameCount = 2;

        virtual void InitializeSwapChain();

        virtual void OnTick();
        virtual void OnInternalRender(double deltaTime);
        virtual void OnInternalUpdate(double deltaTime);
        virtual void OnUpdate(double deltaTime) {}
        virtual void OnRender(double deltaTime) {}

        virtual void OnResizing(Vector2D<UINT> size) {}
        virtual void OnResized(Vector2D<UINT> size) {}

        virtual void OnFocusLost() {}
        virtual void OnFocusGot() {}

        virtual void OnDragEnter() {}
        virtual void OnDragExit() {}

    private:
        Vector2D<UINT> windowSize = Vector2D<UINT>(800, 600);
        std::wstring windowTitle = L"Vertix.GameWindow";

        bool isDraggingWindow = false;
        bool isFocused = false;

        HINSTANCE hInstance = nullptr;
        HWND m_hwnd = nullptr;
        std::chrono::steady_clock::time_point lastTickTime;

        static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    };
}

#endif //VERTIX_GAMEWINDOW_H