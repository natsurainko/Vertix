//
// Created by Natsurainko on 2026/5/28.
//

#include "Vertix/Hosting/GameApplicationBuilder.h"

#include "Vertix/Graphics/Command/CommandQueue.h"

Vertix::GameApplicationBuilder& Vertix::GameApplicationBuilder::ConfigureWindow(const std::function<std::unique_ptr<GameWindow>()> &factory) {
    windowFactory = factory;
    return *this;
}

Vertix::GameApplicationBuilder& Vertix::GameApplicationBuilder::ConfigureGraphicsDevice(const GraphicsDeviceOptions &options) {
    deviceOptions = options;
    return *this;
}

std::unique_ptr<Vertix::GameApplication> Vertix::GameApplicationBuilder::Build() const {
    if (GameApplication::instance != nullptr) throw std::runtime_error("A running instance of GameApplication already exists.");

    auto device      = std::make_unique<GraphicsDevice>(deviceOptions);
    auto application = std::unique_ptr<GameApplication>(new GameApplication());

    application->directCommandQueue  = device->CreateCommandQueue(D3D12_COMMAND_QUEUE_DESC { D3D12_COMMAND_LIST_TYPE_DIRECT });
    application->computeCommandQueue = device->CreateCommandQueue(D3D12_COMMAND_QUEUE_DESC { D3D12_COMMAND_LIST_TYPE_COMPUTE });
    application->copyCommandQueue    = device->CreateCommandQueue(D3D12_COMMAND_QUEUE_DESC { D3D12_COMMAND_LIST_TYPE_COPY });
    application->graphicsDevice      = std::move(device);

    return application;
}
