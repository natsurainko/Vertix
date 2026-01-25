//
// Created by Natsurainko on 2025/12/23.
//

#define _CRTDBG_MAP_ALLOC

#include <crtdbg.h>

#include "DemoMainWindow.h"
#include "Hosting/GameApplicationBuilder.h"

_Use_decl_annotations_

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, const int nShowCmd) {
    int exitCode;
    {
        Vertix::WindowOptions windowOptions = Vertix::WindowOptions::GetDefaultWindowOptions();
        windowOptions.windowTitle = L"D3D12GameDemo.MainWindow";
        windowOptions.windowSize = { 1280, 720 };

        exitCode = Vertix::GameApplicationBuilder(hInstance, lpCmdLine, nShowCmd)
            .ConfigureWindow<DemoMainWindow>(windowOptions)
            .Build()
            .Run();
    }

    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtDumpMemoryLeaks();

    return exitCode;
}