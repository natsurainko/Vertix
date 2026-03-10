//
// Created by Natsurainko on 2025/12/28.
//

#ifndef VERTIX_GAMEOBJECT3D_H
#define VERTIX_GAMEOBJECT3D_H

#include <d3d12/d3d12.h>
#include <DirectXTK12/SimpleMath.h>
#include <wrl/client.h>

namespace Vertix {
    class Mesh;
}

namespace Vertix::Engine {
    class GameObject3D {
    public:
        virtual ~GameObject3D() = default;

        virtual void Draw(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> &commandList) const {}
        virtual void DrawInstanced(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> &commandList, UINT instanceCount) const {}

        virtual void Move(const DirectX::SimpleMath::Vector3 &offset,
                          bool relative = true,
                          bool allowRoll = false);

        virtual void Rotate(const DirectX::SimpleMath::Vector3 &angles, bool allowRoll = false);

        [[nodiscard]]
        const DirectX::SimpleMath::Matrix& GetWorldMatrix() const {
            return world;
        }

        [[nodiscard]]
        const DirectX::SimpleMath::Matrix& GetWorldInverseTranspose() const {
            return worldInverseTranspose;
        }

        [[nodiscard]]
        const DirectX::SimpleMath::Vector3& GetPosition() const {
            return position;
        }

        [[nodiscard]]
        const DirectX::SimpleMath::Quaternion& GetOrientation() const {
            return orientation;
        }

        void SetPosition(const DirectX::SimpleMath::Vector3 &newPosition) {
            this->position = newPosition;
            UpdateWorldMatrix();
        }

        void SetOrientation(const DirectX::SimpleMath::Quaternion &newOrientation) {
            this->orientation = newOrientation;
            UpdateWorldMatrix();
        }

        void SetScale(const DirectX::SimpleMath::Vector3 &newScale) {
            this->scale = newScale;
            UpdateWorldMatrix();
        }

    protected:
        void UpdateWorldMatrix() {
            world = DirectX::SimpleMath::Matrix::CreateScale(scale) *
                DirectX::SimpleMath::Matrix::CreateFromQuaternion(orientation) *
                DirectX::SimpleMath::Matrix::CreateTranslation(position);

            world.Invert(worldInverseTranspose);
            worldInverseTranspose.Transpose(worldInverseTranspose);
        }

        DirectX::SimpleMath::Vector3 position;
        DirectX::SimpleMath::Vector3 scale = DirectX::SimpleMath::Vector3::One;
        DirectX::SimpleMath::Quaternion orientation = DirectX::SimpleMath::Quaternion::Identity;

        DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::Identity;
        DirectX::SimpleMath::Matrix worldInverseTranspose = DirectX::SimpleMath::Matrix::Identity;
    };
}

#endif //VERTIX_GAMEOBJECT3D_H