//
// Created by Natsurainko on 2026/2/18.
//

#pragma once

#include <d3d12/d3d12.h>
#include <wrl/client.h>

#include "Vertix/Graphics/Descriptor/DescriptorHandle.h"

namespace Vertix {
    using TextureHandle = DescriptorHandle;

    class Texture {
    public:
        explicit Texture(const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource) : d3d12Resource(d3d12Resource) {}
        virtual  ~Texture() = default;

        [[nodiscard]]
        const Microsoft::WRL::ComPtr<ID3D12Resource>& GetResource() const noexcept {
            return d3d12Resource;
        }

    protected:
        Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
    };

    class Texture2D : public Texture {
    public:
        explicit Texture2D(const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource) : Texture(d3d12Resource) {}
    };
}
