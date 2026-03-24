//
// Created by Natsurainko on 2025/12/23.
//

#ifndef VERTIX_GAMEAPPLICATION_H
#define VERTIX_GAMEAPPLICATION_H

#include "VERTIX_EXPORT.h"
#include "Graphics/GraphicsDevice.h"
#include "Windowing/GameWindow.h"

namespace Vertix {
    class VERTIX_API GameApplication {
    public:
        GameApplication(
            const HINSTANCE &hInstance,
            LPSTR lpCmdLine,
            int nCmdShow,
            GameWindow* gameWindow,
            const GraphicsDeviceOptions &deviceOptions = {});

        ~GameApplication();

        [[nodiscard]]
        int Run() const;

    private:
        HINSTANCE hInstance;
        LPSTR lpCmdLine;
        int nCmdShow;

        GameWindow* gameWindow = nullptr;
        GraphicsDevice* graphicsDevice = nullptr;
    };
}

#endif //VERTIX_GAMEAPPLICATION_H