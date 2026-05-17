//
// Created by Natsurainko on 2026/5/16.
//

#include "Vertix/Rendering/Pipeline/RenderPipelineBuilder.h"

#include <algorithm>
#include <queue>
#include <stdexcept>
#include <d3d12/d3dx12_core.h>

#include "Vertix/Exceptions/HResultException.h"
#include "Vertix/Rendering/RenderBuffer.h"

Vertix::RenderPipelineBuilder::RenderPipelineBuilder(
    GraphicsDevice *graphicsDevice,
    FrameCommandList *frameCommandList)
: graphicsDevice(graphicsDevice), frameCommandList(frameCommandList)
{
    auto func = [&](const std::string &resourceName, const ResourceDeclaration &resourceDeclaration) {
        if (const auto result = resourceDeclarations.emplace(resourceName, resourceDeclaration); !result.second)
            throw std::runtime_error("Duplicately declared resources");
    };

    Buffers.func = func;
    Textures.func = func;
}

void Vertix::RenderPipelineBuilder::BufferCollection::Add(
    const std::string &resourceName,
    const D3D12_RESOURCE_DESC &resourceDesc,
    const bool resizable) const
{
    func(resourceName, ResourceDeclaration {
        .resourceKind  = RenderResourceKind::Buffer,
        .resourceDesc  = resourceDesc,
        .resizable     = resizable,
        .optimizeClear = false,
    });
}

void Vertix::RenderPipelineBuilder::TextureCollection::Add(
    const std::string &resourceName,
    const D3D12_RESOURCE_DESC &resourceDesc,
    const bool resizable) const
{
    func(resourceName, ResourceDeclaration {
        .resourceKind  = RenderResourceKind::Texture,
        .resourceDesc  = resourceDesc,
        .resizable     = resizable,
        .optimizeClear = false,
    });
}

void Vertix::RenderPipelineBuilder::TextureCollection::Add(
    const std::string &resourceName,
    const D3D12_RESOURCE_DESC &resourceDesc,
    const D3D12_CLEAR_VALUE &clearValue,
    const bool resizable) const
{
    func(resourceName, ResourceDeclaration {
        .resourceKind  = RenderResourceKind::Texture,
        .resourceDesc  = resourceDesc,
        .clearValue    = clearValue,
        .resizable     = resizable,
        .optimizeClear = true,
    });
}

std::unique_ptr<Vertix::RenderPipeline> Vertix::RenderPipelineBuilder::Build() {
    if (!SwapChain.ptr) throw std::runtime_error("Cannot build a RenderPipeline without a valid SwapChain");
    auto renderPipeline = new RenderPipeline(graphicsDevice, frameCommandList, SwapChain.ptr);
    renderPipeline->swapChainResourceName = SwapChain.resourceName;
    renderPipeline->swapChainViewDesc     = SwapChain.resourceViewDesc;

    InitializePipelineGraph(renderPipeline);

    std::unordered_set<std::string> usedResources;
    InitializePipelineResourceStates(renderPipeline, usedResources);
    InitializePipelineResources(renderPipeline, usedResources);
    InitializePipelineResourceViews(renderPipeline);

    InitializePipelinePasses(renderPipeline);
    renderPipeline->CompileBarriers();

    return std::unique_ptr<RenderPipeline>(renderPipeline);
}

