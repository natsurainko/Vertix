//
// Created by Natsurainko on 2025/12/24.
//

#ifndef VERTIX_COMMANDQUEUE_H
#define VERTIX_COMMANDQUEUE_H

#include <vector>
#include <d3d12/d3d12.h>
#include <wrl/client.h>

#include "Vertix/VERTIX_EXPORT.h"
#include "Vertix/Graphics/GraphicsCommandList.h"

namespace Vertix {
    class GraphicsDevice;
    class FrameCommandList : public GraphicsCommandList {
    public:
        VERTIX_API FrameCommandList(
            const GraphicsDevice *graphicsDevice,
            UINT frameCount);

        VERTIX_API ~FrameCommandList() override;

        VERTIX_API void BeginCommand(const Microsoft::WRL::ComPtr<ID3D12PipelineState> &pipelineState) const override;
        VERTIX_API void MoveToNextFrame();
        VERTIX_API void WaitForCommand() override;

    private:
        UINT frameCount;
        UINT currentFrameIndex = 0;
        std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> allocators;

        HANDLE fenceEvent;
        Microsoft::WRL::ComPtr<ID3D12Fence> fence;
        std::vector<UINT64> fenceValues;
    };
}

#endif //VERTIX_COMMANDQUEUE_H
