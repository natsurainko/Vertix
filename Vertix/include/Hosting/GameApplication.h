//
// Created by Natsurainko on 2025/12/23.
//

#ifndef VERTIX_GAMEAPPLICATION_H
#define VERTIX_GAMEAPPLICATION_H

#include "Hosting/GameApplication.h"
#include "Windowing/GameWindow.h"

namespace Vertix {
    class GameApplication {
    public:
        explicit GameApplication(
            HINSTANCE hInstance,
            HINSTANCE hPrevInstance,
            LPSTR lpCmdLine,
            int nCmdShow,
            GameWindow* gameWindow);

        [[nodiscard]] int Run() const;

        ~GameApplication();
    private:
        HINSTANCE hInstance;
        HINSTANCE hPrevInstance;
        LPSTR lpCmdLine;
        int nCmdShow;

        GameWindow* gameWindow = nullptr;
        GraphicsDevice* graphicsDevice = nullptr;
    };
}

#endif //VERTIX_GAMEAPPLICATION_H