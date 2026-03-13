//
// Created by Natsurainko on 2026/2/26.
//

#include <imgui/backends/imgui_impl_win32.h>

#include "MainWindow.h"
#include "Hosting/GameApplicationBuilder.hpp"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, const int nShowCmd) {
#ifndef NDEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    ImGui_ImplWin32_EnableDpiAwareness();
    const float scale = ImGui_ImplWin32_GetDpiScaleForMonitor(MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

    Vertix::WindowOptions windowOptions{};
    windowOptions.windowTitle = L"D3D12ImGuiDemo.MainWindow";
    windowOptions.windowSize = (Vertix::Vector2D<float>{ 1280, 720 } * scale).Cast<UINT>();

    return Vertix::GameApplicationBuilder(hInstance, lpCmdLine, nShowCmd)
        .ConfigureWindow<MainWindow>(windowOptions, scale)
        .Build()
        .Run();
}
