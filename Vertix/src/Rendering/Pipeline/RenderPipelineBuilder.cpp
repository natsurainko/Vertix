//
// Created by Natsurainko on 2026/5/16.
//

#include "Vertix/Rendering/Pipeline/RenderPipelineBuilder.h"

#include <algorithm>
#include <queue>
#include <stdexcept>

#include "Vertix/Exceptions/HResultException.h"
#include "Vertix/Rendering/RenderBuffer.h"

Vertix::RenderPipelineBuilder::RenderPipelineBuilder(
    GraphicsDevice *graphicsDevice,
    FrameCommandList *frameCommandList)
: graphicsDevice(graphicsDevice), frameCommandList(frameCommandList)
{
    auto func = [&](const std::string &resourceName, const ResourceFactoryMethod &method) {
        if (const auto result = resourceRegistries.emplace(resourceName, method); !result.second)
            throw std::runtime_error("Duplicately declared resources");
    };

    Buffers.registerResource = func;
    Textures.registerResource = func;
    Textures.registerResizableResource = [&](const std::string &resourceName) {
        if (const auto result = resizableTextures.insert(resourceName); !result.second)
            throw std::runtime_error("Duplicately declared resources");
    };
}

void Vertix::RenderPipelineBuilder::BufferCollection::Add(
    const std::string &resourceName,
    const D3D12_RESOURCE_DESC &resourceDesc) const
{
    registerResource(resourceName, [=](
        ID3D12Device* device,
        const RenderResourceUsage allUsages,
        const D3D12_RESOURCE_STATES initialState)
    {
        const auto heapProps = DeriveHeapProperties(allUsages);
        auto desc = resourceDesc;
        desc.Flags = DeriveResourceFlags(allUsages);

        Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
        ThrowIfFailed(device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            initialState,
            nullptr,
            IID_PPV_ARGS(&d3d12Resource)
        ));

        return std::make_unique<RenderBuffer>(d3d12Resource, initialState);
    });
}

void Vertix::RenderPipelineBuilder::TextureCollection::Add(
    const std::string &resourceName,
    const D3D12_RESOURCE_DESC &resourceDesc,
    const bool resizable) const
{
    if (resizable) registerResizableResource(resourceName);
    registerResource(resourceName, [=](
        ID3D12Device* device,
        const RenderResourceUsage allUsages,
        const D3D12_RESOURCE_STATES initialState) -> std::unique_ptr<RenderResource>
    {
        const auto heapProps = DeriveHeapProperties(allUsages);
        auto desc = resourceDesc;
        desc.Flags = DeriveResourceFlags(allUsages);

        Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
        ThrowIfFailed(device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            initialState,
            nullptr,
            IID_PPV_ARGS(&d3d12Resource)
        ));

        switch (desc.Dimension) {
            case D3D12_RESOURCE_DIMENSION_TEXTURE1D: return std::make_unique<RenderTexture1D>(d3d12Resource, desc, initialState);
            case D3D12_RESOURCE_DIMENSION_TEXTURE2D: return std::make_unique<RenderTexture2D>(d3d12Resource, desc, initialState);
            case D3D12_RESOURCE_DIMENSION_TEXTURE3D: return std::make_unique<RenderTexture3D>(d3d12Resource, desc, initialState);
            default: throw std::runtime_error("Invalid Dimension in D3D12_RESOURCE_DESC");
        }
    });
}

void Vertix::RenderPipelineBuilder::TextureCollection::Add(
    const std::string &resourceName,
    const D3D12_RESOURCE_DESC &resourceDesc,
    const D3D12_CLEAR_VALUE &clearValue,
    const bool resizable) const
{
    if (resizable) registerResizableResource(resourceName);
    registerResource(resourceName, [=](
        ID3D12Device* device,
        const RenderResourceUsage allUsages,
        const D3D12_RESOURCE_STATES initialState) -> std::unique_ptr<RenderResource>
    {
        const auto heapProps = DeriveHeapProperties(allUsages);
        auto desc = resourceDesc;
        desc.Flags = DeriveResourceFlags(allUsages);

        Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
        ThrowIfFailed(device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            initialState,
            &clearValue,
            IID_PPV_ARGS(&d3d12Resource)
        ));

        switch (desc.Dimension) {
            case D3D12_RESOURCE_DIMENSION_TEXTURE1D: return std::make_unique<RenderTexture1D>(d3d12Resource, desc, initialState, std::optional(clearValue));
            case D3D12_RESOURCE_DIMENSION_TEXTURE2D: return std::make_unique<RenderTexture2D>(d3d12Resource, desc, initialState, std::optional(clearValue));
            case D3D12_RESOURCE_DIMENSION_TEXTURE3D: return std::make_unique<RenderTexture3D>(d3d12Resource, desc, initialState, std::optional(clearValue));
            default: throw std::runtime_error("Invalid Dimension in D3D12_RESOURCE_DESC");
        }
    });
}

