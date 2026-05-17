//
// Created by Natsurainko on 2026/3/28.
//

#include "RayTracingShadowPass.h"

#include <RaytracingShadowPass_DXR.h>
#include <d3d12/d3dx12_root_signature.h>

#include "Vertix/Exceptions/HResultException.h"

#define align_to(_alignment, _val) (((_val + _alignment - 1) / _alignment) * _alignment)

void RayTracingShadowPass::Initialize(ID3D12Device10* device) {
    {
        CD3DX12_DESCRIPTOR_RANGE ranges[4];
        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
        ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
        ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
        ranges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

        CD3DX12_STATIC_SAMPLER_DESC pointSampler(0);
        pointSampler.Filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        pointSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        pointSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        pointSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

        CD3DX12_ROOT_PARAMETER rootParams[5];
        rootParams[0].InitAsShaderResourceView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootParams[1].InitAsDescriptorTable(1, &ranges[1]);
        rootParams[2].InitAsDescriptorTable(1, &ranges[2]);
        rootParams[3].InitAsDescriptorTable(1, &ranges[3]);
        rootParams[4].InitAsConstantBufferView(0);

        CD3DX12_ROOT_SIGNATURE_DESC desc;
        desc.Init(_countof(rootParams), rootParams);
        desc.NumStaticSamplers = 1;
        desc.pStaticSamplers = &pointSampler;
        desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;

        Microsoft::WRL::ComPtr<ID3DBlob> signature;
        Microsoft::WRL::ComPtr<ID3DBlob> error;

        ThrowIfFailed(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
        ThrowIfFailed(device->CreateRootSignature(0,
            signature->GetBufferPointer(),
            signature->GetBufferSize(),
            IID_PPV_ARGS(&globalRootSig)));
    }

    {
        CD3DX12_ROOT_PARAMETER param;
        param.InitAsConstants(4, 0, 1);

        CD3DX12_ROOT_SIGNATURE_DESC desc;
        desc.Init(1, &param);
        desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;

        Microsoft::WRL::ComPtr<ID3DBlob> signature;
        Microsoft::WRL::ComPtr<ID3DBlob> error;

        ThrowIfFailed(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
        ThrowIfFailed(device->CreateRootSignature(0,
            signature->GetBufferPointer(),
            signature->GetBufferSize(),
            IID_PPV_ARGS(&localRootSig)));
    }

    {
        std::vector<D3D12_STATE_SUBOBJECT> subobjects {
            { D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY, nullptr },
            { D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP, nullptr },
            { D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG, nullptr },
            { D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG, nullptr },
            { D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE, nullptr },
            { D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE, nullptr },
            { D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION, nullptr },
        };

        constexpr D3D12_EXPORT_DESC exports[] = {
            { L"RayGen", nullptr, D3D12_EXPORT_FLAG_NONE },
            { L"ShadowMiss", nullptr, D3D12_EXPORT_FLAG_NONE },
            { L"ShadowClosestHit", nullptr, D3D12_EXPORT_FLAG_NONE }
        };

        D3D12_DXIL_LIBRARY_DESC dxilLib = {};
        dxilLib.DXILLibrary = SHADER_BYTECODE(SHADER_BYTECODE_RAYTRACING_SHADOW_PASS_DXR);
        dxilLib.NumExports = 3;
        dxilLib.pExports = exports;
        subobjects[0].pDesc = &dxilLib;

        D3D12_HIT_GROUP_DESC hitGroup = {};
        hitGroup.HitGroupExport = L"ShadowHitGroup";
        hitGroup.ClosestHitShaderImport = L"ShadowClosestHit";
        hitGroup.Type = D3D12_HIT_GROUP_TYPE_TRIANGLES;
        subobjects[1].pDesc = &hitGroup;

        D3D12_RAYTRACING_SHADER_CONFIG shaderConfig = {};
        shaderConfig.MaxPayloadSizeInBytes = 16;
        shaderConfig.MaxAttributeSizeInBytes = 8;
        subobjects[2].pDesc = &shaderConfig;

        D3D12_RAYTRACING_PIPELINE_CONFIG pipelineConfig = {};
        pipelineConfig.MaxTraceRecursionDepth = 1;
        subobjects[3].pDesc = &pipelineConfig;

        D3D12_GLOBAL_ROOT_SIGNATURE globalSig = {};
        globalSig.pGlobalRootSignature = globalRootSig.Get();
        subobjects[4].pDesc = &globalSig;

        D3D12_LOCAL_ROOT_SIGNATURE localSig = {};
        localSig.pLocalRootSignature = localRootSig.Get();
        subobjects[5].pDesc = &localSig;

        const wchar_t* hitShaderExports[] = { L"ShadowHitGroup" };
        D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION localRootAssoc = {};
        localRootAssoc.NumExports = 1;
        localRootAssoc.pExports = hitShaderExports;
        localRootAssoc.pSubobjectToAssociate = &subobjects[5];
        subobjects[6].pDesc = &localRootAssoc;

        D3D12_STATE_OBJECT_DESC desc = {};
        desc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
        desc.NumSubobjects = 7;
        desc.pSubobjects = subobjects.data();

        ThrowIfFailed(device->CreateStateObject(&desc, IID_PPV_ARGS(&rtStateObject)));
    }

    {
        const auto uploadHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

        shaderTableEntrySize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
        shaderTableEntrySize += 8;
        shaderTableEntrySize = align_to(D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT, shaderTableEntrySize);

        uint32_t shaderTableSize = shaderTableEntrySize * 3;
        const auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(shaderTableSize);

        ThrowIfFailed(device->CreateCommittedResource(
            &uploadHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&stbResource)));

        Microsoft::WRL::ComPtr<ID3D12StateObjectProperties> stateObjectProperties;
        ThrowIfFailed(rtStateObject->QueryInterface(IID_PPV_ARGS(&stateObjectProperties)));

        uint8_t* ptr;
        ThrowIfFailed(stbResource->Map(0, nullptr, reinterpret_cast<void **>(&ptr)));
        {
            memcpy(ptr,
                stateObjectProperties->GetShaderIdentifier(L"RayGen"),
                D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
            memcpy(ptr + shaderTableEntrySize,
                stateObjectProperties->GetShaderIdentifier(L"ShadowMiss"),
                D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
            memcpy(ptr + shaderTableEntrySize * 2,
                stateObjectProperties->GetShaderIdentifier(L"ShadowHitGroup"),
                D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
        }
        stbResource->Unmap(0, nullptr);
    }

    descriptorHeap = shadowMaskUAV->heap;
}

void RayTracingShadowPass::Execute(ID3D12GraphicsCommandList5* commandList) {
    const auto topLevelAS = renderContext->TLAS.load(std::memory_order_acquire);
    if (!topLevelAS) return;

    D3D12_DISPATCH_RAYS_DESC raytraceDesc = {};
    raytraceDesc.Width = renderContext->windowSize.X;
    raytraceDesc.Height = renderContext->windowSize.Y;
    raytraceDesc.Depth = 1;

    raytraceDesc.RayGenerationShaderRecord.StartAddress = stbResource->GetGPUVirtualAddress();
    raytraceDesc.RayGenerationShaderRecord.SizeInBytes = shaderTableEntrySize;

    const size_t missOffset = 1 * shaderTableEntrySize;
    raytraceDesc.MissShaderTable.StartAddress = stbResource->GetGPUVirtualAddress() + missOffset;
    raytraceDesc.MissShaderTable.StrideInBytes = shaderTableEntrySize;
    raytraceDesc.MissShaderTable.SizeInBytes = shaderTableEntrySize;

    const size_t hitOffset = 2 * shaderTableEntrySize;
    raytraceDesc.HitGroupTable.StartAddress = stbResource->GetGPUVirtualAddress() + hitOffset;
    raytraceDesc.HitGroupTable.StrideInBytes = shaderTableEntrySize;
    raytraceDesc.HitGroupTable.SizeInBytes = shaderTableEntrySize;

    commandList->SetDescriptorHeaps(1, &descriptorHeap);
    commandList->SetComputeRootSignature(globalRootSig.Get());

    commandList->SetComputeRootShaderResourceView(0, topLevelAS->TLASResource->GetGPUVirtualAddress());
    commandList->SetComputeRootDescriptorTable(1, gPositionDepthSRV->gpuHandle);
    commandList->SetComputeRootDescriptorTable(2, gNormalRoughnessSRV->gpuHandle);
    commandList->SetComputeRootDescriptorTable(3, shadowMaskUAV->gpuHandle);
    commandList->SetComputeRootConstantBufferView(4, renderContext->lightConstantsBuffer.GetGpuVirtualAddress());

    commandList->SetPipelineState1(rtStateObject.Get());
    commandList->DispatchRays(&raytraceDesc);
}
