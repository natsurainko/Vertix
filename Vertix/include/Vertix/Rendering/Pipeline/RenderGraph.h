//
// Created by Natsurainko on 2026/5/14.
//

#ifndef VERTIX_RENDERGRAPH_H
#define VERTIX_RENDERGRAPH_H

#include <memory>
#include <vector>
#include <string>

#include "PassDeclaration.h"

namespace Vertix {
    class RenderPass;
    class RenderResource;
    class SwapChain;

    struct PipelineGraphNode;
    struct PipelineGraphEdge {
        std::string resourceName;
        const PassResourceDeclaration* resourceDeclaration;

        D3D12_RESOURCE_STATES readerState;
        D3D12_RESOURCE_STATES writerState;

        std::weak_ptr<PipelineGraphNode> readerNode;
        std::weak_ptr<PipelineGraphNode> writerNode;
    };
    struct PipelineGraphNode {
        std::unique_ptr<RenderPass> renderPass;
        PassDeclaration passDeclaration;

        std::vector<std::shared_ptr<PipelineGraphEdge>> inEdges;
        std::vector<std::shared_ptr<PipelineGraphEdge>> outEdges;
    };
    struct PipelineGraph {
        std::shared_ptr<PipelineGraphNode> outRootNode;
        std::vector<std::shared_ptr<PipelineGraphNode>> nodeStorage;
    };
}

#endif //VERTIX_RENDERGRAPH_H
