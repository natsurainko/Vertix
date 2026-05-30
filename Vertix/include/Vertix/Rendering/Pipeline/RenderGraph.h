//
// Created by Natsurainko on 2026/5/14.
//

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "PassDeclaration.h"

namespace Vertix {
    class RenderPass;
    struct PipelineGraphNode;

    struct PipelineGraphEdge {
        std::string                    resourceName;
        const PassResourceDeclaration* resourceDeclaration = nullptr;

        D3D12_RESOURCE_STATES readerState = D3D12_RESOURCE_STATE_COMMON;
        D3D12_RESOURCE_STATES writerState = D3D12_RESOURCE_STATE_COMMON;

        PipelineGraphNode* readerNode = nullptr;
        PipelineGraphNode* writerNode = nullptr;
    };

    struct PipelineGraphNode {
        std::unique_ptr<RenderPass> renderPass;
        PassDeclaration             passDeclaration;

        std::vector<PipelineGraphEdge*> inEdges;
        std::vector<PipelineGraphEdge*> outEdges;
    };

    struct PipelineGraph {
        std::shared_ptr<PipelineGraphNode>              outRootNode;
        std::vector<std::shared_ptr<PipelineGraphNode>> nodeStorage;
        std::vector<std::unique_ptr<PipelineGraphEdge>> edgeStorage;
    };
}