void Vertix::RenderPipelineBuilder::InitializePipelineGraph(RenderPipeline* renderPipeline) {
    // Create PipelineGraph
    PipelineGraph &pipelineGraph = renderPipeline->pipelineGraph;
    pipelineGraph.outRootNode = std::make_shared<PipelineGraphNode>();
    pipelineGraph.outRootNode->renderPass = std::make_unique<EndRenderPass>();
    pipelineGraph.outRootNode->passDeclaration.Resources.push_back({
        .operation        = Op::READ,
        .resourceName     = SwapChain.resourceName,
        .viewDesc         = SwapChain.resourceViewDesc,
    });

    std::unordered_map<std::string, std::vector<std::shared_ptr<PipelineGraphNode>>> writerMap;
    std::unordered_map<std::string, std::vector<std::shared_ptr<PipelineGraphNode>>> readerMap;
    std::unordered_map<std::type_index, std::shared_ptr<PipelineGraphNode>> passTypeMap;

    for (auto& passDecl : Passes.decl) {
        auto node = std::make_shared<PipelineGraphNode>();
        node->renderPass      = passDecl.factory();
        node->passDeclaration = std::move(passDecl);

        for (auto& res : node->passDeclaration.Resources) {
            if (res.operation == Op::WRITE)
                writerMap[res.resourceName].push_back(node);
            else
                readerMap[res.resourceName].push_back(node);
        }
        pipelineGraph.nodeStorage.push_back(node);
        passTypeMap.emplace(node->passDeclaration.passType, node);
    }

    std::queue<std::shared_ptr<PipelineGraphNode>> queue;
    std::unordered_set<PipelineGraphNode*> visited;
    queue.emplace(pipelineGraph.outRootNode);

    while (!queue.empty()) {
        auto currentNode = queue.front(); queue.pop();
        if (!visited.insert(currentNode.get()).second) continue;

        for (const auto& res : currentNode->passDeclaration.Resources) {
            if (res.operation == Op::WRITE) continue;

            auto it = writerMap.find(res.resourceName);
            if (it == writerMap.end()) continue;

            for (auto& writerNode : it->second) {
                auto edge = std::make_shared<PipelineGraphEdge>();
                edge->resourceName        = res.resourceName;
                edge->resourceDeclaration = &res;
                edge->writerNode   = writerNode;
                edge->readerNode   = currentNode;

                currentNode->inEdges.push_back(edge);
                writerNode->outEdges.push_back(edge);
                queue.push(writerNode);
            }
        }
    }

    // Handle SideEffect passes
    for (auto& node : pipelineGraph.nodeStorage) {
        if (node->passDeclaration.hasSideEffect && !visited.contains(node.get())) {
            visited.insert(node.get());
            queue.push(node);

            while (!queue.empty()) {
                auto n = queue.front(); queue.pop();
                if (!visited.insert(n.get()).second) continue;

                for (const auto& res : n->passDeclaration.Resources) {
                    if (res.operation != Op::READ) continue;

                    auto it = writerMap.find(res.resourceName);
                    if (it == writerMap.end()) continue;

                    for (auto& w : it->second)
                        queue.push(w);
                }
            }
        }
    }

    // Handle DependsAfter passes
    for (auto& node : pipelineGraph.nodeStorage) {
        for (auto& typeIndex : node->passDeclaration.PassDependencies) {
            auto it = passTypeMap.find(typeIndex);
            if (it == passTypeMap.end())
                throw std::runtime_error("Unknown pass dependency: " + std::string(typeIndex.name()));

            auto edge = std::make_shared<PipelineGraphEdge>();
            edge->writerNode = it->second;
            edge->readerNode = node;

            node->inEdges.push_back(edge);
            it->second->outEdges.push_back(edge);
        }
    }

    // Topological sorting
    std::vector<PipelineGraphNode*> &sortedNodes = renderPipeline->pipelineGraphNodes;
    {
        std::queue<PipelineGraphNode*> topoQueue;
        std::unordered_map<PipelineGraphNode*, int> remainingInDegree;
        for (auto* node : visited) {
            remainingInDegree[node] = static_cast<int>(node->inEdges.size());
            if (node->inEdges.empty()) topoQueue.emplace(node);
        }

        while (!topoQueue.empty()) {
            auto* currentNode = topoQueue.front(); topoQueue.pop();
            sortedNodes.emplace_back(currentNode);

            for (auto& edge : currentNode->outEdges) {
                auto reader = edge->readerNode.lock();
                if (!reader) continue;
                if (--remainingInDegree[reader.get()] == 0)
                    topoQueue.push(reader.get());
            }
        }

        if (sortedNodes.size() != visited.size()) throw std::runtime_error("RenderPass topology cycle detected");
    }
}

void Vertix::RenderPipelineBuilder::InitializePipelineResourceStates(
    RenderPipeline* renderPipeline,
    std::unordered_set<std::string> &usedResources) const
{
    auto &outViewDescs = renderPipeline->resourcesViewDescs;
    auto &outInitialStates = renderPipeline->resourcesInitialStates;

    for (const auto& node : renderPipeline->pipelineGraphNodes) {
        std::unordered_map<std::string, PassResourceDeclaration*> declMap;
        for (auto& res : node->passDeclaration.Resources)
            declMap[res.resourceName] = &res;

        auto collectViewDesc = [&](const std::string& resName, RenderResourceViewDesc& viewDesc) {
            auto& ptrs = outViewDescs[resName];
            if (!std::ranges::any_of(ptrs, [&](const RenderResourceViewDesc* p) { return *p == viewDesc; }))
                ptrs.push_back(&viewDesc);
        };

        // current node is writer node
        for (const auto& edge : node->outEdges) {
            if (edge->resourceName.empty() || edge->resourceName == SwapChain.resourceName) continue;

            auto it = declMap.find(edge->resourceName);
            if (it == declMap.end()) throw std::runtime_error("Writer node missing declaration for: " + edge->resourceName);

            edge->writerState = GetWriterState(it->second->viewDesc);
            usedResources.insert(edge->resourceName);
            outInitialStates.emplace(edge->resourceName, edge->writerState);
            collectViewDesc(edge->resourceName, it->second->viewDesc);
        }

        // current node is reader node
        for (const auto& edge : node->inEdges) {
            if (edge->resourceName.empty() || edge->resourceName == SwapChain.resourceName) continue;

            auto it = declMap.find(edge->resourceName);
            if (it == declMap.end()) throw std::runtime_error("Reader node missing declaration for: " + edge->resourceName);

            edge->readerState = GetReaderState(it->second->viewDesc);
            usedResources.insert(edge->resourceName);
            outInitialStates.emplace(edge->resourceName, edge->readerState);
            collectViewDesc(edge->resourceName, it->second->viewDesc);
        }

        if (node->passDeclaration.hasSideEffect) {
            for (auto& passRes : node->passDeclaration.Resources) {
                if (passRes.operation != Op::WRITE) continue;
                if (passRes.resourceName.empty() || passRes.resourceName == SwapChain.resourceName) continue;

                if (std::ranges::any_of(node->outEdges, [&](const auto& e) { return e->resourceName == passRes.resourceName; })) continue;

                usedResources.insert(passRes.resourceName);
                outInitialStates.emplace(passRes.resourceName, GetWriterState(passRes.viewDesc));
                collectViewDesc(passRes.resourceName, passRes.viewDesc);
            }
        }
    }
}

