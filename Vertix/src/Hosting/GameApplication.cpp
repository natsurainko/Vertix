//
// Created by Natsurainko on 2025/12/23.
//

#include "Vertix/Hosting/GameApplication.h"

#include "Vertix/Graphics/GraphicsDevice.h"
#include "Vertix/Windowing/GameWindow.h"

Vertix::GameApplication::GameApplication(
    const HINSTANCE &hInstance,
    const LPSTR lpCmdLine,
    const int nCmdShow,
    GameWindow *gameWindow,
    const GraphicsDeviceOptions &deviceOptions) : hInstance(hInstance), lpCmdLine(lpCmdLine), nCmdShow(nCmdShow), gameWindow(gameWindow)
{
    graphicsDevice = new GraphicsDevice(deviceOptions.useSoftware, deviceOptions.enableDebugLayer);
    gameWindow->InitializeDevice(graphicsDevice);
}

Vertix::GameApplication::~GameApplication() {
    delete gameWindow;
    delete graphicsDevice;
}

int Vertix::GameApplication::Run() const {
    gameWindow->NativeInitialize(hInstance);
    gameWindow->OnInitialize();
    gameWindow->Show(nCmdShow);

    WPARAM result;
    gameWindow->RunMessageLoop(result);
    gameWindow->OnDestroy();

    return static_cast<char>(result);
}
