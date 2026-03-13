//
// Created by Natsurainko on 2025/12/23.
//

#ifndef VERTIX_GAMEAPPLICATION_H
#define VERTIX_GAMEAPPLICATION_H

#include "Windowing/GameWindow.h"
#include "VERTIX_EXPORT.h"

namespace Vertix {
    class VERTIX_API GameApplication {
    public:
        GameApplication(
            const HINSTANCE &hInstance,
            LPSTR lpCmdLine,
            int nCmdShow,
            GameWindow* gameWindow);

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