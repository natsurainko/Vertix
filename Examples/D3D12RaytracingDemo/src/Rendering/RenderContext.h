//
// Created by Natsurainko on 2026/3/28.
//

#ifndef VERTIX_RENDERCONTEXT_H
#define VERTIX_RENDERCONTEXT_H

#include <d3d12/d3dx12_core.h>
#include <structures.h>

#include "Camera/PerspectiveCamera.h"
#include "Graphics/DescriptorHeap.h"
#include "Graphics/Buffers/ConstantBuffer.hpp"
#include "Graphics/Buffers/ConstantBufferPageArray.hpp"
#include "Graphics/Raytracing/TopLevelAccelerationStructure.h"
#include "Helpers/MathHelper.h"
#include "Helpers/VectorHelper.h"
#include "Math/Vector2D.hpp"
#include "Pool/MaterialPool.hpp"
#include "Pool/ModelPool.hpp"
#include "Pool/TexturePool.hpp"
#include "Scene/SceneObject3D.hpp"

#define SHADER_BYTECODE(T) CD3DX12_SHADER_BYTECODE(T, sizeof(T))

class RenderContext {
public:
    explicit RenderContext(const Vertix::GraphicsDevice* graphicsDevice) :
        frameConstantsBuffer(graphicsDevice),
        lightConstantsBuffer(graphicsDevice),
        objectConstantsBuffer(graphicsDevice, 512),
        srvUavDescriptorHeap(graphicsDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 512, true),
        rtvDescriptorHeap(graphicsDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV),
        dsvDescriptorHeap(graphicsDevice, D3D12_DESCRIPTOR_HEAP_TYPE_DSV),
        texturePool(graphicsDevice),
        materialPool(graphicsDevice),
        graphicsDevice(graphicsDevice)
    {
        srvUavDescriptorHeap.AllocDescriptorHandle(tlasSrvHandle, tlasSrvGpuHandle);

        perspectiveCamera.SetFieldOfView(Vertix::Engine::DegreesToRadians(60));
        perspectiveCamera.Move({-2.5, 0.5, 0.0});
    }

    std::vector<std::unique_ptr<Vertix::Engine::SceneObject3D>> sceneObjects;

    Vertix::Engine::PerspectiveCamera perspectiveCamera;

    ID3D12Resource* currentFrameBuffer = nullptr;
    std::unique_ptr<Vertix::TopLevelAccelerationStructure> TLAS = nullptr;
    std::unique_ptr<Vertix::VertexBuffer> fullScreenVertex = nullptr;

    Vertix::ConstantBuffer<FrameConstants> frameConstantsBuffer;
    Vertix::ConstantBuffer<LightConstants> lightConstantsBuffer;
    Vertix::ConstantBufferPageArray<ObjectConstants> objectConstantsBuffer;

    D3D12_CPU_DESCRIPTOR_HANDLE tlasSrvHandle{};

    D3D12_GPU_DESCRIPTOR_HANDLE geometrySrvGpuHandles[4] = {};
    D3D12_GPU_DESCRIPTOR_HANDLE shadowSrvGpuHandle{};
    D3D12_GPU_DESCRIPTOR_HANDLE tlasSrvGpuHandle{};

    D3D12_RESOURCE_BARRIER geometryRtvBarriers[4] = {};
    D3D12_RESOURCE_BARRIER shadowUavBarrier{};

    Vertix::DescriptorHeap srvUavDescriptorHeap;
    Vertix::DescriptorHeap rtvDescriptorHeap;
    Vertix::DescriptorHeap dsvDescriptorHeap;

    Vertix::TexturePool<> texturePool;
    Vertix::DefaultMaterialPool<> materialPool;
    Vertix::ModelPool modelPool;

    Vertix::Vector2D<UINT> windowSize;
    CD3DX12_VIEWPORT viewport{0.f,0.f, 0.f, 0.f};
    CD3DX12_RECT scissorRect{};

    void BuildTLAS() {
        if (TLAS) {
            TLAS.reset();
        }

        Vertix::GraphicsCommandList graphicsCommandList { graphicsDevice->GetD3D12Device(), graphicsDevice->GetDefaultD3D12CommandQueue() };

        const auto d3d12Device = graphicsDevice->GetD3D12Device();
        const auto& commandList = graphicsCommandList.GetD3D12GraphicsCommandList();
        graphicsCommandList.BeginCommand(nullptr);
        {
            UINT instanceId = 1;
            std::vector<D3D12_RAYTRACING_INSTANCE_DESC> instanceDescs;

            for (const auto &sceneObject : sceneObjects) {
                sceneObject->SceneModel->UploadBLASToGPU(d3d12Device, commandList);
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

            TLAS = std::unique_ptr<Vertix::TopLevelAccelerationStructure>(
                Vertix::TopLevelAccelerationStructure::Create(d3d12Device, commandList, instanceDescs));

            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.RaytracingAccelerationStructure.Location = TLAS->TLASResource->GetGPUVirtualAddress();
            d3d12Device->CreateShaderResourceView(nullptr, &srvDesc, tlasSrvHandle);
        }
        graphicsCommandList.EndCommand();
        graphicsCommandList.WaitForCommand();
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
        viewport.Width = static_cast<float>(windowSize.X);
        viewport.Height = static_cast<float>(windowSize.Y);
        scissorRect.right = static_cast<LONG>(windowSize.X);
        scissorRect.bottom = static_cast<LONG>(windowSize.Y);

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
    const Vertix::GraphicsDevice *graphicsDevice = nullptr;

    FrameConstants frameConstants;
    ObjectConstants objectConstants{};
};

#endif //VERTIX_RENDERCONTEXT_H