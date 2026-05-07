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

#include "Vertix/VERTIX_EXPORT.h"
#include "Vertix/Dispatching/DispatcherQueue.hpp"
#include "Vertix/Math/Vector2D.hpp"

namespace Vertix {
    struct WindowOptions {
        UINT swapChainFrameCount = 2;
        DXGI_FORMAT swapChainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        Vector2D<UINT> windowSize { 800, 600 };
        std::wstring windowTitle = L"Vertix.GameWindow";
        std::wstring windowClassName = L"Vertix_GameWindow";
        bool enableVSync = true;
    };

    enum WindowState {
        Normal,
        Minimized,
        Maximized,
    };

    struct KeyboardEventArgs;
    class GraphicsDevice;
    class FrameCommandList;
    class SwapChain;
    class GameWindow {
    public:
        VERTIX_API GameWindow();
        VERTIX_API explicit GameWindow(const WindowOptions &options);
        VERTIX_API virtual ~GameWindow();

        VERTIX_API void NativeInitialize(const HINSTANCE &hInstance);
        VERTIX_API void InitializeDevice(GraphicsDevice* device);

        VERTIX_API virtual void RunMessageLoop(WPARAM &result);

        virtual void OnInitialize() {}
        virtual void OnDestroy() {}

        [[nodiscard]]
        VERTIX_API LRESULT HitTest(const Vector2D<UINT> &point) const;

        [[nodiscard]]
        VERTIX_API LRESULT HitTest(const POINT &point) const;

        VERTIX_API void Show(int nCmdShow = SW_SHOW) const;

        [[nodiscard]]
        const Vector2D<UINT>& GetWindowSize() const noexcept {
            return windowSize;
        }

        [[nodiscard]]
        const std::wstring& GetWindowTitle() const noexcept {
            return windowTitle;
        }

        [[nodiscard]]
        const HWND& GetWindowHandle() const noexcept {
            return m_hwnd;
        }

        [[nodiscard]]
        bool GetDraggingState() const noexcept {
            return isDraggingWindow;
        }

        [[nodiscard]]
        bool GetFocusingState() const noexcept {
            return isFocused;
        }

        [[nodiscard]]
        WindowState GetWindowState() const noexcept {
            return windowState;
        }

        [[nodiscard]]
        SwapChain* GetSwapChain() const noexcept {
            return swapChain;
        }

        void GetD3D12ViewportRectSize(CD3DX12_VIEWPORT &viewport, CD3DX12_RECT &scissorRect) const noexcept {
            viewport.Width = static_cast<float>(windowSize.X);
            viewport.Height = static_cast<float>(windowSize.Y);
            scissorRect.right = static_cast<LONG>(windowSize.X);
            scissorRect.bottom = static_cast<LONG>(windowSize.Y);
        }

        VERTIX_API void SetWindowTitle(const std::wstring& title);
        VERTIX_API void SetCursorCenterWindow() const;

    protected:
        GraphicsDevice* graphicsDevice = nullptr;
        FrameCommandList* frameCommandList = nullptr;
        DispatcherQueue dispatcherQueue;
        SwapChain* swapChain = nullptr;

        VERTIX_API virtual void InitializeSwapChain();

        VERTIX_API virtual void OnTick();
        VERTIX_API virtual void OnInternalRender(double deltaTime);
        VERTIX_API virtual void OnInternalUpdate(double deltaTime);
        virtual void OnUpdate(double deltaTime) {}
        virtual void OnRender(double deltaTime) {}

        virtual void OnResizing(const Vector2D<UINT> &size) {}
        virtual void OnResized(const Vector2D<UINT> &size) {}

        virtual void OnFocusLost() {}
        virtual void OnFocusGot() {}

        virtual void OnDragEnter() {}
        virtual void OnDragExit() {}

        virtual LRESULT BeforeWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) { return 0; }

    private:
        WindowOptions windowOptions{};
        Vector2D<UINT> windowSize;
        std::wstring windowTitle;

        bool isDraggingWindow = false;
        bool isFocused = false;
        WindowState windowState = Normal;

        std::chrono::steady_clock::time_point lastTickTime;

        HWND m_hwnd = nullptr;

        static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
        static void RegisterDefaultWindowClass(const HINSTANCE &hInstance);

    public:
        VERTIX_API static void ApplySystemThemeMode(const HWND &hwnd);
    };
}

#endif //VERTIX_GAMEWINDOW_H
