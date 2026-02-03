//
// Created by Natsurainko on 2025/12/23.
//

#include "Hosting/GameApplication.h"

#include "Graphics/GraphicsDevice.h"
#include "Windowing/GameWindow.h"

namespace Vertix {
    GameApplication::GameApplication(const HINSTANCE &hInstance, const LPSTR lpCmdLine, const int nCmdShow,
        GameWindow *gameWindow): hInstance(hInstance), lpCmdLine(lpCmdLine), nCmdShow(nCmdShow),
            gameWindow(gameWindow) {
        graphicsDevice = new GraphicsDevice();
        gameWindow->InitializeDevice(graphicsDevice);
    }

    int GameApplication::Run() const {
        gameWindow->NativeInitialize(hInstance);
        gameWindow->OnInitialize();
        gameWindow->Show(nCmdShow);

        WPARAM result;
        gameWindow->RunMessageLoop(result);
        gameWindow->OnDestroy();

        return static_cast<char>(result);
    }

    GameApplication::~GameApplication() {
        delete gameWindow;
        delete graphicsDevice;
    }
}