std::unique_ptr<Vertix::RenderPipeline> Vertix::RenderPipelineBuilder::Build() {
    if (!SwapChain.ptr) throw std::runtime_error("Cannot build a RenderPipeline without a valid SwapChain");
    auto renderPipeline = new RenderPipeline(graphicsDevice, frameCommandList, SwapChain.ptr);
    renderPipeline->swapChainResourceName = SwapChain.swapChainResourceName;
    renderPipeline->swapChainViewDesc     = SwapChain.swapChainViewDesc;

    InitializePipelineGraph(renderPipeline);

    std::unordered_set<std::string> usedResources;
    InitializePipelineResourceStates(renderPipeline, usedResources);
    InitializePipelineResources(renderPipeline, usedResources);
    InitializePipelineResourceViews(renderPipeline, usedResources);

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
        .resourceName  = SwapChain.swapChainResourceName,
        .resourceUsage = RenderResourceUsage::Present,
    });

    std::unordered_map<std::string, std::vector<std::shared_ptr<PipelineGraphNode>>> writerMap;
    std::unordered_map<std::type_index, std::shared_ptr<PipelineGraphNode>> passTypeMap;
    std::unordered_set<PipelineGraphNode*> visited;

    for (auto& passDecl : Passes.decl) {
        auto node = std::make_shared<PipelineGraphNode>();
        node->renderPass      = passDecl.factory();
        node->passDeclaration = std::move(passDecl);

        for (auto& res : node->passDeclaration.Resources) {
            if (IsWriteUsage(res.resourceUsage))
                writerMap[res.resourceName].push_back(node);
        }

        pipelineGraph.nodeStorage.push_back(node);
        passTypeMap.emplace(node->passDeclaration.passType, node);
    }

    TraceDataflowEdges(pipelineGraph.outRootNode.get(), pipelineGraph, writerMap, visited);

    // Handle SideEffect passes
    for (auto& node : pipelineGraph.nodeStorage) {
        if (node->passDeclaration.hasSideEffect && !visited.contains(node.get())) {
            TraceDataflowEdges(node.get(), pipelineGraph, writerMap, visited);
        }
    }

    // Handle DependsAfter passes
    for (auto& node : pipelineGraph.nodeStorage) {
        for (auto& typeIndex : node->passDeclaration.PassDependencies) {
            auto it = passTypeMap.find(typeIndex);
            if (it == passTypeMap.end())
                throw std::runtime_error("Unknown pass dependency: " + std::string(typeIndex.name()));

            auto edge = std::make_unique<PipelineGraphEdge>();
            edge->writerNode = it->second.get();
            edge->readerNode = node.get();

            node->inEdges.push_back(edge.get());
            it->second->outEdges.push_back(edge.get());
            pipelineGraph.edgeStorage.push_back(std::move(edge));
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
                auto* reader = edge->readerNode; if (!reader) continue;
                if (--remainingInDegree[reader] == 0)
                    topoQueue.push(reader);
            }
        }

        if (sortedNodes.size() != visited.size()) throw std::runtime_error("RenderPass topology cycle detected");
    }
}

