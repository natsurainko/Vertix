//
// Created by Natsurainko on 2026/3/28.
//

#ifndef VERTIX_RENDERCONTEXT_H
#define VERTIX_RENDERCONTEXT_H

#include <structures.h>
#include <thread>

#include "Vertix/Graphics/Buffers/ConstantBuffer.hpp"
#include "Vertix/Graphics/Buffers/ConstantBufferPageArray.hpp"
#include "Vertix/Graphics/Raytracing/TopLevelAccelerationStructure.h"
#include "Vertix.Engine/Camera/PerspectiveCamera.h"
#include "Vertix.Engine/Helpers/MathHelper.h"
#include "Vertix.Engine/Helpers/VectorHelper.h"
#include "Vertix.Engine/Scene/SceneObject3D.hpp"
#include "Vertix/Graphics/FrameCommandList.h"
#include "Vertix/Math/Vector2D.hpp"
#include "Vertix/Pool/ModelPool.hpp"
#include "Vertix/Rendering/RenderResourceViewAllocator.hpp"

#define SHADER_BYTECODE(T) CD3DX12_SHADER_BYTECODE(T, sizeof(T))

class RenderContext {
public:
    explicit RenderContext(
        const Vertix::GraphicsDevice* graphicsDevice,
        Vertix::FrameCommandList* frameCommandList)
    : frameConstantsBuffer(graphicsDevice), lightConstantsBuffer(graphicsDevice), objectConstantsBuffer(graphicsDevice, 512),
      graphicsDevice(graphicsDevice)
    {
        Vertix::ResourceUploadHeap resourceUploadHeap {};
        frameCommandList->BeginCommand(nullptr);
        fullScreenVertex = std::unique_ptr<Vertix::VertexBuffer>(Vertix::VertexBuffer::CreateFullScreenRect(graphicsDevice, frameCommandList, resourceUploadHeap));
        frameCommandList->EndCommand();
        frameCommandList->WaitForCommand();

        perspectiveCamera.SetFieldOfView(Vertix::Engine::DegreesToRadians(60));
        perspectiveCamera.Move({-2.5, 0.5, 0.0});
    }

    Vertix::Engine::PerspectiveCamera perspectiveCamera;

    std::unique_ptr<Vertix::VertexBuffer> fullScreenVertex;
    std::atomic<std::shared_ptr<Vertix::TopLevelAccelerationStructure>> TLAS;

    Vertix::ConstantBuffer<FrameConstants> frameConstantsBuffer;
    Vertix::ConstantBuffer<LightConstants> lightConstantsBuffer;
    Vertix::ConstantBufferPageArray<ObjectConstants> objectConstantsBuffer;

    Vertix::ModelPool modelPool;
    std::vector<std::shared_ptr<Vertix::Engine::SceneObject3D>> sceneObjects;

    Vertix::Vector2D<UINT> windowSize;

    void BuildTLASAsync(const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& computeCommandQueue) {
        std::thread([
            d3d12Device  = graphicsDevice->GetD3D12Device(),
            computeQueue = computeCommandQueue,
            sceneObjects = sceneObjects,
            tlasOut      = &TLAS
        ]() -> void {
            Vertix::GraphicsCommandList graphicsCommandList { d3d12Device, computeQueue, D3D12_COMMAND_LIST_TYPE_COMPUTE };
            const auto& commandList = graphicsCommandList.GetD3D12GraphicsCommandList();
            graphicsCommandList.BeginCommand(nullptr);
            {
                UINT instanceId = 1;
                std::vector<D3D12_RAYTRACING_INSTANCE_DESC> instanceDescs;

                for (const auto &sceneObject : sceneObjects) {
                    const auto world = sceneObject->GetWorldMatrix();
                    for (const auto &mesh : sceneObject->SceneModel->Meshes) {
                        D3D12_RAYTRACING_INSTANCE_DESC instanceDesc = {};
                        instanceDesc.AccelerationStructure = mesh.BLAS->BLASResource->GetGPUVirtualAddress();
                        instanceDesc.InstanceID = instanceId++;
                        instanceDesc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
                        instanceDesc.InstanceContributionToHitGroupIndex = 0;
                        instanceDesc.InstanceMask = 0xFF;
                        instanceDesc.Transform[0][0] = world._11; instanceDesc.Transform[0][1] = world._21; instanceDesc.Transform[0][2] = world._31; instanceDesc.Transform[0][3] = world._41;
                        instanceDesc.Transform[1][0] = world._12; instanceDesc.Transform[1][1] = world._22; instanceDesc.Transform[1][2] = world._32; instanceDesc.Transform[1][3] = world._42;
                        instanceDesc.Transform[2][0] = world._13; instanceDesc.Transform[2][1] = world._23; instanceDesc.Transform[2][2] = world._33; instanceDesc.Transform[2][3] = world._43;
                        instanceDescs.emplace_back(instanceDesc);
                    }
                }

                tlasOut->store(std::shared_ptr<Vertix::TopLevelAccelerationStructure>(
                    Vertix::TopLevelAccelerationStructure::Create(d3d12Device, commandList, instanceDescs)
                ), std::memory_order_release);
            }
            graphicsCommandList.EndCommand();
            graphicsCommandList.WaitForCommand();
        }).detach();
    }

    void UpdateFrameConstants() {
        perspectiveCamera.GetViewMatrix(frameConstants.View);
        frameConstants.ViewProjection = frameConstants.View * frameConstants.Projection;
        frameConstants.ViewProjection.Invert(frameConstants.InvViewProjection);
        Vertix::Engine::FillVector4(frameConstants.CameraPosition, perspectiveCamera.GetPosition());
        frameConstantsBuffer.Fill(frameConstants);
    }

    void UpdateLightConstants() {
        LightConstants.LightDirection.Normalize(LightConstants.LightDirection);
        lightConstantsBuffer.Fill(LightConstants);
    }

    void UpdateObjectConstants() {
        for (UINT i = 0; i < sceneObjects.size(); i++) {
            const auto &sceneObject = sceneObjects[i];
            objectConstants.World = sceneObject->GetWorldMatrix();
            objectConstants.WorldInverseTranspose = sceneObject->GetWorldInverseTranspose();
            objectConstantsBuffer.FillAt(i, objectConstants);
        }
    }

    void SetWindowSize(const Vertix::Vector2D<UINT> &size) {
        windowSize = size;
        perspectiveCamera.SetAspect(static_cast<float>(windowSize.X) / static_cast<float>(windowSize.Y));
        perspectiveCamera.GetProjectionMatrix(frameConstants.Projection);
    }

    LightConstants LightConstants {
        .LightDirection = float3 { 0.3f, -0.925f, -0.225f },
        .AmbientIntensity = 0.1f,
        .LightColor = float3 { 1.0f, 1.0f, 1.0f },
        .LightIntensity = 4.5f,
    };
private:
    const Vertix::GraphicsDevice* graphicsDevice = nullptr;

    FrameConstants frameConstants;
    ObjectConstants objectConstants{};
};

#endif //VERTIX_RENDERCONTEXT_H
