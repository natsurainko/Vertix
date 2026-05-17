//
// Created by Natsurainko on 2026/3/28.
//

#ifndef VERTIX_MAINWINDOW_H
#define VERTIX_MAINWINDOW_H

#include <imgui/imgui.h>
#include <Vertix/Rendering/Pipeline/RenderPipeline.h>
#include <Vertix.Engine/Controlling/DefaultPositionController.hpp>
#include <Vertix.Engine/Controlling/DefaultRotationController.hpp>
#include <Vertix/Windowing/GameWindow.h>

#include "Controlling/KeyboardControllerInput.h"
#include "Controlling/MouseControllerInput.h"
#include "Rendering/RenderContext.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class MainWindow : public Vertix::GameWindow {
public:
    explicit MainWindow(const Vertix::WindowOptions &options) : GameWindow(options) {}
    void OnInitialize() override;

protected:
    void OnRender(double deltaTime) override;
    void OnUpdate(double deltaTime) override;
    void OnResized(const Vertix::Vector2D<unsigned> &size) override;
    void OnFocusLost() override;

    LRESULT BeforeWindowProc(const HWND hWnd, const UINT msg, const WPARAM wParam, const LPARAM lParam) override {
        return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
    }

private:
    void BuildRenderPipeline();

    ImGuiIO* imGuiIO = nullptr;
    std::unique_ptr<Vertix::RenderPipeline> renderPipeline;
    std::unique_ptr<RenderContext> renderContext;

    KeyboardControllerInput keyboardControllerInput {};
    MouseControllerInput mouseControllerInput {this};

    Vertix::Engine::DefaultPositionController defaultPositionController {&keyboardControllerInput};
    Vertix::Engine::DefaultRotationController defaultRotationController {&mouseControllerInput};

    Microsoft::WRL::ComPtr<ID3D12CommandQueue> copyCommandQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> computeCommandQueue;
};

#endif //VERTIX_MAINWINDOW_H
