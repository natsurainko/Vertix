//
// Created by Natsurainko on 2026/3/28.
//

#include <crtdbg.h>

#include "MainWindow.h"
#include "Vertix/Hosting/GameApplicationBuilder.hpp"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, const int nShowCmd) {
#ifndef NDEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    constexpr Vertix::GraphicsDeviceOptions graphicsDeviceOptions {
        .enableDebugLayer = true
    };

    const Vertix::WindowOptions windowOptions {
        .windowSize = { 1280, 768 },
        .windowTitle = L"D3D12RaytracingDemo.MainWindow",
    };

    return Vertix::GameApplicationBuilder(hInstance, lpCmdLine, nShowCmd)
        .ConfigureGraphicsDevice(graphicsDeviceOptions)
        .ConfigureWindow<MainWindow>(windowOptions)
        .Build()
        .Run();
}
