//
// Created by Natsurainko on 2025/12/23.
//

#include <crtdbg.h>

#include "DemoMainWindow.h"
#include "Hosting/GameApplicationBuilder.hpp"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, const int nShowCmd) {
#ifndef NDEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    Vertix::WindowOptions windowOptions{};
    windowOptions.windowTitle = L"D3D12GameDemo.MainWindow";
    windowOptions.windowSize = { 1280, 720 };

    return Vertix::GameApplicationBuilder(hInstance, lpCmdLine, nShowCmd)
        .ConfigureWindow<DemoMainWindow>(windowOptions)
        .Build()
        .Run();
}