void Vertix::RenderPipelineBuilder::InitializePipelineResourceStates(
    RenderPipeline* renderPipeline,
    std::unordered_set<std::string> &usedResources) const
{
    auto &initialStates = renderPipeline->resourcesInitialStates;
    auto &allUsages = renderPipeline->resourcesAllUsages;

    for (const auto* node : renderPipeline->pipelineGraphNodes) {
        for (const auto& res : node->passDeclaration.Resources) {
            if (res.resourceName.empty() || res.resourceName == SwapChain.swapChainResourceName) continue;
            usedResources.insert(res.resourceName);
            initialStates.emplace(res.resourceName, DeriveState(res.resourceUsage));
            allUsages[res.resourceName] = allUsages[res.resourceName] | res.resourceUsage;
        }
    }
}

void Vertix::RenderPipelineBuilder::InitializePipelineResources(
    RenderPipeline* renderPipeline,
    const std::unordered_set<std::string>& usedResources) const
{
    ID3D12Device* device = graphicsDevice->GetD3D12Device().Get();
    for (const auto &resourceName : usedResources) {
        const D3D12_RESOURCE_STATES initState = renderPipeline->resourcesInitialStates.at(resourceName);
        const RenderResourceUsage   allUsages = renderPipeline->resourcesAllUsages.at(resourceName);

        const auto& factoryMethod = resourceRegistries.at(resourceName);
        renderPipeline->resources.emplace(resourceName, factoryMethod(device, allUsages, initState));

        if (resizableTextures.contains(resourceName))
            renderPipeline->resizableTextures.emplace(resourceName, static_cast<RenderTexture*>(renderPipeline->resources.at(resourceName).get()));
    }
}

