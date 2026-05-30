//
// Created by Natsurainko on 2025/12/23.
//

#include "Vertix/Hosting/GameApplication.h"

#include "Vertix/Graphics/GraphicsDevice.h"
#include "Vertix/Graphics/Command/CommandQueue.h"
#include "Vertix/Windowing/GameWindow.h"

Vertix::GameApplication::GameApplication() noexcept { instance = this; }
Vertix::GameApplication::~GameApplication() noexcept { instance = nullptr; }

int Vertix::GameApplication::Run() const {
    WPARAM result;
    mainWindow->Show();
    mainWindow->Run(result);
    return static_cast<int>(result);
}