void Vertix::RenderPipelineBuilder::InitializePipelineResources(
    RenderPipeline* renderPipeline,
    const std::unordered_set<std::string>& usedResources)
{
    const auto device = graphicsDevice->GetD3D12Device();
    const auto &inInitialStates = renderPipeline->resourcesInitialStates;
    auto &resizableTextures = renderPipeline->resizableTextures;
    auto &resources = renderPipeline->resources;

    for (const auto &resourceName : usedResources) {
        auto &resDecl = resourceDeclarations.at(resourceName);
        GetResourceStateFlags(renderPipeline, resourceName, resDecl.resourceDesc.Flags);

        Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
        const D3D12_RESOURCE_STATES initState = inInitialStates.at(resourceName);
        const CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);
        ThrowIfFailed(device->CreateCommittedResource(
            &defaultHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &resDecl.resourceDesc,
            initState,
            resDecl.optimizeClear ? &resDecl.clearValue : nullptr,
            IID_PPV_ARGS(&d3d12Resource)
        ));

        if (resDecl.resourceKind == RenderResourceKind::Buffer) {
            resources.emplace(resourceName, std::make_unique<RenderBuffer>(d3d12Resource, initState));
        } else if (resDecl.resourceKind == RenderResourceKind::Texture) {
            const D3D12_RESOURCE_DESC &desc = resDecl.resourceDesc;
            const std::optional<D3D12_CLEAR_VALUE> &clearValue = resDecl.optimizeClear ? std::optional(resDecl.clearValue) : std::nullopt;

            switch (resDecl.resourceDesc.Dimension) {
                case D3D12_RESOURCE_DIMENSION_TEXTURE1D: resources.emplace(resourceName, std::make_unique<RenderTexture1D>(d3d12Resource, desc, initState, clearValue)); break;
                case D3D12_RESOURCE_DIMENSION_TEXTURE2D: resources.emplace(resourceName, std::make_unique<RenderTexture2D>(d3d12Resource, desc, initState, clearValue)); break;
                case D3D12_RESOURCE_DIMENSION_TEXTURE3D: resources.emplace(resourceName, std::make_unique<RenderTexture3D>(d3d12Resource, desc, initState, clearValue)); break;
                default:
                    throw std::runtime_error("Not support this resource dimension");
            }

            if (resDecl.resizable) resizableTextures.emplace(resourceName, static_cast<RenderTexture*>(resources.at(resourceName).get()));
        }
    }
}

