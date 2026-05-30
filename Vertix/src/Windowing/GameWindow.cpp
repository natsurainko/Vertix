//
// Created by Natsurainko on 2025/12/23.
//

#include "Vertix/Windowing/GameWindow.h"

#include <dwmapi.h>

#include "Vertix/Dispatching/DispatcherQueue.h"
#include "Vertix/Graphics/GraphicsDevice.h"
#include "Vertix/Graphics/Command/CommandList.h"
#include "Vertix/Graphics/Command/CommandQueue.h"
#include "Vertix/Hosting/GameApplication.h"
#include "Vertix/Windowing/SwapChain.h"

Vertix::GameWindow::GameWindow(const WindowOptions &options) {
    windowOptions = options;
    windowSize    = windowOptions.windowSize;
    windowTitle   = windowOptions.windowTitle;

    const auto application = GameApplication::GetInstance();
    commandQueue           = application->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
    graphicsDevice         = application->GetGraphicsDevice();

    const HINSTANCE hinstance = GetModuleHandle(nullptr);
    RegisterWindowClass(hinstance);
    NativeInitialize(hinstance);
    ThrowIfFailed(graphicsDevice->GetDxgiFactory()->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

    if (!windowOptions.swapChainDescFactoryMethod)
        windowOptions.swapChainDescFactoryMethod = CreateSwapChainDesc;

    auto swapChainDesc   = windowOptions.swapChainDescFactoryMethod();
    swapChainDesc.Width  = windowOptions.windowSize.X;
    swapChainDesc.Height = windowOptions.windowSize.Y;
    swapChain            = graphicsDevice->CreateSwapChain(commandQueue, hWnd, swapChainDesc);

    const auto device     = graphicsDevice->GetD3D12Device();
    const auto frameCount = swapChain->GetFrameCount();

    commandAllocators.reserve(frameCount);
    for (uint32_t i = 0; i < frameCount; ++i) {
        commandAllocators.emplace_back(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
    }

#if VERTIX_D3D12_DEVICE_VERSION >= 5
    commandList = std::make_unique<CommandList>(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
#else
    commandList = std::make_unique<CommandList>(device, &commandAllocators[swapChain->GetCurrentFrameIndex()], D3D12_COMMAND_LIST_TYPE_DIRECT);
#endif
}

Vertix::GameWindow::~GameWindow() = default;

void Vertix::GameWindow::NativeInitialize(const HINSTANCE hInstance) {
    lastTickTime = std::chrono::steady_clock::now();

    RECT windowRect   = {};
    windowRect.right  = static_cast<LONG>(windowOptions.windowSize.X);
    windowRect.bottom = static_cast<LONG>(windowOptions.windowSize.Y);

    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    hWnd = CreateWindowExW(
        0L,
        WindowClassName.data(),
        windowOptions.windowTitle.data(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,
        nullptr,
        hInstance,
        this
    );

    if (windowOptions.windowStartupLocation == WindowStartupLocation::CenterScreen) {
        RECT workArea = {};
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0);

        const int width  = windowRect.right - windowRect.left;
        const int height = windowRect.bottom - windowRect.top;
        const int x      = workArea.left + (workArea.right - workArea.left - width) / 2;
        const int y      = workArea.top + (workArea.bottom - workArea.top - height) / 2;

        SetWindowPos(hWnd, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }

    ApplySystemThemeMode(hWnd);
}

void Vertix::GameWindow::Run(WPARAM &result) {
    OnInitialize();
    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        } else {
            OnTick();
        }
    }
    result = msg.wParam;
    OnDestroy();
}

LRESULT Vertix::GameWindow::HitTest(const Vector2D<UINT> &point) const noexcept {
    return SendMessageW(hWnd, WM_NCHITTEST, 0, MAKELPARAM(point.X, point.Y));
}

LRESULT Vertix::GameWindow::HitTest(const POINT &point) const noexcept {
    return SendMessageW(hWnd, WM_NCHITTEST, 0, MAKELPARAM(point.x, point.y));
}

void Vertix::GameWindow::Show(const int nCmdShow) const noexcept {
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
}

void Vertix::GameWindow::SetWindowTitle(const std::wstring_view &title) {
    windowTitle = title;
    SetWindowTextW(hWnd, title.data());
}

void Vertix::GameWindow::SetCursorCenterWindow() const {
    RECT rect;
    GetClientRect(hWnd, &rect);

    POINT center;
    center.x = rect.right / 2;
    center.y = rect.bottom / 2;

    ClientToScreen(hWnd, &center);
    SetCursorPos(center.x, center.y);
}

void Vertix::GameWindow::OnTick() {
    const auto   currentTime = std::chrono::steady_clock::now();
    const double deltaTime   = std::chrono::duration<double>(currentTime - lastTickTime).count();
    lastTickTime             = currentTime;

    OnInternalUpdate(deltaTime);
    OnInternalRender(deltaTime);
}

void Vertix::GameWindow::OnInternalUpdate(const double deltaTime) {
    OnUpdate(deltaTime);
    dispatcherQueue->FlushQueue();
}

void Vertix::GameWindow::OnInternalRender(const double deltaTime) {
    commandList->BeginCommand(&commandAllocators[swapChain->GetCurrentFrameIndex()]);
    OnRender(deltaTime);
    commandList->EndCommand();
    commandQueue->ExecuteCommandLists(commandList.get());
    swapChain->PresentFrame();
}

LRESULT Vertix::GameWindow::WndProc(
    const HWND   hWnd,
    const UINT   message,
    const WPARAM wParam,
    const LPARAM lParam) {
    auto* gameWindow = reinterpret_cast<GameWindow*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    if (gameWindow) {
        if (const LRESULT result = gameWindow->BeforeWndProc(hWnd, message, wParam, lParam); result) {
            return result;
        }
    }

    switch (message) {
        case WM_PAINT: if (gameWindow) {
                gameWindow->OnTick();

                PAINTSTRUCT ps;
                BeginPaint(hWnd, &ps);
                EndPaint(hWnd, &ps);
            }
            return 0;

        case WM_SIZING: if (gameWindow) {
                const auto newSize = Vector2D<UINT>(LOWORD(lParam), HIWORD(lParam));
                gameWindow->OnResizing(newSize);
            }
            return 0;

        case WM_SIZE: if (gameWindow) {
                switch (wParam) {
                    case SIZE_MINIMIZED: gameWindow->windowState = WindowState::Minimized;
                        return 0;
                    case SIZE_MAXIMIZED: gameWindow->windowState = WindowState::Maximized;
                        break;
                    default: gameWindow->windowState = WindowState::Normal;
                        break;
                }

                if (const auto newSize = Vector2D<UINT>(LOWORD(lParam), HIWORD(lParam)); newSize != gameWindow->windowSize) {
                    gameWindow->windowSize = newSize;
                    gameWindow->OnResized(gameWindow->windowSize);
                }
            }
            return 0;

        case WM_ENTERSIZEMOVE: if (gameWindow) {
                gameWindow->isDragging = true;
                gameWindow->OnDragEnter();
            }
            return 0;

        case WM_EXITSIZEMOVE: if (gameWindow) {
                if (gameWindow->isDragging) {
                    gameWindow->isDragging = false;
                    gameWindow->OnDragExit();
                }
            }
            return 0;

        case WM_SETFOCUS: if (gameWindow) {
                gameWindow->isFocusing = true;
                gameWindow->OnFocusGot();
            }
            return 0;

        case WM_KILLFOCUS: if (gameWindow) {
                gameWindow->isFocusing = false;
                gameWindow->OnFocusLost();
            }
            return 0;

        case WM_SETTINGCHANGE: if (gameWindow) {
                ApplySystemThemeMode(gameWindow->hWnd);
            }
            return 0;

        case WM_CREATE: {
            const auto pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
            return 0;
        }

        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProcW(hWnd, message, wParam, lParam);
}

void Vertix::GameWindow::RegisterWindowClass(const HINSTANCE &hInstance) {
    static bool defaultWindowClassRegistered = false;
    if (defaultWindowClassRegistered) return;

    WNDCLASSEXW windowClass   = {};
    windowClass.cbSize        = sizeof(WNDCLASSEXW);
    windowClass.style         = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc   = WndProc;
    windowClass.hInstance     = hInstance;
    windowClass.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    windowClass.lpszClassName = WindowClassName.data();
    RegisterClassExW(&windowClass);

    defaultWindowClassRegistered = true;
}

DXGI_SWAP_CHAIN_DESC1 Vertix::GameWindow::CreateSwapChainDesc() {
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Format                = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapChainDesc.Stereo                = false;
    swapChainDesc.SampleDesc            = DXGI_SAMPLE_DESC(1, 0);
    swapChainDesc.Scaling               = DXGI_SCALING_NONE;
    swapChainDesc.BufferUsage           = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount           = 2;
    swapChainDesc.SwapEffect            = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.AlphaMode             = DXGI_ALPHA_MODE_UNSPECIFIED;
    swapChainDesc.Flags                 = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    return swapChainDesc;
}

void Vertix::GameWindow::ApplySystemThemeMode(const HWND &hwnd) {
    static int  Support_DWMWA_USE_IMMERSIVE_DARK_MODE = -1;
    static BOOL use_immersive_dark_mode               = false;

    if (Support_DWMWA_USE_IMMERSIVE_DARK_MODE == 0) return;

    {
        DWORD value = 0;
        DWORD size  = sizeof(value);

        const LSTATUS result = RegGetValueW(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            L"AppsUseLightTheme",
            RRF_RT_REG_DWORD,
            nullptr,
            &value,
            &size
        );

        if (result == ERROR_SUCCESS) {
            if (use_immersive_dark_mode == (value == 0)) return;
            use_immersive_dark_mode = value == 0;
        }
    }

    {
        const HRESULT hr = DwmSetWindowAttribute(
            hwnd,
            DWMWA_USE_IMMERSIVE_DARK_MODE,
            &use_immersive_dark_mode,
            sizeof(use_immersive_dark_mode)
        );

        if (Support_DWMWA_USE_IMMERSIVE_DARK_MODE == -1) {
            Support_DWMWA_USE_IMMERSIVE_DARK_MODE = hr == E_INVALIDARG ? 0 : 1;
        }
    }
}
