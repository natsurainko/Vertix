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
#include <d3d12/d3dx12_core.h>

#include "Math/Vector2D.h"
#include "Windowing/WindowOptions.h"

namespace Vertix {
    struct KeyboardEventArgs;
    class GraphicsDevice;
    class FrameCommandList;
    class SwapChain;
    class GameWindow {
    public:
        GameWindow();
        explicit GameWindow(const WindowOptions &options);
        virtual ~GameWindow();

        void NativeInitialize(const HINSTANCE &hInstance);
        void InitializeDevice(GraphicsDevice* device);

        virtual void RunMessageLoop(WPARAM &result);

        virtual void OnInitialize() {}
        virtual void OnDestroy() {}

        [[nodiscard]]
        LRESULT HitTest(const Vector2D<UINT> &point) const;

        [[nodiscard]]
        LRESULT HitTest(const POINT &point) const;

        void Show(int nCmdShow = SW_SHOW) const;

        [[nodiscard]]
        const Vector2D<UINT>& GetWindowSize() const {
            return windowSize;
        }

        [[nodiscard]]
        const std::wstring& GetWindowTitle() const {
            return windowTitle;
        }

        [[nodiscard]]
        HWND GetWindowHandle() const {
            return m_hwnd;
        }

        [[nodiscard]]
        bool GetDraggingState() const {
            return isDraggingWindow;
        }

        [[nodiscard]]
        bool GetFocusingState() const {
            return isFocused;
        }

        [[nodiscard]]
        SwapChain* GetSwapChain() const {
            return swapChain;
        }

        void GetD3D12ViewportRectSize(CD3DX12_VIEWPORT &viewport, CD3DX12_RECT &scissorRect) const {
            viewport.Width = static_cast<float>(windowSize.X);
            viewport.Height = static_cast<float>(windowSize.Y);
            scissorRect.right = static_cast<LONG>(windowSize.X);
            scissorRect.bottom = static_cast<LONG>(windowSize.Y);
        }

        void SetWindowTitle(const std::wstring& title);
        void SetCursorCenterWindow() const;

    protected:
        GraphicsDevice* graphicsDevice = nullptr;
        FrameCommandList* frameCommandList = nullptr;
        SwapChain* swapChain = nullptr;

        virtual void InitializeSwapChain();

        virtual void OnTick();
        virtual void OnInternalRender(double deltaTime);
        virtual void OnInternalUpdate(double deltaTime);
        virtual void OnUpdate(double deltaTime) {}
        virtual void OnRender(double deltaTime) {}

        virtual void OnResizing(const Vector2D<UINT> &size) {}
        virtual void OnResized(const Vector2D<UINT> &size) {}

        virtual void OnFocusLost() {}
        virtual void OnFocusGot() {}

        virtual void OnDragEnter() {}
        virtual void OnDragExit() {}

    private:
        WindowOptions windowOptions;
        Vector2D<UINT> windowSize;
        std::wstring windowTitle;

        bool isDraggingWindow = false;
        bool isFocused = false;

        std::chrono::steady_clock::time_point lastTickTime;

        HWND m_hwnd = nullptr;

        static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
        static void RegisterDefaultWindowClass(const HINSTANCE &hInstance);

    public:
        static void ApplySystemThemeMode(const HWND &hwnd);
    };
}

#endif //VERTIX_GAMEWINDOW_H