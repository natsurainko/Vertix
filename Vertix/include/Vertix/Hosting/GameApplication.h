//
// Created by Natsurainko on 2025/12/23.
//

#pragma once

namespace Vertix {
    class CommandQueue;
    class GameWindow;
    class GraphicsDevice;

    class GameApplication {
        friend class GameApplicationBuilder;

        inline static GameApplication* instance = nullptr;

        std::unique_ptr<GraphicsDevice> graphicsDevice;

        std::unique_ptr<CommandQueue> directCommandQueue;
        std::unique_ptr<CommandQueue> computeCommandQueue;
        std::unique_ptr<CommandQueue> copyCommandQueue;

        std::unique_ptr<GameWindow> mainWindow;

        GameApplication() noexcept;

    public:
        VERTIX_API ~GameApplication() noexcept;

        [[nodiscard]] VERTIX_API int Run() const;

        [[nodiscard]] static GameApplication* GetInstance() noexcept { return instance; }
        [[nodiscard]] GraphicsDevice*         GetGraphicsDevice() const noexcept { return graphicsDevice.get(); }
        [[nodiscard]] GameWindow*             GetMainWindow() const noexcept { return mainWindow.get(); }

        [[nodiscard]] CommandQueue* GetCommandQueue(const D3D12_COMMAND_LIST_TYPE type) const {
            switch (type) {
                case D3D12_COMMAND_LIST_TYPE_DIRECT: return directCommandQueue.get();
                case D3D12_COMMAND_LIST_TYPE_COMPUTE: return computeCommandQueue.get();
                case D3D12_COMMAND_LIST_TYPE_COPY: return copyCommandQueue.get();
                default: throw std::runtime_error("Not Support D3D12_COMMAND_LIST_TYPE");
            }
        }
    };
}
