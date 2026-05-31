//
// Created by Natsurainko on 2025/12/28.
//

#pragma once

#include <d3d12/d3d12.h>
#include <DirectXTK12/SimpleMath.h>
#include <wrl/client.h>

namespace Vertix::Engine {
    class GameObject3D {
    protected:
        DirectX::SimpleMath::Vector3    position;
        DirectX::SimpleMath::Vector3    scale       = DirectX::SimpleMath::Vector3::One;
        DirectX::SimpleMath::Quaternion orientation = DirectX::SimpleMath::Quaternion::Identity;

        DirectX::SimpleMath::Matrix world                 = DirectX::SimpleMath::Matrix::Identity;
        DirectX::SimpleMath::Matrix worldInverseTranspose = DirectX::SimpleMath::Matrix::Identity;

    public:
        virtual ~GameObject3D() = default;

        virtual void Draw(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> &commandList) const {}

        virtual void DrawInstanced(
            const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> &commandList,
            UINT                                                      instanceCount) const {}

        VERTIX_ENGINE_API virtual void Move(
            const DirectX::SimpleMath::Vector3 &offset,
            bool                                relative  = true,
            bool                                allowRoll = false) noexcept;

        VERTIX_ENGINE_API virtual void Rotate(
            const DirectX::SimpleMath::Vector3 &angles,
            bool                                allowRoll = false) noexcept;

        virtual void SetPosition(const DirectX::SimpleMath::Vector3 &newPosition) noexcept {
            this->position = newPosition;
            UpdateWorldMatrix();
        }

        virtual void SetOrientation(const DirectX::SimpleMath::Quaternion &newOrientation) noexcept {
            this->orientation = newOrientation;
            UpdateWorldMatrix();
        }

        virtual void SetScale(const DirectX::SimpleMath::Vector3 &newScale) noexcept {
            this->scale = newScale;
            UpdateWorldMatrix();
        }

        [[nodiscard]] const DirectX::SimpleMath::Matrix&     GetWorldMatrix() const noexcept { return world; }
        [[nodiscard]] const DirectX::SimpleMath::Matrix&     GetWorldInverseTranspose() const noexcept { return worldInverseTranspose; }
        [[nodiscard]] const DirectX::SimpleMath::Vector3&    GetPosition() const noexcept { return position; }
        [[nodiscard]] const DirectX::SimpleMath::Quaternion& GetOrientation() const noexcept { return orientation; }

    protected:
        void UpdateWorldMatrix() noexcept {
            world = DirectX::SimpleMath::Matrix::CreateScale(scale) *
                    DirectX::SimpleMath::Matrix::CreateFromQuaternion(orientation) *
                    DirectX::SimpleMath::Matrix::CreateTranslation(position);

            world.Invert(worldInverseTranspose);
            worldInverseTranspose.Transpose(worldInverseTranspose);
        }
    };
}
