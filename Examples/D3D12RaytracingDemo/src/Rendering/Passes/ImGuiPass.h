//
// Created by Natsurainko on 2026/3/7.
//

#ifndef VERTIX_IMGUIPASS_H
#define VERTIX_IMGUIPASS_H

#include <imgui/imgui.h>

#include "../RenderContext.h"
#include "Vertix/Graphics/DescriptorHeap.h"
#include "Vertix/Rendering/RenderPass.hpp"

static Vertix::DescriptorHeap* imguiSrvDescriptorHeap = nullptr;

class ImGuiPass : public Vertix::RenderPass<RenderContext> {
public:
    explicit ImGuiPass(const Vertix::GameWindow* window);
    ~ImGuiPass() override;

    void Initialize(
        Vertix::GraphicsDevice* device,
        RenderContext* context) override;

    void Execute(ID3D12GraphicsCommandList5* commandList) override;
private:
    const Vertix::GameWindow *window;
    Vertix::SwapChain* swapChain;
    ImGuiIO* io = nullptr;
};

#endif //VERTIX_IMGUIPASS_H
