//
// Created by Natsurainko on 2025/12/23.
//

#include <crtdbg.h>

#include "DemoMainWindow.h"
#include "Vertix/Hosting/GameApplicationBuilder.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#ifndef NDEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    constexpr Vertix::WindowOptions windowOptions = {
        .windowSize = { 1280, 720 },
        .windowTitle = L"D3D12GameDemo.MainWindow"
    };

    const auto application = Vertix::GameApplicationBuilder()
        .ConfigureWindow<DemoMainWindow>(windowOptions)
        .Build();

    return application->Run();
}