void Vertix::RenderPipelineBuilder::InitializePipelineResourceViews(
    RenderPipeline* renderPipeline,
    const std::unordered_set<std::string>& usedResources)
{
    uint32_t heapsCapacity[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] = {
        (std::max)(Descriptors.reservedSharedDescriptorCount + 1, static_cast<uint32_t>(1)),
        (std::max)(Descriptors.reservedSamplerDescriptorCount, static_cast<uint32_t>(1)),
        (std::max)(Descriptors.reservedRTVDescriptorCount + SwapChain.ptr->GetFrameCount(), static_cast<uint32_t>(1)),
        (std::max)(Descriptors.reservedDSVDescriptorCount, static_cast<uint32_t>(1)),
    };

    struct Allocation {
        D3D12_DESCRIPTOR_HEAP_TYPE heapType;
        const ViewFactoryMethod &factory;
        const std::string &resourceName;
        DescriptorHandle &handle;
    };

    std::vector<Allocation> allocations;
    for (const auto &resourceName : usedResources) {
        if (!viewRegistries.contains(resourceName)) continue;
        for (auto &viewRegistry : viewRegistries.at(resourceName)) {
            switch (viewRegistry.usage) {
                case RenderResourceUsage::RenderTarget: {
                    ++heapsCapacity[D3D12_DESCRIPTOR_HEAP_TYPE_RTV];
                    allocations.emplace_back(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, viewRegistry.factoryMethod, resourceName, viewRegistry.handle);
                } break;
                case RenderResourceUsage::DepthRead:
                case RenderResourceUsage::DepthWrite: {
                    ++heapsCapacity[D3D12_DESCRIPTOR_HEAP_TYPE_DSV];
                    allocations.emplace_back(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, viewRegistry.factoryMethod, resourceName, viewRegistry.handle);
                } break;
                case RenderResourceUsage::ConstantBuffer:
                case RenderResourceUsage::UnorderedAccess:
                case RenderResourceUsage::NonPixelShaderResource:
                case RenderResourceUsage::PixelShaderResource:
                case RenderResourceUsage::AllShaderResource: {
                    ++heapsCapacity[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV];
                    allocations.emplace_back(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, viewRegistry.factoryMethod, resourceName, viewRegistry.handle);
                } break;
                default: throw std::runtime_error("Unreachable");
            }
        }
    }

    ID3D12Device* device = graphicsDevice->GetD3D12Device().Get();
    renderPipeline->descriptorHeapSet = std::make_unique<DescriptorHeapSet>(device, heapsCapacity);

    const DescriptorHeapSet &heapSet = *renderPipeline->descriptorHeapSet;
    renderPipeline->nullHandle = heapSet[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->AllocDescriptorHandle();
    if (Descriptors.onDescriptorSetCreated) Descriptors.onDescriptorSetCreated(&heapSet);

    for (const auto &[heapType, factory, resourceName, handle] : allocations) {
        handle = heapSet[heapType]->AllocDescriptorHandle();
        factory(device, handle, renderPipeline->resources);
        renderPipeline->descriptorViews[resourceName].emplace(handle, factory);
    }

    const auto frameCount = SwapChain.ptr->GetFrameCount();
    renderPipeline->frameRTVs = std::make_unique<DescriptorView<RenderResourceUsage::RenderTarget>[]>(frameCount);
    for (UINT i = 0; i < frameCount; ++i) {
        const auto handle = heapSet[D3D12_DESCRIPTOR_HEAP_TYPE_RTV]->AllocDescriptorHandle();
        device->CreateRenderTargetView(
            SwapChain.ptr->GetBuffer(i)->GetResource(),
            SwapChain.swapChainViewDesc.has_value() ? &SwapChain.swapChainViewDesc.value() : nullptr,
            handle.cpuHandle
        );
        renderPipeline->frameRTVs[i] = handle;
    }
}

void Vertix::RenderPipelineBuilder::InitializePipelinePasses(RenderPipeline* renderPipeline) const {
    const auto device = graphicsDevice->GetD3D12Device().Get();
    const auto &resources = renderPipeline->resources;
    auto &frameInjectors = renderPipeline->frameInjectors;

    for (const auto& node : renderPipeline->pipelineGraphNodes) {
        for (auto &passRes : node->passDeclaration.Resources) {
            if (!passRes.resourceBinding) continue;
            passRes.resourceBinding->Target(node->renderPass.get());

            if (passRes.resourceName == SwapChain.swapChainResourceName) {
                frameInjectors.push_back(passRes.resourceBinding);
                continue;
            }

            if (passRes.method == Md::Resource) {
                const auto field = resources.at(passRes.resourceName).get();
                passRes.resourceBinding->InjectValue(&field);
            } else if (passRes.method == Md::GPUAddress) {
                const auto field = resources.at(passRes.resourceName)->GetGPUVirtualAddress();
                passRes.resourceBinding->InjectValue(&field);
            }
        }
    }

    for (const auto &registries: viewRegistries | std::views::values) {
        for (const auto& registry : registries) {
            for (const auto& weakBinding : registry.bindings) {
                if (const auto b = weakBinding.lock())
                    b->InjectValue(&registry.handle);
            }
        }
    }

    for (const auto& node : renderPipeline->pipelineGraphNodes) {
        node->renderPass->Initialize(device);
    }
}

void Vertix::RenderPipelineBuilder::TraceDataflowEdges(
    PipelineGraphNode* startNode,
    PipelineGraph& graph,
    const std::unordered_map<std::string, std::vector<std::shared_ptr<PipelineGraphNode>>>& writerMap,
    std::unordered_set<PipelineGraphNode*>& visited)
{
    std::queue<PipelineGraphNode*> queue;
    queue.push(startNode);

    while (!queue.empty()) {
        auto* currentNode = queue.front(); queue.pop();
        if (!visited.insert(currentNode).second) continue;

        for (const auto& res : currentNode->passDeclaration.Resources) {
            if (IsWriteUsage(res.resourceUsage)) continue;

            auto it = writerMap.find(res.resourceName);
            if (it == writerMap.end()) continue;

            for (auto& writerNode : it->second) {
                auto edge = std::make_unique<PipelineGraphEdge>();
                edge->resourceName        = res.resourceName;
                edge->resourceDeclaration = &res;
                edge->writerNode = writerNode.get();
                edge->readerNode = currentNode;
                edge->readerState = DeriveState(res.resourceUsage);

                auto wDecl = std::ranges::find_if(writerNode->passDeclaration.Resources,
                    [&](auto& wr) { return wr.resourceName == res.resourceName && IsWriteUsage(wr.resourceUsage); });
                if (wDecl != writerNode->passDeclaration.Resources.end())
                    edge->writerState = DeriveState(wDecl->resourceUsage);

                currentNode->inEdges.push_back(edge.get());
                writerNode->outEdges.push_back(edge.get());
                graph.edgeStorage.push_back(std::move(edge));
                queue.push(writerNode.get());
            }
        }
    }
}
