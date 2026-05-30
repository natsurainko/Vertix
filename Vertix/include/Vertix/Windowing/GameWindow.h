//
// Created by Natsurainko on 2025/12/23.
//

#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <chrono>
#include <string>
#include <windows.h>

#include "Vertix/Graphics/Command/CommandAllocator.h"
#include "Vertix/Math/Vector2D.hpp"

namespace Vertix {
    enum class WindowState {
        Normal,
        Minimized,
        Maximized,
    };

    enum class WindowStartupLocation {
        Manual,
        CenterScreen
    };

    struct WindowOptions {
        using SwapChainDescFunc = DXGI_SWAP_CHAIN_DESC1 (*)();

        bool              swapChainEnableVSync       = true;
        SwapChainDescFunc swapChainDescFactoryMethod = nullptr;

        Vector2D<uint32_t>    windowSize            = { 800, 600 };
        std::wstring_view     windowTitle           = L"Vertix.GameWindow";
        WindowStartupLocation windowStartupLocation = WindowStartupLocation::CenterScreen;
    };

    class GraphicsDevice;
    class CommandList;
    class CommandQueue;
    class DispatcherQueue;
    class SwapChain;

    class GameWindow {
        WindowOptions windowOptions;

        Vector2D<UINT> windowSize;
        std::wstring   windowTitle;
        WindowState    windowState = WindowState::Normal;

        bool isDragging = false;
        bool isResizing = false;
        bool isFocusing = false;

        std::chrono::steady_clock::time_point lastTickTime;

        HWND hWnd = nullptr;

    protected:
        GraphicsDevice* graphicsDevice = nullptr;
        CommandQueue*   commandQueue   = nullptr;

        std::unique_ptr<SwapChain>       swapChain;
        std::unique_ptr<CommandList>     commandList;
        std::unique_ptr<DispatcherQueue> dispatcherQueue;

        std::vector<CommandAllocator> commandAllocators;

    public:
        VERTIX_API explicit GameWindow(const WindowOptions &options);
        virtual             ~GameWindow();

        VERTIX_API virtual void Run(WPARAM &result);

        [[nodiscard]] VERTIX_API LRESULT HitTest(const Vector2D<UINT> &point) const noexcept;
        [[nodiscard]] VERTIX_API LRESULT HitTest(const POINT &point) const noexcept;

        VERTIX_API void Show(int nCmdShow = SW_SHOW) const noexcept;
        VERTIX_API void SetWindowTitle(const std::wstring_view &title);
        VERTIX_API void SetCursorCenterWindow() const;

        [[nodiscard]] const HWND&           GetWindowHandle() const noexcept { return hWnd; }
        [[nodiscard]] const Vector2D<UINT>& GetWindowSize() const noexcept { return windowSize; }
        [[nodiscard]] const std::wstring&   GetWindowTitle() const noexcept { return windowTitle; }
        [[nodiscard]] WindowState           GetWindowState() const noexcept { return windowState; }

        [[nodiscard]] bool GetDraggingState() const noexcept { return isDragging; }
        [[nodiscard]] bool GetFocusingState() const noexcept { return isFocusing; }

        [[nodiscard]] SwapChain*       GetSwapChain() const noexcept { return swapChain.get(); }
        [[nodiscard]] DispatcherQueue* GetDispatcherQueue() const noexcept { return dispatcherQueue.get(); }
        [[nodiscard]] GraphicsDevice*  GetGraphicsDevice() const noexcept { return graphicsDevice; }

    protected:
        VERTIX_API virtual void OnTick();
        VERTIX_API virtual void OnInternalRender(double deltaTime);
        VERTIX_API virtual void OnInternalUpdate(double deltaTime);

        virtual void OnInitialize() {}
        virtual void OnDestroy() {}
        virtual void OnUpdate(double deltaTime) {}
        virtual void OnRender(double deltaTime) {}
        virtual void OnResizing(const Vector2D<UINT> &size) {}
        virtual void OnResized(const Vector2D<UINT> &size) {}
        virtual void OnFocusLost() {}
        virtual void OnFocusGot() {}
        virtual void OnDragEnter() {}
        virtual void OnDragExit() {}

        virtual LRESULT BeforeWndProc(HWND, UINT, WPARAM, LPARAM) { return 0; }

    private:
        void NativeInitialize(HINSTANCE hInstance);

        static LRESULT CALLBACK      WndProc(HWND, UINT, WPARAM, LPARAM);
        static void                  RegisterWindowClass(const HINSTANCE &hInstance);
        static DXGI_SWAP_CHAIN_DESC1 CreateSwapChainDesc();

    public:
        static constexpr std::wstring_view WindowClassName = L"Vertix_GameWindow";

        VERTIX_API static void ApplySystemThemeMode(const HWND &hwnd);
    };
}
