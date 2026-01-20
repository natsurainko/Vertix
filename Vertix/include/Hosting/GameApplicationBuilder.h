//
// Created by Natsurainko on 2025/12/23.
//

#ifndef VERTIX_GAMEAPPLICATIONBUILDER_H
#define VERTIX_GAMEAPPLICATIONBUILDER_H

#include <stdexcept>

#include "GameApplication.h"

namespace Vertix {
    template<typename TWindow>
        concept GameWindowType =
            std::derived_from<TWindow, GameWindow>;

    class GameApplicationBuilder {
    public:
        GameApplicationBuilder(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
            : hInstance(hInstance), hPrevInstance(hPrevInstance), lpCmdLine(lpCmdLine), nCmdShow(nCmdShow) {
        }

        template <GameWindowType TWindow>
        void ConfigureWindow() {
            gameWindow = new TWindow();
        }

        [[nodiscard]] GameApplication* Build() const {
            if (gameWindow == nullptr)
                throw std::invalid_argument("GameWindow is null");

            return new GameApplication(
                hInstance,
                hPrevInstance,
                lpCmdLine,
                nCmdShow,
                gameWindow);
        }
    private:
        GameWindow* gameWindow = nullptr;

        HINSTANCE hInstance;
        HINSTANCE hPrevInstance;
        LPSTR lpCmdLine;
        int nCmdShow;
    };
}

#endif //VERTIX_GAMEAPPLICATIONBUILDER_H