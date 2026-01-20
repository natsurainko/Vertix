//
// Created by Natsurainko on 2025/12/28.
//

#ifndef VERTIX_GAMEOBJECT3D_H
#define VERTIX_GAMEOBJECT3D_H

#include "directXTK/simpleMath/SimpleMath.h"

namespace Vertix::Engine {
    class GameObject3D {
    public:
        void Move(const DirectX::SimpleMath::Vector3 &offset, bool relative = true, bool allowRoll = false);
        void Rotate(const DirectX::SimpleMath::Vector3 &angles, bool allowRoll = false);

    protected:
        void UpdateWorldMatrix() {
            world = DirectX::SimpleMath::Matrix::CreateScale(scale) *
                DirectX::SimpleMath::Matrix::CreateFromQuaternion(orientation) *
                DirectX::SimpleMath::Matrix::CreateTranslation(position);
        }

        DirectX::SimpleMath::Vector3 position;
        DirectX::SimpleMath::Vector3 scale = DirectX::SimpleMath::Vector3::One;
        DirectX::SimpleMath::Quaternion orientation = DirectX::SimpleMath::Quaternion::Identity;

        DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::Identity;
    };
}

#endif //VERTIX_GAMEOBJECT3D_H