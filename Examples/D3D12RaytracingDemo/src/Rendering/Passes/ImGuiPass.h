//
// Created by Natsurainko on 2026/3/7.
//

#ifndef VERTIX_IMGUIPASS_H
#define VERTIX_IMGUIPASS_H

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_dx12.h>
#include <imgui/backends/imgui_impl_win32.h>
#include <../../../../../Vertix/include/Vertix/Graphics/Descriptor/DescriptorHeap.h>
#include <../../../../../Vertix/include/Vertix/Graphics/Descriptor/DescriptorView.h>
#include <Vertix/Rendering/RenderResourceUsage.h>
#include <Vertix/Rendering/Pipeline/RenderPass.h>
#include <Vertix/Windowing/GameWindow.h>

#include "../RenderContext.h"

class ImGuiPass : public Vertix::RenderPass {
public:
    explicit ImGuiPass(
        const Vertix::GameWindow* window,
        RenderContext* renderContext)
    : swapChain(window->GetSwapChain()), renderContext(renderContext), window(window) {}

    ~ImGuiPass() override;

    void Initialize(ID3D12Device10* device) override;
    void Execute(ID3D12GraphicsCommandList5* commandList) override;

    Vertix::DescriptorView<Vertix::RenderResourceUsage::RenderTarget> currentFrameRTV;

private:
    ImGuiIO* io = nullptr;
    Vertix::SwapChain* swapChain;
    RenderContext* renderContext;
    const Vertix::GameWindow* window;
};

#endif //VERTIX_IMGUIPASS_H
