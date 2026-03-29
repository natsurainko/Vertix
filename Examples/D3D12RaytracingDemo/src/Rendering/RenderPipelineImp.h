//
// Created by Natsurainko on 2026/1/27.
//

#ifndef D3D12_RENDER_PIPELINE_DEMO_RENDER_PIPELINE_IMP_H
#define D3D12_RENDER_PIPELINE_DEMO_RENDER_PIPELINE_IMP_H

#include "RenderContext.h"
#include "Graphics/FrameCommandList.h"
#include "Rendering/RenderPipeline.hpp"
#include "Windowing/GameWindow.h"

class RenderPipelineImp : public Vertix::RenderPipeline<RenderContext> {
public:
    RenderPipelineImp(
        Vertix::GraphicsDevice* graphicsDevice,
        Vertix::FrameCommandList* commandList,
        Vertix::GameWindow* gameWindow);

    void Execute() override;
    void Resize(const Vertix::Vector2D<unsigned> &size) override;

private:
    Vertix::SwapChain* swapChain;
    Vertix::GameWindow* window;
    Vertix::FrameCommandList* frameCommandList;
};

#endif //D3D12_RENDER_PIPELINE_DEMO_RENDER_PIPELINE_IMP_H