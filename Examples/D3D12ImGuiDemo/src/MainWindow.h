//
// Created by Natsurainko on 2026/2/26.
//

#ifndef VERTIX_MAINWINDOW_H
#define VERTIX_MAINWINDOW_H

#include <imgui/imgui.h>

#include "Vertix/Graphics/DescriptorHeap.h"
#include "Vertix/Rendering/RenderTexture.hpp"
#include "Vertix/Rendering/RenderResourceView.h"
#include "Vertix/Rendering/RenderResourceViewAllocator.hpp"
#include "Vertix/Windowing/GameWindow.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static Vertix::DescriptorHeap* imguiSrvDescriptorHeap = nullptr;

class MainWindow : public Vertix::GameWindow {
public:
    explicit MainWindow(const Vertix::WindowOptions &options, const float dpiScale) : GameWindow(options), dpiScale(dpiScale) {}

    void OnInitialize() override;
    void OnDestroy() override;

protected:
    void OnResized(const Vertix::Vector2D<unsigned> &size) override;
    void OnRender(double deltaTime) override;

    LRESULT BeforeWindowProc(const HWND hWnd, const UINT msg, const WPARAM wParam, const LPARAM lParam) override {
        return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
    }

private:
    float dpiScale = 1;
    ImGuiIO* io = nullptr;

    std::unique_ptr<Vertix::RenderResourceViewAllocator> renderTextureViewAllocator;
    Vertix::RenderResourceView<Vertix::RenderTarget> renderTargetViews[2] = {};

    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> commandList;
};

#endif //VERTIX_MAINWINDOW_H
