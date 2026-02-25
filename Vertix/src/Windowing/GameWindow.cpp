//
// Created by Natsurainko on 2025/12/23.
//

#include <utility>

#include "Windowing/GameWindow.h"

#include <dwmapi.h>

#include "Exceptions/HResultException.h"
#include "Graphics/FrameCommandList.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/SwapChain.h"

Vertix::GameWindow::GameWindow() : windowOptions(WindowOptions::GetDefaultWindowOptions()) {
    windowSize = windowOptions.windowSize;
    windowTitle = windowOptions.windowTitle;
}

Vertix::GameWindow::GameWindow(const WindowOptions &options) {
    windowOptions = options;
    windowSize = windowOptions.windowSize;
    windowTitle = windowOptions.windowTitle;
}

Vertix::GameWindow::~GameWindow() {
    if (frameCommandList) {
        frameCommandList->WaitAllFrames();
        delete frameCommandList;
        frameCommandList = nullptr;
    }

    if (swapChain) {
        delete swapChain;
        swapChain = nullptr;
    }
}

void Vertix::GameWindow::NativeInitialize(const HINSTANCE &hInstance) {
    lastTickTime = std::chrono::steady_clock::now();

    RECT windowRect{};
    windowRect.right = static_cast<LONG>(windowOptions.windowSize.X);
    windowRect.bottom = static_cast<LONG>(windowOptions.windowSize.Y);

    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
    RegisterDefaultWindowClass(hInstance);

    m_hwnd = CreateWindow(
        windowOptions.windowClassName.c_str(),
        windowOptions.windowTitle.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,
        nullptr,
        hInstance,
        this);

    ApplySystemThemeMode(m_hwnd);

    InitializeSwapChain();
}

void Vertix::GameWindow::InitializeDevice(GraphicsDevice *device) {
    graphicsDevice = device;
    ThrowIfFailed(graphicsDevice->GetDxgiFactory()->MakeWindowAssociation(m_hwnd, DXGI_MWA_NO_ALT_ENTER));
    frameCommandList = new FrameCommandList(graphicsDevice, windowOptions.swapChainFrameCount);
}

void Vertix::GameWindow::InitializeSwapChain() {
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
    swapChainDesc.Width = this->windowSize.X;
    swapChainDesc.Height = this->windowSize.Y;
    swapChainDesc.Format = windowOptions.swapChainFormat;
    swapChainDesc.Stereo = false;

    swapChainDesc.SampleDesc = DXGI_SAMPLE_DESC(1, 0);

    swapChainDesc.Scaling = DXGI_SCALING_NONE;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = windowOptions.swapChainFrameCount;

    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = windowOptions.renderTargetFormat;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

    swapChain = new SwapChain(graphicsDevice, m_hwnd, swapChainDesc, &rtvDesc);
    swapChain->SetEnableVSync(windowOptions.enableVSync);
}

void Vertix::GameWindow::RunMessageLoop(WPARAM &result) {
    MSG msg = {};

    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            OnTick();
        }
    }

    result = msg.wParam;
}

LRESULT Vertix::GameWindow::HitTest(const Vector2D<UINT> &point) const {
    return SendMessage(m_hwnd, WM_NCHITTEST, 0, MAKELPARAM(point.X, point.Y));
}

LRESULT Vertix::GameWindow::HitTest(const POINT &point) const {
    return SendMessage(m_hwnd, WM_NCHITTEST, 0, MAKELPARAM(point.x, point.y));
}

void Vertix::GameWindow::Show(const int nCmdShow) const {
    ShowWindow(m_hwnd, nCmdShow);
    UpdateWindow(m_hwnd);
}

void Vertix::GameWindow::OnTick() {
    const auto currentTime = std::chrono::steady_clock::now();
    const double deltaTime = std::chrono::duration<double>(currentTime - lastTickTime).count();
    lastTickTime = currentTime;

    OnInternalUpdate(deltaTime);
    OnInternalRender(deltaTime);
}

