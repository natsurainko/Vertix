//
// Created by Natsurainko on 2026/2/2.
//

#ifndef VERTIX_CONSTANTBUFFERPAGEARRAY_H
#define VERTIX_CONSTANTBUFFERPAGEARRAY_H

#include <vector>
#include <d3d12/d3d12.h>
#include <d3d12/d3dx12_core.h>
#include <wrl/client.h>

#include "Exceptions/HResultException.h"
#include "Graphics/GraphicsDevice.h"

namespace Vertix {
    template <typename T>
    class ConstantBufferPageArray {
        struct ConstantBufferPage {
            Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
            D3D12_GPU_VIRTUAL_ADDRESS gpuAddress{};
            BYTE* mappedPtr = nullptr;
        };

    public:
        explicit ConstantBufferPageArray(const GraphicsDevice* graphicsDevice, const UINT pageElementCount = 32)
            : pageElementCount(pageElementCount) {
            d3d12Device = graphicsDevice->GetD3D12Device();

            elementMemorySize = (sizeof(T) + 255) & ~255;
            pageMemorySize = elementMemorySize * pageElementCount;

            CreateBufferPage();
        }

        ~ConstantBufferPageArray() {
            for (auto& page : bufferPages) {
                if (page.mappedPtr) {
                    page.d3d12Resource->Unmap(0, nullptr);
                    page.mappedPtr = nullptr;
                }
            }
        }

        void FillAt(const UINT index, const T &value) {
            if (index >= totalElementCount) {
                throw std::out_of_range("Index out of range");
            }

            const int pageIndex = index / pageElementCount;
            const int elementIndex = index % pageElementCount;
            const ConstantBufferPage& bufferPage = bufferPages[pageIndex];

            memcpy(bufferPage.mappedPtr + elementIndex * elementMemorySize, &value, sizeof(T));
        }

        void AddPage() {
            CreateBufferPage();
        }

        [[nodiscard]]
        const Microsoft::WRL::ComPtr<ID3D12Resource>& GetD3D12ResourceAt(const UINT index) const {
            const UINT pageIndex = index / pageElementCount;
            const ConstantBufferPage& bufferPage = bufferPages[pageIndex];
            return bufferPage.d3d12Resource;
        }

        [[nodiscard]]
        D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddressAt(const UINT index) const {
            const UINT pageIndex = index / pageElementCount;
            const UINT elementIndex = index % pageElementCount;
            const ConstantBufferPage& bufferPage = bufferPages[pageIndex];
            return bufferPage.gpuAddress + elementIndex * elementMemorySize;
        }

        [[nodiscard]]
        UINT GetTotalElementCount() const {
            return totalElementCount;
        }

        ConstantBufferPageArray(const ConstantBufferPageArray&) = delete;
        ConstantBufferPageArray& operator=(const ConstantBufferPageArray&) = delete;
        ConstantBufferPageArray(ConstantBufferPageArray&& other) noexcept = default;
        ConstantBufferPageArray& operator=(ConstantBufferPageArray&& other) noexcept = default;
    private:
        Microsoft::WRL::ComPtr<ID3D12Device10> d3d12Device;

        UINT elementMemorySize;
        UINT pageMemorySize;

        UINT pageElementCount;
        UINT totalPageCount = 0;
        UINT totalElementCount = 0;

        std::vector<ConstantBufferPage> bufferPages;

        void CreateBufferPage() {
            ConstantBufferPage constantBufferPage;
            const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
            const CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(pageMemorySize);

            ThrowIfFailed(d3d12Device->CreateCommittedResource(
                &heapProps,
                D3D12_HEAP_FLAG_NONE,
                &bufferDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&constantBufferPage.d3d12Resource)
            ));

            const CD3DX12_RANGE readRange(0, 0);
            ThrowIfFailed(constantBufferPage.d3d12Resource->Map(0, &readRange,
                reinterpret_cast<void**>(&constantBufferPage.mappedPtr)));

            constantBufferPage.gpuAddress = constantBufferPage.d3d12Resource->GetGPUVirtualAddress();

            bufferPages.push_back(constantBufferPage);
            totalPageCount++;
            totalElementCount += pageElementCount;
        }
    };
}

#endif //VERTIX_CONSTANTBUFFERPAGEARRAY_H