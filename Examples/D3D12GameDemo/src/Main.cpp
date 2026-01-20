//
// Created by Natsurainko on 2025/12/23.
//

#define _CRTDBG_MAP_ALLOC

#include <crtdbg.h>

#include "DemoMainWindow.h"
#include "Hosting/GameApplicationBuilder.h"

_Use_decl_annotations_

int WINAPI WinMain(
    const HINSTANCE hInstance,
    const HINSTANCE hPrevInstance,
    const LPSTR lpCmdLine,
    const int nCmdShow)
{
    Vertix::GameApplicationBuilder builder(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    builder.ConfigureWindow<DemoMainWindow>();

    const auto application = builder.Build();
    const auto exitCode = application->Run();
    delete application;

    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtDumpMemoryLeaks();

    return exitCode;
}