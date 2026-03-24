//
// Created by Natsurainko on 2025/12/23.
//

#ifndef VERTIX_GAMEAPPLICATIONBUILDER_H
#define VERTIX_GAMEAPPLICATIONBUILDER_H

#include "GameApplication.h"

namespace Vertix {
    template<typename TWindow>
    concept GameWindowType = std::derived_from<TWindow, GameWindow>;

    class GameApplicationBuilder {
    public:
        GameApplicationBuilder(
            const HINSTANCE &hInstance,
            const LPSTR &lpCmdLine,
            const int &nCmdShow)
            : hInstance(hInstance), lpCmdLine(lpCmdLine), nCmdShow(nCmdShow) {}

        template <GameWindowType TWindow>
        GameApplicationBuilder ConfigureWindow() {
            gameWindow = new TWindow();
            return *this;
        }

        template <GameWindowType TWindow>
        GameApplicationBuilder ConfigureWindow(const WindowOptions &options) {
            gameWindow = new TWindow(options);
            return *this;
        }

        template <GameWindowType TWindow,  typename... Args>
        GameApplicationBuilder ConfigureWindow(const WindowOptions &options, Args... args) {
            gameWindow = new TWindow(options, args...);
            return *this;
        }

        template <GameWindowType TWindow>
        GameApplicationBuilder ConfigureWindow(TWindow* window) noexcept {
            gameWindow = window;
            return *this;
        }

        GameApplicationBuilder ConfigureGraphicsDevice(const GraphicsDeviceOptions &options) {
            graphicDeviceOptions = options;
            return *this;
        }

        [[nodiscard]]
        GameApplication Build() const {
            if (!gameWindow)
                throw std::invalid_argument("GameWindow is null");

            return {hInstance, lpCmdLine, nCmdShow, gameWindow, graphicDeviceOptions};
        }

    private:
        GameWindow* gameWindow = nullptr;
        GraphicsDeviceOptions graphicDeviceOptions{};

        HINSTANCE hInstance;
        LPSTR lpCmdLine;
        int nCmdShow;
    };
}

#endif //VERTIX_GAMEAPPLICATIONBUILDER_H