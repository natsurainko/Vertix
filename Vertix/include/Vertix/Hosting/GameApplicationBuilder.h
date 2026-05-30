//
// Created by Natsurainko on 2025/12/23.
//

#pragma once

#include <functional>

#include "Vertix/Graphics/GraphicsDevice.h"
#include "Vertix/Hosting/GameApplication.h"
#include "Vertix/Windowing/GameWindow.h"

namespace Vertix {
    template <typename TWindow>
    concept GameWindowType = std::derived_from<TWindow, GameWindow>;

    class GameApplicationBuilder {
        GraphicsDeviceOptions                        deviceOptions = {};
        std::function<std::unique_ptr<GameWindow>()> windowFactory = nullptr;

    public:
        GameApplicationBuilder() = default;

        template <GameWindowType TWindow, typename... Args>
        [[nodiscard]] GameApplicationBuilder& ConfigureWindow(const WindowOptions &options, Args... args) {
            windowFactory = [args = std::make_tuple(options, std::forward<Args>(args)...)]() mutable {
                return std::apply(
                    []<typename... TArg>(TArg &&... a) {
                        return std::make_unique<TWindow>(std::forward<TArg>(a)...);
                    },
                    std::move(args)
                );
            };
            return *this;
        }

        [[nodiscard]] VERTIX_API GameApplicationBuilder& ConfigureWindow(const std::function<std::unique_ptr<GameWindow>()> &);
        [[nodiscard]] VERTIX_API GameApplicationBuilder& ConfigureGraphicsDevice(const GraphicsDeviceOptions &options);

        [[nodiscard]] VERTIX_API std::unique_ptr<GameApplication> Build() const;
    };
}