void Vertix::GameWindow::OnInternalUpdate(const double deltaTime) {
    frameCommandList->WaitPreviousFrame();
    OnUpdate(deltaTime);
}

void Vertix::GameWindow::OnInternalRender(const double deltaTime) {
    frameCommandList->BeginCommand(nullptr);
    OnRender(deltaTime);
    frameCommandList->EndCommand();
    swapChain->PresentFrame();
    frameCommandList->MoveToNextFrame();
}

void Vertix::GameWindow::SetWindowTitle(const std::wstring &title) {
    windowTitle = title;

    if (m_hwnd) {
        SetWindowText(m_hwnd, title.c_str());
    }
}

void Vertix::GameWindow::SetCursorCenterWindow() const {
    RECT rect;
    GetClientRect(m_hwnd, &rect);

    POINT center;
    center.x = rect.right / 2;
    center.y = rect.bottom / 2;

    ClientToScreen(m_hwnd, &center);
    SetCursorPos(center.x, center.y);
}

LRESULT Vertix::GameWindow::WindowProc(const HWND hWnd,
                                       const UINT message,
                                       const WPARAM wParam,
                                       const LPARAM lParam) {
    auto* gameWindow = reinterpret_cast<GameWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (message) {
        case WM_PAINT:
            if (gameWindow) {
                gameWindow->OnTick();

                PAINTSTRUCT ps;
                BeginPaint(hWnd, &ps);
                EndPaint(hWnd, &ps);
            }
            return 0;

        case WM_SIZING:
            if (gameWindow) {
                const auto newSize = Vector2D<UINT>(LOWORD(lParam), HIWORD(lParam));
                gameWindow->OnResizing(newSize);
            }
            return 0;

        case WM_SIZE:
            if (gameWindow) {
                if (wParam != SIZE_MINIMIZED) {
                    if (const auto newSize = Vector2D<UINT>(LOWORD(lParam), HIWORD(lParam)); newSize != gameWindow->windowSize) {
                        gameWindow->windowSize = newSize;
                        gameWindow->OnResized(gameWindow->windowSize);
                    }
                }
            }
            return 0;

        case WM_ENTERSIZEMOVE:
            if (gameWindow) {
                gameWindow->isDraggingWindow = true;
                gameWindow->OnDragEnter();
            }
            return 0;

        case WM_EXITSIZEMOVE:
            if (gameWindow) {
                if (gameWindow->isDraggingWindow) {
                    gameWindow->isDraggingWindow = false;
                    gameWindow->OnDragExit();
                }
            }
            return 0;

        case WM_SETFOCUS:
            if (gameWindow) {
                gameWindow->isFocused = true;
                gameWindow->OnFocusGot();
            }
            return 0;

        case WM_KILLFOCUS:
            if (gameWindow) {
                gameWindow->isFocused = false;
                gameWindow->OnFocusLost();
            }
            return 0;

        case WM_SETTINGCHANGE:
            if (gameWindow) {
                ApplySystemThemeMode(gameWindow->m_hwnd);
            }
            return 0;

        case WM_CREATE: {
            const auto pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
            return 0;
        }

        case WM_DESTROY: {
            if (gameWindow->frameCommandList)
                gameWindow->frameCommandList->WaitAllFrames();

            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

void Vertix::GameWindow::RegisterDefaultWindowClass(const HINSTANCE &hInstance) {
    static bool defaultWindowClassRegistered = false;
    if (defaultWindowClassRegistered) return;

    WNDCLASSEX windowClass = {};
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.lpszClassName = L"Vertix_GameWindow";
    RegisterClassEx(&windowClass);

    defaultWindowClassRegistered = true;
}

void Vertix::GameWindow::ApplySystemThemeMode(const HWND &hwnd) {
    static int Support_DWMWA_USE_IMMERSIVE_DARK_MODE = -1;
    static BOOL use_immersive_dark_mode = false;

    if (Support_DWMWA_USE_IMMERSIVE_DARK_MODE == 0) return;

    {
        DWORD value = 0;
        DWORD size = sizeof(value);

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