void Vertix::RenderPipelineBuilder::InitializePipelineResourceViews(RenderPipeline* renderPipeline) const {
    const auto &resources = renderPipeline->resources;
    const auto &resourceViewDescs = renderPipeline->resourcesViewDescs;

    renderPipeline->viewAllocator = std::make_unique<RenderResourceViewAllocator>(graphicsDevice);
    auto allocator = renderPipeline->viewAllocator.get();
    auto &views = renderPipeline->views;

    uint32_t rtvDescriptorSize    = Descriptors.reservedRTVDescriptorCount + SwapChain.ptr->GetFrameCount();
    uint32_t dsvDescriptorSize    = Descriptors.reservedDSVDescriptorCount;
    uint32_t sharedDescriptorSize = Descriptors.reservedSharedDescriptorCount + 1;

    for (const auto &viewDescs: resourceViewDescs | std::views::values) {
        for (const auto& viewDesc : viewDescs) {
            switch (viewDesc->type) {
                case RenderResourceViewType::RenderTarget: ++rtvDescriptorSize; break;
                case RenderResourceViewType::DepthStencil: ++dsvDescriptorSize; break;
                case RenderResourceViewType::ConstantBuffer:
                case RenderResourceViewType::UnorderedAccess:
                case RenderResourceViewType::ShaderResource: ++sharedDescriptorSize; break;
            }
        }
    }

    allocator->InitRenderTargetDescriptorHeap(rtvDescriptorSize);
    allocator->InitDepthStencilDescriptorHeap(dsvDescriptorSize);
    allocator->InitSharedDescriptorHeap(sharedDescriptorSize);

    renderPipeline->nullHandle = allocator->GetSharedDescriptorHeap()->AllocDescriptorHandle();
    if (Descriptors.onAllocatorCreated) Descriptors.onAllocatorCreated(allocator);

    for (const auto &[resourceName, viewDescs]: resourceViewDescs) {
        if (resourceName == SwapChain.resourceName) continue;

        for (const auto& viewDesc : viewDescs) {
            views.emplace(viewDesc, allocator->CreateView(resources.at(resourceName).get(), *viewDesc));
        }
    }

    for (UINT i = 0; i < SwapChain.ptr->GetFrameCount(); ++i) {
        renderPipeline->frameRTVs.emplace_back(allocator->CreateView(SwapChain.ptr->GetBuffer(i), SwapChain.resourceViewDesc));
    }
}

void Vertix::RenderPipelineBuilder::InitializePipelinePasses(RenderPipeline* renderPipeline) const {
    const auto device = graphicsDevice->GetD3D12Device().Get();
    const auto &views = renderPipeline->views;
    auto &frameInjectors = renderPipeline->frameInjectors;

    for (const auto& node : renderPipeline->pipelineGraphNodes) {
        for (auto &passRes : node->passDeclaration.Resources) {
            if (!passRes.resourceBinding) continue;
            passRes.resourceBinding->Target(node->renderPass.get());

            if (passRes.resourceName == SwapChain.resourceName) {
                frameInjectors.push_back(passRes.resourceBinding);
                continue;
            }

            const auto& registeredDescs = renderPipeline->resourcesViewDescs.at(passRes.resourceName);
            auto canonicalIt = std::ranges::find_if(registeredDescs, [&](const RenderResourceViewDesc* p) { return *p == passRes.viewDesc; });

            if (canonicalIt == registeredDescs.end())
                throw std::runtime_error("No view found for resource: " + passRes.resourceName);

            passRes.resourceBinding->Inject(&views.at(*canonicalIt));
        }

        node->renderPass->Initialize(device);
    }
}

D3D12_RESOURCE_STATES Vertix::RenderPipelineBuilder::GetWriterState(const RenderResourceViewDesc &viewDesc) {
    switch (viewDesc.type) {
        case RenderResourceViewType::RenderTarget:    return D3D12_RESOURCE_STATE_RENDER_TARGET;
        case RenderResourceViewType::DepthStencil:    return D3D12_RESOURCE_STATE_DEPTH_WRITE;
        case RenderResourceViewType::UnorderedAccess: return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        default: throw std::runtime_error("Unsupported view type for WRITE operation.");
    }
}

D3D12_RESOURCE_STATES Vertix::RenderPipelineBuilder::GetReaderState(const RenderResourceViewDesc &viewDesc) {
    switch (viewDesc.type) {
        case RenderResourceViewType::ShaderResource:  return D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
        case RenderResourceViewType::ConstantBuffer:  return D3D12_RESOURCE_STATE_GENERIC_READ;
        case RenderResourceViewType::UnorderedAccess: return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        case RenderResourceViewType::RenderTarget:    throw std::runtime_error("RenderTarget does not allow READ.");
        case RenderResourceViewType::DepthStencil:    throw std::runtime_error("DepthStencil does not allow READ.");
        default: throw std::runtime_error("Unknown view type for READ operation.");
    }
}

void Vertix::RenderPipelineBuilder::GetResourceStateFlags(
    const RenderPipeline* renderPipeline,
    const std::string &resourceName,
    D3D12_RESOURCE_FLAGS &flags)
{
    for (const auto &viewDesc : renderPipeline->resourcesViewDescs.at(resourceName)) {
        if (const auto type = viewDesc->type; type == RenderResourceViewType::RenderTarget) {
            flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        } else if (type == RenderResourceViewType::DepthStencil) {
            flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        } else if (type == RenderResourceViewType::UnorderedAccess) {
            flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        }
    }

    if (flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET && flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
        throw std::runtime_error("RenderTarget and DepthStencil cannot be combined on the same resource.");
}
