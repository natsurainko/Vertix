//
// Created by Natsurainko on 2026/3/7.
//

#ifndef VERTIX_IMGUIPASS_H
#define VERTIX_IMGUIPASS_H

#include <imgui/imgui.h>

#include "../RenderContext.h"
#include "Vertix/Graphics/DescriptorHeap.h"
#include "Vertix/Rendering/Pipeline/RenderPass.h"
#include "Vertix/Windowing/GameWindow.h"

static Vertix::DescriptorHeap* imguiSrvDescriptorHeap = nullptr;

class ImGuiPass : public Vertix::RenderPass<RenderContext> {
public:
    explicit ImGuiPass(const Vertix::GameWindow* window) : window(window), swapChain(window->GetSwapChain()) {}
    ~ImGuiPass() override;

    void Initialize(
        const Vertix::GraphicsDevice* device,
        const Vertix::PassInitializationContext &views,
        RenderContext* context) override;

    void Execute(ID3D12GraphicsCommandList5* commandList) override;
private:
    const Vertix::RenderResourceView<Vertix::RenderTarget>** currentFrameRTV = nullptr;

    const Vertix::GameWindow *window;
    Vertix::SwapChain* swapChain;
    ImGuiIO* io = nullptr;
};

#endif //VERTIX_IMGUIPASS_H
