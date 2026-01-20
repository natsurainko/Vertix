//
// Created by Natsurainko on 2025/12/23.
//

#include "Windowing/GameWindow.h"

#include "d3d12/d3dx12_core.h"
#include "Exceptions/HResultException.h"
#include "Graphics/FrameCommandList.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/SwapChain.h"

void Vertix::GameWindow::NativeInitialize(HINSTANCE _hInstance) {
    hInstance = _hInstance;
    lastTickTime = std::chrono::steady_clock::now();

    WNDCLASSEX windowClass = {};
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.lpszClassName = L"GameWindow";
    RegisterClassEx(&windowClass);

    const auto size = GetWindowSize();

    RECT windowRect = { 0, 0, static_cast<LONG>(size.X), static_cast<LONG>(size.Y) };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    m_hwnd = CreateWindow(
        windowClass.lpszClassName,
        GetWindowTitle().c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,
        nullptr,
        hInstance,
        this);

    InitializeSwapChain();
}

void Vertix::GameWindow::InitializeDevice(GraphicsDevice *device) {
    graphicsDevice = device;
    frameCommandList = new FrameCommandList(graphicsDevice, frameCount);
}

void Vertix::GameWindow::InitializeSwapChain() {
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};

    swapChainDesc.Width = this->windowSize.X;
    swapChainDesc.Height = this->windowSize.Y;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.Stereo = false;

    swapChainDesc.SampleDesc = DXGI_SAMPLE_DESC(1, 0);

    swapChainDesc.Scaling = DXGI_SCALING_NONE;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = frameCount;

    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    swapChainDesc.Flags = 0;

    swapChain = new SwapChain(graphicsDevice, m_hwnd, swapChainDesc);
    ThrowIfFailed(graphicsDevice->GetDxgiFactory()->MakeWindowAssociation(m_hwnd, DXGI_MWA_NO_ALT_ENTER));
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

LRESULT Vertix::GameWindow::HitTest(const Vector2D<UINT> &point) const {
    return SendMessage(m_hwnd, WM_NCHITTEST, 0, MAKELPARAM(point.X, point.Y));
}

void Vertix::GameWindow::Show(const int nCmdShow) const {
    ShowWindow(m_hwnd, nCmdShow);
    UpdateWindow(m_hwnd);
}

Vertix::Vector2D<UINT> Vertix::GameWindow::GetWindowSize() const {
    return windowSize;
}

std::wstring Vertix::GameWindow::GetWindowTitle() const {
    return windowTitle;
}

HWND Vertix::GameWindow::GetWindowHandle() const {
    return m_hwnd;
}

bool Vertix::GameWindow::GetDraggingState() const {
    return isDraggingWindow;
}

bool Vertix::GameWindow::GetFocusingState() const {
    return isFocused;
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

void Vertix::GameWindow::GetD3D12ViewportAndScissorRect(CD3DX12_VIEWPORT &viewport, CD3DX12_RECT &scissorRect) const {
    const auto size = windowSize;

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<float>(size.X);
    viewport.Height = static_cast<float>(size.Y);

    scissorRect.left = 0;
    scissorRect.top = 0;
    scissorRect.right = static_cast<LONG>(size.X);
    scissorRect.bottom = static_cast<LONG>(size.Y);
}

Vertix::GameWindow::~GameWindow() {
    frameCommandList->WaitAllFrames();
    delete frameCommandList;
    delete swapChain;
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

LRESULT Vertix::GameWindow::WindowProc(const HWND hWnd, const UINT message, const WPARAM wParam, const LPARAM lParam) {
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
                if (const auto newSize = Vector2D<UINT>(LOWORD(lParam), HIWORD(lParam)); newSize != gameWindow->windowSize) {
                    gameWindow->windowSize = newSize;
                    gameWindow->OnResized(gameWindow->windowSize);
                }
            }
            return 0;

        case WM_NCLBUTTONDOWN:
            if (gameWindow) {
                gameWindow->isDraggingWindow = true;
                gameWindow->OnDragEnter();
            }
            return DefWindowProc(hWnd, message, wParam, lParam);

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

        case WM_CREATE: {
            const auto pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
        }
            return 0;

        case WM_DESTROY:
            if (gameWindow->frameCommandList) {
                gameWindow->frameCommandList->WaitAllFrames();
            }

            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}
