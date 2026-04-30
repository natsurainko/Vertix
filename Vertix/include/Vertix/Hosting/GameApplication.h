//
// Created by Natsurainko on 2025/12/23.
//

#ifndef VERTIX_GAMEAPPLICATION_H
#define VERTIX_GAMEAPPLICATION_H

#include "Vertix/VERTIX_EXPORT.h"
#include "Vertix/Graphics/GraphicsDevice.h"
#include "Vertix/Windowing/GameWindow.h"

namespace Vertix {
    class GameApplication {
    public:
        VERTIX_API GameApplication(
            const HINSTANCE &hInstance,
            LPSTR lpCmdLine,
            int nCmdShow,
            GameWindow* gameWindow,
            const GraphicsDeviceOptions &deviceOptions = {});

        VERTIX_API ~GameApplication();

        [[nodiscard]]
        VERTIX_API int Run() const;

    private:
        HINSTANCE hInstance;
        LPSTR lpCmdLine;
        int nCmdShow;

        GameWindow* gameWindow = nullptr;
        GraphicsDevice* graphicsDevice = nullptr;
    };
}

#endif //VERTIX_GAMEAPPLICATION_H
