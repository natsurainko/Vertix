//
// Created by Natsurainko on 2025/12/24.
//

#ifndef VERTIX_COMMANDQUEUE_H
#define VERTIX_COMMANDQUEUE_H

#include <vector>
#include <wrl/client.h>

#include "GraphicsCommandList.h"
#include "d3d12/d3d12.h"

namespace Vertix {
    class GraphicsDevice;
    class FrameCommandList : public GraphicsCommandList {
    public:
        FrameCommandList(const GraphicsDevice *graphicsDevice, UINT frameCount);
        ~FrameCommandList() override;

        void BeginCommand(const Microsoft::WRL::ComPtr<ID3D12PipelineState> &pipelineState) const override;

        void WaitAllFrames() const;
        void WaitPreviousFrame() const;
        void MoveToNextFrame();
    private:
        UINT frameCount;
        UINT currentFrameIndex = 0;
        std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> allocators;

        UINT64 fenceValue;
        HANDLE fenceEvent;
        Microsoft::WRL::ComPtr<ID3D12Fence> fence;
        std::vector<UINT64> fenceValues;
    };
}

#endif //VERTIX_COMMANDQUEUE_H